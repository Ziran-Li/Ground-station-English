
// X-监视器。 h: 这是内容应用程序的主头文件。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH 偵懳偟偰偙偺僼傽僀儖傪僀儞僋儖乕僪偡傞慜偵 'stdafx.h' 傪僀儞僋儖乕僪偟偰偔偩偝偄"
#endif

#include "resource.h"		// 主要符号

// --- 由从这里程序员编程 ----
#define BYTEORDER 1 //Wirelesslink的字节顺序设置，以小尾数

#include <queue>

#include "Uavlink.h"
#include "ExclusiveObject.h"
#include "AppSetting.h"
#include "MyNetworkExtension.h" // 库与在其他的Winsock沟通过程
#include "DataManage.h"			//XML数据管理
#include "UavLinkManage.h"		// 无人机主要环节管理
#include "FlightLog.h"			// 飞行记录

// X-Monitor的操作模式
enum XMonitorOpeMode{
	MODE_REC_CLOSED = 0,			// 数据存储模式（不开飞行日志）
	MODE_REC_OPENED,				// 数据存储模式（开启飞行日志）
	MODE_REC_WAITING_START_REPLY,	// 数据存储模式（等待数据存储开始响应）
	MODE_REC_WAITING_END_REPLY,		// 数据存储模式（等待数据存储完成应答）
	MODE_PLAY_WAITING_OPEN_REPLY,	// 数据播放模式（等待飞行日志公开回应）
	MODE_PLAY_OPENED,				// 数据播放模式（开启飞行日志）
	MODE_PLAY_WAITING_CLOSE_REPLY,	// 数据播放模式（等待飞行日志接近响应）
	MODE_PLAY_WAITING_EDIT_REPLY,	// 数据播放模式（等待飞行日志编辑响应）
};

// 为定义为存储各种客户机的信息
enum ClientType{
	CLIENTTYPE_COMMON = 0,	// 一般客户
	CLIENTTYPE_GUI,			// GUI
	CLIENTTYPE_REC,			// 数据存储方案
	CLIENTTYPE_PLAY,		// 数据回放程序
};

enum WaitingStatus{
	WAITING_NOTHING = 0,					// 无需等待
	WAITING_MLTWPT_COMMAND_ACK,				// ACK等待//多路点命令
	WAITING_REC_COMMAND_REPLY,				// 回复等待// save命令
	WAITING_PLAY_COMMAND_REPLY,				// 回复等待//播放命令
	WAITING_CONFIG_COMMAND_REPLY,			//回复等待//设置命令
};

struct GetMsgInfo{
	int cycle;				// 数据的发送周期// [毫秒]
	BOOL hasUnsentData;		// //存在或不存在的未发送数据的

	GetMsgInfo() : cycle(100), hasUnsentData(FALSE) {}
};

typedef std::map<std::string, GetMsgInfo> GetMsgInfoMap;	

struct ClientInfo{
	ClientType type;				// 类型的客户端
	WaitingStatus status;			// （答辩，命令等）等待状态 
	GetMsgInfoMap getMsgInfo;		// Get信息//获取消息

	ClientInfo() : type(CLIENTTYPE_COMMON), status(WAITING_NOTHING), getMsgInfo() {}
};

typedef std::map<UINT, ClientInfo> ClientInfoMap;	


//对于这个类的实现，请参阅X-Monitor.cpp。
//

//***************************************************************************************************************************
//锁需要执行的独家控制对象的顺序
//法：
//什么发布Akusesuko类的外部被锁定到最早期，以避免对来自外部类的任何访问僵局。
//尽快锁定为存取时间长对象。
//对象细访问频繁出现的锁秩序放缓。
//
//规则执行：
//锁定在降序排列优先级。注意锁定功能之内不可见
//如果你的对象Akusesuko，锁定的功能：填写该列中的对象的代码。
//锁定：填写代码列的顺序是一个高优先顺序。
//如果通过代码的变化的最高阶的对象已经改变时，该锁以便检查​​函数的调用者必须确保被保持。
//优先顺序∞的对象// CODE是锁定：不能在列中输入
// --- Programmed by Programmer 偙偙傑偱 ----
//优先顺序 - 代码 - 对象名称：
//（最大：访问频率低）
//-∞ - [SendMessage函数]：消息目的地的窗口一直被认为是一个锁定[SendMessage函数。换句话说，Akusesuko SendMessage函数在收购中禁止原则！
// 99 - 设置 - m_AppSetting：应用程序配置信息获取经常被访问。
//∞ - - m_LogStringQueue：总是AppendLog，通过PopLog访问。它不能m_LogStringQueue的在同一线程锁的过程中发生的其他锁。
//（最低：访问频率高）
//
//确定僵局是由于//向图
//锁定为了在线程①函数：MAV→MLK→SET
//锁定在一个②线程B功能的顺序：MLK→MAV
//上述③，①用并绘制向图中的一个从②是如下的载体。
// MAV→MLK→SET
//↑│
//└──┘
//如果④在向图闭环存在，存在发生死锁的可能性。上面的例子导致死锁。
//原因是因为线程B是忽略了锁顺序。线程B必须被锁定在MAV→MLK的顺序。

