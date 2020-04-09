#include "stdafx.h"
#include "StrXml.h"

StrXml::StrXml(){
	loadResult = S_FALSE;
};

StrXml::StrXml(BSTR bstrXML){
	loadResult = S_FALSE;
	loadStrXml(bstrXML);
};


//
//�@Xml������̓Ǎ���
//
HRESULT StrXml::loadStrXml(BSTR bstrXML)
{

	//Create the XmlDocument

	HRESULT hr  = pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument60) );
    if (FAILED(hr)) 
    {
		printf("Failed to instantiate an XML DOM.\n");
		loadResult = S_FALSE;
    }
	pDoc->async = VARIANT_FALSE;	

	//Load the XML string
	VARIANT_BOOL isSuccess;

	HRESULT hr1  = pDoc->raw_loadXML(bstrXML, &isSuccess);
    if (FAILED(hr1)) 
	{
		MSXML2::IXMLDOMParseErrorPtr pErr;
		BSTR bstrErr = NULL;
		pDoc->get_parseError(&pErr);
		pErr->get_reason(&bstrErr);
		printf("XML DOM loaded errror \n",bstrErr);
		::SysFreeString(bstrErr);

		loadResult = S_FALSE;
	} 
	else
	{
		loadResult = S_OK;
	}

	return loadResult;
}

_bstr_t StrXml::getData(BSTR keyword)
{
	if (loadResult != S_OK) return _bstr_t(L"");
	 
	MSXML2::IXMLDOMNodePtr xl =pDoc->selectSingleNode(keyword);
    if(xl)
    {
		return xl->Gettext();
    }
	else
	{
		return _bstr_t(L"");
	}

}

VARIANT_BOOL StrXml::isSame(BSTR keyword, BSTR str)
{
	if (loadResult != S_OK) return VARIANT_FALSE;

	if (keyword == NULL || SysStringLen(keyword) == 0)
	{
		if (str == NULL || SysStringLen(str) == 0)
		{
			return VARIANT_TRUE;
		}
		else
		{
			return VARIANT_FALSE;
		}
	}
	else
	{
		if (str == NULL || SysStringLen(str) == 0)
		{
			return VARIANT_FALSE;
		}
		else if (wcscmp(keyword, str))
		{
			return VARIANT_FALSE;
		}
		else
		{
			return VARIANT_TRUE;
		}
	}

}

_bstr_t StrXml::getData(BSTR keyword, ELEMATTRIBUTES attr)
{
	if (loadResult != S_OK) return _bstr_t(L"");

	MSXML2::IXMLDOMNodeListPtr pNodes = pDoc->selectNodes(keyword);
	for (int iNode = 0; iNode < pNodes->Getlength(); iNode++)	//�m�[�h�̐��������[�v
	{
		MSXML2::IXMLDOMElementPtr pChild = pNodes->Getitem(iNode);//�q�m�[�h
		if (pChild != NULL)
		{
			MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pChild->Getattributes();
			MSXML2::IXMLDOMNodePtr pAttA  = attNodePtr->getNamedItem("a");
			MSXML2::IXMLDOMNodePtr pAttC  = attNodePtr->getNamedItem("c");
			MSXML2::IXMLDOMNodePtr pAttI  = attNodePtr->getNamedItem("i");
			MSXML2::IXMLDOMNodePtr pAttTm = attNodePtr->getNamedItem("tm");

			// ����i
			if (pAttI == NULL)
			{
				if (isSame(attr.i.GetBSTR(), NULL) == VARIANT_FALSE) continue;
			}
			else
			{
				_bstr_t i = pAttI->Gettext();
				if (isSame(attr.i.GetBSTR(), i.GetBSTR()) == VARIANT_FALSE) continue;
			}
			// ����a
			if (pAttA == NULL)
			{
				if (isSame(attr.a.GetBSTR(), NULL) == VARIANT_FALSE) continue;
			}
			else
			{
				_bstr_t a = pAttA->Gettext();
				if (isSame(attr.a.GetBSTR(), a.GetBSTR()) == VARIANT_FALSE) continue;
			}
			// ����c
			if (pAttC == NULL)
			{
				if (isSame(attr.c.GetBSTR(), NULL) == VARIANT_FALSE) continue;
			}
			else
			{
				_bstr_t c = pAttC->Gettext();
				if (isSame(attr.c.GetBSTR(), c.GetBSTR()) == VARIANT_FALSE) continue;
			}

			// ����tm
			if (pAttTm == NULL)
			{
				if (isSame(attr.tm.GetBSTR(), NULL) == VARIANT_FALSE) continue;
			}
			else
			{
				_bstr_t tm = pAttTm->Gettext();
				if (isSame(attr.tm.GetBSTR(), tm.GetBSTR()) == VARIANT_FALSE) continue;
			}


			// �S��������v�A�m�[�h�e�L�X�g�����^�[��
			return pChild->Gettext();
		}
	}

	return _bstr_t(L"");	//�}�b�`������̂��Ȃ��P�[�X

}

