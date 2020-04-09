//
//		AppSetting.h
//
//		設定データ管理クラス
//
//		'09/02/15	Ver 0.01	プログラミング開始	by iwakura
//
//
//		注：MSXML が提供するポインタ型(IXMLDOM...Ptr)はスマートポインタなので自動で初期化・開放されます
//			C...App の InitInstance に AfxOleInit() を追加してください．
//
//

#ifndef INCLUDED_APPSETTING_H
#define INCLUDED_APPSETTING_H

#include <windows.h>
#include <vector>

//#import "msxml3.dll" named_guids
#import "msxml6.dll" named_guids

class AppSetting
{
public:
	// typedef
	typedef std::vector<int> intvec;

	// --------------------------------------------------
	// アプリケーション関連の設定
	// --------------------------------------------------
	class _ClassApp {
	public:
		CString MonitorIdName; // モニター識別子（クライアント名兼用）

	public:
		_ClassApp();

		BOOL TestValue_MonitorIdName(CString& MonitorIdName);

		HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
		HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	} App;

	// --------------------------------------------------
	// ファイル記録 関連機能の設定
	// --------------------------------------------------
	//class _ClassDataFileRecorder {
	//public:
	//	CGuid LastSelectedFormatID;

	//public:
	//	_ClassDataFileRecorder();

	//	HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//	HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//} DataFileRecorder;

	// --------------------------------------------------
	// 監視機能の設定
	// --------------------------------------------------
	//class _ClassWatch {
	//public:
	//	double MinVoltage;         // バッテリーの最低電圧
	//	double MaxVoltage;         // バッテリーの最高電圧
	//	double AlertVoltage;       // アラート電圧
	//	BOOL   VoltageAlertEnable; // 警告音を鳴らす

	//public:
	//	_ClassWatch();
	//	
	//	BOOL   SetValue_Voltage(double MinVoltage, double MaxVoltage, double AlertVoltage, BOOL VoltageAlertEnable);
	//	DWORD  TestValue_Voltage(double MinVoltage, double MaxVoltage, double AlertVoltage, BOOL VoltageAlertEnable);

	//	HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//	HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//} Watch;

	// --------------------------------------------------
	// シリアルポート関連の設定
	// --------------------------------------------------
	class _ClassComm {
	public:
		_ClassComm();

		int  MainPort;      // MainLink 用の COM ポート番号 ( -1 は未設定 )

		BOOL SubLinkEnable; // SubLink を使用する
		int  SubPort;       // SubLink 用の COM ポート番号 ( -1 は未設定 )
		
		HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
		HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	} Comm;
	
	// --------------------------------------------------
	// MainLinkの設定
	// --------------------------------------------------
	//class _ClassMainLink {
	//public:
	//	_ClassMainLink();

	//	BOOL MinimumPropoPacket; // プロポのパケットサイズを最小にする

	//	HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//	HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//} MainLink;

	// --------------------------------------------------
	// Winsock サーバー(XMonitorServer)の設定
	// --------------------------------------------------
	class _ClassXMonitorServer {
	public:
		_ClassXMonitorServer();

		BOOL Enabled; // TCP/IP ネットワークで他のアプリケーションと通信する
		WORD PortNo;  // XMonitorServerが使用するポート番号


		BOOL  SetValue(BOOL Enabled, WORD PortNo);
		DWORD TestValue(BOOL Enabled, WORD PortNo);

		HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
		HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	} XMonitorServer;

	//// --------------------------------------------------
	//// USBプロポの設定
	//// --------------------------------------------------
	//class _ClassUSBPropo {
	//public:
	//	_ClassUSBPropo();

	//	BOOL Enabled;                // USBプロポを有効にする
	//	BOOL Swap_Throttle_Elevator; // スロットルとエレベータを入れ替える
	//	BOOL Swap_Aileron_Rudder;    // エルロンとラダーを入れ替える
	//	int  Trim_Throttle;          // スロットルのトリム
	//	int  Trim_Aileron;           // エルロンのトリム
	//	int  Trim_Elevator;          // エレベータのトリム
	//	int  Trim_Rudder;            // ラダーのトリム
	//	BOOL Invert_Sw1;             // Sw1を反転する
	//	BOOL Invert_Sw2;             // Sw2を反転する
	//	BOOL Invert_Sw3;             // Sw3を反転する
	//	BOOL Invert_Sw4;             // Sw4を反転する

	//	HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//	HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//} USBPropo;
	
	// --------------------------------------------------
	// データ保存/再生の設定
	// --------------------------------------------------
	class _ClassFlightLog {
	public:
		_ClassFlightLog();

		std::string FilePath;			// 飛行ログファイルのパス
		int RecStopTimeFromDataStop;	// UAVからデータが来ない状態が何秒続いたら保存をやめるか
		int DivideSizeInByte;			// 飛行ログの分割サイズ[byte]
		int StoragePeriodInDay;			// 飛行ログの保存期間[day]（自動削除用）

		HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
		HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	} FlightLog;


	// ウィンドウ関連の設定

private: // プライベートメンバ変数

	HANDLE m_hFile; // 設定ファイルのハンドル

public:
	AppSetting();
	virtual ~AppSetting();

	// 設定データを保存
	HRESULT Load();
	HRESULT Save();

private: // プライベートメンバ関数

	// XMLファイルをオープン
	HRESULT _OpenFile();
	HRESULT _CommandLine();
	HRESULT _CreateFile(MSXML2::IXMLDOMDocument2Ptr& pDoc); //空のファイルを作成
	HRESULT _LockFile();
	HRESULT _UnlockFile();
};

#endif