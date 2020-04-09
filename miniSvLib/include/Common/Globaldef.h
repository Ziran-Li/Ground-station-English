//
//	    Globaldef.h
//
//		構造体・マクロ・型定義
//
//		'08/01/06	Ver 1.00	初版                                             by S-Suzuki
//		~08/10/08	Ver 1.01	定数の定義をいろいろと追加                       by iwakura
//		'08/10/16	Ver 1.02	フライトモード追加                               by iwakura
//		'08/10/28	Ver 1.02.1	デバッグ用変数追加                               by iwakura
//		'09/01/22	Ver 1.03	パケット情報追加                                 by iwakura
//		'09/02/27	Ver 1.04	フライトモード追加(USER1~5)                      by iwakura
//		         	        	パケットID追加(USER1~5)
//		'09/03/12	Ver 1.05	MAVInfoの追加                                    by iwakura
//		'09/03/15	Ver 1.06	MAVInfoのメンバ追加                              by iwakura
//		'09/04/02	Ver 1.07	EstDataの追加
//		'09/09/11	Ver 1.08	ファイルの場所変更に伴い微修正                   by iwakura
//		         	        	構造体名の変更(MavData)
//		         	        	パケットID追加(MAV_CONDITION)
//		'09/09/23	Ver 1.09	PosData にコンストラクタ追加                     by iwakura
//		         	        	PosData に値設定用の関数追加
//		'09/12/21	Ver 1.09.1	ファイル名・ファイルの場所変更に伴い微修正       by iwakura
//		'10/01/04	Ver 1.10	パケットID追加 (PACKETID_ATTREF2, PACKETID_VELREF2, PACKETID_GPSREF2)
//		         	        	                                                 by iwakura
//		'10/01/05	Ver 1.11	PI, EXP, PACKETID_PROPO_SIMPLE を追加            by iwakura
//		'10/01/30	Ver 1.12	PACKETID_GENERALINT, PACKETID_GENERALREAL を規定 by iwakura
//		'10/02/07	Ver 1.13	XUFO_COMMANDをMAV_COMMAND に改名                 by iwakura
//								COMMAND_PING を追加
//		'10/05/20	Ver 1.14	構造体名の変更(PPmdata -> CtrlData)              by iwakura
//		'10/06/20	Ver 1.15	構造体・PACKETID・PACKETSIZEの名前変更           by iwakura
//								PropoDataを初期化するようにした
//		'11/02/10	Ver 1.16	gDebugの初期化タイミングを変更                   by iwakura
//		∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽
//			ここから下のバージョンはプロジェクトのバージョン
//		∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽∽
//		'13/??/??	Ver 2.0.X	超いろいろと変更                                 by iwakura
//		'13/06/09	Ver 2.1.0	Data3D構造体を追加                               by iwakura
//		'12/07/06	Ver 2.1.1	EulerAngle, Quaternion構造体を追加               by iwakura
//		'13/08/15	Ver 3.2.0	Commondef_High に移動：
//		         	         	    "LowLevelMCUData"
//		         	         	Commondef_Low に移動：
//		         	         	    "QUADWORD"
//
//

#ifndef INCLUDED_GLOBALDEF_H
#define INCLUDED_GLOBALDEF_H

#include "Common/StandardDef.h"

// 汎用状態ベクトル
class StateVector
{
protected:
	size_t  m_Length;
	
	double* m_pX;

public:
	StateVector();
	~StateVector();
	
	CRESULT SetLength(size_t length);                      // 領域確保用．
	size_t  GetLength() const;                             // ベクトル長の取得．
	double* GetPointer(unsigned int index, size_t length); // ポインタの取得．length が一致しない場合 NULL が返る．
	double* GetPointer(size_t length) { return GetPointer(0, length); }
};



#endif
