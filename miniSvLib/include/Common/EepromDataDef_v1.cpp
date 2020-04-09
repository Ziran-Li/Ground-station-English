#include "EepromDataDef_v1.h"
#include <string.h>

//*******************************************************************************
/// @brief  EepromDataへのコピー用関数
///
/// @param[out] dest - コピー先
/// @param[in]  pSrc - コピー元
/// @param[in]  BlockID - コピー元データのブロックID
//*******************************************************************************
void E2PDEF_Operator::_Copy(EepromData& dest, const void* pSrc, _UWORD BlockID)
{
	dest.BlockID = BlockID;
	memcpy(dest.byte, pSrc, sizeof(dest.byte));
}

//*******************************************************************************
/// @brief  EepromDataからのコピー用関数
///
/// @param[out] pDest - コピー先
/// @param[in]  src   - コピー元
/// @param[in]  nSize - コピー元のサイズ
//*******************************************************************************
void E2PDEF_Operator::_Copy(void* pDest, const EepromData& src, size_t nSize)
{
	memcpy(pDest, src.byte, nSize);
}

//*******************************************************************************
/// @brief  データ比較用関数
///
/// @param[in] pA - 比較対象1
/// @param[in] pB - 比較対象2
/// @param[in] nSize - 比較対象のサイズ
//*******************************************************************************
bool E2PDEF_Operator::_IsSame(const void* pA, const void* pB, size_t nSize)
{
	return memcmp(pA, pB, nSize) == 0;
}