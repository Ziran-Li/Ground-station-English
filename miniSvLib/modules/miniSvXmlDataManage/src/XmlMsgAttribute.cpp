#include "stdafx.h"
#include <tchar.h>
#include "XmlMessage.h"
#include "XmlMsgAttribute.h"
#include "Base64Data.h"

//
// ����send����
//
//  �@attNodePtr  �F�����m�[�hMAP�̃|�C���^
//
//�@�@�߂�l      : VARIANT_TRUE �Genable 
//�@�@            : VARIANT_FALSE�Gdisable 
//
VARIANT_BOOL getAttSendValue(INamedNodeMapPtr attNodePtr)
{
	if(attNodePtr->length >0 )	//����������H
	{
		//����i�̃m�[�h���擾
		IXMLDOMNodePtr ptr = attNodePtr->getNamedItem("send");

		if(ptr != NULL) // ����send�͂���H
		{
			//����send�̒l���擾
			variant_t send;
			ptr->get_nodeValue(&send);

			if(send.vt != VT_NULL)	//����send��value��NULL�ł͂Ȃ��H
			{
				//����send�̒l�`�F�b�N
				if(_tcscmp((_bstr_t)send , SEND_ENABLE)==0)
				{
					return VARIANT_TRUE;	
				}
				else
				{
					return VARIANT_FALSE;
				}
			}
		}
	}
	return VARIANT_TRUE;	//�������Ȃ��ꍇ�Asend="enable"�Ƃ��Ĉ���

}

//
// ����i�̒l���擾
//
//  �@attNodePtr  �F�����m�[�hMAP�̃|�C���^
//  �@idx	      �F����i�̒l�i�����ɕϊ���̒l�j
//
//�@�@�߂�l      : VARIANT_TRUE �G����i�L��
//�@�@            : VARIANT_FALSE�G����i����
//
VARIANT_BOOL getAttIValue(INamedNodeMapPtr attNodePtr, int *idx)
{
	if(attNodePtr->length >0 )	//����������H
	{
		//����i�̃m�[�h���擾
		IXMLDOMNodePtr ptr = attNodePtr->getNamedItem("i");

		if(ptr != NULL) // ����i�͂���H
		{

			//����i�̒l���擾
			variant_t idx_db;
			ptr->get_nodeValue(&idx_db);

			if(idx_db.vt != VT_NULL)	//����i��value��NULL�ł͂Ȃ��H
			{
				//����i�̒l�𐮐��^�ɕϊ�
				//��������int�^�ɕϊ�����O�ɁAidx_db�������l�ł��邩�`�F�b�N
				int i = 0;
				BSTR c = idx_db.bstrVal;
				while (c[i] != '\0'){
					if (isdigit(idx_db.bstrVal[i]) == 0) return VARIANT_FALSE;
					i++;
				}
				::SysFreeString(c);

				idx_db.ChangeType(VT_I4);
				*idx = idx_db;
				return VARIANT_TRUE;	
			}
		}
	}
	return VARIANT_FALSE;	

}

//
// �����̒l���擾
//
//  �@attNodePtr  �F�����m�[�hMAP�̃|�C���^
//  �@keyword	  �F�����̖��O
//  �@att	      �F�����̒l
//
//�@�@�߂�l      : VARIANT_TRUE �G�����L��
//�@�@            : VARIANT_FALSE�G��������
//
//
VARIANT_BOOL getAtt(INamedNodeMapPtr attNodePtr, _bstr_t keyword, _bstr_t *att)
{
	*att = _bstr_t(L"");

	if (attNodePtr->length >0)	//����������H
	{
		//�����̃m�[�h���擾
		IXMLDOMNodePtr ptr = attNodePtr->getNamedItem(keyword);
		if (keyword == _bstr_t("t")) //����t�H
		{
			if (ptr != NULL) // �����͂���H
			{
				char attT[8];
				int  num;
				getAttTValue(ptr, attT, &num);
				if (_bstr_t(attT) == _bstr_t(L""))
				{
					//�ȗ����̓f�t�H���g��"R"
					*att = _bstr_t(L"R");
				}
				else
				{
					*att = _bstr_t(attT);
				}
			}
			else
			{
				*att = _bstr_t(L"R");
			}
		}
		else if (ptr != NULL) // �����͂���H
		{
			//�����̒l���擾
			variant_t idx_db;
			ptr->get_nodeValue(&idx_db);
			if (idx_db.vt != VT_NULL)	//�����̒l��NULL�ł͂Ȃ��H
			{
				*att = (idx_db);
			}
		}
	}

	if (wcscmp(*att, L"") == 0) return VARIANT_FALSE;
	return VARIANT_TRUE;

}
//
// �����̒l�̈�v���m�F
//
//  �@attNodePtr  �F�����m�[�hMAP�̃|�C���^
//  �@keyword	  �F�����̖��O
//  �@att	      �F�����̒l
//
//�@�@�߂�l      : VARIANT_TRUE �G��v
//�@�@            : VARIANT_FALSE�G�s��v
//
//
VARIANT_BOOL matchAttrs(INamedNodeMapPtr attNodePtr1, INamedNodeMapPtr attNodePtr2, _bstr_t keyword)
{
	_bstr_t att1;
	_bstr_t att2;

	VARIANT_BOOL bool1 = getAtt(attNodePtr1, keyword, &att1);
	VARIANT_BOOL bool2 = getAtt(attNodePtr2, keyword, &att2);

	if (bool1 != bool2) return VARIANT_FALSE; //�s��v
	if (att1  != att2) return VARIANT_FALSE; //�s��v

	return VARIANT_TRUE; //��v
}

