//
//	可変長FIFOバッファ
//
//	一つの回線に対し一つの基本型のパケットを使用してください．
//	異なる基本型のパケットを同一回線で混在させることはできません．
//
//	バージョン２構想：
//		スレッドセーフに設計して，ブロックが重ならない限りバッファへの追加と読み出しが同時に行えるようにする．
//
//	変更履歴：
//		2010/10/20  サイズ用データの型を unsigned long から size_t へ変更
//		            NO_LIMIT 定数を追加
//		2010/11/22  スタティックライブラリ化
//		            ブロックサイズと最大バッファサイズを変更できるように修正
//		2012/04/22	TxTempBuffer に関数を追加 (GetLength)
//					一部を const 関数化
//
//

#pragma once
#include <list>

namespace CommBuffer {

	//*******************************************************************************
	// 受信関数に渡すためのバッファのポインタと長さ
	//*******************************************************************************
	struct BUFFER
	{
		void* pBuff;
		const size_t nLen;

		BUFFER(void* _pBuff, size_t _nLen) : pBuff(_pBuff), nLen(_nLen) {}
		BUFFER(const BUFFER& obj) : pBuff(obj.pBuff), nLen(obj.nLen) {}
		BUFFER operator = (const BUFFER& obj) {
			pBuff = obj.pBuff;
			*(const_cast<size_t*>(&nLen)) = obj.nLen;
			return *this;
		}
	};

	struct CBUFFER
	{
		const void* pBuff;
		const size_t nLen;

		CBUFFER(const void* _pBuff, size_t _nLen) : pBuff(_pBuff), nLen(_nLen) {}
		CBUFFER(const CBUFFER& obj) : pBuff(obj.pBuff), nLen(obj.nLen) {}
		CBUFFER operator = (const CBUFFER& obj) {
			pBuff = obj.pBuff;
			*(const_cast<size_t*>(&nLen)) = obj.nLen;
			return *this;
		}
	};

	//*******************************************************************************
	// 送信/受信バッファの基本クラス
	//*******************************************************************************
	class BufferBase
	{
	public:
		enum {
			DEFAULT  = 1024,
			NO_LIMIT = -1,	
		};

	protected:
		struct DATABLOCK
		{
			unsigned char* pBuff;
			const size_t   nSize;   // バッファのサイズ
			size_t         nFilled; // 入力済みデータ長

			DATABLOCK() : pBuff(0), nSize(0), nFilled(0) {}
			DATABLOCK(size_t nLen) : pBuff(new unsigned char[nLen]),
				                 nSize(nLen),
			                         nFilled(0) {}
		};

		typedef ::std::list<DATABLOCK> BlockList;

		size_t    m_nBlockSize;   // ブロック単体の最大サイズ
		size_t    m_nMaximumSize; // バッファ全体の最大サイズ

		BlockList m_BlockList;
		size_t    m_nReadFromFront; // List の先頭から何バイト読み出したか
		size_t    m_nBuffLeftLen;   // バッファの未入力領域の長さ
		size_t    m_nTotalBuffLen;  // 確保した領域の合計

	public:
		BufferBase(size_t nBlockSize = DEFAULT, size_t nMaximumSize = NO_LIMIT);
		virtual ~BufferBase();

		// 設定変更用関数
		void SetBlockSize(size_t nBlockSize);
		void SetMaximumSize(size_t nMaximumSize);

		// バッファ操作用関数
		void Skip(size_t nSkip); // 指定した数（単位：バイト）だけ読み出しポイントをシークする
		void Clear();            // バッファをクリアする

		// 状態取得用関数
		size_t GetLength() const;     // 有効データ長を取得
		bool   IsAllocatable() const; // 新たにブロックを拡張可能かどうか調べる

	protected:
		void _ReleaseUntil(BlockList::iterator it);
	};


