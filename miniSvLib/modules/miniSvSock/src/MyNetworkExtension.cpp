#include "stdafx.h"
#include "MyNetworkExtension.h"

//#################################################################################################################################
//	MyRxBuffer
//#################################################################################################################################

//*******************************************************************************
//	�R���X�g���N�^
//*******************************************************************************
MyRxBuffer::MyRxBuffer(size_t nBlockSize, size_t nMaximumSize, size_t nAllowedPacketSize) : RxBuffer(nBlockSize, nMaximumSize) 
{
	m_AllowedPacketSize = nAllowedPacketSize;
}

////*******************************************************************************
////	�p�P�b�g�̃w�b�_�����o���CPacketHead ��Ԃ�
////
////	�߂�l�F
////		PacketHead - ���o���ꂽ�p�P�b�g�̃w�b�_
////*******************************************************************************
//PacketHead MyRxBuffer::FindPacket()
//{
//	while(true) {
//		if( Seek(PACKET_HEAD_STR, PacketHead::SIZEOF_PREAMBLE) ) {
//
//			if( GetLength() >= sizeof(PacketHead) ) {
//				// �p�P�b�g�̍ŏ��T�C�Y�ȏ��M���Ă���
//				WORD  tmp_id   = 0;
//				DWORD tmp_size = 0;
//				Read(offsetof(PacketHead, id) - offsetof(PacketHead, preamble), &tmp_id);
//				Read(offsetof(PacketHead, size) - offsetof(PacketHead, preamble), &tmp_size);
//
//				// size �Ɋւ���G���[�`�F�b�N
//				if( (tmp_size >= PacketHead::SIZEOF_PACKETHEAD) && (tmp_size <= m_AllowedPacketSize) ) {
//					// �G���[�Ȃ�
//					return PacketHead(tmp_id, tmp_size);
//					break;
//				} else {
//					// �G���[�i������x�w�b�_�o�C�g��T���j
//					Skip(PacketHead::SIZEOF_PREAMBLE);
//					continue;
//				}
//			} else {
//				// �p�P�b�g�̍ŏ��T�C�Y�ȏ��M���Ă��Ȃ�
//				return PacketHead();
//				break;
//			}
//		} else {
//			// �w�b�_�o�C�g��������Ȃ�
//			return PacketHead();
//			break;
//		}
//	};
//}
//
////*******************************************************************************
////	FindPacket �̌�ŌĂԂƁC���ݐ擪�ɂ���p�P�b�g���X�L�b�v����
////*******************************************************************************
//void MyRxBuffer::NextPacket()
//{
//	Skip(PacketHead::SIZEOF_PACKETHEAD);
//}

