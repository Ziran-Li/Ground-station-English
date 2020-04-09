#include "stdafx.h"
#include "MyNetworkExtension.h"

//#################################################################################################################################
//	MyRxBuffer
//#################################################################################################################################

//*******************************************************************************
//	コンストラクタ
//*******************************************************************************
MyRxBuffer::MyRxBuffer(size_t nBlockSize, size_t nMaximumSize, size_t nAllowedPacketSize) : RxBuffer(nBlockSize, nMaximumSize) 
{
	m_AllowedPacketSize = nAllowedPacketSize;
}

////*******************************************************************************
////	パケットのヘッダを検出し，PacketHead を返す
////
////	戻り値：
////		PacketHead - 検出されたパケットのヘッダ
////*******************************************************************************
//PacketHead MyRxBuffer::FindPacket()
//{
//	while(true) {
//		if( Seek(PACKET_HEAD_STR, PacketHead::SIZEOF_PREAMBLE) ) {
//
//			if( GetLength() >= sizeof(PacketHead) ) {
//				// パケットの最小サイズ以上受信している
//				WORD  tmp_id   = 0;
//				DWORD tmp_size = 0;
//				Read(offsetof(PacketHead, id) - offsetof(PacketHead, preamble), &tmp_id);
//				Read(offsetof(PacketHead, size) - offsetof(PacketHead, preamble), &tmp_size);
//
//				// size に関するエラーチェック
//				if( (tmp_size >= PacketHead::SIZEOF_PACKETHEAD) && (tmp_size <= m_AllowedPacketSize) ) {
//					// エラーなし
//					return PacketHead(tmp_id, tmp_size);
//					break;
//				} else {
//					// エラー（もう一度ヘッダバイトを探す）
//					Skip(PacketHead::SIZEOF_PREAMBLE);
//					continue;
//				}
//			} else {
//				// パケットの最小サイズ以上受信していない
//				return PacketHead();
//				break;
//			}
//		} else {
//			// ヘッダバイトが見つからない
//			return PacketHead();
//			break;
//		}
//	};
//}
//
////*******************************************************************************
////	FindPacket の後で呼ぶと，現在先頭にあるパケットをスキップする
////*******************************************************************************
//void MyRxBuffer::NextPacket()
//{
//	Skip(PacketHead::SIZEOF_PACKETHEAD);
//}

//*******************************************************************************
//	受信バッファからパケットのデータを取得する
//　デリミタ(NULL)まで取得する(=1メッセージのみ取得する)
//　取得したデータはバッファからリリースする
//　デリミタが見つからなかったら、FALSEを返す(受信バッファはそのまま)
//*******************************************************************************
BOOL MyRxBuffer::GetPacket(std::string &strPacket, const unsigned char delimiter)
{
	// 受信データが無い場合はFALSEを返す
	if (GetLength() == 0) return FALSE;

	BOOL bMatch = FALSE;
	size_t nReadFromFrontOri = m_nReadFromFront;

	BlockList::iterator pp = m_BlockList.begin();

	for (; pp != m_BlockList.end(); pp++) {

		unsigned char* pSrc = pp->pBuff + m_nReadFromFront;

		while (m_nReadFromFront < pp->nFilled) { // ブロック内の有効データの末尾までループ

			// 1文字取得
			strPacket += *pSrc;
			++m_nReadFromFront;

			if (*(pSrc++) == delimiter) {
				// 一致した
				bMatch = TRUE;
				_ReleaseUntil(pp); // 一個前までのブロックを削除
				break;
			}
		}

		if (bMatch == TRUE) break;

		m_nReadFromFront = 0;
	}

	// デリミタが最後までみつからなかった → 受信バッファのデータはそのまま残しておく、データの取得はしない
	if (pp == m_BlockList.end()) {
		strPacket.clear();
		m_nReadFromFront = nReadFromFrontOri;

		return FALSE;
	}

	// 受信バッファのデータをすべて取得した　→　受信バッファをリセット
	if (GetLength() == 0) {
		//_ReleaseUntil(--m_BlockList.end());
		BlockList::iterator pp = m_BlockList.begin();
		pp->nFilled = 0;
		m_nReadFromFront = 0;
		m_nBuffLeftLen = pp->nSize;
	}

	return TRUE;
}

//#################################################################################################################################
//	MyTxBuffer
//#################################################################################################################################