	//*******************************************************************************
	// 受信データを管理するためのクラス
	//
	// 一定サイズのブロック単位で受信バッファを拡張する．
	//*******************************************************************************
	class RxBufferBase : public BufferBase
	{
	protected:
		virtual bool _Pop(void* pData, size_t nSize, size_t nLen) = 0;
		virtual bool _PopArray(void* pDest, size_t nItemSize, size_t nItem) = 0;
		virtual bool _Read(size_t nOffset, void* pData, size_t nSize, size_t nLen) const = 0;

	public:
		RxBufferBase(size_t nBlockSize = DEFAULT, size_t nMaximumSize = NO_LIMIT);
		virtual ~RxBufferBase();

		// データセット用関数
		BUFFER PushBackBegin();
		bool   PushBackEnd(size_t ReceiveSize);

		// データ読み出し用関数
		bool Seek(const unsigned char* pPattern, size_t nSize);
		bool Seek(const char* pPattern, size_t nSize);
		
		// データ抽出用関数
		template<typename T> bool Pop(T* pData, size_t nLen = 0)
		{
			if( nLen == 0 ) nLen = sizeof(T);

			if( sizeof(T) < nLen )   return false; // sizeof(T) 以下しか指定できない
			if( nLen > GetLength() ) return false; // 読み出し可能サイズを上回る
			
			return _Pop(pData, sizeof(T), nLen);
		}

		template<typename T> bool PopArray(T* pDest, size_t nItem)
		{
			if( sizeof(T) * nItem > GetLength() ) return false; // 読み出し可能サイズを上回る

			return _PopArray(pDest, sizeof(T), nItem);
		}

		template<typename T> bool Read(size_t nOffset, T* pData, size_t nLen = 0) const
		{
			if( nLen == 0 ) nLen = sizeof(T);

			if( sizeof(T) < nLen )             return false; // sizeof(T) 以下しか指定できない
			if( nLen + nOffset > GetLength() ) return false; // 読み出し可能サイズを上回る

			return _Read(nOffset, pData, sizeof(T), nLen);
		}
		
		// チェックサム計算は継承＆コードの雛形を用意することで解決する

	};

	//*******************************************************************************
	// 送信データを管理するためのクラス
	//
	// 固定サイズの送信バッファ
	// バックグラウンド型の非同期送信には使えない
	//*******************************************************************************
	class TxTempBufferBase
	{
	protected:
		unsigned char* m_pBuff;
		size_t         m_nBuffSize; // バッファサイズ（空白領域も含む）
		size_t         m_nDataLen;  // 既に入力されたデータの量（意味のあるデータの長さ）

	protected:
		virtual bool _Push(const void* pData, size_t nSize, size_t nLen) = 0;
		virtual bool _PushArray(const void* pData, size_t nItemSize, size_t nItem) = 0;

	public:
		TxTempBufferBase(size_t nSize);
		virtual ~TxTempBufferBase();

		// データセット用関数
		template<typename T> bool Push(T Data, size_t nLen = 0)
		{
			if( nLen <= 0 ) nLen = sizeof(T);

			if( nLen > sizeof(T) )                return false; // 引数不正
			if( m_nDataLen + nLen > m_nBuffSize ) return false; // バッファサイズを上回る

			return _Push(&Data, sizeof(T), nLen);
		}

		template<typename T> bool PushArray(const T* pData, size_t nItem)
		{
			if( m_nDataLen + sizeof(T) * nItem > m_nBuffSize ) return false; // バッファサイズを上回る

			return _PushArray(pData, sizeof(T), nItem);
		}

		// 状態取得用関数
		size_t GetLength() const;

		// データ読み出し用関数
		CBUFFER GetBuffer() const;

		// チェックサムにはクラスの継承で対応する
	};

	//*******************************************************************************
	// 送信データを管理するためのクラス
	//
	// 一定サイズ以上のブロック単位でバッファを拡張する．
	// 最大サイズを設定するとバッファの最大サイズをオーバーするようなデータはセットできなくなる．
	//*******************************************************************************
	class TxBufferBase : public BufferBase
	{
	protected:
		virtual bool _Push(const void* pData, size_t nSize, size_t nLen) = 0;
		virtual bool _PushArray(const void* pData, size_t nItemSize, size_t nItem) = 0;

