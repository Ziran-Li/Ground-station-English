#include "stdafx.h"
#include "FlightLog.h"

//
//  飛行ログファイル情報解析
//
struct flightLog FlightLog::getFlightLogInfo(BSTR msgXml)
{
	struct flightLog tmp = { 0 };
	tmp.rec = REC_PLAY_NA;
	tmp.play = REC_PLAY_NA;
	tmp.ope = REC_PLAY_NA;
	tmp.guiMsg  = _bstr_t(msgXml);
	StrXml *msg = new StrXml(msgXml);

	//モードmap
	typedef map<_bstr_t, int>Table;
	Table tbl;

	tbl[REC_END]       = REC_MODE::END;
	tbl[REC_SAVE]      = REC_MODE::SAVE;

	tbl[PLAY_CLOSE]    = PLAY_MODE::CLOSE;
	tbl[PLAY_OPEN]     = PLAY_MODE::OPEN;
	tbl[PLAY_EDIT]     = PLAY_MODE::EDIT;

	tbl[PLAY_STOP]     = OPE_MODE::STOP;
	tbl[PLAY_PLAY]     = OPE_MODE::PLAY;
	tbl[PLAY_PAUSE]    = OPE_MODE::PAUSE;
	tbl[PLAY_NEXT]     = OPE_MODE::NEXT;
	tbl[PLAY_PREV]     = OPE_MODE::PREV;
	tbl[PLAY_SLIDEBAR] = OPE_MODE::SLIDEBAR;

	//Rec
	_bstr_t result_rec = msg->getData(L"root/message/FlightLog/Rec/FileMode");
	Table::iterator ret_rec = tbl.find(result_rec);
	if (ret_rec != tbl.end())
	{
		tmp.rec = ret_rec->second;
		if (tmp.rec == REC_MODE::SAVE) tmp.play = REC_PLAY_NA;
	}

	//Play
	_bstr_t result_play = msg->getData(L"root/message/FlightLog/Play/FileMode");
	Table::iterator ret_play = tbl.find(result_play);
	if (ret_play != tbl.end())
	{
		tmp.play = ret_play->second;
		if (tmp.play == PLAY_MODE::OPEN) tmp.rec = REC_PLAY_NA;
	}

	_bstr_t result_play_edit = msg->getData(L"root/message/FlightLog/Play/Edit/SaveFilePath");
	if (result_play_edit.length() > 0)  tmp.play = PLAY_MODE::EDIT;

	//OpeMode
	_bstr_t result_ope = msg->getData(L"root/message/FlightLog/Play/OpeMode");
	Table::iterator ret_ope = tbl.find(result_ope);
	if (ret_ope != tbl.end())
	{
		tmp.ope = ret_ope->second;
	}


	delete msg;
	return tmp;
}

double FlightLog::getPlayCurrentTm(BSTR msgXml)
{
	StrXml *msg = new StrXml(msgXml);
	_bstr_t result = msg->getData(L"root/message/FlightLog/Play/CurrentTime");
	delete msg;

	char*  curtime = _com_util::ConvertBSTRToString(result.GetBSTR());
	double ctime = atof(curtime);
	delete[] curtime;
    
	return ctime;
}

