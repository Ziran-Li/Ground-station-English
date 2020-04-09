//  UAV串行通信
//
//
//		有关数据包的格式
//			[      1      ] -            头
//			[      2      ] -            头
//			[      3      ] -            PacketID
//          [ ----------- ] - (DATA)     有效数据
//          [PACKET_SIZE-1] - CRC16       CRC-16
//          [PACKET_SIZE  ] - CRC16       CRC-16
//
//       ビッグエンディアン方式
//      
//       CRC计算范围：数据包ID、数据部分
//
//     注）原则上，数据包的内容，它被假定为进行此类的使用侧的，
//     因为只有这个类中自动生成COMMAND_SLEEP的响应报文。
//    （通过发送数据包的数量，因为应答数据包的内容被改变。）

#ifndef __CUAVLINK_H__
#define __CUAVLINK_H__

#include <map>
#include <vector>
#include "Common/WirelessDevice_FEP.h"
#include "Common/Globaldef.h"
#include "Common/Protocol_2GDef.h"

#define HEAD1    (unsigned char)0xA5			//报头
#define UNHEAD1  (unsigned char)(~HEAD1 & 0xFF)	//头和更多值
#define HEAD2    (unsigned char)0x5A			//标题2
#define UNHEAD2  (unsigned char)(~HEAD2 & 0xFF)

#define CRC16_POLYNOMIAL  (unsigned short)0xBAAD      
#define CRC16_POLYNOMIAL_INV  (unsigned short)0xB55D     
#define CRC16_INITIAL     (unsigned short)0xFFFF     

typedef std::map<PACKETID, PACKETSIZE> RxPacketSizeMap;	// ID和所接收的分组的大小


class CUavLink {

public:
	enum {
		TXBUFFSIZE					= 128,  //发送缓冲区的大小[byte]
		RXBUFFSIZE					= 128,  //接收缓冲区大小[byte]
		DOWNLINK_STOP_TIME_DEFAULT	= 360,	//COMMAND_SLEEP后的暂停时间[ms]
		DOWNLINK_STOP_TIME_MAX		= 1000,	//设定可能休息时间的最大值[ms]
		DOWNLINK_STOP_TIME_DELTA	= 30,	//建议增加的每接收一个数据包的延时[ms]
	};

private:

	enum{
		LEN_CRC16 = 2,						//CRC16的字节长度
		LEN_SYNC = 2,						//头(序言)的字节长度
		LEN_HEADER = LEN_SYNC + 1,			//报头长度(header1 header2 packetID)
	};

	// 日志
	FILE* m_fptr; 
	BOOL m_flgLog;
	unsigned int m_fileNo;
	TCHAR m_dirPath[256];
	TCHAR m_filePath[256];

protected:
	//发送缓冲
	struct tagTx{
		int           nDataCount;		//存储的数据数量
		unsigned char Data[TXBUFFSIZE];	//缓冲区
	} m_Tx;

	//接收缓冲区
	struct tagRx{
		int           nDataCount;		//存储数据的数量
		unsigned char Data[RXBUFFSIZE];	//缓冲区

		int           nPacketSize;		//读取数据包的长度
		int           nDataRead;		//从缓冲区中读取的字节数量 
	} m_Rx;

	//所接收的分组和大小的ID
	RxPacketSizeMap m_RxPacketSizeMap;

	// 无限设备对象
	Wireless_FEP* m_wireless;

	// 命令缓冲区
	struct tagCmdBuf{
		tagTx tx;				// 发送缓冲区
		BOOL continuousSend;	// TRUE:连续发送  FALSE：只发送一次		
	};
	std::vector<tagCmdBuf> m_cmdBuf;
	
	// 在命令缓冲区中有COMMAND_SLEEP的回复吗？(GCS(X-Monitor)为True, UAV(UAVSim)一侧设定为FALSE)
	BOOL m_hasCmdSleepReply;

	// 命令缓冲区内的COMMAND_SLEEP回复的ID
	_UWORD m_cmdSleepReplyId;

public:
	CUavLink();
	virtual ~CUavLink();

	//初始化
	CRESULT Initialize(_UBYTE sciport, _UDWORD br = 0);
	
	//往返时间采集
	_UWORD GetRoundTripTime(size_t TxBytes = -1, size_t RxBytes = -1) const {return m_wireless->GetRoundTripTime(TxBytes, RxBytes);}

	//********************************
	//         数据传输
	//********************************
	//提交启动                    
	CRESULT TxBegin(PACKETID PacketID);

	//传递结束（实际上并没有唯一的累积发送命令缓冲区）                         
	CRESULT TxEnd(_UWORD cmdBufId);