//*******************************************************************************
//	構築・破棄
//*******************************************************************************
MyTxBuffer::MyTxBuffer(size_t nBlockSize, size_t nMaximumSize) : TxBuffer(nBlockSize, nMaximumSize)
{
}

//*******************************************************************************
//	パケットのデータを送信バッファに設定する
//*******************************************************************************
BOOL MyTxBuffer::SetPacket(const std::string &strPacket)
{
	PushArray(strPacket.c_str(), strPacket.size());
	return TRUE;
}

//#################################################################################################################################
//	MyWSAConnection
//#################################################################################################################################

//*******************************************************************************
//	構築
//*******************************************************************************
MyWSAConnection::MyWSAConnection(size_t RxBlockSize, size_t TxBlockSize) : m_RxBuff(new MyRxBuffer(RxBlockSize)), m_TxBuff(new MyTxBuffer(TxBlockSize))
{
}

//*******************************************************************************
//	パケットを送る
//
//	Note:
//		送信バッファをロックする．OnSend でも送信バッファをロックするので安全なはず
//*******************************************************************************
BOOL MyWSAConnection::SendPacket(const std::string &strPacket)
{
	exc::TxBuffer::ACCESSOR pTxBuff = m_TxBuff.GetAccessor();

	// ロックしてパケットを設定する(デリミタ（NULL）を追加)
	pTxBuff->SetPacket(strPacket+'\0');

	// ロック状態でバッファ送信
	while( true ) {
		CommBuffer::CBUFFER buf = pTxBuff->PopFrontBegin();
		int nSend = SendData(buf.pBuff, buf.nLen, FALSE);
		if( !pTxBuff->PopFrontEnd(nSend) ) break;
	}

	return TRUE;
}

void MyWSAConnection::OnSend()
{
	exc::TxBuffer::ACCESSOR pTxBuff = m_TxBuff.GetAccessor();

	// ロック状態でバッファ送信
	while( true ) {
		CommBuffer::CBUFFER buf = pTxBuff->PopFrontBegin();
		int nSend = SendData(buf.pBuff, buf.nLen, FALSE);
		if( !pTxBuff->PopFrontEnd(nSend) ) break;
	}
}

void MyWSAConnection::OnReceive()
{
	exc::RxBuffer::ACCESSOR pRxBuff = m_RxBuff.GetAccessor();

	while( true ) {
		CommBuffer::BUFFER buf = pRxBuff->PushBackBegin();
		int nReceive = ReceiveData(buf.pBuff, buf.nLen, FALSE);
		if( !pRxBuff->PushBackEnd(nReceive) ) break;
	}
}

void MyWSAConnection::OnClose()
{
	m_RxBuff.GetAccessor()->Clear();
	m_TxBuff.GetAccessor()->Clear();
}

//#################################################################################################################################
//	XMonitorServer
//#################################################################################################################################

//*******************************************************************************
//	コンストラクタ
//*******************************************************************************
XMonitorServer::XMonitorServer()
{
	m_ReceiveNoticeMessage_idThread = 0;
	m_ReceiveNoticeMessage_Msg      = 0;

	m_AcceptNoticeMessage_idThread = 0;
	m_AcceptNoticeMessage_Msg      = 0;

	m_CloseNoticeMessage_idThread   = 0;
	m_CloseNoticeMessage_Msg        = 0;
}

//*******************************************************************************
//	受信通知メッセージのポスト先スレッドの登録
//*******************************************************************************
BOOL XMonitorServer::RegisterReceiveNoticeMessage(DWORD idThread, UINT Msg)
{
	this->m_ReceiveNoticeMessage_idThread = idThread;
	this->m_ReceiveNoticeMessage_Msg      = Msg;
	return TRUE;
}

//*******************************************************************************
//	接続通知メッセージのポスト先スレッドの登録
//*******************************************************************************
BOOL XMonitorServer::RegisterAcceptNoticeMessage(DWORD idThread, UINT Msg)
{
	this->m_AcceptNoticeMessage_idThread = idThread;
	this->m_AcceptNoticeMessage_Msg      = Msg;
	return TRUE;
}

//*******************************************************************************
//	切断通知メッセージのポスト先スレッドの登録
//*******************************************************************************
BOOL XMonitorServer::RegisterCloseNoticeMessage(DWORD idThread, UINT Msg)
{
	this->m_CloseNoticeMessage_idThread = idThread;
	this->m_CloseNoticeMessage_Msg      = Msg;
	return TRUE;
}


