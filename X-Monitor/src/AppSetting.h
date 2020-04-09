//
//		AppSetting.h
//
//		�ݒ�f�[�^�Ǘ��N���X
//
//		'09/02/15	Ver 0.01	�v���O���~���O�J�n	by iwakura
//
//
//		���FMSXML ���񋟂���|�C���^�^(IXMLDOM...Ptr)�̓X�}�[�g�|�C���^�Ȃ̂Ŏ����ŏ������E�J������܂�
//			C...App �� InitInstance �� AfxOleInit() ��ǉ����Ă��������D
//
//

#ifndef INCLUDED_APPSETTING_H
#define INCLUDED_APPSETTING_H

#include <windows.h>
#include <vector>

//#import "msxml3.dll" named_guids
#import "msxml6.dll" named_guids

class AppSetting
{
public:
	// typedef
	typedef std::vector<int> intvec;

	// --------------------------------------------------
	// �A�v���P�[�V�����֘A�̐ݒ�
	// --------------------------------------------------
	class _ClassApp {
	public:
		CString MonitorIdName; // ���j�^�[���ʎq�i�N���C�A���g�����p�j

	public:
		_ClassApp();

		BOOL TestValue_MonitorIdName(CString& MonitorIdName);

		HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
		HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	} App;

	// --------------------------------------------------
	// �t�@�C���L�^ �֘A�@�\�̐ݒ�
	// --------------------------------------------------
	//class _ClassDataFileRecorder {
	//public:
	//	CGuid LastSelectedFormatID;

	//public:
	//	_ClassDataFileRecorder();

	//	HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//	HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//} DataFileRecorder;

	// --------------------------------------------------
	// �Ď��@�\�̐ݒ�
	// --------------------------------------------------
	//class _ClassWatch {
	//public:
	//	double MinVoltage;         // �o�b�e���[�̍Œ�d��
	//	double MaxVoltage;         // �o�b�e���[�̍ō��d��
	//	double AlertVoltage;       // �A���[�g�d��
	//	BOOL   VoltageAlertEnable; // �x������炷

	//public:
	//	_ClassWatch();
	//	
	//	BOOL   SetValue_Voltage(double MinVoltage, double MaxVoltage, double AlertVoltage, BOOL VoltageAlertEnable);
	//	DWORD  TestValue_Voltage(double MinVoltage, double MaxVoltage, double AlertVoltage, BOOL VoltageAlertEnable);

	//	HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//	HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//} Watch;

	// --------------------------------------------------
	// �V���A���|�[�g�֘A�̐ݒ�
	// --------------------------------------------------
	class _ClassComm {
	public:
		_ClassComm();

		int  MainPort;      // MainLink �p�� COM �|�[�g�ԍ� ( -1 �͖��ݒ� )

		BOOL SubLinkEnable; // SubLink ���g�p����
		int  SubPort;       // SubLink �p�� COM �|�[�g�ԍ� ( -1 �͖��ݒ� )
		
		HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
		HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	} Comm;
	
	// --------------------------------------------------
	// MainLink�̐ݒ�
	// --------------------------------------------------
	//class _ClassMainLink {
	//public:
	//	_ClassMainLink();

	//	BOOL MinimumPropoPacket; // �v���|�̃p�P�b�g�T�C�Y���ŏ��ɂ���

	//	HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//	HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//} MainLink;

	// --------------------------------------------------
	// Winsock �T�[�o�[(XMonitorServer)�̐ݒ�
	// --------------------------------------------------
	class _ClassXMonitorServer {
	public:
		_ClassXMonitorServer();

		BOOL Enabled; // TCP/IP �l�b�g���[�N�ő��̃A�v���P�[�V�����ƒʐM����
		WORD PortNo;  // XMonitorServer���g�p����|�[�g�ԍ�


		BOOL  SetValue(BOOL Enabled, WORD PortNo);
		DWORD TestValue(BOOL Enabled, WORD PortNo);

		HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
		HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	} XMonitorServer;

	//// --------------------------------------------------
	//// USB�v���|�̐ݒ�
	//// --------------------------------------------------
	//class _ClassUSBPropo {
	//public:
	//	_ClassUSBPropo();

	//	BOOL Enabled;                // USB�v���|��L���ɂ���
	//	BOOL Swap_Throttle_Elevator; // �X���b�g���ƃG���x�[�^�����ւ���
	//	BOOL Swap_Aileron_Rudder;    // �G�������ƃ��_�[�����ւ���
	//	int  Trim_Throttle;          // �X���b�g���̃g����
	//	int  Trim_Aileron;           // �G�������̃g����
	//	int  Trim_Elevator;          // �G���x�[�^�̃g����
	//	int  Trim_Rudder;            // ���_�[�̃g����
	//	BOOL Invert_Sw1;             // Sw1�𔽓]����
	//	BOOL Invert_Sw2;             // Sw2�𔽓]����
	//	BOOL Invert_Sw3;             // Sw3�𔽓]����
	//	BOOL Invert_Sw4;             // Sw4�𔽓]����

	//	HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//	HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	//} USBPropo;
	
	// --------------------------------------------------
	// �f�[�^�ۑ�/�Đ��̐ݒ�
	// --------------------------------------------------
	class _ClassFlightLog {
	public:
		_ClassFlightLog();

		std::string FilePath;			// ��s���O�t�@�C���̃p�X
		int RecStopTimeFromDataStop;	// UAV����f�[�^�����Ȃ���Ԃ����b��������ۑ�����߂邩
		int DivideSizeInByte;			// ��s���O�̕����T�C�Y[byte]
		int StoragePeriodInDay;			// ��s���O�̕ۑ�����[day]�i�����폜�p�j

		HRESULT _Load(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
		HRESULT _Save(MSXML2::IXMLDOMDocument2Ptr& pDoc, MSXML2::IXMLDOMNodePtr& pRootNode);
	} FlightLog;


	// �E�B���h�E�֘A�̐ݒ�

private: // �v���C�x�[�g�����o�ϐ�

	HANDLE m_hFile; // �ݒ�t�@�C���̃n���h��

public:
	AppSetting();
	virtual ~AppSetting();

	// �ݒ�f�[�^��ۑ�
	HRESULT Load();
	HRESULT Save();

private: // �v���C�x�[�g�����o�֐�

	// XML�t�@�C�����I�[�v��
	HRESULT _OpenFile();
	HRESULT _CommandLine();
	HRESULT _CreateFile(MSXML2::IXMLDOMDocument2Ptr& pDoc); //��̃t�@�C�����쐬
	HRESULT _LockFile();
	HRESULT _UnlockFile();
};

#endif