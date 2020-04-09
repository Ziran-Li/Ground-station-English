#include "stdafx.h"
#include <tchar.h>
#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "UavLink.h"
#include "SystemTimeSetter_Win.h"


CUavLink::CUavLink()
{
	m_fptr = NULL;

	m_Tx.nDataCount = 0;

	m_Rx.nDataCount = 0;
	m_Rx.nPacketSize = 0;
	m_Rx.nDataRead = 0;

	m_RxPacketSizeMap.clear();
	m_wireless = NULL;
	m_cmdBuf.clear();
	m_hasCmdSleepReply = FALSE;
	m_cmdSleepReplyId = 0;

	m_flgLog = FALSE;	// û����־���
}

CUavLink::~CUavLink()
{
	if(m_wireless){
		delete m_wireless;
	}

	if (m_flgLog){
		fclose(m_fptr);
	}
}

//��־�ļ���ʼ��
BOOL CUavLink::LogInitialize(BOOL flg, TCHAR* path)
{
	m_flgLog = flg;

	if (m_flgLog){
		m_fileNo = 0;
		_tcscpy_s(m_dirPath, path);  //�ļ�������
		_tcscpy_s(m_filePath, m_dirPath);
		_tcscat_s(m_filePath, L"\\UavLinkLog");

		TCHAR fileNostr[16];
		swprintf_s(fileNostr, L"%04d.txt", m_fileNo);

		TCHAR fileName[256];
		_tcscpy_s(fileName, m_filePath);
		_tcscat_s(fileName, fileNostr);  //�ļ���
		_tfopen_s(&m_fptr, fileName, L"w");
		if (NULL == m_fptr){
			return FALSE;
		}
	}

	return TRUE;
}

//��ʼ��
CRESULT CUavLink::Initialize(_UBYTE sciport, _UDWORD br)
{
	CloseSci();

	m_wireless = new Wireless_FEP(sciport);
	CRESULT res = m_wireless->Initialize(br);
	//CRESULT res = InitializeSci(sciport, br);

	if (ISOK(res))
	{
		m_Tx.nDataCount = 0;
		m_RxPacketSizeMap.clear();
		m_cmdBuf.clear();
	}
	return res;
}

//�ύ����
CRESULT CUavLink::TxBegin(PACKETID PacketID)
{
	if (m_wireless == NULL || !IsPortEnable()) return C_FAIL;
	//if (m_Tx.nDataCount){
	//	TxEnd();
	//}

	m_Tx.Data[0] = HEAD1; 
	m_Tx.Data[1] = HEAD2; 
	m_Tx.Data[2] = (unsigned char)PacketID;
	m_Tx.nDataCount = LEN_HEADER;

	return C_OK;
}

//���ݽ���
CRESULT CUavLink::TxEnd(_UWORD cmdBufId)
{
	if (m_wireless == NULL || !IsPortEnable()) return C_FAIL;
	if (m_Tx.nDataCount == 0) return C_FAIL;
	if (m_cmdBuf.size() <= cmdBufId) return C_INVALIDRANGE;

	//CRC16(���㷶ΧHEADER2��Ч�غ�)
	const int idxHeader2 = LEN_SYNC - 1;
	unsigned char *buf = &(m_Tx.Data[idxHeader2]);  //HEADER2�ӵ�ַ
	unsigned short crc = CalculateBlockCRC16(buf, m_Tx.nDataCount - idxHeader2);

	//nDataCount��nDataCount+1��CRC16Ԫ��
	m_Tx.Data[m_Tx.nDataCount] = (unsigned char)((crc >> 8) & 0xFF);
	m_Tx.Data[m_Tx.nDataCount + 1] = (unsigned char)(crc & 0xFF);

	m_Tx.nDataCount += LEN_CRC16;

	//��־���
	if (m_flgLog){
		if (!ChkFileSize()) return C_FAIL;

		WriteLog("\n����\n");
		WriteLogTime();

		for (int i = 0; i < m_Tx.nDataCount; i++){
			WriteLog(i, m_Tx.Data[i]);
		}
	}

	// �洢���������
	m_cmdBuf.at(cmdBufId).tx = m_Tx;

	// ���÷��ͻ�����
	m_Tx.nDataCount = 0;	

	return C_OK;
}

