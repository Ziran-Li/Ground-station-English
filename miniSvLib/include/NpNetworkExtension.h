
//	���������V�X�e���̃l�b�g���[�N�p�ɃJ�X�^�}�C�Y�����N���X�Q

#pragma once
#include "MyNetworkExtension.h"

#include "ExclusiveObject.h"
#include "WinsockTCP2.h"

#include <queue>


// 
//*******************************************************************************
//	���������V�X�e���p�ʐM�f�[�^�p�\���̂���ђ�`
//
//	�����F
//*******************************************************************************

// �萔
enum {
	CAM_DATA_MAX_SIZE = 50000,	// �����/�������J����JPEG�f�[�^�̍ő�T�C�Y�@temporary
	CH1_4_SIZE = 5,				// UDP5��Ch1�`Ch4�̃T�C�Y[byte]
	LRF_DATA_H_SIZE = 416,		// LRF�f�[�^(����)�̃T�C�Y[byte]
	LRF_DATA_V_SIZE = 169,		// LRF�f�[�^(����)�̃T�C�Y[byte]
	CLT_MATRIX_SIZE = 3,		// ���G�x�s��̃T�C�Y
	DEBUG_SIZE = 8,				// �f�o�b�O�p�f�[�^�̃T�C�Y
	EXCHG_STATUS_SIZE = 16,		// �o�b�e�������@�p�f�[�^�̃T�C�Y
};

// UDP�p�P�b�gID
enum UDP_PACKET_ID{
	PACKET_ID_UDP1 = 0x0102,
	PACKET_ID_UDP2 = 0x0201,
	PACKET_ID_UDP3 = 0x0301,
	PACKET_ID_UDP4 = 0x0401,
	PACKET_ID_UDP5 = 0x0501,
	PACKET_ID_UDP6 = 0x0601,
};


#pragma pack(1) // �\���̂̃A���C�����g��1�o�C�g�ɂ���

// UDP�p�P�b�g�̃w�b�_
struct UdpPacketHeader {
	BYTE preamble1;		// HEADER1
	BYTE preamble2;		// HEADER2
	USHORT packetId;	// �p�P�b�gID
	UINT payloadLen;	// �y�C���[�h��

	UdpPacketHeader() : preamble1(0xAA), preamble2(0xAA), packetId(0x0000), payloadLen(0) {}
};

// UDP�p�P�b�g�̃t�b�^
struct UdpPacketFooter {
	BYTE postamble1;	// FOOTTER1
	BYTE postamble2;	// FOOTTER2

	UdpPacketFooter() : postamble1(0x55), postamble2(0x55) {}
};


