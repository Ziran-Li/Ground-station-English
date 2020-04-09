#include "WirelessDevice_FEP.h"
#include "Common/SystemTime.h"
#include "Common/ConnectionCreator.h"
#include <string.h>

//!< コマンドリトライ回数
#define N_COMMAND_RETRY 5


//!< コマンド部のシリアル通信時間
#define COMMANDCOMMUNICATIONTIME (12.0f * 1000.0f * 10.0f / DEFAULT_BAUDRATE)

//!< 1バイトあたりの無線通信時間 + シリアル通信時間
#define COMMUNICATIONTIMEPERBYTE (0.16f + 1000.0f * 10.0f / DEFAULT_BAUDRATE)   

//!< 固定通信処理時間
#define CONSTANTCOMMUNICATIONTIME (33.0f + 2.0f + 21.0f + 2.0f)
//#define CONSTANTCOMMUNICATIONTIME 33.0f

//!< 最小の休止時間 + コマンド部のシリアル通信時間
#define MINIMUMCOMMUNICATIONTIME (51.0f + 12 * 1000.0f * 10.0f / DEFAULT_BAUDRATE)

enum
{
	DEFAULT_BAUDRATE     = 115200,
	WIRELESSPACKET_LIMIT = 128, //!< 一つの無線パケットで送れるデータの最大値
};

enum FLAGS
{
	FLAG_READY   = 1, //!< デバイス利用準備OK
	FLAG_TX_BUSY = 2, //!< 送信ビジー状態
};

enum
{
	INITSEQ_START,     //!< 初期フェーズ
	INITSEQ_REG_CHECK, //!< レジスタチェック
	INITSEQ_REG_SET,   //!< レジスタ再設定
	INITSEQ_END,       //!< 完了
};

enum RECEIVE_MODE
{
	RXMODE_SLEEP,
	RXMODE_RECEIVE_WAITING,     //!< 先頭文字'R'の受信待ち状態
	RXMODE_HEADER_DECODING,     //!< RBN部受信中
	RXMODE_HEADER_DECODING2,    //!< TBN部受信中
	RXMODE_ADDR_DECODING,       //!< アドレス部(***)受信中
	RXMODE_ADDR_DECODING2,      //!< アドレス部(TBN後の自身のアドレス)受信中
	RXMODE_DATALENGTH_DECODING, //!< データ長(###)受信中
	RXMODE_PAYLOAD_DECODING,    //!< バイナリデータ部受信中
	RXMODE_FOOTER_DECODING,     //!< 最後の部分を受信中
	RXMODE_RSSIDATA_DECODING,   //!< [電強]部受信中
	RXMODE_COMMAND_ARG,
	RXMODE_COMMAND_ARG_RESPONCE_HEADER_DECODING,  //!< @ARGコマンドに対する応答データ待ち
	RXMODE_COMMAND_ARG_RESPONCE_REGDATA_DECODING,
};


// インライン宣言をヘッダでしないためのせこいマクロ
#define _IsRxOk() (m_Flags & FLAG_READY)
#define _IsTxOk() ((m_Flags & FLAG_READY) && !(m_Flags & FLAG_TX_BUSY))


// 2文字の16進数を数値に変換する関数
static bool _xtoi(const _UBYTE c[2], _UBYTE* pReturn)
{
	*pReturn = 0;

	if (c[0] - '0' <= 9)
	{
		*pReturn += (c[0] - '0') * 16;
	}
	else if (c[0] - 'A' <= 5)
	{
		*pReturn += (c[0] - 'A' + 10) * 16;
	}
	else
	{
		return false;
	}

	if (c[1] - '0' <= 9)
	{
		*pReturn += (c[1] - '0');
	}
	else if (c[1] - 'A' <= 5)
	{
		*pReturn += (c[1] - 'A' + 10);
	}
	else
	{
		return false;
	}

	return true;
}

// 数値を高速で3文字の文字列に変換する関数
static void _atoi128(unsigned char n, unsigned char ptr[3])
{
	// xが0～128の範囲であれば、y = int(103 * x) / 1024 は y = x / 10 の結果に等しい
	unsigned char m;

	m = (103 * (unsigned short)n) >> 10;
	ptr[2] = n - m * 10 + '0';
	n = (103 * (unsigned short)m) >> 10;
	ptr[1] = m - n * 10 + '0';
	m = (103 * (unsigned short)n) >> 10;
	ptr[0] = n - m * 10 + '0';
}

