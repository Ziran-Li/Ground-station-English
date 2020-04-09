
#include "stdafx.h"
#include "CommBuffer.h"
#include "common/cpuinfo.h"
#include <string.h>

using namespace CommBuffer;

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■	BufferBase
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//*******************************************************************************
//	仕様：
//		常に m_nBlockSize    <= m_nMaximumSize が成り立つ
//		常に m_nTotalBuffLen <= m_nMaximumSize が成り立つ
//*******************************************************************************

//*******************************************************************************
//	BufferBase のコンストラクタ
//*******************************************************************************
BufferBase::BufferBase(size_t nBlockSize, size_t nMaximumSize) : 
m_nBlockSize(nMaximumSize >= nBlockSize ? nBlockSize : nMaximumSize),
m_nMaximumSize(nMaximumSize),
m_nReadFromFront(0),
m_nBuffLeftLen(0),
m_nTotalBuffLen(0)
{
}

//*******************************************************************************
//	BufferBase のデストラクタ
//*******************************************************************************
BufferBase::~BufferBase() 
{
	BlockList::iterator pp = m_BlockList.begin();

	for( ; pp != m_BlockList.end(); pp++ ) {
		delete pp->pBuff;
	}
}

//*******************************************************************************
//	ブロックサイズを再設定する
//
//	引数：
//		nBlockSize - 新しいブロックサイズ
//
//	動作：
//		すでに確保されているブロックのサイズは変更しない
//		これから確保されるブロックから設定値が反映される
//*******************************************************************************
void BufferBase::SetBlockSize(size_t nBlockSize) 
{
	m_nBlockSize = nBlockSize;
}

//*******************************************************************************
//	バッファの最大サイズを再設定する
//
//	引数：
//		nMaximumSize - 新しいブロックサイズ
//
//	動作：
//		ブロックサイズより小さな値，すでに確保されているバッファより小さな値
//		は設定できない
//*******************************************************************************
void BufferBase::SetMaximumSize(size_t nMaximumSize)
{
	if( nMaximumSize < m_nBlockSize    ) nMaximumSize = m_nBlockSize;
	if( nMaximumSize < m_nTotalBuffLen ) nMaximumSize = m_nTotalBuffLen;

	m_nMaximumSize = nMaximumSize;
}

//*******************************************************************************
//	指定バイト数分バッファの先頭からデータを削除する
//
//	引数：
//		nSkip - スキップ量
//
//	動作：
//		スキップされたデータは削除扱いになる
//*******************************************************************************
void BufferBase::Skip(size_t nSkip)
{
	m_nReadFromFront += nSkip;
	BlockList::iterator pp = m_BlockList.begin();

	for( ; pp != m_BlockList.end() ; pp++ ) {
		if( m_nReadFromFront >= pp->nFilled ) {
			m_nReadFromFront -= pp->nFilled;
		} else {
			break;
		}
	}

	if( pp == m_BlockList.end() ) {
		// 完全削除

		if( m_BlockList.size() ) {
			// 1つ以上ブロックが存在する
			_ReleaseUntil(--pp); // 最後のブロックを残して開放

			pp->nFilled = 0;
			m_nReadFromFront = 0;
			m_nBuffLeftLen   = pp->nSize;
		}
	} else {
		_ReleaseUntil(pp);
	}
}

//*******************************************************************************
//	バッファのデータをすべて消去する
//*******************************************************************************
void BufferBase::Clear()
{
	_ReleaseUntil(m_BlockList.end());
	m_nReadFromFront = 0;
	m_nBuffLeftLen   = 0;
}

//*******************************************************************************
//	有効なデータの長さ
//*******************************************************************************
size_t BufferBase::GetLength() const
{
	return m_nTotalBuffLen - m_nReadFromFront - m_nBuffLeftLen;
}

//*******************************************************************************
//	新たにブロックを拡張可能かどうか調べる
//*******************************************************************************
bool BufferBase::IsAllocatable() const
{
	return m_nTotalBuffLen < m_nMaximumSize;
}

