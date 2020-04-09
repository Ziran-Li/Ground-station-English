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
#ifndef CPP_CLI
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
#else
pubric ref struct LowLevelMCUData
{
public:
	_UBYTE              NumOfRotors;       // ���[�^��
	CScaledControlData  ScaledControlData; // ������̓f�[�^
	
	CSysState           System;            // �V�X�e����ԃf�[�^

	LowLevelMCUData() : NumOfRotors(0)
	{
	}
	LowLevelMCUData(const ::LowLevelMCUData& obj) {}
};

#endif





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
};

#endif