static void _atoi256(unsigned char n, unsigned char ptr[3])
{
	// xが0～256の範囲であれば、y = int(51 * (x+1)) / 512 は y = x / 10 の結果に等しい
	unsigned char m;

	m = (51 * (unsigned short)(n + 1)) >> 9;
	ptr[2] = n - m * 10 + '0';
	n = (51 * (unsigned short)(m + 1)) >> 9;
	ptr[1] = m - n * 10 + '0';
	m = (51 * (unsigned short)(n + 1)) >> 9;
	ptr[0] = n - m * 10 + '0';
}

Wireless_FEP::Wireless_FEP(_UBYTE PortNo) : WirelessDevice(PortNo), m_TempBuffer(WIRELESSPACKET_LIMIT)
{
	m_pSci = ConnectionCreator::Create(PSci(PortNo)); // インスタンス生成保障

	m_Flags = 0;

	memset(&m_Initializer, 0, sizeof(m_Initializer));
	m_Initializer.InitSequaence = INITSEQ_START;

	m_TimeNextSend = 0;

	m_TempBufferCount = 0;
	m_ReceiveMode = RXMODE_SLEEP;
	m_ReceiveStateCount = 0;
	m_IsReceived = false;
	m_Rssi = 0;
}

Wireless_FEP::~Wireless_FEP() 
{
	ConnectionCreator::Release(m_pSci);
}

CRESULT Wireless_FEP::Initialize(_UDWORD Baudrate)
{
	if (Baudrate == 0) Baudrate = DEFAULT_BAUDRATE;

	CRESULT res = C_OK;

	if (ISOK(res)) res = m_pSci->SetBaudrate(Baudrate);
	if (ISOK(res)) res = m_pSci->SetParity(SCI_PARITY_NONE);
	if (ISOK(res)) res = m_pSci->SetStopBits(SCI_STOPBIT_ONE);
	if (ISOK(res)) res = m_pSci->Open();

	return res;
}

void Wireless_FEP::Update()
{
	// 未初期化だった場合
	//if (m_pSci->) return;

	// デバイスの利用準備がまだだった場合、デバイスを利用可能にする処理を実行する
	if ((m_Flags & FLAG_READY) == 0)
	{
		_InitSequence();
		return;
	}

	// デバイスの利用準備が整っている場合、
#if 0
	if ((_SWORD)(m_TimeNextSend - ::GetSystemTimeShort()) <= 0)
	{
		// ビジー時間経過！

		// 送信ビジー状態の解除
		_ResetBusy();
	}
#else
	_UWORD nowTime = ::GetSystemTimeShort();
	_UWORD diffTime;
	
	if (nowTime <= m_TimeNextSend)
	{
		diffTime = m_TimeNextSend - nowTime;

		if (diffTime <= 0x7FFF)
		{
			if (nowTime == m_TimeNextSend)
			{
				// ビジー時間経過！

				// 送信ビジー状態の解除
				_ResetBusy();
			}
		}
		else
		{
			// 0x10000またぎ
			// ビジー時間経過！

			// 送信ビジー状態の解除
			_ResetBusy();
		}
	}
	else
	{
		diffTime = nowTime - m_TimeNextSend;

		if (diffTime <= 0x7FFF)
		{
			// ビジー時間経過！

			// 送信ビジー状態の解除
			_ResetBusy();
		}
		else
		{
			// 0x10000またぎ
			// ビジー時間未経過

		}
	}
#endif
}

bool Wireless_FEP::IsReady()
{
	return m_Flags & FLAG_READY;
}

bool Wireless_FEP::IsTxBusy()
{
	return !_IsTxOk();
}


CRESULT Wireless_FEP::SendBegin(size_t TotalLength)
{
	// 未初期化だった場合
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_BUSY;

	if (m_TempBufferCount + TotalLength > WIRELESSPACKET_LIMIT) return C_OUTOFBUFFER;

	return C_OK;
}

