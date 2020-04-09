//
//	    SCIDeviceBase.h
//
//		RS232(EIA232)デバイス用コミュニケーションクラスの基本クラス
//
//		'08/10/23	Ver 0.01	暫定版                              by iwakura
//		'08/11/26	Ver 0.02	IsInit() 追加                       by iwakura
//		'09/12/21	Ver 0.02.1	includeファイル名の変更に伴い微修正 by iwakura
//
//		CSciの実装の差異を吸収し，継承先のクラスに共通の機能を提供する
//
//		設計が微妙…
//		接続ポートを自動認識するのがベスト？
//
//		
//

#ifndef __SCIDEVICE_BASE_H__
#define __SCIDEVICE_BASE_H__

#include "SCI.h"
#include "common/StandardDef.h"

class CSciDeviceBase
{
protected:
	// シリアル通信クラスのポインタ(InitializeSciで自動設定)
	CSciBase* m_pSci;
	
	// シリアル通信クラスの初期化(必ず初期化で呼び出すこと)
	CRESULT InitializeSci(SCIPORT sciport, BAUDRATE br, BYTE Parity = NOPARITY, BYTE StopBits = ONESTOPBIT);
	
public:
	// 構築・消滅
	CSciDeviceBase();
	virtual ~CSciDeviceBase();
	
	// 初期化処理関数
	//virtual CRESULT Initialize(SCIPORT sciport) = 0;
	
	// 切断
	CRESULT CloseSci();

	// 各種状態取得
	bool     IsInit() { return m_pSci != NULL; } // 初期化されたかどうか
	BOOL     IsPortEnable() const; // 現在の COM ポートが有効かどうか
	SCIPORT  GetCurrentSciPort();  // 現在の COM ポート番号を取得
	BAUDRATE GetCurrentBaudrate(); // 現在のボーレートを取得
	UINT     GetTransmitBits();    // 現在の転送ビット数を取得
	void     ClearTransmitBits();
	UINT     GetReceiveBits();
	void     ClearReceiveBits();
};

#endif