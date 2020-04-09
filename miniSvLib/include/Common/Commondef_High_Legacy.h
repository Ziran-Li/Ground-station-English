//
//		Commondef_High_Legacy.h
//
//		���V�X�e���p�~�j�T�[�x�C���[�v���W�F�N�g���ʒ�`
//

#ifndef INCLUDED_COMMONDEF_HIGH_LEGACY_H
#define INCLUDED_COMMONDEF_HIGH_LEGACY_H

// �v���|�̏��i�v���|�̎d�l�ɑΉ��j
struct PropoData
{
	unsigned int PRoll;  // Aileron      - 1000(  ) �` 2000(    ), Neutral : 1520
	unsigned int PPitch; // Elevator     - 1000(  ) �` 2000(    ), Neutral : 1520
	unsigned int PPower; // Thrust       - 1000(UP) �` 2000(DOWN), Neutral : 1520
	unsigned int PYaw;   // Rudder       - 1000     �` 2000      , Neutral : 1520
	int Sw1, Sw2, Sw3, Sw4; // Switch State - 0:OFF / 1:ON

	PropoData() : PRoll(1520), PPitch(1520), PPower(1520), PYaw(1520), Sw1(0), Sw2(0), Sw3(0), Sw4(0) {}
	PropoData(const ::PropoData& obj) : PRoll(obj.PRoll), PPitch(obj.PPitch), PPower(obj.PPower), PYaw(obj.PYaw), Sw1(obj.Sw1), Sw2(obj.Sw2), Sw3(obj.Sw3), Sw4(obj.Sw4) {}
};

//*******************************************************************************
//	�e��萔�̒�`
//*******************************************************************************

// ���䃂�[�h (0�`7)
enum CONTROLMODE
{
	CONTROLMODE_UNDEFINED         = 0, // ����`���
	CONTROLMODE_MANUAL            = 1, // �}�j���A�����c
	CONTROLMODE_ASSISTEDMANUAL    = 2, // �p���A�V�X�g�}�j���A��
	CONTROLMODE_AUTOMATICCONTROL  = 3, // ��������i�t���C�g���[�h�Ŕ�s�j
	CONTROLMODE_EMERGENCY_LANDING = 6, // �ً}�������[�h
	CONTROLMODE_FAILSAFE          = 7, // �ʐM�t�F�C���Z�[�t
};

// �t���C�g���[�h (0�`31)
enum FLIGHTMODE 
{
	FLIGHTMODE_UNDEFINED          = 0,  // ����`/�s��
	FLIGHTMODE_AUTOMATIC          = 1,  // ���S����
	FLIGHTMODE_VELOCITY_CONTROL   = 2,  // ���x����i�ꕔ�}�j���A���j
	FLIGHTMODE_AUTOMATIC_TAKEOFF  = 3,  // �������� (��AUTOMATIC)
	FLIGHTMODE_AUTOMATIC_LANDING  = 4,  // ��������
	FLIGHTMODE_START_STOP         = 6,  // �v���y���n��/��~ (��NEUTRAL)
	FLIGHTMODE_NEUTRAL            = 7,  // �ҋ@
	FLIGHTMODE_MULTIWAYPOINT      = 8,  // �}���`�E�F�C�|�C���g��s
	FLIGHTMODE_TEST               = 9,  // �e�X�g���[�h
	FLIGHTMODE_OBSTACLE_AVOIDANCE = 20, // ��Q�����
	FLIGHTMODE_USER1              = 27, // ���[�U�[��`1
	FLIGHTMODE_USER2              = 28, // ���[�U�[��`2
	FLIGHTMODE_USER3              = 29, // ���[�U�[��`3
	FLIGHTMODE_USER4              = 30, // ���[�U�[��`4
	FLIGHTMODE_USER5              = 31, // ���[�U�[��`5
};
#define IS_USERFLIGHTMODE(FMODE) ((FMODE) >= (FLIGHTMODE)20) // ���[�U�[�\�񂩂ǂ���

#define FLIGHTMODETEXT_UNKNOWN           "�s��"
#define FLIGHTMODETEXT_UNDEFINED         "����`"
#define FLIGHTMODETEXT_AUTOMATIC         "���S����"
#define FLIGHTMODETEXT_VELOCITY_CONTROL  "���x����"
#define FLIGHTMODETEXT_AUTOMATIC_TAKEOFF "��������"
#define FLIGHTMODETEXT_AUTOMATIC_LANDING "��������"
#define FLIGHTMODETEXT_START_STOP        "�v���y���n���^��~"
#define FLIGHTMODETEXT_NEUTRAL           "�ҋ@"
#define FLIGHTMODETEXT_MULTIWAYPOINT     "�}���`�E�F�C�|�C���g"
#define FLIGHTMODETEXT_TEST              "�J���p"
#define FLIGHTMODETEXT_USER1             "���[�U�[��`1"
#define FLIGHTMODETEXT_USER2             "���[�U�[��`2"
#define FLIGHTMODETEXT_USER3             "���[�U�[��`3"
#define FLIGHTMODETEXT_USER4             "���[�U�[��`4"
#define FLIGHTMODETEXT_USER5             "���[�U�[��`5"

#endif