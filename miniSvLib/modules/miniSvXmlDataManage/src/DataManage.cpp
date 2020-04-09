#include "stdafx.h"
#include <tchar.h>
#include <vector> 

#include "DataManage.h"

//
// ����ID�̂��̂����邩�`�F�b�N
//
//  �@dataID   �FID
//  �@list     �FdataDB�I�u�W�F�N�g�̃|�C���^
//  �@listIdx  : ����ID�����v�f�̃C���f�b�N�X
//
//    �߂�l    : =VARIANT_TRUE; ����ID�L��
//              : =VARIANT_FALSE;����ID����
//
VARIANT_BOOL dataManage::hasSameID(int dataID, class dataDB list, unsigned int *listIdx)
{
	int size = list.data.size(); //���X�g�T�C�Y

	if(!list.data.empty())		//���X�g�͋�ł͂Ȃ��H
	{

		vector<XmlMessage>::iterator it = list.data.begin(); 

		for(int i=0; i < size; i++)			//���X�g�̍\���v�f�̌������[�v
		{
			if(it->getDataID() == dataID)	//�����N���C�A���gID�̂��̂�����H
			{
				*listIdx = i;				//�v�f�C���f�b�N�X����

				return VARIANT_TRUE;	
			}
			++it;
		}
	}
		
	return VARIANT_FALSE;	

}

// 
// ����name�����̂��̂����邩�`�F�b�N
//
//  �@msgName �Fname
//  �@archive �Fget���b�Z�[�W���X�g
//  �@listIdx �F����name�����v�f�̃C���f�b�N�X
//
//    �߂�l    : =VARIANT_TRUE; ����name�����L��
//              : =VARIANT_FALSE;����name��������
//
VARIANT_BOOL dataManage::hasSameName(BSTR msgName, XmlMessage *archive, unsigned int *listIdx)
{

	VARIANT_BOOL bMatch = VARIANT_FALSE;
	
	return archive->hasSameName(msgName, listIdx);	
}

// 
//�@�f�[�^�ۑ�
//
//  �@xml      : XmlMessage�I�u�W�F�N�g�̃|�C���^
//	  dataID   : ID
//
void dataManage::saveData(XmlMessage *xml, unsigned int dataID)
{
	unsigned int listIdx;

	int msgID = 0;		//TODO�Fmessage�ɂ�1��message�݂̂Ɍ���B��Ŋg��
	int type  = xml->getMsgType(msgID);	//TODO:�P��xml�f�[�^�̒���set��get�̍��݂͂Ȃ��Ƃ̑z��
								//���̑z��ŗǂ����m�F�K�v�B�܂��AxmlMessage��load���ɍ��ݎ��̃G���[�����K�v���H�����܂ł��H
	
	xml->setDataID(dataID);

	switch(type)
	{

	case XmlMessage::msgSet:	// set���b�Z�[�W

		if(hasSameID(dataID,setList, &listIdx)) //����ID������H
		{
			vector<XmlMessage>::iterator it = setList.data.begin();
			for (unsigned int i = 0; i<listIdx; i++) it++;
			_bstr_t name = xml->list_name.at(msgID);
			XmlMessage archive = *it;

			if (hasSameName(name, &archive, &listIdx))//����name����������H
			{
				//��������C��  
				BSTR xmlMsg = xml->pDoc->Getxml();
				addSetMsgData(dataID, &xmlMsg);
				::SysFreeString(xmlMsg);
			}
			else
			{
				//�Ȃ�������ǉ�
				setList.data.push_back(*xml); 
			}
			
		}
		else
		{
			//������Βǉ�
			setList.data.push_back(*xml);
		}
		break;

	case XmlMessage::msgGet:	// get���b�Z�[�W

		if(hasSameID(dataID, getList, &listIdx)) //�����N���C�A���gID������H
		{						
			//name�������������̂�����H
			vector<XmlMessage>::iterator it = getList.data.begin();
			for (unsigned int i = 0; i<listIdx; i++) it++;
			_bstr_t name = xml->list_name.at(msgID); //TODO:�{�����b�Z�[�W�������[�v���K�v
			XmlMessage archive = *it;

			if (hasSameName(name, &archive, &listIdx))//����name����������H
			{
				int tmp = archive.getMsgType(msgID);
				if(archive.getMsgType(msgID) == XmlMessage::msgClose)
				{
					//msgType�O��l��close�Ȃ�΁A�f�[�^���폜���Ă���msgType=get�Œǉ�����
					it = getList.data.erase(it);		// it�̈ʒu�̗v�f���폜
					getList.data.insert(it, *xml );     // it�̈ʒu�ɒǉ�
				}
				else
				{
					//msgType�O��l��get�Ȃ�΃��b�Z�[�W���C��
					BSTR xmlMsg = xml->pDoc->Getxml();
					addSetMsgData(dataID, &xmlMsg);
					::SysFreeString(xmlMsg);
				}
			}
			else
			{
				//�Ȃ�������ǉ�
				getList.data.push_back(*xml); 
			}
		}
		else //
		{
			getList.data.push_back(*xml);								
		}
		break;

	case XmlMessage::msgClose:	//close���b�Z�[�W

		if(hasSameID(dataID, getList, &listIdx)) //�����N���C�A���gID������H
		{

			vector<XmlMessage>::iterator it = getList.data.begin();
			for(unsigned int i =0; i<listIdx; i++) it++;
			_bstr_t name   = xml->list_name.at(msgID);
			XmlMessage archive = *it;

			if(hasSameName(name, &archive ,&listIdx))//����name����������H
			{
				MSXML2::IXMLDOMNodePtr pMsg =archive.pDoc->selectSingleNode(L"/root/message");

				MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pMsg->Getattributes();
				MSXML2::IXMLDOMNodePtr pType = attNodePtr->getNamedItem("type");

				pType->put_text(L"close");			//message�^�C�v��close�ɕύX
				it->setMsgType(msgID, XmlMessage::msgClose);
			}	
		}
	
		break;

	case XmlMessage::msgMainLink:	// main link ���b�Z�[�W

		if (hasSameID(dataID, mainLinkList, &listIdx)) //�����N���C�A���gID������H
		{
			//����ID�̃f�[�^���폜���Ă���ǉ�����
			vector<XmlMessage>::iterator it = mainLinkList.data.begin();
			for(unsigned int i =0; i<listIdx; i++) it++;
			it = mainLinkList.data.erase(it);		// it�̈ʒu�̗v�f���폜
			mainLinkList.data.insert(it, *xml);     // it�̈ʒu�ɒǉ�
		}
		else
		{
			mainLinkList.data.push_back(*xml);
		}
		break;

	case XmlMessage::msgTypeErr:

		default:
		break;
	}
	
}
// 
// get���b�Z�[�W�Ɋ�Â��ăf�[�^�擾
//
//   pParent    �F�v�f�|�C���^
//   str_query�@�Fget����
//   pLeaf      �F���O����v�����m�[�h�|�C���^
//
//
//    �߂�l    : =VARIANT_TRUE; �f�[�^�擾����
//              : =VARIANT_FALSE; get���b�Z�[�W�ɑΉ������f�[�^�Ȃ�
//
VARIANT_BOOL dataManage::getSetData(MSXML2::IXMLDOMElementPtr pParent, struct getMsg *str_query, MSXML2::IXMLDOMElementPtr *pLeaf)
{

	vector<_bstr_t> path_parent = str_query->path;
	int path_final_idx       = str_query->path.size() - 1 ;
	_bstr_t  name;

	int idx = str_query->path_idx;

	MSXML2::IXMLDOMNodeListPtr pNodes = NULL;	

	if(idx < path_final_idx)	//�q�m�[�h������H
	{
		idx++; //vpath�̃C���f�b�N�X��i�߂�

		MSXML2::IXMLDOMNodeListPtr pNodes = pParent->GetchildNodes();
			
		long ll1 = pNodes->Getlength();
		for(int iNode = 0 ; iNode < pNodes->Getlength(); iNode++)	//�q�m�[�h�̐��������[�v
		{
			name = path_parent.at(idx);  

			MSXML2::IXMLDOMElementPtr pChild = pNodes->Getitem(iNode);//�q�m�[�h
			if(pChild != NULL)
			{
				_bstr_t temp = pChild->GetnodeName();
				if(_tcscmp(pChild->GetnodeName(),name) ==0)	//�v�f������v�H
				{
					*pLeaf = pChild;
					str_query->path_idx = idx;
					if(getSetData(pChild, str_query, pLeaf)) 
					{
						return VARIANT_TRUE;//
					}
				}
			}
		}
		//��v������̂������̂Œǉ� TODO:����m�F�܂�

		if(str_query->mode == XmlMessage::modifiedSet)
		{
		
			MSXML2::IXMLDOMDocument2Ptr  pDoc =  pParent->GetownerDocument();

			name = path_parent.at(idx);  //�ǉ�
		    IElementPtr pElem = pDoc->createElement(name);
			pParent->appendChild(pElem);

			IElementPtr pPElem = pElem;

			while(++idx <= path_final_idx) 
			{
				name = path_parent.at(idx);  
			    pElem = pParent->GetownerDocument()->createElement(name);
				pPElem->appendChild(pElem);
				pPElem = pElem;
			}
	
			MSXML2::IXMLDOMNamedNodeMapPtr map = str_query->attNodePtr; //����
			if(map!=NULL)
			{						
				for(long i =0; i <map->Getlength(); i++)	//�����̐������ǉ�
				{				
					MSXML2::IXMLDOMNodePtr  pNode = map->Getitem(i);
					pElem->setAttribute(pNode->GetnodeName(), pNode->GetnodeValue());
				}
			}
			*pLeaf = pElem;
			return VARIANT_TRUE;
		}
	}
	else  //�ŉ��w�̃m�[�h
 	{

		//path�Ɋ�Â��ăf�[�^�擾		
		name      = path_parent.at(idx);  
		VARIANT_BOOL bMatch = TRUE;
		if(_tcscmp(pParent->GetnodeName(),name) ==0) //�v�f������v�H
		{
			//����a�̈�v���m�F
			_bstr_t attA1, attA2;
			VARIANT_BOOL bool1, bool2;
			MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr1, attNodePtr2;
			attNodePtr1 = str_query->attNodePtr;
			attNodePtr2 = pParent->Getattributes();

/*
			attNodePtr = str_query->attNodePtr;
			bool1 = getAttA(attNodePtr,&attA1);		//����a�l�擾�iget�v��xml)
			attNodePtr = pParent->Getattributes();
			bool2 = getAttA(attNodePtr,&attA2);		//����a�l�擾�idb�j

			if( bool1 != bool2) return VARIANT_FALSE; //��������
			if( attA1 != attA2) return VARIANT_FALSE; //��������
*/


			//�����̈�v���m�F
			if (!matchAttrs(attNodePtr1, attNodePtr2, L"a"))  return VARIANT_FALSE; //��������
			if (!matchAttrs(attNodePtr1, attNodePtr2, L"c"))  return VARIANT_FALSE; //��������
			if (!matchAttrs(attNodePtr1, attNodePtr2, L"tm")) return VARIANT_FALSE; //��������			
			//if (!matchAttrs(attNodePtr1, attNodePtr2, L"t"))  return VARIANT_FALSE; //��������
			//����i�̈�v���m�F
			int idx1, idx2;
			bool1 = getAttIValue(attNodePtr1, &idx1);		//����i�l�擾�iget�v��xml)
			bool2 = getAttIValue(attNodePtr2, &idx2);		//����i�l�擾�idb)

			if( bool1 != bool2) return VARIANT_FALSE; //��������
			if( bool1 == VARIANT_FALSE)  //����i���Ȃ�����
			{
				*pLeaf = pParent;
				return VARIANT_TRUE;
			}
			if( idx1  == idx2 )			//����i����v
			{
				*pLeaf = pParent;
				return VARIANT_TRUE;
			}
		}
	}

	return VARIANT_FALSE; // not found
	
}

// 
// ���b�Z�[�W�̉��
//
//
//    �߂�l    : =VARIANT_TRUE; �f�[�^�擾����
//              : =VARIANT_FALSE; get���b�Z�[�W�ɑΉ������f�[�^�Ȃ�
//
VARIANT_BOOL dataManage::anaMessage(XmlMessage *setData, struct getMsg *str_query, 
									MSXML2::IXMLDOMElementPtr *pLeaf)
{

	int idx            = 0;
	_bstr_t  name;

	vector<_bstr_t> path  = str_query->path;	// get����
	int path_final_idx = path.size() - 1 ;	// �m�[�h�p�X�̃C���f�b�N�X�ő�l


	MSXML2::IXMLDOMDocument2Ptr pXMLDom = setData->pDoc;		//set���b�Z�[�W��document�|�C���^
	MSXML2::IXMLDOMElementPtr  pRoot   = pXMLDom->documentElement;//set���b�Z�[�W�̃��[�g�|�C���^
		
	idx++;

	if(pRoot->hasChildNodes())									//���[�g�����Ɏq�m�[�h������H
	{
		MSXML2::IXMLDOMNodeListPtr pMessage = pRoot->GetchildNodes();
		int msgNum = pMessage->Getlength();	
					
		for(int iMsg = 0; iMsg < msgNum; iMsg++)				//message�̐��������[�v
		{
			MSXML2::IXMLDOMElementPtr
				pMsgElem = pMessage->Getitem(iMsg);				//message�v�f

			name      = path.at(idx);  //get�����v�f��

			if(_tcscmp(pMsgElem->GetnodeName(),name) ==0)		//�v�f������v�H�@
			{
				if(idx < path_final_idx) 
				{
					if(pMsgElem->hasChildNodes())				//�q�m�[�h������H
					{
						str_query->path_idx = idx;
						if (getSetData(pMsgElem, str_query, pLeaf))  //message�^�O����n���iData�^�O���Ή��j
						{
							return VARIANT_TRUE;
						}
					}
					else
					{
						//�Y������f�[�^���Ȃ�
					}
				}
			}
		}
	}

	return VARIANT_FALSE;	//	TODO:message�v�f�̉��ɂ͏��Ȃ��Ƃ��P�̗v�f���K�v
	
}

