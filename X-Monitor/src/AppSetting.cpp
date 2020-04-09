#include "stdafx.h"
#include "AppSetting.h"
#include <msxml.h>
#include <stdio.h>
#include <stdarg.h>
#include "MsXmlPlus.h"
#include "common/CommonMacro.h"

//MSXML3(?) 使用宣言
using namespace MSXML2;

#define FNAME       L"Setting.xml"
#define XMLFILENAME _bstr_t(FNAME)

#define ROOTNODE L"root"

BOOL SaveFormatData(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pTarget, LPCTSTR NodeName, LPCTSTR pszFormat, ...);
BOOL LoadData(MSXML2::IXMLDOMNodePtr& pSourceNode, LPCTSTR NodeName, int& Destination);
BOOL LoadData(MSXML2::IXMLDOMNodePtr& pSourceNode, LPCTSTR NodeName, unsigned short& Destination);
BOOL LoadData(MSXML2::IXMLDOMNodePtr& pSourceNode, LPCTSTR NodeName, double& Destination);
BOOL LoadData(MSXML2::IXMLDOMNodePtr& pSourceNode, LPCTSTR NodeName, std::string& Destination);

//#################################################################################################################################
//	AppSetting
//#################################################################################################################################

// コンストラクタ
AppSetting::AppSetting()
{
	m_hFile = NULL;
}

// デストラクタ
AppSetting::~AppSetting()
{
	_UnlockFile();
}

// 設定の読み出し
HRESULT AppSetting::Load()
{
	if (__argc > 1)
	{
		_OpenFile();
		_CommandLine();
	}
	else
	{
		// XML設定ファイル読み込み
		_OpenFile();
	}

	
	return FALSE;
}

// 設定の保存
HRESULT AppSetting::Save()
{
	if (m_hFile == NULL) return E_FAIL;
	if (__argc >= 11)  return E_FAIL;		// コマンドラインの場合保存しない
	
	HRESULT hr;
	VARIANT_BOOL f;

	// ファイルをアンロック
	_UnlockFile();

	// MSXML3 インスタンス生成
	MSXML2::IXMLDOMDocument2Ptr pDoc;
	hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if (FAILED(hr)) return hr; // インスタンスの生成に失敗

	// XML ファイルのロード
	TCHAR strPathIni[MAX_PATH + 1];
	setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
	SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, strPathIni);
	wsprintf(strPathIni, TEXT("%s\\ACSL\\X-Monitor\\Setting\\Core\\Setting.xml"), strPathIni);

	pDoc->put_async(VARIANT_FALSE); // loadの完了まで待つ
	f = pDoc->load(_variant_t(_bstr_t(strPathIni)));
	if (f == VARIANT_FALSE) return E_FAIL;

	// ロードに成功
	MSXML2::IXMLDOMNodePtr pRootNode = pDoc->selectSingleNode(_bstr_t("/root")); // root ノードを選択
	this->App._Save(pDoc, pRootNode);
	//this->DataFileRecorder._Save(pDoc, pRootNode);
	//this->Watch._Save(pDoc, pRootNode);
	this->Comm._Save(pDoc, pRootNode);
	//this->MainLink._Save(pDoc, pRootNode);
	this->XMonitorServer._Save(pDoc, pRootNode);
	//this->USBPropo._Save(pDoc, pRootNode);
	this->FlightLog._Save(pDoc, pRootNode);

	// ファイルに保存
	//pDoc->save(_variant_t(_bstr_t(strPathIni)));
	pDoc->save(_variant_t(_bstr_t(strPathIni)));

	// ファイルをロック
	_LockFile();

	return S_OK;
}