//�����͵ķ�������
CRESULT CUavLink::TxFlush()
{
	if (m_wireless == NULL || !IsPortEnable()) return C_FAIL;

	// ���������Ŀ�������ͣ������Ҫ����COMMAND_SLEEP��Ӧ�����
	// (��GCS��)
	if(m_hasCmdSleepReply){
		int packetNum = 0;
		for(unsigned int i=0; i<m_cmdBuf.size(); ++i){
			if(m_cmdBuf[i].tx.nDataCount != 0) ++packetNum;
		}
		if(packetNum == 0){
			// ����Ӧ������ȡ��������ͣʱ��
			SetCmdSleepReply(0);	
		}
		else if(packetNum >= 2){
			// ����Ӧ���������ӳ�������ͣʱ��
			SetCmdSleepReply(min(DOWNLINK_STOP_TIME_MAX, DOWNLINK_STOP_TIME_DEFAULT+DOWNLINK_STOP_TIME_DELTA*packetNum));
		}
	}

	// �ͷŴ���æ
	SetSystemTime((_UDWORD)timeGetTime());	// �ڲ�����ʱ��
	m_wireless->Update();					

	for(unsigned int i=0; i<m_cmdBuf.size(); ++i){
		if(m_cmdBuf[i].tx.nDataCount != 0){
			if(ISOK(m_wireless->SendBegin(m_cmdBuf[i].tx.nDataCount))){
				// �������ݴ��ݵ�����װ��
				m_wireless->Send(m_cmdBuf[i].tx.Data, m_cmdBuf[i].tx.nDataCount);
				m_wireless->SendEnd();
				// ��������������ͣ����÷��ͻ�����
				if(!m_cmdBuf[i].continuousSend) m_cmdBuf[i].tx.nDataCount = 0;
			}
			else{
				break;	// �������ͷ���������豸�Ĵ��������������û��׼�������������豸

			}
		}
	}

	// �����豸���ݰ������봫�䴫��
	return m_wireless->Flush();
}

//����һ���������
CRESULT CUavLink::MakeCmdBuf(std::vector<BOOL> &continuousSend, BOOL hasCmdSleepReply, _UWORD cmdSleepReplyId)
{
	if(continuousSend.empty()) return C_FAIL;

	for (unsigned int i = 0; i < continuousSend.size(); ++i)
	{
		tagCmdBuf cmdBuf;
		cmdBuf.tx.nDataCount = 0;
		cmdBuf.continuousSend = continuousSend[i];
		m_cmdBuf.push_back(cmdBuf);
	}

	m_hasCmdSleepReply = hasCmdSleepReply;
	m_cmdSleepReplyId = cmdSleepReplyId;

	return C_OK;
}

//����������ָ������
CRESULT CUavLink::ClearCmdBuf(_UWORD cmdBufId)
{
	try{
		m_cmdBuf.at(cmdBufId).tx.nDataCount = 0;
	}
	catch(std::out_of_range oor){
		return C_INVALIDRANGE;
	}

	return C_OK;
}

//������е��������
CRESULT CUavLink::ClearAllCmdBuf()
{
	for (std::vector<tagCmdBuf>::iterator it = m_cmdBuf.begin(); it != m_cmdBuf.end(); ++it) {
		it->tx.nDataCount = 0;
	}
	
	return C_OK;
}

//�Ӵ���ʽע��
CRESULT CUavLink::RxRegisterFormat(PACKETID PacketID, PACKETSIZE PacketSize)
{
	// �����С��������ע�ᣨ�������Ԥ����
	if (PacketSize > RXBUFFSIZE) return C_FAIL;

	typedef RxPacketSizeMap::iterator ite;
	std::pair<ite, bool> result;

	// ��½
	result = m_RxPacketSizeMap.insert(std::make_pair(PacketID, PacketSize));

	if (result.second == true){
		return C_OK;
	}
	else{
		return C_FAIL; 	// �ظ��Ǽ�NG
	}
}

