// X-Monitor_Rec.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//
#include <tchar.h>
#include "stdafx.h"
#include <atlstr.h>	// for CW2A
#include "miniSv.h"
#include "DataManage.h"
#include "Logger.h"
#include "FlightLog.h"
#include "kmlLogger.h"

#define N_LOOPCYCLE		      (10)   // 周期処理の単位[msec]
#define READ_BUFFER_SIZE      (2048) // バッファサイズ
#define STATE_FILEOPEN_DONE   (1)    // ファイルOPEN済み

//フライトログメッセージの属性a
#define ATTA_FLIGHTLOG        (L"FlightLog")
//フライトログメッセージの属性a
#define ATTA_CONFIG           (L"Config")
//クライアントメッセージ
#define PATH_CLIENTTYPEMSG	  (L"./message/recClientTypeMessageToCore.xml")	
#define DEFAULT_CLIENTTYPEMSG ("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='ClientType'><ClientType><Function t='S'>REC</Function></ClientType></message></root>")
//getメッセージ（保存期間）
#define PATH_GETMSGSTORAGE	  (L"./message/GetMessageStoragePeriodToCore.xml")	
#define DEFAULT_GETMSGSTORAGE ("<?xml version='1.0' encoding='UTF-8'?><root><message type='get' name='xxxx' a='Config'><Config><FlightLog><StoragePeriodInDay t='UI'></StoragePeriodInDay>")
#define DEFAULT_GETPATH       ("<FilePath t='S'></FilePath></FlightLog></Config></message></root>")
//closeメッセージ（保存期間）
#define CLOSE_MSG_STORAGE	  ("<?xml version='1.0' encoding='UTF-8'?><root><message type='close' name='xxxx'></message></root>")
//getメッセージ（保存項目）
#define PATH_GETMSGTOCORE     (L"./message/getMessageToCore.xml")	
#define DEFAULT_GETMSG	      ("<?xml version='1.0' encoding='UTF-8'?><root><message type='get' name='RecPlay' cycle = '20'><Data encoding='text'><UAV/></Data></message></root>")
//closeメッセージ（保存項目）
#define CLOSE_MSG	          ("<?xml version='1.0' encoding='UTF-8'?><root><message type='close' name='RecPlay'></message></root>")
//リプライメッセージ
#define REC_REPLYMSG_1        ("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='FlightLogReply'><FlightLogReply><Rec><Result t='S'>")
#define REC_REPLYMSG_2        ("</Result></Rec></FlightLogReply></message></root>")

map<int, string> dataTypeMap;/* global変数*/

Logger *logger=NULL;
BOOL exitFlag = FALSE;	// プログラム終了判定

