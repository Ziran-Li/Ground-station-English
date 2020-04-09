///
/// @file       MultiwaypointData.h
///
/// @brief      �}���`�E�F�C�|�C���g�֘A�f�[�^��`
///
/// �}���`�E�F�C�|�C���g��s���s���ۂɈ����f�[�^�̒�`
///


#ifndef INCLUDED_MULTIWAYPOINTDATA_H
#define INCLUDED_MULTIWAYPOINTDATA_H

#include "Common/StandardDef.h"

///
/// @brief �E�F�C�|�C���g���B/�o�����ɃR�[���o�b�N����֐��̒�`
///
typedef void (*WaypointCallbackFunc)(void* pUser, int WaypointIndex, _UDWORD func_id);

///
/// @brief �}���`�E�F�C�|�C���g�v�f�f�[�^
///
struct MultiWaypointElement
{
	_UDWORD WaypointID; //!< �d����M��h�����߂�ID�D�������CWaypointID = 0 �̃p�P�b�g����M������E�F�C�|�C���g���X�g���N���A����
	double  RefLat;     //!< �E�F�C�|�C���g�̈ܓx
	double  RefLon;     //!< �E�F�C�|�C���g�̌o�x
	float   RefAlt;     //!< �E�F�C�|�C���g�̍��x
	float   RefYaw;     //!< �E�F�C�|�C���g���B���̕���
	float   speed;      //!< �E�F�C�|�C���g�֌������ۂ̔�s���x
	float   wait;       //!< �E�F�C�|�C���g�ł̑ҋ@����
	float   accuracy;   //!< �E�F�C�|�C���g���B����~���a
	_UDWORD func_id;    //!< �R�}���hID

	MultiWaypointElement() : WaypointID(0), RefLat(0.0), RefLon(0.0), RefAlt(0.0), RefYaw(0.0), speed(0.0), wait(0.0), accuracy(0.0), func_id(0) {}
	MultiWaypointElement(const ::MultiWaypointElement& obj) : WaypointID(obj.WaypointID), RefLat(obj.RefLat), RefLon(obj.RefLon), RefAlt(obj.RefAlt), RefYaw(obj.RefYaw),
		speed(obj.speed), wait(obj.wait), accuracy(obj.accuracy), func_id(obj.func_id) {}
};



#endif