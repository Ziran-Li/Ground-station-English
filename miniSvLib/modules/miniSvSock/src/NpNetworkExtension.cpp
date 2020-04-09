#include "stdafx.h"
#include "NpNetworkExtension.h"

//#################################################################################################################################
//	UdpSendPacket
//#################################################################################################################################

//*******************************************************************************
//	パケットにヘッダを追加する（領域は自動で確保）
//*******************************************************************************
BOOL UdpSendPacket::PushHeader(USHORT packetId, UINT payloadLen)
{
	// UDPパケットヘッダの作成
	UdpPacketHeader header;
	header.packetId = packetId;
	header.payloadLen = payloadLen;

	return PushData(&header, sizeof(UdpPacketHeader));
}

//*******************************************************************************
//	パケットにフッタを追加する（領域は自動で確保）
//*******************************************************************************
BOOL UdpSendPacket::PushFooter()
{
	// UDPパケットフッタの作成
	UdpPacketFooter footer;

	return PushData(&footer, sizeof(UdpPacketFooter));
}

//*******************************************************************************
//	パケットにデータを追加する（領域は自動で確保）
//*******************************************************************************
BOOL UdpSendPacket::PushData(const void *src, UINT srcSize)
{
	// エラーチェック
	if (srcSize < 0) return FALSE;

	// 格納領域を確保(実際のサイズはこの時点では変わらない)
	m_buf.reserve(m_buf.size() + srcSize);

	// データを追加
	for (UINT i = 0; i < srcSize; ++i) {
		m_buf.push_back(static_cast<const UCHAR*>(src)[i]);
	}

	return TRUE;
}


//#################################################################################################################################
//	UdpRecvPacket
//#################################################################################################################################

//*******************************************************************************
//	パケットIDを取得する
//*******************************************************************************
USHORT UdpRecvPacket::GetPacketId() const
{
	if (m_buf.size() < sizeof(UdpPacketHeader)) return 0;

	UdpPacketHeader header;
	::CopyMemory(&header, &m_buf[0], sizeof(UdpPacketHeader));

	return header.packetId;
}

//*******************************************************************************
//	ペイロード長を取得する
//*******************************************************************************
UINT UdpRecvPacket::GetPayloadLen() const
{
	if (m_buf.size() < sizeof(UdpPacketHeader)) return 0;

	UdpPacketHeader header;
	::CopyMemory(&header, &m_buf[0], sizeof(UdpPacketHeader));

	return header.payloadLen;
}

//*******************************************************************************
//	ペイロードデータを取得する
//*******************************************************************************
BOOL UdpRecvPacket::GetPayloadData(void *dest, UINT destSize, UINT offset)
{
	if (m_buf.size() < (sizeof(UdpPacketHeader)+offset+destSize)) return FALSE;

	::CopyMemory(dest, &m_buf[sizeof(UdpPacketHeader)+offset], destSize);

	return TRUE;
}


//#################################################################################################################################
//	NpUdpServer
//#################################################################################################################################

//*******************************************************************************
//	コンストラクタ
//*******************************************************************************
NpUdpServer::NpUdpServer()
{
	m_ReceiveNoticeMessage_idThread = 0;
	m_ReceiveNoticeMessage_Msg      = 0;
}

//*******************************************************************************
//	受信通知メッセージのポスト先スレッドの登録
//*******************************************************************************
BOOL NpUdpServer::RegisterReceiveNoticeMessage(DWORD idThread, UINT Msg)
{
	this->m_ReceiveNoticeMessage_idThread = idThread;
	this->m_ReceiveNoticeMessage_Msg      = Msg;
	return TRUE;
}

//*******************************************************************************
//	受信データ取得
//*******************************************************************************
BOOL NpUdpServer::GetDataUDP1(Payload_UDP1 &payload_udp1)
{
	DataQueueUDP1::ACCESSOR pDataQueueUDP1 = m_dataUDP1.GetAccessor();

	// 空だった場合
	if (pDataQueueUDP1->size() == 0) return FALSE;

	// 受信データをコピー
	payload_udp1 = pDataQueueUDP1->front();

	// 先頭データを削除
	pDataQueueUDP1->pop();

	return TRUE;
}

