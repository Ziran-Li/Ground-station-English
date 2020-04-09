// X-Monitor_Play.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//
#include "stdafx.h"
#include <atlstr.h>	// for CW2A
#include "miniSv.h"
#include "MMTimer.h"
#include "DataManage.h"
#include "Logger.h"
#include "FlightLog.h"
#include "kmlLogger.h"

#define N_LOOPCYCLE		(10)	// ���������̒P��[msec]
#define DEFFAULT_MSGNO	(0)		// ���b�Z�[�W�ԍ��̃f�t�H���g�l

//�N���C�A���g���b�Z�[�W
#define PATH_CLIENTTYPEMSG	  (L"./message/playClientTypeMessageToCore.xml")	
#define DEFAULT_CLIENTTYPEMSG ("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='ClientType'><ClientType><Function t='S'>Play</Function></ClientType></message></root>")
#define READ_BUFFER_SIZE      (2048)
//���v���C���b�Z�[�W(�t�@�C��OPEN)
#define PLAY_OPEN_REPLYMSG_1 ("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='FlightLogReply'><FlightLogReply><Play><Result t='S'>")
#define PLAY_OPEN_REPLYMSG_2 ("</Result><TotalTime>")
#define PLAY_OPEN_REPLYMSG_3 ("</TotalTime></Play></FlightLogReply></message></root>")
//���v���C���b�Z�[�W
#define PLAY_REPLYMSG_1 ("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='FlightLogReply'><FlightLogReply><Play><Result t='S'>")
#define PLAY_REPLYMSG_2 ("</Result></Play></FlightLogReply></message></root>")


map<int, string> dataTypeMap;/* global�ϐ�*/

DWORD localClockRef;
DWORD localClock;
BOOL exitFlag = FALSE;	// �v���O�����I������

//
// �R�A���֑��M �i�N���C�A���g���b�Z�[�W�j
//
std::string clientTypeMsgToCore()
{
	//�t�@�C��OPEN
	FILE* fp;
	errno_t errorcode = _wfopen_s(&fp, PATH_CLIENTTYPEMSG, L"r");

	if (errorcode)
	{
		//�t�@�C�����Ȃ����̓f�t�H���g���g�p
		return std::string(DEFAULT_CLIENTTYPEMSG);
	}
	else
	{

		//�t�@�C����ǂݍ���

		char buffer[READ_BUFFER_SIZE];
		std::string  oneMsg;

		while (!feof(fp))
		{
			memset(buffer, 0, READ_BUFFER_SIZE);
			fgets(buffer, READ_BUFFER_SIZE, fp);
			size_t len = strlen(buffer);
			std::string  tmp = std::string(buffer);
			oneMsg += tmp;
		}
		fclose(fp);

		return oneMsg;
	}
}

//
//
//
void resetClock(DWORD nowtime, double fileTm)
{
	long  lFileTm  = (long)(fileTm*1000.0); //�~���Z�b�N
	DWORD ulFileTm = lFileTm;//�~���Z�b�N

	// clockRef�Fclock��@�i���t�@�C���擪�f�[�^�̍Đ��^�C�~���O�j
	//
	//�@�@ �A���A�ꎞ��~�A���[�h����A���[�h�߂����̂悤��
	//     �Đ����s�A���ɂȂ鎞�́A���z�I�ɘA���Đ����Ă�����̂Ƃ��A
	//     clock����Đݒ肷��
	//   

	if (nowtime > ulFileTm)
	{
		localClockRef = nowtime - ulFileTm;
	}
	else
	{
		localClockRef = ULONG_MAX - (ulFileTm - nowtime);//DWORD�̃I�[�o�[�t���[�΍�
	}
}

void setClock(DWORD nowtime)
{
	if (nowtime >= localClockRef)
	{
		localClock = nowtime - localClockRef;
	}
	else
	{
		localClock = ULONG_MAX - (localClockRef - nowtime);//DWORD�̃I�[�o�[�t���[�΍�
	}
}

VARIANT_BOOL stopTimer(double fileTm)
{
	double dClock = (double)localClock;
	double dFileTm = fileTm * 1000.0;

	if (dClock >= dFileTm)
	{
		return VARIANT_TRUE;
	}
	else
	{
		return VARIANT_FALSE;
	}
}