//
// ����name�̒l���擾
//
//  �@attNodePtr  �F�����m�[�hMAP�̃|�C���^
//
//�@�@�߂�l      : ����name�̒l�iBSTR)
//
//
BSTR getAttName(INamedNodeMapPtr attNodePtr)
{
	if(attNodePtr->length >0 )	//����������H
	{
		//����name�̃m�[�h���擾
		IXMLDOMNodePtr ptr = attNodePtr->getNamedItem("name");

		if(ptr != NULL) // ����name�͂���H
		{

			//����name�̒l���擾
			variant_t idx_db;
			ptr->get_nodeValue(&idx_db);

			if(idx_db.vt != VT_NULL)	//����name��value��NULL�ł͂Ȃ��H
			{
				idx_db.ChangeType(VT_BSTR);
				return  idx_db.bstrVal;	
			}
		}
	}
	return NULL;	

}
//
// ����name�̒l���擾
//
//  �@pDoc        �FDOM�h�L�������g�|�C���^
//    msgNo       �Fmessage�ԍ�
//  �@name	      �F����name�̒l
//
//�@�@�߂�l      : VARIANT_TRUE �G����name�L��
//�@�@            : VARIANT_FALSE�G����name����
//
//
BSTR getAttName(MSXML2::IXMLDOMDocument2Ptr pDoc, unsigned int msgNo)
{
	IElementPtr pMsgElem = pDoc->documentElement->GetchildNodes()->Getitem(msgNo);  //message�v�f
	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pMsgElem->Getattributes();

	return getAttName(attNodePtr);
}

//
// ����a�̒l���擾
//
//  �@attNodePtr  �F�����m�[�hMAP�̃|�C���^
//  �@name	      �F����a�̒l
//
//�@�@�߂�l      : VARIANT_TRUE �G����a�L��
//�@�@            : VARIANT_FALSE�G����a����
//
//
VARIANT_BOOL getAttA(INamedNodeMapPtr attNodePtr, _bstr_t *name)
{
	if(attNodePtr->length >0 )	//����������H
	{
		//����a�̃m�[�h���擾
		IXMLDOMNodePtr ptr = attNodePtr->getNamedItem("a");

		if(ptr != NULL) // ����a�͂���H
		{

			//����a�̒l���擾
			variant_t idx_db;
			ptr->get_nodeValue(&idx_db);

			if(idx_db.vt != VT_NULL)	//����a��value��NULL�ł͂Ȃ��H
			{
				*name = (idx_db);
				if(wcscmp(*name , L"") ==0) return VARIANT_FALSE;
				return VARIANT_TRUE;	
			}
		}
	}
	*name = "";
	return VARIANT_FALSE;	

}


//
// ���b�Z�[�W�^�C�v�̐ݒ�
//
//  �@pDoc�@�@�@  �FDOM�h�L�������g�|�C���^
//  �@type        �F����type�̒l
//
//�@�@�߂�l      : VARIANT_TRUE �G�ݒ萬��
//�@�@            : VARIANT_FALSE�G�ݒ莸�s
//
//
VARIANT_BOOL setAttType(MSXML2::IXMLDOMDocument2Ptr pDoc, BSTR type)
{
	MSXML2::IXMLDOMNodePtr pMsg = pDoc->selectSingleNode(L"/root/message");
	if(pMsg==NULL) 
	{
		printf("xml���b�Z�[�W��message�^�O������܂��� FILE=%s, LINE=%d",__FILE__, __LINE__);
		return VARIANT_FALSE;	
	}

	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pMsg->Getattributes();
	if(attNodePtr->length >0 )	//����������H
	{
		MSXML2::IXMLDOMNodePtr pType = attNodePtr->getNamedItem(L"type");
		if(pType != NULL) // ����type�͂���H
		{
			pType->put_text(type);	//message�^�C�v��ݒ�
			return VARIANT_TRUE;	
		}
	}

	printf("xml���b�Z�[�W��type����������܂��� FILE=%s, LINE=%d",__FILE__, __LINE__);
	return VARIANT_FALSE;	

}


