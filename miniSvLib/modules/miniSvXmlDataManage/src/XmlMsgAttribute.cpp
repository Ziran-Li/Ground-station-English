#include "stdafx.h"
#include <tchar.h>
#include "XmlMessage.h"
#include "XmlMsgAttribute.h"
#include "Base64Data.h"

//
// 属性send判定
//
//  　attNodePtr  ：属性ノードMAPのポインタ
//
//　　戻り値      : VARIANT_TRUE ；enable 
//　　            : VARIANT_FALSE；disable 
//
VARIANT_BOOL getAttSendValue(INamedNodeMapPtr attNodePtr)
{
	if(attNodePtr->length >0 )	//属性がある？
	{
		//属性iのノードを取得
		IXMLDOMNodePtr ptr = attNodePtr->getNamedItem("send");

		if(ptr != NULL) // 属性sendはある？
		{
			//属性sendの値を取得
			variant_t send;
			ptr->get_nodeValue(&send);

			if(send.vt != VT_NULL)	//属性sendのvalueはNULLではない？
			{
				//属性sendの値チェック
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
	return VARIANT_TRUE;	//属性がない場合、send="enable"として扱う

}

//
// 属性iの値を取得
//
//  　attNodePtr  ：属性ノードMAPのポインタ
//  　idx	      ：属性iの値（整数に変換後の値）
//
//　　戻り値      : VARIANT_TRUE ；属性i有り
//　　            : VARIANT_FALSE；属性i無し
//
VARIANT_BOOL getAttIValue(INamedNodeMapPtr attNodePtr, int *idx)
{
	if(attNodePtr->length >0 )	//属性がある？
	{
		//属性iのノードを取得
		IXMLDOMNodePtr ptr = attNodePtr->getNamedItem("i");

		if(ptr != NULL) // 属性iはある？
		{

			//属性iの値を取得
			variant_t idx_db;
			ptr->get_nodeValue(&idx_db);

			if(idx_db.vt != VT_NULL)	//属性iのvalueはNULLではない？
			{
				//属性iの値を整数型に変換
				//文字からint型に変換する前に、idx_dbが整数値であるかチェック
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
// 属性の値を取得
//
//  　attNodePtr  ：属性ノードMAPのポインタ
//  　keyword	  ：属性の名前
//  　att	      ：属性の値
//
//　　戻り値      : VARIANT_TRUE ；属性有り
//　　            : VARIANT_FALSE；属性無し
//
//
VARIANT_BOOL getAtt(INamedNodeMapPtr attNodePtr, _bstr_t keyword, _bstr_t *att)
{
	*att = _bstr_t(L"");

	if (attNodePtr->length >0)	//属性がある？
	{
		//属性のノードを取得
		IXMLDOMNodePtr ptr = attNodePtr->getNamedItem(keyword);
		if (keyword == _bstr_t("t")) //属性t？
		{
			if (ptr != NULL) // 属性はある？
			{
				char attT[8];
				int  num;
				getAttTValue(ptr, attT, &num);
				if (_bstr_t(attT) == _bstr_t(L""))
				{
					//省略時はデフォルトの"R"
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
		else if (ptr != NULL) // 属性はある？
		{
			//属性の値を取得
			variant_t idx_db;
			ptr->get_nodeValue(&idx_db);
			if (idx_db.vt != VT_NULL)	//属性の値はNULLではない？
			{
				*att = (idx_db);
			}
		}
	}

	if (wcscmp(*att, L"") == 0) return VARIANT_FALSE;
	return VARIANT_TRUE;

}
//
// 属性の値の一致性確認
//
//  　attNodePtr  ：属性ノードMAPのポインタ
//  　keyword	  ：属性の名前
//  　att	      ：属性の値
//
//　　戻り値      : VARIANT_TRUE ；一致
//　　            : VARIANT_FALSE；不一致
//
//
VARIANT_BOOL matchAttrs(INamedNodeMapPtr attNodePtr1, INamedNodeMapPtr attNodePtr2, _bstr_t keyword)
{
	_bstr_t att1;
	_bstr_t att2;

	VARIANT_BOOL bool1 = getAtt(attNodePtr1, keyword, &att1);
	VARIANT_BOOL bool2 = getAtt(attNodePtr2, keyword, &att2);

	if (bool1 != bool2) return VARIANT_FALSE; //不一致
	if (att1  != att2) return VARIANT_FALSE; //不一致

	return VARIANT_TRUE; //一致
}

//
// 属性nameの値を取得
//
//  　attNodePtr  ：属性ノードMAPのポインタ
//
//　　戻り値      : 属性nameの値（BSTR)
//
//
BSTR getAttName(INamedNodeMapPtr attNodePtr)
{
	if(attNodePtr->length >0 )	//属性がある？
	{
		//属性nameのノードを取得
		IXMLDOMNodePtr ptr = attNodePtr->getNamedItem("name");

		if(ptr != NULL) // 属性nameはある？
		{

			//属性nameの値を取得
			variant_t idx_db;
			ptr->get_nodeValue(&idx_db);

			if(idx_db.vt != VT_NULL)	//属性nameのvalueはNULLではない？
			{
				idx_db.ChangeType(VT_BSTR);
				return  idx_db.bstrVal;	
			}
		}
	}
	return NULL;	

}
//
// 属性nameの値を取得
//
//  　pDoc        ：DOMドキュメントポインタ
//    msgNo       ：message番号
//  　name	      ：属性nameの値
//
//　　戻り値      : VARIANT_TRUE ；属性name有り
//　　            : VARIANT_FALSE；属性name無し
//
//
BSTR getAttName(MSXML2::IXMLDOMDocument2Ptr pDoc, unsigned int msgNo)
{
	IElementPtr pMsgElem = pDoc->documentElement->GetchildNodes()->Getitem(msgNo);  //message要素
	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pMsgElem->Getattributes();

	return getAttName(attNodePtr);
}

//
// 属性aの値を取得
//
//  　attNodePtr  ：属性ノードMAPのポインタ
//  　name	      ：属性aの値
//
//　　戻り値      : VARIANT_TRUE ；属性a有り
//　　            : VARIANT_FALSE；属性a無し
//
//
VARIANT_BOOL getAttA(INamedNodeMapPtr attNodePtr, _bstr_t *name)
{
	if(attNodePtr->length >0 )	//属性がある？
	{
		//属性aのノードを取得
		IXMLDOMNodePtr ptr = attNodePtr->getNamedItem("a");

		if(ptr != NULL) // 属性aはある？
		{

			//属性aの値を取得
			variant_t idx_db;
			ptr->get_nodeValue(&idx_db);

			if(idx_db.vt != VT_NULL)	//属性aのvalueはNULLではない？
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
// メッセージタイプの設定
//
//  　pDoc　　　  ：DOMドキュメントポインタ
//  　type        ：属性typeの値
//
//　　戻り値      : VARIANT_TRUE ；設定成功
//　　            : VARIANT_FALSE；設定失敗
//
//
VARIANT_BOOL setAttType(MSXML2::IXMLDOMDocument2Ptr pDoc, BSTR type)
{
	MSXML2::IXMLDOMNodePtr pMsg = pDoc->selectSingleNode(L"/root/message");
	if(pMsg==NULL) 
	{
		printf("xmlメッセージにmessageタグがありません FILE=%s, LINE=%d",__FILE__, __LINE__);
		return VARIANT_FALSE;	
	}

	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pMsg->Getattributes();
	if(attNodePtr->length >0 )	//属性がある？
	{
		MSXML2::IXMLDOMNodePtr pType = attNodePtr->getNamedItem(L"type");
		if(pType != NULL) // 属性typeはある？
		{
			pType->put_text(type);	//messageタイプを設定
			return VARIANT_TRUE;	
		}
	}

	printf("xmlメッセージにtype属性がありません FILE=%s, LINE=%d",__FILE__, __LINE__);
	return VARIANT_FALSE;	

}


//
// エンコードタイプの設定
//
//  　pDoc　　　  ：DOMドキュメントポインタ
//  　encode      ：属性encodeの値
//
//　　戻り値      : VARIANT_TRUE ；設定成功
//　　            : VARIANT_FALSE；設定失敗
//
//
VARIANT_BOOL setAttEncode(MSXML2::IXMLDOMDocument2Ptr pDoc, BSTR encode)
{
	MSXML2::IXMLDOMNodePtr pData = pDoc->selectSingleNode(L"/root/message/Data");
	if(pData==NULL) 
	{
		printf("xmlメッセージにDataタグがありません FILE=%s, LINE=%d",__FILE__, __LINE__);
		return VARIANT_FALSE;	
	}

	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pData->Getattributes();

	if(attNodePtr->length >0 )	//属性がある？
	{
		MSXML2::IXMLDOMNodePtr pEncode = attNodePtr->getNamedItem(L"encoding");
		if(pEncode != NULL) // 属性encodingはある？
		{
			pEncode->put_text(encode);	//encodingを設定
			return VARIANT_TRUE;	
		}
	}

	//無いので属性を追加する
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
	SysFreeString(tmp);//SysFreeStringｾﾍｺﾍdelete,freeﾀ猴ﾆ｣ｬﾖｻﾊﾇｸｺﾔﾑﾄﾚｴ貊ﾘﾊﾕ｣ｬｵｫﾊﾇｲｻｸｺﾔﾑﾄﾚｴ豬ﾄﾄﾚﾈﾝﾇ蠢ﾕ

	attr.a = A;
	attr.c = C;
	attr.i = I;

	BSTR tm = NULL;
	attr.tm = tm;
	return attr;
}
//
// ノード属性を設定
//
//  　A　　　　　 ：属性a
//  　C　　　　　 ：属性c
//  　I　　　　　 ：属性i
//  　Tm　　　　　：属性tm
//
//　　戻り値      : ELEMATTRIBUTES
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
// 属性tの値を取得
//
//  　attNodePtr  ：属性ノードMAPのポインタ
//  　c　　　　　 ：データタイプ（文字列）
//    n　　　     :データ要素数
//
//　　戻り値      : VARIANT_TRUE ；属性i有り
//　　            : VARIANT_FALSE；属性i無し
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
		for (c_size = 0; c_size< strlen(c); c_size++)	//数字部を抜き出す
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
// エンコードタイプの取得
//
//  　pDoc　　　  ：DOMドキュメントポインタ
//  　encode      ：属性encodeの値
//
//　　戻り値      : VARIANT_TRUE ；取得成功
//　　            : VARIANT_FALSE；取得失敗
//
VARIANT_BOOL getAttEncode(MSXML2::IXMLDOMDocument2Ptr pDoc, BSTR *encode)
{
	MSXML2::IXMLDOMNodePtr pData = pDoc->selectSingleNode(L"/root/message/Data");
	if (pData == NULL)
	{
		printf("xmlメッセージにDataタグがありません FILE=%s, LINE=%d", __FILE__, __LINE__);
		return VARIANT_FALSE;
	}

	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr = pData->Getattributes();

	if (attNodePtr->length >0)	//属性がある？
	{
		MSXML2::IXMLDOMNodePtr pEncode = attNodePtr->getNamedItem(L"encoding");
		if (pEncode != NULL) // 属性encodingはある？
		{
			*encode = pEncode->Gettext();
			return VARIANT_TRUE;
		}
	}
	return VARIANT_FALSE;
}

//messageタグの属性構造体を設定
MSGATTRIBUTES setMsgAttribute(BSTR type, BSTR name, BSTR cycle, BSTR a)
{
	MSGATTRIBUTES attr;

	attr.type = type;
	attr.name = name;
	attr.cycle = cycle;
	attr.a = a;

	return attr;
}