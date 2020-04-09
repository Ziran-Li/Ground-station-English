#include "stdafx.h"
#include <tchar.h>
#include <vector> 

#include "DataManage.h"

//
// 摨偠ID偺傕偺偑偁傞偐僠僃僢僋
//
//  丂dataID   丗ID
//  丂list     丗dataDB僆僽僕僃僋僩偺億僀儞僞
//  丂listIdx  : 摨偠ID傪帩偮梫慺偺僀儞僨僢僋僗
//
//    栠傝抣    : =VARIANT_TRUE; 摨偠ID桳傝
//              : =VARIANT_FALSE;摨偠ID柍偟
//
VARIANT_BOOL dataManage::hasSameID(int dataID, class dataDB list, unsigned int *listIdx)
{
	int size = list.data.size(); //儕僗僩僒僀僘

	if(!list.data.empty())		//儕僗僩偼嬻偱偼側偄丠
	{

		vector<XmlMessage>::iterator it = list.data.begin(); 

		for(int i=0; i < size; i++)			//儕僗僩偺峔惉梫慺偺屄悢暘儖乕僾
		{
			if(it->getDataID() == dataID)	//摨偠僋儔僀傾儞僩ID偺傕偺偑偁傞丠
			{
				*listIdx = i;				//梫慺僀儞僨僢僋僗傪戙擖

				return VARIANT_TRUE;	
			}
			++it;
		}
	}
		
	return VARIANT_FALSE;	

}

// 
// 摨偠name懏惈偺傕偺偑偁傞偐僠僃僢僋
//
//  丂msgName 丗name
//  丂archive 丗get儊僢僙乕僕儕僗僩
//  丂listIdx 丗摨偠name傪帩偮梫慺偺僀儞僨僢僋僗
//
//    栠傝抣    : =VARIANT_TRUE; 摨偠name懏惈桳傝
//              : =VARIANT_FALSE;摨偠name懏惈柍偟
//
VARIANT_BOOL dataManage::hasSameName(BSTR msgName, XmlMessage *archive, unsigned int *listIdx)
{

	VARIANT_BOOL bMatch = VARIANT_FALSE;
	
	return archive->hasSameName(msgName, listIdx);	
}

