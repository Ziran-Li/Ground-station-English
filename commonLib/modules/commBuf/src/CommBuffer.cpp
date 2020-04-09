
#include "stdafx.h"
#include "CommBuffer.h"
#include "common/cpuinfo.h"
#include <string.h>

using namespace CommBuffer;

//������������������������������������������������������������������������������������������������
//��	BufferBase
//������������������������������������������������������������������������������������������������
//*******************************************************************************
//	�d�l�F
//		��� m_nBlockSize    <= m_nMaximumSize �����藧��
//		��� m_nTotalBuffLen <= m_nMaximumSize �����藧��
//*******************************************************************************

//*******************************************************************************
//	BufferBase �̃R���X�g���N�^
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
//	BufferBase �̃f�X�g���N�^
//*******************************************************************************
BufferBase::~BufferBase() 
{
	BlockList::iterator pp = m_BlockList.begin();

	for( ; pp != m_BlockList.end(); pp++ ) {
		delete pp->pBuff;
	}
}

//*******************************************************************************
//	�u���b�N�T�C�Y���Đݒ肷��
//
//	�����F
//		nBlockSize - �V�����u���b�N�T�C�Y
//
//	����F
//		���łɊm�ۂ���Ă���u���b�N�̃T�C�Y�͕ύX���Ȃ�
//		���ꂩ��m�ۂ����u���b�N����ݒ�l�����f�����
//*******************************************************************************
void BufferBase::SetBlockSize(size_t nBlockSize) 
{
	m_nBlockSize = nBlockSize;
}

//*******************************************************************************
//	�o�b�t�@�̍ő�T�C�Y���Đݒ肷��
//
//	�����F
//		nMaximumSize - �V�����u���b�N�T�C�Y
//
//	����F
//		�u���b�N�T�C�Y��菬���Ȓl�C���łɊm�ۂ���Ă���o�b�t�@��菬���Ȓl
//		�͐ݒ�ł��Ȃ�
//*******************************************************************************
void BufferBase::SetMaximumSize(size_t nMaximumSize)
{
	if( nMaximumSize < m_nBlockSize    ) nMaximumSize = m_nBlockSize;
	if( nMaximumSize < m_nTotalBuffLen ) nMaximumSize = m_nTotalBuffLen;

	m_nMaximumSize = nMaximumSize;
}

//*******************************************************************************
//	�w��o�C�g�����o�b�t�@�̐擪����f�[�^���폜����
//
//	�����F
//		nSkip - �X�L�b�v��
//
//	����F
//		�X�L�b�v���ꂽ�f�[�^�͍폜�����ɂȂ�
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
		// ���S�폜

		if( m_BlockList.size() ) {
			// 1�ȏ�u���b�N�����݂���
			_ReleaseUntil(--pp); // �Ō�̃u���b�N���c���ĊJ��

			pp->nFilled = 0;
			m_nReadFromFront = 0;
			m_nBuffLeftLen   = pp->nSize;
		}
	} else {
		_ReleaseUntil(pp);
	}
}

//*******************************************************************************
//	�o�b�t�@�̃f�[�^�����ׂď�������
//*******************************************************************************
void BufferBase::Clear()
{
	_ReleaseUntil(m_BlockList.end());
	m_nReadFromFront = 0;
	m_nBuffLeftLen   = 0;
}

//*******************************************************************************
//	�L���ȃf�[�^�̒���
//*******************************************************************************
size_t BufferBase::GetLength() const
{
	return m_nTotalBuffLen - m_nReadFromFront - m_nBuffLeftLen;
}

//*******************************************************************************
//	�V���Ƀu���b�N���g���\���ǂ������ׂ�
//*******************************************************************************
bool BufferBase::IsAllocatable() const
{
	return m_nTotalBuffLen < m_nMaximumSize;
}

//*******************************************************************************
//	�w�肳�ꂽ�C�e���[�^�̑O�܂Ńo�b�t�@���J������
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

