#include "stdafx.h"
#include "Memory.h"

using namespace Memory;

EmbeddedRam::EmbeddedRam()
{
	m_ptr = NULL;
}

EmbeddedRam::~EmbeddedRam() 
{ 	
	// Windows�Ń��������[�N�����Ȃ��悤�ɂ����ƊJ������
	if( m_ptr ) delete[] m_ptr;
}

_UDWORD* EmbeddedRam::Allocate(size_t size)
{
	// �q�[�v���烁�����̈���擾����

	size_t new_blocks = size % sizeof(void*) == 0 ? size / sizeof(void*) : size / sizeof(void*) + 1; // �|�C���^���̔{���łȂ��Ƃ��͌J��グ��

	m_ptr = new _UDWORD[new_blocks];

	return m_ptr;
}

IRam* RamCreator::Create()
{
	return new EmbeddedRam();
}