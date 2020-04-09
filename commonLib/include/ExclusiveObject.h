//
//
//	Note:
//	デッドロックの問題を解決する（2回目のロック以降にタイムアウトを設定する）
//	複数オブジェクトの同時ロック手段を提供する
//
//	本ヘッダファイルのインクルード前に #define USE_BOOST_LIBRARY を定義すると，boostのshared_ptrを使用します．
//

#pragma once

#include <windows.h>
#include <memory>



//#define USE_BOOST_LIBRARY

#ifdef USE_BOOST_LIBRARY
#include <boost/shared_ptr.hpp>
#endif

//*******************************************************************************
//	内部クラス
//
//	このインスタンスは CAccessor が保持するスマートポインタによって管理されている．
//	本クラスのインスタンスを参照する CAccessor が１つも無くなったときに，スマート
//	ポインタの開放によってデストラクタが呼ばれ，CExclusiveObject のミューテックス
//	がシグナル状態に戻る．
//	（→オブジェクトへの排他的アクセスが完了した際）
//*******************************************************************************
class _CUnlocker
{
public:
	HANDLE m_hMutex;
	void*  m_pData;

public:
	//----------------------------------------
	// _CUnlocker のコンストラクタ
	//----------------------------------------
	_CUnlocker(HANDLE hMutex, void* pData)
	{
		m_hMutex = hMutex;
		m_pData  = pData;
	}

	//----------------------------------------
	// _CUnlocker のデストラクタ
	//----------------------------------------
	virtual ~_CUnlocker()
	{
		// Mutex のアンロック
		::ReleaseMutex(m_hMutex);
	}
};

//*******************************************************************************
//	アクセス子
//
//	排他制御を受けるデータへアクセスするためのオブジェクト．
//	データへはアロー演算子を用いてアクセスする．
//	
//	注意：
//	アクセス子のインスタンスは自動変数と同じように扱うこと．
//	開放されない限り排他制御は終了しないので，データへのアクセスが終わったら直ちに
//	開放を行うこと．
//	また，開放は必ず親である CExclusiveObject のインスタンスが開放される前に行うこと．
//
//	アクセス子からはデータへのポインタや参照を取得できるが，取得したポインタや参照に
//	対するアクセスは，アクセス子の実体がある間のみ許される．アクセス子が解放された後
//	は取得したポインタや参照にアクセスしてはいけない．
//	この機能は基本的に，関数へポインタや参照を渡さなければならない場合に使用する．
//
//	＜注意点＞
//	func(m_foo.GetAccessor()->bar)
//	のように書くと，関数 func が実行された後でアクセス子の解放が行われる．
//*******************************************************************************
template <class TYPE> class CAccessor
{
	friend class CAccessor;

public:
#ifdef USE_BOOST_LIBRARY
	typedef boost::shared_ptr<_CUnlocker> UNLOCKER_PTR;
#else
	typedef std::tr1::shared_ptr<_CUnlocker> UNLOCKER_PTR;
#endif

private:
	UNLOCKER_PTR m_Unlocker;

	UNLOCKER_PTR m_ParentUnlocker; // テスト中

public:
	//----------------------------------------
	// CAccessor のコンストラクタ
	//----------------------------------------
	CAccessor(HANDLE hMutex, TYPE* pData) : m_Unlocker(UNLOCKER_PTR(new _CUnlocker(hMutex, (void*)(pData)))) {}

	//----------------------------------------
	// CAccessor のコピーコンストラクタ
	//----------------------------------------
	CAccessor(CAccessor& obj)
	{
		m_Unlocker = obj.m_Unlocker;
		m_ParentUnlocker = obj.m_ParentUnlocker;
	}

	//----------------------------------------
	// 上位のアクセス子を登録する（テスト中）
	//
	//		[PARENT] → [this] のようなロック関係を１つのアクセス子で実現したいときに使用する
	//----------------------------------------
	template<typename ACCESSOR> void setParent(ACCESSOR& parent)
	{
		m_ParentUnlocker = parent.m_Unlocker;
	}
	
	//----------------------------------------
	// アクセス権のリリース
	//
	//     データへの排他的アクセスを手動で終了する
	//     release() を行った後はデータへアクセスすることはできない．
	//----------------------------------------
	void release()
	{
		m_Unlocker.reset();
		m_ParentUnlocker.reset();
	}

	//----------------------------------------
	// データのポインタを取得する
	//----------------------------------------
	TYPE* get() const
	{
		return static_cast<TYPE*>(m_Unlocker->m_pData);
	}

	//----------------------------------------
	// データの参照を取得する
	//----------------------------------------
	TYPE& getReference() const
	{
		return *(static_cast<TYPE*>(m_Unlocker->m_pData));
	}

	//----------------------------------------
	// アロー演算子のオーバーロード
	//
	//     排他制御を受けるデータへのアクセス手段を提供する
	//----------------------------------------
	TYPE* operator -> () const
	{
		return static_cast<TYPE*>(m_Unlocker->m_pData);
	}

	//----------------------------------------
	// ポインタへのキャスト
	//----------------------------------------
	operator TYPE*() const
	{
		return static_cast<TYPE*>(m_Unlocker->m_pData);
	}

	//----------------------------------------
	// ポインタの間接参照演算子 * のオーバーロード
	//----------------------------------------
	TYPE& operator * () const
	{
		return *static_cast<TYPE*>(m_Unlocker->m_pData);
	}
};