//
// ログ出力初期化
//
void logInit()
{
	if (logger == NULL)
	{
		// 第１引数：ディレクトリパス,第２引数：ファイル名, 第３引数：ファイル分割間隔（秒）
		logger = new Logger(L"./", L"log_X-Monitor_Rec", LLONG_MAX);

		if (logger->RecStart()) //初期化 
		{
			//初期化エラー
			delete logger;
			logger = NULL;
		}
	}

}
//
// ログ出力
//
void logOut(BSTR bstr)
{
	if (logger != NULL) // 初期化済み
	{
		logger->WriteFile_YYYYMMDDHHMMSS(bstr);
	}
}
//
// ログ出力終了処理
//
void logEnd()
{
	if (logger == NULL)  return;

	fpos_t size = logger->getFileSize();

	if (size <= 0) //ファイルサイズが0?
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
// コア部へ送信 （getメッセージ（保存期間と削除対象ディレクトリ））
//
std::string getMsgStoragePeriodToCore()
{
	//ファイルOPEN
	FILE* fp;
	errno_t errorcode = _wfopen_s(&fp, PATH_GETMSGSTORAGE, L"r");

	if (errorcode)
	{
		//ファイルがない時はデフォルトを使用
		std::string msg = std::string(DEFAULT_GETMSGSTORAGE) + std::string(DEFAULT_GETPATH);
		return msg;
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
// コア部へ送信 （getメッセージ（保存項目））
//
std::string getMsgToCore()
{
	//ファイルOPEN
	FILE* fp;
	errno_t errorcode = _wfopen_s(&fp, PATH_GETMSGTOCORE, L"r");

	if (errorcode)
	{
		//ファイルがない時はデフォルトを使用
		return std::string(DEFAULT_GETMSG);
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
//　リプライメッセージ
//
std::string replyMsgToCore(std::string OK_NG)
{
	std::string msg = REC_REPLYMSG_1 + OK_NG + REC_REPLYMSG_2;
	return msg;
}

//
//　ファイル削除
//
void deleteSaveFiles(wchar_t path[], double saveInterval)
{
	vector <std::wstring>deleteList;
	std::string delList;

	wchar_t subpath[_MAX_PATH];
	wchar_t temp[_MAX_PATH];
	WIN32_FIND_DATA lp;

	//現在時刻
	time_t now = time(NULL);

	//フォルダ内のファイル検索
	setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
	wcscpy_s(temp, path);
	HANDLE h = FindFirstFile(temp, &lp);
	temp[wcslen(temp) - 1] = '\0';
	do
	{
		if ((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& wcscmp(lp.cFileName, L"..") != 0 && wcscmp(lp.cFileName, L".") != 0)
		{
			//下位フォルダ内のファイル
			wsprintf(subpath, L"%s%s\\*", temp, lp.cFileName);
			deleteSaveFiles(subpath, saveInterval);
		}
		if ((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
		{
			//ファイル時間
			FILETIME fileTime;
			SYSTEMTIME creationtime;

			//更新日時
			//（作成日時は、ファイルを移動しただけでも書き変わってしまうので、更新日時で判断する）
			FileTimeToLocalFileTime(&lp.ftLastWriteTime, &fileTime);
			FileTimeToSystemTime(&fileTime, &creationtime);
			struct tm  file_tm;
			file_tm.tm_hour = creationtime.wHour;
			file_tm.tm_min = creationtime.wMinute;
			file_tm.tm_mday = creationtime.wDay;
			file_tm.tm_mon = creationtime.wMonth - 1;
			file_tm.tm_sec = creationtime.wSecond;
			file_tm.tm_year = creationtime.wYear - 1900;

			//経過秒
			double passedDay = difftime(now, mktime(&file_tm)) / 86400.0;

			//ファイルパス
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
		delList = L"ファイルを削除してもよろしいでしょうか？\n";
		for (size_t i = 0; i < deleteList.size(); i++)
		{
			delList += deleteList.at(i);
			delList += L"\n";
		}
		BSTR msg = SysAllocString(delList.c_str());
		int on_button = MessageBox(NULL, msg, L"確認", MB_YESNO | MB_ICONQUESTION);
		if (on_button == IDYES)
		{
			//ファイルの削除
			for (size_t i = 0; i < deleteList.size(); i++)
			{
				_tremove(deleteList.at(i).c_str());
			}

			//ディレクトリの削除 
			//_trmdir(temp);
		}
		SysFreeString(msg);
	}
	FindClose(h);
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
// 飛行ログ保存プログラム
//
int _tmain(int argc, _TCHAR* argv[])
{
	// 第1引数：コア部のIPアドレス
	// 第2引数：コア部のポート番号

	// ログ出力初期化
//	logInit();
	
	// システムのイベントを処理するハンドラを登録
	SetConsoleCtrlHandler(Handler, TRUE);

	// COMの初期化
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (!SUCCEEDED(hr))
	{
		MessageBox(NULL, L"COMの初期化に失敗しました[ログ保存機能]", L"エラー", MB_OK);
		exit(1);
	}

	// TCPクライアントオブジェクトの初期化
	::XMonitorSyncClient tcpClient;
	if(argc < 3 || !tcpClient.Initialize(CW2A(argv[1]), atoi(CW2A(argv[2])))){
		MessageBox(NULL, L"TCPクライアントの初期化に失敗しました[ログ保存機能]", L"エラー", MB_OK);
		exit(1);
	}	

	// TCPクライアントが受信したXML文を格納するバッファ
	std::vector<std::string> xmlStr;

	// 受信データ確認のタイムアウト[ms]
	DWORD timeout_msec = 1000;	// 仮設定

	// データタイプMap初期化(XMLデータ管理クラス)
	Base64Data::datatypeMap();

	//　KMLログ保存・再生部クラス
//	kmlLogger logFile(L"w");
	kmlLogger *logFile = new kmlLogger(L"w");

	//　ファイル削除処理フラグ
	static VARIANT_BOOL deleteFilesFlag = VARIANT_FALSE; //=FALSE;未　=TRUE;処理済み
	//　ステートフラグ
	static int state = 0; //=0;=1;ファイルOPEN済み
	//　クライアントタイプ送信フラグ
	static int clintTypeMsg = 0;	//=0;未送信、=1;送信済み
	//　getメッセージ（保存期間）送信フラグ
	static int getMsgStoragePeriod = 0;	//=0;未送信、=1;送信済み
    //　ファイルパス
	//_bstr_t fileName;
	bool endFlag = true;

	while (!exitFlag) {

		// コア部と接続していないとき
		if (tcpClient.IsConnected() == FALSE) {
			::puts("接続なし");
			::Sleep(1000);
			endFlag = true;
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
					MessageBox(NULL, L"クライアントタイプの送信に失敗しました[ログ保存機能]", L"エラー", MB_OK);
					exit(1);
				}
			}
            //getメッセージ（ファイル保存期間と削除対象ディレクトリ）をコア部へ送信
			if (!getMsgStoragePeriod)
			{
				if (tcpClient.SendData(getMsgStoragePeriodToCore()))
				{
					getMsgStoragePeriod = 1;
				}
				else
				{
					MessageBox(NULL, L"ファイル保存期間のgetメッセージの送信に失敗しました[ログ保存機能]", L"エラー", MB_OK);
					exit(1);
				}
			}

			// TCPクライアントが受信したXML文を取得
			xmlStr.clear();
			if (tcpClient.GetData(xmlStr, timeout_msec) == FALSE) {
				//::puts("タイムアウトしました");
				endFlag = true;
				continue;
			}
			
			endFlag = false;
			// 受信データがある場合の処理
			for (size_t i = 0; i < xmlStr.size(); i++)
			{				
				BSTR tmp = _com_util::ConvertStringToBSTR(xmlStr.at(i).c_str());
				//_bstr_t tmp(xmlStr.at(i).c_str());

				HRESULT hr;
				class   XmlMessage *xml = new XmlMessage(tmp, &hr);

				int     msgNo = 0;
				if (xml->getMsgType(msgNo) == XmlMessage::msgSet) //メッセージタイプは"set"?
				{					
					_bstr_t attA = xml->getMsgAttA(msgNo); // 属性aを取得

					if (attA == _bstr_t(ATTA_FLIGHTLOG))
					{
						StrXml *configMsg = new StrXml(tmp);
						_bstr_t configMsgTxtStr = configMsg->convertBin2Txt(); //base64->text変換
						delete configMsg;
#ifdef _DEBUG
						wprintf(configMsgTxtStr);
#endif
						struct flightLog log = FlightLog::getFlightLogInfo(configMsgTxtStr.GetBSTR());
						if (log.rec == REC_MODE::SAVE) //保存
						{
							if (state < STATE_FILEOPEN_DONE)
							{
								StrXml guiMsg(log.guiMsg);
								//ファイル名取得
								//fileName = guiMsg.getData(L"root/message/FlightLog/Rec/FilePath");
								//logFile->setFilename(fileName);
								setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
								BSTR bstrFilePath = guiMsg.getData(L"root/message/FlightLog/Rec/FilePath");
								logFile->setFilename(bstrFilePath);
								//ファイルサイズ
								logFile->setDivedeSize(guiMsg.getData(L"root/message/FlightLog/Rec/DivideSizeInByte"));

								//ファイルOPEN
								if (logFile->Initialize())
								{
									//コア部へOPENコマンドに対するリプライメッセージを送信
									tcpClient.SendData(replyMsgToCore("NG"));
									endFlag = true;
								}
								else
								{
									//コア部へOPENコマンドに対するリプライメッセージを送信
									tcpClient.SendData(replyMsgToCore("OK"));
									//コア部へgetメッセージを送信
									tcpClient.SendData(getMsgToCore());
									//ステートフラグの更新
									state = STATE_FILEOPEN_DONE;//OPEN成功
								}
							}
						}
						else if (log.rec == REC_MODE::END) //終了
						{
							if (state) //ファイルOPEN済み
							{
								//ファイルCLOSE
								if (logFile->Close())
								{
									//コア部へ終了コマンドに対するリプライメッセージを送信
									tcpClient.SendData(replyMsgToCore("OK"));
								}
								else
								{
									//コア部へ終了コマンドに対するリプライメッセージを送信
									tcpClient.SendData(replyMsgToCore("NG"));
								}
								//コア部へcloseメッセージを送信
								tcpClient.SendData(CLOSE_MSG);
								//ステートフラグの更新
								state = 0;
							}
						}
					}
					else if (attA == _bstr_t(ATTA_CONFIG)) //messageの属性a="Config"？
					{
						if (!deleteFilesFlag)
						{
							//ファイル削除処理実施済みにする
							deleteFilesFlag = VARIANT_TRUE;

							//コア部へcloseメッセージを送信
							tcpClient.SendData(CLOSE_MSG_STORAGE);

							//保存期間取得
							StrXml *configMsg = new StrXml(tmp);
							_bstr_t period = configMsg->getData(L"root/message/Config/FlightLog/StoragePeriodInDay");
							//ファイル削除ディレクトリ
							_bstr_t delete_dir = configMsg->getData(L"root/message/Config/FlightLog/FilePath");
							_bstr_t deletePath = delete_dir + "*";
							delete configMsg;
							//ファイル削除
							deleteSaveFiles(deletePath, atof(period));
						}
					}
					else
					{
						if (state)//ファイルOPEN済み
						{
							//書き込み
							logFile->WriteOneData(tmp);
						}
					}
				}
				SysFreeString(tmp);
				delete xml;
			}
		}
	}

	// 飛行ログの保存
	if (endFlag==false)
	{
		if (logFile->Close())
		{
			//コア部へ終了コマンドに対するリプライメッセージを送信
			tcpClient.SendData(replyMsgToCore("OK"));
		}
		
	}
	//KMLログ保存・再生部クリア
	delete logFile;

	//mapコンテナクリア
	dataTypeMap.clear();
	// COMの終了処理
	CoUninitialize();
	// ログ出力終了処理
//	logEnd();

	return 0;
}


