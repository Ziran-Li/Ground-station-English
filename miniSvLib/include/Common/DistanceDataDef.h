#ifndef INCLUDED_DISTANCEDATADEF_H
#define INCLUDED_DISTANCEDATADEF_H

enum DISTANCE_DATA_FLAG
{
	FLAG_DOWN        = 0x0001, // 下
	FLAG_UP          = 0x0002, // 上
	FLAG_FRONT       = 0x0004, // 前   4方向   0°/ 6方向(A)   0°/ 8方向   0°
	FLAG_RIGHT       = 0x0008, // 右   4方向  90°/ 6方向(B)  90°/ 8方向  90°
	FLAG_REAR        = 0x0010, // 後   4方向 180°/ 6方向(A) 180°/ 8方向 180°
	FLAG_LEFT        = 0x0020, // 左   4方向 -90°/ 6方向(B) -90°/ 8方向 -90°
	FLAG_RIGHT_FRONT = 0x0040, // 右前 6方向(A)   60°/ 6方向(B)   30°/ 8方向   45°
	FLAG_RIGHT_REAR  = 0x0080, // 右後 6方向(A)  120°/ 6方向(B)  150°/ 8方向  135°
	FLAG_LEFT_REAR   = 0x0100, // 左後 6方向(A) -120°/ 6方向(B) -150°/ 8方向 -135°
	FLAG_LEFT_FRONT  = 0x0200, // 左前 6方向(A)  -60°/ 6方向(B)  -30°/ 8方向  -45°
};

enum DISTANCE_LAYOUT_MODE
{
	LAYOUT_DISABLED     =   0, // 距離センサを使用しない
	LAYOUT_4DIRECTION_A =   1, // 4方向レイアウト(前/右/後/左)
	LAYOUT_4DIRECTION_B =   2, // 予約
	LAYOUT_6DIRECTION_A =   3, // 6方向レイアウトA ( 0°/ 60°/ 120°/  180°/ -120°/ -60°)
	LAYOUT_6DIRECTION_B =   4, // 6方向レイアウトB (30°/ 90°/ 150°/ -150°/  -90°/ -30°)
	LAYOUT_8DIRECTION_A =   5, // 8方向レイアウト  ( 0°/ 45°/  90°/  135°/ -135°/ -45°)
	LAYOUT_8DIRECTION_B =   6, // 予約
	LAYOUT_FREE         = 255, // 予約
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
	unsigned short flagAvailable; // データが利用可能か（有効レンジ外のときにクリアするかどうかはセンサ側の任意）
	unsigned short flagUpdated;   // データが今回更新されたか（毎回最初に全ビットがクリアされる）
	unsigned short flagInRange;   // センサの有効レンジ内かどうか（有効レンジ内なら衝突回避を行う）
	unsigned char  Layout;        // 配置モード（汎用の超音波I/Fを使う場合はEEPROMから読み込む。専用I/Fを使う場合は専用I/Fから取得する）
	unsigned char  Reserved[3];   // 予約

	// 距離データの値と方向（オフセット未除去）
	float up;            // 上方向距離 [m]
	float down;          // 下方向距離 [m]

	float horizontal[N_MAX_HORIZONTAL_DISTANCE_SENSOR]; // 水平面内 [m]
	float direction[N_MAX_HORIZONTAL_DISTANCE_SENSOR];  // 配置方向 [rad] (前方を 0rad とし，右回りを正とする．-π～π)

	//
	// 距離オフセット値（衝突回避は 生の値 - オフセット > α > 0 となるように行われる）
	//
	// 距離データ値とオフセット値の関係：
	//
	// ↓機体重心　↓センサ　↓機体外形　　　↓計測点
	// ●─────■────╂───────◆
	// 　　　　　　　　距離データ値（測定値）
	// 　　　　　　　<-──────────->
	// 　　　　　　　　オフセット
	// 　　　　　　　<-───>
	// 　　　　　　　　　　　　安全確保距離α(例)
	// 　　　　　　　　　　　 <───>
	//
	float up_offset;
	float down_offset;
	float horizontal_offset[N_MAX_HORIZONTAL_DISTANCE_SENSOR];

	// 方向値の設定
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