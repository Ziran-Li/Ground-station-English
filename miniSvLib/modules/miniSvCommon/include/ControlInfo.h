//
//		ControlInfo.h
//
//		������i�[�p�N���X�̒�`
//
//		'09/01/17	Ver 0.01	�v���O���~���O�J�n
//
//		X-UFO �g�ݍ��ݗp�� ControlInfo �Ƃ͕ʕ��Ȃ̂Œ��ӁI
//
//

#ifndef INCLUDED_CONTROLINFO_H
#define INCLUDED_CONTROLINFO_H

#include <windows.h>
//#include "Core/Common/Globaldef.h"

#ifdef CPP_CLI
#undef INCLUDED_GLOBALDEF_H
#include "Globaldef.h"
#else
#include "Globaldef.h"
#endif

//*******************************************************************************
//	C++, C++/CLI ���Ή��ɂ��邽�߂̃v���v���Z�b�T��`
//
//	Note:
//		�{�w�b�_�t�@�C�����C���N���[�h����O��
//
//		#define CPP_CLI
//
//		�Ƃ���ƁC�N���X�E�\���̒�`��C++/CLI�p�i�}�l�[�W�h�R�[�h�j�ɂȂ�܂�
//
//		__CLASS__, __STRUCT__ �Œ�`�����N���X/�\���͕̂K���R���X�g���N�^��
//		�N���X��(const ::�N���X��& obj) �̌`���̃R�s�[�R���X�g���N�^��L����
//		���Ȃ���΂Ȃ�܂���D��҂̃R�[�h�� C++ �̃R�[�h�ł͒P�Ȃ�R�s�[�R���X�g
//		���N�^�ƂȂ�܂��DC++/CLI �ł� C++ ��`�̃C���X�^���X���� C++/CLI 
//		��`�̃C���X�^���X�𐶐���������t���R���X�g���N�^�ƂȂ�܂��D
//		�O���[�o�����O��Ԃ���̎Q�Ǝq"::"�͖Y�ꂸ�ɕt���Ă��������D
//*******************************************************************************
#ifdef CPP_CLI

// C++/CLI ��
#define __CLASS     public ref class  // C++/CLI �N���X��`
#define __STRUCT    public ref struct // C++/CLI �\���̒�`
#define __ENUM      enum class        // C++/CLI �񋓑̒�`�i�N���X���j
#define __CH(TYPE)  TYPE^             // �N���X�n���h��
#define __CHR(TYPE) TYPE^
#define __IND(PTR)  PTR
#define __GCN       gcnew             // �C���X�^���X���L�[���[�h gcnew

#else

// C++ ��
#define __CLASS     class             // �ʏ�̃N���X��`
#define __STRUCT    struct            // �ʏ�̍\���̒�`
#define __ENUM      enum              // �ʏ�̗񋓑̒�`
#define __CH(TYPE)  TYPE              // ���̂܂܌^��Ԃ� �iC++/CLI�łł� ^ ���^���̌��ɕt�������j
#define __CHR(TYPE) TYPE&
#define __IND(PTR)  *(PTR)
#define __GCN                         // �󕶎��ɒu��     �iC++/CLI�łł� gcnew �ɂȂ�j

#endif

__STRUCT SYSTEMTIME_T
{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;

	SYSTEMTIME_T() : wYear(0), wMonth(0), wDayOfWeek(0), wDay(0), wHour(0), wMinute(0), wSecond(0), wMilliseconds(0) {}
	SYSTEMTIME_T(const ::SYSTEMTIME_T& obj) : wYear(obj.wYear), wMonth(obj.wMonth), wDayOfWeek(obj.wDayOfWeek), wHour(obj.wHour), wMinute(obj.wMinute), wSecond(obj.wSecond),
		wMilliseconds(obj.wMilliseconds) {}
	SYSTEMTIME_T(const ::SYSTEMTIME& obj) : wYear(obj.wYear), wMonth(obj.wMonth), wDayOfWeek(obj.wDayOfWeek), wHour(obj.wHour), wMinute(obj.wMinute), wSecond(obj.wSecond),
		wMilliseconds(obj.wMilliseconds) {}
	__CHR(SYSTEMTIME_T) operator = (const ::SYSTEMTIME& obj)
	{
		wYear         = obj.wYear;
		wMonth        = obj.wMonth;
		wDayOfWeek    = obj.wDayOfWeek;
		wDay          = obj.wDay;
		wHour         = obj.wHour;
		wMinute       = obj.wMinute;
		wSecond       = obj.wSecond;
		wMilliseconds = obj.wMilliseconds;
		return __IND(this);
	}
};