size_t StrXml::getAttTID(BSTR dataType)
{
	char* c = _com_util::ConvertBSTRToString(dataType);
	size_t c_size = 0;

	for(c_size=0; c_size< strlen(c); c_size++)	//�A���t�@�x�b�g���𔲂��o��
	{
		if(!isalpha(c[c_size])) break;
	}

	map<int,string>::iterator it = dataTypeMap.begin();
	while( it != dataTypeMap.end() ) 
	{
		if(c_size == strlen((it->second).c_str())) 
		{
			if (strncmp((it->second).c_str(), c, strlen((it->second).c_str())) == 0) //�������v
			{
				delete c;
				return it->first;
			}
		}
		++it;
	}

	delete c;
	return dataTypeMap.size();
}


void StrXml::childNode(IElementPtr pElem, int mode)
{
	
	if (pElem != NULL && pElem->hasChildNodes())	//�q�m�[�h������H
	{
		//	����	  
		MSXML2::IXMLDOMNodeListPtr pNodes = pElem->GetchildNodes();
		for (int iNode = 0; iNode < pNodes->Getlength(); iNode++)	//�q�m�[�h�̐��������[�v
		{
			IElementPtr pChild = pNodes->Getitem(iNode);//�q�m�[�h
			if (pChild != NULL)
			{
				childNode(pChild, mode);
			}
			else 
			{
				//T����
				MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pElem->Getattributes();
				MSXML2::IXMLDOMNodePtr attT = attNodePtr->getNamedItem("t");
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

				_bstr_t   str = pElem->Gettext();

				if (str.length() != 0)
				{
					int attTID = getAttTID(dataType);
					if (mode == 0) //txt->bin
					{
						char *data = _com_util::ConvertBSTRToString(pElem->Gettext());
						TCHAR  out[BASE64_BUFFER_SIZE];
						Base64Data::convTxtValue(attTID, data, out);
						delete[] data;
						pElem->put_text(out);
					}
					else
					{
						char  cout[BASE64_BUFFER_SIZE];
						Base64Data::convBinValue(attTID, pElem->Gettext(), cout);
						_bstr_t out(cout);
						pElem->put_text(out);

					}
				}

				::SysFreeString(dataType);

				break;
			}
		}

	}
	else
	{
		//	�Ȃ�

		//T����
		MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pElem->Getattributes();
		MSXML2::IXMLDOMNodePtr attT = attNodePtr->getNamedItem("t"); 
		BSTR dataType =NULL;			
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

		_bstr_t   str = pElem->Gettext();

		if (str.length() != 0)
		{
			int attTID = getAttTID(dataType);
			if (mode == 0) //txt->bin
			{
				char *data = _com_util::ConvertBSTRToString(pElem->Gettext());
				TCHAR  out[BASE64_BUFFER_SIZE];
				Base64Data::convTxtValue(attTID, data, out);
				delete[] data;
				pElem->put_text(out);
			}
			else
			{
				char  cout[BASE64_BUFFER_SIZE];
				Base64Data::convBinValue(attTID, pElem->Gettext(), cout);
				_bstr_t out(cout);
				pElem->put_text(out);

			}
		}
		::SysFreeString(dataType);
	}
}

_bstr_t StrXml::convertTxt2Bin()
{
	if (loadResult != S_OK) return _bstr_t(L"");
	 
	MSXML2::IXMLDOMElementPtr  pElem   = pDoc->documentElement;//���[�g�|�C���^
	childNode(pElem, 0);


	return pDoc->Getxml();
}


_bstr_t StrXml::convertBin2Txt()
{
	if (loadResult != S_OK) return _bstr_t(L"");

	MSXML2::IXMLDOMElementPtr  pElem = pDoc->documentElement;//���[�g�|�C���^
	childNode(pElem, 1);


	return pDoc->Getxml();
}


//
// �q�m�[�h�̖��O���擾
//
//  �@vpath�@�@�@  �F�����p�X�imessage
//  �@nodeName     �F�q�m�[�h�̖��O
//
//�@�@�߂�l      : VARIANT_TRUE �G�q�m�[�h����
//�@�@            : VARIANT_FALSE�G�q�m�[�h�Ȃ�

VARIANT_BOOL StrXml::getChileNodeName(vector<_bstr_t> vpath, vector<_bstr_t> *nodeName)
{
	if (loadResult != S_OK) return VARIANT_FALSE;
	size_t size = vpath.size();
	if (size == 0) return VARIANT_FALSE;

	//�@�p�X����
	_bstr_t path = L"/root/message/";
	for (size_t i = 0; i < size - 1; i++)
	{
		path += vpath.at(i);
		path += L"/";
	}
	path += vpath.at(size - 1);

	//�@�q�m�[�h������
	MSXML2::IXMLDOMNodePtr xl = pDoc->selectSingleNode(path);
	if (xl)
	{
		MSXML2::IXMLDOMNodeListPtr pChilds = xl->GetchildNodes();
		int num = pChilds->Getlength();
		for (int j = 0; j < num; j++)
		{
			MSXML2::IXMLDOMElementPtr pChild = pChilds->Getitem(j);
			nodeName->push_back(pChild->GetnodeName());
		}
	}
	else
	{
		nodeName->push_back(L"");
		return VARIANT_FALSE;
	}
	return VARIANT_TRUE;
}
