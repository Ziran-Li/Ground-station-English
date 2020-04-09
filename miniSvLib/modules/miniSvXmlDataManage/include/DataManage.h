#ifndef DATAMANAGE_H
#define DATAMANAGE_H
#include <stdio.h>
#include <tchar.h>
#import  <msxml6.dll>  // MSXML���C�u�����̃C���|�[�g  
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

#define MSG_TYPE_NG     (2)	  //���b�Z�[�W�^�C�v���s���l

//getElement�֐��n
#define ERR_DATA_ATT_T  (5)	  //�f�[�^�^�C�v�̕s����
#define ERR_TAG_NAME    (6)  //�^�O���̃T�C�Y��0
#define ERR_DATA_GET    (7)  //�f�[�^�擾�G���[
#define ERR_ARRAY_SIZE  (8)  //�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
#define ERR_BASE64_COVERT (9)  //base64�ϊ��G���[


//getData�֐�
#define GET_ERR_MSG_TYPE   (MSG_TYPE_NG) //���b�Z�[�W�^�C�v���s���l
#define GET_ERR_NEW_XMLMSG (3) //XmlMessage�N���X�̃C���X�^���X�G���[
#define GET_ERR_NO_CID     (4) //�N���C�A���gID�ɑΉ����郁�b�Z�[�W���Ȃ�

//
#define DEFAULT_SETMSG (L"<?xml version='1.0' encoding='UTF-8'?><root><message type='set'><Data></Data></message></root>")


enum dataAttrEncoding {text, base64,};
enum dataAttrArray {spaced, tagged,};

struct dataAttribute
{
	int mode; 
	int encoding;			//encoding�����@=0:text; =1;base64
	int array_delimiter;	//array�����@=0:spaced;=1;tagged
	int ID;					//ID
};

struct getMsg
{
	int path_idx;
	vector<_bstr_t> path;
	MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr;
	MSXML2::IXMLDOMElementPtr pElement;
	int mode; //���b�Z�[�W�������[�h
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
		dbIDMap.insert(make_pair(SET_MAINDB, ""));  //dbIDMap������(MAIN�̂�)

		HRESULT hr;
		class XmlMessage *xml = new XmlMessage(DEFAULT_SETMSG, &hr);	//�f�[�^�X�V
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

	////root/message/Data �ȊO�̃^�O�ł̃f�[�^�ݒ�
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

	//mainlink send���b�Z�[�W���f�[�^�擾
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

	//���C��DB���f�[�^�擾
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

	//root/message/Data �ȊO�̃^�O�ł̃f�[�^�擾
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, double *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, int *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned short *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, unsigned char *value);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, char value[1]);
	HRESULT getOneElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, MSGATTRIBUTES msg_attr, LPTSTR vlaue);

	//�f�[�^�擾�i�z��j
	//  �߂�l      : =S_OK          (0);����I��
	//              : =S_FALSE       (1);�ُ�I��
	//              : =ERR_DATA_ATT_T(5);�f�[�^�^�C�v�̕s����
	//              : =ERR_TAG_NAME  (6);�^�O���̃T�C�Y��0
	//			    : =ERR_DATA_GET  (7) ;�f�[�^�擾�G���[
	//			    : =ERR_ARRAY_SIZE(8) ;�o�b�t�@�T�C�Y�����R�s�[�T�C�Y����
	template<typename T, int N> HRESULT getArrayElement(vector<_bstr_t>vpath, int num, T(&value)[N])
	{
		HRESULT hr;
		ELEMATTRIBUTES attr;

		attr = setAttributeACI(NULL, NULL, NULL);	//����a, ����c, ����i
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

		attr = setAttributeACI(NULL, NULL, NULL);	//����a, ����c, ����i
		hr = getArrayElement(vpath, attr, st, ed, value);

		return hr;
	}
	
	template<typename T, int N> HRESULT getArrayElement(vector<_bstr_t>vpath, ELEMATTRIBUTES attr, int st, int ed, T(&value)[N])
	{
		HRESULT hr;

		if (st > ed) return ERR_DATA_GET;
		if (st < 0 || ed < 0) return ERR_DATA_GET;
		int num = ed - st + 1;
		if (N < num)  //�z��v�f�������Ȃ�
		{
			printf("GET:%s �z��̗v�f�����w�萔��菭�Ȃ��ł��B\n", (char*)vpath.at(vpath.size()-1));
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
