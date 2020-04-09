
// UAVSim.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "UAVSim.h"
#include "UAVSimDlg.h"
#include "MMTimer.h"
#include "Codec.h"
#include "common/StandardDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUAVSimApp

BEGIN_MESSAGE_MAP(CUAVSimApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUAVSimApp �R���X�g���N�V����

CUAVSimApp::CUAVSimApp()
{
	// �ċN���}�l�[�W���[���T�|�[�g���܂�
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� CUAVSimApp �I�u�W�F�N�g�ł��B

CUAVSimApp theApp;


// CUAVSimApp ������

BOOL CUAVSimApp::InitInstance()
{
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// �_�C�A���O�ɃV�F�� �c���[ �r���[�܂��̓V�F�� ���X�g �r���[ �R���g���[����
	// �܂܂�Ă���ꍇ�ɃV�F�� �}�l�[�W���[���쐬���܂��B
	CShellManager *pShellManager = new CShellManager;

	// MFC �R���g���[���Ńe�[�}��L���ɂ��邽�߂ɁA"Windows �l�C�e�B�u" �̃r�W���A�� �}�l�[�W���[���A�N�e�B�u��
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// TODO: ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
	SetRegistryKey(_T("�A�v���P�[�V���� �E�B�U�[�h�Ő������ꂽ���[�J�� �A�v���P�[�V����"));

	// �X���b�h�����iWaitForSingleObject�j
	m_pWorkThread = AfxBeginThread(ThreadFunc, (LPVOID)this, 0, 0, CREATE_SUSPENDED); // ��~��Ԃ̃X���b�h�𐶐�
	m_pWorkThread->m_bAutoDelete = FALSE; // �X���b�h�I�����̎����J����}�~

	CUAVSimDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �_�C�A���O�� <OK> �ŏ����ꂽ���̃R�[�h��
		//  �L�q���Ă��������B
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �_�C�A���O�� <�L�����Z��> �ŏ����ꂽ���̃R�[�h��
		//  �L�q���Ă��������B
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "�x��: �_�C�A���O�̍쐬�Ɏ��s���܂����B�A�v���P�[�V�����͗\�������ɏI�����܂��B\n");
		TRACE(traceAppMsg, 0, "�x��: �_�C�A���O�� MFC �R���g���[�����g�p���Ă���ꍇ�A#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS ���w��ł��܂���B\n");
	}

	// �X���b�h�ɏI�����߂��|�X�g
	m_pWorkThread->PostThreadMessageW(WM_QUIT, 0, 0);
	::WaitForSingleObject(m_pWorkThread->m_hThread, INFINITE);
	delete m_pWorkThread; // �X���b�h��j��

	// ��ō쐬���ꂽ�V�F�� �}�l�[�W���[���폜���܂��B
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	//  �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}

//	�o�b�N�O���E���h���������邽�߂̃X���b�h
#define N_LOOPCYCLE 5                         // 10ms ���� nLoopCount ���C���N�������g
#define N_SAMPLING_TIME 20                     // �T���v�����O�^�C��(20ms)

UINT CUAVSimApp::ThreadFunc(LPVOID pParam)
{
	CUAVSimApp* pThis = (CUAVSimApp*)pParam;
	CMMTimer    mmTimer;
	DWORD       tickNext20ms;
	DWORD       tickNext250ms;
	BOOL		flagSendData = FALSE;

	mmTimer.OpenTimer(N_LOOPCYCLE, NULL);
	tickNext20ms = mmTimer.timeGetTime();
	tickNext250ms = mmTimer.timeGetTime();

	// �V���A���|�[�g���I�[�v������
	pThis->OpenWirelessLink();

	while (true) {
		mmTimer.WaitTimeoutEvent();

		volatile DWORD nowtime = mmTimer.timeGetTime();
		BOOL flag20msSync = FALSE;
		BOOL flag250msSync = FALSE;

		// �m���Ɏw�莞�Ԃ� TRUE �ɂȂ�t���O
		if (tickNext20ms <= nowtime) {
			while (tickNext20ms <= nowtime){
				tickNext20ms += N_SAMPLING_TIME;
			}
			flag20msSync = TRUE;
		}
		if (tickNext250ms <= nowtime) {
			while (tickNext250ms <= nowtime){
				tickNext250ms += 250;
			}
			flag250msSync = TRUE;
		}

		// �X���b�h���b�Z�[�W�̃`�F�b�N
		MSG  msg;
		bool flagExit = false;
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			while (GetMessage(&msg, NULL, 0, 0) >= 0) {

				switch (msg.message) {
				case WM_QUIT: // �I������
					flagExit = true;
					break;

				case WM_SEND_TEST_DATA:			// �e�X�g�f�[�^�̑��M
					pThis->SendTestData();
					pThis->AppendLog(_T("�e�X�g�f�[�^���M�i1��j"));
					break;

				case WM_START_SEND_TEST_DATA:	// �e�X�g�f�[�^�̘A�����M�J�n
					flagSendData = TRUE;
					pThis->AppendLog(_T("�e�X�g�f�[�^���M�J�n"));
					break;

				case WM_STOP_SEND_TEST_DATA:	// �e�X�g�f�[�^�̘A�����M��~
					flagSendData = FALSE;
					pThis->AppendLog(_T("�e�X�g�f�[�^���M�I��"));
					break;

				}

				// ���b�Z�[�W���c���ĂȂ������烋�[�v�𔲂���
				if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == 0) {
					break;
				}
			}
		}
		if (flagExit) break; // ���[�v�𔲂���

		// �A�����M
		if (flagSendData && flag250msSync){
			pThis->SendTestData();
		}

		// ��M�m�F
		if (flag20msSync){
			pThis->RecvTestData();
		}

		mmTimer.ResetTimeoutEvent();
	}

	mmTimer.CloseTimer();

	return 0;
}

//	���C���E�B���h�E�̏����������������Ƃ��̏���
void CUAVSimApp::OnMainWndInit(CWnd* pMainWnd)
{
	// ���[�N�X���b�h�̎��s���J�n����
	if (m_pWorkThread) {
		m_pWorkThread->ResumeThread();
	}

	AppendLog(_T("���[�J�X���b�h���s�J�n"));
}

// ���O�o��
BOOL CUAVSimApp::AppendLog(LPCTSTR str)
{
	{
		CExclusiveObject< std::queue<CString> >::ACCESSOR pLogStringQueue = this->m_LogStringQueue.GetAccessor();
		pLogStringQueue->push(str);
	}

	return PostMessageToMainWnd_Safe(WM_APPEND_LOG) == 0;
}

// ���O�L���[���烁�b�Z�[�W���|�b�v����
BOOL CUAVSimApp::PopLog(CString& str)
{
	CExclusiveObject< std::queue<CString> >::ACCESSOR pLogStringQueue = this->m_LogStringQueue.GetAccessor();

	if (pLogStringQueue->empty() == false) {
		str = pLogStringQueue->front();
		pLogStringQueue->pop();

		return TRUE;
	}
	else {
		return FALSE;
	}
}

// ���[�J�X���b�h�Ƀ��b�Z�[�W�𑗂�
BOOL CUAVSimApp::PostMessageToWorkThread(UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pWorkThread->PostThreadMessageW(message, wParam, lParam);
}

// �V���A���|�[�g���I�[�v������
BOOL CUAVSimApp::OpenWirelessLink()
{
	if (m_mainLink.GetCurrentSciPort() != COMPORT_NO) {
		// �ڑ����̃|�[�g�Ɛݒ肪�قȂ�Ƃ�
		m_mainLink.CloseSci();
		return Connect();
	}
	else {
		// COM �|�[�g�ɐڑ����Ă���Ƃ��́C�ؒf���`�F�b�N����
		if (m_mainLink.IsPortEnable() == FALSE) {
			// �ؒf���ꂽ �� �Đڑ������݂�
			AppendLog(_T("MainLink ���ؒf����܂���"));
			return Connect(); // [MLK, SET]
		}

		CString s;
		s.Format(_T("MainLink �֐ڑ��ς݂ł� (COM%d)"), COMPORT_NO);
		AppendLog(s);

		return TRUE;
	}
}

//	�V���A�������N�̐ڑ�
BOOL CUAVSimApp::Connect()
{
	SCIPORT ComPortNo = (SCIPORT)COMPORT_NO;

	if (ISOK(m_mainLink.Initialize(ComPortNo))) {
		// �I�[�v���ɐ���

		// �e�p�P�b�g�̃T�C�Y��o�^(��M�p)
		m_mainLink.RxRegisterFormat(PACKETID_COMMAND, PACKETSIZE_COMMAND);
		m_mainLink.RxRegisterFormat(PACKETID_REPLY, PACKETSIZE_REPLY);
		m_mainLink.RxRegisterFormat(PACKETID_INPUTDATA, PACKETSIZE_INPUTDATA);
		m_mainLink.RxRegisterFormat(PACKETID_INPUTDATA_6R, PACKETSIZE_INPUTDATA_6R);
		m_mainLink.RxRegisterFormat(PACKETID_INPUTDATA_12R, PACKETSIZE_INPUTDATA_12R);
		m_mainLink.RxRegisterFormat(PACKETID_INPUTDATA_MIN, PACKETSIZE_INPUTDATA_MIN);
		m_mainLink.RxRegisterFormat(PACKETID_IMU_PS_DATA, PACKETSIZE_IMU_PS_DATA);
		m_mainLink.RxRegisterFormat(PACKETID_MAGDATA, PACKETSIZE_MAGDATA);
		m_mainLink.RxRegisterFormat(PACKETID_GPSDATA, PACKETSIZE_GPSDATA);
		m_mainLink.RxRegisterFormat(PACKETID_GPSINFO, PACKETSIZE_GPSINFO);
		m_mainLink.RxRegisterFormat(PACKETID_NAVDATA, PACKETSIZE_NAVDATA);
		m_mainLink.RxRegisterFormat(PACKETID_NAVDATA_EX, PACKETSIZE_NAVDATA_EX);
		m_mainLink.RxRegisterFormat(PACKETID_REFDATA, PACKETSIZE_REFDATA);
		m_mainLink.RxRegisterFormat(PACKETID_WAYPOINTDATA, PACKETSIZE_WAYPOINTDATA);
		m_mainLink.RxRegisterFormat(PACKETID_HOMEDATA, PACKETSIZE_HOMEDATA);
		m_mainLink.RxRegisterFormat(PACKETID_SYSTEMDATA, PACKETSIZE_SYSTEMDATA);
		m_mainLink.RxRegisterFormat(PACKETID_ERRORDATA, PACKETSIZE_ERRORDATA);
		m_mainLink.RxRegisterFormat(PACKETID_ACTIONPOINT, PACKETSIZE_ACTIONPOINT);
		m_mainLink.RxRegisterFormat(PACKETID_ACTIONPOINT_ACK, PACKETSIZE_ACTIONPOINT_ACK);
		m_mainLink.RxRegisterFormat(PACKETID_MULTIWAYPOINTDATA, PACKETSIZE_MULTIWAYPOINTDATA);
		m_mainLink.RxRegisterFormat(PACKETID_MULTIWAYPOINT_ACK, PACKETSIZE_MULTIWAYPOINT_ACK);
		m_mainLink.RxRegisterFormat(PACKETID_FIRMWARE_INFO, PACKETSIZE_FIRMWARE_INFO);
		m_mainLink.RxRegisterFormat(PACKETID_PROPODATA, PACKETSIZE_PROPODATA);
		m_mainLink.RxRegisterFormat(PACKETID_EEPROMDATA, PACKETSIZE_EEPROMDATA);
		m_mainLink.RxRegisterFormat(PACKETID_EEPROM_COMMAND, PACKETSIZE_EEPROM_COMMAND);
		m_mainLink.RxRegisterFormat(PACKETID_DEBUGDATA_H, PACKETSIZE_DEBUGDATA_H);
		m_mainLink.RxRegisterFormat(PACKETID_DEBUGDATA_L, PACKETSIZE_DEBUGDATA_L);

		// �R�}���h(GCS�ւ̑��M�f�[�^)�o�b�t�@�̍쐬(���M�p)
		std::vector<BOOL> continuousSend(CMD_BUF_NUM, FALSE);
		m_mainLink.MakeCmdBuf(continuousSend, false);

		CString s;
		s.Format(_T("MainLink �ւ̐ڑ��ɐ��� (COM%d)"), ComPortNo);
		AppendLog(s);

		return TRUE;
	}
	else {
		// ���s
		CString s;
		s.Format(_T("MainLink �ɐڑ��ł��܂��� (COM%d)"), ComPortNo);
		AppendLog(s);

		return FALSE;
	}
}

// �e�X�g�f�[�^�𑗐M����
void CUAVSimApp::SendTestData()
{
	//SendInputData_6R();
	//SendInputDataPacket_12R();
	//SendImuPsDataPacket();
	SendGpsInfo();
	SendNavDataEx();
	SendSystemData();
	SendActionPoint();
	SendCommand(COMMAND_REQUEST_TIME);
	SendCommand(COMMAND_SLEEP);

	// �����p�P�b�g�쐬
	m_mainLink.TxFlush();

	//CString s;
	//s.Format(_T("�e�X�g�f�[�^���M (t=%d)"), ::timeGetTime());
	//AppendLog(s);
}

// �e�X�g�f�[�^����M����
void CUAVSimApp::RecvTestData()
{
	PACKETID PacketID; // ��M�����p�P�b�gID
	int packetCount = 0;	// ��M�����p�P�b�g��(�p�P�b�gID���F���ł�����)

	while (ISOK(m_mainLink.RxBegin(&PacketID))) {
		// �p�P�b�g����M
		switch (PacketID) {
		case PACKETID_COMMAND:
			DecodeCommand();
			break;

		case PACKETID_REPLY:
			DecodeReply();
			break;

		case PACKETID_ACTIONPOINT_ACK:
			DecodeActionPointAck();
			break;

		case PACKETID_WAYPOINTDATA:
			DecodeWayPointData();
			break;

		case PACKETID_MULTIWAYPOINTDATA:
			DecodeMultiWayPointData();
			break;

		default:
			AppendLog(L"��M�i�s���p�P�b�g�j");
			break;
		}

		++packetCount;

		_UBYTE rssi;
		m_mainLink.RxEnd(&rssi);
	}

}

// ���C���E�B���h�E��SendMessage����(�u���b�N����)
LRESULT CUAVSimApp::SendMessageToMainWnd_Safe(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (CWnd* pMainWnd = this->GetMainWnd()) {
		if (pMainWnd->GetSafeHwnd()) {
			return pMainWnd->SendMessage(message, wParam, lParam);
		}
	}

	return 0;
}

// ���C���E�B���h�E��PostMessage����(�u���b�N���Ȃ�)
LRESULT CUAVSimApp::PostMessageToMainWnd_Safe(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (CWnd* pMainWnd = this->GetMainWnd()) {
		if (pMainWnd->GetSafeHwnd()) {
			return pMainWnd->PostMessage(message, wParam, lParam);
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// �e�X�g�p�P�b�g���M
/////////////////////////////////////////////////////////////////////////////

// UAV/GCS�ɑ΂���R�}���h
void CUAVSimApp::SendCommand(UAV_COMMAND cmdId)
{
	static _UWORD cnt = 0x0000;

	switch(cmdId){

	case COMMAND_SLEEP:
		m_mainLink.TxBegin(PACKETID_COMMAND);
		m_mainLink.TxPush<_UWORD>(COMMAND_SLEEP);
		m_mainLink.TxPush<_UBYTE>(0x00);
		m_mainLink.TxPush<_UBYTE>(0x00);
		m_mainLink.TxPush<_UBYTE>(0x00);
		m_mainLink.TxPush<_UBYTE>(0x00);
		m_mainLink.TxEnd(CMD_BUF_CMD_SLEEP);
		break;

	case COMMAND_REQUEST_TIME:
		if(cnt == 0){
			AppendLog(L"���M�iCOMMAND_REQUEST_TIME�j");
			m_mainLink.TxBegin(PACKETID_COMMAND);
			m_mainLink.TxPush<_UWORD>(COMMAND_REQUEST_TIME);
			m_mainLink.TxPush<_UBYTE>(0x00);
			m_mainLink.TxPush<_UBYTE>(0x00);
			m_mainLink.TxPush<_UBYTE>(0x00);
			m_mainLink.TxPush<_UBYTE>(0x00);
			m_mainLink.TxEnd(CMD_BUF_REQ_TIME);
		}
		break;

	default:
		break;
	}

	cnt = (cnt++) % 1000;	// 1000�Ń��Z�b�g
}

// COMMAND_PACKET�ɑ΂���ԓ�
void CUAVSimApp::SendReply()
{

}

// ����n�̐�����̓f�[�^
void CUAVSimApp::SendInputData()
{

}

// 6�̃��[�^�ɑ΂�����̓f�[�^���t����������n�̐�����̓f�[�^
void CUAVSimApp::SendInputData_6R()
{
	static _UWORD cnt = 0x0000;

	m_mainLink.TxBegin(PACKETID_INPUTDATA_6R);
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(520.0+(double)cnt++));
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(521.0));
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(522.0));
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(523.0));
	m_mainLink.TxPush<_UWORD>(400);
	m_mainLink.TxPush<_SWORD>(-500);
	m_mainLink.TxPush<_SWORD>(-600);
	m_mainLink.TxPush<_SWORD>(-700);
	m_mainLink.TxPush<_UWORD>(800);
	m_mainLink.TxPush<_UWORD>(900);
	m_mainLink.TxPush<_UWORD>(1000);
	m_mainLink.TxPush<_UWORD>(2003);
	m_mainLink.TxPush<_UWORD>(1);
	m_mainLink.TxPush<_UWORD>(0);
	m_mainLink.TxEnd(CMD_BUF_INPUTDATA_6R);

	cnt = cnt % 1000;	// 1000�Ń��Z�b�g
}

