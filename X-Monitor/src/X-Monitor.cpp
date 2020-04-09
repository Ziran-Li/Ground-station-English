
// X-Monitor.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"

#include "X-Monitor.h"
#include "X-MonitorDlg.h"
#include "MMTimer.h"
#include "XmlMessage.h"
#include "XmlData.h"
#include "Base64Data.h"
#include "Codec.h"
#include "FlightLog.h"
#include "XmlMsgDef.h"
#include <fstream>
#include "common/SystemTimeSetter_Win.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace MiniSvCodec;

/* 全局变量*/
map<int, string> dataTypeMap;

// CXMonitorApp

BEGIN_MESSAGE_MAP(CXMonitorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CXMonitorApp 构造函数

CXMonitorApp::CXMonitorApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此位置添加构造代码。

	
	//请写在 InitInstance 中所有重要的初始化到这里。
	::AllowOnlyMainThread(TRUE);

	m_pWorkThread = NULL;
}


// 这是唯一的 CXMonitorApp 对象。

CXMonitorApp theApp;


// CXMonitorApp 初始化

BOOL CXMonitorApp::InitInstance()
{
	CWinApp::InitInstance();

	// 在对话框中的命令行管理程序树视图或shell列表视图控件
	//创建一个shell经理如果它包含//。
	CShellManager *pShellManager = new CShellManager;

	// 激活"窗口本机"视觉管理器来启用 MFC ActiveX 控件的主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	//标准初始化

	//而不是使用这些功能的最终的可执行文件

	//如果您想要减少的大小从以下必要的初始化

	//请删除该例程。

	//更改存储设置的注册表项。

	//TODO : 适当的字符串，例如一个公司或组织的名称

	//   请更改此字符串。
	SetRegistryKey(_T("应用程序向导产生的本地应用"));

	// --- 直到最后由从这里程序员编程----

	// COM口初始化
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (!SUCCEEDED(hr))
	{
		AfxMessageBox(_T("COM口初始化失败"));
	}

	// 生成 XML 数据管理类
	m_xmlDataDb = new dataManage;

	// 映射初始化数据类型 (XML 数据管理类)
	Base64Data::datatypeMap();

	// 您可以使用主链接管理类
	m_MainLinkManage.Initialize(&m_MainLink, m_xmlDataDb);

	// X 监视器模式
	m_opeMode = MODE_REC_CLOSED;

	// 重置主链接数据收到相关变量
	m_mainLinkNoDataCnt = 0;

	// 时间的测量
	m_prevTime = 0;
	m_aveCycle = 0;
	m_cntCycle = 0;

	for(int i=0; i<4; ++i) m_cntOver[i] = 0;

	// 保存在启动时的当前目录
	DWORD length = ::GetCurrentDirectory(0, NULL);
	TCHAR* curdir = new TCHAR[length];
	::GetCurrentDirectory(length, curdir);
	m_InitialDirectory = curdir;
	delete[] curdir;

	//加载设置
	m_AppSetting.GetAccessor()->Load();
	m_CurrentMonitorIdName = m_AppSetting.GetAccessor()->App.MonitorIdName;

	// 注册配置数据库
	RegisterSettingValToDB();

	// 拥有同样的MonitorIdName过程其他存在的场合，不启动 不会重复启动
	CString MutexName = _T("XMonitorApp_");
	MutexName += GetCurrentMonitorIdName();
	CMutex SingleProc(FALSE, MutexName);
	if (SingleProc.Lock(0)) {
		//MutexName  同样拥有其他过程是不存在的

			//从这里 / / == == = 独占控制必须 == == =

			// 线程（WaitForSingleObject可）生成

		m_pWorkThread = AfxBeginThread(CXMonitorApp_ThreadFunc, (LPVOID)this, 0, 0, CREATE_SUSPENDED); // 挂起的线程
		m_pWorkThread->m_bAutoDelete = FALSE; // 防止自动开启的退出线程

		// 生成对话框并显示
		CXMonitorDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal(); // 此线程是在这里停止，直到对话框关闭

		// 线程终止声明张贴
		m_pWorkThread->PostThreadMessageW(WM_QUIT, 0, 0);
		::WaitForSingleObject(m_pWorkThread->m_hThread, INFINITE);
		delete m_pWorkThread; // 删除 m_pWorkThread; 丢弃的线程

		//= = = = = 在这里我们不需要专用系统 = = = = = 。

		//从数据库中检索的设置
		RetrieveSettingValFromDB();

		// 要保存的设置
		m_AppSetting.GetAccessor()->Save();

	}
	else {
		// 多重引导
		AfxMessageBox(_T("ID ") + GetCurrentMonitorIdName() + _T(" 因为有其他监视器启动，启动终止。"));
	}

	// 删除管理器创建的管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 关闭此对话框。 无法启动应用程序的消息泵

	//返回 FALSE 来终止该应用程序。

	return FALSE;
}

int CXMonitorApp::ExitInstance()
{
	// TODO: 偙偙偵摿掕側僐乕僪傪捛壛偡傞偐丄傕偟偔偼婎掙僋儔僗傪屇傃弌偟偰偔偩偝偄丅

	//	因为之前释放 MSXML 级对象(dataManage 类控股)，

	//删除这里的 dataManage 类的对象

	//删除 m_xmlDataDb;
	delete m_xmlDataDb;

	// COM关闭过程
	CoUninitialize();

	return CWinApp::ExitInstance();
}

//*******************************************************************************
//*******************************************************************************

//在主窗口中发送消息

//

//注意 :

//或不在这个以确保可以焦虑...确保如果的 try - catch 吗 ?
//*******************************************************************************
LRESULT CXMonitorApp::SendMessageToMainWnd_Safe(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (CWnd* pMainWnd = this->GetMainWnd()) {
		if (pMainWnd->GetSafeHwnd()) {
			return pMainWnd->SendMessage(message, wParam, lParam);
		}
	}

	return 0;
}

LRESULT CXMonitorApp::PostMessageToMainWnd_Safe(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (CWnd* pMainWnd = this->GetMainWnd()) {
		if (pMainWnd->GetSafeHwnd()) {
			return pMainWnd->PostMessage(message, wParam, lParam);
		}
	}

	return 0;
}

