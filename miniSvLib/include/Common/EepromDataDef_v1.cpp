#include "EepromDataDef_v1.h"
#include <string.h>

//*******************************************************************************
/// @brief  EepromData�ւ̃R�s�[�p�֐�
///
/// @param[out] dest - �R�s�[��
/// @param[in]  pSrc - �R�s�[��
/// @param[in]  BlockID - �R�s�[���f�[�^�̃u���b�NID
//*******************************************************************************
void E2PDEF_Operator::_Copy(EepromData& dest, const void* pSrc, _UWORD BlockID)
{
	dest.BlockID = BlockID;
	memcpy(dest.byte, pSrc, sizeof(dest.byte));
}

//*******************************************************************************
/// @brief  EepromData����̃R�s�[�p�֐�
///
/// @param[out] pDest - �R�s�[��
/// @param[in]  src   - �R�s�[��
/// @param[in]  nSize - �R�s�[���̃T�C�Y
//*******************************************************************************
void E2PDEF_Operator::_Copy(void* pDest, const EepromData& src, size_t nSize)
{
	memcpy(pDest, src.byte, nSize);
}

//*******************************************************************************
/// @brief  �f�[�^��r�p�֐�
///
/// @param[in] pA - ��r�Ώ�1
/// @param[in] pB - ��r�Ώ�2
/// @param[in] nSize - ��r�Ώۂ̃T�C�Y
//*******************************************************************************
bool E2PDEF_Operator::_IsSame(const void* pA, const void* pB, size_t nSize)
{
	return memcmp(pA, pB, nSize) == 0;
}