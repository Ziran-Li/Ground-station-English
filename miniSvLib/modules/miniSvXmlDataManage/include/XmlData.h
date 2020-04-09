#ifndef XMLDATAS_H
#define XMLDATAS_H

#include <vector> 
#import  <msxml6.dll>  // MSXML���C�u�����̃C���|�[�g  
using namespace std; 

// typedef
typedef MSXML2::IXMLDOMElementPtr	IElementPtr;
typedef MSXML2::IXMLDOMProcessingInstructionPtr IProcessingInstructionPtr;

#define XML_VERSION	(L"version='1.0' encoding='UTF-8'")
#define ROOT_TAG	(L"root")
#define MESSAGE_KY  (L"message") 
#define TYPE_KY     (L"type") 
#define CRLF        (0) //=0�G�v�f�Ԃɉ��s�Ȃ��A=1;���s����

//
//�@�����N���X
//
struct ATTRCONTENT {
  LPCTSTR AttrName;          //������
  LPCTSTR AttrVal;           //value
};
typedef struct ATTRCONTENT AttrContent;

//
//�@�v�f�N���X
//
class ElementData
{
private:

	LPCTSTR name;			//�v�f��
	LPCTSTR text;			//value

    vector<ATTRCONTENT>list_attr; //�������X�g�i������,value)

public:

	ElementData()
	{
		name = NULL;
		text = NULL;
	}
	ElementData(LPCTSTR elemName, LPCTSTR elemText)
	{
		name = elemName;
		text = elemText;
	}
	~ElementData()
	{
	}

	void setAttribute(LPCTSTR name , LPCTSTR val) //�����ݒ�
	{
		AttrContent temp;
		temp.AttrName = name;
		temp.AttrVal = val;

		list_attr.push_back(temp);

	}

	void setName(LPCTSTR elemName) { name = elemName;};	
	LPCTSTR getName() {return name;};	
	LPCTSTR getText() {return text;};
	vector<ATTRCONTENT> getAttribute(){	return list_attr;}
};

//
//�@���b�Z�[�W�^�O
//
class MessageTag  : public ElementData
{
public:
	MessageTag ()
	{
		
		setName(MESSAGE_KY); //�^�O�̖��O��ݒ�
		messageType = NULL;
		pMsg        = NULL;

	}
	MessageTag (LPCTSTR type)
	{
		setName(MESSAGE_KY);//�^�O�̖��O��ݒ�		
		setAttribute(TYPE_KY, type);//������ݒ�
		messageType = type;
		pMsg        = NULL;
	}	
	~MessageTag (){}

	void		setElementPtr(IElementPtr ptr)	{pMsg = ptr;}
	IElementPtr getMsgPtr()			{return pMsg;}
	LPCTSTR		getMessageType()	{return messageType;}

private:
	LPCTSTR     messageType;	//���b�Z�[�W�^�C�v	
    IElementPtr pMsg;

};

//
//�@XmlData�N���X
//
class XmlData
{
private:

	MSXML2::IXMLDOMDocument2Ptr pDoc;

public:
	XmlData();
	XmlData(HRESULT *hr);

    void addMessageTag(class MessageTag *msg);

	IElementPtr addChild(IElementPtr pParentPtr, LPCTSTR elemName);
	IElementPtr addChild(IElementPtr pParentPtr, ElementData *elem);

	void getXML(BSTR *bstrXml);

	IElementPtr getRootPtr() { return pDoc->getElementsByTagName(ROOT_TAG)->Getitem(0); }
		
};

#endif /**/
