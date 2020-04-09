#ifndef STRXML_H
#define STRXML_H
#include <stdlib.h>
#include <stdio.h>
#include <comutil.h>
#include <tchar.h>
#import  <msxml6.dll>  // MSXML���C�u�����̃C���|�[�g  
#include "Base64Data.h" 
#include "XmlData.h"
#include "XmlMsgAttribute.h"
//
//�@StrXml �N���X
//
class StrXml
{
private:
	HRESULT loadResult;
	MSXML2::IXMLDOMDocument2Ptr pDoc;	//DOM�h�L�������g�|�C���^
	HRESULT loadStrXml(BSTR bstrXML);
	size_t  getAttTID(BSTR dataType);
	VARIANT_BOOL isSame(BSTR keyword, BSTR str);

public:
	StrXml();
	StrXml(BSTR bstrXML);
	_bstr_t getData(BSTR keyword);
	_bstr_t getData(BSTR keyword, ELEMATTRIBUTES attr);
	_bstr_t convertTxt2Bin();
	_bstr_t convertBin2Txt();
	void    childNode(IElementPtr  pElem, int );
	VARIANT_BOOL getChileNodeName(vector<_bstr_t> path, vector<_bstr_t> *nodename);
	
};

#endif /**/