//
//	text������������
//
void dataManage::putText(MSXML2::IXMLDOMElementPtr pParent, int encoding, struct getMsg str_query, BSTR str_temp)
{
			
	BSTR str   = str_temp;

	if(encoding == dataAttrEncoding::text)
	{
		IXMLDOMNodePtr attT= str_query.attNodePtr->getNamedItem("t"); //�f�[�^�^�y�уx�N�g���v�f���̑���
				
	
		BSTR dataType =NULL;
				
		if (attT == NULL) //T����
		{
			dataType = SysAllocString(L"R");	//�f�t�H���g��ݒ�
		}
		else
		{
			attT->get_text(&dataType);
		}
				
		//����t ID
		Base64Data::convertBintoText(str_temp, dataType, &str);
		::SysFreeString(dataType);
	}
	pParent->put_text(str);	//xml�ɔ��f
	::SysFreeString(str);
}

//
//
//
void dataManage::lastBranch(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath,
struct dataAttribute *dataAttr, int mode)
{
	struct getMsg str_query;
	str_query.path = vpath;
	str_query.path_idx = 0;
	str_query.mode = mode;
	if (pParent!=NULL) str_query.attNodePtr = pParent->Getattributes();
		
	MSXML2::IXMLDOMElementPtr pLeaf   = NULL;
	MSXML2::IXMLDOMElementPtr pDatas  = NULL;

	VARIANT_BOOL vbool  = VARIANT_FALSE;
	VARIANT_BOOL iStart = VARIANT_FALSE;
	XmlMessage setData;
	XmlMessage getData;
	XmlMessage mainLinkData;

	switch (mode)
	{
	case XmlMessage::replyMainLinkMsg: //mainLink send���b�Z�[�W�ɑ΂��鉞��
		for (unsigned int i = 0; i < mainLinkList.data.size(); i++)
		{
			XmlMessage mainLinkData = mainLinkList.data.at(i);	
			if(mainLinkData.getDataID() == dataAttr->ID)
			{
				if (vbool=anaMessage(&mainLinkData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf))
				{
					break; //�f�[�^�擾�����̂�for���𔲂���
				}
			}
		}
		if(vbool) 
		{
			/*mainlink send���b�Z�[�W��t����*/
			MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pLeaf->Getattributes();
			MSXML2::IXMLDOMNodePtr pAttT = attNodePtr->getNamedItem("t");

			if (pAttT != NULL)
			{
				/*request ��t����������������*/
				MSXML2::IXMLDOMNodePtr pReqAttT = str_query.attNodePtr->getNamedItem("t");
				pReqAttT->Puttext(pAttT->Gettext());
			}
			putText(pParent, dataAttr->encoding, str_query, pLeaf->Gettext());
		}

		break;


	case XmlMessage::replyGetMsg: //get���b�Z�[�W�ɑ΂��鉞�� 
			
		if (getAttSendValue(str_query.attNodePtr)) //���� send��enabled�Ȃ��
		{
			for (unsigned int i = 0; i < setList.data.size(); i++)
			{
				XmlMessage setData = setList.data.at(i);	
				if (vbool=anaMessage(&setData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf))
				{
					unsigned int dbID;
					//dbID�������Ă��邩�m�F
					dbID = setDBID(&setData);

					if (dataAttr->ID == dbID) //dbID�������Ă����甲����
					{
						break; //�f�[�^�擾�����̂�for���𔲂���
					}
					else{
						vbool = VARIANT_FALSE;
					}
				}
			}
			if(vbool)
			{
				if(pLeaf->hasChildNodes())
				{
					MSXML2::IXMLDOMNodeListPtr  pNodes =  pLeaf->GetchildNodes();
					for (int iNode = 0; iNode < pNodes->Getlength(); iNode++)		//�q�m�[�h�̐��������[�v
					{
						MSXML2::IXMLDOMElementPtr pChild = pNodes->Getitem(iNode);  //�q
						if(pChild!=NULL)
						{
							//GET���ł͍ŏI�m�[�h�����ApLeaf�ł͎q�m�[�h������ꍇ
							appendDeepChild(pChild, dataAttr, pParent);
						}
						else if(iNode ==0)
						{
							/*t����*/
							MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pLeaf->Getattributes();
							MSXML2::IXMLDOMNodePtr pAttT = attNodePtr->getNamedItem("t");
							if(pAttT!=NULL){
								/*request ��t����������������*/
								MSXML2::IXMLDOMNodePtr pReqAttT = str_query.attNodePtr->getNamedItem("t");
								if(pReqAttT!=NULL)
								{
									pReqAttT->Puttext(pAttT->Gettext());
								}
							}
							putText(pParent, dataAttr->encoding, str_query, pLeaf->Gettext());
							break;
						}
					}
				}
				else
				{
					/*t����*/
					MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pLeaf->Getattributes();
					MSXML2::IXMLDOMNodePtr pAttT = attNodePtr->getNamedItem("t");

					if(pAttT!=NULL)
					{
						/*request ��t����������������*/
						MSXML2::IXMLDOMNodePtr pReqAttT = str_query.attNodePtr->getNamedItem("t");
						if(pReqAttT!=NULL)
						{
							pReqAttT->Puttext(pAttT->Gettext());
						}
					}
					putText(pParent, dataAttr->encoding, str_query, pLeaf->Gettext());
				}
			}
			else{
				//������Ȃ�
				printf("%s �f�[�^���擾�ł��܂���\n", (char*)str_query.path.at(str_query.path.size()-1));
			}
		}

		break;

	case XmlMessage::modifiedSet: //set���b�Z�[�W�㏑��

		for (unsigned int i = 0; i < setList.data.size(); i++)
		{			
			setData = setList.data.at(i);
			unsigned int dbID;
			//dbID�������Ă��邩�m�F
			dbID = setDBID(&setData);
			if (dataAttr->ID == dbID)
			{
				if (vbool=anaMessage(&setData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf))
				{
					break;//for���𔲂���
				}
			}
			else
			{
					vbool = VARIANT_FALSE;
			}
			
		}
		if (vbool)
		{			
			putText(pLeaf, dataAttrEncoding::base64, str_query, pParent->Gettext());
		}
		else
		{
			// setData�̒��ɖ����̂Œǉ�����
			//
			//name����������setList��T��
			unsigned int idx = 0;   //name��v������̂�������Ȃ������ꍇ��MAIN�ɒǉ�����
/*			for (unsigned int i = 0; i < getList.data.size(); i++)
			{
				XmlMessage getData = getList.data.at(i);
				_bstr_t nameL = getAttName(getData.pDoc, 0);
				_bstr_t nameP = getAttName(pParent->GetownerDocument(), 0);

				if (nameL == nameP)  //name����v
				{
					idx = i;
					break;
				}
			}
			*/
			
			for (unsigned int i = 0; i < setList.data.size(); i++)
			{
				XmlMessage setData = setList.data.at(i);			
				unsigned int dbID = setDBID(&setData);
				if (dataAttr->ID == dbID) //dbID�������Ă����甲����
				{
					idx = i;
					break; //�f�[�^�擾�����̂�for���𔲂���
				}

			}

			setData = setList.data.at(idx); 
			anaMessage(&setData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf);
			//pLeaf�͈�v�����Ƃ���܂ł̗v�f�|�C���^
			MSXML2::IXMLDOMElementPtr pBranch = pLeaf;
			iStart = VARIANT_FALSE;
			
			for(size_t i = 0 ; i < str_query.path.size(); i++ )
			{
				_bstr_t name =   str_query.path.at(i);

				if(iStart == VARIANT_TRUE)
				{
					pDatas = setData.pDoc->createElement(name); //�v�f�쐬

					MSXML2::IXMLDOMNamedNodeMapPtr map = pParent->Getattributes(); //����
					if(map!=NULL)
					{						
						for(long i =0; i <map->Getlength(); i++)	//�����̐������ǉ�
						{				
							MSXML2::IXMLDOMNodePtr  pNode = map->Getitem(i);
							pDatas->setAttribute(pNode->GetnodeName(), pNode->GetnodeValue());
						}
					}
					pBranch->appendChild(pDatas);
					pBranch = pDatas;
				}

				if(_tcscmp(pLeaf->GetnodeName(),name) ==0)	//�v�f������v�H
				{
					iStart = VARIANT_TRUE; //						
				}
			}	
			pDatas->put_text(pParent->Gettext()); //�e�L�X�g��ǋL
		}

		break;

	case XmlMessage::modifiedGet: //get���b�Z�[�W�㏑��

		iStart = VARIANT_FALSE;

		for (unsigned int i = 0; i < getList.data.size(); i++)
		{
			XmlMessage getData = getList.data.at(i);
			if (vbool=anaMessage(&getData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf))//TODO:�ǂ̑����̈�v���܂Ō��邩�H
			{
				unsigned int dbID;
				//dbID�������Ă��邩�m�F
				dbID = setDBID(&getData);

				if (dataAttr->ID == dbID) //dbID�������Ă����甲����
				{
					break; //�f�[�^�擾�����̂�for���𔲂���
				}
				else{
					vbool = VARIANT_FALSE;
				}
			}
		}
		if (vbool)
		{
			MSXML2::IXMLDOMAttributePtr pAttr = pParent->getAttributeNode("send");
			if (pAttr != NULL)
			{
				pLeaf->setAttribute("send", pAttr->GetnodeValue()); //TODO:���ɏ�����������̂����邩�m�F
			}
		}
		else
		{
			// getData�̒��ɖ����̂ŁA�ǉ�����
			//
			IElementPtr pDatas  = NULL;
			VARIANT_BOOL iStart = VARIANT_FALSE;

			//name����������getList��T��
			unsigned int idx = 0;   //name��v������̂�������Ȃ������ꍇ��MAIN�ɒǉ�����
			for (unsigned int i = 0; i < getList.data.size(); i++)
			{
				XmlMessage getData = getList.data.at(i);
				_bstr_t nameL = getAttName(getData.pDoc, 0);
				_bstr_t nameP = getAttName(pParent->GetownerDocument(), 0);
				if (nameL == nameP)  //name����v
				{
					idx = i;
					break;
				}
			}

			getData = getList.data.at(idx); 
			anaMessage(&getData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf);
			//pLeaf�͈�v�����Ƃ���܂ł̗v�f�|�C���^
			MSXML2::IXMLDOMElementPtr pBranch = pLeaf;

			for(size_t i = 0 ; i < str_query.path.size(); i++ )
			{
				_bstr_t name =   str_query.path.at(i);

				if(iStart == VARIANT_TRUE)
				{
					pDatas = getData.pDoc->createElement(name); //�v�f�쐬

					MSXML2::IXMLDOMNamedNodeMapPtr map = pParent->Getattributes(); //����
					if(map!=NULL)
					{						
						for(long i =0; i <map->Getlength(); i++)	//�����̐������ǉ�
						{				
							MSXML2::IXMLDOMNodePtr  pNode = map->Getitem(i);
							pDatas->setAttribute(pNode->GetnodeName(), pNode->GetnodeValue());
						}
					}
					pBranch->appendChild(pDatas);
					pBranch = pDatas;
				}

				if(pLeaf!=NULL)
				{
					if(_tcscmp(pLeaf->GetnodeName(),name) ==0)	//�v�f������v�H
					{
						iStart = VARIANT_TRUE; //						
					}
				}
			}	
		}

		break;
	default:
			break;
	}

}
// 
// �@�q�m�[�h����
//
void dataManage::getInChild(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath, 
		struct dataAttribute *dataAttr, int mode)
{
	vector<_bstr_t> path_parent = vpath;
	vector<_bstr_t> path_temp;

	MSXML2::IXMLDOMNodeListPtr pNodes = NULL;

