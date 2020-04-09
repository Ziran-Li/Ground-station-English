#include "Protocol_Legacy.h"
#include "Protocol_LegacyDef.h"
#include "Common/Communicator.h"
#include "WirelessDevice.h"
#include <string.h>

Protocol_Legacy::Protocol_Legacy(WirelessDevice& device,
                                 ICommunicator&  communicator) 
	:
	Protocol(device, communicator)
{
	// メンバの初期化
	memset(&m_Rx, 0, sizeof(m_Rx));
	
	// 受信バッファの取得
	void*  pBuff;
	size_t BufferLength;
	
	CRESULT res = communicator.GetReceiveBuffer(&pBuff, &BufferLength);
	
	if( ISOK(res) ) {
		m_Rx.pBuff = reinterpret_cast<unsigned char*>(pBuff);
		m_Rx.BufferLength = BufferLength;	
	}
}

CRESULT Protocol_Legacy::Initialize()
{
	// バッファの取得に失敗していた場合
	if( m_Rx.pBuff == NULL ) return C_FAIL;
	
	// メンバの初期化
	_ClearMemory();
	
	return C_OK;
}

CRESULT Protocol_Legacy::Send(unsigned int ID, const void* pBuff, size_t length)
{
	// バッファの取得に失敗していた場合
	if( m_Rx.pBuff == NULL ) return C_FAIL;
	
	// IDが範囲外の場合
	if( ID > 0xFF ) return C_FAIL;
	
	CRESULT ret = C_OK;
	
	_UWORD Checksum;
	
	// チェックサム計算
	Checksum = _CalculateChecksum(ID, pBuff, length);
	
	if (ISOK(ret)) ret = m_WirelessDevice.SendBegin(length + PACKETSIZE_CONSTANT);
	if (ISOK(ret)) ret = m_WirelessDevice.Send(PREAMBLE1);
	if (ISOK(ret)) ret = m_WirelessDevice.Send(PREAMBLE2);
	if (ISOK(ret)) ret = m_WirelessDevice.Send(ID & 0xFF);
	if (ISOK(ret)) ret = m_WirelessDevice.Send(pBuff, length);
	if (ISOK(ret)) ret = m_WirelessDevice.Send(Checksum >> 8);
	if (ISOK(ret)) ret = m_WirelessDevice.Send(Checksum & 0xFF);
	if (ISOK(ret)) ret = m_WirelessDevice.SendEnd();
	
	return ret;
}

CRESULT Protocol_Legacy::Receive()
{
	// バッファの取得に失敗していた場合
	if( m_Rx.pBuff == NULL ) return C_FAIL;
	
	while( m_WirelessDevice.IsDataReceived() ) 
	{
		unsigned char cur;
		m_WirelessDevice.Receive(&cur);	

		if( m_Rx.DataCount < PREAMBLE_LENGTH ) {
			// ----------------------------------
			//	PREAMBLE1/PREAMBLE2 受信中
			//	条件：m_Rx.DataCount が 0 or 1
			// ----------------------------------
			bool ok = false;
			
			if( m_Rx.DataCount == 0 && cur == PREAMBLE1) {
				m_Rx.pBuff[0] = cur;
				ok = true;
			} else {
				m_WirelessDevice.ProcessUnusedCharacter(cur); // パケットとして認識されなかったデータをデバイスクラスに処理させる	
			}
			
			if( m_Rx.DataCount == 1 && cur == PREAMBLE2) {
				m_Rx.pBuff[1] = cur;
				ok = true;
			} else {
				m_WirelessDevice.ProcessUnusedCharacter(m_Rx.pBuff[0]); // パケットとして認識されなかったデータをデバイスクラスに処理させる
				m_WirelessDevice.ProcessUnusedCharacter(cur);
				m_Rx.DataCount = 0;
			}
			
			if( ok ) {
				m_Rx.DataCount++;	
			}
		
		} else if( m_Rx.DataCount < HEADER_LENGTH ) {
			// ----------------------------------
			//	ID 受信中
			//	条件：m_Rx.nDataCount が HEADER::LENGTH より小さい
			// ----------------------------------
			m_Rx.pBuff[m_Rx.DataCount++] = cur;
		
			if( size_t length = m_Communicator.GetPayloadLengthByID(cur) + HEADER_LENGTH + FOOTER_LENGTH ) {
				// 有効なパケットID
				
				if( length <= m_Rx.BufferLength ) {
					// バッファに収まる
					m_Rx.CurrentPacketLength = static_cast<unsigned char>(length);
					
				} else {
					// バッファに収まらない
					_EraseJunkData();
				}
				
			} else {
				// 無効なパケットID
				_EraseJunkData();
			}
			
		} else {
			// ----------------------------------
			//	PAYLOAD, チェックサム受信中
			//	条件：m_Rx.nDataCount が CurrentPacketLength-1 より小さい
			// ----------------------------------
			m_Rx.pBuff[m_Rx.DataCount++] = cur;
		
			if( m_Rx.DataCount >= m_Rx.CurrentPacketLength ) {
				// ----------------------------------
				//	全データ受信完了
				// ----------------------------------
			
				if( _VerifyChecksum() ) {
					// チェックサムOK
			
					// ペイロードのデコード
					if( m_Communicator.Decode(*(m_Rx.pBuff + PREAMBLE_LENGTH), m_Rx.pBuff + HEADER_LENGTH, m_Rx.CurrentPacketLength - HEADER_LENGTH - FOOTER_LENGTH) ) {
						// デコード成功
					
					} else {
						// デコード失敗	
						
					}
					
					// 受信バッファクリア
					_ClearRxBuffer();
					
				} else {
					// チェックサムNG	
					_EraseJunkData();
				}
			}
		}

		// バッファがあふれそう
		if( m_Rx.DataCount >= m_Rx.BufferLength ) {
			_EraseJunkData();
		}
	}
	
	return C_OK;
}