//���տ�ʼ
CRESULT CUavLink::RxBegin(PACKETID* pPacketID)
{
	if (m_wireless == NULL || !IsPortEnable()) return C_FAIL;

	_UBYTE dummy;
	if (m_Rx.nDataRead != RXBUFFSIZE) RxEnd(&dummy); //����������м��ȡ��ֹ��
	
	SetSystemTime((_UDWORD)timeGetTime());	// �ڲ�����ʱ��
	m_wireless->Update();					

	while (m_wireless->IsDataReceived())
	{
		unsigned char Cur;
		m_wireless->Receive(&Cur);	// 1���ַ���ȡ

		if (m_Rx.nDataCount == 0)
		{
			////ͷ����
			m_Rx.Data[0] = m_Rx.Data[1];
			m_Rx.Data[1] = Cur;

			if ((m_Rx.Data[0] == HEAD1) && (m_Rx.Data[1] == HEAD2))
			{
				m_Rx.nDataCount = LEN_SYNC;
			}
		}
		else if (m_Rx.nDataCount == LEN_SYNC)
		{
			////PacketID������
			m_Rx.Data[LEN_SYNC] = Cur;
			m_Rx.nDataCount = LEN_HEADER;

			//���ݰ�ID����
			RxPacketSizeMap::iterator ite = m_RxPacketSizeMap.find(static_cast<PACKETID>(Cur));
			if (ite != m_RxPacketSizeMap.end()) {  // ע��
				m_Rx.nPacketSize = ite->second;
			}
			else{
				//���ݰ�IDδ��½
				if (m_flgLog) {
					WriteLogRx();
					WriteLog("���ݰ�IDδ��½\n");
				}
				ClearRxBuf();
				continue;
			}
		}
		else if (LEN_HEADER <= m_Rx.nDataCount && m_Rx.nDataCount <= (m_Rx.nPacketSize - LEN_CRC16)) 
		{
			////��Ч�غɲ��ֺ�CRC16�ĵ�һ����
			m_Rx.Data[m_Rx.nDataCount] = Cur;
			++m_Rx.nDataCount;
		}
		else 
		{
			////CRC16�ڶ����ֽ���
			m_Rx.Data[m_Rx.nDataCount] = Cur;

			//CRCУ��
			const int idxHeader2 = LEN_SYNC - 1;
			unsigned char *buf = &(m_Rx.Data[idxHeader2]);  //��HEADER2��ַ
			unsigned short crc = CalculateBlockCRC16(buf, m_Rx.nPacketSize - idxHeader2 - LEN_CRC16);

			unsigned short crc_r = ((m_Rx.Data[m_Rx.nDataCount - 1] << 8) | m_Rx.Data[m_Rx.nDataCount]) & 0xFFFF;
			if (crc == crc_r){
				//CRCһ��
				++m_Rx.nDataCount;
				*pPacketID = static_cast<PACKETID>(m_Rx.Data[LEN_SYNC]);
				m_Rx.nDataRead = LEN_HEADER;

				if (m_flgLog) WriteLogRx();

				return C_OK;
			}
			else{
				//CRC��һ��
				if (m_flgLog) {
					WriteLogRx();
					WriteLog("CRC��һ��\n");
				}
				ClearRxBuf();
				continue;
			}
		}

	}

	return C_FAIL; // ����û���յ�
}

//�������
CRESULT CUavLink::RxEnd(_UBYTE *rssi)
{
	if (m_wireless == NULL || !IsPortEnable()) return C_FAIL;

	if (m_Rx.nDataRead == RXBUFFSIZE) C_FAIL; // RxBegin�ʤ��Όg�з�ֹ

	m_Rx.Data[0] = UNHEAD1;
	m_Rx.Data[1] = UNHEAD2;
	m_Rx.nDataCount = 0;
	m_Rx.nPacketSize = 0;
	m_Rx.nDataRead = RXBUFFSIZE;

	// �ų�ǿ�ȵĲɼ�
	if(m_wireless->GetRssi(rssi) != C_OK)
	{
		rssi = 0;
	}

	return C_OK;
}