BOOL NpUdpServer::GetDataUDP2(Payload_UDP2_wo_DownCam &payload_udp2_wo_DownCam, Payload_UDP2_DownCam &payload_udp2_DownCam)
{
	DataQueueUDP2_wo_DownCam::ACCESSOR pDataQueueUDP2_wo_DownCam = m_dataUDP2_wo_DownCam.GetAccessor();
	DataQueueUDP2_DownCam::ACCESSOR pDataQueueUDP2_DownCam = m_dataUDP2_DownCam.GetAccessor();

	// 空だった場合
	if (pDataQueueUDP2_wo_DownCam->size() == 0 || pDataQueueUDP2_DownCam->size() == 0) return FALSE;

	// 受信データをコピー
	payload_udp2_wo_DownCam = pDataQueueUDP2_wo_DownCam->front();
	payload_udp2_DownCam = pDataQueueUDP2_DownCam->front();

	// 先頭データを削除
	pDataQueueUDP2_wo_DownCam->pop();
	pDataQueueUDP2_DownCam->pop();

	return TRUE;
}

BOOL NpUdpServer::GetDataUDP3(Payload_UDP3_wo_UpCam &payload_udp3_wo_UpCam, Payload_UDP3_UpCam &payload_udp3_UpCam)
{
	DataQueueUDP3_wo_UpCam::ACCESSOR pDataQueueUDP3_wo_UpCam = m_dataUDP3_wo_UpCam.GetAccessor();
	DataQueueUDP3_UpCam::ACCESSOR pDataQueueUDP3_UpCam = m_dataUDP3_UpCam.GetAccessor();

	// 空だった場合
	if (pDataQueueUDP3_wo_UpCam->size() == 0 || pDataQueueUDP3_UpCam->size() == 0) return FALSE;

	// 受信データをコピー
	payload_udp3_wo_UpCam = pDataQueueUDP3_wo_UpCam->front();
	payload_udp3_UpCam = pDataQueueUDP3_UpCam->front();

	// 先頭データを削除
	pDataQueueUDP3_wo_UpCam->pop();
	pDataQueueUDP3_UpCam->pop();

	return TRUE;
}

BOOL NpUdpServer::GetDataUDP4(Payload_UDP4 &payload_udp4)
{
	DataQueueUDP4::ACCESSOR pDataQueueUDP4 = m_dataUDP4.GetAccessor();

	// 空だった場合
	if (pDataQueueUDP4->size() == 0) return FALSE;

	// 受信データをコピー
	payload_udp4 = pDataQueueUDP4->front();

	// 先頭データを削除
	pDataQueueUDP4->pop();

	return TRUE;
}

BOOL NpUdpServer::GetDataUDP5(Payload_UDP5 &payload_udp5)
{
	DataQueueUDP5::ACCESSOR pDataQueueUDP5 = m_dataUDP5.GetAccessor();

	// 空だった場合
	if (pDataQueueUDP5->size() == 0) return FALSE;

	// 受信データをコピー
	payload_udp5 = pDataQueueUDP5->front();

	// 先頭データを削除
	pDataQueueUDP5->pop();

	return TRUE;
}

BOOL NpUdpServer::GetDataUDP6(Payload_UDP6 &payload_udp6)
{
	DataQueueUDP6::ACCESSOR pDataQueueUDP6 = m_dataUDP6.GetAccessor();

	// 空だった場合
	if (pDataQueueUDP6->size() == 0) return FALSE;

	// 受信データをコピー
	payload_udp6 = pDataQueueUDP6->front();

	// 先頭データを削除
	pDataQueueUDP6->pop();

	return TRUE;
}


