// X-Monitor_Play.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include <atlstr.h>	// for CW2A
#include "miniSv.h"
#include "MMTimer.h"
#include "DataManage.h"
#include "Logger.h"
#include "FlightLog.h"
#include "kmlLogger.h"

#define N_LOOPCYCLE		(10)	// 周期処理の単位[msec]
#define DEFFAULT_MSGNO	(0)		// メッセージ番号のデフォルト値

//クライアントメッセージ
#define PATH_CLIENTTYPEMSG	  (L"./message/playClientTypeMessageToCore.xml")	
#define DEFAULT_CLIENTTYPEMSG ("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='ClientType'><ClientType><Function t='S'>Play</Function></ClientType></message></root>")
#define READ_BUFFER_SIZE      (2048)
//リプライメッセージ(ファイルOPEN)
#define PLAY_OPEN_REPLYMSG_1 ("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='FlightLogReply'><FlightLogReply><Play><Result t='S'>")
#define PLAY_OPEN_REPLYMSG_2 ("</Result><TotalTime>")
#define PLAY_OPEN_REPLYMSG_3 ("</TotalTime></Play></FlightLogReply></message></root>")
//リプライメッセージ
#define PLAY_REPLYMSG_1 ("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='FlightLogReply'><FlightLogReply><Play><Result t='S'>")
#define PLAY_REPLYMSG_2 ("</Result></Play></FlightLogReply></message></root>")


map<int, string> dataTypeMap;/* global変数*/

DWORD localClockRef;
DWORD localClock;
BOOL exitFlag = FALSE;	// プログラム終了判定