//COMMAND_SLEEP���洢���������Ӧ��
void CUavLink::SetCmdSleepReply(_UWORD stopTime)
{
	TxBegin(PACKETID_REPLY);
	TxPush(_UWORD(COMMAND_SLEEP));
	TxPush(_UBYTE((stopTime>>0)&0xFF));		// ���ֽ�
	TxPush(_UBYTE((stopTime>>8)&0xFF));		// ���ֽ�
	TxPush(_UBYTE(0));
	TxPush(_UBYTE(0));
	TxEnd(m_cmdSleepReplyId);
}

//������ջ�����
void CUavLink::ClearRxBuf()
{
	m_Rx.Data[0] = UNHEAD1;
	m_Rx.Data[1] = UNHEAD2;
	m_Rx.nDataCount = 0;
	m_Rx.nPacketSize = 0;
	m_Rx.nDataRead = 0;
}

//CRC����
unsigned short  CUavLink::CRC16Value(unsigned short  b)
{
	unsigned short crc = b;

	for (int j = 8; j > 0; j--) {

		if (crc & 1) {
			crc = (crc >> 1) ^ CRC16_POLYNOMIAL;
		}
		else {
			crc >>= 1;
		}
	}

	return crc;
}

unsigned short  CUavLink::CalculateBlockCRC16(const unsigned char* pSrc, unsigned char count)
{
#ifdef USE_BOOST
	boost::crc_basic<16> crc_calc(CRC16_POLYNOMIAL_INV, CRC16_INITIAL, 0, true, true);
	crc_calc.process_bytes(pSrc, count);
	unsigned short crc = crc_calc.checksum();
#else	

	//������λ�������
	unsigned short crc = CRC16_INITIAL;
	while (count-- != 0) {
		crc = CRC16Value(crc ^ *pSrc++);
	}
#endif
	return crc;
}

///////////////////////////////////////////////////////////
// ����Ϊ��־����Ĵ���

//�ַ�ת���ɶ�������(8bit)
char* CUavLink::bin8(unsigned long bin)
{
	static char result[10];
	int i;

	result[8] = '\0';
	for (i = 0; i < 8; i++){
		if (bin % 2 == 0){
			result[(8 - 1) - i] = '0';
		}
		else{
			result[(8 - 1) - i] = '1';
		}
		bin = bin / 2;
	}
	return result;
}

//�ָ������־�ļ��Ĵ�С
BOOL CUavLink::ChkFileSize()
{
	fpos_t fsize = 0;
	fpos_t fsizeb = fseek(m_fptr, 0, SEEK_END);
	fgetpos(m_fptr, &fsize);

	if (fsize > 5000000){  //�������5Mbyte
		fclose(m_fptr);

		m_fileNo++;
		TCHAR fileNostr[16];
		swprintf_s(fileNostr, L"%04d.txt", m_fileNo);

		TCHAR fileName[256];
		_tcscpy_s(fileName, m_filePath);
		_tcscat_s(fileName, fileNostr);  //�ļ���
		_tfopen_s(&m_fptr, fileName, L"w");
		if (NULL == m_fptr){
			return FALSE;
		}
	}

	return TRUE;
}

//�����־
void CUavLink::WriteLog(char* str)
{
	fprintf(m_fptr, str);
}
void CUavLink::WriteLogTime()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char str[256];
	sprintf_s(str, "%02u:%02u:%02u.%03u\t", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);  //tab
	WriteLog(str);
}
void CUavLink::WriteLog(int i, unsigned char data)
{
	//char *tmp;
	char str[256];
	//tmp = bin8(data & 0xFF);
	sprintf_s(str, "[%d]:%X :", i, static_cast<unsigned int>(data));
	WriteLog(str);
	//WriteLog(tmp);
	WriteLog("\n");
}
void CUavLink::WriteLog(unsigned char data)
{
	char *tmp;
	char str[256];
	WriteLog("\n����");
	tmp = bin8(data & 0xFF);
	sprintf_s(str, "%d :", static_cast<unsigned int>(data));
	WriteLog(str);
	WriteLog(tmp);
	WriteLog("\n");
}

void CUavLink::WriteLogRx()
{
	ChkFileSize();

	WriteLog("\n����");
	WriteLogTime();	// time
	WriteLog("\n");
	for (int i = 0; i < m_Rx.nDataCount; ++i){
		WriteLog(i, m_Rx.Data[i]);
	}
}