VARIANT_BOOL isFlightLogMsg(BSTR xmlMsg)
{
	HRESULT hr;
	class XmlMessage *xml = new XmlMessage(xmlMsg, &hr);

	if (xml->getMsgType(DEFFAULT_MSGNO) == XmlMessage::msgSet)
	{
		if (xml->getMsgAttA(DEFFAULT_MSGNO) == _bstr_t(L"FlightLog"))
		{
			delete xml;
			return VARIANT_TRUE;
		}
	}
	delete xml;
	return VARIANT_FALSE;

}


//�@���v���C���b�Z�[�W
std::string replyMsgToCore(std::string OK_NG)
{

	std::string msg = PLAY_REPLYMSG_1 + OK_NG + PLAY_REPLYMSG_2 ;

	return msg;
}

//�@���v���C���b�Z�[�W
std::string openReplyMsgToCore(std::string OK_NG, std::string totalTime)
{
	std::string msg = PLAY_OPEN_REPLYMSG_1 + OK_NG + PLAY_OPEN_REPLYMSG_2 + totalTime + PLAY_OPEN_REPLYMSG_3;

	BSTR msgBstr = _com_util::ConvertStringToBSTR(msg.c_str());
	StrXml *msgXml = new StrXml(msgBstr);
	_bstr_t configMsgTxtStr = msgXml->convertTxt2Bin(); //text->base64�ϊ�
	delete msgXml;

	char *configMsg = _com_util::ConvertBSTRToString(configMsgTxtStr);
	std::string returnStr(configMsg);
	delete[] configMsg;

	SysFreeString(msgBstr);
	return returnStr;
}

//
// �V�X�e���̃C�x���g����������n���h��
//
BOOL WINAPI Handler(DWORD dwCtrlType)
{
	// FALSE��Ԃ��Ƃ��̂܂܃v���O�����I���BTRUE��Ԃ��ƃv���O�������s�B
	// �������ACLOSE_EVENT�����͏������Ԃ����ƁA�����ŏI�����Ă��܂��B

	if (dwCtrlType == CTRL_CLOSE_EVENT) {
		exitFlag = TRUE;
		Sleep(1000);
		//MessageBox(NULL, TEXT("CTRL_CLOSE_EVENT"), TEXT("OK"), MB_OK);
		return TRUE;
	}
	else {
		//MessageBox(NULL, TEXT("OTHER_EVENT"), TEXT("OK"), MB_OK);
		return FALSE;
	}
}

