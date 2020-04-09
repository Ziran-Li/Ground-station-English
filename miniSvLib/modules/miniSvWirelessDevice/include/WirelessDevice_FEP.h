///
/// @file       WirelessDevice_FEP.h
///
/// @brief       Futaba製FEPシリーズ
///
/// @author     iwakura
///

// ◆符号的含义◆
// ***        字符串的三个字符代表对方的地址
// ###        表示的接收或发送的字节数字符串的三个字符
// <CR>, '\r' 回车
// <RF>, '\n' 换行
//
// ◆发送操作◆
// ・FEP的粘点
//进入发送触发后，输入数据<CR> <LF>有一个时区没有。 （在标题无分组传输模式的情况下）
// 时区不在发送命令后输入数据。 (如果数据包发送模式)
// 不能发送 55 ms 无线载波侦听由法律决定后传出的数据包，时区存在。 (任何模式)
// ・应该考虑的 FEP 规范
// 1数据包限制为 128 个字节。
// ・其他
// 因为即使在连续数据包含未被识别为一个命令的命令的任意组合、'@'必需的，除非逃出发送到FEP的数据不被破坏为
//(公认的命令-> 时间 + 空间"@VER \r\n")
// (得不到认可的命令-> 数据 + 一些"@VER \r\n")
//
// □怎么办?
// 每 60 毫秒到发送系统可用。
//发送系统是从 128 字节可用以接受数据。 SendStart() 返回错误并试图将它设置超过一次。
// Flush() 在 @TBN 命令在同一时间的时间在区域中的临时数据发送 FEP。
//
// 发送窗体：
// @TBN***###[二进制数据]<CR><LF>
//
// ◆接收操作◆
// 数据是从 FEP 以下 4 模式输出。
// 从RBN开始，提取二进制部分。
// 要限制 RBN 和后来检查仅接收数据从对的三个字符的地址数据。
//
// 接收模式 A：
//   数据结构    ：RBN***###ABCDEFG[電強]<CR><LF>
//   输出：ABCDEFG
//   条件1         ：在发送的数据包发送模式中使用 @TBN 命令，发送数据
//                   接收器设置要添加到的强度
//   条件2         ：在头模式将发送超时
//
// 接收模式B：
//   数据结构   ：RBN***###ABCDEFG<CR><LF>
//   输出：ABCDEFG
//   条件1         ：在发送的数据包发送模式中使用 @TBN 命令，发送数据
//                   接收方尚未配置增加的电场强度
//   条件2         ：在头模式将发送超时
//
// 接收模式C：
//   数据结构    ：RBN***###ABCDEFG<CR><LF>[電強]<CR><LF> 
//   输出：ABCDEFG<CR><LF> 
//   条件          ：发送发件人在头模式通过追加 < LF > < CR >
//                   接收侧是设置另外的电场强度
//   注意          ：顶它输出垃圾 < CR > < LF >。 没有办法，因为它不能区分一个分隔符或数据应在接收端的输出。
//                   避免了这种模式看到数据包发送模式发送的 FEP，并将数据发送到 @TBN 命令。
//
// 接收模式D：
//   数据结构    ：RBN***###ABCDEFG<CR><LF><CR><LF> 
//  输出：ABCDEFG<CR><LF>
//   条件          ：发送发件人在头模式通过追加 < LF > < CR >
//                   接收方尚未配置增加的电场强度
//   備考          ：接收模式 C 相同
//
//♦ 其他 ♦
//如果你没有连接 FEP(这时有人没有订明的次数响应 @ARG 命令)，直接连接作为主动沟通，
//

#ifndef INCLUDED_WIRELESSDEVICE_FEP_H
#define INCLUDED_WIRELESSDEVICE_FEP_H

#include "WirelessDevice.h"
#include "Lib/Memory.h"

class ISci;

class Wireless_FEP : public WirelessDevice
{
	struct Wireless_FEP_Initializer // 内存减少
	{
		enum
		{
			N_REGISTER = 29,
		};

		_UBYTE  InitSequaence;     //!< 初始化进程的进展情况
		_UBYTE  RegisterReadCount; //!< 加载的寄存器的数
		_UBYTE  Reg[N_REGISTER];   //!< 寄存器的值
		_UBYTE  Workspace[8];      //!< 工作空间数据代码字符串
		_UBYTE  TimeoutCounter;    //!< 超时检测
		_UBYTE  RetryCounter;      //!< 检测出重试
	};

	ISci* m_pSci;                       //!<串行通信接口类

	_UBYTE m_Flags;                     //!< 各种标志

	Wireless_FEP_Initializer m_Initializer;

