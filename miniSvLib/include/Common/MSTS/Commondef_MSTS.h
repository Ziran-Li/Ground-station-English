///
/// @file       Commondef_MSTS.h
///
/// @brief      �~�j�T�[�x�C���[���g�[�^���X�e�[�V�����V�X�e�����ʒ�`
///
/// Higher-Level MCU (C++) �� MSTS Monitor �ŋ��L�����`
///

#ifndef INCLUDED_COMMONDEF_MSTS_H
#define INCLUDED_COMMONDEF_MSTS_H

#include "Common/StandardDef.h"

namespace MSTS
{
	enum
	{
		PACKETSIZE_CONSTANT = 5,
		PACKETSIZE_USER1    = 33 + PACKETSIZE_CONSTANT,
	};

	///
	/// @brief  MSTS Monitor ���� UAV �ɑ���f�[�^�̍\����
	///
	struct GcsData
	{
		double Distance;     //!< �v������ [m]
		double Pitch;        //!< �g�[�^���X�e�[�V�����̋p [rad] 
		double Yaw;          //!< �g�[�^���X�e�[�V�����̕��� [rad]�i���C���ʁj
		double Latitude;     //!< �ܓx [deg]
		double Longitude;    //!< �o�x [deg]
		double Altitude;     //!< ���x [m]
		double DistanceRate; //!< �v�������̎��ԕω����x [m/s]
		double PitchRate;    //!< �s�b�`�p�̎��ԕω����x [rad/s]
		double YawRate;      //!< ���[�p�̎��ԕω����x [rad/s]

		_UWORD Status;
	};

	///
	/// @brief  �t���O�iGCS->UAV)
	///
	enum GCSSTATUS
	{
		STATUS_TRACKING_OK         = 0x0001,
		STATUS_GCS_GPS_ACCURACY_OK = 0x0002,
	};

} // end of namespace MSTS

#endif