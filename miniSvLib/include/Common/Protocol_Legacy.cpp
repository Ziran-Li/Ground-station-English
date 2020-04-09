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
	// �����o�̏�����
	memset(&m_Rx, 0, sizeof(m_Rx));
	
	// ��M�o�b�t�@�̎擾
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
	// �o�b�t�@�̎擾�Ɏ��s���Ă����ꍇ
	if( m_Rx.pBuff == NULL ) return C_FAIL;
	
	// �����o�̏�����
	_ClearMemory();
	
	return C_OK;
}

CRESULT Protocol_Legacy::Send(unsigned int ID, const void* pBuff, size_t length)
{
	// �o�b�t�@�̎擾�Ɏ��s���Ă����ꍇ
	if( m_Rx.pBuff == NULL ) return C_FAIL;
	
	// ID���͈͊O�̏ꍇ
	if( ID > 0xFF ) return C_FAIL;
	
	CRESULT ret = C_OK;
	
	_UWORD Checksum;
	
	// �`�F�b�N�T���v�Z
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
	// �o�b�t�@�̎擾�Ɏ��s���Ă����ꍇ
	if( m_Rx.pBuff == NULL ) return C_FAIL;
	
	while( m_WirelessDevice.IsDataReceived() ) 
	{
		unsigned char cur;
		m_WirelessDevice.Receive(&cur);	

		if( m_Rx.DataCount < PREAMBLE_LENGTH ) {
			// ----------------------------------
			//	PREAMBLE1/PREAMBLE2 ��M��
			//	�����Fm_Rx.DataCount �� 0 or 1
			// ----------------------------------
			bool ok = false;
			
			if( m_Rx.DataCount == 0 && cur == PREAMBLE1) {
				m_Rx.pBuff[0] = cur;
				ok = true;
			} else {
				m_WirelessDevice.ProcessUnusedCharacter(cur); // �p�P�b�g�Ƃ��ĔF������Ȃ������f�[�^���f�o�C�X�N���X�ɏ���������	
			}
			
			if( m_Rx.DataCount == 1 && cur == PREAMBLE2) {
				m_Rx.pBuff[1] = cur;
				ok = true;
			} else {
				m_WirelessDevice.ProcessUnusedCharacter(m_Rx.pBuff[0]); // �p�P�b�g�Ƃ��ĔF������Ȃ������f�[�^���f�o�C�X�N���X�ɏ���������
				m_WirelessDevice.ProcessUnusedCharacter(cur);
				m_Rx.DataCount = 0;
			}
			
			if( ok ) {
				m_Rx.DataCount++;	
			}
		
		} else if( m_Rx.DataCount < HEADER_LENGTH ) {
			// ----------------------------------
			//	ID ��M��
			//	�����Fm_Rx.nDataCount �� HEADER::LENGTH ��菬����
			// ----------------------------------
			m_Rx.pBuff[m_Rx.DataCount++] = cur;
		
			if( size_t length = m_Communicator.GetPayloadLengthByID(cur) + HEADER_LENGTH + FOOTER_LENGTH ) {
				// �L���ȃp�P�b�gID
				
				if( length <= m_Rx.BufferLength ) {
					// �o�b�t�@�Ɏ��܂�
					m_Rx.CurrentPacketLength = static_cast<unsigned char>(length);
					
				} else {
					// �o�b�t�@�Ɏ��܂�Ȃ�
					_EraseJunkData();
				}
				
			} else {
				// �����ȃp�P�b�gID
				_EraseJunkData();
			}
			
		} else {
			// ----------------------------------
			//	PAYLOAD, �`�F�b�N�T����M��
			//	�����Fm_Rx.nDataCount �� CurrentPacketLength-1 ��菬����
			// ----------------------------------
			m_Rx.pBuff[m_Rx.DataCount++] = cur;
		
			if( m_Rx.DataCount >= m_Rx.CurrentPacketLength ) {
				// ----------------------------------
				//	�S�f�[�^��M����
				// ----------------------------------
			
				if( _VerifyChecksum() ) {
					// �`�F�b�N�T��OK
			
					// �y�C���[�h�̃f�R�[�h
					if( m_Communicator.Decode(*(m_Rx.pBuff + PREAMBLE_LENGTH), m_Rx.pBuff + HEADER_LENGTH, m_Rx.CurrentPacketLength - HEADER_LENGTH - FOOTER_LENGTH) ) {
						// �f�R�[�h����
					
					} else {
						// �f�R�[�h���s	
						
					}
					
					// ��M�o�b�t�@�N���A
					_ClearRxBuffer();
					
				} else {
					// �`�F�b�N�T��NG	
					_EraseJunkData();
				}
			}
		}

		// �o�b�t�@�����ӂꂻ��
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
	// �o�b�t�@��T�����C�����o�C�g��������ΐ؂�l�߂�
	// �폜�����f�[�^�̓f�o�C�X�N���X�ɏ���������
	//
	// �p�^�[��1
	// 0xFF, 0xFF, �S�~1, �S�~2, �S�~3, �S�~4, �S�~5
	//     �f�o�C�X�N���X�ɓn���f�[�^�F0xFF, 0xFF, �S�~1, �S�~2, �S�~3, �S�~4, �S�~5
	//     ������̃o�b�t�@�F��
	//
	// �p�^�[��2
	// 0xFF, 0xFF, �S�~1, �S�~2, 0xFF, 0xFF, Any
	//     �f�o�C�X�N���X�ɓn���f�[�^�F0xFF, 0xFF, �S�~1, �S�~2
	//     ������̃o�b�t�@�F0xFF, 0xFF, Any
	//
	// �p�^�[��3
	// 0xFF, 0xFF, �S�~1, �S�~2, �S�~3, �S�~4, 0xFF
	//     �f�o�C�X�N���X�ɓn���f�[�^�F0xFF, 0xFF, �S�~1, �S�~2, �S�~3, �S�~4
	//     ������̃o�b�t�@�F0xFF
	//
	// �e�X�g�f�[�^�F
	//     pBuff(�����f�[�^)                     pBuff(������)    / ProcessUnusedCharacter �֐��ɓn���ꂽ�f�[�^
	//     0xFF, 0xFF                         -> ��               / 0xFF, 0xFF
	//     0xFF, 0xFF, 0xFF                   -> 0xFF, 0xFF       / 0xFF
	//     0xFF, 0xFF, 0x00                   -> ��               / 0xFF, 0xFF, 0x00
	//     0xFF, 0xFF, 0x00, 0x00, 0x00       -> ��               / 0xFF, 0xFF, 0x00, 0x00, 0x00
	//     0xFF, 0xFF, 0x00, 0x00, 0xFF       -> 0xFF             / 0xFF, 0xFF, 0x00, 0x00
	//     0xFF, 0xFF, 0x00, 0xFF, 0xFF       -> 0xFF, 0xFF       / 0xFF, 0xFF, 0x00
	//     0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00 -> 0xFF, 0xFF, 0x00 / 0xFF, 0xFF, 0x00
	//     0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00 -> ��               / 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00
	//
	
	// �擪+1����0xFF, 0xFF�܂ł͂��݁D�o�b�t�@�ɂ���f�[�^��2�o�C�g�ȉ��̏ꍇ�͑S�폜
	if( m_Rx.DataCount <= PREAMBLE_LENGTH ) {
		for( int i = 0; i < PREAMBLE_LENGTH; i++ ) {
			m_WirelessDevice.ProcessUnusedCharacter(m_Rx.pBuff[i]);
		}
		m_Rx.DataCount = 0;
		return;
	}

	unsigned char Prev = m_Rx.pBuff[PREAMBLE_LENGTH-1];
	int           nDelete = 0; // �폜����o�C�g��

	// 0xFF, 0xFF ��T��
	for( int i = PREAMBLE_LENGTH; i < m_Rx.DataCount; i++ ) {
		unsigned char Cur = m_Rx.pBuff[i];

		if( (Prev == PREAMBLE1) && (Cur == PREAMBLE2) ) {
			// �q�b�g
			nDelete = i - 1;
			break;
		}

		Prev = Cur;
	}
	
	// 0xFF, 0xFF ��������Ȃ������ꍇ
	if( nDelete == 0 ) {
		if( m_Rx.pBuff[m_Rx.DataCount-1] == PREAMBLE1 ) {
			// �Ō��1�o�C�g�� 0xFF �������ꍇ
			nDelete = m_Rx.DataCount-1;
		} else {
			// �ǂ��ɂ� 0xFF, 0xFF �����������ꍇ
			nDelete = m_Rx.DataCount;
		}
	}
	
	// �f�o�C�X�N���X�Ƀp�P�b�g�Ƃ��ĔF������Ȃ������f�[�^������������
	for( int i = 0; i < nDelete; i++ ) {
		m_WirelessDevice.ProcessUnusedCharacter(m_Rx.pBuff[i]);	
	}
	
	// �f�[�^�̐؂�l��
	for( int i = 0; i < m_Rx.DataCount - nDelete; i++ ) {
		m_Rx.pBuff[i] = m_Rx.pBuff[i + nDelete];
	}
	m_Rx.DataCount -= nDelete;
}

size_t Protocol_Legacy::GetRxBufferMinimumRequirement() const
{
	return HEADER_LENGTH + FOOTER_LENGTH;	
}
