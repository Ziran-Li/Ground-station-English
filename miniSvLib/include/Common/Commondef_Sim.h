///
///
/// @file       Commondef_Sim.h
///
/// @brief      �~�j�T�[�x�C���[�v���W�F�N�g���ʒ�`
///
///             Lower-Level MCU (C), Higher-Level MCU (C++), MiniSvSim (C++) ����
///
///

#ifndef INCLUDED_COMMONDEF_SIM_H
#define INCLUDED_COMMONDEF_SIM_H

#include "BasicConfiguration.h"
#include <wchar.h>

#ifndef INCLUDED_TYPEDEFINE_H
#	include "CompatibleDefine/typedefine.h"
#endif


///
/// �A�h���X��UDP�|�[�g
///
#define ADDR_WINMINISV    "127.0.0.1"   //!< IP�A�h���X �iWinMinisurveyor�j
#define ADDR_SOLVER       "127.0.0.1"   //!< IP�A�h���X �i�\���o�j
//#define ADDR_GUI          "10.39.22.71" //!< IP�A�h���X �iGUI�j
#define ADDR_GUI          "169.254.98.194" //!< IP�A�h���X �iGUI�j
//#define ADDR_GUI          "127.0.0.1" //!< IP�A�h���X �iGUI�j
#define PORT_LOW2SOLVER   53901         //!< UDP�|�[�g�i����MCU �� �\���o �j
#define PORT_SOLVER2LOW   53902         //!< UDP�|�[�g�i�\���o  �� ����MCU�j
#define PORT_HIGH2SOLVER  53910         //!< UDP�|�[�g�i���MCU �� �\���o �j
#define PORT_SOLVER2HIGH  53907         //!< UDP�|�[�g�i�\���o  �� ���MCU�j
#define PORT_GUI2SOLVER   53908         //!< UDP�|�[�g�iGUI     �� �\���o �j
#define PORT_SOLVER2GUI   53909         //!< UDP�|�[�g�i�\���o  �� GUI    �j


///
/// �p�P�b�g�̃w�b�_�ƃt�b�^
///
#define MINISVSIMPACKET_HEADER 0xAAAA //!< �p�P�b�g���ʃw�b�_
#define MINISVSIMPACKET_FOOTER 0x5555 //!< �p�P�b�g���ʃt�b�^


///
/// �R�}���h�iGUI���\���o�j
///
#define COMMAND_END_SIMULATION  1 //!< �V�~�����[�^���I������

///
/// @brief  ESC�f�[�^�p�P�b�g�i����MCU���\���o�j
///
#pragma pack(1)
typedef struct _tag_EscPacket
{
	uint16_t header;
	double  pwm[MAXIMUM_NUMBER_OF_ROTORS];
	uint16_t footer;

} EscPacket;
#pragma pack()


///
/// @brief  �Z���T�f�[�^�p�P�b�g�i�\���o������MCU�j
///
#pragma pack(1)
typedef struct _tag_GyroPacket
{
	uint16_t header;
	double  wx;
	double  wy;
	double  wz;
	uint16_t footer;

} GyroPacket;
#pragma pack()


///
/// @brief  �Z���T�f�[�^�p�P�b�g�f�[�^�X�V�t���O�i�\���o�����MCU�j
///
typedef enum
{
	MINISVSIM_FLAG_GYRO     = 0x0001, //!< �W���C���Z���T�X�V�t���O
	MINISVSIM_FLAG_ACC      = 0x0002, //!< �����x�Z���T�X�V�t���O
	MINISVSIM_FLAG_MAG      = 0x0004, //!< ���C�Z���T�X�V�t���O
	MINISVSIM_FLAG_ATT      = 0x0008, //!< �p���Z���T�X�V�t���O
	MINISVSIM_FLAG_GPS      = 0x0010, //!< GPS�Z���T�X�V�t���O
	MINISVSIM_FLAG_PS       = 0x0020, //!< �C���Z���T�X�V�t���O
	MINISVSIM_FLAG_GPSINFO  = 0x0040, //!< GPS���X�V�t���O
	MINISVSIM_FLAG_DISTANCE = 0x0080, //!< �����f�[�^�X�V�t���O
	MINISVSIM_FLAG_VOLTAGE  = 0x0100, //!< �d���f�[�^�X�V�t���O

} MINISVSIM_UPDATE_FLAG;


///
/// @brief  �Z���T�f�[�^�p�P�b�g�i�\���o�����MCU�j
///
#pragma pack(1)
typedef struct
{
	_UWORD  header;
	double  wX, wY, wZ;          //!< 3���p���x
	double  aX, aY, aZ;          //!< 3�������x
	double  mX, mY, mZ;          //!< 3������
	double  phi, the, psi;       //!< 3���p��
	double  q0, q1, q2, q3;      //!< �N�H�[�^�j�I��
	double  vN, vE, vD;          //!< GPS���x
	double  lat, lon, alt;       //!< GPS�ʒu
	double  Ps;                  //!< �C�����x
	_UBYTE  NavWorking;          //!< GPS���ʒ�
	_UBYTE  nSats;               //!< GPS�q���⑫��
	double  hPosAcc, vPosAcc;    //!< GPS�ʒu���x
	double  hVelAcc, vVelAcc;    //!< GPS���x���x
	_UBYTE  dist_Layout;         //!< �����Z���T���C�A�E�g
	_UWORD  dist[10];            //!< ��, ��, ����8���� �����f�[�^
	_UWORD  dist_offset[10];     //!< ��, ��, ����8���� �����f�[�^ �I�t�Z�b�g
	_UBYTE  dist_InRange[10];    //!< ��, ��, ����8���� �����f�[�^ �L�������W���t���O
	float   voltage;             //!< �Z�����ϓd��
	_UDWORD flagUpdated;         //!< �X�V�t���O
	float   GpsDelay;            //!< GPS���Ԓx�����
	float   Imu_X, Imu_Y, Imu_Z; //!< IMU�ݒu�ʒu
	float   Mag_Pose;            //!< ���C�Z���T�ݒu�p��
	float   Gps_X, Gps_Y, Gps_Z; //!< GPS�ݒu�ʒu
	float   Ps_X, Ps_Y, Ps_Z;    //!< �C���Z���T�ݒu�ʒu
	float   Cog_X, Cog_Y, Cog_Z; //!< �d�S�ʒu
	_UWORD  footer;

} HighSensorDataPacket;
#pragma pack()


