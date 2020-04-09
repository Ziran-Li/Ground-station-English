#ifndef FLIGHTLOG_H
#define FLIGHTLOG_H
#include <stdio.h>
#include <tchar.h>
#import  <msxml6.dll>  // MSXML���C�u�����̃C���|�[�g  
#include "DataManage.h"
#include "XmlMsgAttribute.h"
#include "StrXml.h"

#define  REC_END        (L"END")
#define  REC_SAVE       (L"SAVE")

#define  PLAY_CLOSE     (L"CLOSE")
#define  PLAY_OPEN      (L"OPEN")
#define  PLAY_EDIT      (L"EDIT")

#define  PLAY_STOP      (L"STOP")
#define  PLAY_PLAY      (L"PLAY")
#define  PLAY_PAUSE     (L"PAUSE")
#define  PLAY_NEXT      (L"NEXT")
#define  PLAY_PREV      (L"PREV")
#define  PLAY_SLIDEBAR  (L"SLIDEBAR")
#define  REC_PLAY_NA    (-1)
enum  REC_MODE {END, SAVE };
enum  PLAY_MODE{CLOSE, OPEN, EDIT };
enum  OPE_MODE {STOP, PLAY, PAUSE, NEXT, PREV, SLIDEBAR };

struct flightLog
{

	int rec;		//�ۑ��R�}���h =-1;NA, =0;END  , 1;SAVE
	int play;		//�Đ��R�}���h =-1;NA, =0;CLOSE, 1;OPEN, =2;EDIT
	int ope;		//����R�}���h =-1;NA, =0;STOP , 1;PLAY ,2;PAUSE,3;NEXT, 4:PREV, 5;SLIDEBAR
	_bstr_t guiMsg; //�@GUI����̃��b�Z�[�W
};

//
//�@FlightLog �N���X
//
class FlightLog
{
private:
	void loadStrXml(BSTR bstrXML);

public:
	static  struct flightLog getFlightLogInfo(BSTR msgXml);
	static double getPlayCurrentTm(BSTR msgXml);
};


#endif /**/