// 12�̃��[�^�ɑ΂�����̓f�[�^���t����������n�̐�����̓f�[�^
void CUAVSimApp::SendInputData_12R()
{
	static _UWORD cnt = 0x0000;

	m_mainLink.TxBegin(PACKETID_INPUTDATA_12R);
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(520.0+(double)cnt++));
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(521.0));
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(522.0));
	m_mainLink.TxPush(MiniSvCodec::ConvHighLevelInputData(523.0));
	m_mainLink.TxPush<_UWORD>(400);
	m_mainLink.TxPush<_SWORD>(-500);
	m_mainLink.TxPush<_SWORD>(-600);
	m_mainLink.TxPush<_SWORD>(-700);
	m_mainLink.TxPush<_UWORD>(800);
	m_mainLink.TxPush<_UWORD>(900);
	m_mainLink.TxPush<_UWORD>(1000);
	m_mainLink.TxPush<_UWORD>(1100);
	m_mainLink.TxPush<_UWORD>(1200);
	m_mainLink.TxPush<_UWORD>(1300);
	m_mainLink.TxPush<_UWORD>(1400);
	m_mainLink.TxPush<_UWORD>(1500);
	m_mainLink.TxPush<_UWORD>(1600);
	m_mainLink.TxPush<_UWORD>(1700);
	m_mainLink.TxPush<_UWORD>(1800);
	m_mainLink.TxPush<_UWORD>(1900);
	m_mainLink.TxEnd(CMD_BUF_INPUTDATA_12R);

	cnt = cnt % 1000;	// 1000�Ń��Z�b�g
}

