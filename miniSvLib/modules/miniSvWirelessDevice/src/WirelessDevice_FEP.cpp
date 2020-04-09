#include "WirelessDevice_FEP.h"
#include "Common/SystemTime.h"
#include "Common/ConnectionCreator.h"
#include <string.h>

//!< 指令重试次数
#define N_COMMAND_RETRY 5


// 指令单位的串行通讯时间
#define COMMANDCOMMUNICATIONTIME_NOREPEAT  (12.0f * 1000.0f * 10.0f / DEFAULT_BAUDRATE) //!< 中继器未使用时的命令部分的串行通信时间
#define COMMANDCOMMUNICATIONTIME_ONEREPEAT (15.0f * 1000.0f * 10.0f / DEFAULT_BAUDRATE) //!< 1级中继器使用时的命令部分的串行通信时间
#define COMMANDCOMMUNICATIONTIME_TWOREPEAT (18.0f * 1000.0f * 10.0f / DEFAULT_BAUDRATE) //!< 2级中继器使用时的命令部分的串行通信时间

//!< 每字节+串口通信的实时无线通信时间
#define COMMUNICATIONTIMEPERBYTE (0.16f + 1000.0f * 10.0f / DEFAULT_BAUDRATE)   

// 固定通信处理时间
#define CONSTANTCOMMUNICATIONTIME_NOREPEAT  (33.0f + 2.0f + 21.0f + 2.0f) //!< 转发器不使用时的固定通信处理时间
#define CONSTANTCOMMUNICATIONTIME_ONEREPEAT (2.0f + 100.0f + 12.0f + 21.0f + 2.0f) //!< 1级转发器使用时的固定通信处理时间
#define CONSTANTCOMMUNICATIONTIME_TWOREPEAT (2.0f + 130.0f + 18.0f + 21.0f + 2.0f) //!< 2级转发器使用时的固定通信处理时间
//#define CONSTANTCOMMUNICATIONTIME 33.0f

//↑的数据需要重新审核！

//!< 停机+命令部分串行通信最短时间
#define MINIMUMCOMMUNICATIONTIME (51.0f + 12 * 1000.0f * 10.0f / DEFAULT_BAUDRATE)

//!< 无线分组接收后，该时间长度内的ACK被发送（在此期间发送忙）
#define BUSYTIME_AFTER_RECEIVE_ONEREPEAT   80 //!< 在第一阶段期间中继器使用时，所发生的传输繁忙期间接收后的长度[毫秒]
#define BUSYTIME_AFTER_RECEIVE_TWOREPEAT  100 //!< 在第二阶段期间中继器使用时，所发生的传输繁忙期间接收后的长度[毫秒]


enum
{
	DEFAULT_BAUDRATE     = 115200,
	WIRELESSPACKET_LIMIT = 128, //!< 无线数据包发送数据的最大值
};

enum FLAGS
{
	FLAG_READY   = 1, //!< 设备准备OK
	FLAG_TX_BUSY = 2, //!< 提交忙碌状态
};

enum
{
	INITSEQ_START,     //!< 初始阶段
	INITSEQ_REG_CHECK, //!< 注册检查
	INITSEQ_REG_SET,   //!< 注册重新设置
	INITSEQ_END,       //!< 完成
};

enum REPEATER_TYPE
{
	NOREPEAT  = 0, //!< 不重复
	ONEREPEAT = 1, //!< 1次重复
	TWOREPEAT = 2, //!< 2次重复
};