// UDP1�̃y�C���[�h
struct Payload_UDP1 {
	ULONG msapTime;					// MSAP�����i�N��������j
	float uavAvel_X;				// �p���x�iX�j
	float uavAvel_Y;				// �p���x�iY�j
	float uavAvel_Z;				// �p���x�iZ�j
	float uavAcc_X;					// �����x�iX�j
	float uavAcc_Y;					// �����x�iY�j
	float uavAcc_Z;					// �����x�iZ�j
	float uavAtt_Roll;				// �p���p�i���[���j
	float uavAtt_Pitch;				// �p���p�i�s�b�`�j
	float presAlt;					// �C�����x
	float uavTgtPos_X;				// �ʒu�ڕW�l�iX�j
	float uavTgtPos_Y;				// �ʒu�ڕW�l�iY�j
	float uavTgtPos_Z;				// �ʒu�ڕW�l�iZ�j
	short uavTgtVel_X;				// ���x�ڕW�l�iX�j
	short uavTgtVel_Y;				// ���x�ڕW�l�iY�j
	short uavTgtVel_Z;				// ���x�ڕW�l�iZ�j
	short uavTgtAtt_Roll;			// �p���ڕW�l�i���[���j
	short uavTgtAtt_Pitch;			// �p���ڕW�l�i�s�b�`�j
	short uavTgtAtt_Yaw;			// �p���ڕW�l�i���[�j
	USHORT uavCtrlInH_Th;			// ��ʐ�����́i�X���b�g���j
	USHORT uavCtrlInH_Roll;			// ��ʐ�����́i���[���j
	USHORT uavCtrlInH_Pitch;		// ��ʐ�����́i�s�b�`�j
	USHORT uavCtrlInH_Yaw;			// ��ʐ�����́i���[�j
	USHORT uavCtrlInL_Th;			// ���ʐ�����́i�X���b�g���j
	short uavCtrlInL_Roll;			// ���ʐ�����́i���[���j
	short uavCtrlInL_Pitch;			// ���ʐ�����́i�s�b�`�j
	short uavCtrlInL_Yaw;			// ���ʐ�����́i���[�j
	USHORT distUs_Back;				// ��������g����
	USHORT distUs_Nad;				// �����������g����
	USHORT distUs_Zen;				// ����������g����
	float voltage;					// �o�b�e���d��
	BYTE flightMode;				// ��s���[�h
	USHORT msapStatus;				// MSAP�X�e�[�^�X
	BYTE debug[DEBUG_SIZE];			// �f�o�b�O�p�f�[�^
	ULONG slamPcTime;				// SLAM-PC�����i���Z�J�n����j
	LONGLONG unixTime_Sec;			// UNIX�����iUTC�j
	int unixTime_uSec;				// UNIX�����i�}�C�N���b�����j
	float uavPos_X;					// �ʒu�iX�j
	float uavPos_Y;					// �ʒu�iY�j
	float uavPos_Z;					// �ʒu�iZ�j
	float uavVel_X;					// ���x�iX�j
	float uavVel_Y;					// ���x�iY�j
	float uavVel_Z;					// ���x�iZ�j
	float uavAtt_Yaw;				// �p���p�i���[�j
	USHORT dist_Fwd;				// �O������
	USHORT dist_FwdRight;			// �E�O������
	USHORT dist_Right;				// �E��������
	USHORT dist_RightBack;			// �E�������
	USHORT dist_LeftBack;			// ���������
	USHORT dist_Left;				// ����������
	USHORT dist_FwdLeft;			// ���O������
	USHORT dist_Nad;				// ����������
	USHORT slamDataStatus;			// SLAM�f�[�^�X�e�[�^�X
	USHORT lrfData[LRF_DATA_H_SIZE];	// ���[�U�X�L�����f�[�^�z��i�����j
	USHORT lrfDataV[LRF_DATA_V_SIZE];	// ���[�U�X�L�����f�[�^�z��i�����j

	Payload_UDP1() : msapTime(0), uavAvel_X(0.f), uavAvel_Y(0.f), uavAvel_Z(0.f),
		uavAcc_X(0.f), uavAcc_Y(0.f), uavAcc_Z(0.f),
		uavAtt_Roll(0.f), uavAtt_Pitch(0.f), presAlt(0.f),
		uavTgtPos_X(0.f), uavTgtPos_Y(0.f), uavTgtPos_Z(0.f),
		uavTgtVel_X(0), uavTgtVel_Y(0), uavTgtVel_Z(0),
		uavTgtAtt_Roll(0), uavTgtAtt_Pitch(0), uavTgtAtt_Yaw(0),
		uavCtrlInH_Th(0), uavCtrlInH_Roll(0), uavCtrlInH_Pitch(0), uavCtrlInH_Yaw(0),
		uavCtrlInL_Th(0), uavCtrlInL_Roll(0), uavCtrlInL_Pitch(0), uavCtrlInL_Yaw(0),
		distUs_Back(0), distUs_Nad(0), distUs_Zen(0),
		voltage(0.f), flightMode(0), msapStatus(0), slamPcTime(0),
		unixTime_Sec(0), unixTime_uSec(0),
		uavPos_X(0.f), uavPos_Y(0.f), uavPos_Z(0.f), uavVel_X(0.f), uavVel_Y(0.f), uavVel_Z(0.f), uavAtt_Yaw(0.f),
		dist_Fwd(0), dist_FwdRight(0), dist_Right(0), dist_RightBack(0),
		dist_LeftBack(0), dist_Left(0), dist_FwdLeft(0), dist_Nad(0), slamDataStatus(0)
	{
		ZeroMemory(debug, DEBUG_SIZE);
		ZeroMemory(lrfData, LRF_DATA_H_SIZE);
		ZeroMemory(lrfDataV, LRF_DATA_V_SIZE);
	}
};