// �ŏ����̐���n�̐�����̓f�[�^
void CUAVSimApp::SendInputData_Min()
{

}

// IMU�Z���T�E�C���Z���T�̃f�[�^
void CUAVSimApp::SendImuPsData()
{
	static _UWORD cnt = 0x0000;

	m_mainLink.TxBegin(PACKETID_IMU_PS_DATA);
	m_mainLink.TxPush(MiniSvCodec::Conv1000To2Byte((double)cnt++));
	m_mainLink.TxPush(MiniSvCodec::Conv1000To2Byte(10.0));
	m_mainLink.TxPush(MiniSvCodec::Conv1000To2Byte(20.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(30.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(40.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(50.0));
	m_mainLink.TxPush(MiniSvCodec::Conv1To2Byte(100));
	m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(-60.0));
	m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(-70.0));
	m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(-80.0));
	m_mainLink.TxPush(MiniSvCodec::Conv10000To3Byte(-90.0), 3);
	m_mainLink.TxEnd(CMD_BUF_IMU_PS_DATA);

	cnt = cnt % 10;	// 10�Ń��Z�b�g
}

// IMU�̎���f�[�^
void CUAVSimApp::SendMagData()
{

}

// GPS���f�[�^
void CUAVSimApp::SendGpsData()
{

}

