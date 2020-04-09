//
//		SCI.h
//
//		Windows用シリアル通信クラス
//

#ifndef INCLUDED_SCI_WIN_H
#define INCLUDED_SCI_WIN_H

#include <windows.h>
#include "Common/ISci.h"

#define PORTNO_NULL  -1

class CSciBase : public ISci
{
public:
	typedef void*         HANDLE;
	typedef unsigned long DWORD;

	enum 
	{
		RXBUFFSIZE = 1024,
	};

private:
	HANDLE  m_hComm;      // COMポートのハンドル
	DCB     m_CurrentDCB;
	DCB     m_DefaultDCB; // 元の設定のバックアップ
	int     m_Port;
	_UDWORD m_Baudrate;
	_UBYTE  m_Parity;
	_UBYTE  m_StopBits;
	
	struct 
	{
		UINT nTransmitBytes;
	} m_TxInfo;

	struct 
	{
		unsigned char Data[RXBUFFSIZE];
		int           wp;
		int           rp;
	} m_Rx;

	struct 
	{
		UINT nReceiveBytes;
	} m_RxInfo;

	void          _RxAdd(unsigned char var); // 受信バッファにデータを追加（デバッグ用）
	unsigned char _RxPop();

public:
	CSciBase(_UBYTE PortNo);
	virtual ~CSciBase();

	virtual int     GetPortNumber() const;

	virtual CRESULT SetBaudrate(_UDWORD Baudrate);
	virtual CRESULT SetParity(PARITY Parity);
	virtual CRESULT SetStopBits(STOPBIT StopBits);

	virtual CRESULT Open();
	virtual void    Close();

	virtual bool    IsEnable() const;

	// データ送信関数
	virtual CRESULT WriteElement(_UBYTE chr);                   // 1Byte送信
	virtual CRESULT WriteString(const _UBYTE* ptr, _UWORD len); // 連続データ送信

	// データ受信関数
	virtual bool    IsReceiveData();                            // データを受信しているか
	virtual CRESULT ReadElement(_UBYTE* ptr);                   // 1Byte受信

	virtual _UDWORD IsTransmittable(void);						// 送信可能チェック

	// 統計情報
	virtual _UDWORD GetTxCount() const;
	virtual _UDWORD GetRxCount() const;
	virtual _UDWORD GetParityErrors() const { return 0; }
	virtual _UDWORD GetFramingErrors() const { return 0; }
	virtual _UDWORD GetOverrunErrors() const { return 0; }

protected:
	// Windows環境専用
	CRESULT _OpenComPort(_UBYTE PortNo, _UDWORD Baudrate, _UBYTE Parity, _UBYTE StopBits);
	CRESULT _Update();

	CRESULT _CloseComPort(); // 接続を閉じる
};

#endif