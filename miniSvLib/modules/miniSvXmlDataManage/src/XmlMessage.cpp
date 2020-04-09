#include "stdafx.h"
#include <tchar.h>
#include <vector> 
#import  <msxml6.dll>  // MSXML儔僀僽儔儕偺僀儞億乕僩  
using namespace std; 
using namespace std::tr1;

#include "XmlMessage.h"
#include "XmlMsgAttribute.h"

//
// XML宍幃偺僗僩儕儞僌傪儘乕僪偡傞
//打开一个xml文件
HRESULT XmlMessage::loadXmlMsg(BSTR quary)
{
	VARIANT_BOOL isSuccess = VARIANT_FALSE;

	//Create the XmlDocument
	// 创建DOMDocument对象
	HRESULT hr  = pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument60) );
    if (FAILED(hr)) 
    {
		printf("Failed to instantiate an XML DOM.\n");
        return S_FALSE; 
    }
	pDoc->async = VARIANT_FALSE;	

	//Load the XML string
	//加载XML文字符串
	HRESULT hr1  = pDoc->raw_loadXML(quary, &isSuccess);
    if (FAILED(hr1)) 
	{
		MSXML2::IXMLDOMParseErrorPtr pErr;
		BSTR bstrErr = NULL;
		pDoc->get_parseError(&pErr);
		pErr->get_reason(&bstrErr);
		printf("XML DOM loaded errror \n",bstrErr);
		::SysFreeString(bstrErr);

		return S_FALSE; 
	} 

	//Get the message node
	 
	INodeListPtr pMsgList = pDoc->getElementsByTagName("message");
	IElementPtr  pMsg;

    int numMessage = pMsgList->Getlength(); //number of the messages
	MsgType msg = msgTypeErr;

    for(int i=0; i< numMessage; i++)
	{
		pMsg =  pMsgList->Getitem(i);	    //pointer of the message node

		// Check the parent node is root
		if(_tcscmp(pMsg->GetparentNode()->GetnodeName() , ROOT_KEY) == 0)
		{
			
			_variant_t att_type = pMsg->getAttribute("type");//get the type attribute

			if(_tcscmp((_bstr_t)att_type, L"set") == 0)
			{
				msg = msgSet;
			}
			else if(_tcscmp((_bstr_t)att_type, L"get") == 0)
			{
				msg = msgGet;
			}
			else if (_tcscmp((_bstr_t)att_type, L"close") == 0)
			{
				msg = msgClose;
			}
			else if (_tcscmp((_bstr_t)att_type, L"mainlink send") == 0)
			{
				msg = msgMainLink;
			}
			else
			{
				msg = msgTypeErr;
			}
			
			list_msg.push_back(msg);  //set the list of list_msg

			_variant_t cycle = pMsg->getAttribute("cycle");//get the type attribute
			if(cycle.vt == VT_NULL) 
			{
				list_cycle.push_back(DEFAULT_CYCLE);  //set the list of list_cycle	
			}
			else
			{
				//TODO: cycle偑悢抣側偺偐敾掕張棟梫傞
				cycle.ChangeType(VT_I4);
				list_cycle.push_back(cycle.intVal);  //set the list of list_cycle	
			}

			_variant_t name = pMsg->getAttribute("name");//get the type attribute
			list_name.push_back(name.bstrVal);  //set the list of list_name

			_variant_t atta = pMsg->getAttribute("a");//get the type attribute
			list_a.push_back(atta.bstrVal);  //set the list of list_a
		}
	}

	return S_OK;
}


//
// 儊僢僙乕僕僞僀僾傪愝掕
//
int XmlMessage::setMsgType(int msgNo, int msg_type)
{
	int size = list_msg.size();

	if( size == 0 || size <= msgNo )
	{
		printf("戞2堷悢偑斖埻傪挻偊偰偄傑偡丅丅FILE = %s , LINE = %d\n",__FILE__,__LINE__);
		return S_FALSE;
	}
	else
	{
		list_msg.at(msgNo) = msg_type;
		return S_OK;
	}

}

//
// 儊僢僙乕僕僞僀僾傪曉偡
//
int XmlMessage::getMsgType(int msgNo)
{
	int size = list_msg.size();

	if( size == 0 || size <= msgNo )
	{
		printf("message暥偺type偑晄揔愗偱偡丅 ");
		printf("message暥偑惍宍偱側偄壜擻傕偁傝傑偡丅\n");
		return msgTypeErr;
	}
	else
	{
		return list_msg.at(msgNo);
	}

}

//
// a懏惈傪曉偡
//
_bstr_t XmlMessage::getMsgAttA(int msgNo)
{

	int size = list_a.size();

	if( size == 0 || size <= msgNo )
	{
		return L"";
	}
	else
	{
		return list_a.at(msgNo);
	}

}
//
// ID傪曉偡
//
unsigned int XmlMessage::getDataID()
{
	return dataID;
}

//
// name懏惈傪曉偡
//
BSTR XmlMessage::getName(int Idx)
{
	int size = list_name.size();

	if( size == 0 || size <= Idx)
	{
		return NULL;
	}
	else
	{
		return list_name.at(Idx);
	}

}

//
// cycyle懏惈傪曉偡
//
int XmlMessage::getCycle(int Idx)
{
	int size = list_cycle.size();

	if( size == 0 || size <= Idx)
	{
		return DEFAULT_CYCLE; //TODO:僨僼僅儖僩50Hz傪曉偡偺偱椙偄偐丠
	}
	else
	{
		return list_cycle.at(Idx);
	}

}