// UDP2�̃y�C���[�h
struct Payload_UDP2_wo_DownCam {
	ULONG syncFlag;			// �t���O
	ULONG slamPcTime;		// SLAM-PC�����i���Z�J�n����j
	LONGLONG unixTime_Sec;	// UNIX�����iUTC�j
	int unixTime_uSec;		// UNIX�����i�}�C�N���b�����j
	char clutterData_cols;	// ���G�x�f�[�^�̗񐔁iu�j
	char clutterData_rows;	// ���G�x�f�[�^�̍s���iv�j
	float clutterData[CLT_MATRIX_SIZE][CLT_MATRIX_SIZE];	// ���G�x
	UINT jpegDataSize;		// JPEG�f�[�^�T�C�Y

	Payload_UDP2_wo_DownCam() : syncFlag(0), slamPcTime(0), unixTime_Sec(0), unixTime_uSec(0),
							  clutterData_cols(0), clutterData_rows(0), jpegDataSize(0)
	{
		for (UINT ui = 0; ui < CLT_MATRIX_SIZE; ++ui){
			for (UINT uj = 0; uj < CLT_MATRIX_SIZE; ++uj){
				clutterData[ui][uj] = 0.;
			}
		}
	}
};

struct Payload_UDP2_DownCam {
	BYTE downCamData[CAM_DATA_MAX_SIZE];	// �������J����JPEG�f�[�^

	Payload_UDP2_DownCam() { ::ZeroMemory(downCamData, CAM_DATA_MAX_SIZE); }
};

// UDP3�̃y�C���[�h(��J����JPEG�f�[�^(�ϒ�)������)
struct Payload_UDP3_wo_UpCam {
	ULONG syncFlag;			// �t���O
	ULONG slamPcTime;		// SLAM-PC�����i���Z�J�n����j
	LONGLONG unixTime_Sec;	// UNIX�����iUTC�j
	int unixTime_uSec;		// UNIX�����i�}�C�N���b�����j
	UINT jpegDataSize;		// JPEG�f�[�^�T�C�Y

	Payload_UDP3_wo_UpCam() : syncFlag(0), slamPcTime(0), unixTime_Sec(0), unixTime_uSec(0), jpegDataSize(0) {}
};

struct Payload_UDP3_UpCam {
	BYTE upCamData[CAM_DATA_MAX_SIZE];	// ������J����JPEG�f�[�^

	Payload_UDP3_UpCam() { ::ZeroMemory(upCamData, CAM_DATA_MAX_SIZE); }
};

// UDP4�̃y�C���[�h
struct Payload_UDP4 {
	ULONG syncFlag;			// �t���O
	ULONG slamPcTime;		// SLAM-PC�����i���Z�J�n����j
	LONGLONG unixTime_Sec;	// UNIX�����iUTC�j
	int unixTime_uSec;		// UNIX�����i�}�C�N���b�����j
	UCHAR statisErr;		// ���v�덷
	float equivDose;		// �������ʗ�

	Payload_UDP4() : syncFlag(0), slamPcTime(0), unixTime_Sec(0), unixTime_uSec(0), statisErr(0), equivDose(0.) {}
};

// UDP5�̃y�C���[�h
struct Payload_UDP5 {
	BYTE  axis[CH1_4_SIZE];		// Ch1�`Ch4
	BYTE  buttonA;				// Ch5�`Ch12
	BYTE  buttonB;				// Ch13�`Ch20

	Payload_UDP5() : buttonA(0), buttonB(0) { ::ZeroMemory(axis, CH1_4_SIZE); }
};

// UDP6�̃y�C���[�h
struct Payload_UDP6 {
	float uavAcc_X;					// �����x�iX�j
	float uavAcc_Y;					// �����x�iY�j
	float uavAcc_Z;					// �����x�iZ�j
	float uavAtt_Roll;				// �p���p�i���[���j
	float uavAtt_Pitch;				// �p���p�i�s�b�`�j
	float presAlt;					// �C�����x
	float uavPos_X;					// �ʒu�iX�j
	float uavPos_Y;					// �ʒu�iY�j
	float uavPos_Z;					// �ʒu�iZ�j
	float uavAtt_Yaw;				// �p���p�i���[�j
	USHORT lrfData[LRF_DATA_H_SIZE];	// ���[�U�X�L�����f�[�^�z��i�����j
	USHORT lrfDataV[LRF_DATA_V_SIZE];	// ���[�U�X�L�����f�[�^�z��i�����j