//******************************************************************************************************************************
// CXMonitorApp:
// 偙偺僋儔僗偺幚憰偵偮偄偰偼丄X-Monitor.cpp 傪嶲徠偟偰偔偩偝偄丅
//




class CXMonitorApp : public CWinApp
{
public:
	CXMonitorApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 执行

	DECLARE_MESSAGE_MAP()

	// --- 由从这里程序员编程 ----
public:
	// 公共成员变量

private:
	// 私有成员变量
	CWinThread*       m_pWorkThread;

	// TTCP 服务器
	XMonitorServer m_TcpServer;

	// 各种类型的信息
	CString           m_InitialDirectory;      // 启动目录: 值设置为启动时并不会改变
	CString           m_CurrentMonitorIdName;  // 当前监视器的标识符: 值设置为启动时并不会改变	
	CExclusiveObject<AppSetting> m_AppSetting; // 应用程序设置: 从类的外部是通过接口函数快速访问。 所有线程访问

	//通信类
	CUavLink m_MainLink;    // 与飞机的通信

	//WM_APPEND_LOG 的字符串队列
	CExclusiveObject< std::queue<CString> > m_LogStringQueue;

	// XML 数据管理类
	dataManage *m_xmlDataDb;	// 想要删除前级可避免对指针成员的 COM 错误

    // XML 将数据提交到客户端的信息
	ClientInfoMap m_ClientInfo;  // 匹配的客户端信息和客户端 ID (每个连接 ID) 电子地图

	//无人机主要环节管理类
	UavLinkManage m_MainLinkManage;

	// X 监视器模式
	XMonitorOpeMode m_opeMode;

	// 计数器增量主要链接数据时不会来
	unsigned long m_mainLinkNoDataCnt;

	// 为调试 (时间测量)
	DWORD m_prevTime;	// 最后的时间 [毫秒]
	double m_aveCycle;	// 平均周期 [毫秒]
	unsigned long long m_cntCycle;	// 周期盘点
	unsigned long m_cntOver[4];		// 点过去一段时间 (ms + 5 和 + 10 ms + 20 ms + 30 ms)

private:
	// 私有成员函数
	static UINT CXMonitorApp_ThreadFunc(LPVOID pParam);

	// 在主窗口中发送消息
	LRESULT SendMessageToMainWnd_Safe(UINT message, WPARAM wParam = 0, LPARAM lParam = 0); // 僨僢僪儘僢僋偺婋尟偑偁傞偨傔巊梡偵拲堄偡傞丏
	LRESULT PostMessageToMainWnd_Safe(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	
	// 从登记数据库和数据库检索
	void RegisterSettingValToDB();
	void RetrieveSettingValFromDB();

public:
	// 公共功能
	void           OnMainWndInit(CWnd* pMainWnd);   // 在调用完成主窗口初始化
	void           BeginWorkThread();
	BOOL           IsOperationSafe();				// 请返回或更改设置的 X 监视器安全，(不在飞行中)
	const CString& GetInitialDirectory() { return m_InitialDirectory; }

	// 主窗口的间接访问
	BOOL AppendLog(LPCTSTR str);
	BOOL PopLog(CString& str);

	// 通过从 Windows 函数内的主窗口接收到的消息
	void NotifySystemResume();

	// App 应用程序设置
	const CString& GetMonitorIdName();
	BOOL           SetMonitorIdName(CString& MonitorIdName);
	BOOL           TestMonitorIdName(CString& MonitorIdName);
	const CString& GetCurrentMonitorIdName();

private:

	// 工作线程在子例程中
	void _wsub_OpenMainLink();
	void _wsub_SendDataToUAV(UINT clientID, const _bstr_t &mainlinkSendMsg);
	void _wsub_SendReadyDataToUAV();
	void _wsub_ReceiveData();
	void _wsub_DecideMultiwayCmdRetry();

	void _wsub_SetupXMonitorServer();
	void _wsub_SendDataToXMonitorClient(UINT nLoopCount);
	void _wsub_ReceiveDataFromXMonitorClient(UINT nLoopCount);
	void _wsub_XMonitorClientAccept(DWORD ipv4dw, UINT clientID);
	void _wsub_XMonitorClientClose(DWORD ipv4dw, UINT clientID);
	void _wsub_ProcClientTypeMsg(UINT clientID);
	void _wsub_ProcFlightLogCmd(const flightLog &log, UINT clientID);
	void _wsub_ProcFlightLogReply(UINT clientID, const _bstr_t &replyMsg);
	void _wsub_ProcConfigCmd(UINT clientID);
	void _wsub_SendFlightLogRecStart(UINT clientID);
	void _wsub_SendFlightLogRecEnd(UINT clientID);
	void _wsub_SendReplyMsg(WaitingStatus status, const _bstr_t &replyMsg);
	void _wsub_SendMultiwayAck(UavLinkManage::MultiwayAck *ack);

	void _wsub_DecideRecStop();

	// 僨僶僢僌梡
	void CalcAveCycle();
	void OutputAveCycle(std::string fileName);

	// --- Programmed by Programmer 偙偙傑偱 ----
};


extern CXMonitorApp theApp;