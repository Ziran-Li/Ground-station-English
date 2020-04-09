#include "stdafx.h"
#include <tchar.h>
#include "XmlData.h"
//
//  コンストラクタ
//
XmlData::XmlData(HRESULT *hr)
{
	//XmlDocument
	*hr  =pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument60) );
    if (FAILED(*hr)) 
    {
        printf("Failed to instantiate an XML DOM.\n");
        return; 
    }
	pDoc->async = VARIANT_FALSE;	

	//XML declaration
	IProcessingInstructionPtr pPI = NULL;
	pPI = pDoc->createProcessingInstruction(L"xml",XML_VERSION); 
	pDoc->insertBefore(pPI, pDoc->documentElement.GetInterfacePtr());
	//
	// memo: XML_VERSIONが	(_T("version='1.0' encoding='UTF-8'"))の場合
	//       XMLに encoding="UTF-8"が出力されない。
	//       MSXMLのデフォルトがUTF-8であり問題ない。

	//ルート
    IElementPtr pRoot = pDoc->createElement(ROOT_TAG);

	//ルートを追加
    pDoc->appendChild(pRoot);
	
}

//
// メッセージタグ追加
//
void XmlData::addMessageTag(class MessageTag *msg)
{
	//ルートポインタ
	IElementPtr pRoot = pDoc->getElementsByTagName(ROOT_TAG)->Getitem(0);

	//messageタグを追加
	IElementPtr pMsg = addChild(pRoot, msg);

	//反映
	msg->setElementPtr(pMsg);

}

//
// 要素名を用いてElement追加
//
IElementPtr XmlData::addChild(IElementPtr pParentPtr, LPCTSTR elemName)
{

    // 要素作成
    IElementPtr pElem = pDoc->createElement(elemName);
	// 追加
	pParentPtr->appendChild(pElem);
	if(CRLF)pParentPtr->appendChild(pDoc->createTextNode(L"\n"));//

	return pElem;

}

//
// ElementDataクラスを用いてElement追加
//
IElementPtr XmlData::addChild(IElementPtr pParentPtr, ElementData *elem)
{
	 
    // 要素作成
    IElementPtr pDatas = pDoc->createElement(elem->getName());
	if(elem->getText() != NULL)pDatas->text = elem->getText();

	vector<ATTRCONTENT> list_attr = elem->getAttribute();

	// 属性設定
	if(!list_attr.empty()) //
	{
		vector<ATTRCONTENT>::iterator it = list_attr.begin();  
		while( it != list_attr.end() ) 
		{			
			pDatas->setAttribute(it->AttrName,it->AttrVal);
			++it;                 
		}
	}

	// 属性を要素に追加
	if(CRLF)pParentPtr->appendChild(pDoc->createTextNode(L"\n"));//
	pParentPtr->appendChild(pDatas);

	return pDatas;


}

//
// XMLデータを返す
//
void XmlData::getXML(BSTR *bstrXml)
{
	pDoc->get_xml(bstrXml);
}

