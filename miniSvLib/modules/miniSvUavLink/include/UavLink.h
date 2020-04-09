//  UAV����ͨ��
//
//
//		�й����ݰ��ĸ�ʽ
//			[      1      ] -            ͷ
//			[      2      ] -            ͷ
//			[      3      ] -            PacketID
//          [ ----------- ] - (DATA)     ��Ч����
//          [PACKET_SIZE-1] - CRC16       CRC-16
//          [PACKET_SIZE  ] - CRC16       CRC-16
//
//       �ӥå�����ǥ�����ʽ
//      
//       CRC���㷶Χ�����ݰ�ID�����ݲ���
//
//     ע��ԭ���ϣ����ݰ������ݣ������ٶ�Ϊ���д����ʹ�ò�ģ�
//     ��Ϊֻ����������Զ�����COMMAND_SLEEP����Ӧ���ġ�
//    ��ͨ���������ݰ�����������ΪӦ�����ݰ������ݱ��ı䡣��

#ifndef __CUAVLINK_H__
#define __CUAVLINK_H__

#include <map>
#include <vector>
#include "Common/WirelessDevice_FEP.h"
#include "Common/Globaldef.h"
#include "Common/Protocol_2GDef.h"

#define HEAD1    (unsigned char)0xA5			//��ͷ
#define UNHEAD1  (unsigned char)(~HEAD1 & 0xFF)	//ͷ�͸���ֵ
#define HEAD2    (unsigned char)0x5A			//����2
#define UNHEAD2  (unsigned char)(~HEAD2 & 0xFF)

#define CRC16_POLYNOMIAL  (unsigned short)0xBAAD      
#define CRC16_POLYNOMIAL_INV  (unsigned short)0xB55D     
#define CRC16_INITIAL     (unsigned short)0xFFFF     

typedef std::map<PACKETID, PACKETSIZE> RxPacketSizeMap;	// ID�������յķ���Ĵ�С


class CUavLink {

public:
	enum {
		TXBUFFSIZE					= 128,  //���ͻ������Ĵ�С[byte]
		RXBUFFSIZE					= 128,  //���ջ�������С[byte]
		DOWNLINK_STOP_TIME_DEFAULT	= 360,	//COMMAND_SLEEP�����ͣʱ��[ms]
		DOWNLINK_STOP_TIME_MAX		= 1000,	//�趨������Ϣʱ������ֵ[ms]
		DOWNLINK_STOP_TIME_DELTA	= 30,	//�������ӵ�ÿ����һ�����ݰ�����ʱ[ms]
	};

private:

	enum{
		LEN_CRC16 = 2,						//CRC16���ֽڳ���
		LEN_SYNC = 2,						//ͷ(����)���ֽڳ���
		LEN_HEADER = LEN_SYNC + 1,			//��ͷ����(header1 header2 packetID)
	};

	// ��־
	FILE* m_fptr; 
	BOOL m_flgLog;
	unsigned int m_fileNo;
	TCHAR m_dirPath[256];
	TCHAR m_filePath[256];

protected:
	//���ͻ���
	struct tagTx{
		int           nDataCount;		//�洢����������
		unsigned char Data[TXBUFFSIZE];	//������
	} m_Tx;

	//���ջ�����
	struct tagRx{
		int           nDataCount;		//�洢���ݵ�����
		unsigned char Data[RXBUFFSIZE];	//������

		int           nPacketSize;		//��ȡ���ݰ��ĳ���
		int           nDataRead;		//�ӻ������ж�ȡ���ֽ����� 
	} m_Rx;

	//�����յķ���ʹ�С��ID
	RxPacketSizeMap m_RxPacketSizeMap;

	// �����豸����
	Wireless_FEP* m_wireless;

	// �������
	struct tagCmdBuf{
		tagTx tx;				// ���ͻ�����
		BOOL continuousSend;	// TRUE:��������  FALSE��ֻ����һ��		
	};
	std::vector<tagCmdBuf> m_cmdBuf;
	
	// �������������COMMAND_SLEEP�Ļظ���(GCS(X-Monitor)ΪTrue, UAV(UAVSim)һ���趨ΪFALSE)
	BOOL m_hasCmdSleepReply;

	// ��������ڵ�COMMAND_SLEEP�ظ���ID
	_UWORD m_cmdSleepReplyId;

public:
	CUavLink();
	virtual ~CUavLink();

	//��ʼ��
	CRESULT Initialize(_UBYTE sciport, _UDWORD br = 0);
	
	//����ʱ��ɼ�
	_UWORD GetRoundTripTime(size_t TxBytes = -1, size_t RxBytes = -1) const {return m_wireless->GetRoundTripTime(TxBytes, RxBytes);}