//*******************************************************************************
//	要注册到 DB 从初始化文件读取的值
//*******************************************************************************
void CXMonitorApp::RegisterSettingValToDB()
{
	HRESULT hr;

	// 路径
	vector<_bstr_t> path;
	path.push_back(L"message");
	path.push_back(L"Config");

	// 属性 (消息)
	MSGATTRIBUTES msg_attr;
	msg_attr = setMsgAttribute(L"set",NULL, NULL, L"Config");  //type, name, cycle, a

	// 属性 (数据))
	ELEMATTRIBUTES elem_attr;
	elem_attr = setAttributeACI(NULL, NULL, NULL);	//懏惈a, 懏惈c, 懏惈i

	//XML 数据管理司对注册表 (错误检查省略

	// COM 端口
	path.push_back(L"COM");
	path.push_back(L"Port");
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, 0, m_AppSetting.GetAccessor()->Comm.MainPort );

	// 飞行日志文件路径 (宽字符从)
	path[2] = L"FlightLog";
	path[3] = L"FilePath";
	setlocale(LC_ALL, "Japanese_Japan.932");	//<-添加
	BSTR bstrFilePath = A2BSTR(m_AppSetting.GetAccessor()->FlightLog.FilePath.c_str());
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, (LPCTSTR)bstrFilePath);

	::SysFreeString(bstrFilePath);

	//飞行日志保留时间
	path[3] = L"StoragePeriodInDay";
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, 0, m_AppSetting.GetAccessor()->FlightLog.StoragePeriodInDay );

	//从UAV数据不来状态持续了几秒保存停止
	path[3] = L"RecStopTimeFromDataStop";
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, 0, m_AppSetting.GetAccessor()->FlightLog.RecStopTimeFromDataStop);

	// 飞行日志分区大小
	path[3] = L"DivideSizeInByt";
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, 0, m_AppSetting.GetAccessor()->FlightLog.DivideSizeInByte);
}

//*******************************************************************************
//从 DB 出发
//*******************************************************************************
void CXMonitorApp::RetrieveSettingValFromDB()
{
	HRESULT hr;

	// 路径
	vector<_bstr_t> path;
	path.push_back(L"message");
	path.push_back(L"Config");

	// 属性 (消息)
	MSGATTRIBUTES msg_attr;
	msg_attr = setMsgAttribute(L"set",NULL, NULL, L"Config");  //type, name, cycle, a

	// 属性 (数据)
	ELEMATTRIBUTES elem_attr;
	elem_attr = setAttributeACI(NULL, NULL, NULL);	//懏惈a, 懏惈c, 懏惈i

	//(错误检查省略) 从 XML 数据管理司

	//	COM 端口
	path.push_back(L"COM");
	path.push_back(L"Port");
	hr = m_xmlDataDb->getOneElement(path, elem_attr, msg_attr, &m_AppSetting.GetAccessor()->Comm.MainPort );

	//飞行日志文件路径
	path[2] = L"FlightLog";
	path[3] = L"FilePath";
	TCHAR val[BASE64_BUFFER_SIZE];
	hr = m_xmlDataDb->getOneElement(path, elem_attr, msg_attr, val);
	m_AppSetting.GetAccessor()->FlightLog.FilePath = CW2A(val);

	//	飞行日志保留时间
	path[3] = L"StoragePeriodInDay";
	hr = m_xmlDataDb->getOneElement(path, elem_attr, msg_attr, &m_AppSetting.GetAccessor()->FlightLog.StoragePeriodInDay );

	// 停止保存状态数据不会来自无人机随后几秒钟?
	path[3] = L"RecStopTimeFromDataStop";
	hr = m_xmlDataDb->getOneElement(path, elem_attr, msg_attr, &m_AppSetting.GetAccessor()->FlightLog.RecStopTimeFromDataStop);

	// 飞行日志分区大小 [字节]
	path[3] = L"DivideSizeInByte";
	hr = m_xmlDataDb->getOneElement(path, elem_attr, msg_attr, &m_AppSetting.GetAccessor()->FlightLog.DivideSizeInByte);
}

BOOL CXMonitorApp::AppendLog(LPCTSTR str)
{
	CExclusiveObject< std::queue<CString> >::ACCESSOR pLogStringQueue = this->m_LogStringQueue.GetAccessor();

	pLogStringQueue->push(str);

	return PostMessageToMainWnd_Safe(WM_APPEND_LOG) == 0;
}

BOOL CXMonitorApp::PopLog(CString& str)
{
	CExclusiveObject< std::queue<CString> >::ACCESSOR pLogStringQueue = this->m_LogStringQueue.GetAccessor();

	if (pLogStringQueue->empty() == false) {
		str = pLogStringQueue->front();
		pLogStringQueue->pop();

		return TRUE;
	}
	else {
		return FALSE;
	}
}

//*******************************************************************************
//	一个线程为后台处理的
//*******************************************************************************
#define N_LOOPCYCLE 10								// 增量每 10 毫秒 nLoopCount
#define N_SAMPLING_TIME 20							// 采样时间 (20 毫秒)
#define N_LOOPCOUNT_PER_CYCLE 6000					//重置 nLoopCount 每 1 分钟
#define N_LOOPCOUNT_SCIOPEN_INTERVAL 500			//每5 s 尝试打开一次 COM 端口
#define N_LOOPCOUNT_CALCTRAFFIC_INTERVAL 100		// 计算通信情况，每隔1秒
#define N_LOOPCOUNT_STARTUP_XMONITORSERVER 100		// 1s 执行一个X - Monitor Server的设置
#define N_LOOPCOUNT_RECEND_DECIDE_INTERVAL 50		// 判断每一个500毫秒的数据存储结束
#define N_LOOPCOUNT_MLTWAY_RETRY_DECIDE_INTERVAL 4	// 每40ms 进行多路命令来判断的重试