CRESULT Wireless_FEP::Send(_UBYTE data)
{
	// 未初期化だった場合
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_FAIL;

	m_TempBuffer[m_TempBufferCount++] = data;

	return C_OK;
}

CRESULT Wireless_FEP::Send(const void* pData, size_t length)
{
	// 未初期化だった場合
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_FAIL;

	const _UBYTE* pSrc = reinterpret_cast<const _UBYTE*>(pData);
	const _UBYTE* pEnd = pSrc + length;
	_UBYTE* pDest = &m_TempBuffer[m_TempBufferCount];

	while (pSrc < pEnd)
	{
		*pDest++ = *pSrc++;
 	}

	m_TempBufferCount += length;

	return C_OK;
}

CRESULT Wireless_FEP::SendEnd()
{
	// 未初期化だった場合
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_FAIL;

	return C_OK;
}

CRESULT Wireless_FEP::Flush()
{
	// 未初期化だった場合
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_FAIL;

	if (m_TempBufferCount)
	{
		// 送信バイト数を文字列に変換する
		_UBYTE len[3];
		_atoi128(m_TempBufferCount, len);

		// シリアル通信データ出力
		m_pSci->WriteString((const _UBYTE*)"@TBN", 4);
		m_pSci->WriteString(m_DestAddr, 3);
		m_pSci->WriteString(len, 3);
		m_pSci->WriteString(&m_TempBuffer[0], m_TempBufferCount);
		m_pSci->WriteString((const _UBYTE*)"\r\n", 2);

		// 送信ビジー状態に設定
		_SetBusy(m_TempBufferCount);
		
		m_TempBufferCount = 0; // バッファクリア
	}

	return C_OK;
}
	
CRESULT Wireless_FEP::Receive(_UBYTE* pData)
{
	// 未初期化だった場合
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsRxOk()) return C_FAIL;

	_Rx(); // m_ReceiveData と m_IsReceived の状態を更新する

	if (m_IsReceived)
	{
		*pData = m_ReceiveData;
		m_IsReceived = false;

		return C_OK;
	}
	else
	{
		return C_FAIL;
	}
}
	
bool Wireless_FEP::IsDataReceived()
{
	// 未初期化だった場合
	//if (!IsInitSci()) return false;

	if (!_IsRxOk()) return false;

	_Rx(); // m_ReceiveData と m_IsReceived の状態を更新する

	return m_IsReceived;
}
	
void Wireless_FEP::ProcessUnusedCharacter(_UBYTE c)
{
}
	
CRESULT Wireless_FEP::GetRssi(_UBYTE* pRssi) const
{
	if (m_Rssi == 0)
	{
		return C_NODATA;
	}
	else
	{
		*pRssi = m_Rssi;
		return C_OK;
	}
}

_UWORD Wireless_FEP::GetRoundTripTime(size_t TxBytes, size_t RxBytes) const
{
	if (TxBytes > WIRELESSPACKET_LIMIT) TxBytes = WIRELESSPACKET_LIMIT;
	if (RxBytes > WIRELESSPACKET_LIMIT) RxBytes = WIRELESSPACKET_LIMIT;

	return _GetBusyTime(TxBytes) + _GetBusyTime(RxBytes);
}

