//
//
//	Note:
//	�f�b�h���b�N�̖�����������i2��ڂ̃��b�N�ȍ~�Ƀ^�C���A�E�g��ݒ肷��j
//	�����I�u�W�F�N�g�̓������b�N��i��񋟂���
//
//	�{�w�b�_�t�@�C���̃C���N���[�h�O�� #define USE_BOOST_LIBRARY ���`����ƁCboost��shared_ptr���g�p���܂��D
//

#pragma once

#include <windows.h>
#include <memory>



//#define USE_BOOST_LIBRARY

#ifdef USE_BOOST_LIBRARY
#include <boost/shared_ptr.hpp>
#endif

//*******************************************************************************
//	�����N���X
//
//	���̃C���X�^���X�� CAccessor ���ێ�����X�}�[�g�|�C���^�ɂ���ĊǗ�����Ă���D
//	�{�N���X�̃C���X�^���X���Q�Ƃ��� CAccessor ���P�������Ȃ����Ƃ��ɁC�X�}�[�g
//	�|�C���^�̊J���ɂ���ăf�X�g���N�^���Ă΂�CCExclusiveObject �̃~���[�e�b�N�X
//	���V�O�i����Ԃɖ߂�D
//	�i���I�u�W�F�N�g�ւ̔r���I�A�N�Z�X�����������ہj
//*******************************************************************************
class _CUnlocker
{
public:
	HANDLE m_hMutex;
	void*  m_pData;

public:
	//----------------------------------------
	// _CUnlocker �̃R���X�g���N�^
	//----------------------------------------
	_CUnlocker(HANDLE hMutex, void* pData)
	{
		m_hMutex = hMutex;
		m_pData  = pData;
	}

	//----------------------------------------
	// _CUnlocker �̃f�X�g���N�^
	//----------------------------------------
	virtual ~_CUnlocker()
	{
		// Mutex �̃A�����b�N
		::ReleaseMutex(m_hMutex);
	}
};

//*******************************************************************************
//	�A�N�Z�X�q
//
//	�r��������󂯂�f�[�^�փA�N�Z�X���邽�߂̃I�u�W�F�N�g�D
//	�f�[�^�ւ̓A���[���Z�q��p���ăA�N�Z�X����D
//	
//	���ӁF
//	�A�N�Z�X�q�̃C���X�^���X�͎����ϐ��Ɠ����悤�Ɉ������ƁD
//	�J������Ȃ�����r������͏I�����Ȃ��̂ŁC�f�[�^�ւ̃A�N�Z�X���I������璼����
//	�J�����s�����ƁD
//	�܂��C�J���͕K���e�ł��� CExclusiveObject �̃C���X�^���X���J�������O�ɍs�����ƁD
//
//	�A�N�Z�X�q����̓f�[�^�ւ̃|�C���^��Q�Ƃ��擾�ł��邪�C�擾�����|�C���^��Q�Ƃ�
//	�΂���A�N�Z�X�́C�A�N�Z�X�q�̎��̂�����Ԃ̂݋������D�A�N�Z�X�q��������ꂽ��
//	�͎擾�����|�C���^��Q�ƂɃA�N�Z�X���Ă͂����Ȃ��D
//	���̋@�\�͊�{�I�ɁC�֐��փ|�C���^��Q�Ƃ�n���Ȃ���΂Ȃ�Ȃ��ꍇ�Ɏg�p����D
//
//	�����ӓ_��
//	func(m_foo.GetAccessor()->bar)
//	�̂悤�ɏ����ƁC�֐� func �����s���ꂽ��ŃA�N�Z�X�q�̉�����s����D
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

	UNLOCKER_PTR m_ParentUnlocker; // �e�X�g��

