#ifndef DATAMANAGE_H
#define DATAMANAGE_H
#include <stdio.h>
#include <tchar.h>
#import  <msxml6.dll>  // MSXMLライブラリのインポート  
#include <list>
#include <map>
#include <string.h>
using namespace std; 

#include "XmlMessage.h"
#include "XmlMsgAttribute.h"
#include "XmlData.h"
#include "Base64Data.h"

#define SET_MAINDB (0) //
#define GET_MAINDB (0) //

#define MSG_TYPE_NG     (2)	  //メッセージタイプが不正値

//getElement関数系
#define ERR_DATA_ATT_T  (5)	  //データタイプの不整合
#define ERR_TAG_NAME    (6)  //タグ名のサイズが0
#define ERR_DATA_GET    (7)  //データ取得エラー
#define ERR_ARRAY_SIZE  (8)  //バッファサイズよりもコピーサイズが大
#define ERR_BASE64_COVERT (9)  //base64変換エラー


//getData関数
#define GET_ERR_MSG_TYPE   (MSG_TYPE_NG) //メッセージタイプが不正値
#define GET_ERR_NEW_XMLMSG (3) //XmlMessageクラスのインスタンスエラー
#define GET_ERR_NO_CID     (4) //クライアントIDに対応するメッセージがない

//
#define DEFAULT_SETMSG (L"<?xml version='1.0' encoding='UTF-8'?><root><message type='set'><Data></Data></message></root>")


enum dataAttrEncoding {text, base64,};
enum dataAttrArray {spaced, tagged,};

struct dataAttribute
{
	int mode; 
	int encoding;			//encoding属性　=0:text; =1;base64
	int array_delimiter;	//array属性　=0:spaced;=1;tagged
	int ID;					//ID
};

struct getMsg
{
	int path_idx;
	vector<_bstr_t> path;
	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr;
	MSXML2::IXMLDOMElementPtr pElement;
	int mode; //メッセージ処理モード
};

class dataDB
{
public:
	vector<XmlMessage> data; 

};

class dataManage
{
public:
	map< unsigned int, string> dbIDMap;

	dataManage()
	{
		dbIDMap.insert(make_pair(SET_MAINDB, ""));  //dbIDMap初期化(MAINのみ)

		HRESULT hr;
		class XmlMessage *xml = new XmlMessage(DEFAULT_SETMSG, &hr);	//データ更新
		saveData(xml, SET_MAINDB);
		delete xml;
	};
	~dataManage()
	{
		dbIDMap.clear();
	};

	void  saveData(XmlMessage *xml, unsigned int dataID);

	unsigned int getData(unsigned int clientID, BSTR *result);
	unsigned int getData(unsigned int clientID, BSTR name, BSTR *result);
	unsigned int addSetMsgData(unsigned int clientID, BSTR *result);
	unsigned int getDatabyDBID(unsigned int DataID, BSTR *result);
	unsigned int getMsgOnMainSetDB(unsigned int DataID, unsigned int clientID, BSTR *result);

	HRESULT setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, double value);
	HRESULT setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, float value);
	HRESULT setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, long double value);
	HRESULT setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, int value);
	HRESULT setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, short value);
	HRESULT setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, long long value);
	HRESULT setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, unsigned int value);
	HRESULT setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, unsigned short int value);
	HRESULT setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, unsigned long long value);
	HRESULT setOneElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, unsigned char value);

	////root/message/Data 以外のタグでのデータ設定
	HRESULT setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, double value);
	HRESULT setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, float value);
	HRESULT setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, long double value);
	HRESULT setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, int value);
	HRESULT setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, short value);
	HRESULT setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, long long value);
	HRESULT setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, unsigned int value);
	HRESULT setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, unsigned short int value);
	HRESULT setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, unsigned long long value);
	HRESULT setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, unsigned char value);
	HRESULT setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, char value);
	HRESULT setOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, LPCTSTR value);

	//mainlink sendメッセージよりデータ取得
	HRESULT getOneElement(unsigned int clientID, vector<_bstr_t>vpath, double *value);
	HRESULT getOneElement(unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, double *value);
	HRESULT getOneElement(unsigned int clientID, vector<_bstr_t>vpath, int *value);
	HRESULT getOneElement(unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, int *value);
	HRESULT getOneElement(unsigned int clientID, vector<_bstr_t>vpath, unsigned short *value);
	HRESULT getOneElement(unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, unsigned short *value);
	HRESULT getOneElement(unsigned int clientID, vector<_bstr_t>vpath, unsigned char *value);
	HRESULT getOneElement(unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, unsigned char *value);
	HRESULT getOneElement(unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, char value[1]);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, BSTR *value);

	HRESULT getOneElement(unsigned int clientID, vector<_bstr_t>vpath, ELEMATTRIBUTES attr, char value[1]);

	//メインDBよりデータ取得
	HRESULT getOneElement(vector<_bstr_t>vpath, double *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, LPCTSTR idx, double *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, int *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, LPCTSTR idx, int *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, unsigned short *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, LPCTSTR idx, unsigned short *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, unsigned char *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, LPCTSTR idx, unsigned char *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, LPCTSTR idx, char value[1]);

	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, double *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, int *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, unsigned short *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, unsigned char *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, char value[1]);

	//root/message/Data 以外のタグでのデータ取得
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, double *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, int *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned short *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned char *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, char value[1]);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, LPTSTR vlaue);

	//データ取得（配列）
	//  戻り値      : =S_OK          (0);正常終了
	//              : =S_FALSE       (1);異常終了
	//              : =ERR_DATA_ATT_T(5);データタイプの不整合
	//              : =ERR_TAG_NAME  (6);タグ名のサイズが0
	//			    : =ERR_DATA_GET  (7) ;データ取得エラー
	//			    : =ERR_ARRAY_SIZE(8) ;バッファサイズよりもコピーサイズが大
	template<typename T, int N> HRESULT getArrayElement(vector<_bstr_t>vpath, int num, T(&value)[N])
	{
		HRESULT hr;
		ELEMATTRIBUTES attr;

		attr = setAttributeACI(NULL, NULL, NULL);	//属性a, 属性c, 属性i
		hr = getArrayElement(vpath, attr, 0, num - 1,  value);

		return hr;
	}
	template<typename T, int N> HRESULT getArrayElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, int num, T(&value)[N])
	{
		HRESULT hr;

		hr = getArrayElement(vpath, attr, 0, num - 1,  value);

		return hr;
	}
	template<typename T, int N> HRESULT getArrayElement(vector<_bstr_t>vpath, int st, int ed, T(&value)[N])
	{
		HRESULT hr;
		ELEMATTRIBUTES attr;

		attr = setAttributeACI(NULL, NULL, NULL);	//属性a, 属性c, 属性i
		hr = getArrayElement(vpath, attr, st, ed, value);

		return hr;
	}
	
	template<typename T, int N> HRESULT getArrayElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, int st, int ed, T(&value)[N])
	{
		HRESULT hr;

		if (st > ed) return ERR_DATA_GET;
		if (st < 0 || ed < 0) return ERR_DATA_GET;
		int num = ed - st + 1;
		if (N < num)  //配列要素数が少ない
		{
			printf("GET:%s 配列の要素数が指定数より少ないです。\n", (char*)vpath.at(vpath.size()-1));
			return ERR_ARRAY_SIZE;
		}
		for (int i = st; i <= ed; i++)
		{
			TCHAR idx[8];
			_stprintf_s(idx, 8, _T("%d"), i);
			attr.i = _bstr_t(idx);

			hr = getOneElement(vpath, attr, &value[i - st]);
		}
		return hr;
	}

	unsigned int setDBID(BSTR attr_a);
	unsigned int setDBID(XmlMessage *xml);

	VARIANT_BOOL clearSetMsg(unsigned int dataID, int msgID);