//*******************************************************************************
//	指定されたイテレータの前までバッファを開放する
//*******************************************************************************
void BufferBase::_ReleaseUntil(RxBufferBase::BlockList::iterator it)
{
	BlockList::iterator pp = m_BlockList.begin();
	for( ; pp != it; pp++ ) {
		delete pp->pBuff;
		m_nTotalBuffLen -= pp->nSize;
	}

	m_BlockList.erase(m_BlockList.begin(), pp);
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■	RxBufferBase
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//*******************************************************************************
//	RxBufferBase のコンストラクタ
//*******************************************************************************
RxBufferBase::RxBufferBase(size_t nBlockSize, size_t nMaximumSize) : 
BufferBase(nBlockSize, nMaximumSize)
{
}

//*******************************************************************************
//	RxBufferBase のデストラクタ
//*******************************************************************************
RxBufferBase::~RxBufferBase()
{
}

//*******************************************************************************
//	受信データの登録を開始する
//
//	戻り値：
//		受信関数に渡すためのバッファのポインタとバッファのサイズが入った BUFFER
//		構造体
//
//	コーディング例：
//		・戻り値に受信したデータの長さを返す受信関数を次のように定義する
//			int receive(void* pBuff, int nBuffSize)
//		・RxBuffer のインスタンスの変数名は Rxb とする
//		
//		while( true ) {
//			BUFFER buf = Rxb.PushBackBegin();
//			int nReceive = receive(buf.pBuff, buf.nLen);
//			if( !Rxb.PushBackEnd(nReceive) ) break;
//		};
//
//	注意事項：
//		PushBackBegin ～ PushBackEnd の間に RxBuffer のメンバ関数を挟まないこと
//*******************************************************************************
BUFFER RxBufferBase::PushBackBegin() 
{
	void*  pReturnBuff;
	size_t ReturnBuffLen;

	if( m_nBuffLeftLen == 0 ) {
		// 空きが無いとき

		if( m_nTotalBuffLen <= m_nMaximumSize - m_nBlockSize ) { // ※仕様よりアンダーフローしない
			// 最大サイズ(m_nBlockSize) で領域を確保可能

			DATABLOCK block(m_nBlockSize); // 内部で new
			pReturnBuff   = block.pBuff;
			ReturnBuffLen = m_nBuffLeftLen = block.nSize;
			m_BlockList.push_back(block);
			m_nTotalBuffLen += ReturnBuffLen;

		} else if( m_nTotalBuffLen < m_nMaximumSize) {
			// m_nTotalBuffLen が m_nMaximumSize に達するまで領域を確保可能

			DATABLOCK block(m_nMaximumSize - m_nTotalBuffLen); // 内部で new
			pReturnBuff   = block.pBuff;
			ReturnBuffLen = m_nBuffLeftLen = block.nSize;
			m_BlockList.push_back(block);
			m_nTotalBuffLen += ReturnBuffLen;

		} else {
			// 領域確保不能
			pReturnBuff   = NULL;
			ReturnBuffLen = 0;
		}

	} else {
		// まだ空きがあるとき
		BlockList::iterator pp = --m_BlockList.end();
		pReturnBuff   = pp->pBuff + pp->nFilled;
		ReturnBuffLen = m_nBuffLeftLen;
	}
	return BUFFER(pReturnBuff, ReturnBuffLen);
}

//*******************************************************************************
//	受信データの登録を完了する
//
//	引数：
//		ReceiveSize - 受信関数が返した受信バイト数
//
//	コーディング例：
//		・戻り値に受信したデータの長さを返す受信関数を次のように定義する
//			int receive(void* pBuff, int nBuffSize)
//		・RxBuffer のインスタンスの変数名は Rxb とする
//		
//		while( true ) {
//			BUFFER buf = Rxb.PushBackBegin();
//			int nReceive = receive(buf.pBuff, buf.nLen);
//			if( !Rxb.PushBackEnd(nReceive) ) break;
//		};
//
//	戻り値：
//		true  - PushBackBegin で発行したバッファのサイズと ReceiveSize が等しいとき
//		        ⇒システムの受信バッファにまだデータが残っている可能性がある
//		false - PushBackBegin で発行したバッファのサイズより ReceiveSize が小さい
//		        とき / 受信したデータがバッファの最大値に達したとき / エラー時
//		        ⇒システムの受信バッファにデータは無い or 続行不能
//*******************************************************************************
bool RxBufferBase::PushBackEnd(size_t ReceiveSize)
{
	BlockList::iterator footit = --m_BlockList.end();

	if( ReceiveSize > m_nBuffLeftLen ) {
		// 例外
		m_nBuffLeftLen = 0;
		footit->nFilled = footit->nSize;
	} else {
		m_nBuffLeftLen -= ReceiveSize;
		footit->nFilled += ReceiveSize;
	}

	return (m_nBuffLeftLen == 0) && IsAllocatable();
}

//*******************************************************************************
//	指定されたデータ列と一致する箇所まで内部データの先頭位置を移動する
//
//	引数：
//		pPattern - 検索したいバイト列の先頭のポインタ
//		nSize    - データ列の大きさ
//
//	戻り値：
//		true  - 完全一致した箇所があった場合
//		false - 上記以外
//
//	完全一致箇所がなかった場合の動作：
//		内部データの最後部で指定バイト列と一部一致した場合は，その先頭箇所まで
//		先頭位置を移動する．
//		全く一致する箇所がなかった場合は RxBuffer は初期化される
//*******************************************************************************
bool RxBufferBase::Seek(const unsigned char* pPattern, size_t nSize)
{
	int nMatch = 0;
	BlockList::iterator FirstMatchBlock;
	size_t              FirstMatchIndex;

	BlockList::iterator pp = m_BlockList.begin();

	for( ; pp != m_BlockList.end(); pp++ ) {

		unsigned char* pTgt = pp->pBuff + m_nReadFromFront;

		for( ; m_nReadFromFront < pp->nFilled; m_nReadFromFront++ ) { // ブロック内の有効データの末尾までループ

			if( *(pTgt++) == *((unsigned char*)pPattern + nMatch) ) {
				// pPattern の nMatch 番目まで一致
				if( nMatch == 0 ) {
					FirstMatchBlock = pp;               // 先頭が一致したときのイテレータを記録（ブロックの削除に使用）
					FirstMatchIndex = m_nReadFromFront; // 先頭が一致したときのバッファのインデックスを記録
				}

				nMatch++;

				if( nMatch == nSize ) {
					// 完全一致
					_ReleaseUntil(FirstMatchBlock); // FirstMatchBlock の一個前までのブロックを削除
					m_nReadFromFront = FirstMatchIndex;
					break;
				}

			} else {
				// 一致しなかったときはやり直し
				nMatch = 0;
			}
		}

		if( nMatch == nSize ) break;

		m_nReadFromFront = 0;
	}

	//
	if( nMatch == 0 ) {
		// 最後の最後まで見つからなかった → 1ブロック残してリセット
		if( m_BlockList.size() ) {
			// 1ブロック以上ある
			_ReleaseUntil(--m_BlockList.end());
			BlockList::iterator pp = m_BlockList.begin();
			pp->nFilled = 0;
			m_nReadFromFront = 0;
			m_nBuffLeftLen   = pp->nSize;
		}
	} else {
		// 完全一致 or バッファの最後で一部一致
		_ReleaseUntil(FirstMatchBlock);
		m_nReadFromFront = FirstMatchIndex;
	}

	return nMatch == nSize;
}

bool RxBufferBase::Seek(const char *pPattern, size_t nSize)
{
	return Seek(reinterpret_cast<const unsigned char*>(pPattern), nSize); // バイナリ列の比較なので大丈夫
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■	TxTempBufferBase
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//*******************************************************************************
//	TxTempBufferBase のコンストラクタ
//*******************************************************************************
TxTempBufferBase::TxTempBufferBase(size_t nSize)
{
	m_pBuff = new unsigned char[nSize];
	m_nBuffSize = nSize;
	m_nDataLen = 0;
}

//*******************************************************************************
//	TxTempBufferBase のデストラクタ
//*******************************************************************************
TxTempBufferBase::~TxTempBufferBase()
{
	delete m_pBuff;
}

//*******************************************************************************
//	有効データ長を取得する
//*******************************************************************************
size_t TxTempBufferBase::GetLength() const
{
	return m_nDataLen;
}

//*******************************************************************************
//	送信関数に渡すバッファを示す構造体を取得する
//*******************************************************************************
CBUFFER TxTempBufferBase::GetBuffer() const
{
	return CBUFFER(m_pBuff, m_nDataLen);
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■	TxBufferBase
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//*******************************************************************************
//	TxBufferBase のコンストラクタ
//*******************************************************************************
TxBufferBase::TxBufferBase(size_t nBlockSize, size_t nMaximumSize) :
BufferBase(nBlockSize, nMaximumSize)
{
}

//*******************************************************************************
//	TxBufferBase のデストラクタ
//*******************************************************************************
TxBufferBase::~TxBufferBase() 
{
}

//*******************************************************************************
//	バッファの送信を開始する
//
//	戻り値：
//		送信関数に渡すためのバッファのポインタとバッファのサイズが入った CBUFFER
//		構造体
//
//	コーディング例：
//		・戻り値に送信したデータの長さを返す送信関数を次のように定義する
//			int send(void* pBuff, int nBuffSize)
//		・TxBuffer のインスタンスの変数名は Txb とする
//
//		while( true ) {
//			CBUFFER buf = Txb.PopFrontBegin();
//			int nSend = send(buf.pBuff, buf.nLen);
//			if( !Txb.PopFrontEnd(nSend) ) break;
//		}
//
//	注意事項：
//		PopFrontBegin ～ PopFrontEnd の間に TxBuffer のメンバ関数を挟まないこと
//*******************************************************************************
CBUFFER TxBufferBase::PopFrontBegin()
{
	if( m_BlockList.size() ) {
		// バッファが存在する
		BlockList::iterator pp = m_BlockList.begin();

		return CBUFFER(pp->pBuff + m_nReadFromFront, pp->nFilled - m_nReadFromFront);
	} else {
		// バッファが存在しない
		return CBUFFER(NULL, 0);
	}
}

//*******************************************************************************
//	バッファの送信を完了する
//
//	引数：
//		SendSize - 送信関数が返した送信バイト数
//
//	戻り値：
//		true  - PopFrontBegin で発行したバッファのサイズと SendSize が等しいとき
//		        ⇒システムの送信バッファには空きがある可能性がある
//		false - PopFrontBegin で発行したバッファのサイズより SendSize が小さいとき
//		        / 送信できるデータが無いとき / エラー時
//		        ⇒システムの送信バッファに空きは無い or 続行不能
//*******************************************************************************
bool TxBufferBase::PopFrontEnd(size_t SendSize)
{
	bool ret;

	if( SendSize == 0 ) {
		return false;
	}

	BlockList::iterator pp = m_BlockList.begin();

	if( SendSize == pp->nFilled - m_nReadFromFront ) {
		ret = true;
		Skip(pp->nFilled - m_nReadFromFront); // ブロックに含まれるデータのサイズを超えることは無い
	} else {
		ret = false;
		Skip(SendSize);
	}

	if( GetLength() == 0 ) {
		ret = false;
	}

	return ret;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■	BigEndianProtocol::RxBuffer
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//*******************************************************************************
//	データを読み出す
//*******************************************************************************
bool BigEndianProtocol::RxBuffer::_Pop(void* pData, size_t nSize, size_t nLen)
{

#if BYTEORDER == BIG_ENDIAN

	unsigned char* pDest = (unsigned char*)pData + nSize - nLen;
	unsigned char* pEnd  = (unsigned char*)pData + nSize;

	BlockList::iterator pp = m_BlockList.begin();
	for( ; pp != m_BlockList.end(); pp++ ) {
		unsigned char* pSrc = pp->pBuff + m_nReadFromFront;

		while( m_nReadFromFront < pp->nSize ) {
			*(pDest++) = *(pSrc++);
			m_nReadFromFront++;
			if( pDest == pEnd ) goto EndLine;
		}

		m_nReadFromFront = 0;
	}
EndLine:

	_ReleaseUntil(pp);

#elif BYTEORDER == LITTLE_ENDIAN

	unsigned char* pDest = (unsigned char*)pData + nLen;

	BlockList::iterator pp = m_BlockList.begin();
	for( ; pp != m_BlockList.end(); pp++ ) {
		unsigned char* pSrc = pp->pBuff + m_nReadFromFront;

		while( m_nReadFromFront < pp->nFilled ) {
			*(--pDest) = *(pSrc++);
			m_nReadFromFront++;
			if( pDest == pData ) goto EndLine;
		}

		m_nReadFromFront = 0;
	}
EndLine:

	_ReleaseUntil(pp);
	
#else
#error Byte order is not defined.
#endif

	return true;
}

//*******************************************************************************
//	配列データを読み出す
//*******************************************************************************
bool BigEndianProtocol::RxBuffer::_PopArray(void* pDest, size_t nItemSize, size_t nItem)
{

#if BYTEORDER == BIG_ENDIAN

	unsigned char* _pDest = (unsigned char*)pDest;
	unsigned char* pEnd   = (unsigned char*)pDest + nItemSize * nItem;

	BlockList::iterator pp = m_BlockList.begin();
	for( ; pp != m_BlockList.end(); pp++ ) {
		unsigned char* pSrc = pp->pBuff + m_nReadFromFront;

		while( m_nReadFromFront < pp->nSize ) {
			*(_pDest++) = *(pSrc++);
			m_nReadFromFront++;

			if( _pDest == pEnd ) goto EndLine;
		}

		m_nReadFromFront = 0;
	}
EndLine:

	_ReleaseUntil(pp);

#elif BYTEORDER == LITTLE_ENDIAN

	if( nItem == 0 ) return true;

	unsigned char* _pDest       = (unsigned char*)pDest + nItemSize;
	unsigned char* pEnd         = (unsigned char*)pDest + nItemSize * (nItem - 1);
	unsigned char* pElementEnd  = (unsigned char*)pDest;

	BlockList::iterator pp = m_BlockList.begin();
	for( ; pp != m_BlockList.end(); pp++ ) {
		unsigned char* pSrc = pp->pBuff + m_nReadFromFront;

		while( m_nReadFromFront < pp->nFilled ) {
			*(--_pDest) = *(pSrc++);
			m_nReadFromFront++;

			if( _pDest == pEnd ) goto EndLine;

			if( _pDest == pElementEnd ) {
				_pDest       += (nItemSize << 1);
				pElementEnd  += nItemSize;
			}
		}

		m_nReadFromFront = 0;
	}

EndLine:

	_ReleaseUntil(pp);
	
#else
#error Byte order is not defined.
#endif

	return true;
}

//*******************************************************************************
//	読み出し点をシフトせずにデータを読み出す
//*******************************************************************************
bool BigEndianProtocol::RxBuffer::_Read(size_t nOffset, void *pData, size_t nSize, size_t nLen) const
{

#if BYTEORDER == BIG_ENDIAN

	unsigned char* pDest = (unsigned char*)pData + nSize - nLen;
	unsigned char* pEnd  = (unsigned char*)pData + nSize;
	size_t nReadFromFront = m_nReadFromFront + nOffset;

	BlockList::const_iterator pp = m_BlockList.begin();
	for( ; pp != m_BlockList.end(); pp++ ) {
		unsigned char* pSrc = pp->pBuff + nReadFromFront;

		while( nReadFromFront < pp->nDataLen ) {
			*(pDest++) = *(pSrc++);
			nReadFromFront++;
			if( pDest == pEnd ) goto EndLine;
		}

		nReadFromFront -= pp->nDataLen;
	}
EndLine:

#elif BYTEORDER == LITTLE_ENDIAN

	unsigned char* pDest = (unsigned char*)pData + nLen;
	size_t nReadFromFront = m_nReadFromFront + nOffset;

	BlockList::const_iterator pp = m_BlockList.begin();
	for( ; pp != m_BlockList.end(); pp++ ) {
		unsigned char* pSrc = pp->pBuff + nReadFromFront;

		while( nReadFromFront < pp->nFilled ) {
			*(--pDest) = *(pSrc++);
			nReadFromFront++;
			if( pDest == pData ) goto EndLine;
		}

		nReadFromFront -= pp->nFilled;
	}
EndLine:
	
#else
#error Byte order is not defined.
#endif

	return true;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■	BigEndianProtocol::TxTempBuffer
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//*******************************************************************************
//	データを書き込む
//*******************************************************************************
bool BigEndianProtocol::TxTempBuffer::_Push(const void *pData, size_t nSize, size_t nLen)
{
	unsigned char* pDest = m_pBuff + m_nDataLen;

#if BYTEORDER == BIG_ENDIAN

	unsigned char* pSrc = (unsigned char*)pData + nSize - nLen;

	for( size_t i = 0; i < nLen; i++ ) {
		*(pDest++) = *(pSrc++);
	}

#elif BYTEORDER == LITTLE_ENDIAN

	unsigned char* pSrc  = (unsigned char*)pData + nLen;
	
	for( size_t i = 0; i < nLen; i++ ) {
		*(pDest++) = *(--pSrc);
	}

#else
#error Byte order is not defined.
#endif

	m_nDataLen += nLen;

	return true;
}

//*******************************************************************************
//	配列データを書き込む
//*******************************************************************************
bool BigEndianProtocol::TxTempBuffer::_PushArray(const void* pData, size_t nItemSize, size_t nItem)
{
	unsigned char* pDest = m_pBuff + m_nDataLen;

#if BYTEORDER == BIG_ENDIAN

	CopyMemory(pDest, (unsigned char*)pData, nItemSize * nItem);

#elif BYTEORDER == LITTLE_ENDIAN

	unsigned char* pSrc = (unsigned char*)pData + nItemSize;

	for( size_t j = 0; j < nItem; j++ ) {
		for( size_t i = 0; i < nItemSize; i++ ) {
			*(pDest++) = *(--pSrc);
		}
		pSrc += (nItemSize << 1);
	}

#else
#error Byte order is not defined.
#endif

	m_nDataLen += nItem * nItemSize;

	return true;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■	BigEndianProtocol::TxBuffer
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//*******************************************************************************
//	データを書き込む
//*******************************************************************************
bool BigEndianProtocol::TxBuffer::_Push(const void *pData, size_t nSize, size_t nLen)
{
	BlockList::iterator pp;

	if( m_nBuffLeftLen < nLen ) {
		// バッファに空きが無いとき

		size_t nAllocateSize;

		if( nLen - m_nBuffLeftLen > m_nBlockSize ) {
			// 最小ブロックサイズよりも大きいサイズの領域が必要なとき
			nAllocateSize = nLen - m_nBuffLeftLen;
		} else {
			// 最小ブロックサイズよりも小さいサイズの領域が必要なとき
			nAllocateSize = m_nBlockSize;
		}

		DATABLOCK block(nAllocateSize);
		m_BlockList.push_back(block);

		if( m_nBuffLeftLen == 0 ) {
			pp = --m_BlockList.end();
		} else {
			pp = ----m_BlockList.end();
		}

		m_nBuffLeftLen  += nAllocateSize;
		m_nTotalBuffLen += nAllocateSize;

	} else {
		// バッファに十分な空きがあるとき

		pp = -- m_BlockList.end();
	}

#if BYTEORDER == BIG_ENDIAN

	unsigned char* pSrc    = (unsigned char*)pData + nSize - nLen;
	unsigned char* pSrcEnd = pSrc + nLen;
	unsigned char* pDest   = pp->pBuff + pp->nFilled;
	size_t         nFilled = pp->nFilled;

	do {
		*(pDest++) = *(pSrc++);
		nFilled++;
		if( pSrc == pSrcEnd) break;
	} while( nFilled != pp->nSize);
	pp->nFilled = nFilled;

	// 次のブロックへ
	if( ++pp != m_BlockList.end() ) {
		pDest = pp->pBuff;

		do {
			*(pDest++) = *(pSrc++);
		} while( pSrc == pSrcEnd );
		pp->nFilled = nLen - (m_nBuffLeftLen - pp->nSize);
	}

#elif BYTEORDER == LITTLE_ENDIAN

	unsigned char* pSrc    = (unsigned char*)pData + nLen;
	unsigned char* pDest   = pp->pBuff + pp->nFilled;
	size_t         nFilled = pp->nFilled;
	
	do {
		*(pDest++) = *(--pSrc);
		nFilled++;
		if( pSrc == (unsigned char*)pData ) break;
	} while( nFilled != pp->nSize);
	pp->nFilled = nFilled;

	// 次のブロックへ
	if( ++pp != m_BlockList.end() ) {
		pDest = pp->pBuff;

		do {
			*(pDest++) = *(--pSrc);
		} while( pSrc != (unsigned char*)pData );
		pp->nFilled = nLen - (m_nBuffLeftLen - pp->nSize);
	}

#else
#error Byte order is not defined.
#endif

	m_nBuffLeftLen -= nLen;

	return true;
}

//*******************************************************************************
//	配列データを書き込む
//*******************************************************************************
bool BigEndianProtocol::TxBuffer::_PushArray(const void* pData, size_t nItemSize, size_t nItem)
{
	BlockList::iterator pp;
	size_t nAllocateSize = 0;

	if( nItem * nItemSize == 0 ) return true;

	if( m_nBuffLeftLen < nItemSize * nItem ) {
		// バッファに空きが無いとき

		if( nItemSize * nItem - m_nBuffLeftLen > m_nBlockSize ) {
			// 最小ブロックサイズよりも大きいサイズの領域が必要なとき
			nAllocateSize = nItemSize * nItem - m_nBuffLeftLen;
		} else {
			// 最小ブロックサイズよりも小さいサイズの領域が必要なとき
			nAllocateSize = m_nBlockSize;
		}

		DATABLOCK block(nAllocateSize);
		m_BlockList.push_back(block);

		if( m_nBuffLeftLen == 0 ) {
			pp = --m_BlockList.end();
		} else {
			pp = ----m_BlockList.end();
		}

		m_nBuffLeftLen  += nAllocateSize;
		m_nTotalBuffLen += nAllocateSize;

	} else {
		// バッファに十分な空きがあるとき

		pp = --m_BlockList.end();
	}

#if BYTEORDER == BIG_ENDIAN

	if( nAllocateSize ) {
		// 領域の拡張を行った

		memcpy(pp->pBuff + pp->nFilled, pData, m_nBuffLeftLen - nAllocateSize);
		pp->nFilled = pp->nSize;

		pp++;

		pp->nFilled = nItemSize * nItem - (m_nBuffLeftLen - nAllocateSize);
		memcpy(pp->pBuff, (unsigned char*)pData + m_nBuffLeftLen - nAllocateSize, pp->nFilled);

	} else {
		// 領域の拡張を行わなかった

		memcpy(pp->pBuff + pp->nFilled, pData, nItemSize * nItem);
		pp->nFilled += nItemSize * nItem;
	}

#elif BYTEORDER == LITTLE_ENDIAN

	unsigned char* pSrc        = (unsigned char*)pData + nItemSize;
	unsigned char* pDest       = pp->pBuff + pp->nFilled;
	unsigned char* pEnd        = (unsigned char*)pData + nItemSize * (nItem - 1);
	unsigned char* pElementEnd = (unsigned char*)pData;

	size_t nFilled = pp->nFilled;

	if( nAllocateSize ) {
		// 領域の拡張を行った

		unsigned char* pDestEnd = pp->pBuff + pp->nSize;

		do {
			*(pDest++) = *(--pSrc);

			if( pSrc == pElementEnd ) {
				pSrc        += (nItemSize << 1);
				pElementEnd += nItemSize;
			}
		} while( pDest != pDestEnd );
		pp->nFilled = pp->nSize;

		// 次のブロックへ
		pp++;
		pDest = pp->pBuff;

		do {
			*(pDest++) = *(--pSrc);

			if( pSrc == pEnd ) break;

			if( pSrc == pElementEnd ) {
				pSrc        += (nItemSize << 1);
				pElementEnd += nItemSize;
			}
		} while( true );
		pp->nFilled = nItemSize * nItem - (m_nBuffLeftLen - nAllocateSize);

	} else {
		// 領域の拡張を行わなかった

		do {
			*(pDest++) = *(--pSrc);

			if( pSrc == pEnd ) break;

			if( pSrc == pElementEnd ) {
				pSrc        += (nItemSize << 1);
				pElementEnd += nItemSize;
			}
		} while( true );
		pp->nFilled += nItemSize * nItem;
	}

#else
#error Byte order is not defined.
#endif

	m_nBuffLeftLen -= nItemSize * nItem;

	return true;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■	LittleEndianProtocol::RxBuffer
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//*******************************************************************************
//	データを読み出す
//*******************************************************************************
bool LittleEndianProtocol::RxBuffer::_Pop(void* pData, size_t nSize, size_t nLen)
{

#if BYTEORDER == BIG_ENDIAN

#error Not programmed yet.

#elif BYTEORDER == LITTLE_ENDIAN

	unsigned char* pDest = (unsigned char*)pData;
	unsigned char* pEnd  = (unsigned char*)pData + nLen;

	BlockList::iterator pp = m_BlockList.begin();
	for( ; pp != m_BlockList.end(); pp++ ) {
		unsigned char* pSrc = pp->pBuff + m_nReadFromFront;

		while( m_nReadFromFront < pp->nSize ) {
			*(pDest++) = *(pSrc++);
			m_nReadFromFront++;
			if( pDest == pEnd ) goto EndLine;
		}

		m_nReadFromFront = 0;
	}
EndLine:

	_ReleaseUntil(pp);
	
#else
#error Byte order is not defined.
#endif

	return true;
}


//*******************************************************************************
//	配列データを読み出す
//*******************************************************************************
bool LittleEndianProtocol::RxBuffer::_PopArray(void* pDest, size_t nItemSize, size_t nItem)
{

#if BYTEORDER == BIG_ENDIAN

#error Not programmed yet.

#elif BYTEORDER == LITTLE_ENDIAN

	unsigned char* _pDest = (unsigned char*)pDest;
	unsigned char* pEnd   = (unsigned char*)pDest + nItemSize * nItem;

	BlockList::iterator pp = m_BlockList.begin();
	for( ; pp != m_BlockList.end(); pp++ ) {

		// コピーサイズを決定
		size_t nCopy = static_cast<unsigned int>(pEnd - _pDest) > pp->nFilled - m_nReadFromFront ? pp->nFilled - m_nReadFromFront : pEnd - _pDest;
		
		// メモリコピー
		memcpy(_pDest, pp->pBuff + m_nReadFromFront, nCopy);
		_pDest += nCopy;
		m_nReadFromFront += nCopy;

		// 終了判定
		if( _pDest == pEnd ) break;

		m_nReadFromFront = 0;
	}

	_ReleaseUntil(pp);
	
#else
#error Byte order is not defined.
#endif

	return true;
}

//*******************************************************************************
//	読み出し点をシフトせずにデータを読み出す
//*******************************************************************************
bool LittleEndianProtocol::RxBuffer::_Read(size_t nOffset, void *pData, size_t nSize, size_t nLen) const
{

#if BYTEORDER == BIG_ENDIAN

#error Not programmed yet.

#elif BYTEORDER == LITTLE_ENDIAN

	unsigned char* pDest = (unsigned char*)pData;
	unsigned char* pEnd  = (unsigned char*)pData + nLen;
	size_t  nReadFromFront = m_nReadFromFront + nOffset;

	BlockList::const_iterator pp = m_BlockList.begin();
	for( ; pp != m_BlockList.end(); pp++ ) {
		unsigned char* pSrc = pp->pBuff + nReadFromFront;

		while( nReadFromFront < pp->nFilled ) {
			*(pDest++) = *(pSrc++);
			nReadFromFront++;
			if( pDest == pEnd ) goto EndLine;
		}

		nReadFromFront -= pp->nFilled;
	}
EndLine:

#else
#error Byte order is not defined.
#endif

	return true;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■	BigEndianProtocol::TxTempBuffer
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//*******************************************************************************
//	データを書き込む
//*******************************************************************************
bool LittleEndianProtocol::TxTempBuffer::_Push(const void *pData, size_t nSize, size_t nLen)
{
	unsigned char* pDest = m_pBuff + m_nDataLen;

#if BYTEORDER == BIG_ENDIAN

#error Not programmed yet.

#elif BYTEORDER == LITTLE_ENDIAN

	unsigned char* pSrc = (unsigned char*)pData;

	for( size_t i = 0; i < nLen; i++ ) {
		*(pDest++) = *(pSrc++);
	}

#else
#error Byte order is not defined.
#endif

	m_nDataLen += nLen;

	return true;
}

//*******************************************************************************
//	配列データを書き込む
//*******************************************************************************
bool LittleEndianProtocol::TxTempBuffer::_PushArray(const void* pData, size_t nItemSize, size_t nItem)
{
	unsigned char* pDest = m_pBuff + m_nDataLen;

#if BYTEORDER == BIG_ENDIAN

#error Not programmed yet.

#elif BYTEORDER == LITTLE_ENDIAN

	memcpy(pDest, static_cast<const unsigned char*>(pData), nItemSize * nItem);

#else
#error Byte order is not defined.
#endif

	m_nDataLen += nItem * nItemSize;

	return true;
}


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■	BigEndianProtocol::TxBuffer
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//*******************************************************************************
//	データを書き込む
//*******************************************************************************
bool LittleEndianProtocol::TxBuffer::_Push(const void *pData, size_t nSize, size_t nLen)
{
	BlockList::iterator pp;

	if( m_nBuffLeftLen < nLen ) {
		// バッファに空きが無いとき

		size_t nAllocateSize;

		if( nLen - m_nBuffLeftLen > m_nBlockSize ) {
			// 最小ブロックサイズよりも大きいサイズの領域が必要なとき
			nAllocateSize = nLen - m_nBuffLeftLen;
		} else {
			// 最小ブロックサイズよりも小さいサイズの領域が必要なとき
			nAllocateSize = m_nBlockSize;
		}

		DATABLOCK block(nAllocateSize);
		m_BlockList.push_back(block);

		if( m_nBuffLeftLen == 0 ) {
			pp = --m_BlockList.end();
		} else {
			pp = ----m_BlockList.end();
		}

		m_nBuffLeftLen  += nAllocateSize;
		m_nTotalBuffLen += nAllocateSize;

	} else {
		// バッファに十分な空きがあるとき

		pp = -- m_BlockList.end();
	}

#if BYTEORDER == BIG_ENDIAN

#error Not programmed yet.

#elif BYTEORDER == LITTLE_ENDIAN

	unsigned char* pSrc    = (unsigned char*)pData;
	unsigned char* pSrcEnd = pSrc + nLen;
	unsigned char* pDest   = pp->pBuff + pp->nFilled;
	size_t  nFilled = pp->nFilled;

	do {
		*(pDest++) = *(pSrc++);
		nFilled++;
		if( pSrc == pSrcEnd) break;
	} while( nFilled != pp->nSize);
	pp->nFilled = nFilled;

	// 次のブロックへ
	if( ++pp != m_BlockList.end() ) {
		pDest = pp->pBuff;

		do {
			*(pDest++) = *(pSrc++);
		} while( pSrc == pSrcEnd );
		pp->nFilled = nLen - (m_nBuffLeftLen - pp->nSize);
	}

#else
#error Byte order is not defined.
#endif

	m_nBuffLeftLen -= nLen;

	return true;
}

//*******************************************************************************
//	配列データを書き込む
//*******************************************************************************
bool LittleEndianProtocol::TxBuffer::_PushArray(const void* pData, size_t nItemSize, size_t nItem)
{
	BlockList::iterator pp;
	size_t nAllocateSize = 0;

	if( nItem * nItemSize == 0 ) return true;

	if( m_nBuffLeftLen < nItemSize * nItem ) {
		// バッファに空きが無いとき

		if( nItemSize * nItem - m_nBuffLeftLen > m_nBlockSize ) {
			// 最小ブロックサイズよりも大きいサイズの領域が必要なとき
			nAllocateSize = nItemSize * nItem - m_nBuffLeftLen;
		} else {
			// 最小ブロックサイズよりも小さいサイズの領域が必要なとき
			nAllocateSize = m_nBlockSize;
		}

		DATABLOCK block(nAllocateSize);
		m_BlockList.push_back(block);

		if( m_nBuffLeftLen == 0 ) {
			pp = --m_BlockList.end();
			m_nBuffLeftLen  += nAllocateSize;
			m_nTotalBuffLen += nAllocateSize;
			nAllocateSize = 0;
		} else {
			pp = ----m_BlockList.end();
		}

		m_nBuffLeftLen  += nAllocateSize;
		m_nTotalBuffLen += nAllocateSize;

	} else {
		// バッファに十分な空きがあるとき

		pp = --m_BlockList.end();
	}

#if BYTEORDER == BIG_ENDIAN

#error Not programmed yet.

#elif BYTEORDER == LITTLE_ENDIAN

	if( nAllocateSize ) {
		// 領域の拡張を行った

		memcpy(pp->pBuff + pp->nFilled, pData, m_nBuffLeftLen - nAllocateSize);
		pp->nFilled = pp->nSize;

		pp++;

		pp->nFilled = nItemSize * nItem - (m_nBuffLeftLen - nAllocateSize);
		memcpy(pp->pBuff, (unsigned char*)pData + m_nBuffLeftLen - nAllocateSize, pp->nFilled);

	} else {
		// 領域の拡張を行わなかった

		memcpy(pp->pBuff + pp->nFilled, pData, nItemSize * nItem);
		pp->nFilled += nItemSize * nItem;
	}

#else
#error Byte order is not defined.
#endif

	m_nBuffLeftLen -= nItemSize * nItem;

	return true;
}