//*******************************************************************************
//	指定クライアントにパケットを送信
//*******************************************************************************
BOOL XMonitorServer::SendPacket(const std::string &packet, UINT connectionID)
{
	_LockClientMap();

	for(ClientMap::iterator it = m_ClientMap.begin(); it != m_ClientMap.end(); it++ ) {
		ClientType* pClient = it->second;
		if (it->second->GetConnectionID() == connectionID) {
			pClient->SendPacket(packet);
		}     
	}

	_UnlockClientMap();

	return TRUE;
}

////*******************************************************************************
////	クライアント全体にパケットを送信
////*******************************************************************************
//BOOL XMonitorServer::SendPacket(const PacketBase* pPacket)
//{
//	_LockClientMap();
//
//	for(ClientMap::iterator it = m_ClientMap.begin(); it != m_ClientMap.end(); it++ ) {
//		ClientType* pClient = it->second;
//		pClient->SendPacket(pPacket);
//	}
//
//	_UnlockClientMap();
//
//	return TRUE;
//}
//
////*******************************************************************************
////	クライアント全体に MavStatePacket を送信
////*******************************************************************************
//BOOL XMonitorServer::SendMAVData(const CMavState &Mav)
//{
//	MavStatePacket Packet(Mav);
//	return SendPacket(&Packet);
//}
//
////*******************************************************************************
////	クライアント全体に GcsStatePacket を送信
////*******************************************************************************
//BOOL XMonitorServer::SendGCSData(const CGcsState &Gcs)
//{
//	GcsStatePacket Packet(Gcs);
//	return SendPacket(&Packet);
//}

////*******************************************************************************
////	クライアント全体に SyncPacket を送信
////*******************************************************************************
//BOOL XMonitorServer::SendSync()
//{
//	SyncPacket Packet;
//	return SendPacket(&Packet);
//}

//*******************************************************************************
//	受信データ取得
//*******************************************************************************
BOOL XMonitorServer::PopRecvData(std::string& recvData, unsigned int &connectionID)
{
	CExclusiveObject< std::queue<XMLRecvData> >::ACCESSOR pXMLRecvData = m_XMLRecvDataQueue.GetAccessor();

	// 空だった場合
	if (pXMLRecvData->size() == 0) return FALSE;

	// XML文と接続IDを取得
	recvData = pXMLRecvData->front().xmlData;
	connectionID = pXMLRecvData->front().connectionID;

	// 先頭データを削除
	pXMLRecvData->pop();

	return TRUE;
}