	if (pParent!= NULL && pParent->hasChildNodes())	//�q�m�[�h������H
	{
		MSXML2::IXMLDOMNodeListPtr pNodes = pParent->GetchildNodes();
		long size = pNodes->Getlength();
		for (int iNode = 0; iNode < size; iNode++)	 //�q�m�[�h�̐��������[�v
		{
			MSXML2::IXMLDOMElementPtr pNode = pNodes->Getitem(iNode);//�q�m�[�h
			if (pNode == NULL)
			{
				getLastBranchAttr(pParent, vpath, dataAttr, mode);
			}
			else
			{
				path_temp = path_parent; //�e�m�[�h�̉��ɒǉ����邽�߁A��������e�m�[�h���R�s�[
				path_temp.push_back(pNode->GetnodeName());

				if (mode == XmlMessage::modifiedGet)  //get���b�Z�[�W�㏑���̏ꍇ
				{
					struct getMsg str_query;
					str_query.path = path_temp;
					str_query.path_idx = 0;
					str_query.mode = mode;
					str_query.attNodePtr = pNode->Getattributes();
					MSXML2::IXMLDOMElementPtr pLeaf = NULL;
					VARIANT_BOOL vbool = VARIANT_FALSE;
					for (unsigned int i = 0; i < getList.data.size(); i++)
					{
						XmlMessage getData = getList.data.at(i);
						if (vbool = anaMessage(&getData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf))//TODO:�ǂ̑����̈�v���܂Ō��邩�H
						{
							break;//for���𔲂���
						}
					}
					if (vbool)
					{
						MSXML2::IXMLDOMAttributePtr pAttr = pNode->getAttributeNode("send");
						if (pAttr != NULL)
						{
							pLeaf->setAttribute("send", pAttr->GetnodeValue()); //�q�m�[�h�����݂���m�[�h��send�����𔽉f������
						}
					}
				}

				getInChild(pNode, path_temp, dataAttr, mode);	//

				path_temp.pop_back();
			}
			size = pNodes->Getlength();  //�z����΂炷�ꍇ�ȂǁA�v�f��ǉ����邱�Ƃ�����̂�
		}
	}
	else						 //�ŉ��w�̃m�[�h
	{
		//lastBranch(pParent, vpath, dataAttr, mode);
		getLastBranchAttr(pParent, vpath, dataAttr, mode);
	}
}

// 
// Data�^�O�̑����ݒ�
//
//
void dataManage::setDataTagAttr(MSXML2::IXMLDOMElementPtr elementPtr, struct dataAttribute* dataAttr)
{

	dataAttr->encoding = dataAttrEncoding::text;	//�f�t�H���g�l�ݒ�
	dataAttr->array_delimiter = dataAttrArray::tagged;	//�f�t�H���g�l�ݒ�

	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = elementPtr->Getattributes();

	//encoding����
	MSXML2::IXMLDOMNodePtr encodePtr = attNodePtr->getNamedItem("encoding");						
	if(encodePtr != NULL)
	{
		_bstr_t encode = encodePtr->Gettext();
		if(_tcscmp(encode , L"base64")==0)
		{
			dataAttr->encoding = dataAttrEncoding::base64;
			
		}
	}


	//array����
	MSXML2::IXMLDOMNodePtr arrayDelimiterPtr = attNodePtr->getNamedItem("array");						
	if(arrayDelimiterPtr != NULL)
	{
		_bstr_t delimiter = arrayDelimiterPtr->Gettext();
		if(_tcscmp(delimiter , L"spaced")==0)
		{
			dataAttr->array_delimiter = dataAttrArray::spaced;
		}
		else
		{
			dataAttr->array_delimiter = dataAttrArray::tagged;
		}

	}

}

// 
// get���b�Z�[�W�̉��
//
//	result		:XML�f�[�^
//
//  �߂�l      : =S_OK       (0) ;����I��
//              : =S_FALSE    (1) ;�ُ�I��
//              : =MSG_TYPE_NG(2) ;type�^�O���s��
//
unsigned int  dataManage::setRequestData(XmlMessage *request, int DATAID)
{
	vector<_bstr_t> vpath, vpath_msg, vpath_root;	//�v�f�̊K�w���X�g

	MSXML2::IXMLDOMDocument2Ptr pXMLDom = request->pDoc;	
	IElementPtr  pRoot   = pXMLDom->documentElement;
		
	vpath_root.push_back(pRoot->GetnodeName());	//�v�f�����K�w���X�g�ɒǉ�

	if(pRoot->hasChildNodes())	//���[�g�����Ɏq�m�[�h�imessage�m�[�h�j������H
	{
		MSXML2::IXMLDOMNodeListPtr pMessage = pRoot->GetchildNodes();
		int msgNum = pMessage->Getlength();	
					
		for(int iMsg = 0; iMsg < msgNum; iMsg++)			//message�̐��������[�v
		{
			IElementPtr pMsgElem = pMessage->Getitem(iMsg);	//message�v�f			
			int msgType = request->getMsgType(iMsg);

			if(msgType == XmlMessage::msgGet || msgType == XmlMessage::msgMainLink) 
			{
				vpath_msg = vpath_root;
				vpath_msg.push_back(pMsgElem->GetnodeName());	//�v�f�����K�w���X�g�ɒǉ�

				//���b�Z�[�W�^�C�v��set�ɕς���
			    INodePtr ptr = pMsgElem->attributes->getNamedItem("type");
				ptr->put_text(L"set");

				if(pMsgElem->hasChildNodes())						//message�����Ɏq�m�[�h������H
				{
					INodeListPtr pNodes = pMsgElem->GetchildNodes(); //Data�v�f
			
					for(int iNode = 0 ; iNode < pNodes->Getlength(); iNode++)	//�q�m�[�h�̐��������[�v
					{
						IElementPtr pChild = pNodes->Getitem(iNode);//message�̑��m�[�h
						vpath = vpath_msg;
						vpath.push_back(pChild->GetnodeName());		//�v�f�����K�w���X�g�ɒǉ�


						struct dataAttribute dataAttr;

						setDataTagAttr(pChild, &dataAttr);
						dataAttr.ID = DATAID;
						if(msgType == XmlMessage::msgMainLink)
						{
							dataAttr.mode = XmlMessage::replyMainLinkMsg;
							getInChild(pChild, vpath, &dataAttr, XmlMessage::replyMainLinkMsg );
						}
						else
						{
							dataAttr.mode = XmlMessage::replyGetMsg;
							getInChild(pChild, vpath, &dataAttr, XmlMessage::replyGetMsg);
						}
					}
					return S_OK;								
				}
			}
			else
			{
				return MSG_TYPE_NG;	//���b�Z�[�W��type�^�O���s��
			}
		}
	}

	return S_FALSE;	//	TODO:message�v�f�̉��ɂ͏��Ȃ��Ƃ��P�̗v�f���K�v	
}

