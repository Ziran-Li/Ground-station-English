// X-Monitor_Rec.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//
#include <tchar.h>
#include "stdafx.h"
#include <atlstr.h>	// for CW2A
#include "miniSv.h"
#include "DataManage.h"
#include "Logger.h"
#include "FlightLog.h"
#include "kmlLogger.h"

#define N_LOOPCYCLE		      (10)   // ���������̒P��[msec]
#define READ_BUFFER_SIZE      (2048) // �o�b�t�@�T�C�Y
#define STATE_FILEOPEN_DONE   (1)    // �t�@�C��OPEN�ς�

//�t���C�g���O���b�Z�[�W�̑���a
#define ATTA_FLIGHTLOG        (L"FlightLog")
//�t���C�g���O���b�Z�[�W�̑���a
#define ATTA_CONFIG           (L"Config")
//�N���C�A���g���b�Z�[�W
#define PATH_CLIENTTYPEMSG	  (L"./message/recClientTypeMessageToCore.xml")	
#define DEFAULT_CLIENTTYPEMSG ("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='ClientType'><ClientType><Function t='S'>REC</Function></ClientType></message></root>")
//get���b�Z�[�W�i�ۑ����ԁj
#define PATH_GETMSGSTORAGE	  (L"./message/GetMessageStoragePeriodToCore.xml")	
#define DEFAULT_GETMSGSTORAGE ("<?xml version='1.0' encoding='UTF-8'?><root><message type='get' name='xxxx' a='Config'><Config><FlightLog><StoragePeriodInDay t='UI'></StoragePeriodInDay>")
#define DEFAULT_GETPATH       ("<FilePath t='S'></FilePath></FlightLog></Config></message></root>")
//close���b�Z�[�W�i�ۑ����ԁj
#define CLOSE_MSG_STORAGE	  ("<?xml version='1.0' encoding='UTF-8'?><root><message type='close' name='xxxx'></message></root>")
//get���b�Z�[�W�i�ۑ����ځj
#define PATH_GETMSGTOCORE     (L"./message/getMessageToCore.xml")	
#define DEFAULT_GETMSG	      ("<?xml version='1.0' encoding='UTF-8'?><root><message type='get' name='RecPlay' cycle = '20'><Data encoding='text'><UAV/></Data></message></root>")
//close���b�Z�[�W�i�ۑ����ځj
#define CLOSE_MSG	          ("<?xml version='1.0' encoding='UTF-8'?><root><message type='close' name='RecPlay'></message></root>")
//���v���C���b�Z�[�W
#define REC_REPLYMSG_1        ("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='FlightLogReply'><FlightLogReply><Rec><Result t='S'>")
#define REC_REPLYMSG_2        ("</Result></Rec></FlightLogReply></message></root>")

map<int, string> dataTypeMap;/* global�ϐ�*/

Logger *logger=NULL;
BOOL exitFlag = FALSE;	// �v���O�����I������

//
// ���O�o�͏�����
//
void logInit()
{
	if (logger == NULL)
	{
		// ��P�����F�f�B���N�g���p�X,��Q�����F�t�@�C����, ��R�����F�t�@�C�������Ԋu�i�b�j
		logger = new Logger(L"./", L"log_X-Monitor_Rec", LLONG_MAX);

		if (logger->RecStart()) //������ 
		{
			//�������G���[
			delete logger;
			logger = NULL;
		}
	}

}
//
// ���O�o��
//
void logOut(BSTR bstr)
{
	if (logger != NULL) // �������ς�
	{
		logger->WriteFile_YYYYMMDDHHMMSS(bstr);
	}
}
//
// ���O�o�͏I������
//
void logEnd()
{
	if (logger == NULL)  return;

	fpos_t size = logger->getFileSize();

	if (size <= 0) //�t�@�C���T�C�Y��0?
	{
		logger->Close();
		logger->del();
	}
	else
	{
		logger->Close();
	}
	delete logger;
	logger = NULL;

}
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
// �R�A���֑��M �iget���b�Z�[�W�i�ۑ����Ԃƍ폜�Ώۃf�B���N�g���j�j
//
std::string getMsgStoragePeriodToCore()
{
	//�t�@�C��OPEN
	FILE* fp;
	errno_t errorcode = _wfopen_s(&fp, PATH_GETMSGSTORAGE, L"r");

	if (errorcode)
	{
		//�t�@�C�����Ȃ����̓f�t�H���g���g�p
		std::string msg = std::string(DEFAULT_GETMSGSTORAGE) + std::string(DEFAULT_GETPATH);
		return msg;
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
// �R�A���֑��M �iget���b�Z�[�W�i�ۑ����ځj�j
//
std::string getMsgToCore()
{
	//�t�@�C��OPEN
	FILE* fp;
	errno_t errorcode = _wfopen_s(&fp, PATH_GETMSGTOCORE, L"r");

	if (errorcode)
	{
		//�t�@�C�����Ȃ����̓f�t�H���g���g�p
		return std::string(DEFAULT_GETMSG);
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
//�@���v���C���b�Z�[�W
//
std::string replyMsgToCore(std::string OK_NG)
{
	std::string msg = REC_REPLYMSG_1 + OK_NG + REC_REPLYMSG_2;
	return msg;
}

//
//�@�t�@�C���폜
//
void deleteSaveFiles(wchar_t path[], double saveInterval)
{
	vector <std::wstring>deleteList;
	std::string delList;

	wchar_t subpath[_MAX_PATH];
	wchar_t temp[_MAX_PATH];
	WIN32_FIND_DATA lp;

	//���ݎ���
	time_t now = time(NULL);

	//�t�H���_���̃t�@�C������
	setlocale(LC_ALL, "Japanese_Japan.932");	//<-�ǉ�
	wcscpy_s(temp, path);
	HANDLE h = FindFirstFile(temp, &lp);
	temp[wcslen(temp) - 1] = '\0';
	do
	{
		if ((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& wcscmp(lp.cFileName, L"..") != 0 && wcscmp(lp.cFileName, L".") != 0)
		{
			//���ʃt�H���_���̃t�@�C��
			wsprintf(subpath, L"%s%s\\*", temp, lp.cFileName);
			deleteSaveFiles(subpath, saveInterval);
		}
		if ((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
		{
			//�t�@�C������
			FILETIME fileTime;
			SYSTEMTIME creationtime;

			//�X�V����
			//�i�쐬�����́A�t�@�C�����ړ����������ł������ς���Ă��܂��̂ŁA�X�V�����Ŕ��f����j
			FileTimeToLocalFileTime(&lp.ftLastWriteTime, &fileTime);
			FileTimeToSystemTime(&fileTime, &creationtime);
			struct tm  file_tm;
			file_tm.tm_hour = creationtime.wHour;
			file_tm.tm_min = creationtime.wMinute;
			file_tm.tm_mday = creationtime.wDay;
			file_tm.tm_mon = creationtime.wMonth - 1;
			file_tm.tm_sec = creationtime.wSecond;
			file_tm.tm_year = creationtime.wYear - 1900;

			//�o�ߕb
			double passedDay = difftime(now, mktime(&file_tm)) / 86400.0;

			//�t�@�C���p�X
			wsprintf(subpath, L"%s%s", temp, lp.cFileName);
			if (wcsstr(subpath, L".kml") != NULL || wcsstr(subpath, L".kmz"))
			{
				if (passedDay >= saveInterval)
				{
					deleteList.push_back(subpath);
				}
			}

		}
	} while (FindNextFile(h, &lp));


	if (deleteList.size() > 0)
	{
		std::wstring delList;
		delList = L"�t�@�C�����폜���Ă���낵���ł��傤���H\n";
		for (size_t i = 0; i < deleteList.size(); i++)
		{
			delList += deleteList.at(i);
			delList += L"\n";
		}
		BSTR msg = SysAllocString(delList.c_str());
		int on_button = MessageBox(NULL, msg, L"�m�F", MB_YESNO | MB_ICONQUESTION);
		if (on_button == IDYES)
		{
			//�t�@�C���̍폜
			for (size_t i = 0; i < deleteList.size(); i++)
			{
				_tremove(deleteList.at(i).c_str());
			}

			//�f�B���N�g���̍폜 
			//_trmdir(temp);
		}
		SysFreeString(msg);
	}
	FindClose(h);
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
// ��s���O�ۑ��v���O����
//
int _tmain(int argc, _TCHAR* argv[])
{
	// ��1�����F�R�A����IP�A�h���X
	// ��2�����F�R�A���̃|�[�g�ԍ�

	// ���O�o�͏�����
//	logInit();
	
	// �V�X�e���̃C�x���g����������n���h����o�^
	SetConsoleCtrlHandler(Handler, TRUE);

	// COM�̏�����
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (!SUCCEEDED(hr))
	{
		MessageBox(NULL, L"COM�̏������Ɏ��s���܂���[���O�ۑ��@�\]", L"�G���[", MB_OK);
		exit(1);
	}

	// TCP�N���C�A���g�I�u�W�F�N�g�̏�����
	::XMonitorSyncClient tcpClient;
	if(argc < 3 || !tcpClient.Initialize(CW2A(argv[1]), atoi(CW2A(argv[2])))){
		MessageBox(NULL, L"TCP�N���C�A���g�̏������Ɏ��s���܂���[���O�ۑ��@�\]", L"�G���[", MB_OK);
		exit(1);
	}	

	// TCP�N���C�A���g����M����XML�����i�[����o�b�t�@
	std::vector<std::string> xmlStr;

	// ��M�f�[�^�m�F�̃^�C���A�E�g[ms]
	DWORD timeout_msec = 1000;	// ���ݒ�

	// �f�[�^�^�C�vMap������(XML�f�[�^�Ǘ��N���X)
	Base64Data::datatypeMap();

	//�@KML���O�ۑ��E�Đ����N���X
//	kmlLogger logFile(L"w");
	kmlLogger *logFile = new kmlLogger(L"w");

	//�@�t�@�C���폜�����t���O
	static VARIANT_BOOL deleteFilesFlag = VARIANT_FALSE; //=FALSE;���@=TRUE;�����ς�
	//�@�X�e�[�g�t���O
	static int state = 0; //=0;=1;�t�@�C��OPEN�ς�
	//�@�N���C�A���g�^�C�v���M�t���O
	static int clintTypeMsg = 0;	//=0;�����M�A=1;���M�ς�
	//�@get���b�Z�[�W�i�ۑ����ԁj���M�t���O
	static int getMsgStoragePeriod = 0;	//=0;�����M�A=1;���M�ς�
    //�@�t�@�C���p�X
	//_bstr_t fileName;
	bool endFlag = true;

	while (!exitFlag) {

		// �R�A���Ɛڑ����Ă��Ȃ��Ƃ�
		if (tcpClient.IsConnected() == FALSE) {
			::puts("�ڑ��Ȃ�");
			::Sleep(1000);
			endFlag = true;
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
					MessageBox(NULL, L"�N���C�A���g�^�C�v�̑��M�Ɏ��s���܂���[���O�ۑ��@�\]", L"�G���[", MB_OK);
					exit(1);
				}
			}
            //get���b�Z�[�W�i�t�@�C���ۑ����Ԃƍ폜�Ώۃf�B���N�g���j���R�A���֑��M
			if (!getMsgStoragePeriod)
			{
				if (tcpClient.SendData(getMsgStoragePeriodToCore()))
				{
					getMsgStoragePeriod = 1;
				}
				else
				{
					MessageBox(NULL, L"�t�@�C���ۑ����Ԃ�get���b�Z�[�W�̑��M�Ɏ��s���܂���[���O�ۑ��@�\]", L"�G���[", MB_OK);
					exit(1);
				}
			}

			// TCP�N���C�A���g����M����XML�����擾
			xmlStr.clear();
			if (tcpClient.GetData(xmlStr, timeout_msec) == FALSE) {
				//::puts("�^�C���A�E�g���܂���");
				endFlag = true;
				continue;
			}
			
			endFlag = false;
			// ��M�f�[�^������ꍇ�̏���
			for (size_t i = 0; i < xmlStr.size(); i++)
			{				
				BSTR tmp = _com_util::ConvertStringToBSTR(xmlStr.at(i).c_str());
				//_bstr_t tmp(xmlStr.at(i).c_str());

				HRESULT hr;
				class   XmlMessage *xml = new XmlMessage(tmp, &hr);

				int     msgNo = 0;
				if (xml->getMsgType(msgNo) == XmlMessage::msgSet) //���b�Z�[�W�^�C�v��"set"?
				{					
					_bstr_t attA = xml->getMsgAttA(msgNo); // ����a���擾

					if (attA == _bstr_t(ATTA_FLIGHTLOG))
					{
						StrXml *configMsg = new StrXml(tmp);
						_bstr_t configMsgTxtStr = configMsg->convertBin2Txt(); //base64->text�ϊ�
						delete configMsg;
#ifdef _DEBUG
						wprintf(configMsgTxtStr);
#endif
						struct flightLog log = FlightLog::getFlightLogInfo(configMsgTxtStr.GetBSTR());
						if (log.rec == REC_MODE::SAVE) //�ۑ�
						{
							if (state < STATE_FILEOPEN_DONE)
							{
								StrXml guiMsg(log.guiMsg);
								//�t�@�C�����擾
								//fileName = guiMsg.getData(L"root/message/FlightLog/Rec/FilePath");
								//logFile->setFilename(fileName);
								setlocale(LC_ALL, "Japanese_Japan.932");	//<-�ǉ�
								BSTR bstrFilePath = guiMsg.getData(L"root/message/FlightLog/Rec/FilePath");
								logFile->setFilename(bstrFilePath);
								//�t�@�C���T�C�Y
								logFile->setDivedeSize(guiMsg.getData(L"root/message/FlightLog/Rec/DivideSizeInByte"));

								//�t�@�C��OPEN
								if (logFile->Initialize())
								{
									//�R�A����OPEN�R�}���h�ɑ΂��郊�v���C���b�Z�[�W�𑗐M
									tcpClient.SendData(replyMsgToCore("NG"));
									endFlag = true;
								}
								else
								{
									//�R�A����OPEN�R�}���h�ɑ΂��郊�v���C���b�Z�[�W�𑗐M
									tcpClient.SendData(replyMsgToCore("OK"));
									//�R�A����get���b�Z�[�W�𑗐M
									tcpClient.SendData(getMsgToCore());
									//�X�e�[�g�t���O�̍X�V
									state = STATE_FILEOPEN_DONE;//OPEN����
								}
							}
						}
						else if (log.rec == REC_MODE::END) //�I��
						{
							if (state) //�t�@�C��OPEN�ς�
							{
								//�t�@�C��CLOSE
								if (logFile->Close())
								{
									//�R�A���֏I���R�}���h�ɑ΂��郊�v���C���b�Z�[�W�𑗐M
									tcpClient.SendData(replyMsgToCore("OK"));
								}
								else
								{
									//�R�A���֏I���R�}���h�ɑ΂��郊�v���C���b�Z�[�W�𑗐M
									tcpClient.SendData(replyMsgToCore("NG"));
								}
								//�R�A����close���b�Z�[�W�𑗐M
								tcpClient.SendData(CLOSE_MSG);
								//�X�e�[�g�t���O�̍X�V
								state = 0;
							}
						}
					}
					else if (attA == _bstr_t(ATTA_CONFIG)) //message�̑���a="Config"�H
					{
						if (!deleteFilesFlag)
						{
							//�t�@�C���폜�������{�ς݂ɂ���
							deleteFilesFlag = VARIANT_TRUE;

							//�R�A����close���b�Z�[�W�𑗐M
							tcpClient.SendData(CLOSE_MSG_STORAGE);

							//�ۑ����Ԏ擾
							StrXml *configMsg = new StrXml(tmp);
							_bstr_t period = configMsg->getData(L"root/message/Config/FlightLog/StoragePeriodInDay");
							//�t�@�C���폜�f�B���N�g��
							_bstr_t delete_dir = configMsg->getData(L"root/message/Config/FlightLog/FilePath");
							_bstr_t deletePath = delete_dir + "*";
							delete configMsg;
							//�t�@�C���폜
							deleteSaveFiles(deletePath, atof(period));
						}
					}
					else
					{
						if (state)//�t�@�C��OPEN�ς�
						{
							//��������
							logFile->WriteOneData(tmp);
						}
					}
				}
				SysFreeString(tmp);
				delete xml;
			}
		}
	}

	// ��s���O�̕ۑ�
	if (endFlag==false)
	{
		if (logFile->Close())
		{
			//�R�A���֏I���R�}���h�ɑ΂��郊�v���C���b�Z�[�W�𑗐M
			tcpClient.SendData(replyMsgToCore("OK"));
		}
		
	}
	//KML���O�ۑ��E�Đ����N���A
	delete logFile;

	//map�R���e�i�N���A
	dataTypeMap.clear();
	// COM�̏I������
	CoUninitialize();
	// ���O�o�͏I������
//	logEnd();

	return 0;
}