//*******************************************************************************
//	�@�̃f�[�^
//*******************************************************************************
__CLASS CMavState
{
public:

	__ENUM CONST_VALUES {
		LENGTH_MAVNAME   = 32,
		SIZE_GENERALAREA = 32,
	};

public:

	// �@�̖�
#ifdef CPP_CLI
	array<WCHAR>^ m_MavName;
#else
	WCHAR m_MavName[LENGTH_MAVNAME];
#endif

	// ���䃂�[�h
	CONTROLMODE m_nControlMode;
	
	// ��s���[�h
	FLIGHTMODE  m_nFlightMode;

	// ���ʐ���pMCU�Ɋւ���f�[�^
#ifdef CPP_CLI
	
#else
	LowLevelMCUData m_LowLevelMCUData;
#endif

	// �V�X�e�����S�x�f�[�^
#ifndef CPP_CLI
	SystemHealthData m_HealthData;
#endif

	// �t�@���N�V�������s�n�_�f�[�^
#ifndef CPP_CLI
	ActionPoint m_ActionPoint;
#endif

	// �e��Z���T�f�[�^
	__CH(IMUData) m_IMUData;
	__CH(GPSData) m_GPSData;
	__CH(GPSInfo) m_GPSInfo;
	__CH(NAVData) m_NAVData;
	__CH(RefData) m_RefData;

	// ���W�f�[�^
	__CH(PosData) m_OrgData;     // GPS���_���
	__CH(LocalPos) m_LocalPos;    // GPS���_����Ƃ���Ǐ����W
	
	// �}���`�E�F�C�|�C���g�Ō���f�[�^
	__CH(MultiWaypointElement) m_MultiWaypointEnd;

	// �@�̃f�[�^
	__CH(SysData) m_SysData;

	// �ėp�I�Ɏg����L�^�G���A
#ifdef CPP_CLI
	array<long long>^ m_UndefinedInt;
	array<double>^    m_UndefinedReal;
#else
	long long m_UndefinedInt [SIZE_GENERALAREA]; // ���e���K�肵�Ă��Ȃ������f�[�^�iMAV�͂ǂ�ȃf�[�^�𑗂��Ă��悢�D
	double    m_UndefinedReal[SIZE_GENERALAREA]; // ���e���K�肵�Ă��Ȃ������f�[�^  �������Ƀf�[�^�̓��e���K�肷��D�j
#endif

	// PPM������̓f�[�^
	__CH(PropoData) m_InputData;

	// �f�o�b�O�f�[�^
#ifndef CPP_CLI
	__CH(CQUADWORD) m_Debug;
#endif

public:
	CMavState() :
		m_nControlMode(CONTROLMODE_UNDEFINED),
		m_nFlightMode(FLIGHTMODE_UNDEFINED),
		m_IMUData(__GCN IMUData()),
		m_GPSData(__GCN GPSData()),
		m_GPSInfo(__GCN GPSInfo()),
		m_NAVData(__GCN NAVData()),
		m_RefData(__GCN RefData()),
		m_OrgData(__GCN PosData()),
		m_LocalPos(__GCN LocalPos()),
		m_MultiWaypointEnd(__GCN MultiWaypointElement()),
		m_SysData(__GCN SysData()),
		m_InputData(__GCN PropoData())
	{
#ifndef CPP_CLI
		ZeroMemory(m_MavName,       sizeof(m_MavName));
		ZeroMemory(m_UndefinedInt,  sizeof(m_UndefinedInt));
		ZeroMemory(m_UndefinedReal, sizeof(m_UndefinedReal));
#else
		m_MavName       = gcnew array<WCHAR>((int)CONST_VALUES::LENGTH_MAVNAME);
		m_UndefinedInt  = gcnew array<long long>((int)CONST_VALUES::SIZE_GENERALAREA);
		m_UndefinedReal = gcnew array<double>   ((int)CONST_VALUES::SIZE_GENERALAREA);
#endif
	}

	CMavState(const ::CMavState& obj) :
		m_nControlMode((CONTROLMODE)obj.m_nControlMode),
		m_nFlightMode((FLIGHTMODE)obj.m_nFlightMode),
		m_IMUData(__GCN IMUData(obj.m_IMUData)),
		m_GPSData(__GCN GPSData(obj.m_GPSData)),
		m_GPSInfo(__GCN GPSInfo(obj.m_GPSInfo)),
		m_NAVData(__GCN NAVData(obj.m_NAVData)),
		m_RefData(__GCN RefData(obj.m_RefData)),
		m_OrgData(__GCN PosData(obj.m_OrgData)),
		m_LocalPos(__GCN LocalPos(obj.m_LocalPos)),
		m_MultiWaypointEnd(__GCN MultiWaypointElement(obj.m_MultiWaypointEnd)),
		m_SysData(__GCN SysData(obj.m_SysData)),
		m_InputData(__GCN PropoData(obj.m_InputData))
	{
#ifndef CPP_CLI
		CopyMemory(m_MavName,       obj.m_MavName,       sizeof(m_MavName));
		CopyMemory(m_UndefinedInt,  obj.m_UndefinedInt,  sizeof(m_UndefinedInt));
		CopyMemory(m_UndefinedReal, obj.m_UndefinedReal, sizeof(m_UndefinedReal));
		m_LowLevelMCUData = obj.m_LowLevelMCUData;
		m_ActionPoint = obj.m_ActionPoint;
		m_Debug = obj.m_Debug;
#else
		m_MavName = gcnew array<WCHAR>((int)CONST_VALUES::LENGTH_MAVNAME);
		for( int i = 0; i < (int)CONST_VALUES::LENGTH_MAVNAME; i++ ) {
			m_MavName[i] = obj.m_MavName[i];
		}

		m_UndefinedInt  = gcnew array<long long>((int)CONST_VALUES::SIZE_GENERALAREA);
		m_UndefinedReal = gcnew array<double>   ((int)CONST_VALUES::SIZE_GENERALAREA);
		for( int i = 0; i < (int)CONST_VALUES::SIZE_GENERALAREA; i++ ) {
			m_UndefinedInt[i]  = obj.m_UndefinedInt[i];
			m_UndefinedReal[i] = obj.m_UndefinedReal[i];
		}
#endif
	}
};