///
/// @brief  ����V�X�e���f�[�^�p�P�b�g�i���MCU���\���o�j
///
#pragma pack(1)
typedef struct
{
	_UWORD header;
	double CtrlT, CtrlR, CtrlP, CtrlY; //!< 4���������
	double RefRoll, RefPitch, RefYaw;  //!< 3���p���ڕW�l
	double RefVelX, RefVelY,  RefVelZ; //!< 3�����x�ڕW�l
	double RefYawRate;                 //!< ���[�p���x�ڕW�l
	double RefX,    RefY,     RefZ;    //!< 3���ʒu�ڕW�l
	_UWORD footer;

} HighMcuControlSystemDataPacket;
#pragma pack()


///
/// @brief  ��GUI��ԃf�[�^�p�P�b�g�i�\���o��GUI�j
///
#pragma pack(1)
typedef struct
{
	_UWORD  header;
	_SDWORD PacketID;                    //!< ID
	float   q0, q1, q2, q3;              //!< �N�H�[�^�j�I��
	float   Wx, Wy, Wz;                  //!< 3���p���x
	float   x,  y,  z;                   //!< 3���ʒu
	float   Vx, Vy, Vz;                  //!< 3�����x
	float   N[MAXIMUM_NUMBER_OF_ROTORS]; //!< ���[�^��]��
    wchar_t Message[64];                 //!< ���b�Z�[�W
    float   GinbalAtt0_Phi;              //!< �W���o���p�x0 ��
    float   GinbalAtt0_The;              //!< �W���o���p�x0 ��
    float   GinbalAtt0_Psi;              //!< �W���o���p�x0 ��
    float   GinbalAtt1_Phi;              //!< �W���o���p�x1 ��
    float   GinbalAtt1_The;              //!< �W���o���p�x1 ��
    float   GinbalAtt1_Psi;              //!< �W���o���p�x1 ��
    float   GinbalAtt2_Phi;              //!< �W���o���p�x2 ��
    float   GinbalAtt2_The;              //!< �W���o���p�x2 ��
    float   GinbalAtt2_Psi;              //!< �W���o���p�x2 ��
    float   GinbalAtt3_Phi;              //!< �W���o���p�x3 ��
    float   GinbalAtt3_The;              //!< �W���o���p�x3 ��
    float   GinbalAtt3_Psi;              //!< �W���o���p�x3 ��
    float   GinbalAtt4_Phi;              //!< �W���o���p�x4 ��
    float   GinbalAtt4_The;              //!< �W���o���p�x4 ��
    float   GinbalAtt4_Psi;              //!< �W���o���p�x4 ��
	_UWORD  footer;
} StateDataPacket;
#pragma pack()
#define STATEDATAPACKET_ID  1


///
/// @brief  �Փ˃f�[�^�A�C�e��
///
#pragma pack(1)
typedef struct
{
	float Xb;  //!< �Փ˃|�C���g(x)
	float Yb;  //!< �Փ˃|�C���g(y)
	float Zb;  //!< �Փ˃|�C���g(z)
	float Vxb; //!< �Փˑ��x (x)
	float Vyb; //!< �Փˑ��x (y)
	float Vzb; //!< �Փˑ��x (z)
	float nx;  //!< �@���x�N�g�� (x)
	float ny;  //!< �@���x�N�g�� (y)
	float nz;  //!< �@���x�N�g�� (z)
	float d;   //!< �߂肱�񂾋���
	float F1;  //!< �Ö��C�W��
	float F2;  //!< �����C�W��

} CollisionPoint;
#pragma pack()


///
/// @brief  ��GUI��ԏՓ˃f�[�^�p�P�b�g�iGUI���\���o�j
///
#pragma pack(1)
typedef struct
{
	_UWORD  header;
	_SDWORD PacketID;       //!< ID
    _UDWORD Command;        //!< �R�}���h
	_UBYTE  nPoint;         //!< �Փ˓_�̐�
	CollisionPoint* pPoint; //!< �Փ˓_�f�[�^�inPoint�̐���������j
	_UBYTE  PropellerCollisionState[MAXIMUM_NUMBER_OF_ROTORS]; //!< �v���y���ɑ΂���Փˏ�ԁi0:���� 255:���S�j���j
	_UWORD  footer;

} CollisionDataPacket;
#pragma pack()
#define COLLISIONDATAPACKET_ID  1

#endif