	Payload_UDP6() : uavAcc_X(0.f), uavAcc_Y(0.f), uavAcc_Z(0.f),
		uavAtt_Roll(0.f), uavAtt_Pitch(0.f), presAlt(0.f),
		uavPos_X(0.f), uavPos_Y(0.f), uavPos_Z(0.f), uavAtt_Yaw(0.f)
	{
		ZeroMemory(lrfData, LRF_DATA_H_SIZE);
		ZeroMemory(lrfDataV, LRF_DATA_V_SIZE);
	}
};

#pragma pack() // �\���̂̃A���C�����g���f�t�H���g�ɖ߂�

// TCP1�̎�M�y�C���[�h
struct Payload_TCP1_Recv {
	double uavSlamvel_X;
	double uavSlamvel_Y;
	double uavRoll;
	double uavPitch;
	double uavYaw;
	double uavPos_X;
	double uavPos_Y;
	USHORT dist_Fwd;
	USHORT dist_FwdRight;
	USHORT dist_Right;
	USHORT dist_Left;
	USHORT dist_FwdLeft;
	USHORT distUs_Back;
	USHORT distUs_Nad;
	USHORT distUs_Zen;
	double voltage;
	BYTE flightMode;
	USHORT msapStatus;
	USHORT slampcStatus;
	USHORT uavCtrlInH_Th;
	USHORT uavCtrlInH_Roll;
	USHORT uavCtrlInH_Pitch;
	USHORT uavCtrlInH_Yaw;
	double uavViconvel_X;
	double uavViconvel_Y;
	double uavViconvel_Z;
	double uavViconpos_X;
	double uavViconpos_Y;
	double uavViconpos_Z;
	double uavViconatt_X;
	double uavViconatt_Y;
	double uavViconatt_Z;
	BYTE uavViconStatus;
	BYTE exchgStatus[EXCHG_STATUS_SIZE];

	Payload_TCP1_Recv() : uavSlamvel_X(0.0), uavSlamvel_Y(0.0),
		uavRoll(0.0), uavPitch(0.0), uavYaw(0.0),
		uavPos_X(0.0), uavPos_Y(0.0),
		dist_Fwd(0), dist_FwdRight(0), dist_Right(0), dist_Left(0), dist_FwdLeft(0),
		distUs_Nad(0), distUs_Zen(0), voltage(0.0), flightMode(0),
		msapStatus(0), slampcStatus(0),
		uavCtrlInH_Th(0), uavCtrlInH_Roll(0), uavCtrlInH_Pitch(0), uavCtrlInH_Yaw(0),
		uavViconvel_X(0.0), uavViconvel_Y(0.0), uavViconvel_Z(0.0),
		uavViconpos_X(0.0), uavViconpos_Y(0.0), uavViconpos_Z(0.0),
		uavViconatt_X(0.0), uavViconatt_Y(0.0), uavViconatt_Z(0.0),
		uavViconStatus(0)
	{
		::ZeroMemory(exchgStatus, sizeof(exchgStatus));
	}
};

// TCP1�̑��M�y�C���[�h
struct Payload_TCP1_Send {
	char fpvRecv;

	Payload_TCP1_Send() : fpvRecv(NULL)
	{
	}
};

//*******************************************************************************
//	���������V�X�e���pUDP��{�p�P�b�g�N���X
//
//	�����F
//*******************************************************************************
class UdpPacket
{
protected:
	std::vector<UCHAR> m_buf;

public:
	UdpPacket() {};
	virtual ~UdpPacket() {};

	size_t GetBufSize() const { return m_buf.size(); }
	UCHAR *GetBufPtr() { return &m_buf[0]; }
	void Clear() { m_buf.clear(); }
	void ZeroClear() { std::fill(m_buf.begin(), m_buf.end(), 0); }	// ���ׂĂ̗v�f��0�Ŗ��߂�
};

//*******************************************************************************
//	���������V�X�e���pUDP���M�p�P�b�g�N���X
//
//	�����F
//*******************************************************************************
class UdpSendPacket : public UdpPacket
{
protected:

public:
	UdpSendPacket() {};
	virtual ~UdpSendPacket() {};

	BOOL PushHeader(USHORT packetId, UINT payloadLen);
	BOOL PushFooter();
	BOOL PushData(const void *src, UINT srcSize);
};