enum RECEIVE_MODE
{
	RXMODE_SLEEP,
	RXMODE_RECEIVE_WAITING,          //!< 等待接收第一个字母的状态'R'
	RXMODE_HEADER_DECODING,          //!< RBN接收中
	RXMODE_HEADER_DECODING2,         //!< TBN接收中
	RXMODE_ADDR_DECODING,            //!< 地址部分（***）接收中
	RXMODE_ADDR_DECODING2,           //!< 地址部分（TBN后自己的地址）接收中
	RXMODE_REPEATER1_ADDR_DECODING,  //!< 中继器的地址第1部分（$$$）接待※“RBR”期间跟随时
	RXMODE_REPEATER1_ADDR_DECODING2, //!< 中继器的地址第1部分（$$$）接待※“RB2”期间跟随时
	RXMODE_REPEATER2_ADDR_DECODING,  //!< 中继器2的地址部分(???)接收中
	RXMODE_DATALENGTH_DECODING,      //!< 数据长度(###)接收中
	RXMODE_PAYLOAD_DECODING,         //!< 二进制数据部分接收中
	RXMODE_FOOTER_DECODING,          //!< 最后的部分接收中
	RXMODE_RSSIDATA_DECODING,        //!< [电强度]接收中
	RXMODE_COMMAND_ARG,
	RXMODE_COMMAND_ARG_RESPONCE_HEADER_DECODING,  //!< @ARG命令等待数据响应
	RXMODE_COMMAND_ARG_RESPONCE_REGDATA_DECODING,
};


// インライン宣言をヘッダでしないためのせこいマクロ
#define _IsRxOk() (m_Flags & FLAG_READY)
#define _IsTxOk() ((m_Flags & FLAG_READY) && !(m_Flags & FLAG_TX_BUSY))


// 两个字符转换十六进制数功能
static bool _xtoi(const _UBYTE c[2], _UBYTE* pReturn)
{
	*pReturn = 0;

	if (c[0] - '0' <= 9)
	{
		*pReturn += (c[0] - '0') * 16;
	}
	else if (c[0] - 'A' <= 5)
	{
		*pReturn += (c[0] - 'A' + 10) * 16;
	}
	else
	{
		return false;
	}

	if (c[1] - '0' <= 9)
	{
		*pReturn += (c[1] - '0');
	}
	else if (c[1] - 'A' <= 5)
	{
		*pReturn += (c[1] - 'A' + 10);
	}
	else
	{
		return false;
	}

	return true;
}

// 将数字转换为高速的三个字符的字符串函数
static void _atoi128(unsigned char n, unsigned char ptr[3])
{
	// 如果x的范围是0〜128，Y = INT（103×x）/ 1024相当于Y = X/ 10的结果
	unsigned char m;

	m = (103 * (unsigned short)n) >> 10;
	ptr[2] = n - m * 10 + '0';
	n = (103 * (unsigned short)m) >> 10;
	ptr[1] = m - n * 10 + '0';
	m = (103 * (unsigned short)n) >> 10;
	ptr[0] = n - m * 10 + '0';
}

static void _atoi256(unsigned char n, unsigned char ptr[3])
{
	// 如果x的范围是0〜256，Y = INT（51 *（X +1））/512等于y的结果= X/ 10
	unsigned char m;

	m = (51 * (unsigned short)(n + 1)) >> 9;
	ptr[2] = n - m * 10 + '0';
	n = (51 * (unsigned short)(m + 1)) >> 9;
	ptr[1] = m - n * 10 + '0';
	m = (51 * (unsigned short)(n + 1)) >> 9;
	ptr[0] = n - m * 10 + '0';
}

Wireless_FEP::Wireless_FEP(_UBYTE PortNo) : WirelessDevice(PortNo), m_TempBuffer(WIRELESSPACKET_LIMIT)
{
	m_pSci = ConnectionCreator::Create(PSci(PortNo)); // 实例化安全

	m_Flags = 0;

	memset(&m_Initializer, 0, sizeof(m_Initializer));
	m_Initializer.InitSequaence = INITSEQ_START;

	m_TimeNextSend = 0;

	m_TempBufferCount = 0;
	m_RepeaterType = NOREPEAT;
	m_ReceiveMode = RXMODE_SLEEP;
	m_ReceiveStateCount = 0;
	m_IsReceived = false;
	m_Rssi = 0;
}

Wireless_FEP::~Wireless_FEP() 
{
	ConnectionCreator::Release(m_pSci);
}

