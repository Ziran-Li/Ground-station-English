#include "stdafx.h"
#include <tchar.h>
#include <vector> 
#import  <msxml6.dll>  // MSXML���C�u�����̃C���|�[�g  
using namespace std; 
using namespace std::tr1;

#include "XmlMessage.h"
#include "XmlMsgAttribute.h"

//
// XML�`���̃X�g�����O�����[�h����
//��һ��xml�ļ�
HRESULT XmlMessage::loadXmlMsg(BSTR quary)
{
	VARIANT_BOOL isSuccess = VARIANT_FALSE;

	//Create the XmlDocument
	// ����DOMDocument����
	HRESULT hr  = pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument60) );
    if (FAILED(hr)) 
    {
		printf("Failed to instantiate an XML DOM.\n");
        return S_FALSE; 
    }
	pDoc->async = VARIANT_FALSE;	

	//Load the XML string
	//����XML���ַ���
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
				//TODO: cycle�����l�Ȃ̂����菈���v��
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
// ���b�Z�[�W�^�C�v��ݒ�
//
int XmlMessage::setMsgType(int msgNo, int msg_type)
{
	int size = list_msg.size();

	if( size == 0 || size <= msgNo )
	{
		printf("��2�������͈͂𒴂��Ă��܂��B�BFILE = %s , LINE = %d\n",__FILE__,__LINE__);
		return S_FALSE;
	}
	else
	{
		list_msg.at(msgNo) = msg_type;
		return S_OK;
	}

}

//
// ���b�Z�[�W�^�C�v��Ԃ�
//
int XmlMessage::getMsgType(int msgNo)
{
	int size = list_msg.size();

	if( size == 0 || size <= msgNo )
	{
		printf("message����type���s�K�؂ł��B ");
		printf("message�������`�łȂ��\������܂��B\n");
		return msgTypeErr;
	}
	else
	{
		return list_msg.at(msgNo);
	}

}

//
// a������Ԃ�
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
// ID��Ԃ�
//
unsigned int XmlMessage::getDataID()
{
	return dataID;
}

//
// name������Ԃ�
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
// cycyle������Ԃ�
//
int XmlMessage::getCycle(int Idx)
{
	int size = list_cycle.size();

	if( size == 0 || size <= Idx)
	{
		return DEFAULT_CYCLE; //TODO:�f�t�H���g50Hz��Ԃ��̂ŗǂ����H
	}
	else
	{
		return list_cycle.at(Idx);
	}

}

//
// name�������������̂����邩����
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

	int remove = 0;	//�폜��
	int num = -1;

	_bstr_t name = pParent->parentNode->GetnodeName(); //�e�m�[�h�̖��O

	if(pParent->hasChildNodes())
	{
		INodeListPtr pNodes = pParent->GetchildNodes();

		num = pNodes->Getlength();
		for(int iNode = 0 ; iNode < num; iNode++)	//�q�m�[�h�̐��������[�v
		{
			//pParent�̐e��message�̏ꍇ�A�^�O���̂�remove���Ȃ��Ŏq�m�[�h��remove
			INamedNodeMapPtr attPtrP = pParent->Getattributes();
			if (_tcscmp(name, L"message") == 0 && !getAttSendValue(attPtrP))
			{
				IElementPtr pChild = pNodes->Getitem(iNode - remove);  //�q�m�[�h���폜
				pParent->removeChild(pChild);
				remove++;
			}
			else{
				IElementPtr pChild = pNodes->Getitem(iNode - remove);

				if (pChild != NULL)
				{
					INamedNodeMapPtr attPtr = pChild->Getattributes();
					if (!getAttSendValue(attPtr)) //���� send��disabled�Ȃ��
					{						
						//remove����
						IElementPtr x = pParent->removeChild(pChild);
						remove++;
					}
					else//enabled�Ȃ��
					{
						BOOL flg = FALSE;
						if (pChild->GetchildNodes()->Getitem(0)){
							flg = TRUE;
						}

						removeEachDisenable(pChild);	//���̉��ʃm�[�h�𒲍�

						if (!pChild->GetchildNodes()->Getitem(0) && flg == TRUE){  //pChild�̎q�m�[�h��1��������remove����O�͎q�m�[�h���������ꍇ
							pParent->removeChild(pChild);
							remove++;
						}
					}
				}
			}
		}
	}
	
	if (_tcscmp(name, L"message") == 0) //message�^�O�̎q�m�[�h�̏ꍇ
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pParent->Getattributes();
		if (!getAttSendValue(attNodePtr))  //disable
		{
			//���̂܂܁@send�����͏����Ȃ�
		}
		else{
			//�q�m�[�h�����͂��������S��remove���ꂽ�ꍇ�[��disable
			if (remove > 0 && !pParent->GetchildNodes()->Getitem(0))
			{
				if (attNodePtr->length >0)	//����������H
				{
					MSXML2::IXMLDOMNodePtr pSend = attNodePtr->getNamedItem(L"send");
					if (pSend != NULL) // ����send�͂���H
					{
						pSend->put_text(L"disabled");
					}
				}
				//�����̂ő�����ǉ�����
				MSXML2::IXMLDOMAttributePtr att = pDoc->createAttribute(L"send");
				att->put_text(L"disabled");
				attNodePtr->setNamedItem(att);
			}
			//�q�m�[�h��������Ȃ��ꍇ�[��enable
			//���̂܂�
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

	IElementPtr  pRoot   = pDoc->documentElement;//set���b�Z�[�W�̃��[�g�|�C���^

	//Debug
	//printf("\n remove�O\n");
	//printf(pDoc->Getxml());

	if(pRoot->hasChildNodes())	//���[�g�����Ɏq�m�[�h������H
	{
		INodeListPtr pMessage = pRoot->GetchildNodes();
		int msgNum = pMessage->Getlength();	

		for(int iMsg = 0; iMsg < msgNum; iMsg++)		    //message�̐��������[�v
		{
			IElementPtr pMsgElem = pMessage->Getitem(iMsg);	//message�v�f

			if(pMsgElem->hasChildNodes())				    //message�����Ɏq�m�[�h������H
			{
				INodeListPtr pNodes = pMsgElem->GetchildNodes();
				for(int iNode = 0; iNode < pNodes->Getlength(); iNode++)//�m�[�h�̐��������[�v
				{
					INodePtr pChild = pNodes->Getitem(iNode);//�q�m�[�h
					removeEachDisenable(pChild);
				}
			}	
		}

		//Debug
		//printf("\n remove��\n");
		//printf(pDoc->Getxml());
	}
}