//
// name懏惈偑摨偠傕偺偑偁傞偐敾掕
//
VARIANT_BOOL XmlMessage::hasSameName(BSTR name, unsigned int *listIdx)
{

	int size = list_name.size();

	if(!list_name.empty()) //if list has elements then do
	{
		vector<_bstr_t>::iterator it = list_name.begin(); 

		for(int i=0; i < size; i++)
		{
			if(SysStringLen(list_name.at(i))>0 && SysStringLen(name)>0)
			{
				if(wcscmp(list_name.at(i), name)==0)
				{
					*listIdx = i;
					return VARIANT_TRUE;	//TRUE
				}
			}
			else if(SysStringLen(list_name.at(i))==0 && SysStringLen(name)==0)
			{
				*listIdx = i;
				return VARIANT_TRUE;	//TRUE

			}
			++it;
		}
	}
	return VARIANT_FALSE;	
}

//
// 
//

VARIANT_BOOL XmlMessage::removeEachDisenable(const MSXML2::IXMLDOMNodePtr pParent)
{

	int remove = 0;	//嶍彍夞悢
	int num = -1;

	_bstr_t name = pParent->parentNode->GetnodeName(); //恊僲乕僪偺柤慜

	if(pParent->hasChildNodes())
	{
		INodeListPtr pNodes = pParent->GetchildNodes();

		num = pNodes->Getlength();
		for(int iNode = 0 ; iNode < num; iNode++)	//巕僲乕僪偺悢偩偗儖乕僾
		{
			//pParent偺恊偑message偺応崌丄僞僌帺懱偼remove偟側偄偱巕僲乕僪傪remove
			INamedNodeMapPtr attPtrP = pParent->Getattributes();
			if (_tcscmp(name, L"message") == 0 && !getAttSendValue(attPtrP))
			{
				IElementPtr pChild = pNodes->Getitem(iNode - remove);  //巕僲乕僪傪嶍彍
				pParent->removeChild(pChild);
				remove++;
			}
			else{
				IElementPtr pChild = pNodes->Getitem(iNode - remove);

				if (pChild != NULL)
				{
					INamedNodeMapPtr attPtr = pChild->Getattributes();
					if (!getAttSendValue(attPtr)) //懏惈 send偑disabled側傜偽
					{						
						//remove偡傞
						IElementPtr x = pParent->removeChild(pChild);
						remove++;
					}
					else//enabled側傜偽
					{
						BOOL flg = FALSE;
						if (pChild->GetchildNodes()->Getitem(0)){
							flg = TRUE;
						}

						removeEachDisenable(pChild);	//師偺壓埵僲乕僪傪挷嵏

						if (!pChild->GetchildNodes()->Getitem(0) && flg == TRUE){  //pChild偺巕僲乕僪偑1偮傕柍偄仌remove偡傞慜偼巕僲乕僪偑偁偭偨応崌
							pParent->removeChild(pChild);
							remove++;
						}
					}
				}
			}
		}
	}
	
	if (_tcscmp(name, L"message") == 0) //message僞僌偺巕僲乕僪偺応崌
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pParent->Getattributes();
		if (!getAttSendValue(attNodePtr))  //disable
		{
			//偦偺傑傑丂send懏惈偼徚偝側偄
		}
		else{
			//巕僲乕僪偑尦偼偁偭偨偑慡偰remove偝傟偨応崌乕亜disable
			if (remove > 0 && !pParent->GetchildNodes()->Getitem(0))
			{
				if (attNodePtr->length >0)	//懏惈偑偁傞丠
				{
					MSXML2::IXMLDOMNodePtr pSend = attNodePtr->getNamedItem(L"send");
					if (pSend != NULL) // 懏惈send偼偁傞丠
					{
						pSend->put_text(L"disabled");
					}
				}
				//柍偄偺偱懏惈傪捛壛偡傞
				MSXML2::IXMLDOMAttributePtr att = pDoc->createAttribute(L"send");
				att->put_text(L"disabled");
				attNodePtr->setNamedItem(att);
			}
			//巕僲乕僪偑尦偐傜側偄応崌乕亜enable
			//偦偺傑傑
		}
	}
	else
	{
		INamedNodeMapPtr attPtr = pParent->Getattributes();
		attPtr->removeNamedItem("send");
	}


	return VARIANT_TRUE; // not found
	
}

//
//  
//
void XmlMessage::removeDisenable()
{

	IElementPtr  pRoot   = pDoc->documentElement;//set儊僢僙乕僕偺儖乕僩億僀儞僞

	//Debug
	//printf("\n remove慜\n");
	//printf(pDoc->Getxml());

	if(pRoot->hasChildNodes())	//儖乕僩捈壓偵巕僲乕僪偑偁傞丠
	{
		INodeListPtr pMessage = pRoot->GetchildNodes();
		int msgNum = pMessage->Getlength();	

		for(int iMsg = 0; iMsg < msgNum; iMsg++)		    //message偺悢偩偗儖乕僾
		{
			IElementPtr pMsgElem = pMessage->Getitem(iMsg);	//message梫慺

			if(pMsgElem->hasChildNodes())				    //message捈壓偵巕僲乕僪偑偁傞丠
			{
				INodeListPtr pNodes = pMsgElem->GetchildNodes();
				for(int iNode = 0; iNode < pNodes->Getlength(); iNode++)//僲乕僪偺悢偩偗儖乕僾
				{
					INodePtr pChild = pNodes->Getitem(iNode);//巕僲乕僪
					removeEachDisenable(pChild);
				}
			}	
		}

		//Debug
		//printf("\n remove屻\n");
		//printf(pDoc->Getxml());
	}
}