CRESULT Wireless_FEP::Initialize(_UDWORD Baudrate)
{
	if (Baudrate == 0) Baudrate = DEFAULT_BAUDRATE;

	CRESULT res = C_OK;

	if (ISOK(res)) res = m_pSci->SetBaudrate(Baudrate);
	if (ISOK(res)) res = m_pSci->SetParity(SCI_PARITY_NONE);
	if (ISOK(res)) res = m_pSci->SetStopBits(SCI_STOPBIT_ONE);
	if (ISOK(res)) res = m_pSci->Open();

	return res;
}

void Wireless_FEP::Update()
{
	// 如果未初始化
	//if (m_pSci->) return;

	// 如果设备的使用准备仍然进行处理，使提供的装置
	if ((m_Flags & FLAG_READY) == 0)
	{
		_InitSequence();
		return;
	}

	// 如果使用Ready设备到位
#if 1
	if ((_SWORD)(m_TimeNextSend - ::GetSystemTimeShort()) <= 0)
	{
		// 忙碌时间已过！

		// 接收状态结束
		_ResetBusy();
	}
#else
	_UWORD nowTime = ::GetSystemTimeShort();
	_UWORD diffTime;
	
	if (nowTime <= m_TimeNextSend)
	{
		diffTime = m_TimeNextSend - nowTime;

		if (diffTime <= 0x7FFF)
		{
			if (nowTime == m_TimeNextSend)
			{
				// 忙碌时间已过！

				// 接收状态结束
				_ResetBusy();
			}
		}
		else
		{
			// 0x10000单步执行
			// 忙碌时间已过！

			// 接收状态结束
			_ResetBusy();
		}
	}
	else
	{
		diffTime = nowTime - m_TimeNextSend;

		if (diffTime <= 0x7FFF)
		{
			// 忙碌时间已过！

			// 接收状态结束
			_ResetBusy();
		}
		else
		{
			// 0x10000单步执行
			// 接收状态未结束

		}
	}
#endif
}

bool Wireless_FEP::IsReady()
{
	return m_Flags & FLAG_READY;
}

bool Wireless_FEP::IsTxBusy()
{
	return !_IsTxOk();
}


CRESULT Wireless_FEP::SendBegin(size_t TotalLength)
{
	// 如果未初始化
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_BUSY;

	if (m_TempBufferCount + TotalLength > WIRELESSPACKET_LIMIT) return C_OUTOFBUFFER;

	return C_OK;
}

CRESULT Wireless_FEP::Send(_UBYTE data)
{
	// 如果未初始化
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_FAIL;

	if (m_TempBufferCount == WIRELESSPACKET_LIMIT) return C_OUTOFBUFFER;

	m_TempBuffer[m_TempBufferCount++] = data;

	return C_OK;
}

CRESULT Wireless_FEP::Send(const void* pData, size_t length)
{
	// 如果未初始化
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_FAIL;

	const _UBYTE* pSrc = reinterpret_cast<const _UBYTE*>(pData);
	const _UBYTE* pEnd = pSrc + length;
	_UBYTE* pDest = &m_TempBuffer[m_TempBufferCount];

	while (pSrc < pEnd)
	{
		*pDest++ = *pSrc++;
 	}

	m_TempBufferCount += length;

	return C_OK;
}

CRESULT Wireless_FEP::SendEnd()
{
	// 如果未初始化
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_FAIL;

	return C_OK;
}

CRESULT Wireless_FEP::Flush()
{
	// 如果未初始化
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_FAIL;

	if (m_TempBufferCount)
	{
		// 将字符串转换为已发送的字节
		_UBYTE len[3];
		_atoi128(m_TempBufferCount, len);

		//指令发送和转发地址设置
		switch (m_RepeaterType)
		{
		case NOREPEAT: // 无中继
			m_pSci->WriteString((const _UBYTE*)"@TBN", 4);
			
			break;

		case ONEREPEAT: // 1级无中继
			m_pSci->WriteString((const _UBYTE*)"@TBR", 4);
			m_pSci->WriteString(m_Repeater1Addr, 3);
			break;

		case TWOREPEAT: // 2级无中继
			m_pSci->WriteString((const _UBYTE*)"@TB2", 4);
			m_pSci->WriteString(m_Repeater1Addr, 3);
			m_pSci->WriteString(m_Repeater2Addr, 3);
			break;

		default:
			return C_FAIL;
			break;
		}

		// 目标设定
		m_pSci->WriteString(m_DestAddr, 3);

		// 串行通信数据输出
		m_pSci->WriteString(len, 3);
		m_pSci->WriteString(&m_TempBuffer[0], m_TempBufferCount);
		m_pSci->WriteString((const _UBYTE*)"\r\n", 2);

		// 设置发送忙
		_SetBusy(_GetBusyTime(static_cast<REPEATER_TYPE>(m_RepeaterType), m_TempBufferCount));
		
		m_TempBufferCount = 0; // 缓冲区明确
	}

	return C_OK;
}
	
