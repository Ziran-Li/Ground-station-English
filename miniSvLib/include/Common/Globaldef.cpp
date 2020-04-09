#include "Globaldef.h"

StateVector::StateVector() 
{ 
	m_Length = 0;
	m_pX     = NULL;
}

StateVector::~StateVector() 
{ 
	if( m_pX ) {
		delete[] m_pX;	
	}
}

CRESULT StateVector::SetLength(size_t length)
{
	if( m_pX == NULL ) {
		// 領域が確保されていない場合，領域を確保して OK
		m_pX = new double[length];
		m_Length = length;
		return C_OK;
		
	} else {
		
		// 領域確保済みの場合，長さが等しければ OK
		if( m_Length == length ) {
			return C_OK;
		} else {
			return C_FAIL;	
		}
	}	
}

size_t StateVector::GetLength() const
{
	return m_Length;	
}

double* StateVector::GetPointer(unsigned int index, size_t length)
{
	// ポインタの取得
	if( m_Length == length && index < m_Length ) {
		return m_pX + index;
	} else {
		return NULL;	
	}
}