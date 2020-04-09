#include "stdafx.h"
#include "SCIDeviceBase.h"

//*******************************************************************************
//	構築・破棄時の処理
//*******************************************************************************
CSciDeviceBase::CSciDeviceBase()
{
	m_pSci = NULL;
}

CSciDeviceBase::~CSciDeviceBase()
{
	if( m_pSci ) delete m_pSci;
}

//*******************************************************************************
//	クラスの初期化を行い，指定したシリアルポートへ接続する
//
// Parameter:
//	sciport - 接続したい COM ポートを指定
//	          既に接続済みで，接続中のポートと引数が同じだった場合 :
//	              既存の接続を閉じてから，新しい接続を試みる
//                接続に失敗した場合は，接続先が無い状態になる
//	          既に接続済みで，接続中のポートと引数が異なる場合 :
//	              接続に成功した場合は，新しい接続に切り替わる
//	              接続に失敗した場合は，古い接続を維持する
//
//	br      - 通信速度を指定する
//*******************************************************************************
CRESULT CSciDeviceBase::InitializeSci(SCIPORT sciport, BAUDRATE br, BYTE Parity, BYTE StopBits)
{
	if( sciport == GetCurrentSciPort() ) {
		// 同じシリアルポートを指定してきたとき
		
		delete m_pSci; // m_pSci はインスタンス生成済み
		m_pSci = NULL;
	}

	CSciBase* pSci_tmp = new CSciBase;
	
	if( ISOK(pSci_tmp->Initialize(sciport, br, Parity, StopBits)) ){
		// 初期化に成功

		if( m_pSci ) delete m_pSci; // インスタンスが存在する場合は開放

		m_pSci = pSci_tmp;
		return C_OK;
	} else {
		// 初期化に失敗

		delete pSci_tmp;

		return C_FAIL;
	}
}

//*******************************************************************************
//	COM ポートを閉じる
//*******************************************************************************
CRESULT CSciDeviceBase::CloseSci()
{
	if( m_pSci ) {
		//m_pSci->CloseComPort(); 
		delete m_pSci; 
		m_pSci = NULL;  
		return C_OK;
	} else {
		return C_OK;
	}
}

//*******************************************************************************
//	現在の COM ポートが有効か調べる
//*******************************************************************************
BOOL CSciDeviceBase::IsPortEnable() const
{
	if( m_pSci ) {
		return m_pSci->IsPortEnable();
	} else {
		return FALSE;
	}
}

//*******************************************************************************
//	現在の COM ポート番号を取得
//*******************************************************************************
SCIPORT CSciDeviceBase::GetCurrentSciPort()
{
	if( m_pSci ) {
		return m_pSci->GetCurrentSciPort();
	} else {
		return PORTNO_NULL;
	}
}

//*******************************************************************************
//	現在のボーレートを取得
//*******************************************************************************
BAUDRATE CSciDeviceBase::GetCurrentBaudrate()
{
	if( m_pSci ) {
		return m_pSci->GetCurrentBaudrate();
	} else {
		return (BAUDRATE)0;
	}
}

//*******************************************************************************
//	転送したビット数を取得
//*******************************************************************************
UINT CSciDeviceBase::GetTransmitBits()
{
	if( m_pSci ) {
		return m_pSci->GetTransmitBytes() * CSciBase::SCIBITS_PER_BYTE;
	} else {
		return 0;
	}
}

//*******************************************************************************
//	転送したビット数をクリア
//*******************************************************************************
void CSciDeviceBase::ClearTransmitBits()
{
	if( m_pSci ) {
		m_pSci->ClearTransmitBytes();
	}
}

//*******************************************************************************
//	受信したビット数を取得
//*******************************************************************************
UINT CSciDeviceBase::GetReceiveBits()
{
	if( m_pSci ) {
		return m_pSci->GetReceiveBytes() * CSciBase::SCIBITS_PER_BYTE;
	} else {
		return 0;
	}
}

//*******************************************************************************
//	受信したビット数をクリア
//*******************************************************************************
void CSciDeviceBase::ClearReceiveBits()
{
	if( m_pSci ) {
		m_pSci->ClearReceiveBytes();
	}
}