#include "stdafx.h"
#include "Memory.h"

using namespace Memory;

EmbeddedRam::EmbeddedRam()
{
	m_ptr = NULL;
}

EmbeddedRam::~EmbeddedRam() 
{ 	
	// Windowsでメモリリークさせないようにちゃんと開放する
	if( m_ptr ) delete[] m_ptr;
}

_UDWORD* EmbeddedRam::Allocate(size_t size)
{
	// ヒープからメモリ領域を取得する

	size_t new_blocks = size % sizeof(void*) == 0 ? size / sizeof(void*) : size / sizeof(void*) + 1; // ポインタ長の倍数でないときは繰り上げる

	m_ptr = new _UDWORD[new_blocks];

	return m_ptr;
}

IRam* RamCreator::Create()
{
	return new EmbeddedRam();
}