//BOOL XMonitorServer::PopPayload_ChangeData(IMUData& Imu)
//{
//	CExclusiveObject< std::queue<IMUData> >::ACCESSOR pChangeDataPayloadQueue_ImuData = m_ChangeDataPayloadQueue_ImuData.GetAccessor();
//	
//	// 空だった場合
//	if( pChangeDataPayloadQueue_ImuData->size() == 0 ) return FALSE;
//
//	// データ取得
//	Imu = pChangeDataPayloadQueue_ImuData->front();
//
//	// 先頭データを削除
//	pChangeDataPayloadQueue_ImuData->pop();
//
//	return TRUE;
//}
//
//BOOL XMonitorServer::PopPayload_ChangeData(LocalPos &Loc)
//{
//	CExclusiveObject< std::queue<LocalPos> >::ACCESSOR pChangeDataPayloadQueue_LocalPos = m_ChangeDataPayloadQueue_LocalPos.GetAccessor();
//
//	// 空だった場合
//	if( pChangeDataPayloadQueue_LocalPos->size() == 0 ) return FALSE;
//
//	// データ取得
//	Loc = pChangeDataPayloadQueue_LocalPos->front();
//
//	// 先頭データを削除
//	pChangeDataPayloadQueue_LocalPos->pop();
//
//	return TRUE;
//}
//
//BOOL XMonitorServer::PopPayload_ChangeData(AnemometerData &Anemo)
//{
//	CExclusiveObject< std::queue<AnemometerData> >::ACCESSOR pChangeDataPayloadQueue_AnemometerData = m_ChangeDataPayloadQueue_AnemometerData.GetAccessor();
//
//	// 空だった場合
//	if( pChangeDataPayloadQueue_AnemometerData->size() == 0 ) return FALSE;
//
//	// データ取得
//	Anemo = pChangeDataPayloadQueue_AnemometerData->front();
//
//	// 先頭データを削除
//	pChangeDataPayloadQueue_AnemometerData->pop();
//
//	return TRUE;
//}
//
//BOOL XMonitorServer::PopPayload_ChangeGCSPropoData(PropoData& Propo) 
//{
//	CExclusiveObject< std::queue<PropoData> >::ACCESSOR pChangeDataPayloadQueue_GCSPropoData = m_ChangeDataPayloadQueue_GCSPropoData.GetAccessor();
//
//	// 空だった場合
//	if( pChangeDataPayloadQueue_GCSPropoData->size() == 0 ) return FALSE;
//
//	// データ取得
//	Propo = pChangeDataPayloadQueue_GCSPropoData->front();
//
//	// 先頭データを削除
//	pChangeDataPayloadQueue_GCSPropoData->pop();
//
//	return TRUE;
//}
//
////*******************************************************************************
////	MainLinkDataPacket 先頭のデータサイズを取得
////*******************************************************************************
//size_t XMonitorServer::GetSize_FrontOfMainLinkData() const
//{
//	CExclusiveObject< std::queue<MainLinkDataPacket> >::CONST_ACCESSOR pMainLinkDataQueue = m_MainLinkDataQueue.GetConstAccessor();
//	return pMainLinkDataQueue->front().nDataLength;
//}
//
////*******************************************************************************
////	MainLinkDataPacket のデータを取得
////*******************************************************************************
//BOOL XMonitorServer::PopPayload_MainLinkData(PACKETID& PacketID, BYTE* pData, DWORD DataSize)
//{
//	CExclusiveObject< std::queue<MainLinkDataPacket> >::ACCESSOR pMainLinkDataQueue = m_MainLinkDataQueue.GetAccessor();
//
//	// データが無い場合
//	if( pMainLinkDataQueue->size() == 0 ) return FALSE;
//
//	// 先頭データへの参照を取得
//	std::queue<MainLinkDataPacket>::reference Front = pMainLinkDataQueue->front();
//
//	// バッファサイズが一致する場合のみコピー
//	if( DataSize == Front.nDataLength ) {
//		PacketID = Front.PacketID;
//		CopyMemory(pData, Front.pData, Front.nDataLength);
//
//		// 先頭データを削除
//		pMainLinkDataQueue->pop();
//
//		return TRUE;
//	} else {
//		return FALSE;
//	}
//}
//
//BOOL XMonitorServer::PopPayload_MainLinkData()
//{
//	CExclusiveObject< std::queue<MainLinkDataPacket> >::ACCESSOR pMainLinkDataQueue = m_MainLinkDataQueue.GetAccessor();
//
//	// データが無い場合
//	if( pMainLinkDataQueue->size() == 0 ) return FALSE;
//
//	// 先頭データを削除
//	pMainLinkDataQueue->pop();
//
//	return TRUE;
//}

//*******************************************************************************
//	X-MonitorServer がデータを受信したときの処理
//*******************************************************************************
void XMonitorServer::OnReceive(MyWSAConnection_For_Server* pClient)
{
	exc::RxBuffer::ACCESSOR pRxBuff = pClient->m_RxBuff.GetAccessor();
	CExclusiveObject< std::queue<XMLRecvData> >::ACCESSOR pXMLRecvData = m_XMLRecvDataQueue.GetAccessor();

	XMLRecvData xmlRecvData;
	xmlRecvData.connectionID = pClient->GetConnectionID();  // 接続先（クライアント）の識別子を登録
	BOOL bGetData = FALSE;	// データを取得したかどうか

	while( true ) {
		xmlRecvData.xmlData.clear();
		if (pRxBuff->GetPacket(xmlRecvData.xmlData, '\0') == TRUE){	// 区切り文字はNULL(\0)
			bGetData = TRUE;
			pXMLRecvData->push(xmlRecvData);
		}
		else{
			break;
		}
	}

	// メッセージ送信
	if (!pXMLRecvData->empty() && this->m_ReceiveNoticeMessage_idThread) {
		::PostThreadMessage(this->m_ReceiveNoticeMessage_idThread, this->m_ReceiveNoticeMessage_Msg, 0, 0);
	}
}

