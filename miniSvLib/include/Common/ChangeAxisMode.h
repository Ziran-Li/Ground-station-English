///
/// @file       ChangeAxisMode.h
///
/// @brief      軸再割り当てに関する設定値の定義
///
///             右手系から右手系への変換のみ定義
///
/// @author     iwakura
///

#ifndef INCLUDED_CHANGEAXISMODE_H
#define INCLUDED_CHANGEAXISMODE_H

///
/// @brief  軸再割り当てに関する設定値
///
/// 読み方の例：
/// FRONT_X_RIGHT_Y_DOWN_Z センサのX軸が機体の前，センサのY軸が機体の右，センサのZ軸が機体の下を向いている
typedef enum
{
	FRONT_X_RIGHT_Y_DOWN_Z = 0x80, //!< 前X，右Y，下Z → Xb =  Xs, Yb =  Ys, Zb =  Zs
	FRONT_X_LEFT_Z_DOWN_Y  = 0x81, //!< 前X，左Z，下Y → Xb =  Xs, Yb = -Zs, Zb =  Ys
	FRONT_X_LEFT_Y_UP_Z    = 0x82, //!< 前X，左Y，上Z → Xb =  Xs, Yb = -Ys, Zb = -Zs
	FRONT_X_RIGHT_Z_UP_Y   = 0x83, //!< 前X，右Z，上Y → Xb =  Xs, Yb =  Zs, Zb = -Ys
	
	BACK_X_RIGHT_Y_UP_Z    = 0x84, //!< 後X, 右Y, 上Z → Xb = -Xs, Yb =  Ys, Zb = -Zs
	BACK_X_LEFT_Z_UP_Y     = 0x85, //!< 後X, 左Z, 上Y → Xb = -Xs, Yb = -Zs, Zb = -Ys
	BACK_X_LEFT_Y_DOWN_Z   = 0x86, //!< 後X, 左Y, 下Z → Xb = -Xs, Yb = -Ys, Zb =  Zs
	BACK_X_RIGHT_Z_DOWN_Y  = 0x87, //!< 後X, 右Z, 下Y → Xb = -Xs, Yb =  Zs, Zb =  Ys

	FRONT_Y_RIGHT_Z_DOWN_X = 0x88, //!< 前Y，右Z，上X → Xb =  Ys, Yb =  Zs, Zb =  Xs
	FRONT_Y_LEFT_X_DOWN_Z  = 0x89, //!< 前Y，左X，下Z → Xb =  Ys, Yb = -Xs, Zb =  Zs
	FRONT_Y_LEFT_Z_UP_X    = 0x8A, //!< 前Y，左Z，上X → Xb =  Ys, Yb = -Zs, Zb = -Xs
	FRONT_Y_RIGHT_X_UP_Z   = 0x8B, //!< 前Y，右X，上Z → Xb =  Ys, Yb =  Xs, Zb = -Zs

	BACK_Y_RIGHT_Z_UP_X    = 0x8C, //!< 後Y, 右Z, 上X → Xb = -Ys, Yb =  Zs, Zb = -Xs
	BACK_Y_LEFT_X_UP_Z     = 0x8D, //!< 後Y, 左X, 上Z → Xb = -Ys, Yb = -Xs, Zb = -Zs
	BACK_Y_LEFT_Z_DOWN_X   = 0x8E, //!< 後Y, 左Z, 下X → Xb = -Ys, Yb = -Zs, Zb =  Xs
	BACK_Y_RIGHT_X_DOWN_Z  = 0x8F, //!< 後Y, 右X, 下Z → Xb = -Ys, Yb =  Xs, Zb =  Zs

	FRONT_Z_RIGHT_X_DOWN_Y = 0x90, //!< 前Z，右X，下Y → Xb =  Zs, Yb =  Xs, Zb =  Ys
	FRONT_Z_LEFT_Y_DOWN_X  = 0x91, //!< 前Z，左Y，下X → Xb =  Zs, Yb = -Ys, Zb =  Xs
	FRONT_Z_LEFT_X_UP_Y    = 0x92, //!< 前Z，左X，上Y → Xb =  Zs, Yb = -Xs, Zb = -Ys
	FRONT_Z_RIGHT_Y_UP_X   = 0x93, //!< 前Z，右Y，上X → Xb =  Zs, Yb =  Ys, Zb = -Xs
	
	BACK_Z_RIGHT_X_UP_Y    = 0x94, //!< 後Z, 右X, 上Y → Xb = -Zs, Yb =  Xs, Zb = -Ys
	BACK_Z_LEFT_Y_UP_X     = 0x95, //!< 後Z, 左Y, 上X → Xb = -Zs, Yb = -Ys, Zb = -Xs
	BACK_Z_LEFT_X_DOWN_Y   = 0x96, //!< 後Z, 左X, 下Y → Xb = -Zs, Yb = -Xs, Zb =  Ys
	BACK_Z_RIGHT_Y_DOWN_X  = 0x97, //!< 後Z, 右Y, 下X → Xb = -Zs, Yb =  Ys, Zb =  Xs
	
} AXISCHANGEMODE;


#endif