//
// get���b�Z�[�W�ɂ��f�[�^�擾
//
//	DataID	    :setDB�̃f�[�^ID
//	result		:XML�f�[�^
//
//  �߂�l      : =S_OK              (0) ;����I��
//              : =S_FALSE           (1) ;�ُ�I��
//
unsigned int dataManage::getMsgOnMainSetDB(unsigned int DataID,unsigned int clientID, BSTR *result)
{
	unsigned int match;
	BSTR  bstr;
	BSTR  xmlGetString;

	if(!hasSameID(clientID, getList, &match))	//����ID������H
	{
		if(getDatabyDBID(DataID, &bstr))
		{
			printf("set���b�Z�[�W������܂��� DATA_ID=%d, FILE=%s, LINE=%d",
				DataID, __FILE__, __LINE__);
			return S_FALSE;
		}

		MSXML2::IXMLDOMDocument2Ptr  pDomDoc;
		HRESULT hr     = pDomDoc.CreateInstance( __uuidof(MSXML2::DOMDocument60) );
		pDomDoc->async = VARIANT_FALSE;	

		VARIANT_BOOL isSuccess = VARIANT_FALSE;
		HRESULT hr1  = pDomDoc->raw_loadXML(bstr, &isSuccess);
		::SysFreeString(bstr);

		if(setAttType(pDomDoc, L"get") == VARIANT_TRUE &&
			setAttEncode(pDomDoc, L"text") == VARIANT_TRUE) 
		{
			xmlGetString = pDomDoc->Getxml();
			XmlMessage *xml = new XmlMessage(xmlGetString, &hr);
			::SysFreeString(xmlGetString);
			saveData(xml, clientID);
			delete xml;

			if (getData(clientID, result)!= S_OK)
			{
				printf("xml�����擾�ł��܂���ł���.clientID=%d, FILE=%s, LINE=%d",
					clientID, __FILE__, __LINE__);
				return S_FALSE; 
			}
		}
		else
		{
			return S_FALSE; 
		}
	}
	else
	{
			if (getData(clientID, result)!= S_OK)
			{
				printf("xml�����擾�ł��܂���ł���.clientID=%d, FILE=%s, LINE=%d",
					clientID, __FILE__, __LINE__);
				return S_FALSE; 
			}
	}
	return S_OK; 
}
//
// get���b�Z�[�W�ɂ��f�[�^�擾
//
//	DataID	    :setDB�̃f�[�^ID
//	result		:XML�f�[�^
//
//  �߂�l      : =S_OK              (0) ;����I��
//              : =S_FALSE           (1) ;�ُ�I��
//
unsigned int dataManage::getDatabyDBID(unsigned int DataID, BSTR *result)
{
	unsigned int match;

	if(hasSameID(DataID, setList, &match))	//����ID������H
	{
	    XmlMessage setData = setList.data.at(match); 
		setData.pDoc->get_xml(result);	
	}
	else
	{
		//�f�[�^ID�G���[
		printf("��v����f�[�^ID������܂��� .�@FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}
	return S_OK; 
}

//
//
//  �߂�l      : =S_OK              (0) ;����I��
//              : =S_FALSE           (1) ;�ُ�I��
//              : =GET_ERR_MSG_TYPE  (2) ;type�^�O���s��
//
unsigned int dataManage::appendData(XmlMessage *request)
{
	vector<_bstr_t> vpath, vpath_msg, vpath_root;	//�v�f�̊K�w���X�g

	MSXML2::IXMLDOMDocument2Ptr pXMLDom = request->pDoc;
	IElementPtr  pRoot = pXMLDom->documentElement;
	int mode = XmlMessage::modeErr;

	vpath_root.push_back(pRoot->GetnodeName());	//�v�f�����K�w���X�g�ɒǉ�

	if (pRoot->hasChildNodes())	//���[�g�����Ɏq�m�[�h�imessage�m�[�h�j������H
	{
		MSXML2::IXMLDOMNodeListPtr pMessage = pRoot->GetchildNodes();
		int msgNum = pMessage->Getlength();

		for (int iMsg = 0; iMsg < msgNum; iMsg++)			//message�̐��������[�v
		{
			IElementPtr pMsgElem = pMessage->Getitem(iMsg);	//message�v�f
			
			//DBID���擾
			unsigned int dbID;
			dbID = setDBID(request);

			if (request->getMsgType(iMsg) == XmlMessage::msgGet)
			{
				mode = XmlMessage::modifiedGet;
			}
			else if (request->getMsgType(iMsg) == XmlMessage::msgSet)
			{
				mode = XmlMessage::modifiedSet;
				if(setList.data.size()==0 || dbIDMap.size()> setList.data.size()) 
				{
					saveData(request, dbID); //
					return S_OK;
				}

			}
			else if (request->getMsgType(iMsg) == XmlMessage::msgMainLink)
			{
				mode = XmlMessage::modifiedSet;
			}
			else
			{
				return MSG_TYPE_NG;	//get���b�Z�[�W��type�^�O��"get","set","mainLink send"�ȊO								
			}

			vpath_msg = vpath_root;
			vpath_msg.push_back(pMsgElem->GetnodeName());	//�v�f�����K�w���X�g�ɒǉ�

			//if (pMsgElem->hasChildNodes())						//message�����Ɏq�m�[�h������H
			//{
			//	INodeListPtr pNodes = pMsgElem->GetchildNodes(); //Data�v�f

			//	for (int iNode = 0; iNode < pNodes->Getlength(); iNode++)	//�q�m�[�h�̐��������[�v
			//	{
			//		IElementPtr pChild = pNodes->Getitem(iNode);//message�̑��m�[�h
			//		vpath = vpath_msg;
			//		vpath.push_back(pChild->GetnodeName());	//�v�f�����K�w���X�g�ɒǉ�

			//		struct dataAttribute dataAttr;
			//		setDataTagAttr(pChild, &dataAttr);
			//		dataAttr.mode = mode;
			//		dataAttr.ID = dbID;
			//		getInChild(pChild, vpath, &dataAttr, mode);	//�q�m�[�h�̉��
			//	}
			//	return S_OK;
			//}
			vpath = vpath_msg;
			struct dataAttribute dataAttr;
			setDataTagAttr(pMsgElem, &dataAttr);
			dataAttr.mode = mode;
			dataAttr.ID = dbID;
			getInChild(pMsgElem, vpath, &dataAttr, mode);	//�q�m�[�h�̉�� message����킽���AData�v�f�����͂���
		}
		return S_OK;
	}

	return S_FALSE;	//	TODO:message�v�f�̉��ɂ͏��Ȃ��Ƃ��P�̗v�f���K�v	
}

//
// XML���b�Z�[�WDB�̏C��
//
//	ID	        :ID
//	result		:XML�f�[�^
//
//  �߂�l      : =S_OK              (0) ;����I��
//              : =S_FALSE           (1) ;�ُ�I��
//              : =GET_ERR_MSG_TYPE  (2) ;type�^�O���s��
//
unsigned int dataManage::addSetMsgData(unsigned int ID, BSTR *result)
{
	HRESULT hr = S_FALSE;
	unsigned int ret = S_FALSE;

	XmlMessage *msg = new XmlMessage(*result, &hr);	//�o�͕ҏW�p�̃��b�Z�[�W��������

	if (hr == S_OK)
	{
		if ((ret = appendData(msg)) == 0)	//���b�Z�[�W�̉��
		{
#ifdef _DEBUG
			//printf(msg->pDoc->Getxml());
#endif
			printf(msg->pDoc->Getxml());
		}
	}
	delete msg;
	return ret;
}

// 
// �@�m�[�htext�̃G���R�[�f�B���O
//
//
void dataManage::nodesEncoding(MSXML2::IXMLDOMElementPtr pParent, int encoding)
{

	if(encoding != dataAttrEncoding::text) return;//�f�R�[�h���K�v�Ȃ��Ȃ烊�^�[��
	if(pParent == NULL)return ;
	
	MSXML2::IXMLDOMNodePtr child= pParent->GetfirstChild(); 
    while (child!=NULL) {
		if(child->GetfirstChild())
		{
			nodesEncoding(child, encoding);//�q�m�[�h������Ȃ�΁A�ċA�Ăяo��
		}
		child = child->GetnextSibling();	//�Z��m�[�h���R�s�[
    }
	
	BSTR value_text;
	_bstr_t str = pParent->Gettext();
	if(str.length() > 0) 
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attMapPtr = pParent->Getattributes();
		if(attMapPtr == NULL) 
		{
			//�f�t�H���g"R"
			Base64Data::convertBintoText(str, _bstr_t("R"), &value_text);
		}
		else
		{
			MSXML2::IXMLDOMNodePtr attNodePtr = attMapPtr->getNamedItem("t");
			if(attNodePtr== NULL)
			{
				//�f�t�H���g"R"
				Base64Data::convertBintoText(str, _bstr_t("R"), &value_text);
			}
			else
			{
				_variant_t attT = attNodePtr->Gettext();
				attT.ChangeType(VT_BSTR);
				//base64�ϊ�
				Base64Data::convertBintoText(str, attT.bstrVal, &value_text);
			}
		}
					
		pParent->put_text(value_text);
		::SysFreeString(value_text);
	}

}

//
// �z��v�f���X�y�[�X��؂��set����
//    pParent   �F�v�f�|�C���^
//    vpath     �F�^�O���
//    dataAttr  �F����(ELEMATTRIBUTES)
//    mode      �F���[�h
//    st        �F�z��v�f�̗v�f�ԍ��i�J�n�j
//    ed        �F�z��v�f�̗v�f�ԍ��i�I���j
// 
void dataManage::setArrayData(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath, 
						struct dataAttribute *dataAttr, int mode, int st, int ed)
{
	if (ed < st || st < 0 || ed < 0)
	{
		printf("SET:%s �z��v�f�̎w�肪�s�K�؂ł��B\n", (char*)vpath.at(vpath.size()-1));
		return;
	}
	
	int tnum = ed - st + 1;
	TCHAR str[8];

	if (mode == XmlMessage::modifiedSet)
	{
		TCHAR** cmtext = NULL;
		BSTR text;
		TCHAR ctext[256];
		BSTR bntext;
		TCHAR *tmp;

		cmtext = (TCHAR **)malloc(sizeof(TCHAR *)* tnum);
		for (int i = 0; i < tnum; i++)
		{
			cmtext[i] = (TCHAR *)malloc(sizeof(TCHAR)* BASE64_BUFFER_SIZE);
		}
		if (cmtext == NULL)
		{
			printf("SET:%s �������̊m�ێ��s\n", (char*)vpath.at(vpath.size()-1));
			return;
		}
		//set����f�[�^���擾
		text = pParent->Gettext();

		//text���󔒂ŕ���
		TCHAR *token;
		_stprintf_s(ctext, BASE64_BUFFER_SIZE, _T("%s"), (LPCTSTR)text);
		tmp = _tcstok_s(ctext, L" ", &token);
		_tcscpy_s(cmtext[0], BASE64_BUFFER_SIZE, tmp);
		for (int i = 1; i < tnum; i++)
		{
			tmp = _tcstok_s(NULL, L" ", &token);
			if (tmp != NULL)
			{
				_tcscpy_s(cmtext[i], BASE64_BUFFER_SIZE, tmp);
			}
			else
			{
				printf("SET:%s �z��f�[�^�̐�������܂���\n", (char*)vpath.at(vpath.size()-1));
				//���
				for (int i = 0; i < tnum; i++)
				{
					if (cmtext[i] != NULL)
					{
						free(cmtext[i]);
					}
				}
				free(cmtext);
				::SysFreeString(text);
				return;
			}
		}
		//1���o�^
		for (int i = st; i <= ed; i++)
		{
			_bstr_t bstrt(cmtext[i - st]);   //�w��v�f��0����n�܂�Ƃ͌���Ȃ��̂�
			bntext = bstrt.copy();
			
			_stprintf_s(str, 8, _T("%d"), i);
			pParent->setAttribute(L"i", str);  //i�����ǉ�

			pParent->Puttext(bntext);  //set����f�[�^����������
			lastBranch(pParent, vpath, dataAttr, mode);

			::SysFreeString(bntext);
		}

		//���
		for (int i = 0; i < tnum; i++)
		{
			if (cmtext[i] != NULL)
			{
				free(cmtext[i]);
			}
		}
		free(cmtext);
		::SysFreeString(text);
	}
	else if ((mode == XmlMessage::replyGetMsg) || (mode == XmlMessage::modifiedGet))
	{
		if (dataAttr->array_delimiter == dataAttrArray::tagged)
		{
			//GET����XML�ɒǉ�������
			MSXML2::IXMLDOMElementPtr pParent2;
			////1���o�^
			for (int i = st; i <= ed; i++)
			{
				//�v�f���R�s�[�����肩��
				MSXML2::IXMLDOMNodePtr clone = pParent->cloneNode(VARIANT_TRUE);
				nodesEncoding(clone, dataAttr->encoding);
				pParent2 = pParent->parentNode->insertBefore(clone, pParent.GetInterfacePtr()); //�O�ɒǉ�
				
				_stprintf_s(str, 8, _T("%d"), i);
				pParent2->setAttribute(L"i", str);  //i�����ǉ�
				lastBranch(pParent2, vpath, dataAttr, mode);
			}
			//�m�[�h�폜
			pParent->parentNode->removeChild(pParent);
			pParent = pParent2;
		}
		else
		{
			lastBranchGetArray(pParent, vpath, dataAttr, mode, st, ed);
		}
	}
}

//
// �ŏI�m�[�h�̔z�񑮐���ύX
//    pParent   �F�v�f�|�C���^
//    vpath     �F�^�O���
//    dataAttr  �F����(ELEMATTRIBUTES)
//    mode      �F���[�h
void dataManage::getLastBranchAttr(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath, struct dataAttribute *dataAttr, int mode)
{
	//�z��̏ꍇ
	char a[8];
	int tnum = 0;

	MSXML2::IXMLDOMNodePtr pAttI = pParent->Getattributes()->getNamedItem("i");  //����t�擾
	if (pAttI == NULL)  //����i���Ȃ�����
	{
		MSXML2::IXMLDOMNodePtr pAttT = pParent->Getattributes()->getNamedItem("t");  //����t�擾
		if (pAttT != NULL)  //����t������
		{
			//t�����̃^�C�v�Ɨv�f�����擾
			getAttTValue(pAttT, a, &tnum);

			if (strcmp(a, "S") == 0){  //������
				//�ʏ�̏���
				lastBranch(pParent, vpath, dataAttr, mode);
				return;
			}
		}
		//��������������
		if (tnum > 1)
		{
			//�z���set
			int st, ed;
			st = 0;
			ed = tnum - 1;
			//�z��v�f��set
			setArrayData(pParent, vpath, dataAttr, mode, st, ed);
		}
		else
		{
			//�ʏ�̏���
			lastBranch(pParent, vpath, dataAttr, mode);
		}
	}
	else
	{
		_bstr_t itext_b = pAttI->Gettext();
		TCHAR itext_c[16];
		_stprintf_s(itext_c, 16, _T("%s"), (LPCTSTR)itext_b);

		//i="1:3"�Ȃǂ̏ꍇ
		TCHAR *ret;
		ret = _tcsstr(itext_c, _T(":"));
		if (ret != NULL)
		{
			
			MSXML2::IXMLDOMNodePtr pAttT = pParent->Getattributes()->getNamedItem("t");  //����t�擾
			if (pAttT != NULL)  //����t������
			{
				//t�����̃^�C�v�Ɨv�f�����擾
				getAttTValue(pAttT, a, &tnum);

				if (strcmp(a, "S") == 0){  //������
					//�ʏ�̏���
					lastBranch(pParent, vpath, dataAttr, mode);
					return;
				}
			}
			int st, ed;
			TCHAR *tmp, *token = NULL;
			tmp = _tcstok_s(itext_c, _T(":"), &token);
			st = _tstoi(tmp);
			tmp = _tcstok_s(NULL, L":", &token);
			ed = _tstoi(tmp);

			if (st > ed){
				int n = st;
				st = ed;
				ed = n;
			}
			if (st < 0){
				printf("Warning:%s ��i�����Ƀ}�C�i�X�͎w��ł��܂���B0�ɂ��܂��B\n", (char*)vpath.at(vpath.size()-1));
				st = 0;
			}
			if (tnum < (ed - st+1)){
				printf("Warning:%s ��i�����̎w��v�f����t�����̗v�f�����������ł�\n", (char*)vpath.at(vpath.size()-1));
				ed = tnum + st - 1;
			}
			//�z��v�f��set
			setArrayData(pParent, vpath, dataAttr, mode, st, ed);
		}
		else
		{
			//�ʏ�̏���
			lastBranch(pParent, vpath, dataAttr, mode);
		}
	}
}

//lastBranch��GET���������[�v�ɂ����֐�
void dataManage::lastBranchGetArray(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath, 
									struct dataAttribute *dataAttr, int mode, int st, int ed)
{
	struct getMsg str_query;
	str_query.path = vpath;
	str_query.path_idx = 0;
	str_query.mode = mode;
	if (pParent != NULL) str_query.attNodePtr = pParent->Getattributes();

	MSXML2::IXMLDOMElementPtr pLeaf = NULL;
	MSXML2::IXMLDOMElementPtr pDatas = NULL;

	VARIANT_BOOL vbool = VARIANT_FALSE;
	VARIANT_BOOL iStart = VARIANT_FALSE;
	XmlMessage setData;
	XmlMessage getData;
	XmlMessage mainLinkData;

	BSTR bntext = NULL;
	TCHAR tmp[1024] = { 0 };

	switch (mode)
	{
	case XmlMessage::replyGetMsg: //get���b�Z�[�W�ɑ΂��鉞��
		if (getAttSendValue(str_query.attNodePtr)) //���� send��enabled�Ȃ��
		{
			for (int j = st; j <= ed; j++)
			{    //�v�fi�Ń��[�v
				TCHAR str[8];
				_stprintf_s(str, 8, _T("%d"), j);

				MSXML2::IXMLDOMNodePtr pI = str_query.attNodePtr->getNamedItem(L"i");
				if (pI == NULL){
					pParent->setAttribute(L"i", str);  //i�����ǉ�
					str_query.attNodePtr = pParent->Getattributes();
					pI = str_query.attNodePtr->getNamedItem(L"i");
				}
				_bstr_t bstrI = str;
				pI->Puttext(bstrI);
				
				for (unsigned int i = 0; i < setList.data.size(); i++)
				{
					XmlMessage setData = setList.data.at(i);
					if (vbool = anaMessage(&setData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf))
					{
						unsigned int dbID;
						//dbID�������Ă��邩�m�F
						dbID = setDBID(&setData);

						if (dataAttr->ID == dbID){
							break;//for���𔲂���
						}
						else{
							vbool = VARIANT_FALSE;
						}
					}
				}
				if (vbool)
				{
					if (pLeaf->hasChildNodes())
					{
						MSXML2::IXMLDOMNodeListPtr  pNodes = pLeaf->GetchildNodes();
						for (int iNode = 0; iNode < pNodes->Getlength(); iNode++)		//�q�m�[�h�̐��������[�v
						{
							MSXML2::IXMLDOMElementPtr pChild = pNodes->Getitem(iNode);  //�q
							if (pChild != NULL)
							{
								//MSXML2::IXMLDOMNodePtr clone = pChild->cloneNode(VARIANT_TRUE);
								//nodesEncoding(clone, dataAttr->encoding);
								//pParent->appendChild(clone);
								appendDeepChild(pChild, dataAttr, pParent);
							}
							else if (iNode == 0)
							{
								BSTR dataType = NULL;
								
								/*t����*/
								MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pLeaf->Getattributes();
								MSXML2::IXMLDOMNodePtr pAttT = attNodePtr->getNamedItem("t");
								if (pAttT != NULL){
									/*request ��t����������������*/
									MSXML2::IXMLDOMNodePtr pReqAttT = str_query.attNodePtr->getNamedItem("t");
									if (pReqAttT != NULL)
									{
										pReqAttT->Puttext(pAttT->Gettext());
									}
								}

								pAttT->get_text(&dataType);
								if (dataAttr->encoding == dataAttrEncoding::text)
								{
									Base64Data::convertBintoText(pLeaf->Gettext(), dataType, &bntext);
								}
								else
								{
									bntext = pLeaf->Gettext();  //�l
								}
								_tcsncat_s(tmp, bntext, BASE64_BUFFER_SIZE);
								::SysFreeString(dataType);
							}
						}
					}
					else
					{
						/*t����*/
						MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pLeaf->Getattributes();
						MSXML2::IXMLDOMNodePtr pAttT = attNodePtr->getNamedItem("t");
						BSTR dataType = NULL;

						if (pAttT != NULL)
						{
							/*request ��t����������������*/
							MSXML2::IXMLDOMNodePtr pReqAttT = str_query.attNodePtr->getNamedItem("t");
							if (pReqAttT != NULL)
							{
								pReqAttT->Puttext(pAttT->Gettext());
							}
						}

						pAttT->get_text(&dataType);
						if (dataAttr->encoding == dataAttrEncoding::text)
						{
							Base64Data::convertBintoText(pLeaf->Gettext(), dataType, &bntext);
						}
						else
						{
							bntext = pLeaf->Gettext();  //�l
						}
						_tcsncat_s(tmp, bntext, BASE64_BUFFER_SIZE);
						::SysFreeString(dataType);

					}
					if (j != ed){
						_tcsncat_s(tmp, L" ", 1);
					}
				}
			}
			if (_tcscmp(tmp, L"") != 0){
				_bstr_t bstrt(tmp);
				TCHAR str[8];
				_stprintf_s(str, 8, _T("%d:%d"), st, ed);
				pParent->setAttribute(L"i", str);  //i�����ǉ�
				pParent->put_text(bstrt.copy());	//xml�ɔ��f
			}
			else{
				printf("%s �z��̗v�f�����������Ȃ����A�f�[�^�����݂��܂���B\n", (char*)str_query.path.at(str_query.path.size()-1));
			}
		}
		break;

	case XmlMessage::modifiedGet: //get���b�Z�[�W�㏑��
		lastBranch(pParent, vpath, dataAttr, mode);
		break;

	default:
		break;
	}
	if(bntext != NULL)	::SysFreeString(bntext);
}

//
//GET:�q�m�[�h�ȉ��́i���A�Б��j�m�[�h��ǉ�����
//      pChild�@ �F�ǉ����̃|�C���^�iDB�ۑ��̃f�[�^pLeaf�j
//      pParent�@�F�v�f�|�C���^(�ǉ�����鑤�j�iGET���j
//
//     �߂�l�@�@�FVARIANT_TRUE  �ǉ�����
//                 VARIANT_FALSE�@���s
VARIANT_BOOL dataManage::appendDeepChild(MSXML2::IXMLDOMElementPtr pChild, struct dataAttribute *dataAttr, MSXML2::IXMLDOMElementPtr pParent)
{
	//�q�m�[�h��ǉ�
	MSXML2::IXMLDOMNodePtr clone = pChild->cloneNode(VARIANT_TRUE);
	nodesEncoding(clone, dataAttr->encoding);
	MSXML2::IXMLDOMElementPtr pParent2 = pParent->appendChild(clone);

	//���A�Б��̃m�[�h���R�s�[
	if (pChild->hasChildNodes())
	{		
		MSXML2::IXMLDOMNodeListPtr  pNodes2 = pChild->GetchildNodes();

		for (int iNode2 = 0; iNode2 < pNodes2->Getlength(); iNode2++)
		{
			MSXML2::IXMLDOMElementPtr pChild2 = pNodes2->Getitem(iNode2);
			if (pChild2 != NULL)
			{
				//pChild�Ɏq�m�[�h������ꍇ�A�ǉ�����pParent2�̒��g�ɕs�v�f�[�^(0.0e0)�Ȃǂ������Ă�̂ō폜
				if (iNode2 == 0){
					pParent2->put_text(L"");
				}

				appendDeepChild(pChild2, dataAttr, pParent2);
			}
		}
	}

	return VARIANT_TRUE;
}

#pragma region get
//
//
//  �߂�l      : =S_OK       (0) ;����I��
//              : =S_FALSE    (1) ;�ُ�I��
//              : =ERR_TAG_NAME(3);�^�O���̃T�C�Y��0
//			    : =DATA_GET_ERR   ;�f�[�^�擾�G���[
//			    : =ARRAY_SIZE_ERR ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
//
//
// get���b�Z�[�W�ɂ��f�[�^�擾
//
//	clientID	:�N���C�A���gID
//	result		:XML�f�[�^
//
//  �߂�l      : =S_OK              (0) ;����I��
//              : =S_FALSE           (1) ;�ُ�I��
//              : =GET_ERR_MSG_TYPE  (2) ;type�^�O���s��
//              : =GET_ERR_NEW_XMLMSG(3) ;XmlMessage�N���X�̏������Ɏ��s
//              : =GET_ERR_NO_CID    (4) ;�N���C�A���gID�ɑΉ����郁�b�Z�[�W���Ȃ�
//
unsigned int dataManage::getData(unsigned int clientID, BSTR *result)
{

	unsigned int ret = GET_ERR_NO_CID;

	if (!getList.data.empty()) //get���b�Z�[�W���X�g����łȂ��H
	{
		vector<XmlMessage>::iterator get_list = getList.data.begin();

		for (unsigned int i = 0; i<getList.data.size(); i++)	//get���b�Z�[�W�������[�v
		{

			if (get_list->getDataID() == clientID)		//ClientID����v�H
			{
				BSTR bstrGetXML;
				get_list->pDoc->get_xml(&bstrGetXML);	//get���b�Z�[�W���X�g����get���b�Z�[�W���擾

				HRESULT hr = S_FALSE;
				XmlMessage *msg = new XmlMessage(bstrGetXML, &hr);	//�o�͕ҏW�p�̃��b�Z�[�W��������

				if (hr == S_OK)
				{
					msg->removeDisenable();

					//DBID���擾
					unsigned int dbID;
					dbID = setDBID(msg);

					if ((ret = setRequestData(msg, dbID)) == 0)	//get���b�Z�[�W�̉��
					{
						msg->pDoc->get_xml(result); //XML�f�[�^���擾
					}

				}
				else
				{
					ret = GET_ERR_NEW_XMLMSG;
				}

				::SysFreeString(bstrGetXML);
				delete msg;

				break;
			}

			++get_list;
		}
	}


	return ret;
}
//
//
//
//
//  �߂�l      : =S_OK              (0) ;����I��
//              : =S_FALSE           (1) ;�ُ�I��
//              : =GET_ERR_MSG_TYPE  (2) ;type�^�O���s��
//              : =GET_ERR_NEW_XMLMSG(3) ;XmlMessage�N���X�̏������Ɏ��s
//              : =GET_ERR_NO_CID    (4) ;�N���C�A���gID�ɑΉ����郁�b�Z�[�W���Ȃ�
//
unsigned int dataManage::getData(unsigned int DATAID, BSTR request, BSTR path, BSTR *result, int *attTID)
{
	unsigned int ret = GET_ERR_NO_CID;

	HRESULT hr = S_FALSE;
	XmlMessage *msg = new XmlMessage(request, &hr);	//�o�͕ҏW�p�̃��b�Z�[�W��������
	if (hr == S_OK)
	{
		if ((ret = setRequestData(msg, DATAID)) == 0)	//get���b�Z�[�W�̉��
		{
			BSTR getXml;
			msg->pDoc->get_xml(&getXml);					//XML�f�[�^���擾
			::SysFreeString(getXml);
		}
	}
	else
	{
		ret = GET_ERR_NEW_XMLMSG;
	}

	MSXML2::IXMLDOMNodePtr pChild = msg->pDoc->selectSingleNode(path);
	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pChild->Getattributes();
	MSXML2::IXMLDOMNodePtr attT = attNodePtr->getNamedItem("t"); //T����

	BSTR dataType = NULL;
	if (attT == NULL)
	{
		dataType = SysAllocString(L"R");	//�f�t�H���g��ݒ�
	}
	else
	{
		attT->get_text(&dataType);
		if (SysStringByteLen(dataType) <= 0)
		{
			dataType = SysAllocString(L"R");	//�f�t�H���g��ݒ�
		}
	}

	char* c = _com_util::ConvertBSTRToString(dataType);
	size_t c_size = 0;

	for (c_size = 0; c_size< strlen(c); c_size++)	//�A���t�@�x�b�g���𔲂��o��
	{
		if (!isalpha(c[c_size])) break;
	}

	map<int, string>::iterator it = dataTypeMap.begin();
	while (it != dataTypeMap.end())
	{
		if (c_size == strlen((it->second).c_str()))
		{
			if (strncmp((it->second).c_str(), c, strlen((it->second).c_str())) == 0) //�������v
			{
				*attTID = it->first;
				break;
			}
		}
		++it;
	}

	::SysFreeString(dataType);
	delete[] c;

	//
	*result = pChild->Gettext();
	delete msg;

	return ret;
}
//
// get���b�Z�[�W�ɂ��f�[�^�擾
//
//	clientID	:�N���C�A���gID
//  name        :message�v�f��name�����l
//	result		:XML�f�[�^
//
//  �߂�l      : =S_OK              (0) ;����I��
//              : =S_FALSE           (1) ;�ُ�I��
//              : =GET_ERR_MSG_TYPE  (2) ;type�^�O���s��
//              : =GET_ERR_NEW_XMLMSG(3) ;XmlMessage�N���X�̏������Ɏ��s
//              : =GET_ERR_NO_CID    (4) ;�N���C�A���gID�ɑΉ����郁�b�Z�[�W���Ȃ�
//
unsigned int dataManage::getData(unsigned int clientID, BSTR name, BSTR *result)
{
	unsigned int ret = GET_ERR_NO_CID;

	if (!getList.data.empty()) //get���b�Z�[�W���X�g����łȂ��H
	{
		vector<XmlMessage>::iterator get_list = getList.data.begin();

		for (unsigned int i = 0; i<getList.data.size(); i++)	//get���b�Z�[�W�������[�v
		{
			BSTR name_list = getAttName(get_list->pDoc, 0);

			char* str1 = _com_util::ConvertBSTRToString(name);
			char* str2 = _com_util::ConvertBSTRToString(name_list);
			::SysFreeString(name_list);

			//ClientID����v�H && name��������v�H
			if ((get_list->getDataID() == clientID) && (strncmp(str2, str1, BASE64_BUFFER_SIZE) == 0))
			{
				BSTR bstrGetXML;
				get_list->pDoc->get_xml(&bstrGetXML);	//get���b�Z�[�W���X�g����get���b�Z�[�W���擾

				//Debug
				//printf("\ngetXML�o�^���Ă������\n");
				//wprintf(bstrGetXML);

				HRESULT hr = S_FALSE;
				XmlMessage *msg = new XmlMessage(bstrGetXML, &hr);	//�o�͕ҏW�p�̃��b�Z�[�W��������

				if (hr == S_OK)
				{
					msg->removeDisenable();

					//DBID���擾
					unsigned int dbID;
					dbID = setDBID(msg);

					if ((ret = setRequestData(msg, dbID)) == 0)	//get���b�Z�[�W�̉��
					{
						msg->pDoc->get_xml(result); //XML�f�[�^���擾
					}
				}
				else
				{
					ret = GET_ERR_NEW_XMLMSG;
				}

				::SysFreeString(bstrGetXML);
				delete msg;
				delete[] str1;
				delete[] str2;

				break;
			}
			delete[] str1;
			delete[] str2;
			++get_list;
		}
	}

	return ret;
}

HRESULT dataManage::getElement(unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, LPTSTR result, int* attTID)
{

	if (vpath.size() <= 0)
	{
		printf("�^�O����NULL�ł��BFILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return ERR_TAG_NAME;
	}

	// XML�f�[�^�ۑ��N���X	
	HRESULT hr = S_FALSE;
	XmlData wrt = XmlData(&hr);
	if (FAILED(hr))
	{
		printf("Failed to instantiate an XML DOM.�@FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}

	// ���b�Z�[�W�^�O
	MessageTag msg = MessageTag(L"mainlink send");
	wrt.addMessageTag(&msg);

	//�@�J�e�S���[�^�O
	IElementPtr pChild = wrt.addChild(msg.getMsgPtr(), L"Data"); // ��P�����ɐe���w��
	pChild->setAttribute("encoding", "base64");
	IElementPtr pParent = pChild;

	//  �^�O
	vector<_bstr_t>::iterator it = vpath.begin();
	int max_idx = vpath.size() - 1; //���X�g�T�C�Y

	_bstr_t path = L"/root/message/Data/";

	for (int i = 0; i < max_idx; i++)			//���X�g�̍\���v�f�̌������[�v
	{
		pParent = pChild;
		pChild = wrt.addChild(pParent, vpath.at(i));
		path += (vpath.at(i) + L"/");
	}
	path += vpath.at(max_idx);

	pParent = pChild;
	ElementData child = ElementData(vpath.at(max_idx), NULL);
	if (idx != NULL) child.setAttribute(L"i", idx);
	child.setAttribute(L"t", NULL); //�_�~�[�f�[�^������
	IElementPtr pElem = wrt.addChild(pParent, &child);

	hr = getElementData(&wrt, clientID, path, result, attTID);

	return hr;
}
//
//
//  �߂�l      : =S_OK       (0) ;����I��
//              : =S_FALSE    (1) ;�ُ�I��
//              : =ERR_TAG_NAME(3);�^�O���̃T�C�Y��0
//			    : =DATA_GET_ERR   ;�f�[�^�擾�G���[
//			    : =ARRAY_SIZE_ERR ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
//
HRESULT dataManage::getElement(LPCTSTR type, unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, LPTSTR result, int* attTID)
{

	if (vpath.size() <= 0)
	{
		printf("�^�O����NULL�ł��BFILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return ERR_TAG_NAME;
	}

	// XML�f�[�^�ۑ��N���X	
	HRESULT hr = S_FALSE;
	XmlData wrt = XmlData(&hr);
	if (FAILED(hr))
	{
		printf("Failed to instantiate an XML DOM.�@FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}

	// ���b�Z�[�W�^�O
	MessageTag msg = MessageTag(type);
	wrt.addMessageTag(&msg);

	//�@�J�e�S���[�^�O
	IElementPtr pChild = wrt.addChild(msg.getMsgPtr(), L"Data"); // ��P�����ɐe���w��
	pChild->setAttribute("encoding", "base64");
	IElementPtr pParent = pChild;

	//  �^�O
	vector<_bstr_t>::iterator it = vpath.begin();
	int max_idx = vpath.size() - 1; //���X�g�T�C�Y

	_bstr_t path = L"/root/message/Data/";

	for (int i = 0; i < max_idx; i++)			//���X�g�̍\���v�f�̌������[�v
	{
		pParent = pChild;
		pChild = wrt.addChild(pParent, vpath.at(i));
		path += (vpath.at(i) + L"/");
	}
	path += vpath.at(max_idx);

	pParent = pChild;
	ElementData child = ElementData(vpath.at(max_idx), NULL);
	if (idx != NULL) child.setAttribute(L"i", idx);
	child.setAttribute(L"t", NULL); //�_�~�[�f�[�^������
	IElementPtr pElem = wrt.addChild(pParent, &child);

	hr = getElementData(&wrt, clientID, path, result, attTID);

	return hr;
}
//
//
//  �߂�l      : =S_OK        (0) ;����I��
//              : =S_FALSE     (1) ;�ُ�I��
//              : =ERR_TAG_NAME(3);�^�O���̃T�C�Y��0
//
HRESULT dataManage::setElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, LPCTSTR valtype, LPCTSTR pstr)
{

	if (vpath.size() <= 0)
	{
		printf("�^�O����NULL�ł��BFILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return ERR_TAG_NAME;
	}

	HRESULT hr;

	// XML�f�[�^�ۑ��N���X	
	XmlData wrt = XmlData(&hr);
	if (FAILED(hr))
	{
		printf("Failed to instantiate an XML DOM.�@FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}

	// ���b�Z�[�W�^�O
	MessageTag msg = MessageTag(L"set");
	wrt.addMessageTag(&msg);

	//�@�J�e�S���[�^�O
	IElementPtr pChild = wrt.addChild(msg.getMsgPtr(), L"Data"); // ��P�����ɐe���w��
	IElementPtr pParent = pChild;

	//  �^�O
	vector<_bstr_t>::iterator it = vpath.begin();
	int max_idx = vpath.size() - 1; //���X�g�T�C�Y

	for (int i = 0; i < max_idx; i++)			//���X�g�̍\���v�f�̌������[�v
	{
		pParent = pChild;
		pChild = wrt.addChild(pParent, vpath.at(i));
	}
	pParent = pChild;
	ElementData child = ElementData(vpath.at(max_idx), pstr);
	child.setAttribute(L"t", valtype);
	if (array_size>0)
	{
		TCHAR idx[BASE64_BUFFER_SIZE];
		_stprintf_s(idx, _T("%d"), array_idx);
		child.setAttribute(L"i", idx);
	}
	IElementPtr pElem = wrt.addChild(pParent, &child);

	BSTR bstrXML;
	unsigned int dataID = SET_MAINDB;
	wrt.getXML(&bstrXML);
	addSetMsgData(dataID, &bstrXML);
	::SysFreeString(bstrXML);

	return S_OK;
}

//
// ���C��DB����v�f�̎擾�ydouble�z
//    vpath     �F�^�O���
//    idx       �F����i
//    value     �F�l
// 
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, LPCTSTR idx, double *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::R;
	int attTID = -1;

	HRESULT hr = getElement(L"get", SET_MAINDB, vpath, idx, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((double*)test_decode);

	return hr;

}

//
// ���C��DB����v�f�̎擾�ydouble�z
//    vpath     �F�^�O���
//    value     �F�l
// 
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, double *value)
{
	return getOneElement(vpath, NULL, value);
}
//
// ���C��DB����v�f�̎擾�yint�z
//    clientID  :�N���C�A���gID
//    vpath     �F�^�O���
//    idx       �F����i
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, LPCTSTR idx, int *value)
{

	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::I;
	int attTID = -1;

	HRESULT hr = getElement(L"get", SET_MAINDB, vpath, idx, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((int*)test_decode);

	return hr;

}

//
// ���C��DB����v�f�̎擾�yint�z
//    vpath     �F�^�O���
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, int *value)
{
	return getOneElement(vpath, NULL, value);
}
//
// ���C��DB����v�f�̎擾�yunsigned short�z
//    vpath     �F�^�O���
//    idx       �F����i
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, LPCTSTR idx, unsigned short *value)
{

	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::USI;
	int attTID = -1;

	HRESULT hr = getElement(L"get", SET_MAINDB, vpath, idx, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned short*)test_decode);

	return hr;

}
//
// ���C��DB����v�f�̎擾�yunsigned short�z
//    vpath     �F�^�O���
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, unsigned short *value)
{
	return getOneElement(vpath, NULL, value);
}

//
// ���C��DB����v�f�̎擾�y��������8�r�b�g�����^�z
//    vpath     �F�^�O���
//    idx       �F����i
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
//
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, LPCTSTR idx, unsigned char *value)
{

	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::X;
	int attTID = -1;

	HRESULT hr = getElement(L"get", SET_MAINDB, vpath, idx, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned char*)test_decode);

	return hr;

}
//
// ���C��DB����v�f�̎擾�y�u�[���㐔�^�z
//    vpath     �F�^�O���
//    idx       �F����i
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
//
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, LPCTSTR idx, char value[1])
{

	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::B;
	int attTID = -1;

	HRESULT hr = getElement(L"get", SET_MAINDB, vpath, idx, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decodeString(val, Base64Data::B, test_decode);
	value[0] = test_decode[0];

	return hr;

}
//
// ���C��DB����v�f�̎擾�y��������8�r�b�g�����^�z
//    vpath     �F�^�O���
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, unsigned char *value)
{
	return getOneElement(vpath, NULL, value);
}
//
//Ԫ��//��ȡ[˫]
//�ͻ���ID��ί�������֤
// VPATH����ǩ��Ϣ
// IDX��������
//ֵ��ֵ
//
//����ֵ��= S_OK��0��;�ɹ����
//��= S_FALSE��1��;�쳣��ֹ
//��= ERR_DATA_ATT_T��5��;�������Ͳ�ƥ��
//��= ERR_TAG_NAME��6��;������ǩ��0�Ĵ�С
//��= ERR_DATA_GET��7��;���ݻ�ȡ���
//��= ERR_ARRAY_SIZE��8��;��ӡ��С���ڻ������Ĵ�СԽ��
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, double *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::R;
	int attTID = -1;

	HRESULT hr = getElement(clientID, vpath, idx, val, &attTID);
	if (dataType != attTID)
	{
		printf("�������Ͳ�ƥ��\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("���ݲ��ܵõ���\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((double*)test_decode);

	return hr;

}
//
// ��v�f�̎擾�ydouble�z
//    clientID  :�N���C�A���gID
//    vpath     �F�^�O���
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, double *value)
{
	return getOneElement(clientID, vpath, NULL, value);
}
//
// ��v�f�̎擾�yint�z
//    clientID  :�N���C�A���gID
//    vpath     �F�^�O���
//    idx       �F����i
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, int *value)
{

	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::I;
	int attTID = -1;

	HRESULT hr = getElement(clientID, vpath, idx, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((int*)test_decode);

	return hr;

}
//
// ��v�f�̎擾�yint�z
//    clientID  :�N���C�A���gID
//    vpath     �F�^�O���
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, int *value)
{
	return getOneElement(clientID, vpath, NULL, value);
}