CRESULT Wireless_FEP::Receive(_UBYTE* pData)
{
	// 如果未初始化
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsRxOk()) return C_FAIL;

	_Rx(); // 要更新m_ReceiveData和m_IsReceived的状态

	if (m_IsReceived)
	{
		*pData = m_ReceiveData;
		m_IsReceived = false;

		return C_OK;
	}
	else
	{
		return C_FAIL;
	}
}
	
bool Wireless_FEP::IsDataReceived()
{
	// 如果未初始化
	//if (!IsInitSci()) return false;

	if (!_IsRxOk()) return false;

	_Rx(); //要更新m_ReceiveData和m_IsReceived的状态

	return m_IsReceived;
}
	
void Wireless_FEP::ProcessUnusedCharacter(_UBYTE c)
{
}
	
CRESULT Wireless_FEP::GetRssi(_UBYTE* pRssi) const
{
	if (m_Rssi == 0)
	{
		return C_NODATA;
	}
	else
	{
		*pRssi = m_Rssi;
		return C_OK;
	}
}

_UWORD Wireless_FEP::GetRoundTripTime(size_t TxBytes, size_t RxBytes) const
{
	if (TxBytes > WIRELESSPACKET_LIMIT) TxBytes = WIRELESSPACKET_LIMIT;
	if (RxBytes > WIRELESSPACKET_LIMIT) RxBytes = WIRELESSPACKET_LIMIT;

	return _GetBusyTime(static_cast<REPEATER_TYPE>(m_RepeaterType), TxBytes)
	     + _GetBusyTime(static_cast<REPEATER_TYPE>(m_RepeaterType), RxBytes); // 假定一个中继器的存在或不存在是在前后相同
}