// GPS�̕t�����f�[�^
void CUAVSimApp::SendGpsInfo()
{
	static unsigned long long cnt = 0;
	static int n = 0;

	int freq = 1000 / 250;
	_UBYTE gpsNum[] = {7, 8, 9};
	int time[] = {50, 100, 150};

	m_mainLink.TxBegin(PACKETID_GPSINFO);
	m_mainLink.TxPush(gpsNum[n]);
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(-1.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(-2.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(-3.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(-4.0));
	m_mainLink.TxEnd(CMD_BUF_GPSINFO);

	if(cnt == freq*50){
		n = (++n)%3;
		cnt = 0;
	}
	else{
		++cnt;
	}
}

// �q�@���Z�f�[�^
void CUAVSimApp::SendNavData()
{

}

// �q�@���Z�f�[�^�E�p���f�[�^�t��
void CUAVSimApp::SendNavDataEx()
{
	static unsigned long long cnt = 0;
	static int n = 0;

	//m_mainLink.TxBegin(PACKETID_NAVDATA2);
	//m_mainLink.TxPush<_UQWORD>(MiniSvCodec::ConvDegTo5Byte((double)cnt++));
	//m_mainLink.TxPush<_UQWORD>(MiniSvCodec::ConvDegTo5Byte(10.0));
	//m_mainLink.TxPush<_SDWORD>(MiniSvCodec::Conv10000To3Byte(20.0));
	//m_mainLink.TxPush<_SWORD>(MiniSvCodec::Conv100To2Byte(30.0));
	//m_mainLink.TxPush<_SWORD>(MiniSvCodec::Conv100To2Byte(40.0));
	//m_mainLink.TxPush<_SWORD>(MiniSvCodec::Conv100To2Byte(50.0));
	//m_mainLink.TxPush<_SWORD>(MiniSvCodec::ConvDegTo2Byte(-65.0));
	//m_mainLink.TxPush<_SWORD>(MiniSvCodec::ConvDegTo2Byte(-75.0));
	//m_mainLink.TxPush<_SWORD>(MiniSvCodec::ConvDegTo2Byte(-85.0));
	//m_mainLink.TxEnd();

	//cnt = cnt % 100;	// 100�Ń��Z�b�g

	int freq = 1000 / 250;
	double lat0[] = {35.6309798, 35.630887, 35.6301937, 35.6309798};
	double lon0[] = {140.1031789, 140.1011968, 140.1021838, 140.1031789};
	double alt0[] = {50, 40, 60, 50};
	double roll0[] = {0, 0, 0, 90};
	double pitch0[] = {0, 0, 90, 90};
	double yaw0[] = {0, 90, 90, 90};
	int time[] = {0, 50, 100, 150};
	double lat;
	double lon;
	double alt;
	double roll;
	double pitch;
	double yaw;

	// ���`���
	lat = (lat0[n+1] - lat0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + lat0[n];
	lon = (lon0[n+1] - lon0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + lon0[n];
	alt = (alt0[n+1] - alt0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + alt0[n];
	roll = (roll0[n+1] - roll0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + roll0[n];
	pitch = (pitch0[n+1] - pitch0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + pitch0[n];
	yaw = (yaw0[n+1] - yaw0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + yaw0[n];

	m_mainLink.TxBegin(PACKETID_NAVDATA_EX);
	m_mainLink.TxPush<_SQWORD>(MiniSvCodec::ConvDegTo5Byte(lat), 5);
	m_mainLink.TxPush<_SQWORD>(MiniSvCodec::ConvDegTo5Byte(lon), 5);
	m_mainLink.TxPush<_SDWORD>(MiniSvCodec::Conv10000To3Byte(alt), 3);
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(10.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(20.0));
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(30.0));
	m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(roll));
	m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(pitch));
	m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(yaw));
	m_mainLink.TxEnd(CMD_BUF_NAVDATA_EX);

	if(cnt == freq*50){
		n = (++n)%3;
		cnt = 0;
	}
	else{
		++cnt;
	}
}

// ����ڕW�l�̃f�[�^
void CUAVSimApp::SendRefData()
{

}

// �E�F�C�|�C���g�̃f�[�^����ѐݒ�R�}���h
void CUAVSimApp::SendWayPointData(_SQWORD lat, _SQWORD lon, _SDWORD alt, _SWORD yaw, _UBYTE spd)
{
	// ��M�����f�[�^�Ɠ����f�[�^��Ԃ�
	m_mainLink.TxBegin(PACKETID_WAYPOINTDATA);
	m_mainLink.TxPush(lat, 5);
	m_mainLink.TxPush(lon, 5);
	m_mainLink.TxPush(alt, 3);
	m_mainLink.TxPush(yaw);
	m_mainLink.TxPush(spd);
	m_mainLink.TxEnd(CMD_BUF_WAYPOINTDATA);
}

// �A�Ғn�_�f�[�^
void CUAVSimApp::SendHomeData()
{

}

// �V�X�e���̓�����Ԃ�\���f�[�^
void CUAVSimApp::SendSystemData()
{
	static unsigned long long cnt = 0;
	static int n = 0;

	_UBYTE mode[] = {0x10, 0x11, 0x22, 0x10};

	int freq = 1000 / 250;	

	m_mainLink.TxBegin(PACKETID_SYSTEMDATA);
	m_mainLink.TxPush<_UBYTE>(0xFF);	// �N�����[�h�E�쓮���[�h
	m_mainLink.TxPush<_UBYTE>(mode[n]);	// ���䃂�[�h�E�������[�h
	m_mainLink.TxPush<_UDWORD>(0xFFFFFFFF);	// 
	m_mainLink.TxPush<_UDWORD>(0xFFFFFFFF);	// 
	m_mainLink.TxPush<_UBYTE>(0x0F);	// 
	m_mainLink.TxPush<_UBYTE>(0x0F);	// 
	m_mainLink.TxPush(MiniSvCodec::Conv100To2Byte(10.1));	// �Z�����ϓd���d��
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	// 
	m_mainLink.TxPush<_UBYTE>(0xFF);	//
	m_mainLink.TxEnd(CMD_BUF_SYSTEMDATA);

	if(cnt == freq*50){
		n = (++n)%3;
		cnt = 0;
	}
	else{
		++cnt;
	}
}

// UAV�Ŕ��������G���[�f�[�^
void CUAVSimApp::SendErrorData()
{

}

// �A�N�V�������s�n�_�Ɋւ���f�[�^
void CUAVSimApp::SendActionPoint()
{
	static unsigned long long cnt = 0;
	static int n = 0;

	int freq = 1000 / 250;
	double lat0[] = {35.6309798, 35.630887, 35.6301937, 35.6309798};
	double lon0[] = {140.1031789, 140.1011968, 140.1021838, 140.1031789};
	double alt0[] = {50, 40, 60, 50};
	double roll0[] = {0, 0, 0, 90};
	double pitch0[] = {0, 0, 90, 90};
	double yaw0[] = {0, 90, 90, 90};
	int time[] = {0, 50, 100, 150};
	double lat;
	double lon;
	double alt;
	double roll;
	double pitch;
	double yaw;

	// ���`���
	lat = (lat0[n+1] - lat0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + lat0[n];
	lon = (lon0[n+1] - lon0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + lon0[n];
	alt = (alt0[n+1] - alt0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + alt0[n];
	roll = (roll0[n+1] - roll0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + roll0[n];
	pitch = (pitch0[n+1] - pitch0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + pitch0[n];
	yaw = (yaw0[n+1] - yaw0[n]) / (time[n+1] - time[n]) * ((double)cnt/freq) + yaw0[n];

	if(cnt == freq*50){
		m_mainLink.TxBegin(PACKETID_ACTIONPOINT);
		m_mainLink.TxPush<_UBYTE>(0x01);	// �A�N�V�����^�C�v "Shot"
		m_mainLink.TxPush<_UWORD>(n);	
		m_mainLink.TxPush<_SQWORD>(MiniSvCodec::ConvDegTo5Byte(lat), 5);
		m_mainLink.TxPush<_SQWORD>(MiniSvCodec::ConvDegTo5Byte(lon), 5);
		m_mainLink.TxPush<_SDWORD>(MiniSvCodec::Conv10000To3Byte(alt), 3);
		m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(roll));
		m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(pitch));
		m_mainLink.TxPush(MiniSvCodec::ConvDegTo2Byte(yaw));
		m_mainLink.TxPush<_UDWORD>(0x00000000);
		m_mainLink.TxEnd(CMD_BUF_ACTIONPOINT);

		n = (++n)%3;
		cnt = 0;
	}
	else{
		++cnt;
	}
}

// �A�N�V�������s�n�_�Ɋւ���f�[�^��ACK�y��n��S/W�ł͎�M���Ȃ��z
void CUAVSimApp::SendActionPointAck()
{

}

// �}���`�E�F�C�|�C���g�]���ɑ΂��鉞���f�[�^
void CUAVSimApp::SendMultiWayPointAck()
{

}

// �t�@�[���E�F�A����ʒm���邽�߂̃p�P�b�g
void CUAVSimApp::SendFirmwareInfo()
{

}

// �v���|�f�[�^
void CUAVSimApp::SendPropoData()
{

}

// EEPROM�������ݗv���^EEPROM�擾�f�[�^�y��n��S/W�ł͎�M���Ȃ��z
void CUAVSimApp::SendEepromData()
{

}

// EEPROM����R�}���h�y��n��S/W�ł͎�M���Ȃ��z
void CUAVSimApp::SendEepromCommand()
{

}

// ��ʐ���MCU�̃f�o�b�O�̈�̒l
void CUAVSimApp::SendDebugData_H()
{

}

// ���ʐ���MCU�̃f�o�b�O�̈�̒l
void CUAVSimApp::SendDebugData_L()
{

}

//*******************************************************************************
//	�f�R�[�h�iUAV/GCS�ɑ΂���R�}���h�j
//*******************************************************************************
void CUAVSimApp::DecodeCommand()
{
	_UBYTE ub;
	_UWORD uw;

	m_mainLink.RxPop(&uw);
	m_mainLink.RxPop(&ub);
	m_mainLink.RxPop(&ub);
	m_mainLink.RxPop(&ub);
	m_mainLink.RxPop(&ub);
}

//*******************************************************************************
//	�f�R�[�h�iCOMMAND_PACKET�ɑ΂���ԓ��j
//*******************************************************************************
void CUAVSimApp::DecodeReply()
{
	_UBYTE param[4];
	_UWORD cmdId;
	_UDWORD dateAndTime;
	_UBYTE year;
	_UBYTE month;
	_UBYTE day;
	_UBYTE hour;
	_UBYTE min;
	_UBYTE sec;
	CString csDateAndTime;

	m_mainLink.RxPop(&cmdId);
	m_mainLink.RxPop(&param[0]);
	m_mainLink.RxPop(&param[1]);
	m_mainLink.RxPop(&param[2]);
	m_mainLink.RxPop(&param[3]);

	switch(cmdId){
	case COMMAND_SLEEP:
		//AppendLog(L"��M�iCOMMAND_SLEEP Reply�j");
		break;

	case COMMAND_REQUEST_TIME:
		AppendLog(L"��M�iCOMMAND_REQUEST_TIME Reply�j");
		memcpy(&dateAndTime, param, sizeof(_UDWORD));
		MiniSvCodec::Dec4ByteToDateAndTime(dateAndTime, &year, &month, &day, &hour, &min, &sec);
		csDateAndTime.Format(L" %04d/%02d/%02d %02d:%02d:%02d", 2000+year, month, day, hour, min, sec);
		AppendLog(csDateAndTime);
		break;
	
	default:
		break;
	}
}

//*******************************************************************************
//	�f�R�[�h�i�A�N�V�������s�n�_�f�[�^��ACK�j
//*******************************************************************************
void CUAVSimApp::DecodeActionPointAck()
{
	_UWORD uw;
	_UBYTE ub;
	CString s;

	AppendLog(L"��M�i�A�N�V�����|�C���g�@ACK�j");

	// �A�N�V�����^�C�v
	m_mainLink.RxPop(&ub);
	s.Format(_T(" �A�N�V�����^�C�v: %02X"), ub); AppendLog(s);

	// �J�E���g
	m_mainLink.RxPop(&uw);
	s.Format(_T(" �J�E���g: %04X"), uw); AppendLog(s);
}

//*******************************************************************************
//	�f�R�[�h�i�E�F�C�|�C���g�̃f�[�^����ѐݒ�R�}���h�j
//*******************************************************************************
void CUAVSimApp::DecodeWayPointData()
{
	_SQWORD lat, lon;
	_SDWORD alt;
	_SWORD yaw;
	_UBYTE spd;
	double val;
	CString s;

	AppendLog(L"��M�i�E�F�C�|�C���g�j");

	// �ܓx
	m_mainLink.RxPop(&lat, 5);
	val = MiniSvCodec::Dec5ByteToDeg(lat);
	s.Format(_T(" lat: %f"), val); AppendLog(s);

	// �o�x
	m_mainLink.RxPop(&lon, 5);
	val = MiniSvCodec::Dec5ByteToDeg(lon);
	s.Format(_T(" lon: %f"), val); AppendLog(s);

	// ���x
	m_mainLink.RxPop(&alt, 3);
	val = MiniSvCodec::Dec3ByteTo10000(alt);
	s.Format(_T(" alt: %f"), val); AppendLog(s);

	// ����
	m_mainLink.RxPop(&yaw);
	val = MiniSvCodec::Dec2ByteToDeg(yaw);
	s.Format(_T(" yaw: %f"), val); AppendLog(s);

	// ��s���x
	m_mainLink.RxPop(&spd);
	val = MiniSvCodec::DecAirSpeed(spd);
	s.Format(_T(" spd: %f"), val); AppendLog(s);

	// ���v���C�p�P�b�g����
	SendWayPointData(lat, lon, alt, yaw, spd);
}

//*******************************************************************************
//	�f�R�[�h�i�}���`�E�F�C�|�C���g�]���f�[�^�j
//*******************************************************************************
void CUAVSimApp::DecodeMultiWayPointData()
{
}