//*******************************************************************************
//	���������V�X�e���pUDP��M�p�P�b�g�N���X
//
//	�����F
//*******************************************************************************
class UdpRecvPacket : public UdpPacket
{
protected:

public:
	UdpRecvPacket(size_t bufSize) { m_buf.resize(bufSize); }
	virtual ~UdpRecvPacket() {};

	USHORT GetPacketId() const;
	UINT GetPayloadLen() const;
	BOOL GetPayloadData(void *dest, UINT destSize, UINT offset = 0);
};


//*******************************************************************************
//	���������V�X�e���pUDP�T�[�o�[�N���X
//
//	�����F
//*******************************************************************************
class NpUdpServer : public WSAUdpServer
{
private:
	typedef CExclusiveObject< std::queue<Payload_UDP1> > DataQueueUDP1;
	typedef CExclusiveObject< std::queue<Payload_UDP2_wo_DownCam> > DataQueueUDP2_wo_DownCam;
	typedef CExclusiveObject< std::queue<Payload_UDP2_DownCam> > DataQueueUDP2_DownCam;
	typedef CExclusiveObject< std::queue<Payload_UDP3_wo_UpCam> > DataQueueUDP3_wo_UpCam;
	typedef CExclusiveObject< std::queue<Payload_UDP3_UpCam> > DataQueueUDP3_UpCam;
	typedef CExclusiveObject< std::queue<Payload_UDP4> > DataQueueUDP4;
	typedef CExclusiveObject< std::queue<Payload_UDP5> > DataQueueUDP5;
	typedef CExclusiveObject< std::queue<Payload_UDP6> > DataQueueUDP6;

	DataQueueUDP1 m_dataUDP1;					// �uUDP1�v�p�P�b�g�f�[�^(�r�����䂠��)
	DataQueueUDP2_wo_DownCam m_dataUDP2_wo_DownCam;	// �uUDP2�v�p�P�b�g�f�[�^�F�������J�����f�[�^������(�r�����䂠��)
	DataQueueUDP2_DownCam m_dataUDP2_DownCam;		// �uUDP2�v�p�P�b�g�f�[�^�F�������J�����f�[�^(�r�����䂠��)
	DataQueueUDP3_wo_UpCam m_dataUDP3_wo_UpCam;	// �uUDP3�v�p�P�b�g�f�[�^�F������J�����f�[�^������(�r�����䂠��)
	DataQueueUDP3_UpCam m_dataUDP3_UpCam;		// �uUDP3�v�p�P�b�g�f�[�^�F������J�����f�[�^(�r�����䂠��)
	DataQueueUDP4 m_dataUDP4;					// �uUDP4�v�p�P�b�g�f�[�^(�r�����䂠��)
	DataQueueUDP5 m_dataUDP5;					// �uUDP5�v�p�P�b�g�f�[�^(�r�����䂠��)
	DataQueueUDP6 m_dataUDP6;					// �uUDP6�v�p�P�b�g�f�[�^(�r�����䂠��)
	DWORD m_ReceiveNoticeMessage_idThread;		// ��M�ʒm���b�Z�[�W�̃|�X�g��X���b�h
	UINT  m_ReceiveNoticeMessage_Msg;			// ��M�ʒm���Ɏg�p���郁�b�Z�[�W

public:
	NpUdpServer();
	virtual ~NpUdpServer() {}

	virtual void OnReceive();
	virtual void OnClose() {}	// temporary

	// �uUDP1�v�̃y�C���[�h�f�[�^���擾����
	BOOL GetDataUDP1(Payload_UDP1 &payload_udp1);
	// �uUDP2�v�̃y�C���[�h�f�[�^���擾����
	BOOL GetDataUDP2(Payload_UDP2_wo_DownCam &payload_udp2_wo_DownCam, Payload_UDP2_DownCam &payload_udp2_DownCam);
	// �uUDP3�v�̃y�C���[�h�f�[�^���擾����
	BOOL GetDataUDP3(Payload_UDP3_wo_UpCam &payload_udp3_wo_UpCam, Payload_UDP3_UpCam &payload_udp3_UpCam);	
	// �uUDP4�v�̃y�C���[�h�f�[�^���擾����
	BOOL GetDataUDP4(Payload_UDP4 &payload_udp4);
	// �uUDP5�v�̃y�C���[�h�f�[�^���擾����
	BOOL GetDataUDP5(Payload_UDP5 &payload_udp5);
	// �uUDP6�v�̃y�C���[�h�f�[�^���擾����
	BOOL GetDataUDP6(Payload_UDP6 &payload_udp6);
	// ��M�ʒm���b�Z�[�W�̑�����o�^����
	BOOL RegisterReceiveNoticeMessage(DWORD idThread, UINT Msg);	
};

