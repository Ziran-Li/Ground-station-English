
//	UGV �l�b�g���[�N�p�ɃJ�X�^�}�C�Y�����N���X�Q
//	X-Monitor �p

#pragma once

#include "CommBuffer.h"
#include "common/CommonMacro.h"

#include "ExclusiveObject.h"
#include "WinsockTCP2.h"

#include <queue>

// �N���X�̃v���g�^�C�v�錾
class XMonitorServer;

// �C���e���Z���X��L�����p���邽�߂̖��O���
class MyRxBuffer;
class MyTxBuffer;

namespace exc {
	typedef CExclusivePointer<MyRxBuffer> RxBuffer;
	typedef CExclusivePointer<MyTxBuffer> TxBuffer;
}

struct XMLRecvData{
	unsigned int connectionID;  // �ڑ���������ʂ��邽�߂�ID
	std::string xmlData;        // ��M����XML��

	XMLRecvData() : connectionID(0), xmlData(){};
};

//*******************************************************************************
//	��M�o�b�t�@�̊g����
//
//	�����F
//		���g���G���f�B�A���A���C�����g�̒ʐM�p��M�o�b�t�@�ɁC�p�P�b�g�̓ǂ�
//		�o���@�\��ǉ��������́D
//
//	�ǉ��@�\�F
//*******************************************************************************
class MyRxBuffer : public CommBuffer::LittleEndianProtocol::RxBuffer
{
protected:

	size_t m_AllowedPacketSize; // ���e����ő�̃p�P�b�g�T�C�Y�i���̒l�ȏ�̃T�C�Y���ݒ肳��Ă���p�P�b�g�͖�������j

public:
	MyRxBuffer(size_t nBlockSize, size_t nMaximumSize = NO_LIMIT, size_t nAllowedPacketSize = NO_LIMIT);

	BOOL       GetPacket(std::string &strPacket, const unsigned char delimiter);
};

//*******************************************************************************
//	���M�o�b�t�@�̊g����
//
//	�����F
//		���g���G���f�B�A���A���C�����g�̒ʐM�p���M�o�b�t�@�ɁC�p�P�b�g�̌����Ɠǂ�
//		�o���@�\��ǉ��������́D
//
//	�ǉ��@�\�F
//*******************************************************************************
class MyTxBuffer : public CommBuffer::LittleEndianProtocol::TxBuffer
{
public:
	MyTxBuffer(size_t nBlockSize, size_t nMaximumSize = NO_LIMIT);

	BOOL SetPacket(const std::string &strPacket);
};

//*******************************************************************************
//	�g���P�ʐڑ��N���X�i�T�[�o�[�E�N���C�A���g���ʕ����j
//
//	�����F
//*******************************************************************************
class MyWSAConnection : public WSAConnection
{
protected:
	exc::RxBuffer m_RxBuff;
	exc::TxBuffer m_TxBuff;

public:
	MyWSAConnection(size_t RxBlockSize, size_t TxBlockSize);

	BOOL SendPacket(const std::string &strPacket);

protected:
	virtual void OnSend();
	virtual void OnReceive();
	virtual void OnClose();
};

//*******************************************************************************
//	�T�[�o�[�p�g���P�ʐڑ��N���X
//
//	�����F
//*******************************************************************************
class MyWSAConnection_For_Server : public MyWSAConnection
{
	friend class XMonitorServer;

private:
	enum {
		RXBUFFER_BLOCK_SIZE = 1024,
		TXBUFFER_BLOCK_SIZE = 51200,
	};
public:
	MyWSAConnection_For_Server() : MyWSAConnection(RXBUFFER_BLOCK_SIZE, TXBUFFER_BLOCK_SIZE) {}
};

//*******************************************************************************
//	�N���C�A���g�p�g���P�ʐڑ��N���X
//
//	�����F
//*******************************************************************************
class MyWSAConnection_For_Client : public MyWSAConnection
{
private:
	enum {
		RXBUFFER_BLOCK_SIZE = 51200,
		TXBUFFER_BLOCK_SIZE = 1024,
	};
public:
	MyWSAConnection_For_Client() : MyWSAConnection(RXBUFFER_BLOCK_SIZE, TXBUFFER_BLOCK_SIZE) {}
};

