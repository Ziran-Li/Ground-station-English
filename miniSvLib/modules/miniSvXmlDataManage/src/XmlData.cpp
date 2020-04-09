#include "stdafx.h"
#include <tchar.h>
#include "XmlData.h"
//
//  �R���X�g���N�^
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
	// memo: XML_VERSION��	(_T("version='1.0' encoding='UTF-8'"))�̏ꍇ
	//       XML�� encoding="UTF-8"���o�͂���Ȃ��B
	//       MSXML�̃f�t�H���g��UTF-8�ł�����Ȃ��B

	//���[�g
    IElementPtr pRoot = pDoc->createElement(ROOT_TAG);

	//���[�g��ǉ�
    pDoc->appendChild(pRoot);
	
}

//
// ���b�Z�[�W�^�O�ǉ�
//
void XmlData::addMessageTag(class MessageTag *msg)
{
	//���[�g�|�C���^
	IElementPtr pRoot = pDoc->getElementsByTagName(ROOT_TAG)->Getitem(0);

	//message�^�O��ǉ�
	IElementPtr pMsg = addChild(pRoot, msg);

	//���f
	msg->setElementPtr(pMsg);

}

//
// �v�f����p����Element�ǉ�
//
IElementPtr XmlData::addChild(IElementPtr pParentPtr, LPCTSTR elemName)
{

    // �v�f�쐬
    IElementPtr pElem = pDoc->createElement(elemName);
	// �ǉ�
	pParentPtr->appendChild(pElem);
	if(CRLF)pParentPtr->appendChild(pDoc->createTextNode(L"\n"));//

	return pElem;

}

//
// ElementData�N���X��p����Element�ǉ�
//
IElementPtr XmlData::addChild(IElementPtr pParentPtr, ElementData *elem)
{
	 
    // �v�f�쐬
    IElementPtr pDatas = pDoc->createElement(elem->getName());
	if(elem->getText() != NULL)pDatas->text = elem->getText();

	vector<ATTRCONTENT> list_attr = elem->getAttribute();

	// �����ݒ�
	if(!list_attr.empty()) //
	{
		vector<ATTRCONTENT>::iterator it = list_attr.begin();  
		while( it != list_attr.end() ) 
		{			
			pDatas->setAttribute(it->AttrName,it->AttrVal);
			++it;                 
		}
	}

	// ������v�f�ɒǉ�
	if(CRLF)pParentPtr->appendChild(pDoc->createTextNode(L"\n"));//
	pParentPtr->appendChild(pDatas);

	return pDatas;


}

//
// XML�f�[�^��Ԃ�
//
void XmlData::getXML(BSTR *bstrXml)
{
	pDoc->get_xml(bstrXml);
}

