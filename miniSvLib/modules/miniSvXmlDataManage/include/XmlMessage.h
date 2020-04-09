#ifndef XMLMESSGE_H
#define XMLMESSGE_H

#include <vector> 
#import  <msxml6.dll>  // MSXMLライブラリのインポート  
using namespace std; 

#define ROOT_KEY		(L"root")
#define DEFAULT_CYCLE	(50)	 //属性：通信周期デフォルト値

typedef MSXML2::IXMLDOMElementPtr	IElementPtr;
typedef MSXML2::IXMLDOMNodeListPtr	INodeListPtr;
typedef MSXML2::IXMLDOMNamedNodeMapPtr INamedNodeMapPtr;
typedef MSXML2::IXMLDOMNodePtr      INodePtr;

class XmlMessage
{
	
public:

	enum MsgType{ msgSet, msgGet, msgClose, msgMainLink ,msgTypeErr, }; //メッセージタイプID
	enum arrangeMode{ replyGetMsg, modifiedSet, modifiedGet, replyMainLinkMsg, modeErr, }; //データアレンジモード

public:

	XmlMessage (){	
	}

	XmlMessage (BSTR quary, HRESULT *hr)
	{
		*hr       = loadXmlMsg(quary);
		dataID   = 0;
	}

	XmlMessage (BSTR quary, unsigned int ID, HRESULT *hr)
	{
		*hr       = loadXmlMsg(quary);
		dataID   = ID;
	}

	~XmlMessage() {
	}

	MSXML2::IXMLDOMDocument2Ptr pDoc;	//DOMドキュメントポインタ

	int getCycle(int Idx); 
	int getMsgType(int msgNo);			//メッセージタイプの取得
	int setMsgType(int msgNo, int msg_type);//メッセージタイプの設定
	_bstr_t getMsgAttA(int msgNo);	//属性aの取得

	VARIANT_BOOL hasSameName(BSTR name, unsigned int *listIdx);

	unsigned int getDataID();
	void setDataID(unsigned int ID){dataID = ID;};

    vector<_bstr_t>list_name;			//nameリスト

	void removeDisenable();

private:
	unsigned int dataID;				//クライアントID
    vector<int>list_msg;				//メッセージタイプIDリスト
    vector<int>list_cycle;				//周期リスト
    vector<_bstr_t>list_a;				//a属性リスト

	BSTR getName(int nameNo);	//未使用

	HRESULT loadXmlMsg(BSTR quary);		//メッセージのロード
	VARIANT_BOOL removeEachDisenable(const MSXML2::IXMLDOMNodePtr pParent);
};



#endif /**/
