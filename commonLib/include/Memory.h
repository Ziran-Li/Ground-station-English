///
/// @file       Memory.h
///
/// @brief      �������̈�̊��蓖�Ă��B�����邽�߂̃N���X�Q
///
///             �������̈�̊��蓖�Ă��B�����A�z��N���X����ă������̈�ɃA�N�Z�X����@�\��񋟂��܂�
///             �r���h�̐ݒ�ɂ���ăq�[�v���犄�蓖�Ă邩�ASDRAM�̃A�h���X��Ԃ��犄�蓖�Ă邩�؂�ւ��邱�Ƃ��ł��܂�
///
/// @author     iwakura
///


#ifndef INCLUDED_MEMORY_H
#define INCLUDED_MEMORY_H

#include "Common/StandardDef.h"

namespace Memory
{
	///
	/// @brief  RAM���ۃN���X
	///
	class IRam
	{
	public:
		///
		/// @brief  �f�X�g���N�^
		///
		virtual ~IRam() {};
	
		///
		/// @brief  �������̈�̊m�ہi�������z�֐��j
		///
		virtual _UDWORD* Allocate(size_t size) = 0;
	};

	///
	/// @brief  RAM��ۃN���X�i����RAM�j
	///
	///         �����j�b�g��Windows�Ńr���h����ꍇ�Ɏg�p����
	///
	class EmbeddedRam : public IRam
	{
		_UDWORD* m_ptr;
	
	public:
	
		///
		/// @brief  �R���X�g���N�^
		///
		EmbeddedRam();
	
		///
		/// @brief  �f�X�g���N�^
		///
		virtual ~EmbeddedRam();
	
		///
		/// @brief  �������̈�̊m��
		///
		/// @param[in] size �m�ۂ���o�C�g��
		/// @return �̈�̐擪�̃|�C���^ / NULL - �̈�m�ۂɎ��s
		///
		virtual _UDWORD* Allocate(size_t size);
	};


	///
	/// @brief  �R���p�C���X�C�b�`���B�����邽�߂̃C���X�^���X�����N���X
	///
	class RamCreator
	{
	public:
		///
		/// @brief  �C���X�^���X�̐���
		///
		IRam* Create();	
	};

	///
	/// @brief  �^���m�肷�邽�߂̃e���v���[�g�N���X
	///
	template<class T> class TypedMemoryBlock
	{
		IRam* m_pRam;
	
	public:
	
		///
		/// @brief  �R���X�g���N�^
		///
		TypedMemoryBlock()
		{
			RamCreator creator; 
			m_pRam = creator.Create(); // RAM�N���X�𐶐�����
		}

		///
		/// @brief  �f�X�g���N�^
		///
		virtual ~TypedMemoryBlock()
		{
			delete m_pRam;	
		}

		///
		/// @brief  �������̈���m�ۂ��A�e���v���[�g�����̌^�Ń|�C���^��Ԃ�
		///
		/// @param[in] length �v�f��
		/// @return �擪�v�f�̃|�C���^ / NULL - �̈�m�ۂɎ��s
		///
		T* Allocate(size_t length)
		{
			return reinterpret_cast<T*>(m_pRam->Allocate(GetAllocateSize(length)));
		}
		
		///
		/// @brief  �v�f������m�ۂ���郁�����̈�̃o�C�g�������߂�
		///
		/// @param[in] length �v�f��
		/// @return �m�ۂ����o�C�g��
		///
		static size_t GetAllocateSize(size_t length)
		{
			T dummy[2];
		
			size_t ElementSize = sizeof(dummy) / 2; // �p�f�B���O���܂߂�1�v�f�̃T�C�Y

			dummy[0]; // warning�΍�
			
			return length * ElementSize;
		}
	};

	///
	/// @brief  �z��N���X
	///
	template<class T>class Array
	{
		T*     m_ptr;
		size_t m_length;
		
		TypedMemoryBlock<T> m_memory_block;
	
	public:
	
		///
		/// @brief  �R���X�g���N�^
		///
		///         �������̈���m�ۂ��܂�
		///
		/// @param[in] length �z��̗v�f��
		///
		Array(size_t length)
		{
			m_ptr = m_memory_block.Allocate(length);
			m_length = length;
		};
	
		///
		/// @brief  �z��I�A�N�Z�X�̃T�|�[�g
		///
		///         []�w��ɂ��I�u�W�F�N�g��z��Ƃ��ăA�N�Z�X�ł���悤�ɂ��܂�
		///
		/// @param[in] n �C���f�b�N�X
		/// @return �v�f�ւ̎Q��
		///
		inline T& operator [] (int n) const
		{
			return m_ptr[n];
		}
		
		///
		/// @brief  �̈�m�ۂɐ������Ă��邩���ׂ�
		///
		/// @return true - ���� / false - ���s
		///
		inline bool IsAllocationSucceeded() const
		{
			return m_ptr != NULL;	
		}
		
		///
		/// @brief  �m�ۍς݂̃������̈�̃T�C�Y���o�C�g�P�ʂŎ擾����
		///
		/// @return �p�f�B���O���܂ފm�ۍς݂̃o�C�g��
		///
		inline size_t GetAllocatedMemorySize() const
		{
			return GetAllocateSize(m_length);
		}
	};
}


#endif