//*******************************************************************************
//	X-MonitorServer がデータを受信したときの処理
//*******************************************************************************
void NpUdpServer::OnReceive()
{
	// 受信データの取得
	size_t bufSize = sizeof(UdpPacketHeader) + CAM_DATA_MAX_SIZE + sizeof(UdpPacketFooter);
	UdpRecvPacket recvPacket(bufSize);
	int nReceive;
	nReceive = ReceiveData(recvPacket.GetBufPtr(), recvPacket.GetBufSize());
	if(nReceive == 0){
		// 受信バッファサイズより大きなデータが送られてきた　or　その他の通信エラー
		return;
	}

	// ペイロード部の取得(パケットIDによって登録するキューを変更)
	switch (recvPacket.GetPacketId()){

	case PACKET_ID_UDP1:
		{
			Payload_UDP1 payload;
			recvPacket.GetPayloadData(&payload, sizeof(Payload_UDP1));
			DataQueueUDP1::ACCESSOR pDataQueueUDP1 = m_dataUDP1.GetAccessor();
			pDataQueueUDP1->push(payload);	// ペイロード部をキューに登録
			break;
		}

	case PACKET_ID_UDP2:
		{
			Payload_UDP2_wo_DownCam payload1;
			recvPacket.GetPayloadData(&payload1, sizeof(Payload_UDP2_wo_DownCam));
			DataQueueUDP2_wo_DownCam::ACCESSOR pDataQueueUDP2_wo_DownCam = m_dataUDP2_wo_DownCam.GetAccessor();
			pDataQueueUDP2_wo_DownCam->push(payload1);	// ペイロード部(上方向カメラデータ除く)をキューに登録

			Payload_UDP2_DownCam payload2;
			recvPacket.GetPayloadData(&payload2, payload1.jpegDataSize, sizeof(Payload_UDP2_wo_DownCam));
			DataQueueUDP2_DownCam::ACCESSOR pDataQueueUDP2_DownCam = m_dataUDP2_DownCam.GetAccessor();
			pDataQueueUDP2_DownCam->push(payload2);		// ペイロード部(上方向カメラデータ)をキューに登録
			break;
		}

	case PACKET_ID_UDP3:	
		{
			Payload_UDP3_wo_UpCam payload1;
			recvPacket.GetPayloadData(&payload1, sizeof(Payload_UDP3_wo_UpCam));
			DataQueueUDP3_wo_UpCam::ACCESSOR pDataQueueUDP3_wo_UpCam = m_dataUDP3_wo_UpCam.GetAccessor();
			pDataQueueUDP3_wo_UpCam->push(payload1);	// ペイロード部(上方向カメラデータ除く)をキューに登録

			Payload_UDP3_UpCam payload2;
			recvPacket.GetPayloadData(&payload2, payload1.jpegDataSize, sizeof(Payload_UDP3_wo_UpCam));
			DataQueueUDP3_UpCam::ACCESSOR pDataQueueUDP3_UpCam = m_dataUDP3_UpCam.GetAccessor();
			pDataQueueUDP3_UpCam->push(payload2);		// ペイロード部(上方向カメラデータ)をキューに登録
			break;
		}
	case PACKET_ID_UDP4:
		{
			Payload_UDP4 payload;
			recvPacket.GetPayloadData(&payload, sizeof(Payload_UDP4));
			DataQueueUDP4::ACCESSOR pDataQueueUDP4 = m_dataUDP4.GetAccessor();
			pDataQueueUDP4->push(payload);	// ペイロード部をキューに登録
			break;
		}

	case PACKET_ID_UDP5:	
		{
			Payload_UDP5 payload;
			recvPacket.GetPayloadData(&payload, sizeof(Payload_UDP5));
			DataQueueUDP5::ACCESSOR pDataQueueUDP5 = m_dataUDP5.GetAccessor();
			pDataQueueUDP5->push(payload);	// ペイロード部をキューに登録
			break;
		}

	case PACKET_ID_UDP6:
		{
			Payload_UDP6 payload;
			recvPacket.GetPayloadData(&payload, sizeof(Payload_UDP6));
			DataQueueUDP6::ACCESSOR pDataQueueUDP6 = m_dataUDP6.GetAccessor();
			pDataQueueUDP6->push(payload);	// ペイロード部をキューに登録
			break;
		}
	default:
		// 不正なパケットID
		return;
	}

	// メッセージ送信(データ受信を親スレッドに知らせる)
	if (this->m_ReceiveNoticeMessage_idThread) {
		::PostThreadMessage(this->m_ReceiveNoticeMessage_idThread, this->m_ReceiveNoticeMessage_Msg, recvPacket.GetPacketId(), 0);
	}
}