//*******************************************************************************
//	���������V�X�e���pUDP�N���C�A���g�N���X
//
//	�����F
//*******************************************************************************
class NpUdpClient : public WSAUdpClient
{
protected:
	HANDLE m_hSyncEvent;

	Payload_UDP1 m_dataUDP1;					// �uUDP1�v�p�P�b�g�f�[�^
	Payload_UDP2_wo_DownCam m_dataUDP2_wo_DownCam;	// �uUDP2�v�p�P�b�g�f�[�^�F�������J�����f�[�^������
	Payload_UDP2_DownCam m_dataUDP2_DownCam;		// �uUDP2�v�p�P�b�g�f�[�^�F�������J�����f�[�^
	Payload_UDP3_wo_UpCam m_dataUDP3_wo_UpCam;	// �uUDP3�v�p�P�b�g�f�[�^�F������J�����f�[�^������
	Payload_UDP3_UpCam m_dataUDP3_UpCam;		// �uUDP3�v�p�P�b�g�f�[�^�F������J�����f�[�^
	Payload_UDP4 m_dataUDP4;					// �uUDP4�v�p�P�b�g�f�[�^
	Payload_UDP5 m_dataUDP5;					// �uUDP5�v�p�P�b�g�f�[�^
	Payload_UDP6 m_dataUDP6;					// �uUDP6�v�p�P�b�g�f�[�^

public:
	NpUdpClient()
	{
		m_hSyncEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);	// �������Z�b�g
	}
	virtual ~NpUdpClient()
	{ 
		::CloseHandle(m_hSyncEvent);
		Destroy(); 
	}

	// �uUDP1�v�̃y�C���[�h�f�[�^�𑗐M����
	BOOL SendDataUDP1(const Payload_UDP1 &payload_udp1);
	// �uUDP2�v�̃y�C���[�h�f�[�^�𑗐M����
	BOOL SendDataUDP2(const Payload_UDP2_wo_DownCam &payload_udp2_wo_DownCam, const Payload_UDP2_DownCam payload_udp2_DownCam);
	// �uUDP3�v�̃y�C���[�h�f�[�^�𑗐M����
	BOOL SendDataUDP3(const Payload_UDP3_wo_UpCam &payload_udp3_wo_UpCam, const Payload_UDP3_UpCam payload_udp3_UpCam);
	// �uUDP4�v�̃y�C���[�h�f�[�^�𑗐M����
	BOOL SendDataUDP4(const Payload_UDP4 &payload_udp4);
	// �uUDP5�v�̃y�C���[�h�f�[�^�𑗐M����
	BOOL SendDataUDP5(const Payload_UDP5 &payload_udp5);
	// �uUDP6�v�̃y�C���[�h�f�[�^�𑗐M����
	BOOL SendDataUDP6(const Payload_UDP6 &payload_udp6);

protected:
	virtual void OnReceive();
	virtual void OnClose() {}	// temporary
	virtual void OnSync() {}
};

//*******************************************************************************
//	���������V�X�e���pTCP�N���C�A���g�N���X
//
//	�����F
//*******************************************************************************
class NpTcpClient : public XMonitorSyncClient
{
private:
	DWORD m_ReceiveNoticeMessage_idThread;		// ��M�ʒm���b�Z�[�W�̃|�X�g��X���b�h
	UINT  m_ReceiveNoticeMessage_Msg;			// ��M�ʒm���Ɏg�p���郁�b�Z�[�W
	BOOL m_RequestData;							// �f�[�^�v���t���O

public:
	NpTcpClient();
	virtual ~NpTcpClient() {}

	// ��M�ʒm���b�Z�[�W�̑�����o�^����
	BOOL RegisterReceiveNoticeMessage(DWORD idThread, UINT Msg);

	// �f�[�^�v���擾
	BOOL IsRequested() { return m_RequestData; }
	// �f�[�^�v���ݒ�
	void SetRequest(BOOL req) { m_RequestData = req; }

protected:
	virtual void OnSync();
};