public:
	//----------------------------------------
	// CAccessor �̃R���X�g���N�^
	//----------------------------------------
	CAccessor(HANDLE hMutex, TYPE* pData) : m_Unlocker(UNLOCKER_PTR(new _CUnlocker(hMutex, (void*)(pData)))) {}

	//----------------------------------------
	// CAccessor �̃R�s�[�R���X�g���N�^
	//----------------------------------------
	CAccessor(CAccessor& obj)
	{
		m_Unlocker = obj.m_Unlocker;
		m_ParentUnlocker = obj.m_ParentUnlocker;
	}

	//----------------------------------------
	// ��ʂ̃A�N�Z�X�q��o�^����i�e�X�g���j
	//
	//		[PARENT] �� [this] �̂悤�ȃ��b�N�֌W���P�̃A�N�Z�X�q�Ŏ����������Ƃ��Ɏg�p����
	//----------------------------------------
	template<typename ACCESSOR> void setParent(ACCESSOR& parent)
	{
		m_ParentUnlocker = parent.m_Unlocker;
	}
	
	//----------------------------------------
	// �A�N�Z�X���̃����[�X
	//
	//     �f�[�^�ւ̔r���I�A�N�Z�X���蓮�ŏI������
	//     release() ���s������̓f�[�^�փA�N�Z�X���邱�Ƃ͂ł��Ȃ��D
	//----------------------------------------
	void release()
	{
		m_Unlocker.reset();
		m_ParentUnlocker.reset();
	}

	//----------------------------------------
	// �f�[�^�̃|�C���^���擾����
	//----------------------------------------
	TYPE* get() const
	{
		return static_cast<TYPE*>(m_Unlocker->m_pData);
	}

	//----------------------------------------
	// �f�[�^�̎Q�Ƃ��擾����
	//----------------------------------------
	TYPE& getReference() const
	{
		return *(static_cast<TYPE*>(m_Unlocker->m_pData));
	}

	//----------------------------------------
	// �A���[���Z�q�̃I�[�o�[���[�h
	//
	//     �r��������󂯂�f�[�^�ւ̃A�N�Z�X��i��񋟂���
	//----------------------------------------
	TYPE* operator -> () const
	{
		return static_cast<TYPE*>(m_Unlocker->m_pData);
	}

	//----------------------------------------
	// �|�C���^�ւ̃L���X�g
	//----------------------------------------
	operator TYPE*() const
	{
		return static_cast<TYPE*>(m_Unlocker->m_pData);
	}

	//----------------------------------------
	// �|�C���^�̊ԐڎQ�Ɖ��Z�q * �̃I�[�o�[���[�h
	//----------------------------------------
	TYPE& operator * () const
	{
		return *static_cast<TYPE*>(m_Unlocker->m_pData);
	}
};

//*******************************************************************************
//	�r���I�f�[�^�I�u�W�F�N�g
//
//	�f�[�^�ւ̃A�N�Z�X���P�̃X���b�h�Ɍ��肷�邽�߂̃I�u�W�F�N�g�D
//	�f�[�^�ւ̃A�N�Z�X�� GetAccessor() �Ŏ擾���� ACCESSOR �I�u�W�F�N�g��ʂ��čs���D
//	ACCESSOR �̃C���X�^���X�����݂��Ă���ԁC���̃X���b�h�͓���̃f�[�^�փA�N�Z�X
//	���邱�Ƃ͂ł��Ȃ��D�i GetAccessor() ���Ăяo�������_�ŃX���b�h���X�g�b�v����)
//
//	CONST_ACCESSOR �� const �֐�����������Ƃ��Ɏg�p����
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
	// CExclusiveObject �̃R���X�g���N�^
	//----------------------------------------
	CExclusiveObject()
	{
		m_hMutex = ::CreateMutex(NULL, FALSE, NULL);
	}

	//----------------------------------------
	// CExclusiveObject �̃R���X�g���N�^�i�����l�w��j
	//----------------------------------------
	CExclusiveObject(const TYPE& obj) : m_Data(obj)
	{
		m_hMutex = ::CreateMutex(NULL, FALSE, NULL);
	}

	//----------------------------------------
	// CExclusiveObject �̃f�X�g���N�^
	//----------------------------------------
	virtual ~CExclusiveObject()
	{
		::CloseHandle(m_hMutex);
	}

	//----------------------------------------
	// �A�N�Z�X�q���擾����
	//----------------------------------------
	virtual ACCESSOR GetAccessor()
	{
		// Mutex �̃��b�N
		::WaitForSingleObject(m_hMutex, INFINITE);

		return ACCESSOR(m_hMutex, &m_Data);
	}

	//----------------------------------------
	// const �A�N�Z�X�q���擾����
	//----------------------------------------
	virtual CONST_ACCESSOR GetConstAccessor() const
	{
		// Mutex �̃��b�N
		::WaitForSingleObject(m_hMutex, INFINITE);

		return CONST_ACCESSOR(m_hMutex, &m_Data);
	}
};


//*******************************************************************************
//	�r���I�|�C���^�I�u�W�F�N�g
//
//	�r���I�f�[�^�I�u�W�F�N�g�̃|�C���^��
//
//	�Ƃ肠�����f�[�^�͕��ʂ̃|�C���^�ŕێ�
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
	// �A�N�Z�X�q���擾����
	//----------------------------------------
	ACCESSOR GetAccessor() 
	{
		// Mutex �̃��b�N
		::WaitForSingleObject(m_hMutex, INFINITE);

		//return ACCESSOR(m_pMutex, m_DataPtr.get());
		return ACCESSOR(m_hMutex, m_pData);
	}
};