///#################################################################################################################################
//	NpUdpClient
//#################################################################################################################################
BOOL NpUdpClient::SendDataUDP1(const Payload_UDP1 &payload_udp1)
{
	// 送信パケットの作成
	UdpSendPacket sendPacket;
	sendPacket.PushHeader(PACKET_ID_UDP1, sizeof(Payload_UDP1));
	sendPacket.PushData(&payload_udp1, sizeof(Payload_UDP1));
	sendPacket.PushFooter();

	// パケット送信
	int nTotalSend = SendData(sendPacket.GetBufPtr(), sendPacket.GetBufSize(), FALSE);

	if (nTotalSend == sendPacket.GetBufSize()) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL NpUdpClient::SendDataUDP2(const Payload_UDP2_wo_DownCam &payload_udp2_wo_DownCam, const Payload_UDP2_DownCam payload_udp2_DownCam)
{
	// 送信パケットの作成
	UdpSendPacket sendPacket;
	sendPacket.PushHeader(PACKET_ID_UDP2, sizeof(Payload_UDP2_wo_DownCam)+payload_udp2_wo_DownCam.jpegDataSize);
	sendPacket.PushData(&payload_udp2_wo_DownCam, sizeof(Payload_UDP2_wo_DownCam));
	sendPacket.PushData(&payload_udp2_DownCam, payload_udp2_wo_DownCam.jpegDataSize);
	sendPacket.PushFooter();

	// パケット送信
	int nTotalSend = SendData(sendPacket.GetBufPtr(), sendPacket.GetBufSize(), FALSE);

	if (nTotalSend == sendPacket.GetBufSize()) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL NpUdpClient::SendDataUDP3(const Payload_UDP3_wo_UpCam &payload_udp3_wo_UpCam, const Payload_UDP3_UpCam payload_udp3_UpCam)
{
	// 送信パケットの作成
	UdpSendPacket sendPacket;
	sendPacket.PushHeader(PACKET_ID_UDP3, sizeof(Payload_UDP3_wo_UpCam)+payload_udp3_wo_UpCam.jpegDataSize);
	sendPacket.PushData(&payload_udp3_wo_UpCam, sizeof(Payload_UDP3_wo_UpCam));
	sendPacket.PushData(&payload_udp3_UpCam, payload_udp3_wo_UpCam.jpegDataSize);
	sendPacket.PushFooter();

	// パケット送信
	int nTotalSend = SendData(sendPacket.GetBufPtr(), sendPacket.GetBufSize(), FALSE);

	if (nTotalSend == sendPacket.GetBufSize()) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL NpUdpClient::SendDataUDP4(const Payload_UDP4 &payload_udp4)
{
	// 送信パケットの作成
	UdpSendPacket sendPacket;
	sendPacket.PushHeader(PACKET_ID_UDP4, sizeof(Payload_UDP4));
	sendPacket.PushData(&payload_udp4, sizeof(Payload_UDP4));
	sendPacket.PushFooter();

	// パケット送信
	int nTotalSend = SendData(sendPacket.GetBufPtr(), sendPacket.GetBufSize(), FALSE);

	if (nTotalSend == sendPacket.GetBufSize()) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL NpUdpClient::SendDataUDP5(const Payload_UDP5 &payload_udp5)
{
	// 送信パケットの作成
	UdpSendPacket sendPacket;
	sendPacket.PushHeader(PACKET_ID_UDP5, sizeof(Payload_UDP5));
	sendPacket.PushData(&payload_udp5, sizeof(Payload_UDP5));
	sendPacket.PushFooter();

	// パケット送信
	int nTotalSend = SendData(sendPacket.GetBufPtr(), sendPacket.GetBufSize(), FALSE);

	if (nTotalSend == sendPacket.GetBufSize()) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL NpUdpClient::SendDataUDP6(const Payload_UDP6 &payload_udp6)
{
	// 送信パケットの作成
	UdpSendPacket sendPacket;
	sendPacket.PushHeader(PACKET_ID_UDP6, sizeof(Payload_UDP6));
	sendPacket.PushData(&payload_udp6, sizeof(Payload_UDP6));
	sendPacket.PushFooter();

	// パケット送信
	int nTotalSend = SendData(sendPacket.GetBufPtr(), sendPacket.GetBufSize(), FALSE);

	if (nTotalSend == sendPacket.GetBufSize()) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

//*******************************************************************************
//	X-Monitor Client がデータを受信したときの処理
//*******************************************************************************
void NpUdpClient::OnReceive() 
{
	// クライアントは基本的にデータの受信はしない

	// 空読み
	size_t bufSize = sizeof(UdpPacketHeader) + CAM_DATA_MAX_SIZE + sizeof(UdpPacketFooter);
	UdpRecvPacket recvPacket(bufSize);
	ReceiveData(recvPacket.GetBufPtr(), recvPacket.GetBufSize());
}

///#################################################################################################################################
//	NpTcpClient
//#################################################################################################################################
//*******************************************************************************
//	コンストラクタ
//*******************************************************************************
NpTcpClient::NpTcpClient()
{
	m_ReceiveNoticeMessage_idThread = 0;
	m_ReceiveNoticeMessage_Msg = 0;
	m_RequestData = TRUE;
}

//*******************************************************************************
//	受信通知メッセージのポスト先スレッドの登録
//*******************************************************************************
BOOL NpTcpClient::RegisterReceiveNoticeMessage(DWORD idThread, UINT Msg)
{
	this->m_ReceiveNoticeMessage_idThread = idThread;
	this->m_ReceiveNoticeMessage_Msg = Msg;
	return TRUE;
}

//*******************************************************************************
//	TCP Client がデータを受信したときの処理
//*******************************************************************************
void NpTcpClient::OnSync()
{
	// メッセージ送信(データ受信を親スレッドに知らせる)
	if (this->m_ReceiveNoticeMessage_idThread) {
		::PostThreadMessage(this->m_ReceiveNoticeMessage_idThread, this->m_ReceiveNoticeMessage_Msg, 0, 0);
	}
}