//*******************************************************************************
//	�n��ǃf�[�^
//*******************************************************************************
__CLASS CGcsState
{

public:
	// �V�X�e���^�C��
	__CH(SYSTEMTIME_T)   m_SystemTime;

	// �v���|�f�[�^
	__CH(PropoData)      m_PropoData;
	LONG                 m_Volume;    // �b��D���̂��� PropoData �Ɠ�������

	// �C���v�f�[�^
	__CH(AnemometerData) m_AnemometerData;

	// �ʐM���
	UINT      m_Tx_bps;          // ���݂̓]���g���t�B�b�N
	UINT      m_Rx_bps;          // ���݂̎�M�g���t�B�b�N
	double    m_Ping_Reply_Rate; // PING �R�}���h�̕ԐM��

public:
	CGcsState() :
		m_PropoData(__GCN PropoData()), 
		m_Volume(0), 
		m_Tx_bps(0), 
		m_Rx_bps(0),
		m_Ping_Reply_Rate(0.0)
	{}

	CGcsState(const ::CGcsState& obj) : 
		m_SystemTime(__GCN SYSTEMTIME_T(obj.m_SystemTime)), 
		m_PropoData(__GCN PropoData(obj.m_PropoData)),  
		m_Volume(obj.m_Volume),
		m_AnemometerData(__GCN AnemometerData(obj.m_AnemometerData)),
		m_Tx_bps(obj.m_Tx_bps), 
		m_Rx_bps(obj.m_Rx_bps),
		m_Ping_Reply_Rate(obj.m_Ping_Reply_Rate)
	{}
};

#undef __CLASS
#undef __STRUCT
#undef __ENUM
#undef __CH
#undef __CHR
#undef __IND
#undef __GCN

#endif