void Wireless_FEP::_InitSequence()
{
	// 没有准备好要使用

	_RxCommandResult();

	if (m_Initializer.InitSequaence == INITSEQ_START)
	{
		//----------------------------------
		// 初始登记所有得到的数据
		//----------------------------------
		if (m_ReceiveMode == RXMODE_SLEEP && m_Initializer.RegisterReadCount == 0)
		{
			// 不注册数据
			m_pSci->WriteString((const _UBYTE*)"@ARG\r\n", 6);
			m_ReceiveMode = RXMODE_COMMAND_ARG;
			m_Initializer.TimeoutCounter = 25;
		}
		else if (m_Initializer.RegisterReadCount == Wireless_FEP_Initializer::N_REGISTER)
		{
			// 注册数据采集
			//m_Initializer.InitSequaence = INITSEQ_REG_CHECK;
			m_Initializer.InitSequaence = INITSEQ_END;
			m_Initializer.RetryCounter = 0;

			_atoi256(m_Initializer.Reg[2],  m_DestAddr);      // 从 REG02 获取地址
			_atoi256(m_Initializer.Reg[27], m_Repeater1Addr); // 从 REG27 获取REG27地址
			_atoi256(m_Initializer.Reg[28], m_Repeater2Addr); // 从 REG27 获取REG28地址

			if (m_Initializer.Reg[27] == 0xFF)
			{
				// 没有中继器(REG27:0xFF)
				m_RepeaterType = NOREPEAT;
			}
			else if (m_Initializer.Reg[28] == 0xFF)
			{
				// 通过1级中继器(REG28:0xFF)
				m_RepeaterType = ONEREPEAT;
			}
			else
			{
				// 通过2级中继器
				m_RepeaterType = TWOREPEAT;
			}
		}

		if (m_Initializer.TimeoutCounter == 0)
		{
			if (m_Initializer.RetryCounter < N_COMMAND_RETRY)
			{
				// 混合-> 超时
				m_ReceiveMode = RXMODE_SLEEP;
				m_Initializer.RegisterReadCount = 0;
				m_Initializer.RetryCounter++;
			}
			else
			{
				// 重试出
				m_Initializer.InitSequaence = INITSEQ_END;
				m_Initializer.RetryCounter = 0;
				
				m_DestAddr[0] = '9'; // 可配置地址到目的地
				m_DestAddr[1] = '9';
				m_DestAddr[2] = '9';

				m_RepeaterType = NOREPEAT; // 没有中继器
			}
		}
		else
		{
			m_Initializer.TimeoutCounter--;
		}
	}

	if (m_Initializer.InitSequaence == INITSEQ_REG_CHECK)
	{
		//----------------------------------
		// (2) 初始注册数据检查
		//----------------------------------

		if (m_ReceiveMode == RXMODE_SLEEP)
		{
			if (m_Initializer.Reg[13] != 0x81)
			{
				m_pSci->WriteString((const _UBYTE*)"@REG13:81H\r\n", 12);

			}
		}

		//m_Initializer.RegisterVal[2]
	}

	if (m_Initializer.InitSequaence == INITSEQ_REG_SET)
	{
		//----------------------------------
		// (3) 初始注册重置
		//----------------------------------

	}

	if (m_Initializer.InitSequaence == INITSEQ_END)
	{
		//----------------------------------
		// (4) 初始化完成
		//----------------------------------

		// delete pInitializer;

		m_Flags |= FLAG_READY | FLAG_TX_BUSY;
		m_ReceiveMode = RXMODE_RECEIVE_WAITING;
		m_TimeNextSend = ::GetSystemTimeShort() + _GetBusyTime(static_cast<REPEATER_TYPE>(m_RepeaterType), 0);
	}
}

