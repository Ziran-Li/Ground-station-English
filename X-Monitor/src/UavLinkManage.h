#pragma once

#include <vector>
#include "SCI.h"

//#import "msxml3.dll" named_guids
#import "msxml6.dll" named_guids

class CUavLink;
class dataManage;

class UavLinkManage
{
	//// 定义

	// 计算RSSI的Health值
	enum{
		PSSI_PARAM_A		= -100,		// RSSI结果为零时的结果
		PSSI_PARAM_B		= -80,		// RSSI结果为15时的结果
		PSSI_PARAM_TIMEOUT	= 1000,		// 超时时间[ms]
	};

	// （以便从顶层发送命令）
	enum CmdBufId{
		CMD_BUF_SLEEP_REPLY	= 0,		// 答复 COMMAND_SLEEP 命令
		CMD_BUF_GO_HOME,				// 返航指令
		CMD_BUF_LAND,					// 着陆指令
		CMD_BUF_PAUSE,					// PAUSE指令
		CMD_BUF_RESUME,					// RESUME指令
		CMD_BUF_WAYPOINT,				// 路点命令
		CMD_BUF_ACTIONOINT_ACK,			// 行动点数据ACK
		CMD_BUF_MULTIWAYPOINT,			// 多路点命令
		CMD_BUF_REQ_TIME_REPLY,			// 答复COMMAND_REQUEST_TIME命令
		CMD_BUF_NUM						// 命令缓冲区的元素数目	
	};

	//// 结构体

	// ACK多路点命令
public:
	struct MultiwayAck{
		unsigned long wptId;			// 航点标识符
		double lat;						// 纬度
		double lon;						// 经度
		double alt;						// 高度
		double yaw;						// 航向
		double spd;						// 速度
		double tol;						// 到达指定点半径
		double wait;					// 等待时间
		unsigned long  funcId;			// 功能ID

		MultiwayAck():wptId(0),lat(0),lon(0),alt(0),yaw(0),spd(0),tol(0),wait(0),funcId(0){}
	};

private:
	// 多路点信息
	struct MultiWayInfo{
		unsigned long nextWptId;		// 发送路点标识符
		unsigned long prevSendTime;		// 命令发送最后一次[ms]
		unsigned long randVal;			// 收到的随机值
		BOOL isTransferring;			// 是否在运输途中
		std::vector<MultiwayAck> ack;	// 收到的ACK参数（通常的参数是零个或者一个）
		unsigned short retryInterval;	// 重新发动命令间隔[ms]
		unsigned int dbID;				// 包含命令参数的DBID
		MultiWayInfo():nextWptId(0),prevSendTime(0),randVal(0),isTransferring(FALSE),ack(),retryInterval(0),dbID(0){}
	};

	////成员变量
	CUavLink	*m_mainLink;			// 主要链接
	dataManage	*m_xmlDataDb;			// XML数据管理
	std::vector<_bstr_t> m_commonPath;	// 当在XML数据管理注册路径，路径成为公用的
	DWORD m_recvTime;					// 接受有效数据包用来检测RSSI的健康值[ms]
	MultiWayInfo m_multiwayInfo;		// 多路点信息

	// 发送函数执行预留 COMMAND_SLEEP
	bool m_flagSendReady;

public:
	CString m_dbgMsg;	// 用于调试


	//// 成员函数
public:
	UavLinkManage();
	~UavLinkManage();

	// 初始化
	BOOL Initialize(CUavLink *wl, dataManage *dm);

	// 连接MainLink
	BOOL Connect(SCIPORT comPortNo);

	// 连接检查
	BOOL IsConnected();

	// 传入数据解码及注册xml管理器
	int DecodeAndRegister();

	// 办理本地注册xml管理的时间
	void RegisterLocalTime();

	// MultiWayPointData注册 XML 数据管理
	void RegisterMultiWayPointData(bool flag, BSTR value);
	void RegisterData(MSXML2::IXMLDOMDocument2Ptr& pDoc);