	public:
		TxBufferBase(size_t nBlockSize = DEFAULT, size_t nMaximumSize = NO_LIMIT);
		virtual ~TxBufferBase();

		// データセット用関数
		template<typename T> bool Push(T Data, size_t nLen = 0)
		{
			if( nLen <= 0 ) nLen = sizeof(T);

			if (nLen > sizeof(T)) return false; // 引数不正
			if( m_nTotalBuffLen - m_nBuffLeftLen + nLen > m_nMaximumSize ) return false; // バッファサイズを上回る

			return _Push(&Data, sizeof(T), nLen);
		}

		template<typename T> bool PushArray(const T* pData, size_t nItem)
		{
			if( m_nTotalBuffLen - m_nBuffLeftLen + sizeof(T) * nItem > m_nMaximumSize ) return false; // バッファサイズを上回る

			return _PushArray(pData, sizeof(T), nItem);
		}

		// データ読み出し用関数
		CBUFFER PopFrontBegin();
		bool    PopFrontEnd(size_t SendSize);
	};


	//*******************************************************************************
	// ビッグエンディアンのアライメントで通信を行うバージョン
	//*******************************************************************************
	namespace BigEndianProtocol {
	
		class RxBuffer : public RxBufferBase
		{
		public:
			RxBuffer(size_t nBlockSize = DEFAULT, size_t nMaximumSize = NO_LIMIT) : RxBufferBase(nBlockSize, nMaximumSize) {}

		protected:
			virtual bool _Pop(void* pData, size_t nSize, size_t nLen);
			virtual bool _PopArray(void* pDest, size_t nItemSize, size_t nItem);
			virtual bool _Read(size_t nOffset, void* pData, size_t nSize, size_t nLen) const;
		};

		class TxTempBuffer : public TxTempBufferBase
		{
		public:
			TxTempBuffer(size_t nBufferSize) : TxTempBufferBase(nBufferSize) {}

		protected:
			virtual bool _Push(const void* pData, size_t nSize, size_t nLen);
			virtual bool _PushArray(const void* pData, size_t nItemSize, size_t nItem);
		};

		class TxBuffer : public TxBufferBase
		{
		public:
			TxBuffer(size_t nBlockSize, size_t nMaximumSize = NO_LIMIT) : TxBufferBase(nBlockSize, nMaximumSize) {}

		protected:
			virtual bool _Push(const void* pData, size_t nSize, size_t nLen);
			virtual bool _PushArray(const void* pData, size_t nItemSize, size_t nItem);
		};
	}

	//*******************************************************************************
	// リトルエンディアンのアライメントで通信を行うバージョン
	//*******************************************************************************
	namespace LittleEndianProtocol {

		class RxBuffer : public RxBufferBase
		{
		public:
			RxBuffer(size_t nBlockSize = DEFAULT, size_t nMaximumSize = NO_LIMIT) : RxBufferBase(nBlockSize, nMaximumSize) {}

		protected:
			virtual bool _Pop(void* pData, size_t nSize, size_t nLen);
			virtual bool _PopArray(void* pDest, size_t nItemSize, size_t nItem);
			virtual bool _Read(size_t nOffset, void* pData, size_t nSize, size_t nLen) const;
		};

		class TxTempBuffer : public TxTempBufferBase
		{
		public:
			TxTempBuffer(size_t nBufferSize) : TxTempBufferBase(nBufferSize) {}

		protected:
			virtual bool _Push(const void* pData, size_t nSize, size_t nLen);
			virtual bool _PushArray(const void* pData, size_t nItemSize, size_t nItem);
		};

		class TxBuffer : public TxBufferBase
		{
		public:
			TxBuffer(size_t nBlockSize = DEFAULT, size_t nMaximumSize = NO_LIMIT) : TxBufferBase(nBlockSize, nMaximumSize) {}

		protected:
			virtual bool _Push(const void* pData, size_t nSize, size_t nLen);
			virtual bool _PushArray(const void* pData, size_t nItemSize, size_t nItem);
		};
	}
}