	Memory::Array<_UBYTE> m_TempBuffer; //!<批处理与虚拟缓冲区的命令
	_UBYTE m_TempBufferCount;           //!< 数据设置为虚拟缓冲区数
	
	_UWORD m_TimeNextSend;              //!< 下一个发送周期

	_UBYTE  m_RepeaterType;             //!< 存储转发器传输类型 REPEATER_TYPE 枚举值

	_UBYTE  m_ReceiveMode;              //!< 接收模式 RECEIVE_MODE 枚举值存储
	_UBYTE  m_ReceiveStateCount;        //!< 接收计数在各接收模式的数
	_UBYTE  m_ReceiveLength;            //!< 接收到的二进制数据的长度（在###部分的值）
	bool    m_IsReceived;               //!< 二进制数据接收状态
	_UBYTE  m_ReceiveData;              //!< 接收二进制数据
	_UBYTE  m_RssiDataTemp;             //!< RSSI被解码后的数据
	_UDWORD m_ReceiveStringChecker;     //!< 执行高速字符串比较DWORD值

	_UBYTE  m_DestAddr[3];              //!< 同行地址 (三个数字)
	_UBYTE  m_Repeater1Addr[3];         //!< 中继器 1 地址 (三个数字)
	_UBYTE  m_Repeater2Addr[3];         //!< 中继器 2 地址 (三个数字)

	_UBYTE  m_Rssi;                     //!< 场强的数据进行解码。 0是不存在的数据

public:

	///
	/// @brief 的构造函数
	///
	Wireless_FEP(_UBYTE PortNo);

	///
	/// @brief   析构函数
	///
	virtual ~Wireless_FEP();

	///
	/// @brief 对象初始化
	///
	virtual CRESULT Initialize(_UDWORD Baudrate = 0);

	///
	/// 更新 @brief 对象的内部状态
	///
	virtual void Update();

	///
	/// @brief  可用对象的确认
	///
	virtual bool IsReady();

	virtual bool IsTxBusy();

	///
	/// @brief  连续数据传输开始
	///         128 字节临时不能放入缓冲区，它返回 C_OUTOFBUFFER
	///         无法发送载波侦听，如果 C_BUSY
	///
	virtual CRESULT SendBegin(size_t TotalLength);

	///
	/// @brief  1 字节发送
	///
	virtual CRESULT Send(_UBYTE data);

	///
	/// @brief  发送多个字节
	///
	virtual CRESULT Send(const void* pData, size_t length);

	///
	/// @brief  完成的连续数据传输
	///
	virtual CRESULT SendEnd();

	///
	/// @brief   转移触发器输出
	///
	virtual CRESULT Flush();
	
	///
	/// @brief 1 字节数据采集
	///        函数对象是暂时忙着发如果运行后通过中继器，接收数据
	////	   如果在无头模式 < CR > 发送 < LF > 触发器作为数据发送
	////   	   在结束了接收到的数据列可能是垃圾 < CR > < LF >
	///
	virtual CRESULT Receive(_UBYTE* pData);
	
	///
	/// @brief  找出可用的1字节数据
	///         函数对象是暂时忙着发如果运行后通过中继器，接收数据
	///
	virtual bool    IsDataReceived();
	
	///
	/// @brief  数据不是作为 @brief 数据包数据加工处理
	///
	virtual void ProcessUnusedCharacter(_UBYTE c);
	
	///
	/// @brief  获取 @brief 电场强度数据
	////如果你没有收到一个，此函数将失败，并返回 C_NODATA，
	///
	virtual CRESULT GetRssi(_UBYTE* pRssi) const;

	///
	/// @brief  获取与指定数目的字节数据包的 @brief 往返时间以毫秒为单位
	////从接收传输延迟是不包括在内
	////可能会发生变化取决于对无线信道返回中继器
	///
	virtual _UWORD GetRoundTripTime(size_t TxBytes = (size_t)-1, size_t RxBytes = (size_t)-1) const;


	bool IsPortEnable() const;
	int  GetCurrentSciPort() const;


private:

	// 初始化过程 (寄存器加载，重置)
	void _InitSequence();

	// 检测从接收到的数据串的报头，并提取输入到发送侧处理的数据串
	void _Rx();

	// 发送到 FEP 操作的命令的响应
	void _RxCommandResult();

	// 忙状态
	void _SetBusy(_UWORD msecBusyTime);

	// 取消忙状态
	void _ResetBusy();

	// 计算提交的繁忙时间
	static _UWORD _GetBusyTime(enum REPEATER_TYPE type, _UWORD length);
};



#endif