//
// �G���R�[�h�^�C�v�̐ݒ�
//
//  �@pDoc�@�@�@  �FDOM�h�L�������g�|�C���^
//  �@encode      �F����encode�̒l
//
//�@�@�߂�l      : VARIANT_TRUE �G�ݒ萬��
//�@�@            : VARIANT_FALSE�G�ݒ莸�s
//
//
VARIANT_BOOL setAttEncode(MSXML2::IXMLDOMDocument2Ptr pDoc, BSTR encode)
{
	MSXML2::IXMLDOMNodePtr pData = pDoc->selectSingleNode(L"/root/message/Data");
	if(pData==NULL) 
	{
		printf("xml���b�Z�[�W��Data�^�O������܂��� FILE=%s, LINE=%d",__FILE__, __LINE__);
		return VARIANT_FALSE;	
	}

	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pData->Getattributes();

	if(attNodePtr->length >0 )	//����������H
	{
		MSXML2::IXMLDOMNodePtr pEncode = attNodePtr->getNamedItem(L"encoding");
		if(pEncode != NULL) // ����encoding�͂���H
		{
			pEncode->put_text(encode);	//encoding��ݒ�
			return VARIANT_TRUE;	
		}
	}

	//�����̂ő�����ǉ�����
	MSXML2::IXMLDOMAttributePtr att = pDoc->createAttribute(L"encoding");
	att->put_value(variant_t(encode));
	attNodePtr->setNamedItem(att);
	return VARIANT_TRUE;	

}
//-setAttributeACI(NULL, NULL, NULL);
ELEMATTRIBUTES setAttributeACI(BSTR A, BSTR C, BSTR I)
{
	BSTR tmp = NULL;
	ELEMATTRIBUTES attr = { tmp, tmp, tmp, tmp };
	SysFreeString(tmp);//SysFreeString�ͺ�delete,free���ƣ�ֻ�Ǹ�����ڴ���գ����ǲ�������ڴ���������

	attr.a = A;
	attr.c = C;
	attr.i = I;

	BSTR tm = NULL;
	attr.tm = tm;
	return attr;
}
//
// �m�[�h������ݒ�
//
//  �@A�@�@�@�@�@ �F����a
//  �@C�@�@�@�@�@ �F����c
//  �@I�@�@�@�@�@ �F����i
//  �@Tm�@�@�@�@�@�F����tm
//
//�@�@�߂�l      : ELEMATTRIBUTES
//
ELEMATTRIBUTES setAttributeACITm(BSTR A, BSTR C, BSTR I, BSTR Tm)
{
	ELEMATTRIBUTES attr;

	attr.a = A;
	attr.c = C;
	attr.i = I;
	attr.tm = Tm;

	return attr;
}
//
// ����t�̒l���擾
//
//  �@attNodePtr  �F�����m�[�hMAP�̃|�C���^
//  �@c�@�@�@�@�@ �F�f�[�^�^�C�v�i������j
//    n�@�@�@     :�f�[�^�v�f��
//
//�@�@�߂�l      : VARIANT_TRUE �G����i�L��
//�@�@            : VARIANT_FALSE�G����i����
//
VARIANT_BOOL getAttTValue(MSXML2::IXMLDOMNodePtr pAttT, char *a, int *tnum)
{
	BSTR dataType = NULL;
	size_t c_size = 0, n_size = 0, a_size = 0;
	char n[8];
	memset(a, 0, 8);
	memset(n, 0, 8);

	*tnum = 0;

	if (pAttT != NULL){
		pAttT->get_text(&dataType);
		char* c = _com_util::ConvertBSTRToString(dataType);
		::SysFreeString(dataType);
		for (c_size = 0; c_size< strlen(c); c_size++)	//�������𔲂��o��
		{
			if (isdigit(c[c_size]))
			{
				n[n_size] = c[c_size];
				n_size++;
			}
			else{
				a[a_size] = c[c_size];
				a_size++;
			}
		}
		delete[] c;
		if (n_size > 0){
			*tnum = atoi(n);
		}
		else{
			*tnum = 0;
		}
		return VARIANT_TRUE;
	}
	else{
		return VARIANT_FALSE;
	}
}

//
// �G���R�[�h�^�C�v�̎擾
//
//  �@pDoc�@�@�@  �FDOM�h�L�������g�|�C���^
//  �@encode      �F����encode�̒l
//
//�@�@�߂�l      : VARIANT_TRUE �G�擾����
//�@�@            : VARIANT_FALSE�G�擾���s
//
VARIANT_BOOL getAttEncode(MSXML2::IXMLDOMDocument2Ptr pDoc, BSTR *encode)
{
	MSXML2::IXMLDOMNodePtr pData = pDoc->selectSingleNode(L"/root/message/Data");
	if (pData == NULL)
	{
		printf("xml���b�Z�[�W��Data�^�O������܂��� FILE=%s, LINE=%d", __FILE__, __LINE__);
		return VARIANT_FALSE;
	}

	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pData->Getattributes();

	if (attNodePtr->length >0)	//����������H
	{
		MSXML2::IXMLDOMNodePtr pEncode = attNodePtr->getNamedItem(L"encoding");
		if (pEncode != NULL) // ����encoding�͂���H
		{
			*encode = pEncode->Gettext();
			return VARIANT_TRUE;
		}
	}
	return VARIANT_FALSE;
}

//message�^�O�̑����\���̂�ݒ�
MSGATTRIBUTES setMsgAttribute(BSTR type, BSTR name, BSTR cycle, BSTR a)
{
	MSGATTRIBUTES attr;

	attr.type = type;
	attr.name = name;
	attr.cycle = cycle;
	attr.a = a;

	return attr;
}