void Wireless_FEP::_Rx()
{
	_UBYTE c;

	if (m_IsReceived) return;

	// 重复协议侧的数据，直到读出。
	// 返回 false 如果不能读出
	while (ISOK(m_pSci->ReadElement(&c)))
	{
		// 1字节接收

		if (m_ReceiveMode == RXMODE_PAYLOAD_DECODING)
		{
			//----------------------------------
			//⑤在接收到的数据体接收...
			//----------------------------------
			m_ReceiveData = c;
			m_IsReceived = true;
			m_ReceiveLength--;

			if (m_ReceiveLength == 0)
			{
				m_ReceiveMode = RXMODE_FOOTER_DECODING;
			}

			// 成功接收数据协议
			return;
		}
		else if (m_ReceiveMode == RXMODE_RECEIVE_WAITING)
		{
			//----------------------------------
			// ①接收数据之前
			// 'R'     - RBN***###[二进制数据][電強]<CR><LF>       or
			//           RBR$$$***###[二进制数据][電強]<CR><LF>    or
			//           RB2$$$???***###[二进制数据][電強]<CR><LF> 收到第一个字节?
			// 'T'     - @TBN999###[二进制数据][電強]<CR><LF> 第 2 字节接收吗? (直接连接时)
			//  除上述以外- 视为垃圾
			//----------------------------------
			if (c == 'R') 
			{
				m_ReceiveMode = RXMODE_HEADER_DECODING;
				m_ReceiveStateCount = 0;
				m_ReceiveStringChecker = 0;
			}
			else if (c == 'T')
			{
				// 为与相应的代码的直接连接
				m_ReceiveMode = RXMODE_HEADER_DECODING2;
				m_ReceiveStateCount = 0;
				m_ReceiveStringChecker = 0;
			}
		}
		else if (m_ReceiveMode == RXMODE_HEADER_DECODING || m_ReceiveMode == RXMODE_HEADER_DECODING2)
		{
			//----------------------------------
			// ② 标题"RBN"的"BN"部分 or 
			//          "RBR"的"BR"部分 or
			//          "RB2"的"B2"部分 接收中...or "TBN"的"BN"部分
			//----------------------------------
			m_ReceiveStringChecker <<= 8;
			m_ReceiveStringChecker |= c;
			m_ReceiveStateCount++;

			if (m_ReceiveStateCount == 2)
			{
				if (m_ReceiveStringChecker == (('B' << 8) | 'N'))
				{
					if (m_ReceiveMode == RXMODE_HEADER_DECODING)
					{
						// "RBN"连续接收
						m_ReceiveMode = RXMODE_ADDR_DECODING; //→ ③へ
						m_ReceiveStateCount = 0;
						m_ReceiveStringChecker = 0;
					}
					else
					{
						// "TBN"连续接收
						m_ReceiveMode = RXMODE_ADDR_DECODING2; // → ③へ
						m_ReceiveStateCount = 0;
						m_ReceiveStringChecker = 0;
					}
				}
				else if (m_ReceiveStringChecker == (('B' << 8) | 'R'))
				{
					// "RBR"的连续接收（经由单级路由器）
					m_ReceiveMode = RXMODE_REPEATER1_ADDR_DECODING; //  → ②'へ
					m_ReceiveStateCount = 0;
					m_ReceiveStringChecker = 0;

					// FEP内的ACK正在传输忙碌
					_SetBusy(BUSYTIME_AFTER_RECEIVE_ONEREPEAT);
				}
				else if (m_ReceiveStringChecker == (('B' << 8) | '2'))
				{
					// “RB2”的连续接收（通过两阶段路由器）
					m_ReceiveMode = RXMODE_REPEATER1_ADDR_DECODING2; // → ②'へ
					m_ReceiveStateCount = 0;
					m_ReceiveStringChecker = 0;

					// FEP内的ACK正在传输忙碌
					_SetBusy(BUSYTIME_AFTER_RECEIVE_TWOREPEAT);
				}
				else
				{
					// "RBN","RBR","RB2"不是
					m_ReceiveMode = RXMODE_RECEIVE_WAITING;
				}
			}
		}
		else if (m_ReceiveMode == RXMODE_REPEATER1_ADDR_DECODING || m_ReceiveMode == RXMODE_REPEATER1_ADDR_DECODING2)
		{
			//----------------------------------
			// ②转发器1地址接收中 ...
			//※不执行对转发地址匹配确定
			//----------------------------------
			m_ReceiveStateCount++;

			if (m_ReceiveStateCount == 3)
			{
				if (m_ReceiveMode == RXMODE_REPEATER1_ADDR_DECODING)
				{
					// "RBR$$$"接收完整
					m_ReceiveMode = RXMODE_ADDR_DECODING; // → ③へ
				}
				else
				{
					// "RB2$$$"接收完整
					m_ReceiveMode = RXMODE_REPEATER2_ADDR_DECODING; // → ②''へ
				}

				m_ReceiveStateCount = 0;
			}
		}
		else if (m_ReceiveMode == RXMODE_REPEATER2_ADDR_DECODING)
		{
			//----------------------------------
			// ②转发器2地址接收中 ...
			//※不执行对转发地址匹配确定
			//----------------------------------
			m_ReceiveStateCount++;

			if (m_ReceiveStateCount == 3)
			{
				// "RB2$$$???"接收完整
				m_ReceiveMode = RXMODE_ADDR_DECODING;
				m_ReceiveStateCount = 0;
			}
		}
		else if (m_ReceiveMode == RXMODE_ADDR_DECODING || m_ReceiveMode == RXMODE_ADDR_DECODING2)
		{
			//----------------------------------
			//③源地址接收中
			//----------------------------------
			m_ReceiveStringChecker <<= 8;
			m_ReceiveStringChecker |= c;
			m_ReceiveStateCount++;

			if (m_ReceiveStateCount == 3)
			{
				if ((m_ReceiveMode == RXMODE_ADDR_DECODING && m_ReceiveStringChecker == ((m_DestAddr[0] << 16) | (m_DestAddr[1] << 8) | m_DestAddr[2])) ||
					m_ReceiveMode == RXMODE_ADDR_DECODING2)
				{
					//地址发送端设备的地址相匹配（为3个字节接收之后或TBN）
					m_ReceiveMode = RXMODE_DATALENGTH_DECODING;
					m_ReceiveStateCount = 0;
					m_ReceiveLength = 0;
				}
				else
				{
					// 地址不符
					m_ReceiveMode = RXMODE_RECEIVE_WAITING;
				}
			}
		}
		else if (m_ReceiveMode == RXMODE_DATALENGTH_DECODING)
		{
			//----------------------------------
			// 4) 字节数接收中
			//----------------------------------
			m_ReceiveLength *= 10;
			m_ReceiveLength += c - '0';
			m_ReceiveStateCount++;

			if (m_ReceiveStateCount == 3)
			{
				if (m_ReceiveLength != 0 && m_ReceiveLength <= 128)
				{
					// 字节数的数据代码完成
					m_ReceiveMode = RXMODE_PAYLOAD_DECODING;
					m_ReceiveStateCount = 0;
				}
				else
				{
					// 一个无效的值
					m_ReceiveMode = RXMODE_RECEIVE_WAITING;
				}
			}
		}
		else if (m_ReceiveMode == RXMODE_FOOTER_DECODING)
		{
			//----------------------------------
			// ⑥页脚接收中...
			// 下列任何情况
			// <CR><LF>
			// [電強]<CR><LF>
			//----------------------------------
			if (static_cast<_UBYTE>(c - '0') <= 1) // 第 1 个数字 '0' 或 '1' 但不会不来
			{
				// [電強]<CR><LF> 格局
				m_RssiDataTemp = c - '0';
				m_ReceiveMode = RXMODE_RSSIDATA_DECODING;
				m_ReceiveStateCount = 1;
			}
			else
			{
				// <CR><LF> 模式或异常
				m_ReceiveMode = RXMODE_RECEIVE_WAITING;
			}
		}
		else if (m_ReceiveMode == RXMODE_RSSIDATA_DECODING)
		{
			//----------------------------------
			// ⑦電強接收数据中…
			//----------------------------------
			if (static_cast<_UBYTE>(c - '0') <= 9) // '0' 乣 '9'
			{
				m_RssiDataTemp *= 10;
				m_RssiDataTemp += c - '0';
				m_ReceiveStateCount++;
			}
			else
			{
				//  例外
				m_ReceiveMode = RXMODE_RECEIVE_WAITING;
			}

			if (m_ReceiveStateCount == 3)
			{
				m_Rssi = m_RssiDataTemp;
				m_ReceiveMode = RXMODE_RECEIVE_WAITING;
			}
		}
	}

	// 如果没有读出
	return;
}