UINT CXMonitorApp::CXMonitorApp_ThreadFunc(LPVOID pParam)
{
	CXMonitorApp* pThis = (CXMonitorApp*)pParam;
	CMMTimer      MMTimer;
	UINT          nLoopCount = 0;
	DWORD         tickNext20ms;

	MMTimer.OpenTimer(N_LOOPCYCLE, NULL);
	tickNext20ms = MMTimer.timeGetTime();

	while (true) {
		MMTimer.WaitTimeoutEvent();

		// SLEEP指令发送接收时不能处理的场合，在这里处理
		pThis->_wsub_SendReadyDataToUAV();

		// 测量周期
		pThis->CalcAveCycle();

		volatile DWORD nowtime = MMTimer.timeGetTime();
		
		//SetSystemTime(nowtime);			// 

		BOOL flag20msSync = FALSE;			// 亂TODO亃尰忬偼巊偭偰側偄偑丒丒丒

		// 标志以确保是真的每 20 毫秒
		if (tickNext20ms <= nowtime) {
			tickNext20ms += N_SAMPLING_TIME;
			flag20msSync = TRUE;
		}

		// 检查线程消息
		MSG  msg;
		bool flagExit = false;
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			while (GetMessage(&msg, NULL, 0, 0) >= 0) {

				switch (msg.message) {
				case WM_QUIT: //  退出指令
					flagExit = true;
					break;

				case WM_NOTIFY_SYSTEM_RESUME: // 系统恢复
					tickNext20ms = nowtime + N_SAMPLING_TIME; // 修改时出现的显著转变，当系统关闭
					break;

				case WM_SERVER_DATA_RECEIVE: // 从TCP客户端接收的数据
					pThis->_wsub_ReceiveDataFromXMonitorClient(nLoopCount);
					break;

				case WM_CLIENT_ACCEPT: // TCP客户端接受
					pThis->_wsub_XMonitorClientAccept(msg.wParam, msg.lParam);
					break;

				case WM_CLIENT_CLOSE: // TCP 客户端关闭
					pThis->_wsub_XMonitorClientClose(msg.wParam, msg.lParam);
					break;
				}

				// 退出循环，如果你有邮件
				if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == 0) {
					break;
				}
			}
		}
		if (flagExit) break; //要退出循环

		// 主要链接接收操作
		pThis->_wsub_ReceiveData();

		// 决定重新发送多路点命令
		if (nLoopCount % N_LOOPCOUNT_MLTWAY_RETRY_DECIDE_INTERVAL == 0) { pThis->_wsub_DecideMultiwayCmdRetry(); }

		//连接到 COM 端口
		if (nLoopCount % N_LOOPCOUNT_SCIOPEN_INTERVAL == 0) { pThis->_wsub_OpenMainLink(); }

		//Winsock 服务器
		if (nLoopCount % N_LOOPCOUNT_STARTUP_XMONITORSERVER == 0) { pThis->_wsub_SetupXMonitorServer(); }

		// 发送 TCP 数据
		if (pThis->m_TcpServer.IsInitialized()) { pThis->_wsub_SendDataToXMonitorClient(nLoopCount); }

		//决定要结束数据存储
		if (nLoopCount % N_LOOPCOUNT_RECEND_DECIDE_INTERVAL) { pThis->_wsub_DecideRecStop(); }

		nLoopCount = ++nLoopCount % N_LOOPCOUNT_PER_CYCLE;
		
		

		MMTimer.ResetTimeoutEvent();
	}

	MMTimer.CloseTimer();

	// 平均频率输出到文件 (用于调试)
	//pThis->OutputAveCycle(".\\aveCycle.txt");

	return 0;
}


//*******************************************************************************
//	当主窗口初始化已完成处理
//*******************************************************************************
void CXMonitorApp::OnMainWndInit(CWnd* pMainWnd)
{
	// 要启动辅助线程的执行
	BeginWorkThread();
	this->AppendLog(_T("辅助线程开始运行"));
}

//*******************************************************************************
//	要启动辅助线程
//*******************************************************************************
void CXMonitorApp::BeginWorkThread()
{
	if (m_pWorkThread) {
		m_pWorkThread->ResumeThread();
	}
}

//*******************************************************************************
//	系统重新启动主窗口接收的消息通知 CXMonitorApp
//
// Lock:
//*******************************************************************************
void CXMonitorApp::NotifySystemResume()
{
	// 通知线程恢复
	m_pWorkThread->PostThreadMessage(WM_NOTIFY_SYSTEM_RESUME, 0, 0);
}

//*******************************************************************************
//	启用 / 禁用检查监视器的设置 / 获得 / 价值
//
// 注意:

//更改监视器标识符从一开始的下一步将是有效...
//
// Lock:
//	SET
//*******************************************************************************
BOOL CXMonitorApp::SetMonitorIdName(CString& MonitorIdName)
{
	CExclusiveObject<AppSetting>::ACCESSOR pAppSetting = this->m_AppSetting.GetAccessor();

	if (pAppSetting->App.MonitorIdName != MonitorIdName) {
		AfxMessageBox(_T("监视器标识符的变更从下次的起动开始有效"));
		pAppSetting->App.MonitorIdName = MonitorIdName;
	}

	return TRUE;
}

const CString& CXMonitorApp::GetMonitorIdName()
{
	return m_AppSetting.GetAccessor()->App.MonitorIdName;
}

BOOL CXMonitorApp::TestMonitorIdName(CString& MonitorIdName)
{
	return m_AppSetting.GetAccessor()->App.TestValue_MonitorIdName(MonitorIdName);
}

const CString& CXMonitorApp::GetCurrentMonitorIdName()
{
	return m_CurrentMonitorIdName;
}


//*******************************************************************************
//	MainLinkl链接
//
// Lock:
//	MLK, SET
//*******************************************************************************
void CXMonitorApp::_wsub_OpenMainLink()
{
	if (CWnd* pMainWnd = this->GetMainWnd()) {
		if (pMainWnd->GetSafeHwnd()) {

			// 检索的值在数据库中注册
			RetrieveSettingValFromDB();

			CExclusiveObject<AppSetting>::ACCESSOR pAppSetting = this->m_AppSetting.GetAccessor();
			BOOL doConnect = FALSE;	// TRUE:真:连接  假: 不连接

			if (m_MainLink.GetCurrentSciPort() != pAppSetting->Comm.MainPort) {
				// 端口连接和设置都不同
				m_MainLink.CloseSci();
				doConnect = TRUE;
			}
			else {
				// 若要检查，断开连接到 COM 端口时
				if (m_MainLink.IsPortEnable() == FALSE) {
					// 断开连接尝试重新连接
					this->AppendLog(_T("MainLink断开连接"));
					doConnect = TRUE;
				}
				else{
					// 请求和发送答复邮件到
					_bstr_t replyMsg = XML_MSG_COM_CONFIG_REPLY1;
					replyMsg += "OK";
					replyMsg += XML_MSG_COM_CONFIG_REPLY2;
					_wsub_SendReplyMsg(WAITING_CONFIG_COMMAND_REPLY, replyMsg);
				}
			}

			if (doConnect){
				CString s;
				_bstr_t replyMsg = XML_MSG_COM_CONFIG_REPLY1;
				if (m_MainLinkManage.Connect((SCIPORT)pAppSetting->Comm.MainPort)){
					s.Format(_T("MainLink 连接成功 (COM%d)"), pAppSetting->Comm.MainPort);
					replyMsg += "OK";
				}
				else{
					s.Format(_T("MainLink 连接失败 (COM%d)"), pAppSetting->Comm.MainPort);
					replyMsg += "NG";
				}
				this->AppendLog(s);
				replyMsg += XML_MSG_COM_CONFIG_REPLY2;

				// 请求和发送答复邮件到
				_wsub_SendReplyMsg(WAITING_CONFIG_COMMAND_REPLY, replyMsg);
			}

		}
	}
}

