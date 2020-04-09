///
/// @file       Memory.h
///
/// @brief      メモリ領域の割り当てを隠蔽するためのクラス群
///
///             メモリ領域の割り当てを隠蔽し、配列クラスを介してメモリ領域にアクセスする機能を提供します
///             ビルドの設定によってヒープから割り当てるか、SDRAMのアドレス空間から割り当てるか切り替えることができます
///
/// @author     iwakura
///


#ifndef INCLUDED_MEMORY_H
#define INCLUDED_MEMORY_H

#include "Common/StandardDef.h"

namespace Memory
{
	///
	/// @brief  RAM抽象クラス
	///
	class IRam
	{
	public:
		///
		/// @brief  デストラクタ
		///
		virtual ~IRam() {};
	
		///
		/// @brief  メモリ領域の確保（純粋仮想関数）
		///
		virtual _UDWORD* Allocate(size_t size) = 0;
	};

	///
	/// @brief  RAM具象クラス（内臓RAM）
	///
	///         旧ユニットやWindowsでビルドする場合に使用する
	///
	class EmbeddedRam : public IRam
	{
		_UDWORD* m_ptr;
	
	public:
	
		///
		/// @brief  コンストラクタ
		///
		EmbeddedRam();
	
		///
		/// @brief  デストラクタ
		///
		virtual ~EmbeddedRam();
	
		///
		/// @brief  メモリ領域の確保
		///
		/// @param[in] size 確保するバイト数
		/// @return 領域の先頭のポインタ / NULL - 領域確保に失敗
		///
		virtual _UDWORD* Allocate(size_t size);
	};


	///
	/// @brief  コンパイルスイッチを隠蔽するためのインスタンス生成クラス
	///
	class RamCreator
	{
	public:
		///
		/// @brief  インスタンスの生成
		///
		IRam* Create();	
	};

	///
	/// @brief  型を確定するためのテンプレートクラス
	///
	template<class T> class TypedMemoryBlock
	{
		IRam* m_pRam;
	
	public:
	
		///
		/// @brief  コンストラクタ
		///
		TypedMemoryBlock()
		{
			RamCreator creator; 
			m_pRam = creator.Create(); // RAMクラスを生成する
		}

		///
		/// @brief  デストラクタ
		///
		virtual ~TypedMemoryBlock()
		{
			delete m_pRam;	
		}

		///
		/// @brief  メモリ領域を確保し、テンプレート引数の型でポインタを返す
		///
		/// @param[in] length 要素数
		/// @return 先頭要素のポインタ / NULL - 領域確保に失敗
		///
		T* Allocate(size_t length)
		{
			return reinterpret_cast<T*>(m_pRam->Allocate(GetAllocateSize(length)));
		}
		
		///
		/// @brief  要素数から確保されるメモリ領域のバイト数を求める
		///
		/// @param[in] length 要素数
		/// @return 確保されるバイト数
		///
		static size_t GetAllocateSize(size_t length)
		{
			T dummy[2];
		
			size_t ElementSize = sizeof(dummy) / 2; // パディングを含めた1要素のサイズ

			dummy[0]; // warning対策
			
			return length * ElementSize;
		}
	};

	///
	/// @brief  配列クラス
	///
	template<class T>class Array
	{
		T*     m_ptr;
		size_t m_length;
		
		TypedMemoryBlock<T> m_memory_block;
	
	public:
	
		///
		/// @brief  コンストラクタ
		///
		///         メモリ領域を確保します
		///
		/// @param[in] length 配列の要素数
		///
		Array(size_t length)
		{
			m_ptr = m_memory_block.Allocate(length);
			m_length = length;
		};
	
		///
		/// @brief  配列的アクセスのサポート
		///
		///         []指定によりオブジェクトを配列としてアクセスできるようにします
		///
		/// @param[in] n インデックス
		/// @return 要素への参照
		///
		inline T& operator [] (int n) const
		{
			return m_ptr[n];
		}
		
		///
		/// @brief  領域確保に成功しているか調べる
		///
		/// @return true - 成功 / false - 失敗
		///
		inline bool IsAllocationSucceeded() const
		{
			return m_ptr != NULL;	
		}
		
		///
		/// @brief  確保済みのメモリ領域のサイズをバイト単位で取得する
		///
		/// @return パディングを含む確保済みのバイト数
		///
		inline size_t GetAllocatedMemorySize() const
		{
			return GetAllocateSize(m_length);
		}
	};
}


#endif