void Wireless_FEP::_RxCommandResult()
{
	_UBYTE c;

	while (ISOK(m_pSci->ReadElement(&c)))
	{
		if (m_ReceiveMode == RXMODE_COMMAND_ARG)
		{
			//----------------------------------
			//初始①-（1）立即设置初始值到下一个
			//----------------------------------
			m_ReceiveMode = RXMODE_COMMAND_ARG_RESPONCE_HEADER_DECODING;
			m_ReceiveStateCount = 0;
			m_ReceiveStringChecker = 0;
			m_Initializer.RegisterReadCount = 0;
		}


		if (m_ReceiveMode == RXMODE_COMMAND_ARG_RESPONCE_HEADER_DECODING)
		{
			//----------------------------------
			// 初始①-（2）@ARG命令的响应（REG##:$$H<CR><LF> ×29行的"REG"部分）正在接收...
			//----------------------------------
			m_ReceiveStringChecker <<= 8;
			m_ReceiveStringChecker |= c;

			if ((m_ReceiveStringChecker & 0xFFFFFF) == (('R' << 16) | ('E' << 8) | 'G'))
			{
				m_ReceiveMode = RXMODE_COMMAND_ARG_RESPONCE_REGDATA_DECODING;
				m_ReceiveStateCount = 0;
			}
		}
		else if (m_ReceiveMode == RXMODE_COMMAND_ARG_RESPONCE_REGDATA_DECODING)
		{
			//----------------------------------
			// 初始①-（3）@ARG命令的响应（REG##:$$H<CR><LF> ×29行的<CR><LF部分）正在接收...
			//----------------------------------
			m_Initializer.Workspace[m_ReceiveStateCount++] = c;

			if (m_ReceiveStateCount == 8)
			{
				_UBYTE RegNo = (m_Initializer.Workspace[0] - '0') * 10 + (m_Initializer.Workspace[1] - '0'); // 第4, 5个字符是寄存器编号
				_UBYTE Value;

				// 格式校验
				if (m_Initializer.Workspace[2] == ':' &&
					m_Initializer.Workspace[5] == 'H' &&
					m_Initializer.Workspace[6] == '\r' &&
					m_Initializer.Workspace[7] == '\n' &&
					RegNo == m_Initializer.RegisterReadCount &&
					_xtoi(&m_Initializer.Workspace[3], &Value)) // ←这里的寄存器值的记录。第7, 8个字符是寄存器编号
				{
					// 格式OK

					m_Initializer.Reg[m_Initializer.RegisterReadCount++] = Value;

					if (m_Initializer.RegisterReadCount >= Wireless_FEP_Initializer::N_REGISTER)
					{
						// 所有注册完成
						m_ReceiveMode = RXMODE_SLEEP;
					}
					else
					{
						// 去读取→下一个寄存器还没有已获取
						m_ReceiveMode = RXMODE_COMMAND_ARG_RESPONCE_HEADER_DECODING;
						m_ReceiveStringChecker = 0;
					}
				}
				else
				{
					// 格式NG→重做（@ARG命令重发）

					m_Initializer.RegisterReadCount = 0;
					m_ReceiveMode = RXMODE_SLEEP;
				}
			}
		}
	}
}