// XML ファイルのオープン
HRESULT AppSetting::_OpenFile()
{
	// ファイルオープンを判定するコード
	if (m_hFile) return TRUE;

	HRESULT hr;
	VARIANT_BOOL f;

	// MSXML3 インスタンス生成
	MSXML2::IXMLDOMDocument2Ptr pDoc;
	hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if (FAILED(hr)) return hr; // インスタンスの生成に失敗

	// XML ファイルのロード

	TCHAR strPathIni[MAX_PATH + 1];
	setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
	SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, strPathIni);
	wsprintf(strPathIni, TEXT("%s\\ACSL\\X-Monitor\\Setting\\Core\\Setting.xml"), strPathIni);

	pDoc->put_async(VARIANT_FALSE); // loadの完了まで待つ
	f = pDoc->load(_variant_t(_bstr_t(strPathIni)));
	if (f == VARIANT_FALSE) { // ロードに失敗

		// 空ファイル生成
		HRESULT hr = _CreateFile(pDoc);
		if (FAILED(hr)) return hr; // ファイルの生成に失敗
	}

	if (f == VARIANT_TRUE) {
		// ロードに成功

		// ここに設定を読み込むコードを記述
		MSXML2::IXMLDOMNodePtr pRootNode = pDoc->selectSingleNode(_bstr_t("/root"));
		this->App._Load(pDoc, pRootNode);
		//this->DataFileRecorder._Load(pDoc, pRootNode);
		//this->Watch._Load(pDoc, pRootNode);
		this->Comm._Load(pDoc, pRootNode);
		//this->MainLink._Load(pDoc, pRootNode);
		this->XMonitorServer._Load(pDoc, pRootNode);
		//this->USBPropo._Load(pDoc, pRootNode);
		this->FlightLog._Load(pDoc, pRootNode);
	}

	// 成功
	_LockFile(); // ファイルをロック

	return TRUE;
}

HRESULT AppSetting::_CommandLine()
{

	// コマンド引数
	if (__argc >= 11)
	{
		// ex)UAV-001 6 0 1 1 53798 C:\Users\ogawa\Desktop\test\ 5 100000000 180
		
		// [0] プログラム名
		// [1] <App><MonitorIdName>
		// [2] <Comm><MainPort>
		// [3] <Comm><SubLinkEnable>
		// [4] <Comm><SubPort>
		// [5] <XMonitorServer><Enabled>
		// [6] <XMonitorServer><PortNo>
		// [7] <FlightLog><FilePath>
		// [8] <FlightLog><RecStopTimeFromDataStop>
		// [9] <FlightLog><DivideSizeInByte>
		// [10]<FlightLog><StoragePeriodInDay>

		//this->App.MonitorIdName = __argv[1];
		//this->Comm.MainPort = atoi(__argv[2]);
		//this->Comm.SubLinkEnable = atoi(__argv[3]);
		//this->Comm.SubPort = atoi(__argv[4]);
		//this->XMonitorServer.Enabled = atoi(__argv[5]);
		//this->XMonitorServer.PortNo = atoi(__argv[6]);
		//this->FlightLog.FilePath = __argv[7];
		//this->FlightLog.RecStopTimeFromDataStop = atoi(__argv[8]);
		//this->FlightLog.DivideSizeInByte = atoi(__argv[9]);
		//this->FlightLog.StoragePeriodInDay = atoi(__argv[10]);

		this->App.MonitorIdName = __wargv[1];
		this->Comm.MainPort = _wtoi(__wargv[2]);
		this->Comm.SubLinkEnable = _wtoi(__wargv[3]);
		this->Comm.SubPort = _wtoi(__wargv[4]);
		this->XMonitorServer.Enabled = _wtoi(__wargv[5]);
		this->XMonitorServer.PortNo = _wtoi(__wargv[6]);
		this->FlightLog.FilePath = CW2A(__wargv[7]);
		this->FlightLog.RecStopTimeFromDataStop = _wtoi(__wargv[8]);
		this->FlightLog.DivideSizeInByte = _wtoi(__wargv[9]);
		this->FlightLog.StoragePeriodInDay = _wtoi(__wargv[10]);
	}

	return TRUE;
}

// 空ファイルの生成
HRESULT AppSetting::_CreateFile(IXMLDOMDocument2Ptr& pDoc)
{
	// ファイルオープンを判定するコード
	if (m_hFile) return E_FAIL;

	pDoc->appendChild(
		pDoc->createProcessingInstruction(L"xml", L"version=\'1.0\' encoding=\'Shift_JIS\'")
		);

	MSXML2::IXMLDOMNodePtr pRootNode = pDoc->createElement(ROOTNODE);
	pDoc->appendChild(pRootNode);

	TCHAR strPathIni[MAX_PATH + 1];
	setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
	SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, strPathIni);
	wsprintf(strPathIni, TEXT("%s\\ACSL\\X-Monitor\\Setting\\Core\\Setting.xml"), strPathIni);
	pDoc->save((_variant_t(_bstr_t(strPathIni))));

	return TRUE;
}

