#pragma once

#include <vector>
#include "SCI.h"

//#import "msxml3.dll" named_guids
#import "msxml6.dll" named_guids

class CUavLink;
class dataManage;

class UavLinkManage
{
	//// �萔

	// RSSI��Health�l�̌v�Z�p
	enum{
		PSSI_PARAM_A		= -100,		// �v�Z���ʂ�0�ɂȂ�Ƃ���RSSI�l
		PSSI_PARAM_B		= -80,		// �v�Z���ʂ�15�ɂȂ�Ƃ���RSSI�l
		PSSI_PARAM_TIMEOUT	= 1000,		// �^�C���A�E�g����[ms]
	};

	// �R�}���h�o�b�t�@�̒�`�i�R�}���h�𑗂鏇�Ԃɏォ����ׂ�j
	enum CmdBufId{
		CMD_BUF_SLEEP_REPLY	= 0,		// COMMAND_SLEEP�R�}���h�̃��v���C
		CMD_BUF_GO_HOME,				// GO HOME�R�}���h
		CMD_BUF_LAND,					// LAND�R�}���h
		CMD_BUF_PAUSE,					// PAUSE�R�}���h
		CMD_BUF_RESUME,					// RESUME�R�}���h
		CMD_BUF_WAYPOINT,				// �E�F�C�|�C���g�R�}���h
		CMD_BUF_ACTIONOINT_ACK,			// �A�N�V�������s�n�_�f�[�^��ACK
		CMD_BUF_MULTIWAYPOINT,			// �}���`�E�F�C�|�C���g�R�}���h
		CMD_BUF_REQ_TIME_REPLY,			// COMMAND_REQUEST_TIME�R�}���h�̃��v���C
		CMD_BUF_NUM						// �R�}���h�p�o�b�t�@�̗v�f��	
	};

	//// �\����

	// �}���`�E�F�C�|�C���g�R�}���h�ɑ΂���ACK
public:
	struct MultiwayAck{
		unsigned long wptId;			// �E�F�C�|�C���gID
		double lat;						// �ܓx
		double lon;						// �o�x
		double alt;						// ���x
		double yaw;						// ����
		double spd;						// ��s���x
		double tol;						// ���B����~���a
		double wait;					// �ҋ@����
		unsigned long  funcId;			// �@�\ID

		MultiwayAck():wptId(0),lat(0),lon(0),alt(0),yaw(0),spd(0),tol(0),wait(0),funcId(0){}
	};

private:
	// �}���`�E�F�C�|�C���g���
	struct MultiWayInfo{
		unsigned long nextWptId;		// ���ɑ���ׂ��E�F�C�|�C���gID
		unsigned long prevSendTime;		// ���߂ɃR�}���h�𑗐M��������[ms]
		unsigned long randVal;			// ��M���������_���l
		BOOL isTransferring;			// �]�������ǂ���
		std::vector<MultiwayAck> ack;	// ��M����ACK�̃p�����[�^(�ʏ�A�v�f����0��1��)
		unsigned short retryInterval;	// �R�}���h���đ�����Ԋu[ms]
		unsigned int dbID;				// �R�}���h�p�����[�^���i�[����DB��ID
		MultiWayInfo():nextWptId(0),prevSendTime(0),randVal(0),isTransferring(FALSE),ack(),retryInterval(0),dbID(0){}
	};

	//// �����o�ϐ�
	CUavLink	*m_mainLink;			// ���C�������N
	dataManage	*m_xmlDataDb;			// XML�f�[�^�Ǘ�
	std::vector<_bstr_t> m_commonPath;	// XML�f�[�^�Ǘ����ɓo�^����ۂ̃p�X�i�^�O�j�̋��ʕ���
	DWORD m_recvTime;					// �L���ȃp�P�b�g���󂯎���������iRSSI��Health�l�̌v�Z�Ŏg�p�j[ms]
	MultiWayInfo m_multiwayInfo;		// �}���`�E�F�C�|�C���g���

	// COMMAND_SLEEP�ɑ΂��鑗�M�֐����s�\��
	bool m_flagSendReady;

public:
	CString m_dbgMsg;	// �f�o�b�O�p


	//// �����o�֐�
public:
	UavLinkManage();
	~UavLinkManage();

	// ������
	BOOL Initialize(CUavLink *wl, dataManage *dm);

	// MainLink�֐ڑ�����
	BOOL Connect(SCIPORT comPortNo);

	// �ڑ��`�F�b�N
	BOOL IsConnected();

	// ��M�f�[�^���f�R�[�h����XML�f�[�^�Ǘ����ɓo�^����
	int DecodeAndRegister();

	// ���[�J��������XML�f�[�^�Ǘ����ɓo�^����
	void RegisterLocalTime();