//*******************************************************************************
//	X-Monitor �p�T�[�o�[�N���X
//
//	�����F
//*******************************************************************************
class XMonitorServer : public WSATcpServer<MyWSAConnection_For_Server>
{
public:
	//enum RECEIVE_DATA_TYPE
	//{
	//	CHANGE_NAVDATA,
	//	CHANGE_IMUDATA,
	//	CHANGE_LOCALPOS,
	//	CHANGE_ANEMOMETERDATA,
	//	CHANGE_GCSPROPODATA,
	//	MAINLINK_DATA_TRANSMISSION,
	//};

	//typedef void (*ReceiveCallbackFunc)(XMonitorServer* pServer, RECEIVE_DATA_TYPE ReceiveDataType, void* pUser);

protected:
	// ���b�Z�[�W�֘A�i���̂Ƃ���r������͂��Ă��Ȃ��j
	DWORD m_ReceiveNoticeMessage_idThread; // ��M�ʒm���b�Z�[�W�̃|�X�g��X���b�h
	UINT  m_ReceiveNoticeMessage_Msg;      // ��M�ʒm���Ɏg�p���郁�b�Z�[�W

	DWORD m_AcceptNoticeMessage_idThread;  // �ڑ��ʒm���b�Z�[�W�̃|�X�g��X���b�h
	UINT  m_AcceptNoticeMessage_Msg;       // �ڑ��ʒm���Ɏg�p���郁�b�Z�[�W

	DWORD m_CloseNoticeMessage_idThread;   // �ؒf�ʒm���b�Z�[�W�̃|�X�g��X���b�h
	UINT  m_CloseNoticeMessage_Msg;        // �ؒf�ʒm���Ɏg�p���郁�b�Z�[�W

	// ��M�f�[�^�L���[
	CExclusiveObject< std::queue<XMLRecvData> >            m_XMLRecvDataQueue;

public:
	XMonitorServer();
	virtual ~XMonitorServer() { Destroy(); }

	// ���b�Z�[�W�̓o�^
	BOOL RegisterReceiveNoticeMessage(DWORD idThread, UINT Msg); // ��M�ʒm���b�Z�[�W�̑�����o�^����
	BOOL RegisterAcceptNoticeMessage (DWORD idThread, UINT Msg);
	BOOL RegisterCloseNoticeMessage  (DWORD idThread, UINT Msg);

	// �f�[�^�̑��M
	BOOL SendPacket(const std::string &packet, UINT connectionID); // �ėp�p�P�b�g���M�֐�
	//BOOL SendPacket(const PacketBase* pPacket); // �ėp�p�P�b�g���M�֐�
	//BOOL SendMAVData(const CMavState& Mav);
	//BOOL SendGCSData(const CGcsState& Gcs);
	//BOOL SendSync();

	// ��M�f�[�^�̎擾
	BOOL   PopRecvData(std::string& recvData, unsigned int &connectionID); // ��M�p�P�b�g�̒��g�A�ڑ�ID
	//BOOL   PopPayload_ChangeData(IMUData& Imu);
	//BOOL   PopPayload_ChangeData(LocalPos& Loc);
	//BOOL   PopPayload_ChangeData(AnemometerData& Anemo);
	//BOOL   PopPayload_ChangeGCSPropoData(PropoData& Propo);
	//size_t GetSize_FrontOfMainLinkData() const;
	//BOOL   PopPayload_MainLinkData(PACKETID& PacketID, BYTE* pData, DWORD DataSize);
	//BOOL   PopPayload_MainLinkData(); // �j��

protected:
	virtual void OnReceive(MyWSAConnection_For_Server* pClient);
	virtual void OnAccept (MyWSAConnection_For_Server* pClient);
	virtual void OnClose  (MyWSAConnection_For_Server* pClient);
};

//*******************************************************************************
//	X-Monitor �N���C�A���g�N���X�i�����j
//
//	�����F
//		�{�N���X�́C�����p�P�b�g�𗘗p���邱�Ƃ� X-Monitor ���̎��n��ɐ��m�� 
//		MavState �� GcsState ��񋟂��܂��B
//		������ MavState �� GcsState �̎��n�񂪕K�v�ȏꍇ�ɖ{�N���X���g�p���Ă���
//		�����B
//
//	���ӁF
//		�o�b�t�@�Ƀf�[�^�����܂�Ȃ��悤�ɂ��邽�߁C�ł��邾�� 20ms �ȉ��̎�����
//		�f�[�^�擾���s���Ă��������D
//*******************************************************************************
class XMonitorSyncClient : public WSATcpClient<MyWSAConnection_For_Client>
{
protected:
	HANDLE m_hSyncEvent;