//
// ��s���O�Đ��v���O����
//
int _tmain(int argc, _TCHAR* argv[])
{
	// ��1�����F�R�A����IP�A�h���X
	// ��2�����F�R�A���̃|�[�g�ԍ�

	// �V�X�e���̃C�x���g����������n���h����o�^
	SetConsoleCtrlHandler(Handler, TRUE);

	// COM�̏�����
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (!SUCCEEDED(hr))
	{
		MessageBox(NULL, L"COM�̏������Ɏ��s���܂���[���O�Đ��@�\]", L"�G���[", MB_OK);
		exit(1);
	}

	// TCP�N���C�A���g�I�u�W�F�N�g�̏�����
	::XMonitorSyncClient tcpClient;
	if(argc < 3 || !tcpClient.Initialize(CW2A(argv[1]), atoi(CW2A(argv[2])))){
		MessageBox(NULL, L"TCP�N���C�A���g�̏������Ɏ��s���܂���[���O�Đ��@�\]", L"�G���[", MB_OK);
		exit(1);
	}	

	// TCP�N���C�A���g����M����XML�����i�[����o�b�t�@
	std::vector<std::string> xmlStr;

	// ��M�f�[�^�m�F�̃^�C���A�E�g[ms]
	DWORD timeout_msec = 0;	// ��M�f�[�^�������ꍇ�A�����ɐ��䂪�߂�悤�ɂ���

	// ���������p�^�C�}�[�̏�����
	CMMTimer mmTimer;
	mmTimer.OpenTimer(N_LOOPCYCLE, NULL);

	// �f�[�^�^�C�vMap������(XML�f�[�^�Ǘ��N���X)
	Base64Data::datatypeMap();

	//�@KML���O�ۑ��E�Đ����N���X
	kmlLogger *logFile = new kmlLogger(L"r");

	//�@�N���C�A���g�^�C�v���M�t���O
	static int clintTypeMsg = 0;	//=0;�����M�A=1;���M�ς�

	// �����������[�v
	while (!exitFlag) {

		mmTimer.WaitTimeoutEvent();
		DWORD nowtime = mmTimer.timeGetTime();	// ���ݎ���(�f�[�^�Đ��Ɏg�p)

		// �R�A���Ɛڑ����Ă��Ȃ��Ƃ�
		if (tcpClient.IsConnected() == FALSE) {
			::puts("�ڑ��Ȃ�");
			::Sleep(1000);
			continue;
		}

		// �R�A���Ɛڑ����Ă���Ƃ�
		else{

			//�N���C�A���g�^�C�v���R�A���֑��M
			if (!clintTypeMsg)
			{
				if (tcpClient.SendData(clientTypeMsgToCore()))
				{
					clintTypeMsg = 1;
				}
				else
				{
					MessageBox(NULL, L"�N���C�A���g�^�C�v�̑��M�Ɏ��s���܂���[���O�Đ��@�\]", L"�G���[", MB_OK);
					exit(1);
				}
			}

			// TCP�N���C�A���g����M����XML�����擾
			xmlStr.clear();
			if (tcpClient.GetData(xmlStr, timeout_msec) == FALSE) {
#ifdef _DEBUG
				//wprintf(L"��M�f�[�^�Ȃ�\n");
#endif
			}
			else{
				// TODO: ��M�f�[�^������ꍇ�̏������L�q

				for (size_t i = 0; i < xmlStr.size(); i++)
				{
					setlocale(LC_ALL, "Japanese_Japan.932");	//<-�ǉ�
					BSTR xmlBstr = _com_util::ConvertStringToBSTR(xmlStr.at(i).c_str());

					if (isFlightLogMsg(xmlBstr))//����a��FlightLog?
					{
						//base64->text�ϊ�
						setlocale(LC_ALL, "Japanese_Japan.932");	//<-�ǉ�
						StrXml *xmlBin = new StrXml(xmlBstr);
						_bstr_t xmlTxtStr = xmlBin->convertBin2Txt();
						delete xmlBin;

						struct flightLog log;
#ifdef _DEBUG
						wprintf(L"xmlTxtStr\n");
						wprintf(xmlTxtStr);
#endif
						if (logFile->setPlayOpeMode(xmlTxtStr, &log))
						{
							if (log.play == PLAY_MODE::OPEN)//�Đ��R�}���h
							{
								tcpClient.SendData(openReplyMsgToCore("OK", logFile->getTotalTm())); //���v���C���b�Z�[�W���M
							}
							else if (log.play == PLAY_MODE::CLOSE)
							{
								tcpClient.SendData(replyMsgToCore("OK")); //���v���C���b�Z�[�W���M
							}
							else if (log.play == PLAY_MODE::EDIT)
							{
								tcpClient.SendData(replyMsgToCore("OK")); //���v���C���b�Z�[�W���M
							}
							if (log.ope >= OPE_MODE::PLAY)
							{
								//����R�}���h��Play,Next,Prev,SLIDEBAR�Ȃ�Ί�^�C���Đݒ�
								double ctm = logFile->getCtime();//�Đ��t�@�C���̃J�����g�^�C��(sec)
								resetClock(nowtime, ctm);	//���v�̃��Z�b�g
							}
						}
						else
						{
#ifdef _DEBUG
							wprintf(L"\nERROR\n");
							wprintf(log.guiMsg);
							wprintf(L"\n\n");
#endif
							if (log.play == PLAY_MODE::OPEN)//�Đ��R�}���h
							{
								tcpClient.SendData(openReplyMsgToCore("NG", "")); //���v���C���b�Z�[�W���M
							}
							else if (log.play == PLAY_MODE::EDIT)
							{
								tcpClient.SendData(replyMsgToCore("NG")); //���v���C���b�Z�[�W���M
							}
						}

					}

					::SysFreeString(xmlBstr);
				}
			}

			// TODO: ��M�f�[�^�������Ă��Ȃ��Ă��s�������i���Đ������j���L�q
			// 
			if (logFile->isPlaying())////���샂�[�h=Play�H
			{
				setClock(nowtime);	//���v�Z�b�g
				if (stopTimer(logFile->getCtime()))	//���M�^�C�~���O�H
				{
					_bstr_t xml = logFile->getData();
					if (xml != _bstr_t(L""))
					{
						char *send = _com_util::ConvertBSTRToString(xml.GetBSTR());
						std::string cstr(send);
						tcpClient.SendData(cstr);
						delete[] send;
#ifdef _DEBUG
						wprintf(xml);
						wprintf(L"\n----\n");
#endif
					}
				}
			}
		}

		mmTimer.ResetTimeoutEvent();
	}
	//map�R���e�i�N���A
	dataTypeMap.clear();
	// COM�̏I������
	CoUninitialize();

	return 0;
}