	// MultiWayPointData��XML�f�[�^�Ǘ����ɓo�^����
	void RegisterMultiWayPointData(bool flag, BSTR value);
	void RegisterData(MSXML2::IXMLDOMDocument2Ptr& pDoc);

	// �f�[�^���M(�f�[�^�̒��g�ɏ]���āA�K�؂ȑ��M�֐����R�[��)
	BOOL SendData(UINT dbID, const _bstr_t &sendMsg);

	// �}���`�E�F�C�|�C���g�R�}���h�]�������H
	BOOL IsMultiwayTransferring() const { return m_multiwayInfo.isTransferring; }

	// ��M����"�}���`�E�F�C�|�C���g�R�}���h��ACK"�̒��g�����o��
	BOOL PopMultiwayAckData(MultiwayAck &ack);

	// �}���`�E�F�C�|�C���g�R�}���h�̃��g���C�����邩���f����
	BOOL DecideMultiwayCmdRetry();

	BOOL Send();

private:
	// �f�R�[�h
	void DecodeCommand();			// UAV/GCS�ɑ΂���R�}���h
	void DecodeReply();				// COMMAND_PACKET�ɑ΂���ԓ�
	void DecodeInputData();			// ����n�̐�����̓f�[�^
	void DecodeInputData_6R();		// 6�̃��[�^�ɑ΂�����̓f�[�^���t����������n�̐�����̓f�[�^
	void DecodeInputData_12R();		// 12�̃��[�^�ɑ΂�����̓f�[�^���t����������n�̐�����̓f�[�^
	void DecodeInputData_Min();		// �ŏ����̐���n�̐�����̓f�[�^
	void DecodeImuPsData();			// IMU�Z���T�E�C���Z���T�̃f�[�^
	void DecodeMagData();			// IMU�̎���f�[�^
	void DecodeGpsData();			// GPS���f�[�^
	void DecodeGpsInfo();			// GPS�̕t�����f�[�^
	void DecodeNavData();			// �q�@���Z�f�[�^
	void DecodeNavDataEx();			// �q�@���Z�f�[�^�E�p���f�[�^�t��
	void DecodeRefData();			// ����ڕW�l�̃f�[�^
	void DecodeWayPointData();		// �E�F�C�|�C���g�̃f�[�^����ѐݒ�R�}���h
	void DecodeWayPointData2();		// �E�F�C�|�C���g�̔ԍ��t���f�[�^����ѐݒ�R�}���h
	void DecodeHomeData();			// �A�Ғn�_�f�[�^
	void DecodeSystemData();		// �V�X�e���̓�����Ԃ�\���f�[�^
	void DecodeErrorData();			// UAV�Ŕ��������G���[�f�[�^
	void DecodeActionPoint();		// �A�N�V�������s�n�_�Ɋւ���f�[�^
	void DecodeActionPointAck();	// �A�N�V�������s�n�_�Ɋւ���f�[�^��ACK�y��n��S/W�ł͎�M���Ȃ��z
	void DecodeMultiWayPointData();	// �}���`�E�F�C�|�C���g�]���f�[�^�y��n��S/W�ł͎�M���Ȃ��z
	void DecodeMultiWayPointAck();	// �}���`�E�F�C�|�C���g�]���ɑ΂��鉞���f�[�^
	void DecodeFirmwareInfo();		// �t�@�[���E�F�A����ʒm���邽�߂̃p�P�b�g
	void DecodePropoData();			// �v���|�f�[�^
	void DecodeEepromData();		// EEPROM�������ݗv���^EEPROM�擾�f�[�^�y��n��S/W�ł͎�M���Ȃ��z
	void DecodeEepromCommand();		// EEPROM����R�}���h�y��n��S/W�ł͎�M���Ȃ��z
	void DecodeDebugData_H();		// ��ʐ���MCU�̃f�o�b�O�̈�̒l
	void DecodeDebugData_L();		// ���ʐ���MCU�̃f�o�b�O�̈�̒l
	void DecodeGeneralPurposeData();// �ėp�f�[�^

	// �A�N�V�����^�C�v�̕�������擾
	std::string GetActionString(_UBYTE type) const;

	// �f�[�^���M
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

	// UAV����̃R�}���h�Ή�
	BOOL ProcCmdSleep();
	BOOL ProcCmdReqTime();

	// �d�E���x��XML�f�[�^�Ǘ����ɓo�^����
	BOOL RegisterRssi(_UBYTE rssi);

	// ���[�e�B���e�B
	std::vector<_bstr_t> GetXmlPath(_bstr_t path1, _bstr_t path2 = L"", _bstr_t path3 = L"");	// XML�p�X�𐶐�
};