// 設定ファイルをロック
HRESULT AppSetting::_LockFile()
{
	if (m_hFile) return S_OK;

	// ファイルオープン
	HANDLE hFile = CreateFile(FNAME, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return E_FAIL; // 失敗

	// 成功
	m_hFile = hFile;
	return S_OK;
}

// 設定ファイルをアンロック
HRESULT AppSetting::_UnlockFile()
{
	if (m_hFile == NULL) return E_FAIL; // ファイルが開かれていない

	// ファイルを閉じる
	BOOL b = CloseHandle(m_hFile);
	if (!b) return E_FAIL; // 失敗

	m_hFile = NULL;
	return S_OK;
}

//#################################################################################################################################
//	_ClassApp
//#################################################################################################################################

//*******************************************************************************
//	ClassApp デフォルト値の設定
//*******************************************************************************
AppSetting::_ClassApp::_ClassApp()
{
	MonitorIdName = _T("Default");
}

//*******************************************************************************
//	MonitorIdName の値チェック
//*******************************************************************************
BOOL AppSetting::_ClassApp::TestValue_MonitorIdName(CString &MonitorIdName)
{
	if (MonitorIdName.Find(_T("<")) != -1) return FALSE;
	if (MonitorIdName.Find(_T(">")) != -1) return FALSE;
	if (MonitorIdName.Find(_T("&")) != -1) return FALSE;
	if (MonitorIdName.Find(_T("\r")) != -1) return FALSE;
	if (MonitorIdName.Find(_T("\n")) != -1) return FALSE;
	if (MonitorIdName.Find(_T("\t")) != -1) return FALSE;
	if (MonitorIdName.Find(_T("\\")) != -1) return FALSE; // なぜか gdi 関連で死ぬ

	return TRUE;
}

//*******************************************************************************
//	アプリケーション関連データのロード
//*******************************************************************************
HRESULT AppSetting::_ClassApp::_Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
{
	// App ノードの取得
	MSXML2::IXMLDOMNodePtr pAppNode = pRootNode->selectSingleNode(_bstr_t("./App"));
	if (pAppNode == NULL) return S_FALSE; // App ノードが存在しないのでデフォルト値を使用

	// MonitorIdName のロード
	MSXML2::IXMLDOMNodePtr pMonitorIdNameNode = pAppNode->selectSingleNode(_bstr_t("./MonitorIdName"));
	if (pMonitorIdNameNode) MonitorIdName = (char*)pMonitorIdNameNode->text;

	return S_OK;
}

//*******************************************************************************
//	アプリケーション関連データのセーブ
//*******************************************************************************
HRESULT AppSetting::_ClassApp::_Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
{
	// App ノードの取得
	MSXML2::IXMLDOMNodePtr pAppNode = pRootNode->selectSingleNode(_bstr_t("./App"));

	if (pAppNode) {
		// App ノードが存在する
		pRootNode->removeChild(pAppNode); // 削除
	}

	// App ノードを新規作成
	pAppNode = pDoc->createElement(_bstr_t("App"));

	// MonitorIdName の保存
	MSXML2::IXMLDOMNodePtr pMonitorIdNameNode = pDoc->createElement(_bstr_t("MonitorIdName"));
	pMonitorIdNameNode->text = _bstr_t(MonitorIdName);

	pAppNode->appendChild(pMonitorIdNameNode);
	pRootNode->appendChild(pAppNode);

	return S_OK;
}


////#################################################################################################################################
////	_ClassDataFileRecorder
////#################################################################################################################################
//
////*******************************************************************************
////	ClassDataFileRecorder デフォルト値の設定
////*******************************************************************************
//AppSetting::_ClassDataFileRecorder::_ClassDataFileRecorder()
//{
//}
//
////*******************************************************************************
////	ファイル記録 関連データのロード
////*******************************************************************************
//HRESULT AppSetting::_ClassDataFileRecorder::_Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
//{
//	// Watch ノードの取得
//	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./DataFileRecorder"));
//	if( pThisNode == NULL ) return S_FALSE; // ノードが存在しないのでデフォルト値を使用
//
//	MSXML2::IXMLDOMElementPtr pLastSelectedFormatID = pThisNode->selectSingleNode(_bstr_t("./LastSelectedFormatID"));
//	if( pLastSelectedFormatID ) {
//		LastSelectedFormatID = (char*)pLastSelectedFormatID->text;
//	}
//
//	return S_OK;
//}
//
////*******************************************************************************
////	ファイル記録 関連データのセーブ
////*******************************************************************************
//HRESULT AppSetting::_ClassDataFileRecorder::_Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
//{
//	// Watch ノードの取得
//	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./DataFileRecorder"));
//	
//	if( pThisNode ) {
//		// ノードが存在する
//		pRootNode->removeChild(pThisNode); // 削除
//	}
//	
//	// MainLink ノードを新規作成
//	pThisNode = pDoc->createElement(_bstr_t("DataFileRecorder")); 
//
//	MSXML2::IXMLDOMElementPtr pLastSelectedFormatID = pDoc->createElement(_bstr_t("LastSelectedFormatID"));
//	pLastSelectedFormatID->text = _bstr_t(this->LastSelectedFormatID.operator ATL::CComBSTR());
//
//	pThisNode->appendChild(pLastSelectedFormatID);
//	pRootNode->appendChild(pThisNode);
//
//	return S_OK;
//}

////#################################################################################################################################
////	_ClassWatch
////#################################################################################################################################
//
////*******************************************************************************
////	ClassWatch デフォルト値の設定
////*******************************************************************************
//AppSetting::_ClassWatch::_ClassWatch()
//{
//	this->MinVoltage     = 11.0;
//	this->MaxVoltage     = 12.6;
//	this->AlertVoltage   = 11.5;
//	this->VoltageAlertEnable = TRUE;
//}
//
////*******************************************************************************
////	電圧監視関連の値設定
////*******************************************************************************
//BOOL AppSetting::_ClassWatch::SetValue_Voltage(double MinVoltage, double MaxVoltage, double AlertVoltage, BOOL VoltageAlertEnable)
//{
//	if( TestValue_Voltage(MinVoltage, MaxVoltage, AlertVoltage, VoltageAlertEnable) ) return FALSE;
//
//	this->MinVoltage     = ROUND(MinVoltage * 10.0) / 10.0;
//	this->MaxVoltage     = ROUND(MaxVoltage * 10.0) / 10.0;
//	this->AlertVoltage   = ROUND(AlertVoltage * 10.0) / 10.0;
//
//	return TRUE;
//}
//
////*******************************************************************************
////	電圧監視関連の値チェック
////
//// Return:
////	0     - すべて有効
////	0以外 - 有効でない値がある．（i番目の引数が有効でないとき，下からi番目のビットが立つ）
////*******************************************************************************
//DWORD AppSetting::_ClassWatch::TestValue_Voltage(double MinVoltage, double MaxVoltage, double AlertVoltage, BOOL VoltageAlertEnable)
//{
//	DWORD Ret = 0;
//
//	MinVoltage   = ROUND(MinVoltage * 10.0) / 10.0;
//	MaxVoltage   = ROUND(MaxVoltage * 10.0) / 10.0;
//	AlertVoltage = ROUND(AlertVoltage * 10.0) / 10.0;
//
//	Ret |= (MinVoltage <= 0.0) | (MinVoltage >= MaxVoltage) ? 0x01 : 0;
//	Ret |= (MinVoltage >= MaxVoltage)                       ? 0x02 : 0;
//	Ret |= (AlertVoltage <= 0.0)                            ? 0x04 : 0;
//
//	return Ret;
//}
//
////*******************************************************************************
////	監視機能 関連データのロード
////*******************************************************************************
//HRESULT AppSetting::_ClassWatch::_Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
//{
//	// Watch ノードの取得
//	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./Watch"));
//	if( pThisNode == NULL ) return S_FALSE; // ノードが存在しないのでデフォルト値を使用
//
//	LoadData(pThisNode, _T("MinVoltage"),         this->MinVoltage);     
//	LoadData(pThisNode, _T("MaxVoltage"),         this->MaxVoltage);     
//	LoadData(pThisNode, _T("AlertVoltage"),       this->AlertVoltage);  
//	LoadData(pThisNode, _T("VoltageAlertEnable"), this->VoltageAlertEnable);
//
//	return S_OK;
//}
//
////*******************************************************************************
////	監視機能 関連データのセーブ
////*******************************************************************************
//HRESULT AppSetting::_ClassWatch::_Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
//{
//	// Watch ノードの取得
//	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./Watch"));
//	
//	if( pThisNode ) {
//		// ノードが存在する
//		pRootNode->removeChild(pThisNode); // 削除
//	}
//	
//	// MainLink ノードを新規作成
//	pThisNode = pDoc->createElement(_bstr_t("Watch")); 
//
//	SaveFormatData(pDoc, pThisNode, _T("MinVoltage"),         _T("%4.1f"), this->MinVoltage);
//	SaveFormatData(pDoc, pThisNode, _T("MaxVoltage"),         _T("%4.1f"), this->MaxVoltage);
//	SaveFormatData(pDoc, pThisNode, _T("AlertVoltage"),       _T("%4.1f"), this->AlertVoltage);
//	SaveFormatData(pDoc, pThisNode, _T("VoltageAlertEnable"), _T("%d"),    this->VoltageAlertEnable != FALSE);
//
//	pRootNode->appendChild(pThisNode);
//
//	return S_OK;
//}

//#################################################################################################################################
//	_ClassComm
//#################################################################################################################################

//*******************************************************************************
//	ClassComm デフォルト値の設定
//*******************************************************************************
AppSetting::_ClassComm::_ClassComm()
{
	this->MainPort = 0;

	this->SubLinkEnable = FALSE;
	this->SubPort = 1;
}

//*******************************************************************************
//	COM ポート関連データのロード
//*******************************************************************************
HRESULT AppSetting::_ClassComm::_Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
{
	// ノードの取得
	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./Comm"));
	if (pThisNode == NULL) return S_FALSE; // ノードが存在しないのでデフォルト値を使用

	// MainPort のロード
	LoadData(pThisNode, _T("MainPort"), this->MainPort);

	// SubPort のロード
	LoadData(pThisNode, _T("SubLinkEnable"), this->SubLinkEnable);
	LoadData(pThisNode, _T("SubPort"), this->SubPort);

	return S_OK;
}

//*******************************************************************************
//	COM ポート関連データのセーブ
//*******************************************************************************
HRESULT AppSetting::_ClassComm::_Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
{
	// Comm ノードの取得
	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./Comm"));

	if (pThisNode) {
		// Comm ノードが存在する
		pRootNode->removeChild(pThisNode); // 削除
	}

	// Comm ノードを新規作成
	pThisNode = pDoc->createElement(_bstr_t("Comm"));

	// MainPort の保存
	SaveFormatData(pDoc, pThisNode, _T("MainPort"), _T("%d"), this->MainPort);

	// SubPort の保存
	SaveFormatData(pDoc, pThisNode, _T("SubLinkEnable"), _T("%d"), this->SubLinkEnable != FALSE);
	SaveFormatData(pDoc, pThisNode, _T("SubPort"), _T("%d"), this->SubPort);

	pRootNode->appendChild(pThisNode);

	return S_OK;
}

////#################################################################################################################################
////	_ClassMainLink
////#################################################################################################################################
//
////*******************************************************************************
////	ClassMainLink デフォルト値の設定
////*******************************************************************************
//AppSetting::_ClassMainLink::_ClassMainLink()
//{
//	this->MinimumPropoPacket = TRUE;
//}
//
////*******************************************************************************
////	メイン回線関連データのロード
////*******************************************************************************
//HRESULT AppSetting::_ClassMainLink::_Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
//{
//	// ノードの取得
//	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./MainLink"));
//	if( pThisNode == NULL ) return S_FALSE; // ノードが存在しないのでデフォルト値を使用
//
//	// 値のロード
//	LoadData(pThisNode, _T("MinimumPropoPacket"), this->MinimumPropoPacket);
//	
//	return S_OK;
//}
//
////*******************************************************************************
////	メイン回線関連データのセーブ
////*******************************************************************************
//HRESULT AppSetting::_ClassMainLink::_Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
//{
//	// MainLink ノードの取得
//	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./MainLink"));
//	
//	if( pThisNode ) {
//		// ノードが存在する
//		pRootNode->removeChild(pThisNode); // 削除
//	}
//	
//	// ノードを新規作成
//	pThisNode = pDoc->createElement(_bstr_t("MainLink")); 
//
//	// 値のセーブ
//	SaveFormatData(pDoc, pThisNode, _T("MinimumPropoPacket"), _T("%d"), this->MinimumPropoPacket != FALSE);
//
//	pRootNode->appendChild(pThisNode);
//
//	return S_OK;
//}

//#################################################################################################################################
//	_ClassXMonitorServer
//#################################################################################################################################

//*******************************************************************************
//	ClassXMonitorServer デフォルト値の設定
//*******************************************************************************
AppSetting::_ClassXMonitorServer::_ClassXMonitorServer()
{
	this->Enabled = FALSE;
	this->PortNo = 53799;
}

//*******************************************************************************
//	Port番号の値設定
//*******************************************************************************
BOOL AppSetting::_ClassXMonitorServer::SetValue(BOOL Enabled, WORD PortNo)
{
	if (TestValue(Enabled, PortNo) == 0) {
		this->Enabled = Enabled;
		this->PortNo = PortNo;
		return TRUE;
	}
	else {
		return FALSE;
	}
}

//*******************************************************************************
//	Port番号の値チェック
//*******************************************************************************
DWORD AppSetting::_ClassXMonitorServer::TestValue(BOOL Enabled, WORD PortNo)
{
	return PortNo >= 49152 ? 0 : 0x2;
}

//*******************************************************************************
//	XMonitorServer関連データのロード
//*******************************************************************************
HRESULT AppSetting::_ClassXMonitorServer::_Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
{
	// ノードの取得
	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./XMonitorServer"));
	if (pThisNode == NULL) return S_FALSE; // ノードが存在しないのでデフォルト値を使用

	// 値のロード
	LoadData(pThisNode, _T("Enabled"), this->Enabled);
	WORD PortNo;
	LoadData(pThisNode, _T("PortNo"), PortNo);
	if (TestValue(TRUE, PortNo)) this->PortNo = PortNo;

	return S_OK;
}

//*******************************************************************************
//	XMonitorServer関連データのセーブ
//*******************************************************************************
HRESULT AppSetting::_ClassXMonitorServer::_Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
{
	// ノードの取得
	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./XMonitorServer"));

	if (pThisNode) {
		// ノードが存在する
		pRootNode->removeChild(pThisNode); // 削除
	}

	// ノードを新規作成
	pThisNode = pDoc->createElement(_bstr_t("XMonitorServer"));

	// 値のセーブ
	SaveFormatData(pDoc, pThisNode, _T("Enabled"), _T("%d"), this->Enabled != FALSE);
	SaveFormatData(pDoc, pThisNode, _T("PortNo"), _T("%d"), this->PortNo);

	pRootNode->appendChild(pThisNode);

	return S_OK;
}

////#################################################################################################################################
////	_ClassUSBPropo
////#################################################################################################################################
//
////*******************************************************************************
////	ClassUSBPropo デフォルト値の設定
////*******************************************************************************
//AppSetting::_ClassUSBPropo::_ClassUSBPropo()
//{
//	this->Enabled = TRUE;
//	this->Swap_Throttle_Elevator = FALSE;
//	this->Swap_Aileron_Rudder = FALSE;
//	this->Trim_Throttle = 0;
//	this->Trim_Aileron = 0;
//	this->Trim_Elevator = 0;
//	this->Trim_Rudder = 0;
//	this->Invert_Sw1 = FALSE;
//	this->Invert_Sw2 = FALSE;
//	this->Invert_Sw3 = FALSE;
//	this->Invert_Sw4 = FALSE;
//}
//
////*******************************************************************************
////	USBPropo関連データのロード
////*******************************************************************************
//HRESULT AppSetting::_ClassUSBPropo::_Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
//{
//	// ノードの取得
//	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./USBPropo"));
//	if( pThisNode == NULL ) return S_FALSE; // ノードが存在しないのでデフォルト値を使用
//
//	// 値のロード
//	LoadData(pThisNode, _T("Enabled"),                this->Enabled);
//	LoadData(pThisNode, _T("Swap_Throttle_Elevator"), this->Swap_Throttle_Elevator);
//	LoadData(pThisNode, _T("Swap_Aileron_Rudder"),    this->Swap_Aileron_Rudder);
//	LoadData(pThisNode, _T("Trim_Throttle"),          this->Trim_Throttle);
//	LoadData(pThisNode, _T("Trim_Aileron"),           this->Trim_Aileron);
//	LoadData(pThisNode, _T("Trim_Elevator"),          this->Trim_Elevator);
//	LoadData(pThisNode, _T("Trim_Rudder"),            this->Trim_Rudder);
//	LoadData(pThisNode, _T("Invert_Sw1"),             this->Invert_Sw1);
//	LoadData(pThisNode, _T("Invert_Sw2"),             this->Invert_Sw2);
//	LoadData(pThisNode, _T("Invert_Sw3"),             this->Invert_Sw3);
//	LoadData(pThisNode, _T("Invert_Sw4"),             this->Invert_Sw4);
//	
//	return S_OK;
//}
//
////*******************************************************************************
////	USBPropo関連データのセーブ
////*******************************************************************************
//HRESULT AppSetting::_ClassUSBPropo::_Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
//{
//	// ノードの取得
//	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./USBPropo"));
//	
//	if( pThisNode ) {
//		// ノードが存在する
//		pRootNode->removeChild(pThisNode); // 削除
//	}
//	
//	// ノードを新規作成
//	pThisNode = pDoc->createElement(_bstr_t("USBPropo")); 
//
//	// 値のセーブ
//	SaveFormatData(pDoc, pThisNode, _T("Enabled"),                 _T("%d"), this->Enabled                != FALSE);
//	SaveFormatData(pDoc, pThisNode, _T("Swap_Throttle_Elevator"),  _T("%d"), this->Swap_Throttle_Elevator != FALSE);
//	SaveFormatData(pDoc, pThisNode, _T("Swap_Aileron_Rudder"),     _T("%d"), this->Swap_Aileron_Rudder    != FALSE);
//	SaveFormatData(pDoc, pThisNode, _T("Trim_Throttle"),           _T("%d"), this->Trim_Throttle);
//	SaveFormatData(pDoc, pThisNode, _T("Trim_Aileron"),            _T("%d"), this->Trim_Aileron);
//	SaveFormatData(pDoc, pThisNode, _T("Trim_Elevator"),           _T("%d"), this->Trim_Elevator);
//	SaveFormatData(pDoc, pThisNode, _T("Trim_Rudder"),             _T("%d"), this->Trim_Rudder);
//	SaveFormatData(pDoc, pThisNode, _T("Invert_Sw1"),              _T("%d"), this->Invert_Sw1 != FALSE);
//	SaveFormatData(pDoc, pThisNode, _T("Invert_Sw2"),              _T("%d"), this->Invert_Sw2 != FALSE);
//	SaveFormatData(pDoc, pThisNode, _T("Invert_Sw3"),              _T("%d"), this->Invert_Sw3 != FALSE);
//	SaveFormatData(pDoc, pThisNode, _T("Invert_Sw4"),              _T("%d"), this->Invert_Sw4 != FALSE);
//
//	pRootNode->appendChild(pThisNode);
//
//	return S_OK;
//}


//#################################################################################################################################
//	_ClassFlightLog
//#################################################################################################################################

//*******************************************************************************
//	_ClassFlightLog デフォルト値の設定
//*******************************************************************************
AppSetting::_ClassFlightLog::_ClassFlightLog()
{
	this->FilePath = "C:/Users/Public/Documents/";
	this->RecStopTimeFromDataStop = 5;	// 5sec
	this->DivideSizeInByte = 100000000; // 100MB
	this->StoragePeriodInDay = 180;		// 180日（約6か月）
}

//*******************************************************************************
//	飛行ログ関連データのロード
//*******************************************************************************
HRESULT AppSetting::_ClassFlightLog::_Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
{
	// ノードの取得
	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./FlightLog"));
	if (pThisNode == NULL) return S_FALSE; // ノードが存在しないのでデフォルト値を使用

	// FilePath のロード
	LoadData(pThisNode, _T("FilePath"), this->FilePath);

	// RecStopTimeFromDataStopのロード
	LoadData(pThisNode, _T("RecStopTimeFromDataStop"), this->RecStopTimeFromDataStop);

	// DivideSizeInByteのロード
	LoadData(pThisNode, _T("DivideSizeInByte"), this->DivideSizeInByte);

	// StoragePeriodInDayのロード
	LoadData(pThisNode, _T("StoragePeriodInDay"), this->StoragePeriodInDay);

	return S_OK;
}

//*******************************************************************************
//	飛行ログ関連データのセーブ
//*******************************************************************************
HRESULT AppSetting::_ClassFlightLog::_Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode)
{
	// FlightLog ノードの取得
	MSXML2::IXMLDOMNodePtr pThisNode = pRootNode->selectSingleNode(_bstr_t("./FlightLog"));

	if (pThisNode) {
		// FlightLog ノードが存在する
		pRootNode->removeChild(pThisNode); // 削除
	}

	// FlightLog ノードを新規作成
	pThisNode = pDoc->createElement(_bstr_t("FlightLog"));

	// FilePath の保存(ワイド文字にしてから)
	const size_t newsizew = strlen(this->FilePath.c_str()) + 1;
	size_t convertedCharsNum = 0;
	wchar_t *wcstring = new wchar_t[newsizew];
	setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
	mbstowcs_s(&convertedCharsNum, wcstring, newsizew, this->FilePath.c_str(), _TRUNCATE);
	SaveFormatData(pDoc, pThisNode, _T("FilePath"), _T("%s"), wcstring);
	delete[] wcstring;
	
	// RecStopTimeFromDataStopの保存
	SaveFormatData(pDoc, pThisNode, _T("RecStopTimeFromDataStop"), _T("%d"), this->RecStopTimeFromDataStop);

	// DivideSizeInByteの保存
	SaveFormatData(pDoc, pThisNode, _T("DivideSizeInByte"), _T("%d"), this->DivideSizeInByte);

	// StoragePeriodInDayの保存
	SaveFormatData(pDoc, pThisNode, _T("StoragePeriodInDay"), _T("%d"), this->StoragePeriodInDay);

	pRootNode->appendChild(pThisNode);

	return S_OK;
}