//*******************************************************************************
//	将数据发送到无人机
//
// Lock:
//	MLK
//*******************************************************************************
void CXMonitorApp::_wsub_SendDataToUAV(UINT clientID, const _bstr_t &mainlinkSendMsg)
{
	if (!m_MainLinkManage.SendData(clientID, mainlinkSendMsg)){
		this->AppendLog(_T("UAV数据无法发送"));
	}

	//CString str;
	//str.Format(_T("XML暥佀%s"), ((_bstr_t)mainlinkSendMsg).GetBSTR());
	//this->AppendLog(str);
}
void CXMonitorApp::_wsub_SendReadyDataToUAV()
{
	m_MainLinkManage.Send();
}

//*******************************************************************************
//	接收数据
// Lock :
//	MAV, MLK / PIN
//*******************************************************************************
void CXMonitorApp::_wsub_ReceiveData()
{
	// 如果传入的数据是解码和注册 XML 数据管理司
	int packetNum = m_MainLinkManage.DecodeAndRegister();

	// 如果传入的数据
	if(packetNum != 0){
		// 注册本地时间
		m_MainLinkManage.RegisterLocalTime();
		
		// 重置数据不来标志
		m_mainLinkNoDataCnt = 0;

		//设置非传送标志分别获得每个客户端的消息
		//（即使任何数据即将到来，设置时间为所有的获取信息的标志）
		for (ClientInfoMap::iterator it1 = m_ClientInfo.begin(); it1 != m_ClientInfo.end(); ++it1) {		
			for (GetMsgInfoMap::iterator it2 = it1->second.getMsgInfo.begin(); it2 != it1->second.getMsgInfo.end(); ++it2){
				it2->second.hasUnsentData = TRUE;
			}
		}

		//保存要发送如果您不打开飞行日志的欢迎消息
		if(m_opeMode == MODE_REC_CLOSED){
			for (ClientInfoMap::iterator it = m_ClientInfo.begin(); it != m_ClientInfo.end(); ++it) {
				// 确定注册的客户端信息类型的目的地
				if (it->second.type == CLIENTTYPE_REC) {
					// 飞行日志保留启动消息。
					_wsub_SendFlightLogRecStart(it->first);
				}
			}
		}
		if (m_opeMode == MODE_PLAY_OPENED){
			for (ClientInfoMap::iterator it = m_ClientInfo.begin(); it != m_ClientInfo.end(); ++it) {
				// 确定注册的客户端信息类型的目的地
				if (it->second.type == CLIENTTYPE_REC) {
					// 发送消息保存到的飞行日志
					AppendLog(L"错误飞行日志播放中的飞行日志是不能保存");
					AfxMessageBox(_T("错误飞行日志播放中的机体不能连接 ，nX - Monitor结束，请把机体连接"));
				}
			}
		}


		// 如果您收到了确认点命令解包的多路从发送 ACK 和发送命令
		UavLinkManage::MultiwayAck ack;
		if(m_MainLinkManage.PopMultiwayAckData(ack)){
			_wsub_SendMultiwayAck(&ack);
		}
	}

	// 如果没有传入的数据
	else{
		if(m_opeMode == MODE_REC_OPENED) ++m_mainLinkNoDataCnt;		// 只有当我开了飞行日志
	}

	if(m_MainLinkManage.m_dbgMsg != ""){
		this->AppendLog(m_MainLinkManage.m_dbgMsg);
	}
}

//*******************************************************************************
//	判断是否重试多路点命令
//
//锁定：
//设置
//
// Lock:
//	SET
//*******************************************************************************
void CXMonitorApp::_wsub_DecideMultiwayCmdRetry()
{
	if(m_MainLinkManage.DecideMultiwayCmdRetry()){
		AppendLog(L"多点数据发送");
	}
}


//*******************************************************************************
//	设置在Winsock服务器
//
// Lock:
//	SET
//*******************************************************************************
void CXMonitorApp::_wsub_SetupXMonitorServer()
{
	CExclusiveObject<AppSetting>::ACCESSOR pAppSetting = this->m_AppSetting.GetAccessor();

	if (pAppSetting->XMonitorServer.Enabled && (m_TcpServer.IsInitialized() == FALSE)) {
		m_TcpServer.Initialize(pAppSetting->XMonitorServer.PortNo, 10, INFINITE);

		m_TcpServer.RegisterReceiveNoticeMessage(::GetCurrentThreadId(), WM_SERVER_DATA_RECEIVE);
		m_TcpServer.RegisterAcceptNoticeMessage(::GetCurrentThreadId(), WM_CLIENT_ACCEPT);
		m_TcpServer.RegisterCloseNoticeMessage(::GetCurrentThreadId(), WM_CLIENT_CLOSE);

		CString s;
		s.Format(_T("TCP服务器功能有效。 (PortNo : %d)"), pAppSetting->XMonitorServer.PortNo);
		this->AppendLog(s);
	}
	if ((pAppSetting->XMonitorServer.Enabled == FALSE) && m_TcpServer.IsInitialized()) {
		m_TcpServer.Destroy();

		this->AppendLog(_T("TCP服务器机能停止"));
	}
}

//*******************************************************************************
//	将数据发送到X-MonitorClient
//
// Lock:
//	MAV, GCS
//*******************************************************************************
void CXMonitorApp::_wsub_SendDataToXMonitorClient(UINT nLoopCount)
{
	for (ClientInfoMap::iterator it1 = m_ClientInfo.begin(); it1 != m_ClientInfo.end(); ++it1) {

        // 执行按照在客户信息中注册的每个获取消息信息的传输周期的传输。
		// 然而，hasUnsentData（如果有一个=未发送的数据）的情况下为TRUE只送
		for (GetMsgInfoMap::iterator it2 = it1->second.getMsgInfo.begin(); it2 != it1->second.getMsgInfo.end(); ++it2) {
			if (it2->second.hasUnsentData == TRUE && (nLoopCount*N_LOOPCYCLE) % it2->second.cycle == 0) {
				BSTR bstrResultXml = NULL;
				BSTR bstrAttName = A2BSTR(it2->first.c_str());
				int iRet;

				// 从XML数据管理类获取消息的客户端获取传输的数据

				if ((iRet = m_xmlDataDb->getData(it1->first, bstrAttName, &bstrResultXml)) == 0) {
					// 转换XML语句（BSTR型）为std :: string类型

					std::string resultXml = CW2A(bstrResultXml);

					// 发送到客户端
					m_TcpServer.SendPacket(resultXml, it1->first);

					// 返回未发送的标志
					it2->second.hasUnsentData = FALSE;
				}

				::SysFreeString(bstrResultXml);
				::SysFreeString(bstrAttName);
			}
		}

	}
}