//������������������������������������������������������������������������������������������������
//��	RxBufferBase
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	RxBufferBase �̃R���X�g���N�^
//*******************************************************************************
RxBufferBase::RxBufferBase(size_t nBlockSize, size_t nMaximumSize) : 
BufferBase(nBlockSize, nMaximumSize)
{
}

//*******************************************************************************
//	RxBufferBase �̃f�X�g���N�^
//*******************************************************************************
RxBufferBase::~RxBufferBase()
{
}

//*******************************************************************************
//	��M�f�[�^�̓o�^���J�n����
//
//	�߂�l�F
//		��M�֐��ɓn�����߂̃o�b�t�@�̃|�C���^�ƃo�b�t�@�̃T�C�Y�������� BUFFER
//		�\����
//
//	�R�[�f�B���O��F
//		�E�߂�l�Ɏ�M�����f�[�^�̒�����Ԃ���M�֐������̂悤�ɒ�`����
//			int receive(void* pBuff, int nBuffSize)
//		�ERxBuffer �̃C���X�^���X�̕ϐ����� Rxb �Ƃ���
//		
//		while( true ) {
//			BUFFER buf = Rxb.PushBackBegin();
//			int nReceive = receive(buf.pBuff, buf.nLen);
//			if( !Rxb.PushBackEnd(nReceive) ) break;
//		};
//
//	���ӎ����F
//		PushBackBegin �` PushBackEnd �̊Ԃ� RxBuffer �̃����o�֐������܂Ȃ�����
//*******************************************************************************
BUFFER RxBufferBase::PushBackBegin() 
{
	void*  pReturnBuff;
	size_t ReturnBuffLen;

	if( m_nBuffLeftLen == 0 ) {
		// �󂫂������Ƃ�

		if( m_nTotalBuffLen <= m_nMaximumSize - m_nBlockSize ) { // ���d�l���A���_�[�t���[���Ȃ�
			// �ő�T�C�Y(m_nBlockSize) �ŗ̈���m�ۉ\

			DATABLOCK block(m_nBlockSize); // ������ new
			pReturnBuff   = block.pBuff;
			ReturnBuffLen = m_nBuffLeftLen = block.nSize;
			m_BlockList.push_back(block);
			m_nTotalBuffLen += ReturnBuffLen;

		} else if( m_nTotalBuffLen < m_nMaximumSize) {
			// m_nTotalBuffLen �� m_nMaximumSize �ɒB����܂ŗ̈���m�ۉ\

			DATABLOCK block(m_nMaximumSize - m_nTotalBuffLen); // ������ new
			pReturnBuff   = block.pBuff;
			ReturnBuffLen = m_nBuffLeftLen = block.nSize;
			m_BlockList.push_back(block);
			m_nTotalBuffLen += ReturnBuffLen;

		} else {
			// �̈�m�ەs�\
			pReturnBuff   = NULL;
			ReturnBuffLen = 0;
		}

	} else {
		// �܂��󂫂�����Ƃ�
		BlockList::iterator pp = --m_BlockList.end();
		pReturnBuff   = pp->pBuff + pp->nFilled;
		ReturnBuffLen = m_nBuffLeftLen;
	}
	return BUFFER(pReturnBuff, ReturnBuffLen);
}

