//
//		SCI.h
//
//		Windows用シリアル通信クラス
//
//		'09/01/25	Ver 0.01	プログラミング開始                  by iwakura
//		'09/02/15	Ver 1.00	ひとまず完成                        by iwakura
//		'09/09/11	Ver 1.01	受信バイト数を取得可能にした        by iwakura
//		'09/12/21	Ver 1.01.1	includeファイル名の変更に伴い微修正 by iwakura
//		'11/06/16	Ver 1.02	ポート解放関数を Public に変更      by iwakura
//								接続時にいくつかの変数がリセットされていなかった問題を修正
//

#ifndef __SCI_H__
#define __SCI_H__

#include <windows.h>
#include "common/StandardDef.h"

enum SCIPORT {
	PORTNO_NULL = -1,  // 存在しないポート番号
	PORTNO_MIN  = 0,   // ポート番号の下限
	PORTNO_MAX  = 256, // ポート番号の上限
	SCI0 = 0, SCI1 = 1, SCI2 = 2, SCI3 = 3, SCI4 = 4,
	SCI5 = 5, SCI6 = 6, SCI7 = 7, SCI8 = 8, SCI9 = 9,
};

enum BAUDRATE { // 設定可能なボーレート
	BR_110    = CBR_110,    BR_300    = CBR_300,    BR_600    = CBR_600,   
	BR_1200   = CBR_1200,   BR_2400   = CBR_2400,   BR_4800   = CBR_4800,
	BR_9600   = CBR_9600,   BR_14400  = CBR_14400,  BR_19200  = CBR_19200,
	BR_38400  = CBR_38400,  BR_56000  = CBR_56000,  BR_57600  = CBR_57600,        
	BR_115200 = CBR_115200, BR_128000 = CBR_128000, BR_256000 = CBR_256000    
};

class CSciBase
{
public:
	typedef void*         HANDLE;
	typedef unsigned long DWORD;

	enum {
		RXBUFFSIZE = 1024,
		SCIBITS_PER_BYTE = 10, // STB, DATAB*8, EDB
	};

private:
	HANDLE   m_hComm;      // COMポートのハンドル
	DCB      m_CurrentDCB;
	DCB      m_DefaultDCB; // 元の設定のバックアップ
	SCIPORT  m_CurrentSciPort;
	
	struct {
		UINT nTransmitBytes;
		
	} m_TxInfo;

	struct {
		unsigned char Data[RXBUFFSIZE];
		int           wp;
		int           rp;
	} m_Rx;

	struct {
		UINT nReceiveBytes;
	} m_RxInfo;

	void          _RxAdd(unsigned char var); // 受信バッファにデータを追加（デバッグ用）
	unsigned char _RxPop();

public:
	CSciBase();
	virtual ~CSciBase();

	// 初期化（ここでCOMポートをオープン）
	virtual CRESULT Initialize(SCIPORT sciport, BAUDRATE br, BYTE Parity = NOPARITY, BYTE StopBits = ONESTOPBIT);

	// データ送信関数
	CRESULT WriteString(unsigned char* str, unsigned short nBytes); // 連続したデータを送信する

	// データ受信関数
	bool    IsReceiveData() { _Update(); return m_Rx.wp != m_Rx.rp; } // データを受信しているか
	CRESULT ReadElement(unsigned char* pData);                        // バッファから 1 文字読み出し


	// 各種情報取得
	BOOL     IsPortEnable() const; // 通信可能かどうか調べる
	SCIPORT  GetCurrentSciPort();  // 現在の COM ポート番号を取得
	BAUDRATE GetCurrentBaudrate(); // 現在のボーレートを取得
	UINT     GetTransmitBytes();   // 送信したバイト数を取得
	void     ClearTransmitBytes(); // クリア
	UINT     GetReceiveBytes();    // 受信したバイト数を取得
	void     ClearReceiveBytes();  // クリア

protected:
	// Windows環境専用
	CRESULT _OpenComPort(unsigned char PortNo, DWORD BaudRate, BYTE Parity = NOPARITY, BYTE StopBits = ONESTOPBIT);
	CRESULT _Update();

public:
	CRESULT CloseComPort(); // 接続を閉じる
};

#endif