//*******************************************************************************
////的	处理从X MonitorClient接收到的数据
//
// Lock:
//	MAV, GCS, MLK
//*******************************************************************************
void CXMonitorApp::_wsub_ReceiveDataFromXMonitorClient(UINT nLoopCount)
{

	std::string recvData;                   //  // XML语句（字符串）商店
	HRESULT hr;                             //处理XML数据管理类的结果
	XmlMessage *xml;                        //// 在XML对象
	int msgType = XmlMessage::msgTypeErr;   // XML语句的消息类型
	UINT clientID = 0;						// 客户识别号
	int msgNo = 0;                          //消息编号来获得（当前形势下，假设只有一个消息标记在XML语句）

	// //处理取出所有包含在接收数据队列中的XML语句的
	while (m_TcpServer.PopRecvData(recvData, clientID) == TRUE) {

        //  //将XML语句（的std :: string类型）为BSTR类型（使用MSXML
		USES_CONVERSION;
		setlocale(LC_ALL, "Japanese_Japan.932");	//<- 增加
		BSTR bstrRecvData = A2BSTR(recvData.c_str());

		xml = new XmlMessage(bstrRecvData, &hr);
		if (hr == S_OK) {
			msgType = xml->getMsgType(msgNo);   //消息类型

            //按照消息类型处理
			switch (msgType) {
			case XmlMessage::msgSet:    // 设置消息
			    {
					if(xml->getMsgAttA(msgNo) == _bstr_t(L"FlightLog")){
						this->AppendLog(_T("FlightLog收到指令"));

						// 要处理飞行记录命令
						_wsub_ProcFlightLogCmd(FlightLog::getFlightLogInfo(bstrRecvData), clientID);
					}
					else if(xml->getMsgAttA(msgNo) == _bstr_t(L"FlightLogReply")){
						this->AppendLog(_T("FlightLog收到指令回复"));
						
						// 注册消息到数据库的飞行记录答复
						m_xmlDataDb->saveData(xml, m_xmlDataDb->setDBID(xml));

						// //要处理飞行记录回复
						_wsub_ProcFlightLogReply(clientID, bstrRecvData);
					}
					else if(xml->getMsgAttA(msgNo) == _bstr_t(L"ClientType")){
						this->AppendLog(_T("接收到客户端识别信息"));
						
						// 注册消息到数据库客户端识别消息
						m_xmlDataDb->saveData(xml, m_xmlDataDb->setDBID(xml));

						// 要处理客户身份信息
						_wsub_ProcClientTypeMsg(clientID);
					}
					else if(xml->getMsgAttA(msgNo) == _bstr_t(L"Config")){
						this->AppendLog(_T("接收设定配置命令"));
						
						// 注册消息到数据库的设置
						m_xmlDataDb->saveData(xml, m_xmlDataDb->setDBID(xml));

						//要处理的命令集
						_wsub_ProcConfigCmd(clientID);
					}
					else if (xml->getMsgAttA(msgNo) == _bstr_t(L"FlightPlan")){
						//this->AppendLog(_T("旘峴寁夋庴怣媦傃揮憲姰椆"));
						//m_MainLinkManage.RegisterMultiWayPointData(true, bstrRecvData);
					}
					else if (xml->getMsgAttA(msgNo) == _bstr_t(L"Clear")){
						this->AppendLog(_T("DB清除"));
						// 清除主DB
						m_xmlDataDb->clearSetMsg(0, 0);
					}
					else if (xml->getMsgAttA(msgNo) == _bstr_t(L"Setting")){
						//this->AppendLog(_T("Core-Setting奐巒"));
						//BSTR bstrNodeValueParent = NULL;
						//IXMLDOMNodePtr pNodeChild = NULL;
						//pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Setting/Core");
						//pNodeChild->get_text(&bstrNodeValueParent);

						//// NULL側傜丄僟儈乕偺嬻暥帤楍偵抲姺
						//int len = wcslen((bstrNodeValueParent) ? bstrNodeValueParent : L"");
						//// API
						//if (SysStringLen(bstrNodeValueParent) > 0)
						//{
						//	if (wcscmp(bstrNodeValueParent, L"Visible")){

						//	}
						//	else if (wcscmp(bstrNodeValueParent, L"Hidden")){
						//		//CDiaslog::OnWindowPosChanging(lpwndpos);
						//		//lpwndpos->flags &= ~SWP_SHOWWINDOW;
						//	}
						//}

					}
					else{
						//this->AppendLog(_T("set儊僢僙乕僕傪庴怣偟傑偟偨"));

						//设置非传送标志分别获得每个客户端的消息
						//（即使任何数据即将到来，设置时间为所有的获取信息的标志）
						for (ClientInfoMap::iterator it1 = m_ClientInfo.begin(); it1 != m_ClientInfo.end(); ++it1) {		
							for (GetMsgInfoMap::iterator it2 = it1->second.getMsgInfo.begin(); it2 != it1->second.getMsgInfo.end(); ++it2){
								it2->second.hasUnsentData = TRUE;
							}
						}

						//注册的消息发送到主数据库
						m_xmlDataDb->saveData(xml, m_xmlDataDb->setDBID(xml));
					}
			    }
			    break;

			case XmlMessage::msgGet:    // 得到消息
    			{
					this->AppendLog(_T("get收到了留言"));

					ClientInfoMap::iterator ite = m_ClientInfo.find(clientID);
					if (ite != m_ClientInfo.end()) {  // 注册
						_bstr_t bstrAttName = getAttName(xml->pDoc, msgNo);	//Name属性
						string attName = CW2A(bstrAttName);
						if(xml->getMsgAttA(msgNo) == _bstr_t(L"Config")){							
							ite->second.getMsgInfo[attName].hasUnsentData = TRUE;		// //未发送的数据“有”; 		
						}
						else{
							ite->second.getMsgInfo[attName].hasUnsentData = FALSE;		// 未发送的数据“不								
						}
                        ite->second.getMsgInfo[attName].cycle = xml->getCycle(msgNo);	// 注册传输周期

					}
					else {                              // 未注册
                        // 错误（不应该在此处输入）

					}

                    // 注册的消息发送到数据库
					m_xmlDataDb->saveData(xml, clientID);
			    }
                break;

			case XmlMessage::msgClose:  // 关闭消息
			    {
					this->AppendLog(_T("close收到了留言"));

					// 删除获取消息信息
					_bstr_t bstrAttName = getAttName(xml->pDoc, msgNo);	// Name属性
					string attName = CW2A(bstrAttName);
					m_ClientInfo[clientID].getMsgInfo.erase(attName);

					// 注册的消息发送到数据库
					m_xmlDataDb->saveData(xml, clientID);
			    }
				break;

			case XmlMessage::msgMainLink:  // mainlink发送消息
				{
					this->AppendLog(_T("Mainlink Send收到了留言"));

					// 注册消息到数据库（Mainlink）
					m_xmlDataDb->saveData(xml, clientID);

					////发送到数据 UAV
					_wsub_SendDataToUAV(clientID, bstrRecvData);

					// 与多路点的客户端信息（等待状态）更新
					if(m_ClientInfo[clientID].status == WAITING_NOTHING && m_MainLinkManage.IsMultiwayTransferring()){
						m_ClientInfo[clientID].status = WAITING_MLTWPT_COMMAND_ACK;
						AppendLog(_T("多路点数据传输"));
					}
					else if(m_ClientInfo[clientID].status == WAITING_MLTWPT_COMMAND_ACK && !m_MainLinkManage.IsMultiwayTransferring()){
						m_ClientInfo[clientID].status = WAITING_NOTHING;	// 由客户端转移取消的情况
					}
				}
				break;

			default:
				break;
			}
					
			delete xml;
		}
		else {
			this->AppendLog(_T("收到了不正确的XML文件"));
		}

		::SysFreeString(bstrRecvData);
	}
}