//*******************************************************************************
//	��M�o�b�t�@����p�P�b�g�̃f�[�^���擾����
//�@�f���~�^(NULL)�܂Ŏ擾����(=1���b�Z�[�W�̂ݎ擾����)
//�@�擾�����f�[�^�̓o�b�t�@���烊���[�X����
//�@�f���~�^��������Ȃ�������AFALSE��Ԃ�(��M�o�b�t�@�͂��̂܂�)
//*******************************************************************************
BOOL MyRxBuffer::GetPacket(std::string &strPacket, const unsigned char delimiter)
{
	// ��M�f�[�^�������ꍇ��FALSE��Ԃ�
	if (GetLength() == 0) return FALSE;

	BOOL bMatch = FALSE;
	size_t nReadFromFrontOri = m_nReadFromFront;

	BlockList::iterator pp = m_BlockList.begin();

	for (; pp != m_BlockList.end(); pp++) {

		unsigned char* pSrc = pp->pBuff + m_nReadFromFront;

		while (m_nReadFromFront < pp->nFilled) { // �u���b�N���̗L���f�[�^�̖����܂Ń��[�v

			// 1�����擾
			strPacket += *pSrc;
			++m_nReadFromFront;

			if (*(pSrc++) == delimiter) {
				// ��v����
				bMatch = TRUE;
				_ReleaseUntil(pp); // ��O�܂ł̃u���b�N���폜
				break;
			}
		}

		if (bMatch == TRUE) break;

		m_nReadFromFront = 0;
	}

	// �f���~�^���Ō�܂ł݂���Ȃ����� �� ��M�o�b�t�@�̃f�[�^�͂��̂܂܎c���Ă����A�f�[�^�̎擾�͂��Ȃ�
	if (pp == m_BlockList.end()) {
		strPacket.clear();
		m_nReadFromFront = nReadFromFrontOri;

		return FALSE;
	}

	// ��M�o�b�t�@�̃f�[�^�����ׂĎ擾�����@���@��M�o�b�t�@�����Z�b�g
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
//	�\�z�E�j��
//*******************************************************************************
MyTxBuffer::MyTxBuffer(size_t nBlockSize, size_t nMaximumSize) : TxBuffer(nBlockSize, nMaximumSize)
{
}

//*******************************************************************************
//	�p�P�b�g�̃f�[�^�𑗐M�o�b�t�@�ɐݒ肷��
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
//	�\�z
//*******************************************************************************
MyWSAConnection::MyWSAConnection(size_t RxBlockSize, size_t TxBlockSize) : m_RxBuff(new MyRxBuffer(RxBlockSize)), m_TxBuff(new MyTxBuffer(TxBlockSize))
{
}

//*******************************************************************************
//	�p�P�b�g�𑗂�
//
//	Note:
//		���M�o�b�t�@�����b�N����DOnSend �ł����M�o�b�t�@�����b�N����̂ň��S�Ȃ͂�
//*******************************************************************************
BOOL MyWSAConnection::SendPacket(const std::string &strPacket)
{
	exc::TxBuffer::ACCESSOR pTxBuff = m_TxBuff.GetAccessor();

	// ���b�N���ăp�P�b�g��ݒ肷��(�f���~�^�iNULL�j��ǉ�)
	pTxBuff->SetPacket(strPacket+'\0');

	// ���b�N��ԂŃo�b�t�@���M
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

	// ���b�N��ԂŃo�b�t�@���M
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
//	�R���X�g���N�^
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
//	��M�ʒm���b�Z�[�W�̃|�X�g��X���b�h�̓o�^
//*******************************************************************************
BOOL XMonitorServer::RegisterReceiveNoticeMessage(DWORD idThread, UINT Msg)
{
	this->m_ReceiveNoticeMessage_idThread = idThread;
	this->m_ReceiveNoticeMessage_Msg      = Msg;
	return TRUE;
}

//*******************************************************************************
//	�ڑ��ʒm���b�Z�[�W�̃|�X�g��X���b�h�̓o�^
//*******************************************************************************
BOOL XMonitorServer::RegisterAcceptNoticeMessage(DWORD idThread, UINT Msg)
{
	this->m_AcceptNoticeMessage_idThread = idThread;
	this->m_AcceptNoticeMessage_Msg      = Msg;
	return TRUE;
}

//*******************************************************************************
//	�ؒf�ʒm���b�Z�[�W�̃|�X�g��X���b�h�̓o�^
//*******************************************************************************
BOOL XMonitorServer::RegisterCloseNoticeMessage(DWORD idThread, UINT Msg)
{
	this->m_CloseNoticeMessage_idThread = idThread;
	this->m_CloseNoticeMessage_Msg      = Msg;
	return TRUE;
}


//*******************************************************************************
//	�w��N���C�A���g�Ƀp�P�b�g�𑗐M
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
////	�N���C�A���g�S�̂Ƀp�P�b�g�𑗐M
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
////	�N���C�A���g�S�̂� MavStatePacket �𑗐M
////*******************************************************************************
//BOOL XMonitorServer::SendMAVData(const CMavState &Mav)
//{
//	MavStatePacket Packet(Mav);
//	return SendPacket(&Packet);
//}
//
////*******************************************************************************
////	�N���C�A���g�S�̂� GcsStatePacket �𑗐M
////*******************************************************************************
//BOOL XMonitorServer::SendGCSData(const CGcsState &Gcs)
//{
//	GcsStatePacket Packet(Gcs);
//	return SendPacket(&Packet);
//}

////*******************************************************************************
////	�N���C�A���g�S�̂� SyncPacket �𑗐M
////*******************************************************************************
//BOOL XMonitorServer::SendSync()
//{
//	SyncPacket Packet;
//	return SendPacket(&Packet);
//}

//*******************************************************************************
//	��M�f�[�^�擾
//*******************************************************************************
BOOL XMonitorServer::PopRecvData(std::string& recvData, unsigned int &connectionID)
{
	CExclusiveObject< std::queue<XMLRecvData> >::ACCESSOR pXMLRecvData = m_XMLRecvDataQueue.GetAccessor();

	// �󂾂����ꍇ
	if (pXMLRecvData->size() == 0) return FALSE;

	// XML���Ɛڑ�ID���擾
	recvData = pXMLRecvData->front().xmlData;
	connectionID = pXMLRecvData->front().connectionID;

	// �擪�f�[�^���폜
	pXMLRecvData->pop();

	return TRUE;
}

//BOOL XMonitorServer::PopPayload_ChangeData(IMUData& Imu)
//{
//	CExclusiveObject< std::queue<IMUData> >::ACCESSOR pChangeDataPayloadQueue_ImuData = m_ChangeDataPayloadQueue_ImuData.GetAccessor();
//	
//	// �󂾂����ꍇ
//	if( pChangeDataPayloadQueue_ImuData->size() == 0 ) return FALSE;
//
//	// �f�[�^�擾
//	Imu = pChangeDataPayloadQueue_ImuData->front();
//
//	// �擪�f�[�^���폜
//	pChangeDataPayloadQueue_ImuData->pop();
//
//	return TRUE;
//}
//
//BOOL XMonitorServer::PopPayload_ChangeData(LocalPos &Loc)
//{
//	CExclusiveObject< std::queue<LocalPos> >::ACCESSOR pChangeDataPayloadQueue_LocalPos = m_ChangeDataPayloadQueue_LocalPos.GetAccessor();
//
//	// �󂾂����ꍇ
//	if( pChangeDataPayloadQueue_LocalPos->size() == 0 ) return FALSE;
//
//	// �f�[�^�擾
//	Loc = pChangeDataPayloadQueue_LocalPos->front();
//
//	// �擪�f�[�^���폜
//	pChangeDataPayloadQueue_LocalPos->pop();
//
//	return TRUE;
//}
//
//BOOL XMonitorServer::PopPayload_ChangeData(AnemometerData &Anemo)
//{
//	CExclusiveObject< std::queue<AnemometerData> >::ACCESSOR pChangeDataPayloadQueue_AnemometerData = m_ChangeDataPayloadQueue_AnemometerData.GetAccessor();
//
//	// �󂾂����ꍇ
//	if( pChangeDataPayloadQueue_AnemometerData->size() == 0 ) return FALSE;
//
//	// �f�[�^�擾
//	Anemo = pChangeDataPayloadQueue_AnemometerData->front();
//
//	// �擪�f�[�^���폜
//	pChangeDataPayloadQueue_AnemometerData->pop();
//
//	return TRUE;
//}
//
//BOOL XMonitorServer::PopPayload_ChangeGCSPropoData(PropoData& Propo) 
//{
//	CExclusiveObject< std::queue<PropoData> >::ACCESSOR pChangeDataPayloadQueue_GCSPropoData = m_ChangeDataPayloadQueue_GCSPropoData.GetAccessor();
//
//	// �󂾂����ꍇ
//	if( pChangeDataPayloadQueue_GCSPropoData->size() == 0 ) return FALSE;
//
//	// �f�[�^�擾
//	Propo = pChangeDataPayloadQueue_GCSPropoData->front();
//
//	// �擪�f�[�^���폜
//	pChangeDataPayloadQueue_GCSPropoData->pop();
//
//	return TRUE;
//}
//
////*******************************************************************************
////	MainLinkDataPacket �擪�̃f�[�^�T�C�Y���擾
////*******************************************************************************
//size_t XMonitorServer::GetSize_FrontOfMainLinkData() const
//{
//	CExclusiveObject< std::queue<MainLinkDataPacket> >::CONST_ACCESSOR pMainLinkDataQueue = m_MainLinkDataQueue.GetConstAccessor();
//	return pMainLinkDataQueue->front().nDataLength;
//}
//
////*******************************************************************************
////	MainLinkDataPacket �̃f�[�^���擾
////*******************************************************************************
//BOOL XMonitorServer::PopPayload_MainLinkData(PACKETID& PacketID, BYTE* pData, DWORD DataSize)
//{
//	CExclusiveObject< std::queue<MainLinkDataPacket> >::ACCESSOR pMainLinkDataQueue = m_MainLinkDataQueue.GetAccessor();
//
//	// �f�[�^�������ꍇ
//	if( pMainLinkDataQueue->size() == 0 ) return FALSE;
//
//	// �擪�f�[�^�ւ̎Q�Ƃ��擾
//	std::queue<MainLinkDataPacket>::reference Front = pMainLinkDataQueue->front();
//
//	// �o�b�t�@�T�C�Y����v����ꍇ�̂݃R�s�[
//	if( DataSize == Front.nDataLength ) {
//		PacketID = Front.PacketID;
//		CopyMemory(pData, Front.pData, Front.nDataLength);
//
//		// �擪�f�[�^���폜
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
//	// �f�[�^�������ꍇ
//	if( pMainLinkDataQueue->size() == 0 ) return FALSE;
//
//	// �擪�f�[�^���폜
//	pMainLinkDataQueue->pop();
//
//	return TRUE;
//}

//*******************************************************************************
//	X-MonitorServer ���f�[�^����M�����Ƃ��̏���
//*******************************************************************************
void XMonitorServer::OnReceive(MyWSAConnection_For_Server* pClient)
{
	exc::RxBuffer::ACCESSOR pRxBuff = pClient->m_RxBuff.GetAccessor();
	CExclusiveObject< std::queue<XMLRecvData> >::ACCESSOR pXMLRecvData = m_XMLRecvDataQueue.GetAccessor();

	XMLRecvData xmlRecvData;
	xmlRecvData.connectionID = pClient->GetConnectionID();  // �ڑ���i�N���C�A���g�j�̎��ʎq��o�^
	BOOL bGetData = FALSE;	// �f�[�^���擾�������ǂ���

	while( true ) {
		xmlRecvData.xmlData.clear();
		if (pRxBuff->GetPacket(xmlRecvData.xmlData, '\0') == TRUE){	// ��؂蕶����NULL(\0)
			bGetData = TRUE;
			pXMLRecvData->push(xmlRecvData);
		}
		else{
			break;
		}
	}

	// ���b�Z�[�W���M
	if (!pXMLRecvData->empty() && this->m_ReceiveNoticeMessage_idThread) {
		::PostThreadMessage(this->m_ReceiveNoticeMessage_idThread, this->m_ReceiveNoticeMessage_Msg, 0, 0);
	}
}

//*******************************************************************************
//	X-MonitorServer �ɃN���C�A���g���ڑ������Ƃ��̏���
//*******************************************************************************
void XMonitorServer::OnAccept(MyWSAConnection_For_Server *pClient)
{
	if( this->m_AcceptNoticeMessage_idThread ) {
		::PostThreadMessage(this->m_AcceptNoticeMessage_idThread, this->m_AcceptNoticeMessage_Msg, pClient->m_IPv4dw, 
							pClient->GetConnectionID());
	}
}

//*******************************************************************************
//	X-MonitorServer ����N���C�A���g���ؒf�����Ƃ��̏���
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
		// X-Monitor ���I������ƃR�R
		return FALSE;

	} else {
		RecvDataQueue::ACCESSOR pRecvDataQueue = m_RecvDataQueue.GetAccessor();

		// �L���[�ɓ����Ă���f�[�^�����ׂĎ擾
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
//	X-Monitor Client ���f�[�^����M�����Ƃ��̏���
//*******************************************************************************
void XMonitorSyncClient::OnReceive() 
{
	MyWSAConnection::OnReceive();

	exc::RxBuffer::ACCESSOR pRxBuff = m_RxBuff.GetAccessor();
	BOOL bGetData = FALSE;	// �f�[�^���擾�������ǂ���

	while (true) {
		std::string packet;
		if (pRxBuff->GetPacket(packet, '\0') == TRUE) {	// ��؂蕶����NULL
			bGetData = TRUE;
			CExclusiveObject< std::queue<std::string> >::ACCESSOR pRecvData = m_RecvDataQueue.GetAccessor();
			pRecvData->push(packet);
		}
		else {
			break;
		}
	}

	// ����(�f�[�^���擾�ł����Ƃ�����)
	if (bGetData == TRUE){
		::SetEvent(m_hSyncEvent);
		OnSync();
	}
}

//*******************************************************************************
//	X-Monitor Client �̐ڑ����ؒf���ꂽ�Ƃ��̏���
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
////	��M���� MAV �֘A�f�[�^���擾����
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
////	��M���� GCS �֘A�f�[�^���擾����
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
////	��M���� MAV, GCS �֘A�f�[�^���擾����
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
////	X-Monitor �� NAVData �𑗂��ē����f�[�^������������
////*******************************************************************************
//BOOL XMonitorAsyncClient::ChangeData(const NAVData& Nav)
//{
//	NavDataPacket Packet(Nav);
//	return SendPacket(&Packet);
//}
//
////*******************************************************************************
////	X-Monitor �� IMUData �𑗂��ē����f�[�^������������
////*******************************************************************************
//BOOL XMonitorAsyncClient::ChangeData(const IMUData& Imu)
//{
//	ImuDataPacket Packet(Imu);
//	return SendPacket(&Packet);
//}
//
////*******************************************************************************
////	X-Monitor �� LocalPos �𑗂��ē����f�[�^������������
////*******************************************************************************
//BOOL XMonitorAsyncClient::ChangeData(const LocalPos& Loc)
//{
//	LocalPosPacket Packet(Loc);
//	return SendPacket(&Packet);
//}
//
////*******************************************************************************
////	X-Monitor �� AnemometerData �𑗂��ē����f�[�^������������
////*******************************************************************************
//BOOL XMonitorAsyncClient::ChangeData(const AnemometerData& Anemo)
//{
//	AnemometerDataPacket Packet(Anemo);
//	return SendPacket(&Packet);
//}
//
////*******************************************************************************
////	X-Monitor �� PropoData �𑗂��ē����f�[�^������������
////*******************************************************************************
//BOOL XMonitorAsyncClient::ChangeGCSPropoData(const PropoData& Propo)
//{
//	GcsPropoDataPacket Packet(Propo);
//	return SendPacket(&Packet);
//}
//
////*******************************************************************************
////	X-Monitor �o�R�� MAV �Ƀf�[�^�𑗐M����
////*******************************************************************************
//BOOL XMonitorAsyncClient::TransmitPacketToMAV(PACKETID PacketID, const void* pData, size_t nDataLength)
//{
//	MainLinkDataPacket Packet(PacketID, pData, nDataLength);
//	return SendPacket(&Packet);
//}
//
//
////*******************************************************************************
////	X-Monitor �ɐڑ������Ƃ��̏���
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
////	X-Monitor Client ���f�[�^����M�����Ƃ��̏���
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
////	X-Monitor Client �̐ڑ����ؒf���ꂽ�Ƃ��̏���
////*******************************************************************************
//void XMonitorAsyncClient::OnClose()
//{
//	MyWSAConnection::OnClose();
//}