void Wireless_FEP::_InitSequence()
{
	// 利用準備ができていない

	_RxCommandResult();

	if (m_Initializer.InitSequaence == INITSEQ_START)
	{
		//----------------------------------
		// 初期① レジスタデータ全取得
		//----------------------------------
		if (m_ReceiveMode == RXMODE_SLEEP && m_Initializer.RegisterReadCount == 0)
		{
			// レジスタデータ未取得
			m_pSci->WriteString((const _UBYTE*)"@ARG\r\n", 6);
			m_ReceiveMode = RXMODE_COMMAND_ARG;
			m_Initializer.TimeoutCounter = 25;
		}
		else if (m_Initializer.RegisterReadCount == Wireless_FEP_Initializer::N_REGISTER)
		{
			// レジスタデータ取得完了
			//m_Initializer.InitSequaence = INITSEQ_REG_CHECK;
			m_Initializer.InitSequaence = INITSEQ_END;
			m_Initializer.RetryCounter = 0;

			_atoi256(m_Initializer.Reg[2], m_DestAddr); // 送信先のアドレスを取得
		}

		if (m_Initializer.TimeoutCounter == 0)
		{
			if (m_Initializer.RetryCounter < N_COMMAND_RETRY)
			{
				// タイムアウト → やりなおし
				m_ReceiveMode = RXMODE_SLEEP;
				m_Initializer.RegisterReadCount = 0;
				m_Initializer.RetryCounter++;
			}
			else
			{
				// リトライアウト
				m_Initializer.InitSequaence = INITSEQ_END;
				m_Initializer.RetryCounter = 0;

				m_DestAddr[0] = '9'; // 設定不可能なアドレスを送信先にする
				m_DestAddr[1] = '9';
				m_DestAddr[2] = '9';
			}
		}
		else
		{
			m_Initializer.TimeoutCounter--;
		}
	}

	if (m_Initializer.InitSequaence == INITSEQ_REG_CHECK)
	{
		//----------------------------------
		// 初期② レジスタデータチェック
		//----------------------------------

		if (m_ReceiveMode == RXMODE_SLEEP)
		{
			if (m_Initializer.Reg[13] != 0x81)
			{
				m_pSci->WriteString((const _UBYTE*)"@REG13:81H\r\n", 12);

			}
		}

		//m_Initializer.RegisterVal[2]
	}

	if (m_Initializer.InitSequaence == INITSEQ_REG_SET)
	{
		//----------------------------------
		// 初期③ レジスタ再設定
		//----------------------------------

	}

	if (m_Initializer.InitSequaence == INITSEQ_END)
	{
		//----------------------------------
		// 初期④ 初期化完了
		//----------------------------------

		// delete pInitializer;

		m_Flags |= FLAG_READY | FLAG_TX_BUSY;
		m_ReceiveMode = RXMODE_RECEIVE_WAITING;
		m_TimeNextSend = ::GetSystemTimeShort() + _GetBusyTime(0);
	}
}