// 
//丂僨乕僞曐懚
//
//  丂xml      : XmlMessage僆僽僕僃僋僩偺億僀儞僞
//	  dataID   : ID
//
void dataManage::saveData(XmlMessage *xml, unsigned int dataID)
{
	unsigned int listIdx;

	int msgID = 0;		//TODO丗message偵偼1偮偺message偺傒偵尷掕丅屻偱奼挘
	int type  = xml->getMsgType(msgID);	//TODO:侾偮偺xml僨乕僞偺拞偵set偲get偺崿嵼偼側偄偲偺憐掕
								//偙偺憐掕偱椙偄偐妋擣昁梫丅傑偨丄xmlMessage偱load帪偵崿嵼帪偺僄儔乕張棟昁梫偐丠偦偙傑偱傗傞丠
	
	xml->setDataID(dataID);

	switch(type)
	{

	case XmlMessage::msgSet:	// set儊僢僙乕僕

		if(hasSameID(dataID,setList, &listIdx)) //摨偠ID偑偁傞丠
		{
			vector<XmlMessage>::iterator it = setList.data.begin();
			for (unsigned int i = 0; i<listIdx; i++) it++;
			_bstr_t name = xml->list_name.at(msgID);
			XmlMessage archive = *it;

			if (hasSameName(name, &archive, &listIdx))//摨偠name懏惈偑偁傞丠
			{
				//偁偭偨傜廋惓  
				BSTR xmlMsg = xml->pDoc->Getxml();
				addSetMsgData(dataID, &xmlMsg);
				::SysFreeString(xmlMsg);
			}
			else
			{
				//側偐偭偨傜捛壛
				setList.data.push_back(*xml); 
			}
			
		}
		else
		{
			//柍偗傟偽捛壛
			setList.data.push_back(*xml);
		}
		break;

	case XmlMessage::msgGet:	// get儊僢僙乕僕

		if(hasSameID(dataID, getList, &listIdx)) //摨偠僋儔僀傾儞僩ID偑偁傞丠
		{						
			//name懏惈偑摨偠傕偺偑偁傞丠
			vector<XmlMessage>::iterator it = getList.data.begin();
			for (unsigned int i = 0; i<listIdx; i++) it++;
			_bstr_t name = xml->list_name.at(msgID); //TODO:杮棃儊僢僙乕僕屄悢暘儖乕僾偑昁梫
			XmlMessage archive = *it;

			if (hasSameName(name, &archive, &listIdx))//摨偠name懏惈偑偁傞丠
			{
				int tmp = archive.getMsgType(msgID);
				if(archive.getMsgType(msgID) == XmlMessage::msgClose)
				{
					//msgType慜夞抣偑close側傜偽丄僨乕僞傪嶍彍偟偰偐傜msgType=get偱捛壛偡傞
					it = getList.data.erase(it);		// it偺埵抲偺梫慺傪嶍彍
					getList.data.insert(it, *xml );     // it偺埵抲偵捛壛
				}
				else
				{
					//msgType慜夞抣偑get側傜偽儊僢僙乕僕傪廋惓
					BSTR xmlMsg = xml->pDoc->Getxml();
					addSetMsgData(dataID, &xmlMsg);
					::SysFreeString(xmlMsg);
				}
			}
			else
			{
				//側偐偭偨傜捛壛
				getList.data.push_back(*xml); 
			}
		}
		else //
		{
			getList.data.push_back(*xml);								
		}
		break;

	case XmlMessage::msgClose:	//close儊僢僙乕僕

		if(hasSameID(dataID, getList, &listIdx)) //摨偠僋儔僀傾儞僩ID偑偁傞丠
		{

			vector<XmlMessage>::iterator it = getList.data.begin();
			for(unsigned int i =0; i<listIdx; i++) it++;
			_bstr_t name   = xml->list_name.at(msgID);
			XmlMessage archive = *it;

			if(hasSameName(name, &archive ,&listIdx))//摨偠name懏惈偑偁傞丠
			{
				MSXML2::IXMLDOMNodePtr pMsg =archive.pDoc->selectSingleNode(L"/root/message");

				MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pMsg->Getattributes();
				MSXML2::IXMLDOMNodePtr pType = attNodePtr->getNamedItem("type");

				pType->put_text(L"close");			//message僞僀僾傪close偵曄峏
				it->setMsgType(msgID, XmlMessage::msgClose);
			}	
		}
	
		break;

	case XmlMessage::msgMainLink:	// main link 儊僢僙乕僕

		if (hasSameID(dataID, mainLinkList, &listIdx)) //摨偠僋儔僀傾儞僩ID偑偁傞丠
		{
			//摨偠ID偺僨乕僞傪嶍彍偟偰偐傜捛壛偡傞
			vector<XmlMessage>::iterator it = mainLinkList.data.begin();
			for(unsigned int i =0; i<listIdx; i++) it++;
			it = mainLinkList.data.erase(it);		// it偺埵抲偺梫慺傪嶍彍
			mainLinkList.data.insert(it, *xml);     // it偺埵抲偵捛壛
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
// get儊僢僙乕僕偵婎偯偄偰僨乕僞庢摼
//
//   pParent    丗梫慺億僀儞僞
//   str_query丂丗get忦審
//   pLeaf      丗柤慜偑堦抳偟偨僲乕僪億僀儞僞
//
//
//    栠傝抣    : =VARIANT_TRUE; 僨乕僞庢摼惉岟
//              : =VARIANT_FALSE; get儊僢僙乕僕偵懳墳偟偨僨乕僞側偟
//
VARIANT_BOOL dataManage::getSetData(MSXML2::IXMLDOMElementPtr pParent, struct getMsg *str_query, MSXML2::IXMLDOMElementPtr *pLeaf)
{

	vector<_bstr_t> path_parent = str_query->path;
	int path_final_idx       = str_query->path.size() - 1 ;
	_bstr_t  name;

	int idx = str_query->path_idx;

	MSXML2::IXMLDOMNodeListPtr pNodes = NULL;	

	if(idx < path_final_idx)	//巕僲乕僪偑偁傞丠
	{
		idx++; //vpath偺僀儞僨僢僋僗傪恑傔傞

		MSXML2::IXMLDOMNodeListPtr pNodes = pParent->GetchildNodes();
			
		long ll1 = pNodes->Getlength();
		for(int iNode = 0 ; iNode < pNodes->Getlength(); iNode++)	//巕僲乕僪偺悢偩偗儖乕僾
		{
			name = path_parent.at(idx);  

			MSXML2::IXMLDOMElementPtr pChild = pNodes->Getitem(iNode);//巕僲乕僪
			if(pChild != NULL)
			{
				_bstr_t temp = pChild->GetnodeName();
				if(_tcscmp(pChild->GetnodeName(),name) ==0)	//梫慺柤偑堦抳丠
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
		//堦抳偡傞傕偺偑柍偄偺偱捛壛 TODO:摦嶌妋擣傑偩

		if(str_query->mode == XmlMessage::modifiedSet)
		{
		
			MSXML2::IXMLDOMDocument2Ptr  pDoc =  pParent->GetownerDocument();

			name = path_parent.at(idx);  //捛壛
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
	
			MSXML2::IXMLDOMNamedNodeMapPtr map = str_query->attNodePtr; //懏惈
			if(map!=NULL)
			{						
				for(long i =0; i <map->Getlength(); i++)	//懏惈偺悢偩偗捛壛
				{				
					MSXML2::IXMLDOMNodePtr  pNode = map->Getitem(i);
					pElem->setAttribute(pNode->GetnodeName(), pNode->GetnodeValue());
				}
			}
			*pLeaf = pElem;
			return VARIANT_TRUE;
		}
	}
	else  //嵟壓憌偺僲乕僪
 	{

		//path偵婎偯偄偰僨乕僞庢摼		
		name      = path_parent.at(idx);  
		VARIANT_BOOL bMatch = TRUE;
		if(_tcscmp(pParent->GetnodeName(),name) ==0) //梫慺柤偑堦抳丠
		{
			//懏惈a偺堦抳惈妋擣
			_bstr_t attA1, attA2;
			VARIANT_BOOL bool1, bool2;
			MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr1, attNodePtr2;
			attNodePtr1 = str_query->attNodePtr;
			attNodePtr2 = pParent->Getattributes();

/*
			attNodePtr = str_query->attNodePtr;
			bool1 = getAttA(attNodePtr,&attA1);		//懏惈a抣庢摼乮get梫媮xml)
			attNodePtr = pParent->Getattributes();
			bool2 = getAttA(attNodePtr,&attA2);		//懏惈a抣庢摼乮db乯

			if( bool1 != bool2) return VARIANT_FALSE; //師傪専嶕
			if( attA1 != attA2) return VARIANT_FALSE; //師傪専嶕
*/


			//懏惈偺堦抳惈妋擣
			if (!matchAttrs(attNodePtr1, attNodePtr2, L"a"))  return VARIANT_FALSE; //師傪専嶕
			if (!matchAttrs(attNodePtr1, attNodePtr2, L"c"))  return VARIANT_FALSE; //師傪専嶕
			if (!matchAttrs(attNodePtr1, attNodePtr2, L"tm")) return VARIANT_FALSE; //師傪専嶕			
			//if (!matchAttrs(attNodePtr1, attNodePtr2, L"t"))  return VARIANT_FALSE; //師傪専嶕
			//懏惈i偺堦抳惈妋擣
			int idx1, idx2;
			bool1 = getAttIValue(attNodePtr1, &idx1);		//懏惈i抣庢摼乮get梫媮xml)
			bool2 = getAttIValue(attNodePtr2, &idx2);		//懏惈i抣庢摼乮db)

			if( bool1 != bool2) return VARIANT_FALSE; //師傪専嶕
			if( bool1 == VARIANT_FALSE)  //懏惈i偑側偄傕偺
			{
				*pLeaf = pParent;
				return VARIANT_TRUE;
			}
			if( idx1  == idx2 )			//懏惈i偑堦抳
			{
				*pLeaf = pParent;
				return VARIANT_TRUE;
			}
		}
	}

	return VARIANT_FALSE; // not found
	
}

// 
// 儊僢僙乕僕偺夝愅
//
//
//    栠傝抣    : =VARIANT_TRUE; 僨乕僞庢摼惉岟
//              : =VARIANT_FALSE; get儊僢僙乕僕偵懳墳偟偨僨乕僞側偟
//
VARIANT_BOOL dataManage::anaMessage(XmlMessage *setData, struct getMsg *str_query, 
									MSXML2::IXMLDOMElementPtr *pLeaf)
{

	int idx            = 0;
	_bstr_t  name;

	vector<_bstr_t> path  = str_query->path;	// get忦審
	int path_final_idx = path.size() - 1 ;	// 僲乕僪僷僗偺僀儞僨僢僋僗嵟戝抣


	MSXML2::IXMLDOMDocument2Ptr pXMLDom = setData->pDoc;		//set儊僢僙乕僕偺document億僀儞僞
	MSXML2::IXMLDOMElementPtr  pRoot   = pXMLDom->documentElement;//set儊僢僙乕僕偺儖乕僩億僀儞僞
		
	idx++;

	if(pRoot->hasChildNodes())									//儖乕僩捈壓偵巕僲乕僪偑偁傞丠
	{
		MSXML2::IXMLDOMNodeListPtr pMessage = pRoot->GetchildNodes();
		int msgNum = pMessage->Getlength();	
					
		for(int iMsg = 0; iMsg < msgNum; iMsg++)				//message偺悢偩偗儖乕僾
		{
			MSXML2::IXMLDOMElementPtr
				pMsgElem = pMessage->Getitem(iMsg);				//message梫慺

			name      = path.at(idx);  //get忦審梫慺柤

			if(_tcscmp(pMsgElem->GetnodeName(),name) ==0)		//梫慺柤偑堦抳丠丂
			{
				if(idx < path_final_idx) 
				{
					if(pMsgElem->hasChildNodes())				//巕僲乕僪偑偁傞丠
					{
						str_query->path_idx = idx;
						if (getSetData(pMsgElem, str_query, pLeaf))  //message僞僌偐傜搉偡乮Data僞僌柍懳墳乯
						{
							return VARIANT_TRUE;
						}
					}
					else
					{
						//奩摉偡傞僨乕僞偑側偄
					}
				}
			}
		}
	}

	return VARIANT_FALSE;	//	TODO:message梫慺偺壓偵偼彮側偔偲傕侾偮偺梫慺偑昁梫
	
}

//
//	text傪彂偒姺偊傞
//
void dataManage::putText(MSXML2::IXMLDOMElementPtr pParent, int encoding, struct getMsg str_query, BSTR str_temp)
{
			
	BSTR str   = str_temp;

	if(encoding == dataAttrEncoding::text)
	{
		IXMLDOMNodePtr attT= str_query.attNodePtr->getNamedItem("t"); //僨乕僞宆媦傃儀僋僩儖梫慺悢偺懏惈
				
	
		BSTR dataType =NULL;
				
		if (attT == NULL) //T懏惈
		{
			dataType = SysAllocString(L"R");	//僨僼僅儖僩傪愝掕
		}
		else
		{
			attT->get_text(&dataType);
		}
				
		//懏惈t ID
		Base64Data::convertBintoText(str_temp, dataType, &str);
		::SysFreeString(dataType);
	}
	pParent->put_text(str);	//xml偵斀塮
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
	case XmlMessage::replyMainLinkMsg: //mainLink send儊僢僙乕僕偵懳偡傞墳摎
		for (unsigned int i = 0; i < mainLinkList.data.size(); i++)
		{
			XmlMessage mainLinkData = mainLinkList.data.at(i);	
			if(mainLinkData.getDataID() == dataAttr->ID)
			{
				if (vbool=anaMessage(&mainLinkData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf))
				{
					break; //僨乕僞庢摼偟偨偺偱for暥傪敳偗傞
				}
			}
		}
		if(vbool) 
		{
			/*mainlink send儊僢僙乕僕偺t懏惈*/
			MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pLeaf->Getattributes();
			MSXML2::IXMLDOMNodePtr pAttT = attNodePtr->getNamedItem("t");

			if (pAttT != NULL)
			{
				/*request 偺t懏惈傪彂偒姺偊傞*/
				MSXML2::IXMLDOMNodePtr pReqAttT = str_query.attNodePtr->getNamedItem("t");
				pReqAttT->Puttext(pAttT->Gettext());
			}
			putText(pParent, dataAttr->encoding, str_query, pLeaf->Gettext());
		}

		break;


	case XmlMessage::replyGetMsg: //get儊僢僙乕僕偵懳偡傞墳摎 
			
		if (getAttSendValue(str_query.attNodePtr)) //懏惈 send偑enabled側傜偽
		{
			for (unsigned int i = 0; i < setList.data.size(); i++)
			{
				XmlMessage setData = setList.data.at(i);	
				if (vbool=anaMessage(&setData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf))
				{
					unsigned int dbID;
					//dbID偑偁偭偰偄傞偐妋擣
					dbID = setDBID(&setData);

					if (dataAttr->ID == dbID) //dbID偑偁偭偰偄偨傜敳偗傞
					{
						break; //僨乕僞庢摼偟偨偺偱for暥傪敳偗傞
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
					for (int iNode = 0; iNode < pNodes->Getlength(); iNode++)		//巕僲乕僪偺悢偩偗儖乕僾
					{
						MSXML2::IXMLDOMElementPtr pChild = pNodes->Getitem(iNode);  //巕
						if(pChild!=NULL)
						{
							//GET暥偱偼嵟廔僲乕僪偩偑丄pLeaf偱偼巕僲乕僪偑偁傞応崌
							appendDeepChild(pChild, dataAttr, pParent);
						}
						else if(iNode ==0)
						{
							/*t懏惈*/
							MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pLeaf->Getattributes();
							MSXML2::IXMLDOMNodePtr pAttT = attNodePtr->getNamedItem("t");
							if(pAttT!=NULL){
								/*request 偺t懏惈傪彂偒姺偊傞*/
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
					/*t懏惈*/
					MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pLeaf->Getattributes();
					MSXML2::IXMLDOMNodePtr pAttT = attNodePtr->getNamedItem("t");

					if(pAttT!=NULL)
					{
						/*request 偺t懏惈傪彂偒姺偊傞*/
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
				//尒偮偐傜側偄
				printf("%s 僨乕僞偑庢摼偱偒傑偣傫\n", (char*)str_query.path.at(str_query.path.size()-1));
			}
		}

		break;

	case XmlMessage::modifiedSet: //set儊僢僙乕僕忋彂偒

		for (unsigned int i = 0; i < setList.data.size(); i++)
		{			
			setData = setList.data.at(i);
			unsigned int dbID;
			//dbID偑偁偭偰偄傞偐妋擣
			dbID = setDBID(&setData);
			if (dataAttr->ID == dbID)
			{
				if (vbool=anaMessage(&setData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf))
				{
					break;//for暥傪敳偗傞
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
			// setData偺拞偵柍偄偺偱捛壛偡傞
			//
			//name懏惈偑摨偠setList傪扵偡
			unsigned int idx = 0;   //name堦抳偡傞傕偺偑尒偮偐傜側偐偭偨応崌偼MAIN偵捛壛偡傞
/*			for (unsigned int i = 0; i < getList.data.size(); i++)
			{
				XmlMessage getData = getList.data.at(i);
				_bstr_t nameL = getAttName(getData.pDoc, 0);
				_bstr_t nameP = getAttName(pParent->GetownerDocument(), 0);

				if (nameL == nameP)  //name偑堦抳
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
				if (dataAttr->ID == dbID) //dbID偑偁偭偰偄偨傜敳偗傞
				{
					idx = i;
					break; //僨乕僞庢摼偟偨偺偱for暥傪敳偗傞
				}

			}

			setData = setList.data.at(idx); 
			anaMessage(&setData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf);
			//pLeaf偼堦抳偟偨偲偙傠傑偱偺梫慺億僀儞僞
			MSXML2::IXMLDOMElementPtr pBranch = pLeaf;
			iStart = VARIANT_FALSE;
			
			for(size_t i = 0 ; i < str_query.path.size(); i++ )
			{
				_bstr_t name =   str_query.path.at(i);

				if(iStart == VARIANT_TRUE)
				{
					pDatas = setData.pDoc->createElement(name); //梫慺嶌惉

					MSXML2::IXMLDOMNamedNodeMapPtr map = pParent->Getattributes(); //懏惈
					if(map!=NULL)
					{						
						for(long i =0; i <map->Getlength(); i++)	//懏惈偺悢偩偗捛壛
						{				
							MSXML2::IXMLDOMNodePtr  pNode = map->Getitem(i);
							pDatas->setAttribute(pNode->GetnodeName(), pNode->GetnodeValue());
						}
					}
					pBranch->appendChild(pDatas);
					pBranch = pDatas;
				}

				if(_tcscmp(pLeaf->GetnodeName(),name) ==0)	//梫慺柤偑堦抳丠
				{
					iStart = VARIANT_TRUE; //						
				}
			}	
			pDatas->put_text(pParent->Gettext()); //僥僉僗僩傪捛婰
		}

		break;

	case XmlMessage::modifiedGet: //get儊僢僙乕僕忋彂偒

		iStart = VARIANT_FALSE;

		for (unsigned int i = 0; i < getList.data.size(); i++)
		{
			XmlMessage getData = getList.data.at(i);
			if (vbool=anaMessage(&getData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf))//TODO:偳偺懏惈偺堦抳惈傑偱尒傞偐丠
			{
				unsigned int dbID;
				//dbID偑偁偭偰偄傞偐妋擣
				dbID = setDBID(&getData);

				if (dataAttr->ID == dbID) //dbID偑偁偭偰偄偨傜敳偗傞
				{
					break; //僨乕僞庢摼偟偨偺偱for暥傪敳偗傞
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
				pLeaf->setAttribute("send", pAttr->GetnodeValue()); //TODO:懠偵彂偒姺偊傞傕偺偑偁傞偐妋擣
			}
		}
		else
		{
			// getData偺拞偵柍偄偺偱丄捛壛偡傞
			//
			IElementPtr pDatas  = NULL;
			VARIANT_BOOL iStart = VARIANT_FALSE;

			//name懏惈偑摨偠getList傪扵偡
			unsigned int idx = 0;   //name堦抳偡傞傕偺偑尒偮偐傜側偐偭偨応崌偼MAIN偵捛壛偡傞
			for (unsigned int i = 0; i < getList.data.size(); i++)
			{
				XmlMessage getData = getList.data.at(i);
				_bstr_t nameL = getAttName(getData.pDoc, 0);
				_bstr_t nameP = getAttName(pParent->GetownerDocument(), 0);
				if (nameL == nameP)  //name偑堦抳
				{
					idx = i;
					break;
				}
			}

			getData = getList.data.at(idx); 
			anaMessage(&getData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf);
			//pLeaf偼堦抳偟偨偲偙傠傑偱偺梫慺億僀儞僞
			MSXML2::IXMLDOMElementPtr pBranch = pLeaf;

			for(size_t i = 0 ; i < str_query.path.size(); i++ )
			{
				_bstr_t name =   str_query.path.at(i);

				if(iStart == VARIANT_TRUE)
				{
					pDatas = getData.pDoc->createElement(name); //梫慺嶌惉

					MSXML2::IXMLDOMNamedNodeMapPtr map = pParent->Getattributes(); //懏惈
					if(map!=NULL)
					{						
						for(long i =0; i <map->Getlength(); i++)	//懏惈偺悢偩偗捛壛
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
					if(_tcscmp(pLeaf->GetnodeName(),name) ==0)	//梫慺柤偑堦抳丠
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
// 丂巕僲乕僪専嶕
//
void dataManage::getInChild(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath, 
		struct dataAttribute *dataAttr, int mode)
{
	vector<_bstr_t> path_parent = vpath;
	vector<_bstr_t> path_temp;

	MSXML2::IXMLDOMNodeListPtr pNodes = NULL;

	if (pParent!= NULL && pParent->hasChildNodes())	//巕僲乕僪偑偁傞丠
	{
		MSXML2::IXMLDOMNodeListPtr pNodes = pParent->GetchildNodes();
		long size = pNodes->Getlength();
		for (int iNode = 0; iNode < size; iNode++)	 //巕僲乕僪偺悢偩偗儖乕僾
		{
			MSXML2::IXMLDOMElementPtr pNode = pNodes->Getitem(iNode);//巕僲乕僪
			if (pNode == NULL)
			{
				getLastBranchAttr(pParent, vpath, dataAttr, mode);
			}
			else
			{
				path_temp = path_parent; //恊僲乕僪偺壓偵捛壛偡傞偨傔丄偄偭偨傫恊僲乕僪傪僐僺乕
				path_temp.push_back(pNode->GetnodeName());

				if (mode == XmlMessage::modifiedGet)  //get儊僢僙乕僕忋彂偒偺応崌
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
						if (vbool = anaMessage(&getData, &str_query, (MSXML2::IXMLDOMElementPtr *)&pLeaf))//TODO:偳偺懏惈偺堦抳惈傑偱尒傞偐丠
						{
							break;//for暥傪敳偗傞
						}
					}
					if (vbool)
					{
						MSXML2::IXMLDOMAttributePtr pAttr = pNode->getAttributeNode("send");
						if (pAttr != NULL)
						{
							pLeaf->setAttribute("send", pAttr->GetnodeValue()); //巕僲乕僪偑懚嵼偡傞僲乕僪偺send懏惈傪斀塮偝偣傞
						}
					}
				}

				getInChild(pNode, path_temp, dataAttr, mode);	//

				path_temp.pop_back();
			}
			size = pNodes->Getlength();  //攝楍傪偽傜偡応崌側偳丄梫慺傪捛壛偡傞偙偲偑偁傞偺偱
		}
	}
	else						 //嵟壓憌偺僲乕僪
	{
		//lastBranch(pParent, vpath, dataAttr, mode);
		getLastBranchAttr(pParent, vpath, dataAttr, mode);
	}
}

// 
// Data僞僌偺懏惈愝掕
//
//
void dataManage::setDataTagAttr(MSXML2::IXMLDOMElementPtr elementPtr, struct dataAttribute* dataAttr)
{

	dataAttr->encoding = dataAttrEncoding::text;	//僨僼僅儖僩抣愝掕
	dataAttr->array_delimiter = dataAttrArray::tagged;	//僨僼僅儖僩抣愝掕

	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = elementPtr->Getattributes();

	//encoding懏惈
	MSXML2::IXMLDOMNodePtr encodePtr = attNodePtr->getNamedItem("encoding");						
	if(encodePtr != NULL)
	{
		_bstr_t encode = encodePtr->Gettext();
		if(_tcscmp(encode , L"base64")==0)
		{
			dataAttr->encoding = dataAttrEncoding::base64;
			
		}
	}


	//array懏惈
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
// get儊僢僙乕僕偺夝愅
//
//	result		:XML僨乕僞
//
//  栠傝抣      : =S_OK       (0) ;惓忢廔椆
//              : =S_FALSE    (1) ;堎忢廔椆
//              : =MSG_TYPE_NG(2) ;type僞僌偑晄惓
//
unsigned int  dataManage::setRequestData(XmlMessage *request, int DATAID)
{
	vector<_bstr_t> vpath, vpath_msg, vpath_root;	//梫慺偺奒憌儕僗僩

	MSXML2::IXMLDOMDocument2Ptr pXMLDom = request->pDoc;	
	IElementPtr  pRoot   = pXMLDom->documentElement;
		
	vpath_root.push_back(pRoot->GetnodeName());	//梫慺柤傪奒憌儕僗僩偵捛壛

	if(pRoot->hasChildNodes())	//儖乕僩捈壓偵巕僲乕僪乮message僲乕僪乯偑偁傞丠
	{
		MSXML2::IXMLDOMNodeListPtr pMessage = pRoot->GetchildNodes();
		int msgNum = pMessage->Getlength();	
					
		for(int iMsg = 0; iMsg < msgNum; iMsg++)			//message偺悢偩偗儖乕僾
		{
			IElementPtr pMsgElem = pMessage->Getitem(iMsg);	//message梫慺			
			int msgType = request->getMsgType(iMsg);

			if(msgType == XmlMessage::msgGet || msgType == XmlMessage::msgMainLink) 
			{
				vpath_msg = vpath_root;
				vpath_msg.push_back(pMsgElem->GetnodeName());	//梫慺柤傪奒憌儕僗僩偵捛壛

				//儊僢僙乕僕僞僀僾傪set偵曄偊傞
			    INodePtr ptr = pMsgElem->attributes->getNamedItem("type");
				ptr->put_text(L"set");

				if(pMsgElem->hasChildNodes())						//message捈壓偵巕僲乕僪偑偁傞丠
				{
					INodeListPtr pNodes = pMsgElem->GetchildNodes(); //Data梫慺
			
					for(int iNode = 0 ; iNode < pNodes->Getlength(); iNode++)	//巕僲乕僪偺悢偩偗儖乕僾
					{
						IElementPtr pChild = pNodes->Getitem(iNode);//message偺懛僲乕僪
						vpath = vpath_msg;
						vpath.push_back(pChild->GetnodeName());		//梫慺柤傪奒憌儕僗僩偵捛壛


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
				return MSG_TYPE_NG;	//儊僢僙乕僕偺type僞僌偑晄惓
			}
		}
	}

	return S_FALSE;	//	TODO:message梫慺偺壓偵偼彮側偔偲傕侾偮偺梫慺偑昁梫	
}

//
// get儊僢僙乕僕偵傛傞僨乕僞庢摼
//
//	DataID	    :setDB偺僨乕僞ID
//	result		:XML僨乕僞
//
//  栠傝抣      : =S_OK              (0) ;惓忢廔椆
//              : =S_FALSE           (1) ;堎忢廔椆
//
unsigned int dataManage::getMsgOnMainSetDB(unsigned int DataID,unsigned int clientID, BSTR *result)
{
	unsigned int match;
	BSTR  bstr;
	BSTR  xmlGetString;

	if(!hasSameID(clientID, getList, &match))	//摨偠ID偑偁傞丠
	{
		if(getDatabyDBID(DataID, &bstr))
		{
			printf("set儊僢僙乕僕偑偁傝傑偣傫 DATA_ID=%d, FILE=%s, LINE=%d",
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
				printf("xml暥傪庢摼偱偒傑偣傫偱偟偨.clientID=%d, FILE=%s, LINE=%d",
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
				printf("xml暥傪庢摼偱偒傑偣傫偱偟偨.clientID=%d, FILE=%s, LINE=%d",
					clientID, __FILE__, __LINE__);
				return S_FALSE; 
			}
	}
	return S_OK; 
}
//
// get儊僢僙乕僕偵傛傞僨乕僞庢摼
//
//	DataID	    :setDB偺僨乕僞ID
//	result		:XML僨乕僞
//
//  栠傝抣      : =S_OK              (0) ;惓忢廔椆
//              : =S_FALSE           (1) ;堎忢廔椆
//
unsigned int dataManage::getDatabyDBID(unsigned int DataID, BSTR *result)
{
	unsigned int match;

	if(hasSameID(DataID, setList, &match))	//摨偠ID偑偁傞丠
	{
	    XmlMessage setData = setList.data.at(match); 
		setData.pDoc->get_xml(result);	
	}
	else
	{
		//僨乕僞ID僄儔乕
		printf("堦抳偡傞僨乕僞ID偑偁傝傑偣傫 .丂FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}
	return S_OK; 
}

//
//
//  栠傝抣      : =S_OK              (0) ;惓忢廔椆
//              : =S_FALSE           (1) ;堎忢廔椆
//              : =GET_ERR_MSG_TYPE  (2) ;type僞僌偑晄惓
//
unsigned int dataManage::appendData(XmlMessage *request)
{
	vector<_bstr_t> vpath, vpath_msg, vpath_root;	//梫慺偺奒憌儕僗僩

	MSXML2::IXMLDOMDocument2Ptr pXMLDom = request->pDoc;
	IElementPtr  pRoot = pXMLDom->documentElement;
	int mode = XmlMessage::modeErr;

	vpath_root.push_back(pRoot->GetnodeName());	//梫慺柤傪奒憌儕僗僩偵捛壛

	if (pRoot->hasChildNodes())	//儖乕僩捈壓偵巕僲乕僪乮message僲乕僪乯偑偁傞丠
	{
		MSXML2::IXMLDOMNodeListPtr pMessage = pRoot->GetchildNodes();
		int msgNum = pMessage->Getlength();

		for (int iMsg = 0; iMsg < msgNum; iMsg++)			//message偺悢偩偗儖乕僾
		{
			IElementPtr pMsgElem = pMessage->Getitem(iMsg);	//message梫慺
			
			//DBID傪庢摼
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
				return MSG_TYPE_NG;	//get儊僢僙乕僕偺type僞僌偑"get","set","mainLink send"埲奜								
			}

			vpath_msg = vpath_root;
			vpath_msg.push_back(pMsgElem->GetnodeName());	//梫慺柤傪奒憌儕僗僩偵捛壛

			//if (pMsgElem->hasChildNodes())						//message捈壓偵巕僲乕僪偑偁傞丠
			//{
			//	INodeListPtr pNodes = pMsgElem->GetchildNodes(); //Data梫慺

			//	for (int iNode = 0; iNode < pNodes->Getlength(); iNode++)	//巕僲乕僪偺悢偩偗儖乕僾
			//	{
			//		IElementPtr pChild = pNodes->Getitem(iNode);//message偺懛僲乕僪
			//		vpath = vpath_msg;
			//		vpath.push_back(pChild->GetnodeName());	//梫慺柤傪奒憌儕僗僩偵捛壛

			//		struct dataAttribute dataAttr;
			//		setDataTagAttr(pChild, &dataAttr);
			//		dataAttr.mode = mode;
			//		dataAttr.ID = dbID;
			//		getInChild(pChild, vpath, &dataAttr, mode);	//巕僲乕僪偺夝愅
			//	}
			//	return S_OK;
			//}
			vpath = vpath_msg;
			struct dataAttribute dataAttr;
			setDataTagAttr(pMsgElem, &dataAttr);
			dataAttr.mode = mode;
			dataAttr.ID = dbID;
			getInChild(pMsgElem, vpath, &dataAttr, mode);	//巕僲乕僪偺夝愅 message偐傜傢偨偟丄Data梫慺偐傜夝愅偡傞
		}
		return S_OK;
	}

	return S_FALSE;	//	TODO:message梫慺偺壓偵偼彮側偔偲傕侾偮偺梫慺偑昁梫	
}

//
// XML儊僢僙乕僕DB偺廋惓
//
//	ID	        :ID
//	result		:XML僨乕僞
//
//  栠傝抣      : =S_OK              (0) ;惓忢廔椆
//              : =S_FALSE           (1) ;堎忢廔椆
//              : =GET_ERR_MSG_TYPE  (2) ;type僞僌偑晄惓
//
unsigned int dataManage::addSetMsgData(unsigned int ID, BSTR *result)
{
	HRESULT hr = S_FALSE;
	unsigned int ret = S_FALSE;

	XmlMessage *msg = new XmlMessage(*result, &hr);	//弌椡曇廤梡偺儊僢僙乕僕傪弶婜壔

	if (hr == S_OK)
	{
		if ((ret = appendData(msg)) == 0)	//儊僢僙乕僕偺夝愅
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
// 丂僲乕僪text偺僄儞僐乕僨傿儞僌
//
//
void dataManage::nodesEncoding(MSXML2::IXMLDOMElementPtr pParent, int encoding)
{

	if(encoding != dataAttrEncoding::text) return;//僨僐乕僪偑昁梫側偄側傜儕僞乕儞
	if(pParent == NULL)return ;
	
	MSXML2::IXMLDOMNodePtr child= pParent->GetfirstChild(); 
    while (child!=NULL) {
		if(child->GetfirstChild())
		{
			nodesEncoding(child, encoding);//巕僲乕僪偑偁傞側傜偽丄嵞婣屇傃弌偟
		}
		child = child->GetnextSibling();	//孼掜僲乕僪傪僐僺乕
    }
	
	BSTR value_text;
	_bstr_t str = pParent->Gettext();
	if(str.length() > 0) 
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attMapPtr = pParent->Getattributes();
		if(attMapPtr == NULL) 
		{
			//僨僼僅儖僩"R"
			Base64Data::convertBintoText(str, _bstr_t("R"), &value_text);
		}
		else
		{
			MSXML2::IXMLDOMNodePtr attNodePtr = attMapPtr->getNamedItem("t");
			if(attNodePtr== NULL)
			{
				//僨僼僅儖僩"R"
				Base64Data::convertBintoText(str, _bstr_t("R"), &value_text);
			}
			else
			{
				_variant_t attT = attNodePtr->Gettext();
				attT.ChangeType(VT_BSTR);
				//base64曄姺
				Base64Data::convertBintoText(str, attT.bstrVal, &value_text);
			}
		}
					
		pParent->put_text(value_text);
		::SysFreeString(value_text);
	}

}

//
// 攝楍梫慺傪僗儁乕僗嬫愗傝偱set偡傞
//    pParent   丗梫慺億僀儞僞
//    vpath     丗僞僌忣曬
//    dataAttr  丗懏惈(ELEMATTRIBUTES)
//    mode      丗儌乕僪
//    st        丗攝楍梫慺偺梫慺斣崋乮奐巒乯
//    ed        丗攝楍梫慺偺梫慺斣崋乮廔椆乯
// 
void dataManage::setArrayData(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath, 
						struct dataAttribute *dataAttr, int mode, int st, int ed)
{
	if (ed < st || st < 0 || ed < 0)
	{
		printf("SET:%s 攝楍梫慺偺巜掕偑晄揔愗偱偡丅\n", (char*)vpath.at(vpath.size()-1));
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
			printf("SET:%s 儊儌儕偺妋曐幐攕\n", (char*)vpath.at(vpath.size()-1));
			return;
		}
		//set偡傞僨乕僞傪庢摼
		text = pParent->Gettext();

		//text傪嬻敀偱暘妱
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
				printf("SET:%s 攝楍僨乕僞偺悢偑懌傝傑偣傫\n", (char*)vpath.at(vpath.size()-1));
				//夝曻
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
		//1偮偢偮搊榐
		for (int i = st; i <= ed; i++)
		{
			_bstr_t bstrt(cmtext[i - st]);   //巜掕梫慺偑0偐傜巒傑傞偲偼尷傜側偄偺偱
			bntext = bstrt.copy();
			
			_stprintf_s(str, 8, _T("%d"), i);
			pParent->setAttribute(L"i", str);  //i懏惈捛壛

			pParent->Puttext(bntext);  //set偡傞僨乕僞傪彂偒姺偊
			lastBranch(pParent, vpath, dataAttr, mode);

			::SysFreeString(bntext);
		}

		//夝曻
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
			//GET暥偺XML偵捛壛偟偨偄
			MSXML2::IXMLDOMElementPtr pParent2;
			////1偮偢偮搊榐
			for (int i = st; i <= ed; i++)
			{
				//梫慺傪僐僺乕偡傞傗傝偐偨
				MSXML2::IXMLDOMNodePtr clone = pParent->cloneNode(VARIANT_TRUE);
				nodesEncoding(clone, dataAttr->encoding);
				pParent2 = pParent->parentNode->insertBefore(clone, pParent.GetInterfacePtr()); //慜偵捛壛
				
				_stprintf_s(str, 8, _T("%d"), i);
				pParent2->setAttribute(L"i", str);  //i懏惈捛壛
				lastBranch(pParent2, vpath, dataAttr, mode);
			}
			//僲乕僪嶍彍
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
// 嵟廔僲乕僪偺攝楍懏惈傪曄峏
//    pParent   丗梫慺億僀儞僞
//    vpath     丗僞僌忣曬
//    dataAttr  丗懏惈(ELEMATTRIBUTES)
//    mode      丗儌乕僪
void dataManage::getLastBranchAttr(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath, struct dataAttribute *dataAttr, int mode)
{
	//攝楍偺応崌
	char a[8];
	int tnum = 0;

	MSXML2::IXMLDOMNodePtr pAttI = pParent->Getattributes()->getNamedItem("i");  //懏惈t庢摼
	if (pAttI == NULL)  //懏惈i偑側偄傕偺
	{
		MSXML2::IXMLDOMNodePtr pAttT = pParent->Getattributes()->getNamedItem("t");  //懏惈t庢摼
		if (pAttT != NULL)  //懏惈t偑偁傞
		{
			//t懏惈偺僞僀僾偲梫慺悢傪庢摼
			getAttTValue(pAttT, a, &tnum);

			if (strcmp(a, "S") == 0){  //暥帤楍
				//捠忢偺張棟
				lastBranch(pParent, vpath, dataAttr, mode);
				return;
			}
		}
		//悢帤偑偁偭偨傜
		if (tnum > 1)
		{
			//攝楍傪set
			int st, ed;
			st = 0;
			ed = tnum - 1;
			//攝楍梫慺傪set
			setArrayData(pParent, vpath, dataAttr, mode, st, ed);
		}
		else
		{
			//捠忢偺張棟
			lastBranch(pParent, vpath, dataAttr, mode);
		}
	}
	else
	{
		_bstr_t itext_b = pAttI->Gettext();
		TCHAR itext_c[16];
		_stprintf_s(itext_c, 16, _T("%s"), (LPCTSTR)itext_b);

		//i="1:3"側偳偺応崌
		TCHAR *ret;
		ret = _tcsstr(itext_c, _T(":"));
		if (ret != NULL)
		{
			
			MSXML2::IXMLDOMNodePtr pAttT = pParent->Getattributes()->getNamedItem("t");  //懏惈t庢摼
			if (pAttT != NULL)  //懏惈t偑偁傞
			{
				//t懏惈偺僞僀僾偲梫慺悢傪庢摼
				getAttTValue(pAttT, a, &tnum);

				if (strcmp(a, "S") == 0){  //暥帤楍
					//捠忢偺張棟
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
				printf("Warning:%s 偺i懏惈偵儅僀僫僗偼巜掕偱偒傑偣傫丅0偵偟傑偡丅\n", (char*)vpath.at(vpath.size()-1));
				st = 0;
			}
			if (tnum < (ed - st+1)){
				printf("Warning:%s 偺i懏惈偺巜掕梫慺悢偑t懏惈偺梫慺悢傛傝傕懡偄偱偡\n", (char*)vpath.at(vpath.size()-1));
				ed = tnum + st - 1;
			}
			//攝楍梫慺傪set
			setArrayData(pParent, vpath, dataAttr, mode, st, ed);
		}
		else
		{
			//捠忢偺張棟
			lastBranch(pParent, vpath, dataAttr, mode);
		}
	}
}

//lastBranch偺GET晹暘傪儖乕僾偵偟偨娭悢
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
	case XmlMessage::replyGetMsg: //get儊僢僙乕僕偵懳偡傞墳摎
		if (getAttSendValue(str_query.attNodePtr)) //懏惈 send偑enabled側傜偽
		{
			for (int j = st; j <= ed; j++)
			{    //梫慺i偱儖乕僾
				TCHAR str[8];
				_stprintf_s(str, 8, _T("%d"), j);

				MSXML2::IXMLDOMNodePtr pI = str_query.attNodePtr->getNamedItem(L"i");
				if (pI == NULL){
					pParent->setAttribute(L"i", str);  //i懏惈捛壛
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
						//dbID偑偁偭偰偄傞偐妋擣
						dbID = setDBID(&setData);

						if (dataAttr->ID == dbID){
							break;//for暥傪敳偗傞
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
						for (int iNode = 0; iNode < pNodes->Getlength(); iNode++)		//巕僲乕僪偺悢偩偗儖乕僾
						{
							MSXML2::IXMLDOMElementPtr pChild = pNodes->Getitem(iNode);  //巕
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
								
								/*t懏惈*/
								MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pLeaf->Getattributes();
								MSXML2::IXMLDOMNodePtr pAttT = attNodePtr->getNamedItem("t");
								if (pAttT != NULL){
									/*request 偺t懏惈傪彂偒姺偊傞*/
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
									bntext = pLeaf->Gettext();  //抣
								}
								_tcsncat_s(tmp, bntext, BASE64_BUFFER_SIZE);
								::SysFreeString(dataType);
							}
						}
					}
					else
					{
						/*t懏惈*/
						MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pLeaf->Getattributes();
						MSXML2::IXMLDOMNodePtr pAttT = attNodePtr->getNamedItem("t");
						BSTR dataType = NULL;

						if (pAttT != NULL)
						{
							/*request 偺t懏惈傪彂偒姺偊傞*/
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
							bntext = pLeaf->Gettext();  //抣
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
				pParent->setAttribute(L"i", str);  //i懏惈捛壛
				pParent->put_text(bstrt.copy());	//xml偵斀塮
			}
			else{
				printf("%s 攝楍偺梫慺悢偑惓偟偔側偄偐丄僨乕僞偑懚嵼偟傑偣傫丅\n", (char*)str_query.path.at(str_query.path.size()-1));
			}
		}
		break;

	case XmlMessage::modifiedGet: //get儊僢僙乕僕忋彂偒
		lastBranch(pParent, vpath, dataAttr, mode);
		break;

	default:
		break;
	}
	if(bntext != NULL)	::SysFreeString(bntext);
}

//
//GET:巕僲乕僪埲壓偺乮懛丄傂懛乯僲乕僪傪捛壛偡傞
//      pChild丂 丗捛壛尦偺億僀儞僞乮DB曐懚偺僨乕僞pLeaf乯
//      pParent丂丗梫慺億僀儞僞(捛壛偝傟傞懁乯乮GET暥乯
//
//     栠傝抣丂丂丗VARIANT_TRUE  捛壛惉岟
//                 VARIANT_FALSE丂幐攕
VARIANT_BOOL dataManage::appendDeepChild(MSXML2::IXMLDOMElementPtr pChild, struct dataAttribute *dataAttr, MSXML2::IXMLDOMElementPtr pParent)
{
	//巕僲乕僪傪捛壛
	MSXML2::IXMLDOMNodePtr clone = pChild->cloneNode(VARIANT_TRUE);
	nodesEncoding(clone, dataAttr->encoding);
	MSXML2::IXMLDOMElementPtr pParent2 = pParent->appendChild(clone);

	//懛丄傂懛偺僲乕僪傕僐僺乕
	if (pChild->hasChildNodes())
	{		
		MSXML2::IXMLDOMNodeListPtr  pNodes2 = pChild->GetchildNodes();

		for (int iNode2 = 0; iNode2 < pNodes2->Getlength(); iNode2++)
		{
			MSXML2::IXMLDOMElementPtr pChild2 = pNodes2->Getitem(iNode2);
			if (pChild2 != NULL)
			{
				//pChild偵巕僲乕僪偑偁傞応崌丄捛壛偟偨pParent2偺拞恎偵晄梫僨乕僞(0.0e0)側偳偑擖偭偰傞偺偱嶍彍
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
//  栠傝抣      : =S_OK       (0) ;惓忢廔椆
//              : =S_FALSE    (1) ;堎忢廔椆
//              : =ERR_TAG_NAME(3);僞僌柤偺僒僀僘偑0
//			    : =DATA_GET_ERR   ;僨乕僞庢摼僄儔乕
//			    : =ARRAY_SIZE_ERR ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
//
//
// get儊僢僙乕僕偵傛傞僨乕僞庢摼
//
//	clientID	:僋儔僀傾儞僩ID
//	result		:XML僨乕僞
//
//  栠傝抣      : =S_OK              (0) ;惓忢廔椆
//              : =S_FALSE           (1) ;堎忢廔椆
//              : =GET_ERR_MSG_TYPE  (2) ;type僞僌偑晄惓
//              : =GET_ERR_NEW_XMLMSG(3) ;XmlMessage僋儔僗偺弶婜壔偵幐攕
//              : =GET_ERR_NO_CID    (4) ;僋儔僀傾儞僩ID偵懳墳偡傞儊僢僙乕僕偑側偄
//
unsigned int dataManage::getData(unsigned int clientID, BSTR *result)
{

	unsigned int ret = GET_ERR_NO_CID;

	if (!getList.data.empty()) //get儊僢僙乕僕儕僗僩偑嬻偱側偄丠
	{
		vector<XmlMessage>::iterator get_list = getList.data.begin();

		for (unsigned int i = 0; i<getList.data.size(); i++)	//get儊僢僙乕僕屄悢暘儖乕僾
		{

			if (get_list->getDataID() == clientID)		//ClientID偑堦抳丠
			{
				BSTR bstrGetXML;
				get_list->pDoc->get_xml(&bstrGetXML);	//get儊僢僙乕僕儕僗僩偐傜get儊僢僙乕僕傪庢摼

				HRESULT hr = S_FALSE;
				XmlMessage *msg = new XmlMessage(bstrGetXML, &hr);	//弌椡曇廤梡偺儊僢僙乕僕傪弶婜壔

				if (hr == S_OK)
				{
					msg->removeDisenable();

					//DBID傪庢摼
					unsigned int dbID;
					dbID = setDBID(msg);

					if ((ret = setRequestData(msg, dbID)) == 0)	//get儊僢僙乕僕偺夝愅
					{
						msg->pDoc->get_xml(result); //XML僨乕僞傪庢摼
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
//  栠傝抣      : =S_OK              (0) ;惓忢廔椆
//              : =S_FALSE           (1) ;堎忢廔椆
//              : =GET_ERR_MSG_TYPE  (2) ;type僞僌偑晄惓
//              : =GET_ERR_NEW_XMLMSG(3) ;XmlMessage僋儔僗偺弶婜壔偵幐攕
//              : =GET_ERR_NO_CID    (4) ;僋儔僀傾儞僩ID偵懳墳偡傞儊僢僙乕僕偑側偄
//
unsigned int dataManage::getData(unsigned int DATAID, BSTR request, BSTR path, BSTR *result, int *attTID)
{
	unsigned int ret = GET_ERR_NO_CID;

	HRESULT hr = S_FALSE;
	XmlMessage *msg = new XmlMessage(request, &hr);	//弌椡曇廤梡偺儊僢僙乕僕傪弶婜壔
	if (hr == S_OK)
	{
		if ((ret = setRequestData(msg, DATAID)) == 0)	//get儊僢僙乕僕偺夝愅
		{
			BSTR getXml;
			msg->pDoc->get_xml(&getXml);					//XML僨乕僞傪庢摼
			::SysFreeString(getXml);
		}
	}
	else
	{
		ret = GET_ERR_NEW_XMLMSG;
	}

	MSXML2::IXMLDOMNodePtr pChild = msg->pDoc->selectSingleNode(path);
	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pChild->Getattributes();
	MSXML2::IXMLDOMNodePtr attT = attNodePtr->getNamedItem("t"); //T懏惈

	BSTR dataType = NULL;
	if (attT == NULL)
	{
		dataType = SysAllocString(L"R");	//僨僼僅儖僩傪愝掕
	}
	else
	{
		attT->get_text(&dataType);
		if (SysStringByteLen(dataType) <= 0)
		{
			dataType = SysAllocString(L"R");	//僨僼僅儖僩傪愝掕
		}
	}

	char* c = _com_util::ConvertBSTRToString(dataType);
	size_t c_size = 0;

	for (c_size = 0; c_size< strlen(c); c_size++)	//傾儖僼傽儀僢僩晹傪敳偒弌偡
	{
		if (!isalpha(c[c_size])) break;
	}

	map<int, string>::iterator it = dataTypeMap.begin();
	while (it != dataTypeMap.end())
	{
		if (c_size == strlen((it->second).c_str()))
		{
			if (strncmp((it->second).c_str(), c, strlen((it->second).c_str())) == 0) //暥帤楍堦抳
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
// get儊僢僙乕僕偵傛傞僨乕僞庢摼
//
//	clientID	:僋儔僀傾儞僩ID
//  name        :message梫慺偺name懏惈抣
//	result		:XML僨乕僞
//
//  栠傝抣      : =S_OK              (0) ;惓忢廔椆
//              : =S_FALSE           (1) ;堎忢廔椆
//              : =GET_ERR_MSG_TYPE  (2) ;type僞僌偑晄惓
//              : =GET_ERR_NEW_XMLMSG(3) ;XmlMessage僋儔僗偺弶婜壔偵幐攕
//              : =GET_ERR_NO_CID    (4) ;僋儔僀傾儞僩ID偵懳墳偡傞儊僢僙乕僕偑側偄
//
unsigned int dataManage::getData(unsigned int clientID, BSTR name, BSTR *result)
{
	unsigned int ret = GET_ERR_NO_CID;

	if (!getList.data.empty()) //get儊僢僙乕僕儕僗僩偑嬻偱側偄丠
	{
		vector<XmlMessage>::iterator get_list = getList.data.begin();

		for (unsigned int i = 0; i<getList.data.size(); i++)	//get儊僢僙乕僕屄悢暘儖乕僾
		{
			BSTR name_list = getAttName(get_list->pDoc, 0);

			char* str1 = _com_util::ConvertBSTRToString(name);
			char* str2 = _com_util::ConvertBSTRToString(name_list);
			::SysFreeString(name_list);

			//ClientID偑堦抳丠 && name懏惈偑堦抳丠
			if ((get_list->getDataID() == clientID) && (strncmp(str2, str1, BASE64_BUFFER_SIZE) == 0))
			{
				BSTR bstrGetXML;
				get_list->pDoc->get_xml(&bstrGetXML);	//get儊僢僙乕僕儕僗僩偐傜get儊僢僙乕僕傪庢摼

				//Debug
				//printf("\ngetXML搊榐偟偰偁傞傕偺\n");
				//wprintf(bstrGetXML);

				HRESULT hr = S_FALSE;
				XmlMessage *msg = new XmlMessage(bstrGetXML, &hr);	//弌椡曇廤梡偺儊僢僙乕僕傪弶婜壔

				if (hr == S_OK)
				{
					msg->removeDisenable();

					//DBID傪庢摼
					unsigned int dbID;
					dbID = setDBID(msg);

					if ((ret = setRequestData(msg, dbID)) == 0)	//get儊僢僙乕僕偺夝愅
					{
						msg->pDoc->get_xml(result); //XML僨乕僞傪庢摼
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
		printf("僞僌柤偑NULL偱偡丅FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return ERR_TAG_NAME;
	}

	// XML僨乕僞曐懚僋儔僗	
	HRESULT hr = S_FALSE;
	XmlData wrt = XmlData(&hr);
	if (FAILED(hr))
	{
		printf("Failed to instantiate an XML DOM.丂FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}

	// 儊僢僙乕僕僞僌
	MessageTag msg = MessageTag(L"mainlink send");
	wrt.addMessageTag(&msg);

	//丂僇僥僑儕乕僞僌
	IElementPtr pChild = wrt.addChild(msg.getMsgPtr(), L"Data"); // 戞侾堷悢偵恊傪巜掕
	pChild->setAttribute("encoding", "base64");
	IElementPtr pParent = pChild;

	//  僞僌
	vector<_bstr_t>::iterator it = vpath.begin();
	int max_idx = vpath.size() - 1; //儕僗僩僒僀僘

	_bstr_t path = L"/root/message/Data/";

	for (int i = 0; i < max_idx; i++)			//儕僗僩偺峔惉梫慺偺屄悢暘儖乕僾
	{
		pParent = pChild;
		pChild = wrt.addChild(pParent, vpath.at(i));
		path += (vpath.at(i) + L"/");
	}
	path += vpath.at(max_idx);

	pParent = pChild;
	ElementData child = ElementData(vpath.at(max_idx), NULL);
	if (idx != NULL) child.setAttribute(L"i", idx);
	child.setAttribute(L"t", NULL); //僟儈乕僨乕僞傪擖傟傞
	IElementPtr pElem = wrt.addChild(pParent, &child);

	hr = getElementData(&wrt, clientID, path, result, attTID);

	return hr;
}
//
//
//  栠傝抣      : =S_OK       (0) ;惓忢廔椆
//              : =S_FALSE    (1) ;堎忢廔椆
//              : =ERR_TAG_NAME(3);僞僌柤偺僒僀僘偑0
//			    : =DATA_GET_ERR   ;僨乕僞庢摼僄儔乕
//			    : =ARRAY_SIZE_ERR ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
//
HRESULT dataManage::getElement(LPCTSTR type, unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, LPTSTR result, int* attTID)
{

	if (vpath.size() <= 0)
	{
		printf("僞僌柤偑NULL偱偡丅FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return ERR_TAG_NAME;
	}

	// XML僨乕僞曐懚僋儔僗	
	HRESULT hr = S_FALSE;
	XmlData wrt = XmlData(&hr);
	if (FAILED(hr))
	{
		printf("Failed to instantiate an XML DOM.丂FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}

	// 儊僢僙乕僕僞僌
	MessageTag msg = MessageTag(type);
	wrt.addMessageTag(&msg);

	//丂僇僥僑儕乕僞僌
	IElementPtr pChild = wrt.addChild(msg.getMsgPtr(), L"Data"); // 戞侾堷悢偵恊傪巜掕
	pChild->setAttribute("encoding", "base64");
	IElementPtr pParent = pChild;

	//  僞僌
	vector<_bstr_t>::iterator it = vpath.begin();
	int max_idx = vpath.size() - 1; //儕僗僩僒僀僘

	_bstr_t path = L"/root/message/Data/";

	for (int i = 0; i < max_idx; i++)			//儕僗僩偺峔惉梫慺偺屄悢暘儖乕僾
	{
		pParent = pChild;
		pChild = wrt.addChild(pParent, vpath.at(i));
		path += (vpath.at(i) + L"/");
	}
	path += vpath.at(max_idx);

	pParent = pChild;
	ElementData child = ElementData(vpath.at(max_idx), NULL);
	if (idx != NULL) child.setAttribute(L"i", idx);
	child.setAttribute(L"t", NULL); //僟儈乕僨乕僞傪擖傟傞
	IElementPtr pElem = wrt.addChild(pParent, &child);

	hr = getElementData(&wrt, clientID, path, result, attTID);

	return hr;
}
//
//
//  栠傝抣      : =S_OK        (0) ;惓忢廔椆
//              : =S_FALSE     (1) ;堎忢廔椆
//              : =ERR_TAG_NAME(3);僞僌柤偺僒僀僘偑0
//
HRESULT dataManage::setElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, LPCTSTR valtype, LPCTSTR pstr)
{

	if (vpath.size() <= 0)
	{
		printf("僞僌柤偑NULL偱偡丅FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return ERR_TAG_NAME;
	}

	HRESULT hr;

	// XML僨乕僞曐懚僋儔僗	
	XmlData wrt = XmlData(&hr);
	if (FAILED(hr))
	{
		printf("Failed to instantiate an XML DOM.丂FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}

	// 儊僢僙乕僕僞僌
	MessageTag msg = MessageTag(L"set");
	wrt.addMessageTag(&msg);

	//丂僇僥僑儕乕僞僌
	IElementPtr pChild = wrt.addChild(msg.getMsgPtr(), L"Data"); // 戞侾堷悢偵恊傪巜掕
	IElementPtr pParent = pChild;

	//  僞僌
	vector<_bstr_t>::iterator it = vpath.begin();
	int max_idx = vpath.size() - 1; //儕僗僩僒僀僘

	for (int i = 0; i < max_idx; i++)			//儕僗僩偺峔惉梫慺偺屄悢暘儖乕僾
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
// 儊僀儞DB傛傝堦梫慺偺庢摼亂double亃
//    vpath     丗僞僌忣曬
//    idx       丗懏惈i
//    value     丗抣
// 
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((double*)test_decode);

	return hr;

}

//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂double亃
//    vpath     丗僞僌忣曬
//    value     丗抣
// 
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, double *value)
{
	return getOneElement(vpath, NULL, value);
}
//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂int亃
//    clientID  :僋儔僀傾儞僩ID
//    vpath     丗僞僌忣曬
//    idx       丗懏惈i
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((int*)test_decode);

	return hr;

}

//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂int亃
//    vpath     丗僞僌忣曬
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, int *value)
{
	return getOneElement(vpath, NULL, value);
}
//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂unsigned short亃
//    vpath     丗僞僌忣曬
//    idx       丗懏惈i
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned short*)test_decode);

	return hr;

}
//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂unsigned short亃
//    vpath     丗僞僌忣曬
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, unsigned short *value)
{
	return getOneElement(vpath, NULL, value);
}

//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂晞崋柍偟8價僢僩惍悢宆亃
//    vpath     丗僞僌忣曬
//    idx       丗懏惈i
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned char*)test_decode);

	return hr;

}
//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂僽乕儖戙悢宆亃
//    vpath     丗僞僌忣曬
//    idx       丗懏惈i
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decodeString(val, Base64Data::B, test_decode);
	value[0] = test_decode[0];

	return hr;

}
//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂晞崋柍偟8價僢僩惍悢宆亃
//    vpath     丗僞僌忣曬
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, unsigned char *value)
{
	return getOneElement(vpath, NULL, value);
}
//
//元素//获取[双]
//客户端ID：委托人身份证
// VPATH：标签信息
// IDX：我属性
//值：值
//
//返回值：= S_OK（0）;成功完成
//：= S_FALSE（1）;异常终止
//：= ERR_DATA_ATT_T（5）;数据类型不匹配
//：= ERR_TAG_NAME（6）;所述标签名0的大小
//：= ERR_DATA_GET（7）;数据获取误差
//：= ERR_ARRAY_SIZE（8）;复印大小大于缓冲区的大小越大
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
		printf("数据类型不匹配\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("数据不能得到。\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((double*)test_decode);

	return hr;

}
//
// 堦梫慺偺庢摼亂double亃
//    clientID  :僋儔僀傾儞僩ID
//    vpath     丗僞僌忣曬
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, double *value)
{
	return getOneElement(clientID, vpath, NULL, value);
}
//
// 堦梫慺偺庢摼亂int亃
//    clientID  :僋儔僀傾儞僩ID
//    vpath     丗僞僌忣曬
//    idx       丗懏惈i
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((int*)test_decode);

	return hr;

}
//
// 堦梫慺偺庢摼亂int亃
//    clientID  :僋儔僀傾儞僩ID
//    vpath     丗僞僌忣曬
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, int *value)
{
	return getOneElement(clientID, vpath, NULL, value);
}

//
// 堦梫慺偺庢摼亂unsigned short亃
//    clientID  :僋儔僀傾儞僩ID
//    vpath     丗僞僌忣曬
//    idx       丗懏惈i
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned short*)test_decode);

	return hr;

}
//
// 堦梫慺偺庢摼亂unsigned short亃
//    clientID  :僋儔僀傾儞僩ID
//    vpath     丗僞僌忣曬
//    value     丗抣
//
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, unsigned short *value)
{
	return getOneElement(clientID, vpath, NULL, value);
}

//
// 堦梫慺偺庢摼亂晞崋柍偟8價僢僩惍悢宆亃
//    clientID  :僋儔僀傾儞僩ID
//    vpath     丗僞僌忣曬
//    idx       丗懏惈i
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned char*)test_decode);

	return hr;

}

//
// 堦梫慺偺庢摼亂晞崋柍偟8價僢僩惍悢宆亃
//    clientID  :僋儔僀傾儞僩ID
//    vpath     丗僞僌忣曬
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
HRESULT dataManage::getOneElement(unsigned int clientID, vector<_bstr_t>vpath, unsigned char *value)
{
	return getOneElement(clientID, vpath, NULL, value);
}

//
// 堦梫慺偺庢摼亂僽乕儖戙悢宆亃
//    clientID  :僋儔僀傾儞僩ID
//    vpath     丗僞僌忣曬
//    attr      丗懏惈(ELEMATTRIBUTES)
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decodeString(val, Base64Data::B, test_decode);
	value[0] = test_decode[0];

	return hr;

}

//
// 堦梫慺偺庢摼亂僽乕儖戙悢宆亃
//    clientID  :僋儔僀傾儞僩ID
//    vpath     丗僞僌忣曬
//    idx       丗懏惈i
//    value     丗抣
//
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decodeString(val, Base64Data::B, test_decode);
	value[0] = test_decode[0];

	return hr;

}

//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂double亃
//    vpath     丗僞僌忣曬
//    attr      丗懏惈(ELEMATTRIBUTES)
//    value     丗抣
// 
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((double*)test_decode);

	return hr;

}

//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂int亃
//    vpath     丗僞僌忣曬
//    attr      丗懏惈(ELEMATTRIBUTES)
//    value     丗抣
// 
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((int*)test_decode);

	return hr;

}
//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂unsigned short亃
//    vpath     丗僞僌忣曬
//    attr      丗懏惈(ELEMATTRIBUTES)
//    value     丗抣
// 
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned short*)test_decode);

	return hr;

}
//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂unsigned char亃
//    vpath     丗僞僌忣曬
//    attr      丗懏惈(ELEMATTRIBUTES)
//    value     丗抣
// 
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned char*)test_decode);

	return hr;

}
//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂僽乕儖戙悢宆亃
//    vpath     丗僞僌忣曬
//    attr      丗懏惈(ELEMATTRIBUTES)
//    value     丗抣    
// 
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, char value[1])
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::B;
	int attTID = -1;

	HRESULT hr = getElement(L"get", SET_MAINDB, vpath, attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decodeString(val, Base64Data::B, test_decode);
	value[0] = test_decode[0];

	return hr;

}
//
//
//  栠傝抣      : =S_OK       (0) ;惓忢廔椆
//              : =S_FALSE    (1) ;堎忢廔椆
//              : =ERR_TAG_NAME(3);僞僌柤偺僒僀僘偑0
//			    : =DATA_GET_ERR   ;僨乕僞庢摼僄儔乕
//			    : =ARRAY_SIZE_ERR ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
//
HRESULT dataManage::getElement(LPCTSTR type, unsigned int clientID, vector<_bstr_t>vpath,
	ELEMATTRIBUTES attr, LPTSTR result, int* attTID)
{
	if (vpath.size() <= 0)
	{
		printf("僞僌柤偑NULL偱偡丅FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return ERR_TAG_NAME;
	}

	// XML僨乕僞曐懚僋儔僗	
	HRESULT hr = S_FALSE;
	XmlData wrt = XmlData(&hr);
	if (FAILED(hr))
	{
		printf("Failed to instantiate an XML DOM.丂FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}

	// 儊僢僙乕僕僞僌
	MessageTag msg = MessageTag(type);
	wrt.addMessageTag(&msg);

	//丂僇僥僑儕乕僞僌
	IElementPtr pChild = wrt.addChild(msg.getMsgPtr(), L"Data"); // 戞侾堷悢偵恊傪巜掕
	pChild->setAttribute("encoding", "base64");
	IElementPtr pParent = pChild;

	//  僞僌
	vector<_bstr_t>::iterator it = vpath.begin();
	int max_idx = vpath.size() - 1; //儕僗僩僒僀僘

	_bstr_t path = L"/root/message/Data/";

	for (int i = 0; i < max_idx; i++)			//儕僗僩偺峔惉梫慺偺屄悢暘儖乕僾
	{
		pParent = pChild;
		pChild = wrt.addChild(pParent, vpath.at(i));
		path += (vpath.at(i) + L"/");
	}
	path += vpath.at(max_idx);

	pParent = pChild;
	ElementData child = ElementData(vpath.at(max_idx), NULL);
	if ((attr.i) != _bstr_t(L"")) //NULL偱側偐偭偨傜
	{
		child.setAttribute(L"i", attr.i);
	}
	child.setAttribute(L"a", attr.a);
	child.setAttribute(L"c", attr.c);
	child.setAttribute(L"tm", attr.tm);

	child.setAttribute(L"t", NULL); //僟儈乕僨乕僞傪擖傟傞
	IElementPtr pElem = wrt.addChild(pParent, &child);

	hr = getElementData(&wrt, clientID, path, result, attTID);

	return hr;
}
//
//
//  栠傝抣      : =S_OK       (0) ;惓忢廔椆
//              : =S_FALSE    (1) ;堎忢廔椆
//              : =ERR_TAG_NAME(3);僞僌柤偺僒僀僘偑0
//			    : =DATA_GET_ERR   ;僨乕僞庢摼僄儔乕
//			    : =ARRAY_SIZE_ERR ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
//
HRESULT dataManage::getElement(unsigned int dbID, vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, LPTSTR result, int* attTID)
{
	if (vpath.size() <= 0)
	{
		printf("僞僌柤偑NULL偱偡丅FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return ERR_TAG_NAME;
	}

	// XML僨乕僞曐懚僋儔僗	
	HRESULT hr = S_FALSE;
	XmlData wrt = XmlData(&hr);
	if (FAILED(hr))
	{
		printf("Failed to instantiate an XML DOM.丂FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}

	//儖乕僩億僀儞僞
	IElementPtr pRoot = wrt.getRootPtr();
	_bstr_t path = L"/root/";

	//message僞僌傪捛壛
	IElementPtr pMsg = wrt.addChild(pRoot, vpath.at(0));  //Message僞僌
	pMsg->setAttribute(TYPE_KY, L"get");       //Message Type傪愝掕
	pMsg->setAttribute(L"a", msg_attr.a);  //Message僞僌偵a懏惈傪愝掕
	path += (vpath.at(0) + L"/");

	//丂僇僥僑儕乕僞僌
	IElementPtr pChild = wrt.addChild(pMsg, vpath.at(1)); // 戞侾堷悢偵恊傪巜掕
	pChild->setAttribute("encoding", "base64");
	IElementPtr pParent = pChild;
	path += (vpath.at(1) + L"/");

	//  僞僌
	vector<_bstr_t>::iterator it = vpath.begin();
	int max_idx = vpath.size() - 1; //儕僗僩僒僀僘

	for (int i = 2; i < max_idx; i++)			//儕僗僩偺峔惉梫慺偺屄悢暘儖乕僾
	{
		pParent = pChild;
		pChild = wrt.addChild(pParent, vpath.at(i));
		path += (vpath.at(i) + L"/");
	}
	path += vpath.at(max_idx);

	pParent = pChild;
	ElementData child = ElementData(vpath.at(max_idx), NULL);
	if ((attr.i) != _bstr_t(L"")) //NULL偱側偐偭偨傜
	{
		child.setAttribute(L"i", attr.i);
	}
	child.setAttribute(L"a" , attr.a);
	child.setAttribute(L"c" , attr.c);
	child.setAttribute(L"tm", attr.tm);

	child.setAttribute(L"t", NULL); //僟儈乕僨乕僞傪擖傟傞
	IElementPtr pElem = wrt.addChild(pParent, &child);

	//get
	hr = getElementData(&wrt, dbID, path, result, attTID);

	return hr;
}

//
//
//  栠傝抣      : =S_OK       (0) ;惓忢廔椆
//              : =S_FALSE    (1) ;堎忢廔椆
//              : =ERR_TAG_NAME(3);僞僌柤偺僒僀僘偑0
//			    : =DATA_GET_ERR   ;僨乕僞庢摼僄儔乕
//			    : =ARRAY_SIZE_ERR ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
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
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨 FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		hr = ERR_DATA_GET;
	}
	else
	{
		printf("攝楍僒僀僘偑僐僺乕尦僒僀僘傛傝傕彫偱偡 FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		hr = ERR_ARRAY_SIZE;
	}

	::SysFreeString(bstrXML);
	::SysFreeString(bstrResult);

	return hr;
}

//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂double亃
//    vpath     丗僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    value     丗抣
// 
//  栠傝抣      : =S_OK          (0);惓忢廔椆
//              : =S_FALSE       (1);堎忢廔椆
//              : =ERR_DATA_ATT_T(5);僨乕僞僞僀僾偺晄惍崌
//              : =ERR_TAG_NAME  (6);僞僌柤偺僒僀僘偑0
//			    : =ERR_DATA_GET  (7) ;僨乕僞庢摼僄儔乕
//			    : =ERR_ARRAY_SIZE(8) ;僶僢僼傽僒僀僘傛傝傕僐僺乕僒僀僘偑戝
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, double *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::R;
	int attTID = -1;
	unsigned int dbID;

	//message偺a懏惈偐傜DB傪寛掕
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((double*)test_decode);

	return hr;
}
//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂int亃
//    vpath     丗僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    value     丗抣
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, int *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::I;
	int attTID = -1;
	unsigned int dbID;

	//message偺a懏惈偐傜DB傪寛掕
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((int*)test_decode);

	return hr;
}
//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂unsigned short亃
//    vpath     丗僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    value     丗抣
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned short *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::USI;
	int attTID = -1;
	unsigned int dbID;

	//message偺a懏惈偐傜DB傪寛掕
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned short*)test_decode);

	return hr;
}

//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂unsigned char亃
//    vpath     丗僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    value     丗抣
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned char *value)
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];
	size_t outsize;

	int dataType = Base64Data::X;
	int attTID = -1;
	unsigned int dbID;

	//message偺a懏惈偐傜DB傪寛掕
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decode(val, test_decode, &outsize);
	*value = *((unsigned char*)test_decode);

	return hr;
}
//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂char亃
//    vpath     丗僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    value     丗抣
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, char value[1])
{
	TCHAR  val[BASE64_BUFFER_SIZE];
	char   test_decode[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::B;
	int attTID = -1;
	unsigned int dbID;

	//message偺a懏惈偐傜DB傪寛掕
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	Base64Data::decodeString(val, Base64Data::B, test_decode);
	value[0] = test_decode[0];

	return hr;
}

//
// 儊僀儞DB傛傝堦梫慺偺庢摼亂string亃
//    vpath     丗僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    value     丗抣
HRESULT dataManage::getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, LPTSTR value)
{
	TCHAR  val[BASE64_BUFFER_SIZE*10];

	int dataType = Base64Data::S;
	int attTID = -1;
	unsigned int dbID;

	//message偺a懏惈偐傜DB傪寛掕
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	if (dataType != attTID)
	{
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
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

	//message偺a懏惈偐傜DB傪寛掕
	dbID = setDBID(msg_attr.a);

	HRESULT hr = getElement(dbID, vpath, attr, msg_attr, val, &attTID);
	
	if (dataType != attTID)
	{
		printf("僨乕僞僞僀僾偺晄惍崌\n");
		return ERR_DATA_ATT_T;
	}

	if (!_tcslen(val))
	{
		printf("僨乕僞偑庢摼偱偒傑偣傫偱偟偨丅\n");
		return ERR_DATA_GET;
	}

	//Base64Data::decode(val, test_decode, &outsize);
	*value = val;

	return hr;
}

#pragma endregion get

#pragma region set
//
// 堦梫慺偺峏怴亂double亃
//    vpath      :僞僌忣曬
//    array_size丗攝楍僒僀僘
//    array_idx 丗攝楍僀儞僨僢僋僗
//    value     丗抣
//
//  栠傝抣      : =S_OK             (0) ;惓忢廔椆
//              : =S_FALSE          (1) ;堎忢廔椆
//              : =ERR_TAG_NAME     (3);僞僌柤偺僒僀僘偑0
//              : =ERR_BASE64_COVERT(6);base64曄姺僄儔乕
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, double value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::R;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("R%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("R"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// 堦梫慺偺峏怴亂float亃
//    vpath      :僞僌忣曬
//    array_size丗攝楍僒僀僘
//    array_idx 丗攝楍僀儞僨僢僋僗
//    value     丗抣
//
//  栠傝抣      : =S_OK             (0) ;惓忢廔椆
//              : =S_FALSE          (1) ;堎忢廔椆
//              : =ERR_TAG_NAME     (3);僞僌柤偺僒僀僘偑0
//              : =ERR_BASE64_COVERT(6);base64曄姺僄儔乕
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, float value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::SR;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("SR%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("SR"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// 堦梫慺偺峏怴亂long double亃
//    vpath      :僞僌忣曬
//    array_size丗攝楍僒僀僘
//    array_idx 丗攝楍僀儞僨僢僋僗
//    value     丗抣
//
//  栠傝抣      : =S_OK             (0) ;惓忢廔椆
//              : =S_FALSE          (1) ;堎忢廔椆
//              : =ERR_TAG_NAME     (3);僞僌柤偺僒僀僘偑0
//              : =ERR_BASE64_COVERT(6);base64曄姺僄儔乕
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, long double value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::LR;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("LR%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("LR"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// 堦梫慺偺峏怴亂int亃
//    vpath      :僞僌忣曬
//    array_size丗攝楍僒僀僘
//    array_idx 丗攝楍僀儞僨僢僋僗
//    value     丗抣
//
//  栠傝抣      : =S_OK             (0) ;惓忢廔椆
//              : =S_FALSE          (1) ;堎忢廔椆
//              : =ERR_TAG_NAME     (3);僞僌柤偺僒僀僘偑0
//              : =ERR_BASE64_COVERT(6);base64曄姺僄儔乕
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, int value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::I;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("I%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("I"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// 堦梫慺偺峏怴亂short亃
//    vpath      :僞僌忣曬
//    array_size丗攝楍僒僀僘
//    array_idx 丗攝楍僀儞僨僢僋僗
//    value     丗抣
//
//  栠傝抣      : =S_OK             (0) ;惓忢廔椆
//              : =S_FALSE          (1) ;堎忢廔椆
//              : =ERR_TAG_NAME     (3);僞僌柤偺僒僀僘偑0
//              : =ERR_BASE64_COVERT(6);base64曄姺僄儔乕
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, short value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::SI;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("SI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("SI"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// 堦梫慺偺峏怴亂long long亃
//    vpath      :僞僌忣曬
//    array_size丗攝楍僒僀僘
//    array_idx 丗攝楍僀儞僨僢僋僗
//    value     丗抣
//
//  栠傝抣      : =S_OK             (0) ;惓忢廔椆
//              : =S_FALSE          (1) ;堎忢廔椆
//              : =ERR_TAG_NAME     (3);僞僌柤偺僒僀僘偑0
//              : =ERR_BASE64_COVERT(6);base64曄姺僄儔乕
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, long long value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::SI;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("LI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("LI"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// 堦梫慺偺峏怴亂unsigned int亃
//    vpath      :僞僌忣曬
//    array_size丗攝楍僒僀僘
//    array_idx 丗攝楍僀儞僨僢僋僗
//    value     丗抣
//
//  栠傝抣      : =S_OK             (0) ;惓忢廔椆
//              : =S_FALSE          (1) ;堎忢廔椆
//              : =ERR_TAG_NAME     (3);僞僌柤偺僒僀僘偑0
//              : =ERR_BASE64_COVERT(6);base64曄姺僄儔乕
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, unsigned int value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::SI;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("UI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("UI"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// 堦梫慺偺峏怴亂unsigned short int亃
//    vpath      :僞僌忣曬
//    array_size丗攝楍僒僀僘
//    array_idx 丗攝楍僀儞僨僢僋僗
//    value     丗抣
//
//  栠傝抣      : =S_OK             (0) ;惓忢廔椆
//              : =S_FALSE          (1) ;堎忢廔椆
//              : =ERR_TAG_NAME     (3);僞僌柤偺僒僀僘偑0
//              : =ERR_BASE64_COVERT(6);base64曄姺僄儔乕
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, unsigned short int value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::USI;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("USI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("USI"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// 堦梫慺偺峏怴亂unsigned long long亃
//    vpath      :僞僌忣曬
//    array_size丗攝楍僒僀僘
//    array_idx 丗攝楍僀儞僨僢僋僗
//    value     丗抣
//
//  栠傝抣      : =S_OK             (0) ;惓忢廔椆
//              : =S_FALSE          (1) ;堎忢廔椆
//              : =ERR_TAG_NAME     (3);僞僌柤偺僒僀僘偑0
//              : =ERR_BASE64_COVERT(6);base64曄姺僄儔乕
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, unsigned long long value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::ULI;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("ULI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("ULI"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
// 堦梫慺偺峏怴亂晞崋側偟8價僢僩惍悢宆亃
//    vpath      :僞僌忣曬
//    array_size丗攝楍僒僀僘
//    array_idx 丗攝楍僀儞僨僢僋僗
//    value     丗抣
//
//  栠傝抣      : =S_OK             (0) ;惓忢廔椆
//              : =S_FALSE          (1) ;堎忢廔椆
//              : =ERR_TAG_NAME     (3);僞僌柤偺僒僀僘偑0
//              : =ERR_BASE64_COVERT(6);base64曄姺僄儔乕
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, unsigned char value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::X;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("X%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("X"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, array_size, array_idx, valtype, pstr);
	return hr;
}

//
//
//  栠傝抣      : =S_OK        (0) ;惓忢廔椆
//              : =S_FALSE     (1) ;堎忢廔椆
//              : =ERR_TAG_NAME(3);僞僌柤偺僒僀僘偑0
//
HRESULT dataManage::setElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, LPCTSTR valtype, LPCTSTR pstr)
{
	if (vpath.size() <= 0)
	{
		printf("僞僌柤偑NULL偱偡丅FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return ERR_TAG_NAME;
	}

	HRESULT hr;

	// XML僨乕僞曐懚僋儔僗	
	XmlData wrt = XmlData(&hr);
	if (FAILED(hr))
	{
		printf("Failed to instantiate an XML DOM.丂FILE = %s , LINE = %d\n", __FILE__, __LINE__);
		return S_FALSE;
	}

	//儖乕僩億僀儞僞
	IElementPtr pRoot = wrt.getRootPtr();

	//message僞僌傪捛壛
	IElementPtr pMsg = wrt.addChild(pRoot, vpath.at(0));	//Message僞僌
	pMsg->setAttribute(TYPE_KY, msg_attr.type);				//Message Type傪愝掕
	if(msg_attr.name  != _bstr_t(L"")) pMsg->setAttribute(L"name", msg_attr.name);      //Message name傪愝掕
	if(msg_attr.cycle != _bstr_t(L"")) pMsg->setAttribute(L"cycle", msg_attr.cycle);    //Message cycle傪愝掕
	if(msg_attr.a     != _bstr_t(L"")) pMsg->setAttribute(L"a", msg_attr.a);			//Message僞僌偵a懏惈傪愝掕

	//丂僇僥僑儕乕僞僌
	IElementPtr pChild = wrt.addChild(pMsg, vpath.at(1)); // 戞侾堷悢偵恊傪巜掕
	IElementPtr pParent = pChild;

	//  僞僌
	vector<_bstr_t>::iterator it = vpath.begin();
	int max_idx = vpath.size() - 1; //儕僗僩僒僀僘

	for (int i = 2; i < max_idx; i++)			//儕僗僩偺峔惉梫慺偺屄悢暘儖乕僾
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
////  栠傝抣      : =S_OK        (0) ;惓忢廔椆
////              : =S_FALSE     (1) ;堎忢廔椆
////              : =ERR_TAG_NAME(3);僞僌柤偺僒僀僘偑0
////
//HRESULT dataManage::setElement(vector<_bstr_t>vpath, vector<ELEMATTRIBUTES>attr, MSGATTRIBUTES msg_attr, unsigned int array_size, LPCTSTR valtype, LPCTSTR pstr)
//{
//	if (vpath.size() <= 0)
//	{
//		printf("僞僌柤偑NULL偱偡丅FILE = %s , LINE = %d\n", __FILE__, __LINE__);
//		return ERR_TAG_NAME;
//	}
//
//	HRESULT hr;
//
//	// XML僨乕僞曐懚僋儔僗	
//	XmlData wrt = XmlData(&hr);
//	if (FAILED(hr))
//	{
//		printf("Failed to instantiate an XML DOM.丂FILE = %s , LINE = %d\n", __FILE__, __LINE__);
//		return S_FALSE;
//	}
//
//	//儖乕僩億僀儞僞
//	IElementPtr pRoot = wrt.getRootPtr();
//
//	//message僞僌傪捛壛
////	IElementPtr pMsg = wrt.addChild(pRoot, vpath.at(0));  //Message僞僌
//	IElementPtr pMsg = wrt.addChild(pRoot, L"message");  //Message僞僌
//	pMsg->setAttribute(TYPE_KY, msg_attr.type);       //Message Type傪愝掕
//	pMsg->setAttribute(L"name", msg_attr.name);       //Message name傪愝掕
//	pMsg->setAttribute(L"cycle", msg_attr.cycle);       //Message cycle傪愝掕
////	pMsg->setAttribute(L"a", attr.at(0).a);  //Message僞僌偵a懏惈傪愝掕
//	pMsg->setAttribute(L"a", msg_attr.a);  //Message僞僌偵a懏惈傪愝掕
//
//	//丂僇僥僑儕乕僞僌
//	IElementPtr pChild = wrt.addChild(pMsg, vpath.at(0)); // 戞侾堷悢偵恊傪巜掕
//	pChild->setAttribute(L"a", attr.at(0).a);  //僞僌偵a懏惈傪愝掕
//	IElementPtr pParent = pChild;
//
//	//  僞僌
//	vector<_bstr_t>::iterator it = vpath.begin();
//	int max_idx = vpath.size() - 1; //儕僗僩僒僀僘
//
////	for (int i = 2; i < max_idx; i++)			//儕僗僩偺峔惉梫慺偺屄悢暘儖乕僾
//	for (int i = 1; i < max_idx; i++)			//儕僗僩偺峔惉梫慺偺屄悢暘儖乕僾
//	{
//		pParent = pChild;
//		pChild = wrt.addChild(pParent, vpath.at(i));
//		pChild->setAttribute(L"a", attr.at(i).a);  //僞僌偵a懏惈傪愝掕
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
// 堦梫慺偺峏怴亂double亃
//    vpath      :僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    array_size丗攝楍僒僀僘
//    value     丗抣
//
//  栠傝抣      : =S_OK             (0) ;惓忢廔椆
//              : =S_FALSE          (1) ;堎忢廔椆
//              : =ERR_TAG_NAME     (3);僞僌柤偺僒僀僘偑0
//              : =ERR_BASE64_COVERT(6);base64曄姺僄儔乕
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, double value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::R;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("R%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("R"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// 堦梫慺偺峏怴亂float亃
//    vpath      :僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    array_size丗攝楍僒僀僘
//    value     丗抣
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, float value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::SR;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("SR%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("SR"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// 堦梫慺偺峏怴亂long double亃
//    vpath      :僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    array_size丗攝楍僒僀僘
//    value     丗抣
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, long double value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::LR;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("LR%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("LR"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// 堦梫慺偺峏怴亂int亃
//    vpath      :僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    array_size丗攝楍僒僀僘
//    value     丗抣
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, int value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::I;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("I%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("I"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// 堦梫慺偺峏怴亂short亃
//    vpath      :僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    array_size丗攝楍僒僀僘
//    value     丗抣
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, short value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::SI;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("SI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("SI"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// 堦梫慺偺峏怴亂long long亃
//    vpath      :僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    array_size丗攝楍僒僀僘
//    value     丗抣
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, long long value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::LI;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("LI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("LI"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// 堦梫慺偺峏怴亂unsigned int亃
//    vpath      :僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    array_size丗攝楍僒僀僘
//    value     丗抣
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, unsigned int value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::UI;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("UI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("UI"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// 堦梫慺偺峏怴亂unsigned short int亃
//    vpath      :僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    array_size丗攝楍僒僀僘
//    value     丗抣
//(	GetXmlPath("Ctrl", "Rotor"), 
//::setAttributeACI(NULL, NULL, L"0"),
//setmsg_attr, 6, uw );
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, unsigned short int value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::USI;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("USI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("USI"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// 堦梫慺偺峏怴亂unsigned long long亃
//    vpath      :僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    array_size丗攝楍僒僀僘
//    value     丗抣
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, unsigned long long value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::ULI;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("ULI%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("ULI"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}
//
// 堦梫慺偺峏怴亂unsigned char亃
//    vpath      :僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    array_size丗攝楍僒僀僘
//    value     丗抣
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, unsigned char value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::X;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("X%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("X"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}

//
// 堦梫慺偺峏怴亂 char亃
//    vpath      :僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    array_size丗攝楍僒僀僘
//    value     丗抣
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, char value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::B;

	if (array_size > 0)	//懏惈t愝掕
	{
		_stprintf_s(valtype, _T("B%d"), array_size);
	}
	else
	{
		_tcscpy_s(valtype, _T("B"));
	}

	//丂base64曄姺
	size_t outsize;
	TCHAR  pstr[BASE64_BUFFER_SIZE];
	Base64Data::encode(&value, dataType, pstr, &outsize);
	if (outsize == BASE64_BUFFER_SIZE || outsize >= BASE64_BUFFER_SIZE)
	{
		return ERR_BASE64_COVERT;	//曄姺FAIL
	}

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, attr, msg_attr, array_size, valtype, pstr);
	return hr;
}

//
// 堦梫慺偺峏怴亂string亃
//    vpath      :僞僌忣曬 message偐傜巜掕
//    attr      丗懏惈(ELEMATTRIBUTES) message偐傜巜掕
//    array_size丗攝楍僒僀僘
//    value     丗抣
//
HRESULT dataManage::setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, LPCTSTR value)
{
	TCHAR valtype[BASE64_BUFFER_SIZE];

	int dataType = Base64Data::S;

	_tcscpy_s(valtype, _T("S"));

	//丂梫慺傪捛壛
	HRESULT hr = setElement(vpath, attr, msg_attr, 0, valtype, value);
	return hr;
}
#pragma endregion set

//
// DB偺ID傪愝掕偡傞
//    attr_a    丗message僞僌偺a懏惈偺抣
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
			if (strlen(str) == strlen((it->second).c_str())) //暥帤楍堦抳
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
		//ID偑搊榐偝傟偰偄側偄偺偱丄Map偵搊榐偡傞
		dbIDMap.insert(make_pair(no, str));    //栠傝抣傪傒偰偐傜return偟偨傎偆偑傛偄偐丠
		delete[] str;
		return no;
	}
}

//
// DB偺ID傪愝掕偡傞
//    xml    丗xml message
unsigned int dataManage::setDBID(XmlMessage *xml)
{
	unsigned int dbID;

	IElementPtr pMsgElem = xml->pDoc->documentElement->GetchildNodes()->Getitem(0); //msgNo偼0偱屌掕偟偰偟傑偭偰偄傞
	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pMsgElem->Getattributes();
	_bstr_t attA;  //message僞僌偺a懏惈
	VARIANT_BOOL bl = getAttA(attNodePtr, &attA);
	dbID = setDBID(attA);

	return dbID;
}

// 
//丂set儊僢僙乕僕偺僋儕傾
//
//  丂dataID 丗僨乕僞ID
//  丂msgID  丗儊僢僙乕僕ID
//
//    栠傝抣    : =VARIANT_TRUE; clear
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

			//儖乕僩
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
						// msssage偺巕僲乕僪
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
									pDataElem->removeChild(pDataElem2);//僋儕傾
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