//
// コア部へ送信 （クライアントメッセージ）
//
std::string clientTypeMsgToCore()
{
	//ファイルOPEN
	FILE* fp;
	errno_t errorcode = _wfopen_s(&fp, PATH_CLIENTTYPEMSG, L"r");

	if (errorcode)
	{
		//ファイルがない時はデフォルトを使用
		return std::string(DEFAULT_CLIENTTYPEMSG);
	}
	else
	{

		//ファイルを読み込む

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
	long  lFileTm  = (long)(fileTm*1000.0); //ミリセック
	DWORD ulFileTm = lFileTm;//ミリセック

	// clockRef：clock基準　（＝ファイル先頭データの再生タイミング）
	//
	//　　 但し、一時停止、モード送り、モード戻し時のように
	//     再生が不連続になる時は、仮想的に連続再生しているものとし、
	//     clock基準を再設定する
	//   

	if (nowtime > ulFileTm)
	{
		localClockRef = nowtime - ulFileTm;
	}
	else
	{
		localClockRef = ULONG_MAX - (ulFileTm - nowtime);//DWORDのオーバーフロー対策
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
		localClock = ULONG_MAX - (localClockRef - nowtime);//DWORDのオーバーフロー対策
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


//　リプライメッセージ
std::string replyMsgToCore(std::string OK_NG)
{

	std::string msg = PLAY_REPLYMSG_1 + OK_NG + PLAY_REPLYMSG_2 ;

	return msg;
}

//　リプライメッセージ
std::string openReplyMsgToCore(std::string OK_NG, std::string totalTime)
{
	std::string msg = PLAY_OPEN_REPLYMSG_1 + OK_NG + PLAY_OPEN_REPLYMSG_2 + totalTime + PLAY_OPEN_REPLYMSG_3;

	BSTR msgBstr = _com_util::ConvertStringToBSTR(msg.c_str());
	StrXml *msgXml = new StrXml(msgBstr);
	_bstr_t configMsgTxtStr = msgXml->convertTxt2Bin(); //text->base64変換
	delete msgXml;

	char *configMsg = _com_util::ConvertBSTRToString(configMsgTxtStr);
	std::string returnStr(configMsg);
	delete[] configMsg;

	SysFreeString(msgBstr);
	return returnStr;
}

//
// システムのイベントを処理するハンドラ
//
BOOL WINAPI Handler(DWORD dwCtrlType)
{
	// FALSEを返すとそのままプログラム終了。TRUEを返すとプログラム続行。
	// ただし、CLOSE_EVENTだけは少し時間がたつと、自動で終了してしまう。

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
// 飛行ログ再生プログラム
//
int _tmain(int argc, _TCHAR* argv[])
{
	// 第1引数：コア部のIPアドレス
	// 第2引数：コア部のポート番号

	// システムのイベントを処理するハンドラを登録
	SetConsoleCtrlHandler(Handler, TRUE);

	// COMの初期化
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (!SUCCEEDED(hr))
	{
		MessageBox(NULL, L"COMの初期化に失敗しました[ログ再生機能]", L"エラー", MB_OK);
		exit(1);
	}

	// TCPクライアントオブジェクトの初期化
	::XMonitorSyncClient tcpClient;
	if(argc < 3 || !tcpClient.Initialize(CW2A(argv[1]), atoi(CW2A(argv[2])))){
		MessageBox(NULL, L"TCPクライアントの初期化に失敗しました[ログ再生機能]", L"エラー", MB_OK);
		exit(1);
	}	

	// TCPクライアントが受信したXML文を格納するバッファ
	std::vector<std::string> xmlStr;

	// 受信データ確認のタイムアウト[ms]
	DWORD timeout_msec = 0;	// 受信データが無い場合、すぐに制御が戻るようにする

	// 周期処理用タイマーの初期化
	CMMTimer mmTimer;
	mmTimer.OpenTimer(N_LOOPCYCLE, NULL);

	// データタイプMap初期化(XMLデータ管理クラス)
	Base64Data::datatypeMap();

	//　KMLログ保存・再生部クラス
	kmlLogger *logFile = new kmlLogger(L"r");

	//　クライアントタイプ送信フラグ
	static int clintTypeMsg = 0;	//=0;未送信、=1;送信済み

	// 周期処理ループ
	while (!exitFlag) {

		mmTimer.WaitTimeoutEvent();
		DWORD nowtime = mmTimer.timeGetTime();	// 現在時刻(データ再生に使用)

		// コア部と接続していないとき
		if (tcpClient.IsConnected() == FALSE) {
			::puts("接続なし");
			::Sleep(1000);
			continue;
		}

		// コア部と接続しているとき
		else{

			//クライアントタイプをコア部へ送信
			if (!clintTypeMsg)
			{
				if (tcpClient.SendData(clientTypeMsgToCore()))
				{
					clintTypeMsg = 1;
				}
				else
				{
					MessageBox(NULL, L"クライアントタイプの送信に失敗しました[ログ再生機能]", L"エラー", MB_OK);
					exit(1);
				}
			}

			// TCPクライアントが受信したXML文を取得
			xmlStr.clear();
			if (tcpClient.GetData(xmlStr, timeout_msec) == FALSE) {
#ifdef _DEBUG
				//wprintf(L"受信データなし\n");
#endif
			}
			else{
				// TODO: 受信データがある場合の処理を記述

				for (size_t i = 0; i < xmlStr.size(); i++)
				{
					setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
					BSTR xmlBstr = _com_util::ConvertStringToBSTR(xmlStr.at(i).c_str());

					if (isFlightLogMsg(xmlBstr))//属性aはFlightLog?
					{
						//base64->text変換
						setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
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
							if (log.play == PLAY_MODE::OPEN)//再生コマンド
							{
								tcpClient.SendData(openReplyMsgToCore("OK", logFile->getTotalTm())); //リプライメッセージ送信
							}
							else if (log.play == PLAY_MODE::CLOSE)
							{
								tcpClient.SendData(replyMsgToCore("OK")); //リプライメッセージ送信
							}
							else if (log.play == PLAY_MODE::EDIT)
							{
								tcpClient.SendData(replyMsgToCore("OK")); //リプライメッセージ送信
							}
							if (log.ope >= OPE_MODE::PLAY)
							{
								//操作コマンドがPlay,Next,Prev,SLIDEBARならば基準タイム再設定
								double ctm = logFile->getCtime();//再生ファイルのカレントタイム(sec)
								resetClock(nowtime, ctm);	//時計のリセット
							}
						}
						else
						{
#ifdef _DEBUG
							wprintf(L"\nERROR\n");
							wprintf(log.guiMsg);
							wprintf(L"\n\n");
#endif
							if (log.play == PLAY_MODE::OPEN)//再生コマンド
							{
								tcpClient.SendData(openReplyMsgToCore("NG", "")); //リプライメッセージ送信
							}
							else if (log.play == PLAY_MODE::EDIT)
							{
								tcpClient.SendData(replyMsgToCore("NG")); //リプライメッセージ送信
							}
						}

					}

					::SysFreeString(xmlBstr);
				}
			}

			// TODO: 受信データがあってもなくても行う処理（＝再生処理）を記述
			// 
			if (logFile->isPlaying())////操作モード=Play？
			{
				setClock(nowtime);	//時計セット
				if (stopTimer(logFile->getCtime()))	//送信タイミング？
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
	//mapコンテナクリア
	dataTypeMap.clear();
	// COMの終了処理
	CoUninitialize();

	return 0;
}
