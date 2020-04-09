//
//
//		Commondef_High.h
//
//		�~�j�T�[�x�C���[�v���W�F�N�g���ʒ�`
//		Higher-Level MCU (C++), GCS (C++) ����
//
//

#ifndef INCLUDED_COMMONDEF_HIGH_H
#define INCLUDED_COMMONDEF_HIGH_H

#include "Commondef_Low.h"

//**************************************************
//	Constant Values
//**************************************************

// ------------------------------
//	RC Signal Range Definition
// ------------------------------


// ------------------------------
//	System
// ------------------------------


//**************************************************
//	Data type
//**************************************************

// ���ʐ���pMCU�̏��
struct LowLevelMCUData 
{
public:	
	_UBYTE              NumOfRotors;       // ���[�^��
	CScaledControlData  ScaledControlData; // ������̓f�[�^
	
	CSysState           System;            // �V�X�e����ԃf�[�^

	LowLevelMCUData() : NumOfRotors(0)
	{
	}
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

// ����Ώۂւ̓��́i�v���|�̎d�l�ɑΉ��j
struct CtrlData
{
	double Roll;  // Aileron      - 1000(  ) �` 2000(    ), Neutral : 1520
	double Pitch; // Elevator     - 1000(  ) �` 2000(    ), Neutral : 1520
	double Yaw;   // Rudder       - 1000     �` 2000      , Neutral : 1520
	double Power; // Thrust       - 1000(UP) �` 2000(DOWN), Neutral : 1520
};

// �C�����󂯂�O�̖ڕW�l�f�[�^
struct RawRefData
{
	double Lat;     //!< �ܓx
	double Lon;     //!< �o�x
	double Alt;     //!< ���x

	double VelX;    //!< ���x(X)
	double VelY;    //!< ���x(Y)
	double VelZ;    //!< ���x(Z)
	double Yaw;     //!< ����
	double YawRate; //!< �p���x(Z)

	RawRefData() : Lat(0), Lon(0), Alt(0), VelX(0), VelY(0), VelZ(0), Yaw(0), YawRate(0) {}
};

// �ڕW�l�f�[�^
struct RefData 
{
	double RefRoll, RefPitch, RefYaw;  // Attitude Reference [rad]
	double RefVelY, RefVelX,  RefVelZ; // Velocity Reference [m/s]
	double RefLat,  RefLon,   RefAlt;  // Position Reference [deg, deg, m]
	double RefX, RefY;
	
	// �E�F�C�|�C���g��s�p
	double FlightSpeed; // �ő��s���x [m/s]
	double Radius;      // ���B���蔼�a [m]
	
	RefData() : RefRoll(0), RefPitch(0), RefYaw(0), RefVelY(0), RefVelX(0), RefVelZ(0), RefLat(0), RefLon(0), RefAlt(0), FlightSpeed(0), Radius(0) {}
	RefData(const RefData& obj) : RefRoll(obj.RefRoll), RefPitch(obj.RefPitch), RefYaw(obj.RefYaw), RefVelY(obj.RefVelY), RefVelX(obj.RefVelX), RefVelZ(obj.RefVelZ),
		RefLat(obj.RefLat), RefLon(obj.RefLon), RefAlt(obj.RefAlt), FlightSpeed(obj.FlightSpeed), Radius(obj.Radius) {}
};

// �E�F�C�|�C���g�f�[�^
struct Waypoint
{
	double lat; //!< �ܓx [deg]
	double lon; //!< �o�x [deg]
	double alt; //!< ���x [m]
	double yaw; //!< ���� [rad]
	double spd; //!< ��s���x [m/s]

	Waypoint() : lat(0), lon(0), alt(0), yaw(0), spd(0) {}
	Waypoint(double lat, double lon, double alt, double yaw, double spd) : lat(lat), lon(lon), alt(alt), yaw(yaw), spd(spd) {}

	bool operator == (const Waypoint& obj)
	{
		return (lat == obj.lat) && (lon == obj.lon) && (alt == obj.alt) && (yaw == obj.yaw) && (spd == obj.spd);
	}
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

#endif