void Protocol_Legacy::_ClearRxBuffer()
{
	m_Rx.DataCount           = 0;
	m_Rx.CurrentPacketLength = 0;
}

_UWORD Protocol_Legacy::_CalculateChecksum(unsigned char ID, const void* pData, size_t length)
{
	const _UBYTE* pSrc = reinterpret_cast<const _UBYTE*>(pData);
	_UWORD  sum = ID;
	
	for( size_t i = 0; i < length; i++ ) {
		sum += *pSrc++;
	}
	
	return sum;
}

bool Protocol_Legacy::_VerifyChecksum() const
{	
	_UWORD Checksum	= (m_Rx.pBuff[m_Rx.DataCount-2] << 8) | m_Rx.pBuff[m_Rx.DataCount-1];
	
	return Checksum == _CalculateChecksum(*(m_Rx.pBuff + PREAMBLE_LENGTH), m_Rx.pBuff + HEADER_LENGTH, m_Rx.DataCount - HEADER_LENGTH - FOOTER_LENGTH);
}

void Protocol_Legacy::_ClearMemory()
{
	_ClearRxBuffer();
}


void Protocol_Legacy::_EraseJunkData()
{
	// バッファを探索し，同期バイトが見つかれば切り詰める
	// 削除したデータはデバイスクラスに処理させる
	//
	// パターン1
	// 0xFF, 0xFF, ゴミ1, ゴミ2, ゴミ3, ゴミ4, ゴミ5
	//     デバイスクラスに渡すデータ：0xFF, 0xFF, ゴミ1, ゴミ2, ゴミ3, ゴミ4, ゴミ5
	//     処理後のバッファ：空
	//
	// パターン2
	// 0xFF, 0xFF, ゴミ1, ゴミ2, 0xFF, 0xFF, Any
	//     デバイスクラスに渡すデータ：0xFF, 0xFF, ゴミ1, ゴミ2
	//     処理後のバッファ：0xFF, 0xFF, Any
	//
	// パターン3
	// 0xFF, 0xFF, ゴミ1, ゴミ2, ゴミ3, ゴミ4, 0xFF
	//     デバイスクラスに渡すデータ：0xFF, 0xFF, ゴミ1, ゴミ2, ゴミ3, ゴミ4
	//     処理後のバッファ：0xFF
	//
	// テストデータ：
	//     pBuff(初期データ)                     pBuff(処理後)    / ProcessUnusedCharacter 関数に渡されたデータ
	//     0xFF, 0xFF                         -> 空               / 0xFF, 0xFF
	//     0xFF, 0xFF, 0xFF                   -> 0xFF, 0xFF       / 0xFF
	//     0xFF, 0xFF, 0x00                   -> 空               / 0xFF, 0xFF, 0x00
	//     0xFF, 0xFF, 0x00, 0x00, 0x00       -> 空               / 0xFF, 0xFF, 0x00, 0x00, 0x00
	//     0xFF, 0xFF, 0x00, 0x00, 0xFF       -> 0xFF             / 0xFF, 0xFF, 0x00, 0x00
	//     0xFF, 0xFF, 0x00, 0xFF, 0xFF       -> 0xFF, 0xFF       / 0xFF, 0xFF, 0x00
	//     0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00 -> 0xFF, 0xFF, 0x00 / 0xFF, 0xFF, 0x00
	//     0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00 -> 空               / 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00
	//
	
	// 先頭+1から0xFF, 0xFFまではごみ．バッファにあるデータが2バイト以下の場合は全削除
	if( m_Rx.DataCount <= PREAMBLE_LENGTH ) {
		for( int i = 0; i < PREAMBLE_LENGTH; i++ ) {
			m_WirelessDevice.ProcessUnusedCharacter(m_Rx.pBuff[i]);
		}
		m_Rx.DataCount = 0;
		return;
	}

	unsigned char Prev = m_Rx.pBuff[PREAMBLE_LENGTH-1];
	int           nDelete = 0; // 削除するバイト数

	// 0xFF, 0xFF を探す
	for( int i = PREAMBLE_LENGTH; i < m_Rx.DataCount; i++ ) {
		unsigned char Cur = m_Rx.pBuff[i];

		if( (Prev == PREAMBLE1) && (Cur == PREAMBLE2) ) {
			// ヒット
			nDelete = i - 1;
			break;
		}

		Prev = Cur;
	}
	
	// 0xFF, 0xFF が見つからなかった場合
	if( nDelete == 0 ) {
		if( m_Rx.pBuff[m_Rx.DataCount-1] == PREAMBLE1 ) {
			// 最後の1バイトが 0xFF だった場合
			nDelete = m_Rx.DataCount-1;
		} else {
			// どこにも 0xFF, 0xFF が無かった場合
			nDelete = m_Rx.DataCount;
		}
	}
	
	// デバイスクラスにパケットとして認識されなかったデータを処理させる
	for( int i = 0; i < nDelete; i++ ) {
		m_WirelessDevice.ProcessUnusedCharacter(m_Rx.pBuff[i]);	
	}
	
	// データの切り詰め
	for( int i = 0; i < m_Rx.DataCount - nDelete; i++ ) {
		m_Rx.pBuff[i] = m_Rx.pBuff[i + nDelete];
	}
	m_Rx.DataCount -= nDelete;
}

size_t Protocol_Legacy::GetRxBufferMinimumRequirement() const
{
	return HEADER_LENGTH + FOOTER_LENGTH;	
}