void Wireless_FEP::_SetBusy(_UWORD msecBusyTime)
{
	m_Flags |= FLAG_TX_BUSY;
	m_TimeNextSend = ::GetSystemTimeShort() + msecBusyTime;
}

void Wireless_FEP::_ResetBusy()
{
	m_Flags &= ~FLAG_TX_BUSY;
}

_UWORD Wireless_FEP::_GetBusyTime(enum REPEATER_TYPE type, _UWORD length)
{
	float time;
	
	if (type == NOREPEAT)
	{
		// 没有中继器

		time = CONSTANTCOMMUNICATIONTIME_NOREPEAT + COMMANDCOMMUNICATIONTIME_NOREPEAT + length * COMMUNICATIONTIMEPERBYTE;

		// time 不小于最小值
		if (time < MINIMUMCOMMUNICATIONTIME) time = MINIMUMCOMMUNICATIONTIME;
	}
	else if (type == ONEREPEAT)
	{
		//1级中继器

		time = CONSTANTCOMMUNICATIONTIME_ONEREPEAT + COMMANDCOMMUNICATIONTIME_ONEREPEAT + length * COMMUNICATIONTIMEPERBYTE
		     + BUSYTIME_AFTER_RECEIVE_ONEREPEAT;

		// time 不小于最小值
		if (time < MINIMUMCOMMUNICATIONTIME) time = MINIMUMCOMMUNICATIONTIME;
	}
	else if (type == TWOREPEAT)
	{
		// 2级中继器

		time = CONSTANTCOMMUNICATIONTIME_TWOREPEAT + COMMANDCOMMUNICATIONTIME_TWOREPEAT + length * COMMUNICATIONTIMEPERBYTE
			 + BUSYTIME_AFTER_RECEIVE_TWOREPEAT;

		// time 不小于最小值
		if (time < MINIMUMCOMMUNICATIONTIME) time = MINIMUMCOMMUNICATIONTIME;
	}

	return -static_cast<_SWORD>(-time); // 要将一个整数转换同时推进
}


bool Wireless_FEP::IsPortEnable() const 
{ 
	return m_pSci ? m_pSci->IsEnable() : false; 
}

int  Wireless_FEP::GetCurrentSciPort() const
{
	return m_pSci ? m_pSci->GetPortNumber() : -1;
}