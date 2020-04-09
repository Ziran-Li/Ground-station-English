#ifndef XMLMSGATTRIBUTE_H
#define XMLMSGATTRIBUTE_H

#include "XmlMessage.h"

#define SEND_ENABLE		(L"enabled")
#define SEND_DISABLE	("disabled")


typedef struct attributes
{
	_bstr_t a;	//ëÆê´a
	_bstr_t c;	//ëÆê´b
	_bstr_t i;	//ëÆê´i
	_bstr_t tm;	//ëÆê´tm
}ELEMATTRIBUTES;

typedef struct attributesM
{
	_bstr_t type;
	_bstr_t name;
	_bstr_t cycle;
	_bstr_t a;

}MSGATTRIBUTES;


VARIANT_BOOL getAttSendValue(MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr);
VARIANT_BOOL getAttIValue(MSXML2::IXMLDOMNamedNodeMapPtr attNodePtr, int *idx);
VARIANT_BOOL matchAttrs(INamedNodeMapPtr attNodePtr1, INamedNodeMapPtr attNodePtr2, _bstr_t keyword);
BSTR getAttName(INamedNodeMapPtr attNodePtr);
BSTR getAttName(MSXML2::IXMLDOMDocument2Ptr pDoc, unsigned int msgNo);
VARIANT_BOOL setAttType(MSXML2::IXMLDOMDocument2Ptr pDoc, BSTR type);
VARIANT_BOOL getAttA(INamedNodeMapPtr attNodePtr, _bstr_t *name);
VARIANT_BOOL setAttEncode(MSXML2::IXMLDOMDocument2Ptr pDoc, BSTR encode);
ELEMATTRIBUTES setAttributeACI(BSTR A, BSTR C, BSTR I);
ELEMATTRIBUTES setAttributeACITm(BSTR A, BSTR C, BSTR I, BSTR Tm);
VARIANT_BOOL getAttTValue(MSXML2::IXMLDOMNodePtr pAttT, char *a, int *tnum);
VARIANT_BOOL getAttEncode(MSXML2::IXMLDOMDocument2Ptr pDoc, BSTR *encode);
MSGATTRIBUTES setMsgAttribute(BSTR type, BSTR name, BSTR cycle, BSTR a);

#endif /**/