//*******************************************************************************
//	X-MonitorServer にクライアントが接続したときの処理
//*******************************************************************************
void XMonitorServer::OnAccept(MyWSAConnection_For_Server *pClient)
{
	if( this->m_AcceptNoticeMessage_idThread ) {
		::PostThreadMessage(this->m_AcceptNoticeMessage_idThread, this->m_AcceptNoticeMessage_Msg, pClient->m_IPv4dw, 
							pClient->GetConnectionID());
	}
}

//*******************************************************************************
//	X-MonitorServer からクライアントが切断したときの処理
//*******************************************************************************
void XMonitorServer::OnClose(MyWSAConnection_For_Server *pClient)
{
	if( this->m_CloseNoticeMessage_idThread ) {
		::PostThreadMessage(this->m_CloseNoticeMessage_idThread, this->m_CloseNoticeMessage_Msg, pClient->m_IPv4dw,
							pClient->GetConnectionID());
	}
}

///#################################################################################################################################
//	XMonitorSyncClient
//#################################################################################################################################
BOOL XMonitorSyncClient::GetData(std::vector<std::string>& recvData, DWORD msecTimeout)
{
	DWORD ret = ::WaitForSingleObject(m_hSyncEvent, msecTimeout);

	if( ret == WAIT_TIMEOUT ) {
		// X-Monitor が終了するとココ
		return FALSE;

	} else {
		RecvDataQueue::ACCESSOR pRecvDataQueue = m_RecvDataQueue.GetAccessor();

		// キューに入っているデータをすべて取得
		while (pRecvDataQueue->size() > 0) {
			recvData.push_back(pRecvDataQueue->front());
			pRecvDataQueue->pop();
		}

		return TRUE;
	}
}

BOOL XMonitorSyncClient::SendData(const std::string &recvData)
{
	return SendPacket(recvData);
}

//*******************************************************************************
//	X-Monitor Client がデータを受信したときの処理
//*******************************************************************************
void XMonitorSyncClient::OnReceive() 
{
	MyWSAConnection::OnReceive();

	exc::RxBuffer::ACCESSOR pRxBuff = m_RxBuff.GetAccessor();
	BOOL bGetData = FALSE;	// データを取得したかどうか

	while (true) {
		std::string packet;
		if (pRxBuff->GetPacket(packet, '\0') == TRUE) {	// 区切り文字はNULL
			bGetData = TRUE;
			CExclusiveObject< std::queue<std::string> >::ACCESSOR pRecvData = m_RecvDataQueue.GetAccessor();
			pRecvData->push(packet);
		}
		else {
			break;
		}
	}

	// 同期(データが取得できたときだけ)
	if (bGetData == TRUE){
		::SetEvent(m_hSyncEvent);
		OnSync();
	}
}

//*******************************************************************************
//	X-Monitor Client の接続が切断されたときの処理
//*******************************************************************************
void XMonitorSyncClient::OnClose()
{
	MyWSAConnection::OnClose();

	m_RecvDataQueue.GetAccessor()->empty();
}

