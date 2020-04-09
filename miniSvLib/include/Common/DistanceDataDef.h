#ifndef INCLUDED_DISTANCEDATADEF_H
#define INCLUDED_DISTANCEDATADEF_H

enum DISTANCE_DATA_FLAG
{
	FLAG_DOWN        = 0x0001, // ��
	FLAG_UP          = 0x0002, // ��
	FLAG_FRONT       = 0x0004, // �O   4����   0��/ 6����(A)   0��/ 8����   0��
	FLAG_RIGHT       = 0x0008, // �E   4����  90��/ 6����(B)  90��/ 8����  90��
	FLAG_REAR        = 0x0010, // ��   4���� 180��/ 6����(A) 180��/ 8���� 180��
	FLAG_LEFT        = 0x0020, // ��   4���� -90��/ 6����(B) -90��/ 8���� -90��
	FLAG_RIGHT_FRONT = 0x0040, // �E�O 6����(A)   60��/ 6����(B)   30��/ 8����   45��
	FLAG_RIGHT_REAR  = 0x0080, // �E�� 6����(A)  120��/ 6����(B)  150��/ 8����  135��
	FLAG_LEFT_REAR   = 0x0100, // ���� 6����(A) -120��/ 6����(B) -150��/ 8���� -135��
	FLAG_LEFT_FRONT  = 0x0200, // ���O 6����(A)  -60��/ 6����(B)  -30��/ 8����  -45��
};

enum DISTANCE_LAYOUT_MODE
{
	LAYOUT_DISABLED     =   0, // �����Z���T���g�p���Ȃ�
	LAYOUT_4DIRECTION_A =   1, // 4�������C�A�E�g(�O/�E/��/��)
	LAYOUT_4DIRECTION_B =   2, // �\��
	LAYOUT_6DIRECTION_A =   3, // 6�������C�A�E�gA ( 0��/ 60��/ 120��/  180��/ -120��/ -60��)
	LAYOUT_6DIRECTION_B =   4, // 6�������C�A�E�gB (30��/ 90��/ 150��/ -150��/  -90��/ -30��)
	LAYOUT_8DIRECTION_A =   5, // 8�������C�A�E�g  ( 0��/ 45��/  90��/  135��/ -135��/ -45��)
	LAYOUT_8DIRECTION_B =   6, // �\��
	LAYOUT_FREE         = 255, // �\��
};

#define N_MAX_HORIZONTAL_DISTANCE_SENSOR 8

#define INDEX_4A_FRONT       0
#define INDEX_4A_RIGHT       1
#define INDEX_4A_REAR        2
#define INDEX_4A_LEFT        3
#define INDEX_6A_FRONT       0
#define INDEX_6A_RIGHT_FRONT 1
#define INDEX_6A_RIGHT_REAR  2
#define INDEX_6A_REAR        3
#define INDEX_6A_LEFT_REAR   4
#define INDEX_6A_LEFT_FRONT  5
#define INDEX_6B_RIGHT_FRONT 0
#define INDEX_6B_RIGHT_REAR  1
#define INDEX_6B_RIGHT       2
#define INDEX_6B_LEFT_REAR   3
#define INDEX_6B_LEFT_FRONT  4
#define INDEX_6B_LEFT        5
#define INDEX_8A_FRONT       0
#define INDEX_8A_RIGHT_FRONT 1
#define INDEX_8A_RIGHT       2
#define INDEX_8A_RIGHT_REAR  3
#define INDEX_8A_REAR        4
#define INDEX_8A_LEFT_REAR   5
#define INDEX_8A_LEFT        6
#define INDEX_8A_LEFT_FRONT  7

#define DIRECTION_FRONT          (float)( 0.0 * M_PI)
#define DIRECTION_RIGHT          (float)( 0.5 * M_PI)
#define DIRECTION_REAR           (float)( 1.0 * M_PI)
#define DIRECTION_LEFT           (float)(-0.5 * M_PI)
#define DIRECTION_6A_RIGHT_FRONT (float)( 1.0 / 3.0 * M_PI)
#define DIRECTION_6A_RIGHT_REAR  (float)( 2.0 / 3.0 * M_PI)
#define DIRECTION_6A_LEFT_REAR   (float)(-2.0 / 3.0 * M_PI)
#define DIRECTION_6A_LEFT_FRONT  (float)(-1.0 / 3.0 * M_PI)
#define DIRECTION_6B_RIGHT_FRONT (float)( 1.0 / 6.0 * M_PI)
#define DIRECTION_6B_RIGHT_REAR  (float)( 5.0 / 6.0 * M_PI)
#define DIRECTION_6B_LEFT_REAR   (float)(-5.0 / 6.0 * M_PI)
#define DIRECTION_6B_LEFT_FRONT  (float)(-1.0 / 6.0 * M_PI)
#define DIRECTION_8A_RIGHT_FRONT (float)( 1.0 / 4.0 * M_PI)
#define DIRECTION_8A_RIGHT_REAR  (float)( 3.0 / 4.0 * M_PI)
#define DIRECTION_8A_LEFT_REAR   (float)(-3.0 / 4.0 * M_PI)
#define DIRECTION_8A_LEFT_FRONT  (float)(-1.0 / 4.0 * M_PI)