	// 数据传输(数据的DBID及传输的内容)
	BOOL SendData(UINT dbID, const _bstr_t &sendMsg);

	// 多路点是否正在执行过程中
	BOOL IsMultiwayTransferring() const { return m_multiwayInfo.isTransferring; }

	// 检索已经接收到的内容“多路点ACK命令”
	BOOL PopMultiwayAckData(MultiwayAck &ack);

	// 多路点命令重试，以确定
	BOOL DecideMultiwayCmdRetry();

	BOOL Send();

private:
	// 解码
	void DecodeCommand();			// UAV/GCS命令解码
	void DecodeReply();				// COMMAND_PACKET解码
	void DecodeInputData();			// kong
	void DecodeInputData_6R();		// 控制系统中输入六个转子的控制输入数据
	void DecodeInputData_12R();		// 控制系统中输入12个转子的控制输入数据
	void DecodeInputData_Min();		// 控制的最小输入
	void DecodeImuPsData();			// IMU气压计数据
	void DecodeMagData();			// IMU地磁数据
	void DecodeGpsData();			// 原始GPS数据
	void DecodeGpsInfo();			// GPS附加信息
	void DecodeNavData();			// 导航数据
	void DecodeNavDataEx();			// 导航数据与姿态数据
	void DecodeRefData();			// 目标控制数据
	void DecodeWayPointData();		// 航点数据和命令
	void DecodeWayPointData2();		// 数据与航点和命令的数目
	void DecodeHomeData();			// 返航的数据
	void DecodeSystemData();		// 反应系统内部状态的数据
	void DecodeErrorData();			// UAV上发生的错误数据
	void DecodeActionPoint();		// 行动的执行点数据
	void DecodeActionPointAck();	// 采取行动执行数据 ACK 点 [s/w 基站没有收到]
	void DecodeMultiWayPointData();	// 多点传送数据 [s/w 基站没有收到]
	void DecodeMultiWayPointAck();	// 多路点将响应数据传输
	void DecodeFirmwareInfo();		// 通知固件信息包
	void DecodePropoData();			// 遥控数据
	void DecodeEepromData();		// EEPROM 写请求 /EEPROM [s/w 基站不接收] 数据采集
	void DecodeEepromCommand();		// EEPROM 命令 [s/w 基站没有收到]
	void DecodeDebugData_H();		// 上位机控制单片机值
	void DecodeDebugData_L();		// 下位机控制单片机值
	void DecodeGeneralPurposeData();// 通用数据

	// 获取一个字符串操作类型
	std::string GetActionString(_UBYTE type) const;

	// 发送数据
	BOOL SendWayPointData(UINT dbID);
	BOOL SendMultiWayPointData(UINT dbID, BOOL forcedFlush = FALSE);
	BOOL SendCmdRequestFirmwareinfo(UINT dbID);
	BOOL SendCmdRequestSendCmdRequestSerialNum(UINT dbID);
	BOOL SendCmdGoHome(UINT dbID);
	BOOL SendCmdLand(UINT dbID);
	BOOL SendCmdPause(UINT dbID);
	BOOL SendCmdResume(UINT dbID);
	BOOL SendActionPointAck(_UBYTE actionType, _UWORD count);

	BOOL SendCmdRequestSerialNumber(UINT dbID);
	void DecodeSerialNumber();

	BOOL SendCmdPlannedFlight(UINT dbID);
	BOOL SendError(UINT dbID);

	// 从无人机相应命令
	BOOL ProcCmdSleep();
	BOOL ProcCmdReqTime();

	// 要注册XML数据管理器
	BOOL RegisterRssi(_UBYTE rssi);

	// 实用程序
	std::vector<_bstr_t> GetXmlPath(_bstr_t path1, _bstr_t path2 = L"", _bstr_t path3 = L"");	// 生成的xml路径
};

