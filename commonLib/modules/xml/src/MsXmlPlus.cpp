#include "stdafx.h"
#include "MsXmlPlus.h"

//MSXML3(?) �g�p�錾
using namespace MSXML2;

using namespace std;

//================================================================================
//	�m�[�h�̕������Int�^�̒l�ɕϊ�����
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
//	�m�[�h�̕������Int64�^�̒l�ɕϊ�����
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
//	�m�[�h�̕������BOOL�^�̒l�ɕϊ�����
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
//	�m�[�h�̕������double�^�̒l�ɕϊ�����
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
//	�m�[�h���X�g�̕������std::vector�ɑ������
//================================================================================
BOOL NodeListTextToStringVector(MSXML2::IXMLDOMNodeListPtr& pNodeList, vector<bstr_t>* pStrVecReturn)
{
	if( pStrVecReturn == NULL ) return FALSE;

	// �N���A
	pStrVecReturn->clear();

	for( int i = 0; i < pNodeList->length; i++ ) {
		// �m�[�h���o
		MSXML2::IXMLDOMNodePtr pNode = pNodeList->Getitem(i);
		
		pStrVecReturn->push_back(pNode->text);
	}

	return TRUE;
}