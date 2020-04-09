//
//	    Globaldef.h
//
//		�\���́E�}�N���E�^��`
//
//		'08/01/06	Ver 1.00	����                                             by S-Suzuki
//		~08/10/08	Ver 1.01	�萔�̒�`�����낢��ƒǉ�                       by iwakura
//		'08/10/16	Ver 1.02	�t���C�g���[�h�ǉ�                               by iwakura
//		'08/10/28	Ver 1.02.1	�f�o�b�O�p�ϐ��ǉ�                               by iwakura
//		'09/01/22	Ver 1.03	�p�P�b�g���ǉ�                                 by iwakura
//		'09/02/27	Ver 1.04	�t���C�g���[�h�ǉ�(USER1~5)                      by iwakura
//		         	        	�p�P�b�gID�ǉ�(USER1~5)
//		'09/03/12	Ver 1.05	MAVInfo�̒ǉ�                                    by iwakura
//		'09/03/15	Ver 1.06	MAVInfo�̃����o�ǉ�                              by iwakura
//		'09/04/02	Ver 1.07	EstData�̒ǉ�
//		'09/09/11	Ver 1.08	�t�@�C���̏ꏊ�ύX�ɔ������C��                   by iwakura
//		         	        	�\���̖��̕ύX(MavData)
//		         	        	�p�P�b�gID�ǉ�(MAV_CONDITION)
//		'09/09/23	Ver 1.09	PosData �ɃR���X�g���N�^�ǉ�                     by iwakura
//		         	        	PosData �ɒl�ݒ�p�̊֐��ǉ�
//		'09/12/21	Ver 1.09.1	�t�@�C�����E�t�@�C���̏ꏊ�ύX�ɔ������C��       by iwakura
//		'10/01/04	Ver 1.10	�p�P�b�gID�ǉ� (PACKETID_ATTREF2, PACKETID_VELREF2, PACKETID_GPSREF2)
//		         	        	                                                 by iwakura
//		'10/01/05	Ver 1.11	PI, EXP, PACKETID_PROPO_SIMPLE ��ǉ�            by iwakura
//		'10/01/30	Ver 1.12	PACKETID_GENERALINT, PACKETID_GENERALREAL ���K�� by iwakura
//		'10/02/07	Ver 1.13	XUFO_COMMAND��MAV_COMMAND �ɉ���                 by iwakura
//								COMMAND_PING ��ǉ�
//		'10/05/20	Ver 1.14	�\���̖��̕ύX(PPmdata -> CtrlData)              by iwakura
//		'10/06/20	Ver 1.15	�\���́EPACKETID�EPACKETSIZE�̖��O�ύX           by iwakura
//								PropoData������������悤�ɂ���
//		'11/02/10	Ver 1.16	gDebug�̏������^�C�~���O��ύX                   by iwakura
//		�����������������������������������������
//			�������牺�̃o�[�W�����̓v���W�F�N�g�̃o�[�W����
//		�����������������������������������������
//		'13/??/??	Ver 2.0.X	�����낢��ƕύX                                 by iwakura
//		'13/06/09	Ver 2.1.0	Data3D�\���̂�ǉ�                               by iwakura
//		'12/07/06	Ver 2.1.1	EulerAngle, Quaternion�\���̂�ǉ�               by iwakura
//		'13/08/15	Ver 3.2.0	Commondef_High �Ɉړ��F
//		         	         	    "LowLevelMCUData"
//		         	         	Commondef_Low �Ɉړ��F
//		         	         	    "QUADWORD"
//
//

#ifndef INCLUDED_GLOBALDEF_H
#define INCLUDED_GLOBALDEF_H

#include "Common/StandardDef.h"

// GPS���
struct GPSInfo 
{
	float  hPosAccuracy; // �����ʈʒu���萸�x [m]
	float  vPosAccuracy; // ���x�ʒu���萸�x   [m]
	float  hVelAccuracy; // �����ʑ��x���萸�x [m]
	float  vVelAccuracy; // ���x���x���萸�x   [m]
	_UBYTE NavWorking;   // GPS�L��(1) or ����(0)
	_UBYTE nSat;         // �L���q����

	GPSInfo() : NavWorking(FALSE), nSat(0), hPosAccuracy(0), vPosAccuracy(0), hVelAccuracy(0), vVelAccuracy(0) {}
	GPSInfo(const ::GPSInfo& obj) : NavWorking(obj.NavWorking), nSat(obj.nSat), hPosAccuracy(obj.hPosAccuracy), vPosAccuracy(obj.vPosAccuracy), 
	                                hVelAccuracy(obj.hVelAccuracy), vVelAccuracy(obj.vVelAccuracy) {}
};

// �ڕW�l�f�[�^
/*
__STRUCT RefData {
	double RefRoll, RefPitch, RefYaw;  // Attitude Reference [rad]
	double RefVelY, RefVelX,  RefVelZ; // Velocity Reference [m/s]
	double RefLat,  RefLon,   RefAlt;  // Position Reference [deg, deg, m]
	
	// �E�F�C�|�C���g��s�p
	double FlightSpeed; // �ő��s���x [m/s]
	double Radius;      // ���B���蔼�a [m]
	
	RefData() : RefRoll(0), RefPitch(0), RefYaw(0), RefVelY(0), RefVelX(0), RefVelZ(0), RefLat(0), RefLon(0), RefAlt(0), FlightSpeed(0), Radius(0) {}
	RefData(const ::RefData& obj) : RefRoll(obj.RefRoll), RefPitch(obj.RefPitch), RefYaw(obj.RefYaw), RefVelY(obj.RefVelY), RefVelX(obj.RefVelX), RefVelZ(obj.RefVelZ),
		RefLat(obj.RefLat), RefLon(obj.RefLon), RefAlt(obj.RefAlt), FlightSpeed(obj.FlightSpeed), Radius(obj.Radius) {}
};
*/