//*******************************************************************************
//	排他的データオブジェクト
//
//	データへのアクセスを１つのスレッドに限定するためのオブジェクト．
//	データへのアクセスは GetAccessor() で取得した ACCESSOR オブジェクトを通じて行う．
//	ACCESSOR のインスタンスが存在している間，他のスレッドは同一のデータへアクセス
//	することはできない．（ GetAccessor() を呼び出した時点でスレッドがストップする)
//
//	CONST_ACCESSOR は const 関数を実装するときに使用する
//*******************************************************************************
template <class TYPE> class CExclusiveObject
{
public:
	typedef CAccessor<TYPE> ACCESSOR;
	typedef CAccessor<const TYPE> CONST_ACCESSOR;

private:
	HANDLE m_hMutex;
	TYPE   m_Data;

public:
	//----------------------------------------
	// CExclusiveObject のコンストラクタ
	//----------------------------------------
	CExclusiveObject()
	{
		m_hMutex = ::CreateMutex(NULL, FALSE, NULL);
	}

	//----------------------------------------
	// CExclusiveObject のコンストラクタ（初期値指定）
	//----------------------------------------
	CExclusiveObject(const TYPE& obj) : m_Data(obj)
	{
		m_hMutex = ::CreateMutex(NULL, FALSE, NULL);
	}

	//----------------------------------------
	// CExclusiveObject のデストラクタ
	//----------------------------------------
	virtual ~CExclusiveObject()
	{
		::CloseHandle(m_hMutex);
	}

	//----------------------------------------
	// アクセス子を取得する
	//----------------------------------------
	virtual ACCESSOR GetAccessor()
	{
		// Mutex のロック
		::WaitForSingleObject(m_hMutex, INFINITE);

		return ACCESSOR(m_hMutex, &m_Data);
	}

	//----------------------------------------
	// const アクセス子を取得する
	//----------------------------------------
	virtual CONST_ACCESSOR GetConstAccessor() const
	{
		// Mutex のロック
		::WaitForSingleObject(m_hMutex, INFINITE);

		return CONST_ACCESSOR(m_hMutex, &m_Data);
	}
};


//*******************************************************************************
//	排他的ポインタオブジェクト
//
//	排他的データオブジェクトのポインタ版
//
//	とりあえずデータは普通のポインタで保持
//*******************************************************************************
template <class TYPE> class CExclusivePointer
{
public:
	typedef CAccessor<TYPE> ACCESSOR;

private:
	HANDLE m_hMutex;
	TYPE*  m_pData;
	//std::tr1::shared_ptr<TYPE> m_DataPtr;

public:
	CExclusivePointer()
	{
		m_pData  = NULL;
		m_hMutex = ::CreateMutex(NULL, FALSE, NULL);
		m_AutoDelete = FALSE;
	}

	CExclusivePointer(TYPE* pData)
	{
		m_pData  = pData;
		//m_DataPtr = std::tr1::shared_ptr<TYPE>(pData);
		m_hMutex = ::CreateMutex(NULL, FALSE, NULL);
	}

	virtual ~CExclusivePointer()
	{
		::CloseHandle(m_hMutex);
		if( m_pData ) {
			delete m_pData;
		}
	}

	//----------------------------------------
	// アクセス子を取得する
	//----------------------------------------
	ACCESSOR GetAccessor() 
	{
		// Mutex のロック
		::WaitForSingleObject(m_hMutex, INFINITE);

		//return ACCESSOR(m_pMutex, m_DataPtr.get());
		return ACCESSOR(m_hMutex, m_pData);
	}
};