//
// ��v�f�̎擾�yunsigned short�z
//    clientID  :�N���C�A���gID
//    vpath     �F�^�O���
//    idx       �F����i
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, unsigned short *value)
{

	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::USI;
	int attTID = -1;

	HRESULT hr = getElement(clientID, vpath, idx, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned short*)test_decode);

	return hr;

}
//
// ��v�f�̎擾�yunsigned short�z
//    clientID  :�N���C�A���gID
//    vpath     �F�^�O���
//    value     �F�l
//
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, unsigned short *value)
{
	return getOneElement(clientID, vpath, NULL, value);
}

//
// ��v�f�̎擾�y��������8�r�b�g�����^�z
//    clientID  :�N���C�A���gID
//    vpath     �F�^�O���
//    idx       �F����i
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
//
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, unsigned char *value)
{

	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::X;
	int attTID = -1;

	HRESULT hr = getElement(clientID, vpath, idx, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned char*)test_decode);

	return hr;

}

//
// ��v�f�̎擾�y��������8�r�b�g�����^�z
//    clientID  :�N���C�A���gID
//    vpath     �F�^�O���
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, unsigned char *value)
{
	return getOneElement(clientID, vpath, NULL, value);
}

//
// ��v�f�̎擾�y�u�[���㐔�^�z
//    clientID  :�N���C�A���gID
//    vpath     �F�^�O���
//    attr      �F����(ELEMATTRIBUTES)
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
//
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, ELEMATTRIBUTES attr,
	char value[1])
{

	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::B;
	int attTID = -1;

	HRESULT hr = getElement(L"mainlink send", clientID, vpath, attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decodeString(val, Base64Data::B, test_decode);
	value[0] = test_decode[0];

	return hr;

}

//
// ��v�f�̎擾�y�u�[���㐔�^�z
//    clientID  :�N���C�A���gID
//    vpath     �F�^�O���
//    idx       �F����i
//    value     �F�l
//
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
//
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, char value[1])
{

	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::B;
	int attTID = -1;

	HRESULT hr = getElement(clientID, vpath, idx, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decodeString(val, Base64Data::B, test_decode);
	value[0] = test_decode[0];

	return hr;

}

//
// ���C��DB����v�f�̎擾�ydouble�z
//    vpath     �F�^�O���
//    attr      �F����(ELEMATTRIBUTES)
//    value     �F�l
// 
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, double *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::R;
	int attTID = -1;

	HRESULT hr = getElement(L"get", SET_MAINDB, vpath, attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((double*)test_decode);

	return hr;

}

//
// ���C��DB����v�f�̎擾�yint�z
//    vpath     �F�^�O���
//    attr      �F����(ELEMATTRIBUTES)
//    value     �F�l
// 
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, int *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::I;
	int attTID = -1;

	HRESULT hr = getElement(L"get", SET_MAINDB, vpath, attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((int*)test_decode);

	return hr;

}
//
// ���C��DB����v�f�̎擾�yunsigned short�z
//    vpath     �F�^�O���
//    attr      �F����(ELEMATTRIBUTES)
//    value     �F�l
// 
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, unsigned short *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::USI;
	int attTID = -1;

	HRESULT hr = getElement(L"get", SET_MAINDB, vpath, attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned short*)test_decode);

	return hr;

}
//
// ���C��DB����v�f�̎擾�yunsigned char�z
//    vpath     �F�^�O���
//    attr      �F����(ELEMATTRIBUTES)
//    value     �F�l
// 
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, unsigned char *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::X;
	int attTID = -1;

	HRESULT hr = getElement(L"get", SET_MAINDB, vpath, attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned char*)test_decode);

	return hr;

}
//
// ���C��DB����v�f�̎擾�y�u�[���㐔�^�z
//    vpath     �F�^�O���
//    attr      �F����(ELEMATTRIBUTES)
//    value     �F�l    
// 
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, char value[1])
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::B;
	int attTID = -1;

	HRESULT hr = getElement(L"get", SET_MAINDB, vpath, attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decodeString(val, Base64Data::B, test_decode);
	value[0] = test_decode[0];

	return hr;

}
//
//
//  �߂�l      : =S_OK       (0) ;����I��
//              : =S_FALSE    (1) ;�ُ�I��
//              : =ERR_TAG_NAME(3);�^�O���̃T�C�Y��0
//			    : =DATA_GET_ERR   ;�f�[�^�擾�G���[
//			    : =ARRAY_SIZE_ERR ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
//
HRESULT dataManage::getElement(LPCTSTR type, unsigned int clientID, vector<_bstr_t>vpath,
	ELEMATTRIBUTES attr, LPTSTR result, int* attTID)
{
	if (vpath.size() <= 0)
	{
		printf("�^�O����NULL�ł��BFILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return ERR_TAG_NAME;
	}

	// XML�f�[�^�ۑ��N���X	
	HRESULT hr = S_FALSE;
	XmlData wrt = XmlData(&hr);
	if (FAILED(hr))
	{
		printf("Failed to instantiate an XML DOM.�@FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}

	// ���b�Z�[�W�^�O
	MessageTag msg = MessageTag(type);
	wrt.addMessageTag(&msg);

	//�@�J�e�S���[�^�O
	IElementPtr pChild = wrt.addChild(msg.getMsgPtr(), L"Data"); // ��P�����ɐe���w��
	pChild->setAttribute("encoding", "base64");
	IElementPtr pParent = pChild;

	//  �^�O
	vector<_bstr_t>::iterator it = vpath.begin();
	int max_idx = vpath.size() - 1; //���X�g�T�C�Y

	_bstr_t path = L"/root/message/Data/";

	for (int i = 0; i < max_idx; i++)			//���X�g�̍\���v�f�̌������[�v
	{
		pParent = pChild;
		pChild = wrt.addChild(pParent, vpath.at(i));
		path += (vpath.at(i) + L"/");
	}
	path += vpath.at(max_idx);

	pParent = pChild;
	ElementData child = ElementData(vpath.at(max_idx), NULL);
	if ((attr.i) != _bstr_t(L"")) //NULL�łȂ�������
	{
		child.setAttribute(L"i", attr.i);
	}
	child.setAttribute(L"a", attr.a);
	child.setAttribute(L"c", attr.c);
	child.setAttribute(L"tm", attr.tm);

	child.setAttribute(L"t", NULL); //�_�~�[�f�[�^������
	IElementPtr pElem = wrt.addChild(pParent, &child);

	hr = getElementData(&wrt, clientID, path, result, attTID);

	return hr;
}
//
//
//  �߂�l      : =S_OK       (0) ;����I��
//              : =S_FALSE    (1) ;�ُ�I��
//              : =ERR_TAG_NAME(3);�^�O���̃T�C�Y��0
//			    : =DATA_GET_ERR   ;�f�[�^�擾�G���[
//			    : =ARRAY_SIZE_ERR ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
//
HRESULT dataManage::getElement(unsigned int dbID, vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, LPTSTR result, int* attTID)
{
	if (vpath.size() <= 0)
	{
		printf("�^�O����NULL�ł��BFILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return ERR_TAG_NAME;
	}

	// XML�f�[�^�ۑ��N���X	
	HRESULT hr = S_FALSE;
	XmlData wrt = XmlData(&hr);
	if (FAILED(hr))
	{
		printf("Failed to instantiate an XML DOM.�@FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}

	//���[�g�|�C���^
	IElementPtr pRoot = wrt.getRootPtr();
	_bstr_t path = L"/root/";

	//message�^�O��ǉ�
	IElementPtr pMsg = wrt.addChild(pRoot, vpath.at(0));  //Message�^�O
	pMsg->setAttribute(TYPE_KY, L"get");       //Message Type��ݒ�
	pMsg->setAttribute(L"a", msg_attr.a);  //Message�^�O��a������ݒ�
	path += (vpath.at(0) + L"/");

	//�@�J�e�S���[�^�O
	IElementPtr pChild = wrt.addChild(pMsg, vpath.at(1)); // ��P�����ɐe���w��
	pChild->setAttribute("encoding", "base64");
	IElementPtr pParent = pChild;
	path += (vpath.at(1) + L"/");

	//  �^�O
	vector<_bstr_t>::iterator it = vpath.begin();
	int max_idx = vpath.size() - 1; //���X�g�T�C�Y

	for (int i = 2; i < max_idx; i++)			//���X�g�̍\���v�f�̌������[�v
	{
		pParent = pChild;
		pChild = wrt.addChild(pParent, vpath.at(i));
		path += (vpath.at(i) + L"/");
	}
	path += vpath.at(max_idx);

	pParent = pChild;
	ElementData child = ElementData(vpath.at(max_idx), NULL);
	if ((attr.i) != _bstr_t(L"")) //NULL�łȂ�������
	{
		child.setAttribute(L"i", attr.i);
	}
	child.setAttribute(L"a" , attr.a);
	child.setAttribute(L"c" , attr.c);
	child.setAttribute(L"tm", attr.tm);

	child.setAttribute(L"t", NULL); //�_�~�[�f�[�^������
	IElementPtr pElem = wrt.addChild(pParent, &child);

	//get
	hr = getElementData(&wrt, dbID, path, result, attTID);

	return hr;
}

//
//
//  �߂�l      : =S_OK       (0) ;����I��
//              : =S_FALSE    (1) ;�ُ�I��
//              : =ERR_TAG_NAME(3);�^�O���̃T�C�Y��0
//			    : =DATA_GET_ERR   ;�f�[�^�擾�G���[
//			    : =ARRAY_SIZE_ERR ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
//
HRESULT dataManage::getElementData(XmlData *xml, unsigned int dbID, BSTR path, LPTSTR result, int* attTID)
{
	HRESULT hr = S_OK;
	BSTR bstrXML, bstrResult;
	xml->getXML(&bstrXML);

	getData(dbID, bstrXML, path, &bstrResult, attTID);
	TCHAR szFinal[BASE64_BUFFER_SIZE];
	_tcscpy_s(szFinal, BASE64_BUFFER_SIZE, bstrResult);

	if (_tcslen(szFinal) <= BASE64_BUFFER_SIZE)
	{
		_tcscpy_s(result, BASE64_BUFFER_SIZE, bstrResult);
	}
	else if (_tcslen(szFinal) == 0)
	{
		printf("�f�[�^���擾�ł��܂���ł��� FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		hr = ERR_DATA_GET;
	}
	else
	{
		printf("�z��T�C�Y���R�s�[���T�C�Y�������ł� FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		hr = ERR_ARRAY_SIZE;
	}

	::SysFreeString(bstrXML);
	::SysFreeString(bstrResult);

	return hr;
}

//
// ���C��DB����v�f�̎擾�ydouble�z
//    vpath     �F�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    value     �F�l
// 
//  �߂�l      : =S_OK          (0);����I��
//              : =S_FALSE       (1);�ُ�I��
//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, double *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::R;
	int attTID = -1;
	unsigned int dbID;

	//message��a��������DB������
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((double*)test_decode);

	return hr;
}
//
// ���C��DB����v�f�̎擾�yint�z
//    vpath     �F�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    value     �F�l
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, int *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::I;
	int attTID = -1;
	unsigned int dbID;

	//message��a��������DB������
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((int*)test_decode);

	return hr;
}
//
// ���C��DB����v�f�̎擾�yunsigned short�z
//    vpath     �F�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    value     �F�l
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned short *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::USI;
	int attTID = -1;
	unsigned int dbID;

	//message��a��������DB������
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned short*)test_decode);

	return hr;
}

//
// ���C��DB����v�f�̎擾�yunsigned char�z
//    vpath     �F�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    value     �F�l
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned char *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::X;
	int attTID = -1;
	unsigned int dbID;

	//message��a��������DB������
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned char*)test_decode);

	return hr;
}
//
// ���C��DB����v�f�̎擾�ychar�z
//    vpath     �F�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    value     �F�l
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, char value[1])
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::B;
	int attTID = -1;
	unsigned int dbID;

	//message��a��������DB������
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	Base64Data::decodeString(val, Base64Data::B, test_decode);
	value[0] = test_decode[0];

	return hr;
}