//*******************************************************************************
//	X-MonitorClient 在 Accept 时候的处理
//
// Lock:
//	LOG
//*******************************************************************************
void CXMonitorApp::_wsub_XMonitorClientAccept(DWORD ipv4dw, UINT clientID)
{
	// 注册的客户端信息
	ClientInfo clientInfo;
	m_ClientInfo.insert(make_pair(clientID, clientInfo));

	// 表示
	BYTE  ip1 = ipv4dw & 0xFF;
	BYTE  ip2 = (ipv4dw >> 8) & 0xFF;
	BYTE  ip3 = (ipv4dw >> 16) & 0xFF;
	BYTE  ip4 = (ipv4dw >> 24) & 0xFF;
	CString str;
	str.Format(_T("客户端连接了 (IP : %d.%d.%d.%d / Total : %d)"), ip1, ip2, ip3, ip4, m_ClientInfo.size());
	this->AppendLog(str);
}

//*******************************************************************************
//	X-MonitorClient 进程结束时
//
// Lock:
//	LOG
//*******************************************************************************
void CXMonitorApp::_wsub_XMonitorClientClose(DWORD ipv4dw, UINT clientID)
{
	//删除客户端信息
	m_ClientInfo.erase(clientID);

	// 昞帵
	BYTE  ip1 = ipv4dw & 0xFF;
	BYTE  ip2 = (ipv4dw >> 8) & 0xFF;
	BYTE  ip3 = (ipv4dw >> 16) & 0xFF;
	BYTE  ip4 = (ipv4dw >> 24) & 0xFF;
	CString str;
	str.Format(_T("断开客户端连接 (IP : %d.%d.%d.%d / Total : %d)"), ip1, ip2, ip3, ip4, m_ClientInfo.size());
	this->AppendLog(str);
}

//*******************************************************************************
//	客户端识别消息处理
//
// Lock:
//
//*******************************************************************************
void CXMonitorApp::_wsub_ProcClientTypeMsg(UINT clientID)
{
	//路径
	vector<_bstr_t> path;
	path.push_back(L"message");
	path.push_back(L"ClientType");
	path.push_back(L"Function");

	// 属性 (消息)
	MSGATTRIBUTES msg_attr;
	msg_attr = setMsgAttribute(L"set",NULL, NULL, L"ClientType");  //type, name, cycle, a

	// 属性 (数据)
	ELEMATTRIBUTES attr;
	attr = setAttributeACI(NULL, NULL, NULL);	//懏惈a, 懏惈c, 懏惈i

	// 价值
	TCHAR val[BASE64_BUFFER_SIZE];

	// 获取客户端类型 (函数)
	if (m_xmlDataDb->getOneElement(path, attr, msg_attr, val) == 0){
		
		ClientInfoMap::iterator ite = m_ClientInfo.find(clientID);
		if (ite == m_ClientInfo.end()) {	//不注册

			//错误(应该是不适合在这里)
			return;
		}

		// 注册
		CString str;
		str.Format(L"client ID:%d = ", clientID);
		if(_tcscmp(val, L"GUI") == 0){
			m_ClientInfo[clientID].type = CLIENTTYPE_GUI;
			str += L"GUI";
		}
		else if(_tcscmp(val, L"REC") == 0){
			m_ClientInfo[clientID].type = CLIENTTYPE_REC;
			str += L"REC";
		}
		else if(_tcscmp(val, L"PLAY") == 0){
			m_ClientInfo[clientID].type = CLIENTTYPE_PLAY;
			str += L"PLAY";
		}
		else{
			m_ClientInfo[clientID].type = CLIENTTYPE_COMMON;
			str += L"COMMON";
		}
		//AppendLog(str);
	}
	else{
		AppendLog(_T("未能获取客户端的类别"));
	}
}

//*******************************************************************************
//	飞行日志命令处理
//
// Lock:
//
//*******************************************************************************
void CXMonitorApp::_wsub_ProcFlightLogCmd(const flightLog &log, UINT clientID)
{
	ClientType dest;

	//如果你属于保存命令 (REC 或END)
	if(log.rec == 1 || log.rec == 0){
		dest = CLIENTTYPE_REC;		//  要将程序保存的目的地
	}
	// 如果你属于播放命令
	else{
		dest = CLIENTTYPE_PLAY;		// 要播放节目的目的地

		// 若要设置的操作模式和每个客户端的挂起
		switch(log.play){
		case 0:	// CLOSE
			m_ClientInfo[clientID].status = WAITING_PLAY_COMMAND_REPLY;
			if(m_opeMode == MODE_PLAY_OPENED){
				m_opeMode = MODE_PLAY_WAITING_CLOSE_REPLY;
			}
			else{
				_wsub_SendReplyMsg(WAITING_PLAY_COMMAND_REPLY, XML_MSG_PLAY_REPLY_NG);
				AppendLog(_T("FlightLog指令（飞行日志close）现在不可执行。"));
				return;
			}
			break;

		case 1:	// OPEN
			m_ClientInfo[clientID].status = WAITING_PLAY_COMMAND_REPLY;
			if(m_opeMode == MODE_REC_CLOSED){	// 从UAV数据来的时候是不能播放的
				m_opeMode = MODE_PLAY_WAITING_OPEN_REPLY;
			}
			else{
				_wsub_SendReplyMsg(WAITING_PLAY_COMMAND_REPLY, XML_MSG_PLAY_REPLY_NG);
				AppendLog(_T("FlightLog指令（飞行日志open）现在不可执行。"));
				return;
			}
			break;

		case 2: // EDIT
			m_ClientInfo[clientID].status = WAITING_PLAY_COMMAND_REPLY;
			if(m_opeMode == MODE_PLAY_OPENED){
				m_opeMode = MODE_PLAY_WAITING_EDIT_REPLY;
			}
			else{
				_wsub_SendReplyMsg(WAITING_PLAY_COMMAND_REPLY, XML_MSG_PLAY_REPLY_NG);
				AppendLog(_T("FlightLog指令（编辑）现在不可执行。"));
				return;
			}
			break;

		default:
			break;
		}
	}

	for (ClientInfoMap::iterator it = m_ClientInfo.begin(); it != m_ClientInfo.end(); ++it) {
        // 根据客户信息登记的类别，决定发送地址
		if (it->second.type == dest) {
			// //转换XML语句（_bstr_t型）为std:: string类型
			std::string strXml = CW2A(log.guiMsg);

			// 发送到客户端
			m_TcpServer.SendPacket(strXml, it->first);
        }
	}
}

