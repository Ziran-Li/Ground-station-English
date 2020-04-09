
// UAVSim.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��

#include <queue>

#include "UavLink.h"
#include "ExclusiveObject.h"

#define WM_SEND_TEST_DATA			(WM_APP + 101)		// �e�X�g�f�[�^�𑗐M����i1�񂾂��j
#define WM_START_SEND_TEST_DATA		(WM_APP + 102) 		// �e�X�g�f�[�^�̘A�����M�J�n
#define WM_STOP_SEND_TEST_DATA		(WM_APP + 103) 		// �e�X�g�f�[�^�̘A�����M��~

#define COMPORT_NO	(83)	// COM�|�[�g�̔ԍ��@"2" �� "COM2"

// CUAVSimApp: 
// ���̃N���X�̎����ɂ��ẮAUAVSim.cpp ���Q�Ƃ��Ă��������B
//

class CUAVSimApp : public CWinApp
{
private:
	// �R�}���h�o�b�t�@�̒�`�i�R�}���h�𑗂鏇�Ԃɏォ����ׂ�j
	enum CmdBufId{
		CMD_BUF_INPUTDATA_6R	= 0,	
		CMD_BUF_INPUTDATA_12R,	
		CMD_BUF_IMU_PS_DATA,				
		CMD_BUF_GPSINFO,					
		CMD_BUF_NAVDATA_EX,					
		CMD_BUF_WAYPOINTDATA,					
		CMD_BUF_SYSTEMDATA,				
		CMD_BUF_ACTIONPOINT,			
		CMD_BUF_REQ_TIME,			
		CMD_BUF_CMD_SLEEP,				// �Ō��COMMAND_SLEEP
		CMD_BUF_NUM						// �R�}���h�p�o�b�t�@�̗v�f��	
	};

	CWinThread*     m_pWorkThread;	// ���[�J�X���b�h
	CUavLink		m_mainLink;
	CExclusiveObject< std::queue<CString> > m_LogStringQueue;	// WM_APPEND_LOG �p������L���[

public:
	CUAVSimApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()

public:
	// ���C���E�B���h�E�̏�����������������R�[�������
	void OnMainWndInit(CWnd* pMainWnd);   

	// ��ʂɃ��O�o��
	BOOL AppendLog(LPCTSTR str);
	BOOL PopLog(CString& str);

	// ���[�J�X���b�h�Ƀ��b�Z�[�W�𑗂�
	BOOL PostMessageToWorkThread(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

private:
	static UINT ThreadFunc(LPVOID pParam);	// ���[�J�X���b�h
	BOOL OpenWirelessLink();
	BOOL Connect();
	void SendTestData();
	void RecvTestData();

	// ���C���E�B���h�E�Ɉ��S�Ƀ��b�Z�[�W�𑗂�
	LRESULT SendMessageToMainWnd_Safe(UINT message, WPARAM wParam = 0, LPARAM lParam = 0); // �f�b�h���b�N�̊댯�����邽�ߎg�p�ɒ��ӂ���D
	LRESULT PostMessageToMainWnd_Safe(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	// �e�X�g�p�P�b�g���M
	void SendCommand(UAV_COMMAND cmdId = COMMAND_SLEEP);	// UAV/GCS�ɑ΂���R�}���h
	void SendReply();				// COMMAND_PACKET�ɑ΂���ԓ�
	void SendInputData();			// ����n�̐�����̓f�[�^
	void SendInputData_6R();		// 6�̃��[�^�ɑ΂�����̓f�[�^���t����������n�̐�����̓f�[�^
	void SendInputData_12R();		// 12�̃��[�^�ɑ΂�����̓f�[�^���t����������n�̐�����̓f�[�^
	void SendInputData_Min();		// �ŏ����̐���n�̐�����̓f�[�^
	void SendImuPsData();			// IMU�Z���T�E�C���Z���T�̃f�[�^
	void SendMagData();				// IMU�̎���f�[�^
	void SendGpsData();				// GPS���f�[�^
	void SendGpsInfo();				// GPS�̕t�����f�[�^
	void SendNavData();				// �q�@���Z�f�[�^
	void SendNavDataEx();			// �q�@���Z�f�[�^�E�p���f�[�^�t��
	void SendRefData();				// ����ڕW�l�̃f�[�^
	void SendWayPointData(_SQWORD lat, _SQWORD lon, _SDWORD alt, _SWORD yaw, _UBYTE spd);	// �E�F�C�|�C���g�̃f�[�^����ѐݒ�R�}���h
	void SendHomeData();			// �A�Ғn�_�f�[�^
	void SendSystemData();			// �V�X�e���̓�����Ԃ�\���f�[�^
	void SendErrorData();			// UAV�Ŕ��������G���[�f�[�^
	void SendActionPoint();			// �A�N�V�������s�n�_�Ɋւ���f�[�^
	void SendActionPointAck();		// �A�N�V�������s�n�_�Ɋւ���f�[�^��ACK�y��n��S/W�ł͎�M���Ȃ��z
	void SendMultiWayPointData();	// �}���`�E�F�C�|�C���g�]���f�[�^�y��n��S/W�ł͎�M���Ȃ��z
	void SendMultiWayPointAck();	// �}���`�E�F�C�|�C���g�]���ɑ΂��鉞���f�[�^
	void SendFirmwareInfo();		// �t�@�[���E�F�A����ʒm���邽�߂̃p�P�b�g
	void SendPropoData();			// �v���|�f�[�^
	void SendEepromData();			// EEPROM�������ݗv���^EEPROM�擾�f�[�^�y��n��S/W�ł͎�M���Ȃ��z
	void SendEepromCommand();		// EEPROM����R�}���h�y��n��S/W�ł͎�M���Ȃ��z
	void SendDebugData_H();			// ��ʐ���MCU�̃f�o�b�O�̈�̒l
	void SendDebugData_L();			// ���ʐ���MCU�̃f�o�b�O�̈�̒l

	// �f�R�[�h
	void DecodeCommand();			// UAV/GCS�ɑ΂���R�}���h
	void DecodeReply();				// COMMAND_PACKET�ɑ΂���ԓ�
	void DecodeActionPointAck();	// �A�N�V�������s�n�_�f�[�^��ACK
	void DecodeWayPointData();		// �E�F�C�|�C���g�̃f�[�^����ѐݒ�R�}���h
	void DecodeMultiWayPointData();	// �}���`�E�F�C�|�C���g�]���f�[�^
};

extern CUAVSimApp theApp;