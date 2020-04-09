#ifndef XMLDATAS_H
#define XMLDATAS_H

#include <vector> 
#import  <msxml6.dll>  // MSXMLライブラリのインポート  
using namespace std; 

// typedef
typedef MSXML2::IXMLDOMElementPtr	IElementPtr;
typedef MSXML2::IXMLDOMProcessingInstructionPtr IProcessingInstructionPtr;

#define XML_VERSION	(L"version='1.0' encoding='UTF-8'")
#define ROOT_TAG	(L"root")
#define MESSAGE_KY  (L"message") 
#define TYPE_KY     (L"type") 
#define CRLF        (0) //=0；要素間に改行なし、=1;改行あり

//
//　属性クラス
//
struct ATTRCONTENT {
  LPCTSTR AttrName;          //属性名
  LPCTSTR AttrVal;           //value
};
typedef struct ATTRCONTENT AttrContent;

//
//　要素クラス
//
class ElementData
{
private:

	LPCTSTR name;			//要素名
	LPCTSTR text;			//value

    vector<ATTRCONTENT>list_attr; //属性リスト（属性名,value)

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

	void setAttribute(LPCTSTR name , LPCTSTR val) //属性設定
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
//　メッセージタグ
//
class MessageTag  : public ElementData
{
public:
	MessageTag ()
	{
		
		setName(MESSAGE_KY); //タグの名前を設定
		messageType = NULL;
		pMsg        = NULL;

	}
	MessageTag (LPCTSTR type)
	{
		setName(MESSAGE_KY);//タグの名前を設定		
		setAttribute(TYPE_KY, type);//属性を設定
		messageType = type;
		pMsg        = NULL;
	}	
	~MessageTag (){}

	void		setElementPtr(IElementPtr ptr)	{pMsg = ptr;}
	IElementPtr getMsgPtr()			{return pMsg;}
	LPCTSTR		getMessageType()	{return messageType;}

private:
	LPCTSTR     messageType;	//メッセージタイプ	
    IElementPtr pMsg;

};

//
//　XmlDataクラス
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