struct ST_DISTANCEDATA
{
	unsigned short flagAvailable; // �f�[�^�����p�\���i�L�������W�O�̂Ƃ��ɃN���A���邩�ǂ����̓Z���T���̔C�Ӂj
	unsigned short flagUpdated;   // �f�[�^������X�V���ꂽ���i����ŏ��ɑS�r�b�g���N���A�����j
	unsigned short flagInRange;   // �Z���T�̗L�������W�����ǂ����i�L�������W���Ȃ�Փˉ�����s���j
	unsigned char  Layout;        // �z�u���[�h�i�ėp�̒����gI/F���g���ꍇ��EEPROM����ǂݍ��ށB��pI/F���g���ꍇ�͐�pI/F����擾����j
	unsigned char  Reserved[3];   // �\��

	// �����f�[�^�̒l�ƕ����i�I�t�Z�b�g�������j
	float up;            // ��������� [m]
	float down;          // ���������� [m]

	float horizontal[N_MAX_HORIZONTAL_DISTANCE_SENSOR]; // �����ʓ� [m]
	float direction[N_MAX_HORIZONTAL_DISTANCE_SENSOR];  // �z�u���� [rad] (�O���� 0rad �Ƃ��C�E���𐳂Ƃ���D-�΁`��)

	//
	// �����I�t�Z�b�g�l�i�Փˉ���� ���̒l - �I�t�Z�b�g > �� > 0 �ƂȂ�悤�ɍs����j
	//
	// �����f�[�^�l�ƃI�t�Z�b�g�l�̊֌W�F
	//
	// ���@�̏d�S�@���Z���T�@���@�̊O�`�@�@�@���v���_
	// ����������������������������������������
	// �@�@�@�@�@�@�@�@�����f�[�^�l�i����l�j
	// �@�@�@�@�@�@�@<-��������������������->
	// �@�@�@�@�@�@�@�@�I�t�Z�b�g
	// �@�@�@�@�@�@�@<-������>
	// �@�@�@�@�@�@�@�@�@�@�@�@���S�m�ۋ�����(��)
	// �@�@�@�@�@�@�@�@�@�@�@ <������>
	//
	float up_offset;
	float down_offset;
	float horizontal_offset[N_MAX_HORIZONTAL_DISTANCE_SENSOR];

	// �����l�̐ݒ�
	void SetDistanceSensorDirection()
	{
		switch( Layout ) 
		{
		case LAYOUT_4DIRECTION_A:
			direction[INDEX_4A_FRONT] = DIRECTION_FRONT;
			direction[INDEX_4A_RIGHT] = DIRECTION_RIGHT;
			direction[INDEX_4A_REAR]  = DIRECTION_REAR;
			direction[INDEX_4A_LEFT]  = DIRECTION_LEFT;
			break;
		
		case LAYOUT_6DIRECTION_A:
			direction[INDEX_6A_FRONT]       = DIRECTION_FRONT;
			direction[INDEX_6A_RIGHT_FRONT] = DIRECTION_6A_RIGHT_FRONT;
			direction[INDEX_6A_RIGHT_REAR]  = DIRECTION_6A_RIGHT_REAR;
			direction[INDEX_6A_REAR]        = DIRECTION_REAR;
			direction[INDEX_6A_LEFT_REAR]   = DIRECTION_6A_LEFT_REAR;
			direction[INDEX_6A_LEFT_FRONT]  = DIRECTION_6A_LEFT_FRONT;
			break;
		
		case LAYOUT_6DIRECTION_B:
			direction[INDEX_6B_RIGHT_FRONT] = DIRECTION_6B_RIGHT_FRONT;
			direction[INDEX_6B_RIGHT]       = DIRECTION_RIGHT;
			direction[INDEX_6B_RIGHT_REAR]  = DIRECTION_6B_RIGHT_REAR;
			direction[INDEX_6B_LEFT_REAR]   = DIRECTION_6B_LEFT_REAR;
			direction[INDEX_6B_LEFT]        = DIRECTION_LEFT;
			direction[INDEX_6B_LEFT_FRONT]  = DIRECTION_6B_LEFT_FRONT;
			break;
		
		case LAYOUT_8DIRECTION_A:
			direction[INDEX_8A_FRONT]       = DIRECTION_FRONT;
			direction[INDEX_8A_RIGHT_FRONT] = DIRECTION_8A_RIGHT_FRONT;
			direction[INDEX_8A_RIGHT]       = DIRECTION_RIGHT;
			direction[INDEX_8A_RIGHT_REAR]  = DIRECTION_8A_RIGHT_REAR;
			direction[INDEX_8A_REAR]        = DIRECTION_REAR;
			direction[INDEX_8A_LEFT_REAR]   = DIRECTION_8A_LEFT_REAR;
			direction[INDEX_8A_LEFT]        = DIRECTION_LEFT;
			direction[INDEX_8A_LEFT_FRONT]  = DIRECTION_8A_LEFT_FRONT;
			break;
		
		default:
			break;
		}	
	}
};

#endif