private:
	class dataDB  setList;
	class dataDB  getList;
	class dataDB  mainLinkList;

	VARIANT_BOOL hasSameID(int ID, class dataDB list, unsigned int *listIdx);
	VARIANT_BOOL hasSameName(BSTR msgName, XmlMessage *archive, unsigned int *listIdx);
	
	VARIANT_BOOL getSetData(MSXML2::IXMLDOMElementPtr pParent, struct getMsg *str_query, MSXML2::IXMLDOMElementPtr* pLeaf);
	VARIANT_BOOL anaMessage(XmlMessage *setData, struct getMsg *str_query,MSXML2::IXMLDOMElementPtr *pLeaf);

    void putText(MSXML2::IXMLDOMElementPtr pParent, int encoding, struct getMsg str_query, BSTR str_temp);
	void lastBranch(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath,struct dataAttribute *dataAttr, int mode);
	void getInChild(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath, struct dataAttribute *dataAttr, int mode);
	void setDataTagAttr(MSXML2::IXMLDOMElementPtr elementPtr, struct dataAttribute* dataAttr);

	unsigned int setRequestData(XmlMessage *xml, int DATAID);
	unsigned int appendData(XmlMessage *request);
	
	unsigned int getData(unsigned int clientID, BSTR request, BSTR path, BSTR *result, int *attTID);

	HRESULT setElement(vector<_bstr_t>vpath, unsigned int array_size, unsigned int array_idx, LPCTSTR valtype, LPCTSTR pstr);
	HRESULT setElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned int array_size, LPCTSTR valtype, LPCTSTR pstr);

	HRESULT getElement(vector<_bstr_t>vpath, LPCTSTR idx, LPTSTR result);
	HRESULT getElement(unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, LPTSTR result, int* attTI);
	HRESULT getElement(LPCTSTR type, unsigned int clientID, vector<_bstr_t>vpath, LPCTSTR idx, LPTSTR result, int* attTID);
	HRESULT getElement(LPCTSTR type, unsigned int clientID, vector<_bstr_t>vpath, ELEMATTRIBUTES attr, LPTSTR result, int* attTID);
	
	HRESULT getElement(unsigned int clientID, vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, LPTSTR result, int* attTID);
	HRESULT getElementData(XmlData *xml, unsigned int clientID, BSTR path, LPTSTR result, int* attTID);

	void nodesEncoding(MSXML2::IXMLDOMElementPtr pParent, int encoding);

	void setArrayData(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath, struct dataAttribute *dataAttr, int mode, int st, int ed);

	void getLastBranchAttr(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath, struct dataAttribute *dataAttr, int mode);

	void lastBranchGetArray(MSXML2::IXMLDOMElementPtr pParent, vector<_bstr_t>vpath, struct dataAttribute *dataAttr, int mode, int st, int ed);

	VARIANT_BOOL appendDeepChild(MSXML2::IXMLDOMElementPtr pChild, struct dataAttribute *dataAttr, MSXML2::IXMLDOMElementPtr pParent);
};

#endif /**/