//*******************************************************************************
//	��M�f�[�^�̓o�^����������
//
//	�����F
//		ReceiveSize - ��M�֐����Ԃ�����M�o�C�g��
//
//	�R�[�f�B���O��F
//		�E�߂�l�Ɏ�M�����f�[�^�̒�����Ԃ���M�֐������̂悤�ɒ�`����
//			int receive(void* pBuff, int nBuffSize)
//		�ERxBuffer �̃C���X�^���X�̕ϐ����� Rxb �Ƃ���
//		
//		while( true ) {
//			BUFFER buf = Rxb.PushBackBegin();
//			int nReceive = receive(buf.pBuff, buf.nLen);
//			if( !Rxb.PushBackEnd(nReceive) ) break;
//		};
//
//	�߂�l�F
//		true  - PushBackBegin �Ŕ��s�����o�b�t�@�̃T�C�Y�� ReceiveSize ���������Ƃ�
//		        �˃V�X�e���̎�M�o�b�t�@�ɂ܂��f�[�^���c���Ă���\��������
//		false - PushBackBegin �Ŕ��s�����o�b�t�@�̃T�C�Y��� ReceiveSize ��������
//		        �Ƃ� / ��M�����f�[�^���o�b�t�@�̍ő�l�ɒB�����Ƃ� / �G���[��
//		        �˃V�X�e���̎�M�o�b�t�@�Ƀf�[�^�͖��� or ���s�s�\
//*******************************************************************************
bool RxBufferBase::PushBackEnd(size_t ReceiveSize)
{
	BlockList::iterator footit = --m_BlockList.end();

	if( ReceiveSize > m_nBuffLeftLen ) {
		// ��O
		m_nBuffLeftLen = 0;
		footit->nFilled = footit->nSize;
	} else {
		m_nBuffLeftLen -= ReceiveSize;
		footit->nFilled += ReceiveSize;
	}

	return (m_nBuffLeftLen == 0) && IsAllocatable();
}

//*******************************************************************************
//	�w�肳�ꂽ�f�[�^��ƈ�v����ӏ��܂œ����f�[�^�̐擪�ʒu���ړ�����
//
//	�����F
//		pPattern - �����������o�C�g��̐擪�̃|�C���^
//		nSize    - �f�[�^��̑傫��
//
//	�߂�l�F
//		true  - ���S��v�����ӏ����������ꍇ
//		false - ��L�ȊO
//
//	���S��v�ӏ����Ȃ������ꍇ�̓���F
//		�����f�[�^�̍Ō㕔�Ŏw��o�C�g��ƈꕔ��v�����ꍇ�́C���̐擪�ӏ��܂�
//		�擪�ʒu���ړ�����D
//		�S����v����ӏ����Ȃ������ꍇ�� RxBuffer �͏����������
//*******************************************************************************
bool RxBufferBase::Seek(const unsigned char* pPattern, size_t nSize)
{
	int nMatch = 0;
	BlockList::iterator FirstMatchBlock;
	size_t              FirstMatchIndex;

	BlockList::iterator pp = m_BlockList.begin();

	for( ; pp != m_BlockList.end(); pp++ ) {

		unsigned char* pTgt = pp->pBuff + m_nReadFromFront;

		for( ; m_nReadFromFront < pp->nFilled; m_nReadFromFront++ ) { // �u���b�N���̗L���f�[�^�̖����܂Ń��[�v

			if( *(pTgt++) == *((unsigned char*)pPattern + nMatch) ) {
				// pPattern �� nMatch �Ԗڂ܂ň�v
				if( nMatch == 0 ) {
					FirstMatchBlock = pp;               // �擪����v�����Ƃ��̃C�e���[�^���L�^�i�u���b�N�̍폜�Ɏg�p�j
					FirstMatchIndex = m_nReadFromFront; // �擪����v�����Ƃ��̃o�b�t�@�̃C���f�b�N�X���L�^
				}

				nMatch++;

				if( nMatch == nSize ) {
					// ���S��v
					_ReleaseUntil(FirstMatchBlock); // FirstMatchBlock �̈�O�܂ł̃u���b�N���폜
					m_nReadFromFront = FirstMatchIndex;
					break;
				}

			} else {
				// ��v���Ȃ������Ƃ��͂�蒼��
				nMatch = 0;
			}
		}

		if( nMatch == nSize ) break;

		m_nReadFromFront = 0;
	}

	//
	if( nMatch == 0 ) {
		// �Ō�̍Ō�܂Ō�����Ȃ����� �� 1�u���b�N�c���ă��Z�b�g
		if( m_BlockList.size() ) {
			// 1�u���b�N�ȏ゠��
			_ReleaseUntil(--m_BlockList.end());
			BlockList::iterator pp = m_BlockList.begin();
			pp->nFilled = 0;
			m_nReadFromFront = 0;
			m_nBuffLeftLen   = pp->nSize;
		}
	} else {
		// ���S��v or �o�b�t�@�̍Ō�ňꕔ��v
		_ReleaseUntil(FirstMatchBlock);
		m_nReadFromFront = FirstMatchIndex;
	}

	return nMatch == nSize;
}

