#include "stdafx.h"
#include "MsXmlPlus.h"

//MSXML3(?) 使用宣言
using namespace MSXML2;

using namespace std;

//================================================================================
//	ノードの文字列をInt型の値に変換する
//================================================================================
BOOL NodeTextToInt(MSXML2::IXMLDOMNodePtr& pNode, int* pIntReturn)
{
	if( pIntReturn == NULL ) return FALSE;

	if( pNode->text == bstr_t("0") ) {
		*pIntReturn = 0;
		return TRUE;
	} else {
		int n = atoi((char*)pNode->text);
		if( n ) {
			*pIntReturn = n;
			return TRUE;
		} else {
			return FALSE;
		}
	}
}

//================================================================================
//	ノードの文字列をInt64型の値に変換する
//================================================================================
BOOL NodeTextToInt64(MSXML2::IXMLDOMNodePtr& pNode, long long* pIntReturn)
{
	if( pIntReturn == NULL ) return FALSE;

	if( pNode->text == bstr_t("0") ) {
		*pIntReturn = 0;
		return TRUE;
	} else {
		long long n = ::_atoi64((char*)pNode->text);
		if( n ) {
			*pIntReturn = n;
			return TRUE;
		} else {
			return FALSE;
		}
	}
}

//================================================================================
//	ノードの文字列をBOOL型の値に変換する
//================================================================================
BOOL NodeTextToBOOL(MSXML2::IXMLDOMNodePtr& pNode, BOOL* pBoolReturn)
{
	if( pBoolReturn == NULL ) return FALSE;

	if( pNode->text == bstr_t("0") ) {
		*pBoolReturn = FALSE;
		return TRUE;
	} else if( pNode->text == bstr_t("1") ) {
		*pBoolReturn = TRUE;
		return TRUE;
	} else {
		return FALSE;
	}
}

//================================================================================
//	ノードの文字列をdouble型の値に変換する
//================================================================================
BOOL NodeTextToDouble(MSXML2::IXMLDOMNodePtr& pNode, double* pDoubleReturn)
{
	if( pDoubleReturn == NULL ) return FALSE;

	variant_t varDbl = pNode->text;
	varDbl.ChangeType(VT_R8);
	*pDoubleReturn = varDbl.dblVal;

	return TRUE;
}

//================================================================================
//	ノードリストの文字列をstd::vectorに代入する
//================================================================================
BOOL NodeListTextToStringVector(MSXML2::IXMLDOMNodeListPtr& pNodeList, vector<bstr_t>* pStrVecReturn)
{
	if( pStrVecReturn == NULL ) return FALSE;

	// クリア
	pStrVecReturn->clear();

	for( int i = 0; i < pNodeList->length; i++ ) {
		// ノード抽出
		MSXML2::IXMLDOMNodePtr pNode = pNodeList->Getitem(i);
		
		pStrVecReturn->push_back(pNode->text);
	}

	return TRUE;
}