////#################################################################################################################################
////	XMonitorAsyncClient
////#################################################################################################################################
//
////*******************************************************************************
////	受信した MAV 関連データを取得する
////*******************************************************************************
//BOOL XMonitorAsyncClient::GetData(CMavState& Mav)
//{
//	MavState::ACCESSOR pMavState = m_MavState.GetAccessor();
//
//	if( m_flagMavStateReceived ) {
//		Mav = *pMavState;
//		return TRUE;
//	} else {
//		return FALSE;
//	}
//}
//
////*******************************************************************************
////	受信した GCS 関連データを取得する
////*******************************************************************************
//BOOL XMonitorAsyncClient::GetData(CGcsState& Gcs)
//{
//	GcsState::ACCESSOR pGcsState = m_GcsState.GetAccessor();
//
//	if( m_flagGcsStateReceived ) {
//		Gcs = *pGcsState;
//		return TRUE;
//	} else {
//		return FALSE;
//	}
//}
//
////*******************************************************************************
////	受信した MAV, GCS 関連データを取得する
////*******************************************************************************
//BOOL XMonitorAsyncClient::GetData(CMavState& Mav, CGcsState& Gcs)
//{
//	MavState::ACCESSOR pMavState = m_MavState.GetAccessor();
//	GcsState::ACCESSOR pGcsState = m_GcsState.GetAccessor();
//
//	if( m_flagMavStateReceived && m_flagGcsStateReceived ) {
//		Mav = *pMavState;
//		Gcs = *pGcsState;
//		return TRUE;
//	} else {
//		return FALSE;
//	}
//}
//
////*******************************************************************************
////	X-Monitor に NAVData を送って内部データを書き換える
////*******************************************************************************
//BOOL XMonitorAsyncClient::ChangeData(const NAVData& Nav)
//{
//	NavDataPacket Packet(Nav);
//	return SendPacket(&Packet);
//}
//
////*******************************************************************************
////	X-Monitor に IMUData を送って内部データを書き換える
////*******************************************************************************
//BOOL XMonitorAsyncClient::ChangeData(const IMUData& Imu)
//{
//	ImuDataPacket Packet(Imu);
//	return SendPacket(&Packet);
//}
//
////*******************************************************************************
////	X-Monitor に LocalPos を送って内部データを書き換える
////*******************************************************************************
//BOOL XMonitorAsyncClient::ChangeData(const LocalPos& Loc)
//{
//	LocalPosPacket Packet(Loc);
//	return SendPacket(&Packet);
//}
//
////*******************************************************************************
////	X-Monitor に AnemometerData を送って内部データを書き換える
////*******************************************************************************
//BOOL XMonitorAsyncClient::ChangeData(const AnemometerData& Anemo)
//{
//	AnemometerDataPacket Packet(Anemo);
//	return SendPacket(&Packet);
//}
//
////*******************************************************************************
////	X-Monitor に PropoData を送って内部データを書き換える
////*******************************************************************************
//BOOL XMonitorAsyncClient::ChangeGCSPropoData(const PropoData& Propo)
//{
//	GcsPropoDataPacket Packet(Propo);
//	return SendPacket(&Packet);
//}
//
////*******************************************************************************
////	X-Monitor 経由で MAV にデータを送信する
////*******************************************************************************
//BOOL XMonitorAsyncClient::TransmitPacketToMAV(PACKETID PacketID, const void* pData, size_t nDataLength)
//{
//	MainLinkDataPacket Packet(PacketID, pData, nDataLength);
//	return SendPacket(&Packet);
//}
//
//
////*******************************************************************************
////	X-Monitor に接続したときの処理
////*******************************************************************************
//void XMonitorAsyncClient::OnConnect()
//{
//	MavState::ACCESSOR pMavState = m_MavState.GetAccessor();
//	GcsState::ACCESSOR pGcsState = m_GcsState.GetAccessor();
//
//	m_flagMavStateReceived = FALSE;
//	m_flagGcsStateReceived = FALSE;
//
//	pMavState.release();
//	pGcsState.release();
//}
//
////*******************************************************************************
////	X-Monitor Client がデータを受信したときの処理
////*******************************************************************************
//void XMonitorAsyncClient::OnReceive() 
//{
//	MyWSAConnection::OnReceive();
//
//	exc::RxBuffer::ACCESSOR pRxBuff = m_RxBuff.GetAccessor();
//
//	while( true ) {
//		PacketHead packet = pRxBuff->FindPacket();
//
//		if( packet.id == PacketHead::ID_NULL ) break;
//
//		switch( packet.id ) {
//			case MavStatePacket::ID:
//				{
//					MavStatePacket Packet;
//					pRxBuff->GetPacket(&Packet);
//
//					MavState::ACCESSOR pMavState = m_MavState.GetAccessor();
//
//					*pMavState = Packet.Data;
//
//					m_flagMavStateReceived = TRUE;
//
//					pMavState.release();
//				}
//				break;
//
//			case GcsStatePacket::ID:
//				{
//					GcsStatePacket Packet;
//					pRxBuff->GetPacket(&Packet);
//
//					GcsState::ACCESSOR pGcsState = m_GcsState.GetAccessor();
//
//					*pGcsState = Packet.Data;
//
//					m_flagGcsStateReceived = TRUE;
//
//					pGcsState.release();
//				}
//				break;
//
//			default:
//				pRxBuff->NextPacket();
//				break;
//		}
//	}
//}
//
////*******************************************************************************
////	X-Monitor Client の接続が切断されたときの処理
////*******************************************************************************
//void XMonitorAsyncClient::OnClose()
//{
//	MyWSAConnection::OnClose();
//}