bool RxBufferBase::Seek(const char *pPattern, size_t nSize)
{
	return Seek(reinterpret_cast<const unsigned char*>(pPattern), nSize); // �o�C�i����̔�r�Ȃ̂ő��v
}

//������������������������������������������������������������������������������������������������
//��	TxTempBufferBase
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	TxTempBufferBase �̃R���X�g���N�^
//*******************************************************************************
TxTempBufferBase::TxTempBufferBase(size_t nSize)
{
	m_pBuff = new unsigned char[nSize];
	m_nBuffSize = nSize;
	m_nDataLen = 0;
}

//*******************************************************************************
//	TxTempBufferBase �̃f�X�g���N�^
//*******************************************************************************
TxTempBufferBase::~TxTempBufferBase()
{
	delete m_pBuff;
}

//*******************************************************************************
//	�L���f�[�^�����擾����
//*******************************************************************************
size_t TxTempBufferBase::GetLength() const
{
	return m_nDataLen;
}

//*******************************************************************************
//	���M�֐��ɓn���o�b�t�@�������\���̂��擾����
//*******************************************************************************
CBUFFER TxTempBufferBase::GetBuffer() const
{
	return CBUFFER(m_pBuff, m_nDataLen);
}

//������������������������������������������������������������������������������������������������
//��	TxBufferBase
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	TxBufferBase �̃R���X�g���N�^
//*******************************************************************************
TxBufferBase::TxBufferBase(size_t nBlockSize, size_t nMaximumSize) :
BufferBase(nBlockSize, nMaximumSize)
{
}

//*******************************************************************************
//	TxBufferBase �̃f�X�g���N�^
//*******************************************************************************
TxBufferBase::~TxBufferBase() 
{
}

//*******************************************************************************
//	�o�b�t�@�̑��M���J�n����
//
//	�߂�l�F
//		���M�֐��ɓn�����߂̃o�b�t�@�̃|�C���^�ƃo�b�t�@�̃T�C�Y�������� CBUFFER
//		�\����
//
//	�R�[�f�B���O��F
//		�E�߂�l�ɑ��M�����f�[�^�̒�����Ԃ����M�֐������̂悤�ɒ�`����
//			int send(void* pBuff, int nBuffSize)
//		�ETxBuffer �̃C���X�^���X�̕ϐ����� Txb �Ƃ���
//
//		while( true ) {
//			CBUFFER buf = Txb.PopFrontBegin();
//			int nSend = send(buf.pBuff, buf.nLen);
//			if( !Txb.PopFrontEnd(nSend) ) break;
//		}
//
//	���ӎ����F
//		PopFrontBegin �` PopFrontEnd �̊Ԃ� TxBuffer �̃����o�֐������܂Ȃ�����
//*******************************************************************************
CBUFFER TxBufferBase::PopFrontBegin()
{
	if( m_BlockList.size() ) {
		// �o�b�t�@�����݂���
		BlockList::iterator pp = m_BlockList.begin();

		return CBUFFER(pp->pBuff + m_nReadFromFront, pp->nFilled - m_nReadFromFront);
	} else {
		// �o�b�t�@�����݂��Ȃ�
		return CBUFFER(NULL, 0);
	}
}

//*******************************************************************************
//	�o�b�t�@�̑��M����������
//
//	�����F
//		SendSize - ���M�֐����Ԃ������M�o�C�g��
//
//	�߂�l�F
//		true  - PopFrontBegin �Ŕ��s�����o�b�t�@�̃T�C�Y�� SendSize ���������Ƃ�
//		        �˃V�X�e���̑��M�o�b�t�@�ɂ͋󂫂�����\��������
//		false - PopFrontBegin �Ŕ��s�����o�b�t�@�̃T�C�Y��� SendSize ���������Ƃ�
//		        / ���M�ł���f�[�^�������Ƃ� / �G���[��
//		        �˃V�X�e���̑��M�o�b�t�@�ɋ󂫂͖��� or ���s�s�\
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
		Skip(pp->nFilled - m_nReadFromFront); // �u���b�N�Ɋ܂܂��f�[�^�̃T�C�Y�𒴂��邱�Ƃ͖���
	} else {
		ret = false;
		Skip(SendSize);
	}

	if( GetLength() == 0 ) {
		ret = false;
	}

	return ret;
}