//*******************************************************************************
//	飞行日志回复处理
//
// Lock:
//
//*******************************************************************************
void CXMonitorApp::_wsub_ProcFlightLogReply(UINT clientID, const _bstr_t &replyMsg)
{
	// 路径
	vector<_bstr_t> path;
	path.push_back(L"message");
	path.push_back(L"FlightLogReply");

	// 属性（消息）
	MSGATTRIBUTES msg_attr;
	msg_attr = setMsgAttribute(L"set",NULL, NULL, L"FlightLogReply");  //type, name, cycle, a

	// 属性（数据)
	ELEMATTRIBUTES attr;
	attr = setAttributeACI(NULL, NULL, NULL);	//懏惈a, 懏惈c, 懏惈i

	//值
	TCHAR val[BASE64_BUFFER_SIZE];

	// 从回复保存程序
	if(m_ClientInfo[clientID].type == CLIENTTYPE_REC){
		path.push_back(L"Rec");
		path.push_back(L"Result");

		// 获取结果（结果）
		if (m_xmlDataDb->getOneElement(path, attr, msg_attr, val) == 0){
			// "OK"
			if(_tcscmp(val, L"OK") == 0){
				if(m_opeMode == MODE_REC_WAITING_START_REPLY){
					m_opeMode = MODE_REC_OPENED;
				}
				else if(m_opeMode == MODE_REC_WAITING_END_REPLY){
					m_opeMode = MODE_REC_CLOSED;
				}
			}
			// "NG"
			else{
				// 恢复到原来的模式
				if(m_opeMode == MODE_REC_WAITING_START_REPLY){
					m_opeMode = MODE_REC_CLOSED;
					//要打开失败一个飞行记录，它被认为是，由于路径的初始设定是错误的，
					//显示⇒错误对话框出现故障后，本应做几次，节省处理保存程序
					//从断开（多次以便不发送保存消息的开始）。
					AfxMessageBox(_T("飞行日志不可保存。请给出指令，确认飞行Log的路径设置"));
					for (ClientInfoMap::iterator it = m_ClientInfo.begin(); it != m_ClientInfo.end(); ++it) {
						if (it->second.type == CLIENTTYPE_REC) {
							it->second.type = CLIENTTYPE_COMMON;
						}
					}
				}
				else if(m_opeMode == MODE_REC_WAITING_END_REPLY){
					m_opeMode = MODE_REC_CLOSED;	// 但基本文件关闭不失败，又回到了暂时的初始状态

				}
			}			
		}
	}

	// 从再现程序回复
	else{
		path.push_back(L"Play");
		path.push_back(L"Result");

		// 获取结果（结果）
		if (m_xmlDataDb->getOneElement(path, attr, msg_attr, val) == 0){
			// "OK"
			if(_tcscmp(val, L"OK") == 0){
				if(m_opeMode == MODE_PLAY_WAITING_OPEN_REPLY){
					m_opeMode = MODE_PLAY_OPENED;
				}
				else if(m_opeMode == MODE_PLAY_WAITING_CLOSE_REPLY){
					m_opeMode = MODE_REC_CLOSED;
				}
				else if(m_opeMode == MODE_PLAY_WAITING_EDIT_REPLY){
					m_opeMode = MODE_PLAY_OPENED;
				}
			}
			// "NG"
			else{
				if(m_opeMode == MODE_PLAY_WAITING_OPEN_REPLY){
					m_opeMode = MODE_REC_CLOSED;
				}
				else if(m_opeMode == MODE_PLAY_WAITING_CLOSE_REPLY){
					m_opeMode = MODE_REC_CLOSED;	// //但基本文件关闭不失败，又回到了暂时的初始状态

				}
				else if(m_opeMode == MODE_PLAY_WAITING_EDIT_REPLY){
					m_opeMode = MODE_PLAY_OPENED;
				}
			}			

			// 发送回复给客户的答复等待

			_wsub_SendReplyMsg(WAITING_PLAY_COMMAND_REPLY, replyMsg);

			// 清除主DB
			m_xmlDataDb->clearSetMsg(0, 0);
		}
	}
}

//*******************************************************************************
//	设置命令处理
//
// Lock:
//
//*******************************************************************************
void CXMonitorApp::_wsub_ProcConfigCmd(UINT clientID)
{
		
	ClientInfoMap::iterator ite = m_ClientInfo.find(clientID);
	if (ite == m_ClientInfo.end()) {	//未注册
        //错误（不应该在此处输入）

		return;
	}

	// 要回复等待

	m_ClientInfo[clientID].status = WAITING_CONFIG_COMMAND_REPLY;

}

//*******************************************************************************
//	飞行记录保存消息传输处理的开始
//
// Lock:
//
//*******************************************************************************
void CXMonitorApp::_wsub_SendFlightLogRecStart(UINT clientID)
{
	// 获得在数据库中注册的设置
	RetrieveSettingValFromDB();

	// 飞行日志大小分割
	TCHAR strDivSize[BASE64_BUFFER_SIZE];
	size_t outsize;
	Base64Data::encode(&m_AppSetting.GetAccessor()->FlightLog.DivideSizeInByte, Base64Data::I, strDivSize, &outsize);

	string strXml = XML_MSG_REC_START1;
	strXml += m_AppSetting.GetAccessor()->FlightLog.FilePath;
	strXml += XML_MSG_REC_START2;
	strXml += CW2A(strDivSize);
	strXml += XML_MSG_REC_START3;
	m_TcpServer.SendPacket(strXml, clientID);

	m_opeMode = MODE_REC_WAITING_START_REPLY;

	this->AppendLog(_T("FlightLog指令（保存）开始发送了"));
}