void Wireless_FEP::_Rx()
{
	_UBYTE c;

	if (m_IsReceived) return;

	// プロトコル側のデータが読み出せるまで繰り返す。
	// 読み出せなかったら false を返す
	while (ISOK(m_pSci->ReadElement(&c)))
	{
		// 1バイト受信

		if (m_ReceiveMode == RXMODE_PAYLOAD_DECODING)
		{
			//----------------------------------
			// ⑤ 受信データ本体受信中…
			//----------------------------------
			m_ReceiveData = c;
			m_IsReceived = true;
			m_ReceiveLength--;

			if (m_ReceiveLength == 0)
			{
				m_ReceiveMode = RXMODE_FOOTER_DECODING;
			}

			// プロトコル側のデータを受信できた！
			return;
		}
		else if (m_ReceiveMode == RXMODE_RECEIVE_WAITING)
		{
			//----------------------------------
			// ① 受信データ待ち状態
			// 'R'     - RBN***###[バイナリデータ][電強]<CR><LF> の 1バイト目を受信？
			// 'T'     - @TBN999###[バイナリデータ][電強]<CR><LF> の 2バイト目を受信？（直接接続時）
			// 上記以外 - ゴミとみなす
			//----------------------------------
			if (c == 'R') 
			{
				m_ReceiveMode = RXMODE_HEADER_DECODING;
				m_ReceiveStateCount = 0;
				m_ReceiveStringChecker = 0;
			}
			else if (c == 'T')
			{
				// 直接接続したときに対応するためのコード
				m_ReceiveMode = RXMODE_HEADER_DECODING2;
				m_ReceiveStateCount = 0;
				m_ReceiveStringChecker = 0;
			}
		}
		else if (m_ReceiveMode == RXMODE_HEADER_DECODING || m_ReceiveMode == RXMODE_HEADER_DECODING2)
		{
			//----------------------------------
			// ② ヘッダ"RBN"の"BN"部分受信中…（or "TBN"の"BN"部分）
			//----------------------------------
			m_ReceiveStringChecker <<= 8;
			m_ReceiveStringChecker |= c;
			m_ReceiveStateCount++;

			if (m_ReceiveStateCount == 2)
			{
				if (m_ReceiveStringChecker == (('B' << 8) | 'N'))
				{
					if (m_ReceiveMode == RXMODE_HEADER_DECODING)
					{
						// "RBN"を連続で受信した
						m_ReceiveMode = RXMODE_ADDR_DECODING;
						m_ReceiveStateCount = 0;
						m_ReceiveStringChecker = 0;
					}
					else
					{
						// "TBN"を連続で受信した
						m_ReceiveMode = RXMODE_ADDR_DECODING2;
						m_ReceiveStateCount = 0;
						m_ReceiveStringChecker = 0;
					}
				}
				else
				{
					// "RBN"ではなかった
					m_ReceiveMode = RXMODE_RECEIVE_WAITING;
				}
			}
		}
		else if (m_ReceiveMode == RXMODE_ADDR_DECODING || m_ReceiveMode == RXMODE_ADDR_DECODING2)
		{
			//----------------------------------
			// ③ 送信元アドレス受信中…
			//----------------------------------
			m_ReceiveStringChecker <<= 8;
			m_ReceiveStringChecker |= c;
			m_ReceiveStateCount++;

			if (m_ReceiveStateCount == 3)
			{
				if ((m_ReceiveMode == RXMODE_ADDR_DECODING && m_ReceiveStringChecker == ((m_DestAddr[0] << 16) | (m_DestAddr[1] << 8) | m_DestAddr[2])) ||
					m_ReceiveMode == RXMODE_ADDR_DECODING2)
				{
					// アドレスが送信先のデバイスのアドレスと一致した（or TBN後の3バイトの受信だった）
					m_ReceiveMode = RXMODE_DATALENGTH_DECODING;
					m_ReceiveStateCount = 0;
					m_ReceiveLength = 0;
				}
				else
				{
					// アドレス不一致
					m_ReceiveMode = RXMODE_RECEIVE_WAITING;
				}
			}
		}
		else if (m_ReceiveMode == RXMODE_DATALENGTH_DECODING)
		{
			//----------------------------------
			// ④ バイト数受信中…
			//----------------------------------
			m_ReceiveLength *= 10;
			m_ReceiveLength += c - '0';
			m_ReceiveStateCount++;

			if (m_ReceiveStateCount == 3)
			{
				if (m_ReceiveLength != 0 && m_ReceiveLength <= 128)
				{
					// バイト数データデコード完了
					m_ReceiveMode = RXMODE_PAYLOAD_DECODING;
					m_ReceiveStateCount = 0;
				}
				else
				{
					// 不正な値
					m_ReceiveMode = RXMODE_RECEIVE_WAITING;
				}
			}
		}
		else if (m_ReceiveMode == RXMODE_FOOTER_DECODING)
		{
			//----------------------------------
			// ⑥ フッタ受信中…
			// 次のいずれか
			// <CR><LF>
			// [電強]<CR><LF>
			//----------------------------------
			if (static_cast<_UBYTE>(c - '0') <= 1) // 1文字目は'0' か '1' しか来ない
			{
				// [電強]<CR><LF> のパターン
				m_RssiDataTemp = c - '0';
				m_ReceiveMode = RXMODE_RSSIDATA_DECODING;
				m_ReceiveStateCount = 1;
			}
			else
			{
				// <CR><LF> のパターン もしくは 例外
				m_ReceiveMode = RXMODE_RECEIVE_WAITING;
			}
		}
		else if (m_ReceiveMode == RXMODE_RSSIDATA_DECODING)
		{
			//----------------------------------
			// ⑦電強データ受信中…
			//----------------------------------
			if (static_cast<_UBYTE>(c - '0') <= 9) // '0' ～ '9'
			{
				m_RssiDataTemp *= 10;
				m_RssiDataTemp += c - '0';
				m_ReceiveStateCount++;
			}
			else
			{
				// 例外
				m_ReceiveMode = RXMODE_RECEIVE_WAITING;
			}

			if (m_ReceiveStateCount == 3)
			{
				m_Rssi = m_RssiDataTemp;
				m_ReceiveMode = RXMODE_RECEIVE_WAITING;
			}
		}
	}

	// 読み出せなかった場合
	return;
}