//
// ���C��DB����v�f�̎擾�ystring�z
//    vpath     �F�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    value     �F�l
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, LPTSTR value)
{
	TCHAR  val[BASE64_BUFFER_SIZE*10];

	int dataType = Base64Data::S;
	int attTID = -1;
	unsigned int dbID;

	//message��a��������DB������
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	//Base64Data::decodeString(val, Base64Data::S, test_decode);
	_tcscpy_s(value, BASE64_BUFFER_SIZE * 10, val);

	return hr;
}

HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, BSTR *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::R;
	int attTID = -1;
	unsigned int dbID;

	//message��a��������DB������
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	
	if (dataType != attTID)
	{
		printf("�f�[�^�^�C�v�̕s����\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("�f�[�^���擾�ł��܂���ł����B\n");
		return ERR_DATA_GET;
	}

	//Base64Data::decode(val, test_decode, &outsize);
	*value = val;

	return hr;
}

#pragma endregion get

#pragma region set
//
// ��v�f�̍X�V�ydouble�z
//    vpath      :�^�O���
//    array_size�F�z��T�C�Y
//    array_idx �F�z��C���f�b�N�X
//    value     �F�l
//
//  �߂�l      : =S_OK             (0) ;����I��
//              : =S_FALSE          (1) ;�ُ�I��
//              : =ERR_TAG_NAME     (3);�^�O���̃T�C�Y��0
//              : =ERR_BASE64_COVERT(6);base64�ϊ��G���[
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, double value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::R;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("R%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("R"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// ��v�f�̍X�V�yfloat�z
//    vpath      :�^�O���
//    array_size�F�z��T�C�Y
//    array_idx �F�z��C���f�b�N�X
//    value     �F�l
//
//  �߂�l      : =S_OK             (0) ;����I��
//              : =S_FALSE          (1) ;�ُ�I��
//              : =ERR_TAG_NAME     (3);�^�O���̃T�C�Y��0
//              : =ERR_BASE64_COVERT(6);base64�ϊ��G���[
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, float value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::SR;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("SR%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("SR"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// ��v�f�̍X�V�ylong double�z
//    vpath      :�^�O���
//    array_size�F�z��T�C�Y
//    array_idx �F�z��C���f�b�N�X
//    value     �F�l
//
//  �߂�l      : =S_OK             (0) ;����I��
//              : =S_FALSE          (1) ;�ُ�I��
//              : =ERR_TAG_NAME     (3);�^�O���̃T�C�Y��0
//              : =ERR_BASE64_COVERT(6);base64�ϊ��G���[
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, long double value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::LR;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("LR%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("LR"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// ��v�f�̍X�V�yint�z
//    vpath      :�^�O���
//    array_size�F�z��T�C�Y
//    array_idx �F�z��C���f�b�N�X
//    value     �F�l
//
//  �߂�l      : =S_OK             (0) ;����I��
//              : =S_FALSE          (1) ;�ُ�I��
//              : =ERR_TAG_NAME     (3);�^�O���̃T�C�Y��0
//              : =ERR_BASE64_COVERT(6);base64�ϊ��G���[
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, int value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::I;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("I%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("I"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// ��v�f�̍X�V�yshort�z
//    vpath      :�^�O���
//    array_size�F�z��T�C�Y
//    array_idx �F�z��C���f�b�N�X
//    value     �F�l
//
//  �߂�l      : =S_OK             (0) ;����I��
//              : =S_FALSE          (1) ;�ُ�I��
//              : =ERR_TAG_NAME     (3);�^�O���̃T�C�Y��0
//              : =ERR_BASE64_COVERT(6);base64�ϊ��G���[
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, short value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::SI;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("SI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("SI"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// ��v�f�̍X�V�ylong long�z
//    vpath      :�^�O���
//    array_size�F�z��T�C�Y
//    array_idx �F�z��C���f�b�N�X
//    value     �F�l
//
//  �߂�l      : =S_OK             (0) ;����I��
//              : =S_FALSE          (1) ;�ُ�I��
//              : =ERR_TAG_NAME     (3);�^�O���̃T�C�Y��0
//              : =ERR_BASE64_COVERT(6);base64�ϊ��G���[
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, long long value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::SI;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("LI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("LI"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// ��v�f�̍X�V�yunsigned int�z
//    vpath      :�^�O���
//    array_size�F�z��T�C�Y
//    array_idx �F�z��C���f�b�N�X
//    value     �F�l
//
//  �߂�l      : =S_OK             (0) ;����I��
//              : =S_FALSE          (1) ;�ُ�I��
//              : =ERR_TAG_NAME     (3);�^�O���̃T�C�Y��0
//              : =ERR_BASE64_COVERT(6);base64�ϊ��G���[
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, unsigned int value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::SI;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("UI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("UI"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// ��v�f�̍X�V�yunsigned short int�z
//    vpath      :�^�O���
//    array_size�F�z��T�C�Y
//    array_idx �F�z��C���f�b�N�X
//    value     �F�l
//
//  �߂�l      : =S_OK             (0) ;����I��
//              : =S_FALSE          (1) ;�ُ�I��
//              : =ERR_TAG_NAME     (3);�^�O���̃T�C�Y��0
//              : =ERR_BASE64_COVERT(6);base64�ϊ��G���[
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, unsigned short int value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::USI;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("USI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("USI"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// ��v�f�̍X�V�yunsigned long long�z
//    vpath      :�^�O���
//    array_size�F�z��T�C�Y
//    array_idx �F�z��C���f�b�N�X
//    value     �F�l
//
//  �߂�l      : =S_OK             (0) ;����I��
//              : =S_FALSE          (1) ;�ُ�I��
//              : =ERR_TAG_NAME     (3);�^�O���̃T�C�Y��0
//              : =ERR_BASE64_COVERT(6);base64�ϊ��G���[
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, unsigned long long value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::ULI;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("ULI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("ULI"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// ��v�f�̍X�V�y�����Ȃ�8�r�b�g�����^�z
//    vpath      :�^�O���
//    array_size�F�z��T�C�Y
//    array_idx �F�z��C���f�b�N�X
//    value     �F�l
//
//  �߂�l      : =S_OK             (0) ;����I��
//              : =S_FALSE          (1) ;�ُ�I��
//              : =ERR_TAG_NAME     (3);�^�O���̃T�C�Y��0
//              : =ERR_BASE64_COVERT(6);base64�ϊ��G���[
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, unsigned char value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::X;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("X%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("X"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
//
//  �߂�l      : =S_OK        (0) ;����I��
//              : =S_FALSE     (1) ;�ُ�I��
//              : =ERR_TAG_NAME(3);�^�O���̃T�C�Y��0
//
HRESULT dataManage::setElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, LPCTSTR valtype, LPCTSTR pstr)
{
	if (vpath.size() <= 0)
	{
		printf("�^�O����NULL�ł��BFILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return ERR_TAG_NAME;
	}

	HRESULT hr;

	// XML�f�[�^�ۑ��N���X	
	XmlData wrt = XmlData(&hr);
	if (FAILED(hr))
	{
		printf("Failed to instantiate an XML DOM.�@FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}

	//���[�g�|�C���^
	IElementPtr pRoot = wrt.getRootPtr();

	//message�^�O��ǉ�
	IElementPtr pMsg = wrt.addChild(pRoot, vpath.at(0));	//Message�^�O
	pMsg->setAttribute(TYPE_KY, msg_attr.type);				//Message Type��ݒ�
	if(msg_attr.name  != _bstr_t(L"")) pMsg->setAttribute(L"name", msg_attr.name);      //Message name��ݒ�
	if(msg_attr.cycle != _bstr_t(L"")) pMsg->setAttribute(L"cycle", msg_attr.cycle);    //Message cycle��ݒ�
	if(msg_attr.a     != _bstr_t(L"")) pMsg->setAttribute(L"a", msg_attr.a);			//Message�^�O��a������ݒ�

	//�@�J�e�S���[�^�O
	IElementPtr pChild = wrt.addChild(pMsg, vpath.at(1)); // ��P�����ɐe���w��
	IElementPtr pParent = pChild;

	//  �^�O
	vector<_bstr_t>::iterator it = vpath.begin();
	int max_idx = vpath.size() - 1; //���X�g�T�C�Y

	for (int i = 2; i < max_idx; i++)			//���X�g�̍\���v�f�̌������[�v
	{
		pParent = pChild;
		pChild = wrt.addChild(pParent, vpath.at(i));
	}
	pParent = pChild;
	ElementData child = ElementData(vpath.at(max_idx), pstr);
	child.setAttribute(L"t", valtype);
	if (array_size>0)
	{
		child.setAttribute(L"i", attr.i);
	}
	if(attr.a  != _bstr_t(L"")) child.setAttribute(L"a" , attr.a);
	if(attr.c  != _bstr_t(L"")) child.setAttribute(L"c" , attr.c);
	if(attr.tm != _bstr_t(L"")) child.setAttribute(L"tm", attr.tm);
	IElementPtr pElem = wrt.addChild(pParent, &child);

	BSTR bstrXML;
	unsigned int dataID = SET_MAINDB;
	wrt.getXML(&bstrXML);
	addSetMsgData(dataID, &bstrXML);
	::SysFreeString(bstrXML);

	return S_OK;
}
////
////
////  �߂�l      : =S_OK        (0) ;����I��
////              : =S_FALSE     (1) ;�ُ�I��
////              : =ERR_TAG_NAME(3);�^�O���̃T�C�Y��0
////
//HRESULT dataManage::setElement(vector<_bstr_t>vpath, vector<ELEMATTRIBUTES>attr, MSGATTRIBUTES msg_attr, unsigned int array_size, LPCTSTR valtype, LPCTSTR pstr)
//{
//	if (vpath.size() <= 0)
//	{
//		printf("�^�O����NULL�ł��BFILE = %s , LINE = %d\n", __FILE__, __LINE__);
//		return ERR_TAG_NAME;
//	}
//
//	HRESULT hr;
//
//	// XML�f�[�^�ۑ��N���X	
//	XmlData wrt = XmlData(&hr);
//	if (FAILED(hr))
//	{
//		printf("Failed to instantiate an XML DOM.�@FILE = %s , LINE = %d\n", __FILE__, __LINE__);
//		return S_FALSE;
//	}
//
//	//���[�g�|�C���^
//	IElementPtr pRoot = wrt.getRootPtr();
//
//	//message�^�O��ǉ�
////	IElementPtr pMsg = wrt.addChild(pRoot, vpath.at(0));  //Message�^�O
//	IElementPtr pMsg = wrt.addChild(pRoot, L"message");  //Message�^�O
//	pMsg->setAttribute(TYPE_KY, msg_attr.type);       //Message Type��ݒ�
//	pMsg->setAttribute(L"name", msg_attr.name);       //Message name��ݒ�
//	pMsg->setAttribute(L"cycle", msg_attr.cycle);       //Message cycle��ݒ�
////	pMsg->setAttribute(L"a", attr.at(0).a);  //Message�^�O��a������ݒ�
//	pMsg->setAttribute(L"a", msg_attr.a);  //Message�^�O��a������ݒ�
//
//	//�@�J�e�S���[�^�O
//	IElementPtr pChild = wrt.addChild(pMsg, vpath.at(0)); // ��P�����ɐe���w��
//	pChild->setAttribute(L"a", attr.at(0).a);  //�^�O��a������ݒ�
//	IElementPtr pParent = pChild;
//
//	//  �^�O
//	vector<_bstr_t>::iterator it = vpath.begin();
//	int max_idx = vpath.size() - 1; //���X�g�T�C�Y
//
////	for (int i = 2; i < max_idx; i++)			//���X�g�̍\���v�f�̌������[�v
//	for (int i = 1; i < max_idx; i++)			//���X�g�̍\���v�f�̌������[�v
//	{
//		pParent = pChild;
//		pChild = wrt.addChild(pParent, vpath.at(i));
//		pChild->setAttribute(L"a", attr.at(i).a);  //�^�O��a������ݒ�
//	}
//	pParent = pChild;
//	ElementData child = ElementData(vpath.at(max_idx), pstr);
//	child.setAttribute(L"t", valtype);
//	if (array_size>0)
//	{
//		TCHAR idx[BASE64_BUFFER_SIZE];
//		_stprintf_s(idx, _T("%d"), attr.at(max_idx).i);
//		child.setAttribute(L"i", idx);
//	}
//	child.setAttribute(L"a", attr.at(max_idx).a);
//	child.setAttribute(L"c", attr.at(max_idx).c);
//	IElementPtr pElem = wrt.addChild(pParent, &child);
//
//	BSTR bstrXML;
//	unsigned int dataID = SET_MAINDB;
//	wrt.getXML(&bstrXML);
//	addSetMsgData(dataID, &bstrXML);
//	::SysFreeString(bstrXML);
//
//	return S_OK;
//}
//
// ��v�f�̍X�V�ydouble�z
//    vpath      :�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    array_size�F�z��T�C�Y
//    value     �F�l
//
//  �߂�l      : =S_OK             (0) ;����I��
//              : =S_FALSE          (1) ;�ُ�I��
//              : =ERR_TAG_NAME     (3);�^�O���̃T�C�Y��0
//              : =ERR_BASE64_COVERT(6);base64�ϊ��G���[
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, double value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::R;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("R%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("R"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// ��v�f�̍X�V�yfloat�z
//    vpath      :�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    array_size�F�z��T�C�Y
//    value     �F�l
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, float value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::SR;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("SR%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("SR"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// ��v�f�̍X�V�ylong double�z
//    vpath      :�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    array_size�F�z��T�C�Y
//    value     �F�l
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, long double value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::LR;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("LR%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("LR"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// ��v�f�̍X�V�yint�z
//    vpath      :�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    array_size�F�z��T�C�Y
//    value     �F�l
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, int value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::I;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("I%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("I"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// ��v�f�̍X�V�yshort�z
//    vpath      :�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    array_size�F�z��T�C�Y
//    value     �F�l
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, short value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::SI;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("SI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("SI"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// ��v�f�̍X�V�ylong long�z
//    vpath      :�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    array_size�F�z��T�C�Y
//    value     �F�l
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, long long value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::LI;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("LI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("LI"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// ��v�f�̍X�V�yunsigned int�z
//    vpath      :�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    array_size�F�z��T�C�Y
//    value     �F�l
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, unsigned int value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::UI;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("UI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("UI"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// ��v�f�̍X�V�yunsigned short int�z
//    vpath      :�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    array_size�F�z��T�C�Y
//    value     �F�l
//(	GetXmlPath("Ctrl", "Rotor"), 
//::setAttributeACI(NULL, NULL, L"0"),
//setmsg_attr, 6, uw );
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, unsigned short int value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::USI;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("USI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("USI"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// ��v�f�̍X�V�yunsigned long long�z
//    vpath      :�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    array_size�F�z��T�C�Y
//    value     �F�l
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, unsigned long long value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::ULI;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("ULI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("ULI"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// ��v�f�̍X�V�yunsigned char�z
//    vpath      :�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    array_size�F�z��T�C�Y
//    value     �F�l
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, unsigned char value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::X;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("X%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("X"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}

//
// ��v�f�̍X�V�y char�z
//    vpath      :�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    array_size�F�z��T�C�Y
//    value     �F�l
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, char value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::B;

	if (array_size > 0)	//����t�ݒ�
	{
		_stprintf_s(valtype, _T("B%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("B"));
	}

	//�@base64�ϊ�
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//�ϊ�FAIL
	}

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}

//
// ��v�f�̍X�V�ystring�z
//    vpath      :�^�O��� message����w��
//    attr      �F����(ELEMATTRIBUTES) message����w��
//    array_size�F�z��T�C�Y
//    value     �F�l
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, LPCTSTR value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::S;

	_tcscpy_s(valtype, _T("S"));

	//�@�v�f��ǉ�
	HRESULT hr = setElement(vpath, attr, msg_attr, 0, valtype, value);
	return hr;
}
#pragma endregion set

//
// DB��ID��ݒ肷��
//    attr_a    �Fmessage�^�O��a�����̒l
unsigned int dataManage::setDBID(BSTR attr_a)
{
	if (attr_a == NULL)
	{
		return SET_MAINDB;
	}
	else
	{
		//BSTR->string
		char* str = _com_util::ConvertBSTRToString(attr_a);
		unsigned int no = 0;
		map<unsigned int, string>::iterator it = dbIDMap.begin();
		while (it != dbIDMap.end())
		{
			if (strlen(str) == strlen((it->second).c_str())) //�������v
			{
				if (strcmp((it->second).c_str(), str) == 0)
				{
					delete[] str;
					return it->first;
				}
			}

			++it;
			++no;
		}
		//ID���o�^����Ă��Ȃ��̂ŁAMap�ɓo�^����
		dbIDMap.insert(make_pair(no, str));    //�߂�l���݂Ă���return�����ق����悢���H
		delete[] str;
		return no;
	}
}

//
// DB��ID��ݒ肷��
//    xml    �Fxml message
unsigned int dataManage::setDBID(XmlMessage *xml)
{
	unsigned int dbID;

	IElementPtr pMsgElem = xml->pDoc->documentElement->GetchildNodes()->Getitem(0); //msgNo��0�ŌŒ肵�Ă��܂��Ă���
	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pMsgElem->Getattributes();
	_bstr_t attA;  //message�^�O��a����
	VARIANT_BOOL bl = getAttA(attNodePtr, &attA);
	dbID = setDBID(attA);

	return dbID;
}

// 
//�@set���b�Z�[�W�̃N���A
//
//  �@dataID �F�f�[�^ID
//  �@msgID  �F���b�Z�[�WID
//
//    �߂�l    : =VARIANT_TRUE; clear
//              : =VARIANT_FALSE; clear
//
VARIANT_BOOL dataManage::clearSetMsg(unsigned int dataID, int msgID)
{
	VARIANT_BOOL result = VARIANT_FALSE;

	vector<XmlMessage>::iterator it = setList.data.begin();

	for (unsigned int i = 0; i<setList.data.size(); i++)
	{
		if (it->getDataID() == dataID)
		{
			MSXML2::IXMLDOMDocument2Ptr pXMLDom = it->pDoc;

			//���[�g
			IElementPtr  pRoot = pXMLDom->documentElement;
			if (pRoot->hasChildNodes())
			{
				//message
				MSXML2::IXMLDOMNodeListPtr pMessage = pRoot->GetchildNodes();
				for (int iMsg = 0; iMsg < pMessage->Getlength(); iMsg++)
				{
					IElementPtr pMsgElem = pMessage->Getitem(iMsg);
					if (pMsgElem->hasChildNodes())
					{
						// msssage�̎q�m�[�h
						MSXML2::IXMLDOMNodeListPtr pData = pMsgElem->GetchildNodes();
						int nData = pData->Getlength();
						for (int iData = 0; iData < nData; iData++)
						{
							IElementPtr pDataElem = pData->Getitem(iData);
							if (pDataElem->hasChildNodes())
							{
								MSXML2::IXMLDOMNodeListPtr pData2 = pDataElem->GetchildNodes();
								while (pData2->Getlength()>0)
								{
									IElementPtr pDataElem2 = pData2->Getitem(0);
									pDataElem->removeChild(pDataElem2);//�N���A
								}
							}
						}
					}
				}
			}

			result = VARIANT_TRUE;
		}
		it++;
	}

	return result;
}