//*******************************************************************************
//	飞行日志存储完成信息传输处理
//
// Lock:
//
//*******************************************************************************
void CXMonitorApp::_wsub_SendFlightLogRecEnd(UINT clientID)
{
	string strXml = XML_MSG_REC_END;
	m_TcpServer.SendPacket(strXml, clientID);

	m_opeMode = MODE_REC_WAITING_END_REPLY;
	m_mainLinkNoDataCnt = 0;	//重置

	this->AppendLog(_T("FlightLog指令（保存）发送完成"));
}


//*******************************************************************************
//	确定是否要结束所述数据存储
//
// Lock:
//
//*******************************************************************************
void CXMonitorApp::_wsub_DecideRecStop()
{
	double stopDuration = (double)(m_mainLinkNoDataCnt * N_LOOPCYCLE) / 1000;	// //数据停止时间[s]



	if(stopDuration > m_AppSetting.GetAccessor()->FlightLog.RecStopTimeFromDataStop){

		for (ClientInfoMap::iterator it = m_ClientInfo.begin(); it != m_ClientInfo.end(); ++it) {
			// 根据客户注册信息类型,确定发送目的地
			if (it->second.type == CLIENTTYPE_REC) {
				//发送飞行日志存储完成消息
				_wsub_SendFlightLogRecEnd(it->first);
			}
		}

	}
}

//*******************************************************************************
//	发送回复短信
//
// Lock:
//
//*******************************************************************************
void CXMonitorApp::_wsub_SendReplyMsg(WaitingStatus status, const _bstr_t &replyMsg)
{
	for (ClientInfoMap::iterator it = m_ClientInfo.begin(); it != m_ClientInfo.end(); ++it) {
		// 根据客户端信息被注册了的“等待状态”
		if (it->second.status == status) {
			//发送回复信息
			const string strReplyMsg = CW2A(replyMsg);
			m_TcpServer.SendPacket(strReplyMsg, it->first);

			// 回到基于所述的等待状态//
			it->second.status = WAITING_NOTHING;

			CString str;
			str.Format(_T("回复（or ACK）信息传输完成(client:%d)"), it->first);
			this->AppendLog(str);
		}
	}
}

//*******************************************************************************
//	发送ACK消息给多路点命令
//
// Lock:
//
//*******************************************************************************
void CXMonitorApp::_wsub_SendMultiwayAck(UavLinkManage::MultiwayAck *ack)
{
	TCHAR strBase64[BASE64_BUFFER_SIZE];
	size_t outsize;

	_bstr_t strXml = XML_MSG_MLTWAY_ACK1;

	// 航点ID
	double wptId = (double)ack->wptId;
	Base64Data::encode(&wptId, Base64Data::R, strBase64, &outsize);
	strXml += strBase64;

	strXml += XML_MSG_MLTWAY_ACK2;

	///纬度
	Base64Data::encode(&ack->lat, Base64Data::R, strBase64, &outsize);
	strXml += strBase64;

	strXml += XML_MSG_MLTWAY_ACK3;

	//经度
	Base64Data::encode(&ack->lon, Base64Data::R, strBase64, &outsize);
	strXml += strBase64;

	strXml += XML_MSG_MLTWAY_ACK4;

	// 高度
	Base64Data::encode(&ack->alt, Base64Data::R, strBase64, &outsize);
	strXml += strBase64;

	strXml += XML_MSG_MLTWAY_ACK5;

	// 方位角
	Base64Data::encode(&ack->yaw, Base64Data::R, strBase64, &outsize);
	strXml += strBase64;

	strXml += XML_MSG_MLTWAY_ACK6;

	// 飞行速度
	Base64Data::encode(&ack->spd, Base64Data::R, strBase64, &outsize);
	strXml += strBase64;

	strXml += XML_MSG_MLTWAY_ACK7;

	// 到达指定点半径
	Base64Data::encode(&ack->tol, Base64Data::R, strBase64, &outsize);
	strXml += strBase64;

	strXml += XML_MSG_MLTWAY_ACK8;

	// 等待时间
	Base64Data::encode(&ack->wait, Base64Data::R, strBase64, &outsize);
	strXml += strBase64;

	strXml += XML_MSG_MLTWAY_ACK9;

	// 功能ID
	double funcId = (double)ack->funcId;
	Base64Data::encode(&funcId, Base64Data::R, strBase64, &outsize);
	strXml += strBase64;

	strXml += XML_MSG_MLTWAY_ACK10;

	// 发送到正在等待多路点的客户端的ACK
	_wsub_SendReplyMsg(WAITING_MLTWPT_COMMAND_ACK, strXml);

	// 显示多路点的所述ACK
	CString str;
	str.Format(_T("MultiWay ACK: %d, %.6f, %.6f, %.1f, %.1f, %.1f, %.1f, %.1f, %d"), 
				ack->wptId, ack->lat, ack->lon, ack->alt, ack->yaw, ack->spd, ack->tol, ack->wait, ack->funcId);
	this->AppendLog(str);

}

//////////////////////////////////////////////////////////////////////////////////
//
//	这之后，进行调试
//

//*******************************************************************************
//	平均周期计算//
//
//*******************************************************************************
void CXMonitorApp::CalcAveCycle()
{
	DWORD nowTime = ::timeGetTime();
	DWORD duration = nowTime - m_prevTime;

	if (m_cntCycle > 0){
		m_aveCycle = (m_aveCycle * (m_cntCycle - 1) + duration) / m_cntCycle;

		// 如果时期的大大超标，计数
		if( (N_LOOPCYCLE+5) < duration ) ++m_cntOver[0];
		if( (N_LOOPCYCLE+10) < duration ) ++m_cntOver[1];
		if( (N_LOOPCYCLE+20) < duration ) ++m_cntOver[2];
		if( (N_LOOPCYCLE+30) < duration ) ++m_cntOver[3];
	}
	++m_cntCycle;
	m_prevTime = nowTime;
}

//*******************************************************************************
//	平均周期输出
//
//*******************************************************************************
void CXMonitorApp::OutputAveCycle(string fileName)
{
	ofstream fout(fileName.c_str(), ios::app);
	if (fout){
		SYSTEMTIME st;
		GetLocalTime(&st);

		fout << st.wYear << "/" << st.wMonth << "/" << st.wDay << " "
			<< st.wHour << ":" << st.wMinute << ":" << st.wSecond << "." << st.wMilliseconds
			<< "\t" << "暯嬒廃婜丗" << m_aveCycle << "[ms], ";
		fout << "慡" << m_cntCycle << "夞拞 "
			 << "+5ms丗" << m_cntOver[0] << "[夞], "
			 << "+10ms丗" << m_cntOver[1] << "[夞], "
			 << "+20ms丗" << m_cntOver[2] << "[夞], " 
			 << "+30ms丗" << m_cntOver[3] << "[夞]" << endl;
	}

	fout.close();
}