void Wireless_FEP::_RxCommandResult()
{
	_UBYTE c;

	while (ISOK(m_pSci->ReadElement(&c)))
	{
		if (m_ReceiveMode == RXMODE_COMMAND_ARG)
		{
			//----------------------------------
			// 初期①-(1) 初期値を設定してすぐ次へ
			//----------------------------------
			m_ReceiveMode = RXMODE_COMMAND_ARG_RESPONCE_HEADER_DECODING;
			m_ReceiveStateCount = 0;
			m_ReceiveStringChecker = 0;
			m_Initializer.RegisterReadCount = 0;
		}


		if (m_ReceiveMode == RXMODE_COMMAND_ARG_RESPONCE_HEADER_DECODING)
		{
			//----------------------------------
			// 初期①-(2) @ARGコマンドの応答（REG##:$$H<CR><LF> ×29行 の"REG"部）を受信中…
			//----------------------------------
			m_ReceiveStringChecker <<= 8;
			m_ReceiveStringChecker |= c;

			if ((m_ReceiveStringChecker & 0xFFFFFF) == (('R' << 16) | ('E' << 8) | 'G'))
			{
				m_ReceiveMode = RXMODE_COMMAND_ARG_RESPONCE_REGDATA_DECODING;
				m_ReceiveStateCount = 0;
			}
		}
		else if (m_ReceiveMode == RXMODE_COMMAND_ARG_RESPONCE_REGDATA_DECODING)
		{
			//----------------------------------
			// 初期①-(3) @ARGコマンドの応答（REG##:$$H<CR><LF> ×29行 の"##:$$H"<CR><LF>部）を受信中…
			//----------------------------------
			m_Initializer.Workspace[m_ReceiveStateCount++] = c;

			if (m_ReceiveStateCount == 8)
			{
				_UBYTE RegNo = (m_Initializer.Workspace[0] - '0') * 10 + (m_Initializer.Workspace[1] - '0'); // 4, 5文字目はレジスタ番号
				_UBYTE Value;

				// フォーマットチェック
				if (m_Initializer.Workspace[2] == ':' &&
					m_Initializer.Workspace[5] == 'H' &&
					m_Initializer.Workspace[6] == '\r' &&
					m_Initializer.Workspace[7] == '\n' &&
					RegNo == m_Initializer.RegisterReadCount &&
					_xtoi(&m_Initializer.Workspace[3], &Value)) // ← ここでレジスタ値のデコードをする。7, 8文字目がレジスタ値
				{
					// 書式OK

					m_Initializer.Reg[m_Initializer.RegisterReadCount++] = Value;

					if (m_Initializer.RegisterReadCount >= Wireless_FEP_Initializer::N_REGISTER)
					{
						// 全レジスタ取得完了
						m_ReceiveMode = RXMODE_SLEEP;
					}
					else
					{
						// まだ全部取得していない → 次のレジスタを読みにいく
						m_ReceiveMode = RXMODE_COMMAND_ARG_RESPONCE_HEADER_DECODING;
						m_ReceiveStringChecker = 0;
					}
				}
				else
				{
					// 書式NG → やりなおし(@ARGコマンド再送)

					m_Initializer.RegisterReadCount = 0;
					m_ReceiveMode = RXMODE_SLEEP;
				}
			}
		}
	}
}

void Wireless_FEP::_SetBusy(_UWORD length)
{
	_UWORD BusyTime = _GetBusyTime(length);

	m_Flags |= FLAG_TX_BUSY;
	m_TimeNextSend = ::GetSystemTimeShort() + BusyTime;
}

void Wireless_FEP::_ResetBusy()
{
	m_Flags &= ~FLAG_TX_BUSY;
}

_UWORD Wireless_FEP::_GetBusyTime(_UWORD length) const
{
	float time = CONSTANTCOMMUNICATIONTIME + COMMANDCOMMUNICATIONTIME + length * COMMUNICATIONTIMEPERBYTE;

	// time の値は最小値以下にならない
	if (time < MINIMUMCOMMUNICATIONTIME) time = MINIMUMCOMMUNICATIONTIME;

	return -static_cast<_SWORD>(-time); // 整数に変換すると同時に繰上げを行う
}