	//添加数据（如果你忽略第二个参数，添加所有的数据）
	template<typename T> CRESULT TxPush(T Data, size_t nByte = 0)
	{
		if (m_wireless == NULL || !IsPortEnable()) return C_FAIL;
		if (nByte <= 0) nByte = sizeof(T);
		if (m_Tx.nDataCount < LEN_HEADER) return C_FAIL; // TxBegin未运行
		if (nByte > sizeof(T)) return C_FAIL;            // 非法参数
		if (m_Tx.nDataCount > TXBUFFSIZE - LEN_CRC16 - sizeof(T)) return C_FAIL;	//  sizeof(T)じゃなくてnByte??

		unsigned char* pDest = &(m_Tx.Data[m_Tx.nDataCount]);

		//Data 参数被假定为在小端字节序
		unsigned char* pSrc = (unsigned char*)&Data + nByte;
		for (size_t i = 0; i < nByte; i++) {
			*(pDest++) = *(--pSrc);
		}

		m_Tx.nDataCount += nByte;

		return C_OK;
	}

	//发送数据
	CRESULT TxFlush();

	//创建一个命令缓冲区
	CRESULT MakeCmdBuf(std::vector<BOOL> &continuousSend, BOOL hasCmdSleepReply, _UWORD cmdSleepReplyId=0);

	//清除命令缓冲区指定命令
	CRESULT ClearCmdBuf(_UWORD cmdBufId);

	//清除所有的命令缓冲区
	CRESULT ClearAllCmdBuf();

	//检查是否包含命令到指定的命令缓冲区
	BOOL ExistsCmd(_UWORD cmdBufId) const {return m_cmdBuf.at(cmdBufId).tx.nDataCount > 0 ? TRUE : FALSE;}

	//*************************************
	//             接收数据
	//*************************************
	//按接收格式来登录
	CRESULT RxRegisterFormat(PACKETID PacketID, PACKETSIZE PacketSize);

	//开始接收
	CRESULT RxBegin(PACKETID* PacketID);
	//CRESULT RxBeginTest(PACKETID* PacketID);

	//接收结束(返回RSSI值)
	CRESULT RxEnd(_UBYTE *rssi);

	// 接收的数据（如果省略数据类型字节的第二个参数接收编号）
	template<typename T> CRESULT RxPop(T* pData, size_t nByte = 0)
	{
		if (m_wireless == NULL || !IsPortEnable()) return C_FAIL;
		if (nByte <= 0) nByte = sizeof(T);
		if (sizeof(T) < nByte) return C_FAIL; // 无效的参数
		if (m_Rx.nDataRead > (int)(m_Rx.nDataCount - LEN_CRC16 - nByte)) return C_FAIL;

		unsigned char* pSrc = &(m_Rx.Data[m_Rx.nDataRead]);

		//PData 参数被假定为在小端字节序。
		unsigned char* pDest = (unsigned char*)pData + nByte;
		for (size_t i = 0; i < nByte; i++) {
			*(--pDest) = *(pSrc++);
		}

		m_Rx.nDataRead += static_cast<int>(nByte);

		return C_OK;
	}

	//日志的初始化
	BOOL LogInitialize(BOOL flg, TCHAR* path);

	//串行通信相关
	BOOL	IsPortEnable() const{ return (m_wireless ? m_wireless->IsPortEnable() : FALSE); } // 当前COM端口是否可用
	_UBYTE	GetCurrentSciPort() { return (m_wireless ? m_wireless->GetCurrentSciPort() : 0); }  // 获取当前COM端口号
	//CRESULT	CloseSci() {return (m_wireless ?  m_wireless->CloseSci() : C_FAIL);}
	CRESULT	CloseSci()
	{
		if (m_wireless)
		{
			delete m_wireless;
			m_wireless = NULL;
		}
		return C_OK;
	}

protected:

	//存储COMMAND_SLEEP回复命令缓冲区
	void SetCmdSleepReply(_UWORD stopTime);

	//清除接收缓冲区
	void ClearRxBuf();

	//CRC计算  
	unsigned short  CRC16Value(unsigned short  b);
	unsigned short  CalculateBlockCRC16(const unsigned char* pSrc, unsigned char count);
	char *bin8(unsigned long bin);

	//日志
	//要分割输出的日志文件的大小
	BOOL ChkFileSize();
	//输出日志
	void WriteLog(char* str);  //字符串
	void WriteLogTime();  //时间
	void WriteLog(int i, unsigned char data);  //数据
	void WriteLog(unsigned char data);  //接收到的数据
	void WriteLogRx();
};


#endif