	typedef CExclusiveObject< std::queue<std::string> > RecvDataQueue;

	RecvDataQueue m_RecvDataQueue;

	//// OnReceive �̂݃A�N�Z�X����
	//MavStatePacket m_MavStatePacket;
	//GcsStatePacket m_GcsStatePacket;
	//BOOL           m_flagMavStatePacket;
	//BOOL           m_flagGcsStatePacket;

public:
	XMonitorSyncClient() 
	{
		m_hSyncEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);	// �������Z�b�g
		//m_flagMavStatePacket = FALSE;
		//m_flagGcsStatePacket = FALSE;
	}
	virtual ~XMonitorSyncClient() 
	{ 
		::CloseHandle(m_hSyncEvent);
		Destroy(); 
	}

	//BOOL GetData(CMavState& Mav, CGcsState& Gcs, DWORD msecTimeout = 2000);
	BOOL GetData(std::vector<std::string>& recvData, DWORD msecTimeout = 2000);
	BOOL SendData(const std::string &sendData);

protected:
	virtual void OnReceive();
	virtual void OnClose();
	virtual void OnSync() {}
};

//
////*******************************************************************************
////	X-Monitor �N���C�A���g�N���X�i�񓯊��j
////
////	�����F
////		�{�N���X�́CX-Monitor �����M�����ŐV�̃f�[�^��񋟂��܂��D
////		X-Monitor �Ƃ̓����͍s���Ȃ����߁C�f�[�^�̌�����d�������f�[�^�̎擾��
////		�N���蓾�܂��B
////		�f�[�^�̃��j�^�����O�Ȃǂ̗p�r�Ɏg�p���Ă��������B
////		
////	���ӁF
////		X-Monitor ���Ń��O�f�[�^�̍Đ����s���ꍇ�C�{�N���X��p�����v���O�����ł�
////		����̍Č������ۏ؂���܂���B
////*******************************************************************************
//class XMonitorAsyncClient : public WSATcpClient<MyWSAConnection_For_Client>
//{
//protected:
//	typedef CExclusiveObject< CMavState > MavState;
//	typedef CExclusiveObject< CGcsState > GcsState;
//
//	MavState m_MavState;
//	GcsState m_GcsState;
//
//	BOOL m_flagMavStateReceived; // ��M�t���O
//	BOOL m_flagGcsStateReceived; // ��M�t���O
//
//public:
//	XMonitorAsyncClient() 
//	{
//		m_flagMavStateReceived = FALSE;
//		m_flagGcsStateReceived = FALSE;
//	}
//	virtual ~XMonitorAsyncClient() 
//	{
//		Destroy(); 
//	}
//
//	BOOL GetData(CMavState& Mav);                 // ��M���� MAV �֘A�f�[�^���擾����
//	BOOL GetData(CGcsState& Gcs);                 // ��M���� GCS �֘A�f�[�^���擾����
//	BOOL GetData(CMavState& Mav, CGcsState& Gcs); // ��M���� MAV, GCS �֘A�f�[�^���擾����
//
//	BOOL ChangeData(const NAVData& Nav);          // X-Monitor �� NAVData �𑗂��ē����f�[�^��ύX����
//	BOOL ChangeData(const IMUData& Imu);          // X-Monitor �� IMUData �𑗂��ē����f�[�^��ύX����
//	BOOL ChangeData(const LocalPos& Loc);         // X-Monitor �� LocalPos �𑗂��ē����f�[�^��ύX���� 
//	BOOL ChangeData(const AnemometerData& Anemo); // X-Monitor �� AnemometerData �𑗂��ē����f�[�^��ύX����
//	BOOL ChangeGCSPropoData(const PropoData& Propo);
//
//	BOOL TransmitPacketToMAV(PACKETID PacketID, const void* pData, size_t nDataLength); // X-Monitor �o�R�� MAV �Ƀp�P�b�g�𑗐M����
//
//protected:
//	virtual void OnConnect();
//	virtual void OnReceive();
//	virtual void OnClose();
//};