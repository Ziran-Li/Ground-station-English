//
//	�ϒ�FIFO�o�b�t�@
//
//	��̉���ɑ΂���̊�{�^�̃p�P�b�g���g�p���Ă��������D
//	�قȂ��{�^�̃p�P�b�g�𓯈����ō��݂����邱�Ƃ͂ł��܂���D
//
//	�o�[�W�����Q�\�z�F
//		�X���b�h�Z�[�t�ɐ݌v���āC�u���b�N���d�Ȃ�Ȃ�����o�b�t�@�ւ̒ǉ��Ɠǂݏo���������ɍs����悤�ɂ���D
//
//	�ύX�����F
//		2010/10/20  �T�C�Y�p�f�[�^�̌^�� unsigned long ���� size_t �֕ύX
//		            NO_LIMIT �萔��ǉ�
//		2010/11/22  �X�^�e�B�b�N���C�u������
//		            �u���b�N�T�C�Y�ƍő�o�b�t�@�T�C�Y��ύX�ł���悤�ɏC��
//		2012/04/22	TxTempBuffer �Ɋ֐���ǉ� (GetLength)
//					�ꕔ�� const �֐���
//
//

#pragma once
#include <list>

namespace CommBuffer {

	//*******************************************************************************
	// ��M�֐��ɓn�����߂̃o�b�t�@�̃|�C���^�ƒ���
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
	// ���M/��M�o�b�t�@�̊�{�N���X
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
			const size_t   nSize;   // �o�b�t�@�̃T�C�Y
			size_t         nFilled; // ���͍ς݃f�[�^��

			DATABLOCK() : pBuff(0), nSize(0), nFilled(0) {}
			DATABLOCK(size_t nLen) : pBuff(new unsigned char[nLen]),
				                 nSize(nLen),
			                         nFilled(0) {}
		};

		typedef ::std::list<DATABLOCK> BlockList;

		size_t    m_nBlockSize;   // �u���b�N�P�̂̍ő�T�C�Y
		size_t    m_nMaximumSize; // �o�b�t�@�S�̂̍ő�T�C�Y

		BlockList m_BlockList;
		size_t    m_nReadFromFront; // List �̐擪���牽�o�C�g�ǂݏo������
		size_t    m_nBuffLeftLen;   // �o�b�t�@�̖����͗̈�̒���
		size_t    m_nTotalBuffLen;  // �m�ۂ����̈�̍��v

	public:
		BufferBase(size_t nBlockSize = DEFAULT, size_t nMaximumSize = NO_LIMIT);
		virtual ~BufferBase();

		// �ݒ�ύX�p�֐�
		void SetBlockSize(size_t nBlockSize);
		void SetMaximumSize(size_t nMaximumSize);

		// �o�b�t�@����p�֐�
		void Skip(size_t nSkip); // �w�肵�����i�P�ʁF�o�C�g�j�����ǂݏo���|�C���g���V�[�N����
		void Clear();            // �o�b�t�@���N���A����

		// ��Ԏ擾�p�֐�
		size_t GetLength() const;     // �L���f�[�^�����擾
		bool   IsAllocatable() const; // �V���Ƀu���b�N���g���\���ǂ������ׂ�

	protected:
		void _ReleaseUntil(BlockList::iterator it);
	};


	//*******************************************************************************
	// ��M�f�[�^���Ǘ����邽�߂̃N���X
	//
	// ���T�C�Y�̃u���b�N�P�ʂŎ�M�o�b�t�@���g������D
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

		// �f�[�^�Z�b�g�p�֐�
		BUFFER PushBackBegin();
		bool   PushBackEnd(size_t ReceiveSize);

		// �f�[�^�ǂݏo���p�֐�
		bool Seek(const unsigned char* pPattern, size_t nSize);
		bool Seek(const char* pPattern, size_t nSize);
		
		// �f�[�^���o�p�֐�
		template<typename T> bool Pop(T* pData, size_t nLen = 0)
		{
			if( nLen == 0 ) nLen = sizeof(T);

			if( sizeof(T) < nLen )   return false; // sizeof(T) �ȉ������w��ł��Ȃ�
			if( nLen > GetLength() ) return false; // �ǂݏo���\�T�C�Y������
			
			return _Pop(pData, sizeof(T), nLen);
		}

		template<typename T> bool PopArray(T* pDest, size_t nItem)
		{
			if( sizeof(T) * nItem > GetLength() ) return false; // �ǂݏo���\�T�C�Y������

			return _PopArray(pDest, sizeof(T), nItem);
		}

		template<typename T> bool Read(size_t nOffset, T* pData, size_t nLen = 0) const
		{
			if( nLen == 0 ) nLen = sizeof(T);

			if( sizeof(T) < nLen )             return false; // sizeof(T) �ȉ������w��ł��Ȃ�
			if( nLen + nOffset > GetLength() ) return false; // �ǂݏo���\�T�C�Y������

			return _Read(nOffset, pData, sizeof(T), nLen);
		}
		
		// �`�F�b�N�T���v�Z�͌p�����R�[�h�̐��`��p�ӂ��邱�Ƃŉ�������

	};

	//*******************************************************************************
	// ���M�f�[�^���Ǘ����邽�߂̃N���X
	//
	// �Œ�T�C�Y�̑��M�o�b�t�@
	// �o�b�N�O���E���h�^�̔񓯊����M�ɂ͎g���Ȃ�
	//*******************************************************************************
	class TxTempBufferBase
	{
	protected:
		unsigned char* m_pBuff;
		size_t         m_nBuffSize; // �o�b�t�@�T�C�Y�i�󔒗̈���܂ށj
		size_t         m_nDataLen;  // ���ɓ��͂��ꂽ�f�[�^�̗ʁi�Ӗ��̂���f�[�^�̒����j

	protected:
		virtual bool _Push(const void* pData, size_t nSize, size_t nLen) = 0;
		virtual bool _PushArray(const void* pData, size_t nItemSize, size_t nItem) = 0;

	public:
		TxTempBufferBase(size_t nSize);
		virtual ~TxTempBufferBase();

		// �f�[�^�Z�b�g�p�֐�
		template<typename T> bool Push(T Data, size_t nLen = 0)
		{
			if( nLen <= 0 ) nLen = sizeof(T);

			if( nLen > sizeof(T) )                return false; // �����s��
			if( m_nDataLen + nLen > m_nBuffSize ) return false; // �o�b�t�@�T�C�Y������

			return _Push(&Data, sizeof(T), nLen);
		}

		template<typename T> bool PushArray(const T* pData, size_t nItem)
		{
			if( m_nDataLen + sizeof(T) * nItem > m_nBuffSize ) return false; // �o�b�t�@�T�C�Y������

			return _PushArray(pData, sizeof(T), nItem);
		}

		// ��Ԏ擾�p�֐�
		size_t GetLength() const;

		// �f�[�^�ǂݏo���p�֐�
		CBUFFER GetBuffer() const;

		// �`�F�b�N�T���ɂ̓N���X�̌p���őΉ�����
	};

	//*******************************************************************************
	// ���M�f�[�^���Ǘ����邽�߂̃N���X
	//
	// ���T�C�Y�ȏ�̃u���b�N�P�ʂŃo�b�t�@���g������D
	// �ő�T�C�Y��ݒ肷��ƃo�b�t�@�̍ő�T�C�Y���I�[�o�[����悤�ȃf�[�^�̓Z�b�g�ł��Ȃ��Ȃ�D
	//*******************************************************************************
	class TxBufferBase : public BufferBase
	{
	protected:
		virtual bool _Push(const void* pData, size_t nSize, size_t nLen) = 0;
		virtual bool _PushArray(const void* pData, size_t nItemSize, size_t nItem) = 0;

	public:
		TxBufferBase(size_t nBlockSize = DEFAULT, size_t nMaximumSize = NO_LIMIT);
		virtual ~TxBufferBase();

		// �f�[�^�Z�b�g�p�֐�
		template<typename T> bool Push(T Data, size_t nLen = 0)
		{
			if( nLen <= 0 ) nLen = sizeof(T);

			if (nLen > sizeof(T)) return false; // �����s��
			if( m_nTotalBuffLen - m_nBuffLeftLen + nLen > m_nMaximumSize ) return false; // �o�b�t�@�T�C�Y������

			return _Push(&Data, sizeof(T), nLen);
		}

		template<typename T> bool PushArray(const T* pData, size_t nItem)
		{
			if( m_nTotalBuffLen - m_nBuffLeftLen + sizeof(T) * nItem > m_nMaximumSize ) return false; // �o�b�t�@�T�C�Y������

			return _PushArray(pData, sizeof(T), nItem);
		}

		// �f�[�^�ǂݏo���p�֐�
		CBUFFER PopFrontBegin();
		bool    PopFrontEnd(size_t SendSize);
	};


	//*******************************************************************************
	// �r�b�O�G���f�B�A���̃A���C�����g�ŒʐM���s���o�[�W����
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
	// ���g���G���f�B�A���̃A���C�����g�ŒʐM���s���o�[�W����
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