	//********************************
	//         ���ݴ���
	//********************************
	//�ύ����                    
	CRESULT TxBegin(PACKETID PacketID);

	//���ݽ�����ʵ���ϲ�û��Ψһ���ۻ��������������                         
	CRESULT TxEnd(_UWORD cmdBufId);

	//������ݣ��������Եڶ���������������е����ݣ�
	template<typename T> CRESULT TxPush(T Data, size_t nByte = 0)
	{
		if (m_wireless == NULL || !IsPortEnable()) return C_FAIL;
		if (nByte <= 0) nByte = sizeof(T);
		if (m_Tx.nDataCount < LEN_HEADER) return C_FAIL; // TxBeginδ����
		if (nByte > sizeof(T)) return C_FAIL;            // �Ƿ�����
		if (m_Tx.nDataCount > TXBUFFSIZE - LEN_CRC16 - sizeof(T)) return C_FAIL;	//  sizeof(T)����ʤ���nByte??

		unsigned char* pDest = &(m_Tx.Data[m_Tx.nDataCount]);

		//Data �������ٶ�Ϊ��С���ֽ���
		unsigned char* pSrc = (unsigned char*)&Data + nByte;
		for (size_t i = 0; i < nByte; i++) {
			*(pDest++) = *(--pSrc);
		}

		m_Tx.nDataCount += nByte;

		return C_OK;
	}

	//��������
	CRESULT TxFlush();

	//����һ���������
	CRESULT MakeCmdBuf(std::vector<BOOL> &continuousSend, BOOL hasCmdSleepReply, _UWORD cmdSleepReplyId=0);

	//����������ָ������
	CRESULT ClearCmdBuf(_UWORD cmdBufId);

	//������е��������
	CRESULT ClearAllCmdBuf();

	//����Ƿ�������ָ�����������
	BOOL ExistsCmd(_UWORD cmdBufId) const {return m_cmdBuf.at(cmdBufId).tx.nDataCount > 0 ? TRUE : FALSE;}

	//*************************************
	//             ��������
	//*************************************
	//�����ո�ʽ����¼
	CRESULT RxRegisterFormat(PACKETID PacketID, PACKETSIZE PacketSize);

	//��ʼ����
	CRESULT RxBegin(PACKETID* PacketID);
	//CRESULT RxBeginTest(PACKETID* PacketID);

	//���ս���(����RSSIֵ)
	CRESULT RxEnd(_UBYTE *rssi);

	// ���յ����ݣ����ʡ�����������ֽڵĵڶ����������ձ�ţ�
	template<typename T> CRESULT RxPop(T* pData, size_t nByte = 0)
	{
		if (m_wireless == NULL || !IsPortEnable()) return C_FAIL;
		if (nByte <= 0) nByte = sizeof(T);
		if (sizeof(T) < nByte) return C_FAIL; // ��Ч�Ĳ���
		if (m_Rx.nDataRead > (int)(m_Rx.nDataCount - LEN_CRC16 - nByte)) return C_FAIL;

		unsigned char* pSrc = &(m_Rx.Data[m_Rx.nDataRead]);

		//PData �������ٶ�Ϊ��С���ֽ���
		unsigned char* pDest = (unsigned char*)pData + nByte;
		for (size_t i = 0; i < nByte; i++) {
			*(--pDest) = *(pSrc++);
		}

		m_Rx.nDataRead += static_cast<int>(nByte);

		return C_OK;
	}

	//��־�ĳ�ʼ��
	BOOL LogInitialize(BOOL flg, TCHAR* path);

	//����ͨ�����
	BOOL	IsPortEnable() const{ return (m_wireless ? m_wireless->IsPortEnable() : FALSE); } // ��ǰCOM�˿��Ƿ����
	_UBYTE	GetCurrentSciPort() { return (m_wireless ? m_wireless->GetCurrentSciPort() : 0); }  // ��ȡ��ǰCOM�˿ں�
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

	//�洢COMMAND_SLEEP�ظ��������
	void SetCmdSleepReply(_UWORD stopTime);

	//������ջ�����
	void ClearRxBuf();

	//CRC����  
	unsigned short  CRC16Value(unsigned short  b);
	unsigned short  CalculateBlockCRC16(const unsigned char* pSrc, unsigned char count);
	char *bin8(unsigned long bin);

	//��־
	//Ҫ�ָ��������־�ļ��Ĵ�С
	BOOL ChkFileSize();
	//�����־
	void WriteLog(char* str);  //�ַ���
	void WriteLogTime();  //ʱ��
	void WriteLog(int i, unsigned char data);  //����
	void WriteLog(unsigned char data);  //���յ�������
	void WriteLogRx();
};


#endif
