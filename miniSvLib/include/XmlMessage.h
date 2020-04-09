#ifndef XMLMESSGE_H
#define XMLMESSGE_H

#include <vector> 
#import  <msxml6.dll>  // MSXML���C�u�����̃C���|�[�g  
using namespace std; 

#define ROOT_KEY		(L"root")
#define DEFAULT_CYCLE	(50)	 //�����F�ʐM�����f�t�H���g�l

typedef MSXML2::IXMLDOMElementPtr	IElementPtr;
typedef MSXML2::IXMLDOMNodeListPtr	INodeListPtr;
typedef MSXML2::IXMLDOMNamedNodeMapPtr INamedNodeMapPtr;
typedef MSXML2::IXMLDOMNodePtr      INodePtr;

class XmlMessage
{
	
public:

	enum MsgType{ msgSet, msgGet, msgClose, msgMainLink ,msgTypeErr, }; //���b�Z�[�W�^�C�vID
	enum arrangeMode{ replyGetMsg, modifiedSet, modifiedGet, replyMainLinkMsg, modeErr, }; //�f�[�^�A�����W���[�h

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

	MSXML2::IXMLDOMDocument2Ptr pDoc;	//DOM�h�L�������g�|�C���^

	int getCycle(int Idx); 
	int getMsgType(int msgNo);			//���b�Z�[�W�^�C�v�̎擾
	int setMsgType(int msgNo, int msg_type);//���b�Z�[�W�^�C�v�̐ݒ�
	_bstr_t getMsgAttA(int msgNo);	//����a�̎擾

	VARIANT_BOOL hasSameName(BSTR name, unsigned int *listIdx);

	unsigned int getDataID();
	void setDataID(unsigned int ID){dataID = ID;};

    vector<_bstr_t>list_name;			//name���X�g

	void removeDisenable();

private:
	unsigned int dataID;				//�N���C�A���gID
    vector<int>list_msg;				//���b�Z�[�W�^�C�vID���X�g
    vector<int>list_cycle;				//�������X�g
    vector<_bstr_t>list_a;				//a�������X�g

	BSTR getName(int nameNo);	//���g�p

	HRESULT loadXmlMsg(BSTR quary);		//���b�Z�[�W�̃��[�h
	VARIANT_BOOL removeEachDisenable(const MSXML2::IXMLDOMNodePtr pParent);
};



#endif /**/