// �C�����󂯂�O�̖ڕW�l�f�[�^
struct RawRefData 
{
	double RefYaw; 
	double RefAlt;
	double RefVelX;
	double RefVelY;
	double RefVelZ;
	
	double RefLat;
	double RefLon;
	
	
	RawRefData() : RefYaw(0), RefAlt(0), RefVelX(0), RefVelY(0), RefVelZ(0) {}
};

// �ʒu�f�[�^
struct PosData 
{
	double lat; // �ܓx [deg]
	double lon; // �o�x [deg]
	double alt; // ���x [m]   �d�͂Ƌt�̕�����

	PosData() : lat(0), lon(0), alt(0) {}
	PosData(double latitude, double longitude, double altitude) : lat(latitude), lon(longitude), alt(altitude) {}
	PosData(const ::PosData& obj) : lat(obj.lat), lon(obj.lon), alt(obj.alt) {}
	
	void SetPosition(double latitude, double longitude, double altitude) { lat = latitude; lon = longitude; alt = altitude; }
};

// XYZ�ʒu�f�[�^
struct XYZData 
{
	double x;
	double y;
	double z;

	XYZData() : x(0), y(0), z(0) {}
	XYZData(const ::XYZData& obj) : x(obj.x), y(obj.y), z(obj.z) {}
};

struct LocalPos : public XYZData
{
	LocalPos() {}
	LocalPos(const ::LocalPos& obj) : XYZData(obj) {}
};

// ����Ώۂւ̓��́i�v���|�̎d�l�ɑΉ��j
struct CtrlData {
	double Roll;  // Aileron      - 1000(  ) �` 2000(    ), Neutral : 1520
	double Pitch; // Elevator     - 1000(  ) �` 2000(    ), Neutral : 1520
	double Yaw;   // Rudder       - 1000     �` 2000      , Neutral : 1520
	double Power; // Thrust       - 1000(UP) �` 2000(DOWN), Neutral : 1520
};

// �ėp��ԃx�N�g��
class StateVector
{
protected:
	size_t  m_Length;
	
	double* m_pX;

public:
	StateVector();
	~StateVector();
	
	CRESULT SetLength(size_t length);                      // �̈�m�ۗp�D
	size_t  GetLength() const;                             // �x�N�g�����̎擾�D
	double* GetPointer(unsigned int index, size_t length); // �|�C���^�̎擾�Dlength ����v���Ȃ��ꍇ NULL ���Ԃ�D
	double* GetPointer(size_t length) { return GetPointer(0, length); }
};

// �e�X�g���[�h�̍\���f�[�^
struct TestModeConfig
{
	enum {
		ENABLED_ROLL_PITCH_CONTROL = 0x01, // �p������ON             ��
		ENABLED_VELOCITY_CONTROL   = 0x02, // ���x�E�p������ON       ���ǂꂩ��̂ݐݒ�\
		ENABLED_POSITION_CONTROL   = 0x03, // �ʒu�E���x�E�p������ON ��
		ENABLED_ALTITUDE_CONTROL   = 0x04, // ���x����ON
		ENABLED_YAW_CONTROL        = 0x08, // ���ʐ���ON
		MASK_HORIZONTAL_CONTROL    = 0x03, // ���[�h�i�p��/���x/�ʒu�j���o�p�}�X�N
	};
	
	_UBYTE Mode;
	inline bool IsRollPitchControlEnabled() { return (Mode & MASK_HORIZONTAL_CONTROL) >= ENABLED_ROLL_PITCH_CONTROL ? true : false; }
	inline bool IsVelocityControlEnabled()  { return (Mode & MASK_HORIZONTAL_CONTROL) >= ENABLED_VELOCITY_CONTROL   ? true : false; }
	inline bool IsPositionControlEnabled()  { return (Mode & MASK_HORIZONTAL_CONTROL) >= ENABLED_POSITION_CONTROL   ? true : false; }
	inline bool IsAltitudeControlEnabled()  { return Mode & ENABLED_ALTITUDE_CONTROL ? true : false; }
	inline bool IsYawControlEnabled()       { return Mode & ENABLED_YAW_CONTROL  ? true : false; }
	
	TestModeConfig() : Mode(0) {}
};

// �V�X�e���f�[�^
struct SysData 
{
	float  BatteryVoltage;     // �o�b�e���[�d��     [V]
	_UBYTE CpuUtilizationRate; // CPU�ғ���          [%]
	_UBYTE reserved1;          // �\��
	_UBYTE reserved2;          // �\��
	_UBYTE reserved3;          // �\��
	size_t HeapSize;           // �q�[�v�̈�T�C�Y
	size_t StackSize;          // �X�^�b�N�̈�T�C�Y
	size_t HeapAllocated;      // �q�[�v�̈�m�ۍς݃T�C�Y
	size_t StackAllocated;     // �X�^�b�N�̈�m�ۍς݃T�C�Y
	
	SysData() : BatteryVoltage(0), CpuUtilizationRate(0), HeapSize(0), StackSize(0), HeapAllocated(0), StackAllocated(0) {}
	SysData(const ::SysData& obj) : BatteryVoltage(obj.BatteryVoltage), CpuUtilizationRate(obj.CpuUtilizationRate), 
		HeapSize(obj.HeapSize), StackSize(obj.StackSize), HeapAllocated(obj.HeapAllocated), StackAllocated(obj.StackAllocated) {}
};

#endif