//#################################################################################################################################
//	補助関数
//#################################################################################################################################

BOOL SaveFormatData(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pTarget, LPCTSTR NodeName, LPCTSTR pszFormat, ...)
{
	CString s;

	va_list p;
	va_start(p, pszFormat);

	s.FormatV(pszFormat, p);

	va_end(p);

	// ノード生成
	MSXML2::IXMLDOMNodePtr pDataNode = pDoc->createElement(_bstr_t(NodeName));

	// テキスト設定
	pDataNode->text = _bstr_t(s);

	// 追加
	pTarget->appendChild(pDataNode);

	return TRUE;
}

BOOL LoadData(MSXML2::IXMLDOMNodePtr& pSourceNode, LPCTSTR NodeName, int& Destination)
{
	MSXML2::IXMLDOMNodePtr pDataNode = pSourceNode->selectSingleNode(_bstr_t("./") + NodeName);
	if (pDataNode) {
		return NodeTextToInt(pDataNode, &Destination);
	}
	else {
		return FALSE;
	}
}

BOOL LoadData(MSXML2::IXMLDOMNodePtr& pSourceNode, LPCTSTR NodeName, unsigned short& Destination)
{
	MSXML2::IXMLDOMNodePtr pDataNode = pSourceNode->selectSingleNode(_bstr_t("./") + NodeName);
	if (pDataNode) {
		int  value = -1;
		if (NodeTextToInt(pDataNode, &value)) {
			if (0 <= value && value <= 0xFFFF) {
				Destination = value;
				return TRUE;
			}
		}
		return FALSE;
	}
	else {
		return FALSE;
	}
}

BOOL LoadData(MSXML2::IXMLDOMNodePtr& pSourceNode, LPCTSTR NodeName, double& Destination)
{
	MSXML2::IXMLDOMNodePtr pDataNode = pSourceNode->selectSingleNode(_bstr_t("./") + NodeName);
	if (pDataNode) {
		return NodeTextToDouble(pDataNode, &Destination);
	}
	else {
		return FALSE;
	}
}

BOOL LoadData(MSXML2::IXMLDOMNodePtr& pSourceNode, LPCTSTR NodeName, std::string& Destination)
{
	MSXML2::IXMLDOMNodePtr pDataNode = pSourceNode->selectSingleNode(_bstr_t("./") + NodeName);
	if (pDataNode) {
		Destination = CW2A(pDataNode->text);
		return TRUE;
	}
	else {
		return FALSE;
	}
}