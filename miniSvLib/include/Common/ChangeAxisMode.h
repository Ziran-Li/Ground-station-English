///
/// @file       ChangeAxisMode.h
///
/// @brief      ���Ċ��蓖�ĂɊւ���ݒ�l�̒�`
///
///             �E��n����E��n�ւ̕ϊ��̂ݒ�`
///
/// @author     iwakura
///

#ifndef INCLUDED_CHANGEAXISMODE_H
#define INCLUDED_CHANGEAXISMODE_H

///
/// @brief  ���Ċ��蓖�ĂɊւ���ݒ�l
///
/// �ǂݕ��̗�F
/// FRONT_X_RIGHT_Y_DOWN_Z �Z���T��X�����@�̂̑O�C�Z���T��Y�����@�̂̉E�C�Z���T��Z�����@�̂̉��������Ă���
typedef enum
{
	FRONT_X_RIGHT_Y_DOWN_Z = 0x80, //!< �OX�C�EY�C��Z �� Xb =  Xs, Yb =  Ys, Zb =  Zs
	FRONT_X_LEFT_Z_DOWN_Y  = 0x81, //!< �OX�C��Z�C��Y �� Xb =  Xs, Yb = -Zs, Zb =  Ys
	FRONT_X_LEFT_Y_UP_Z    = 0x82, //!< �OX�C��Y�C��Z �� Xb =  Xs, Yb = -Ys, Zb = -Zs
	FRONT_X_RIGHT_Z_UP_Y   = 0x83, //!< �OX�C�EZ�C��Y �� Xb =  Xs, Yb =  Zs, Zb = -Ys
	
	BACK_X_RIGHT_Y_UP_Z    = 0x84, //!< ��X, �EY, ��Z �� Xb = -Xs, Yb =  Ys, Zb = -Zs
	BACK_X_LEFT_Z_UP_Y     = 0x85, //!< ��X, ��Z, ��Y �� Xb = -Xs, Yb = -Zs, Zb = -Ys
	BACK_X_LEFT_Y_DOWN_Z   = 0x86, //!< ��X, ��Y, ��Z �� Xb = -Xs, Yb = -Ys, Zb =  Zs
	BACK_X_RIGHT_Z_DOWN_Y  = 0x87, //!< ��X, �EZ, ��Y �� Xb = -Xs, Yb =  Zs, Zb =  Ys

	FRONT_Y_RIGHT_Z_DOWN_X = 0x88, //!< �OY�C�EZ�C��X �� Xb =  Ys, Yb =  Zs, Zb =  Xs
	FRONT_Y_LEFT_X_DOWN_Z  = 0x89, //!< �OY�C��X�C��Z �� Xb =  Ys, Yb = -Xs, Zb =  Zs
	FRONT_Y_LEFT_Z_UP_X    = 0x8A, //!< �OY�C��Z�C��X �� Xb =  Ys, Yb = -Zs, Zb = -Xs
	FRONT_Y_RIGHT_X_UP_Z   = 0x8B, //!< �OY�C�EX�C��Z �� Xb =  Ys, Yb =  Xs, Zb = -Zs

	BACK_Y_RIGHT_Z_UP_X    = 0x8C, //!< ��Y, �EZ, ��X �� Xb = -Ys, Yb =  Zs, Zb = -Xs
	BACK_Y_LEFT_X_UP_Z     = 0x8D, //!< ��Y, ��X, ��Z �� Xb = -Ys, Yb = -Xs, Zb = -Zs
	BACK_Y_LEFT_Z_DOWN_X   = 0x8E, //!< ��Y, ��Z, ��X �� Xb = -Ys, Yb = -Zs, Zb =  Xs
	BACK_Y_RIGHT_X_DOWN_Z  = 0x8F, //!< ��Y, �EX, ��Z �� Xb = -Ys, Yb =  Xs, Zb =  Zs

	FRONT_Z_RIGHT_X_DOWN_Y = 0x90, //!< �OZ�C�EX�C��Y �� Xb =  Zs, Yb =  Xs, Zb =  Ys
	FRONT_Z_LEFT_Y_DOWN_X  = 0x91, //!< �OZ�C��Y�C��X �� Xb =  Zs, Yb = -Ys, Zb =  Xs
	FRONT_Z_LEFT_X_UP_Y    = 0x92, //!< �OZ�C��X�C��Y �� Xb =  Zs, Yb = -Xs, Zb = -Ys
	FRONT_Z_RIGHT_Y_UP_X   = 0x93, //!< �OZ�C�EY�C��X �� Xb =  Zs, Yb =  Ys, Zb = -Xs
	
	BACK_Z_RIGHT_X_UP_Y    = 0x94, //!< ��Z, �EX, ��Y �� Xb = -Zs, Yb =  Xs, Zb = -Ys
	BACK_Z_LEFT_Y_UP_X     = 0x95, //!< ��Z, ��Y, ��X �� Xb = -Zs, Yb = -Ys, Zb = -Xs
	BACK_Z_LEFT_X_DOWN_Y   = 0x96, //!< ��Z, ��X, ��Y �� Xb = -Zs, Yb = -Xs, Zb =  Ys
	BACK_Z_RIGHT_Y_DOWN_X  = 0x97, //!< ��Z, �EY, ��X �� Xb = -Zs, Yb =  Ys, Zb =  Xs
	
} AXISCHANGEMODE;


#endif