//������������������������������������������������������������������������������������������������
//��	BigEndianProtocol::RxBuffer
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	�f�[�^��ǂݏo��
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
//	�z��f�[�^��ǂݏo��
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
//	�ǂݏo���_���V�t�g�����Ƀf�[�^��ǂݏo��
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

//������������������������������������������������������������������������������������������������
//��	BigEndianProtocol::TxTempBuffer
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	�f�[�^����������
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
//	�z��f�[�^����������
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

//������������������������������������������������������������������������������������������������
//��	BigEndianProtocol::TxBuffer
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	�f�[�^����������
//*******************************************************************************
bool BigEndianProtocol::TxBuffer::_Push(const void *pData, size_t nSize, size_t nLen)
{
	BlockList::iterator pp;

	if( m_nBuffLeftLen < nLen ) {
		// �o�b�t�@�ɋ󂫂������Ƃ�

		size_t nAllocateSize;

		if( nLen - m_nBuffLeftLen > m_nBlockSize ) {
			// �ŏ��u���b�N�T�C�Y�����傫���T�C�Y�̗̈悪�K�v�ȂƂ�
			nAllocateSize = nLen - m_nBuffLeftLen;
		} else {
			// �ŏ��u���b�N�T�C�Y�����������T�C�Y�̗̈悪�K�v�ȂƂ�
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
		// �o�b�t�@�ɏ\���ȋ󂫂�����Ƃ�

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

	// ���̃u���b�N��
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

	// ���̃u���b�N��
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
//	�z��f�[�^����������
//*******************************************************************************
bool BigEndianProtocol::TxBuffer::_PushArray(const void* pData, size_t nItemSize, size_t nItem)
{
	BlockList::iterator pp;
	size_t nAllocateSize = 0;

	if( nItem * nItemSize == 0 ) return true;

	if( m_nBuffLeftLen < nItemSize * nItem ) {
		// �o�b�t�@�ɋ󂫂������Ƃ�

		if( nItemSize * nItem - m_nBuffLeftLen > m_nBlockSize ) {
			// �ŏ��u���b�N�T�C�Y�����傫���T�C�Y�̗̈悪�K�v�ȂƂ�
			nAllocateSize = nItemSize * nItem - m_nBuffLeftLen;
		} else {
			// �ŏ��u���b�N�T�C�Y�����������T�C�Y�̗̈悪�K�v�ȂƂ�
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
		// �o�b�t�@�ɏ\���ȋ󂫂�����Ƃ�

		pp = --m_BlockList.end();
	}

#if BYTEORDER == BIG_ENDIAN

	if( nAllocateSize ) {
		// �̈�̊g�����s����

		memcpy(pp->pBuff + pp->nFilled, pData, m_nBuffLeftLen - nAllocateSize);
		pp->nFilled = pp->nSize;

		pp++;

		pp->nFilled = nItemSize * nItem - (m_nBuffLeftLen - nAllocateSize);
		memcpy(pp->pBuff, (unsigned char*)pData + m_nBuffLeftLen - nAllocateSize, pp->nFilled);

	} else {
		// �̈�̊g�����s��Ȃ�����

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
		// �̈�̊g�����s����

		unsigned char* pDestEnd = pp->pBuff + pp->nSize;

		do {
			*(pDest++) = *(--pSrc);

			if( pSrc == pElementEnd ) {
				pSrc        += (nItemSize << 1);
				pElementEnd += nItemSize;
			}
		} while( pDest != pDestEnd );
		pp->nFilled = pp->nSize;

		// ���̃u���b�N��
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
		// �̈�̊g�����s��Ȃ�����

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

//������������������������������������������������������������������������������������������������
//��	LittleEndianProtocol::RxBuffer
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	�f�[�^��ǂݏo��
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
//	�z��f�[�^��ǂݏo��
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

		// �R�s�[�T�C�Y������
		size_t nCopy = static_cast<unsigned int>(pEnd - _pDest) > pp->nFilled - m_nReadFromFront ? pp->nFilled - m_nReadFromFront : pEnd - _pDest;
		
		// �������R�s�[
		memcpy(_pDest, pp->pBuff + m_nReadFromFront, nCopy);
		_pDest += nCopy;
		m_nReadFromFront += nCopy;

		// �I������
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
//	�ǂݏo���_���V�t�g�����Ƀf�[�^��ǂݏo��
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

//������������������������������������������������������������������������������������������������
//��	BigEndianProtocol::TxTempBuffer
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	�f�[�^����������
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
//	�z��f�[�^����������
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


//������������������������������������������������������������������������������������������������
//��	BigEndianProtocol::TxBuffer
//������������������������������������������������������������������������������������������������

//*******************************************************************************
//	�f�[�^����������
//*******************************************************************************
bool LittleEndianProtocol::TxBuffer::_Push(const void *pData, size_t nSize, size_t nLen)
{
	BlockList::iterator pp;

	if( m_nBuffLeftLen < nLen ) {
		// �o�b�t�@�ɋ󂫂������Ƃ�

		size_t nAllocateSize;

		if( nLen - m_nBuffLeftLen > m_nBlockSize ) {
			// �ŏ��u���b�N�T�C�Y�����傫���T�C�Y�̗̈悪�K�v�ȂƂ�
			nAllocateSize = nLen - m_nBuffLeftLen;
		} else {
			// �ŏ��u���b�N�T�C�Y�����������T�C�Y�̗̈悪�K�v�ȂƂ�
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
		// �o�b�t�@�ɏ\���ȋ󂫂�����Ƃ�

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

	// ���̃u���b�N��
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
//	�z��f�[�^����������
//*******************************************************************************
bool LittleEndianProtocol::TxBuffer::_PushArray(const void* pData, size_t nItemSize, size_t nItem)
{
	BlockList::iterator pp;
	size_t nAllocateSize = 0;

	if( nItem * nItemSize == 0 ) return true;

	if( m_nBuffLeftLen < nItemSize * nItem ) {
		// �o�b�t�@�ɋ󂫂������Ƃ�

		if( nItemSize * nItem - m_nBuffLeftLen > m_nBlockSize ) {
			// �ŏ��u���b�N�T�C�Y�����傫���T�C�Y�̗̈悪�K�v�ȂƂ�
			nAllocateSize = nItemSize * nItem - m_nBuffLeftLen;
		} else {
			// �ŏ��u���b�N�T�C�Y�����������T�C�Y�̗̈悪�K�v�ȂƂ�
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
		// �o�b�t�@�ɏ\���ȋ󂫂�����Ƃ�

		pp = --m_BlockList.end();
	}

#if BYTEORDER == BIG_ENDIAN

#error Not programmed yet.

#elif BYTEORDER == LITTLE_ENDIAN

	if( nAllocateSize ) {
		// �̈�̊g�����s����

		memcpy(pp->pBuff + pp->nFilled, pData, m_nBuffLeftLen - nAllocateSize);
		pp->nFilled = pp->nSize;

		pp++;

		pp->nFilled = nItemSize * nItem - (m_nBuffLeftLen - nAllocateSize);
		memcpy(pp->pBuff, (unsigned char*)pData + m_nBuffLeftLen - nAllocateSize, pp->nFilled);

	} else {
		// �̈�̊g�����s��Ȃ�����

		memcpy(pp->pBuff + pp->nFilled, pData, nItemSize * nItem);
		pp->nFilled += nItemSize * nItem;
	}

#else
#error Byte order is not defined.
#endif

	m_nBuffLeftLen -= nItemSize * nItem;

	return true;
}