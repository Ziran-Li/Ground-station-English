#include "stdafx.h"

#include <shlwapi.h>
#include <time.h>
#include <tchar.h>
#include <fstream>
#include <iostream>
#include <string>
#pragma comment(lib, "shlwapi.lib") 
#include "kmlLogger.h"

#define ERROR_MSG_SIZE   (256)
#define ERR_VALUE		 (L"none")

extern "C"
{
	int minizip(char *zipfile, char *filename);
	int miniunz(char *zipfile);
}

kmlLogger::kmlLogger(void)
{
	fileBaseName = _bstr_t("FlightLog");
	opePlayOn    = VARIANT_FALSE;
	playOn       = VARIANT_FALSE;
	eof          = VARIANT_FALSE;
}

kmlLogger::kmlLogger(BSTR mode) 
{
	fileBaseName = _bstr_t("FlightLog") ;
	openmode     = mode; 	
	opePlayOn    = VARIANT_FALSE;
	playOn       = VARIANT_FALSE;
	eof          = VARIANT_FALSE;
}

kmlLogger::~kmlLogger(void)
{
}

//
// 初期化
//
//　戻り値：=0;正常終了
//          ≠0；エラーコード
//
int kmlLogger::Initialize()
{

	if(openmode == _bstr_t(L"w"))
	{
		//現在日時の取得
		time_t now = time(NULL);
		char buff[128]="";
		setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
		struct tm now_time;                 // ポインタから、変数実体に変更
		localtime_s(&now_time, &now);
		sprintf_s(buff,"_%04d%02d%02d_%02d%02d_%02d",now_time.tm_year+1900,
			now_time.tm_mon + 1,now_time.tm_mday,now_time.tm_hour,now_time.tm_min,now_time.tm_sec);

		dirName= _bstr_t("./") ;
		fileName = fileBaseName + _bstr_t(buff);

		kmlFilePath = fileName + _bstr_t(".kml");
		//ファイルOPEN
		return Open(kmlFilePath, openmode);
	}
	else //readモード
	{
		return Open(filePath, openmode);
	}
}

//
// Xml文字列の読込み
//
HRESULT  kmlLogger::loadStrXml(BSTR bstrXML)
{
	MSXML2::IXMLDOMDocument2Ptr pDoc;	//DOMドキュメントポインタ

	VARIANT_BOOL isSuccess = VARIANT_FALSE;

	//Create the XmlDocument
	HRESULT hr  = pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument60) );
    if (FAILED(hr)) 
    {
		printf("Failed to instantiate an XML DOM.\n");
        return S_FALSE; 
    }
	pDoc->async = VARIANT_FALSE;	

	//Load the XML string

	HRESULT hr1  = pDoc->raw_loadXML(bstrXML, &isSuccess);
    if (FAILED(hr1)) 
	{
		MSXML2::IXMLDOMParseErrorPtr pErr;
		BSTR bstrErr = NULL;
		pDoc->get_parseError(&pErr);
		pErr->get_reason(&bstrErr);
		printf("XML DOM loaded errror \n",bstrErr);
		::SysFreeString(bstrErr);

		return S_FALSE; 
	} 

	//Lon
	coordinate = getData(pDoc, _bstr_t(KLOG_PATH_DATA) + _bstr_t(KLOG_PATH_LON));
    coordinate += L",";

	//Lat
	coordinate += getData(pDoc, _bstr_t(KLOG_PATH_DATA) + _bstr_t(KLOG_PATH_LAT));
    coordinate += L",";

	//Alt
	coordinate += getData(pDoc, _bstr_t(KLOG_PATH_DATA) + _bstr_t(KLOG_PATH_ALT));
	 
	//GCS
	int yyyy = atoi(getData(pDoc, _bstr_t(KLOG_PATH_DATA) + _bstr_t(KLOG_PATH_YEAR)));
	int mm = atoi(getData(pDoc, _bstr_t(KLOG_PATH_DATA) + _bstr_t(KLOG_PATH_MON)));
	int dd = atoi(getData(pDoc, _bstr_t(KLOG_PATH_DATA) + _bstr_t(KLOG_PATH_DAY)));
	int hh = atoi(getData(pDoc, _bstr_t(KLOG_PATH_DATA) + _bstr_t(KLOG_PATH_HR)));
	int mn = atoi(getData(pDoc, _bstr_t(KLOG_PATH_DATA) + _bstr_t(KLOG_PATH_MIN)));
	int ss = atoi(getData(pDoc, _bstr_t(KLOG_PATH_DATA) + _bstr_t(KLOG_PATH_SEC)));
	int ms = atoi(getData(pDoc, _bstr_t(KLOG_PATH_DATA) + _bstr_t(KLOG_PATH_MSEC)));

	char buff_day[11] = "";
	sprintf_s(buff_day, "%04d-%02d-%02d", yyyy, mm, dd);
	char buff_time[13] = "";
	sprintf_s(buff_time, "%02d:%02d:%02d.%03d", hh, mn, ss, ms);
	
	BSTR bday = _com_util::ConvertStringToBSTR(buff_day);
	BSTR btime = _com_util::ConvertStringToBSTR(buff_time);
	data_day = _bstr_t(bday);
	data_time = _bstr_t(btime);
	SysFreeString(bday);
	SysFreeString(btime);

	//ExtendedData
	extendedData.~_bstr_t();
	MSXML2::IXMLDOMNodePtr xl4 = pDoc->selectSingleNode(KLOG_PATH_DATA);
	if (xl4)
    {
		TCHAR *szFinal = _tcsdup(xl4->Getxml());
		size_t xmlLen  = _tcslen(szFinal) + 1;
		TCHAR *szOut   = new TCHAR[xmlLen];
		ZeroMemory( szOut, xmlLen );
		size_t result, pos = 0;
		TCHAR  *tchar_tmp=NULL, *tmp=NULL, *token = NULL;
		
		tmp = _tcstok_s(szFinal, _T("\t\r\n"), &token);
		while(tmp!=NULL)
		{			
			result = (size_t)(token-tmp)-1;			
			_tcsncpy_s( szOut+pos, xmlLen-pos, tmp,  result);
			pos      += result;
			tchar_tmp = tmp;
			tmp       = _tcstok_s(NULL, _T("\t\r\n"), &token);
		}
		_tcsncpy_s( szOut+pos, xmlLen-pos, &tchar_tmp[result],  1);
		extendedData = _bstr_t(szOut);

		delete[] szFinal;
		delete[] szOut;
	}
    else
    {
		extendedData = L"";
        printf("ExtendedDataデータがない");
    }

	pDoc.Release();
	return S_OK;

}


 _bstr_t kmlLogger::getData(MSXML2::IXMLDOMDocument2Ptr pDoc, _bstr_t path )
{
	_bstr_t  val = ERR_VALUE;
	 
	MSXML2::IXMLDOMNodePtr xl =pDoc->selectSingleNode(path);
    if (xl)
    {
		val = xl->Gettext();
    }

	return val;
}

//
// ファイル書込み
//
//　戻り値：=0；正常終了
//          <0；ファイル書込みエラー
//
int kmlLogger::Write(BSTR bstr)
{

	int iRet =fwprintf_s(fp, L"%s", bstr);
	if(iRet < 0 )  //戻り値が負ならば、エラー
	{
		printf("ファイル書込みエラー FILE = %s , LINE = %d\n",
				 __FILE__,__LINE__);
		return iRet;
	}
	
	return  0;
}

//
// ファイルオープン
//
//　戻り値：=0;ファイルオープン成功
//          ≠0；エラーコード
//
int kmlLogger::Open(BSTR filePath, BSTR mode)
{
	char error_msg[ERROR_MSG_SIZE];
	setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
	errno_t errorcode = _wfopen_s(&fp, filePath, mode);

	if(errorcode != 0 )
	{
        if(openmode == _bstr_t(L"r") && PathIsDirectory(filePath)==false)
		{
            //読み込むファイルが元々無いのでエラーではない.
            //ただし、errorcodeには0以外の値が入っている。
		}
		else
		{
    		strerror_s(error_msg , ERROR_MSG_SIZE, errorcode);
	    	printf("ファイルオープンエラー（%s）。FILE = %s , LINE = %d\n",
				error_msg , __FILE__,__LINE__);
		}
	}
	else if(openmode == _bstr_t(L"w"))
	{
		Write(L"<?xml version=\"1.0\" encoding=\"shift_jis\"?>\n");
		Write(L"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
		Write(L"<Document>\n");
		Write(L"<name>"+getFilename()+L"</name>\n");
	 }

	return errorcode;
}


//
// ファイルクローズ
//
//　戻り値：=0;正常終了
//          ≠0；エラーコード
//
int kmlLogger::Close()
{
	int ret = VARIANT_FALSE;
	if(openmode == _bstr_t(L"w"))
	{
		Write(L"</Document>\n</kml>\n");
	}

	fclose(fp);
	
	if (openmode == _bstr_t(L"w"))
	{

		// KMLからKMZへ変換
		char *kmlfile = _com_util::ConvertBSTRToString(kmlFilePath.GetBSTR());
		char *filename_try = _com_util::ConvertBSTRToString(kmlFilePath.GetBSTR());
		size_t  len = strlen(filename_try);
		filename_try[len - 1] = 'z';

		// TODO:KMZ解凍時のファイル名解読不可の課題が解決するまで保留
		//// 圧縮
		//if (kmlToKmz(filename_try, kmlfile))
		//{
		//	// 圧縮したので不要なKMLファイルを削除
		//	remove(kmlfile);

		//	ret = VARIANT_TRUE;
		//}
		//else
		//{
		//	//圧縮失敗
		//	ret = VARIANT_FALSE;
		//}
		ret = VARIANT_TRUE;

		delete[] kmlfile;
		delete[] filename_try;
		
	}

	return ret;
}

//
// ファイルサイズ取得
//
//　戻り値：ファイルサイズ
//
fpos_t kmlLogger::getFileSize()
{
	fpos_t fsize = 0;
	fpos_t fsizeb = fseek(fp, 0, SEEK_END);
	fgetpos(fp, &fsize);

	return fsize;
}
//
// １セットデータ作成
//
void kmlLogger::WriteOneData(BSTR bstrSetXML)
{

	if (getFileSize() > divideSizeInByte)
	{

		Close(); //現在開いているファイルのクローズ

		Initialize();  //ファイルの初期化（オープン＆ヘッダ部出力）
	}

	//１データセットをファイルに書込み
	OneData(bstrSetXML);

}
//
// １セットデータ作成
//
void kmlLogger::OneData(BSTR bstrSetXML)
{	
	loadStrXml(bstrSetXML);

	Write(L"<Placemark>") ;

	Write(L"<TimeStamp><when>");	////TODO タイムスタンプをどうするか？				 
	_bstr_t when = getDate()+"T"+getTime()+"+09:00";
	Write(when);
	Write(L"</when></TimeStamp>");


	Write(L"<Point>");
	
	Write(L"<altitudeMode>absolute</altitudeMode>") ;

	Write(L"<coordinates>"+getLLA()+L"</coordinates>");

	Write(L"</Point>");

	
	Write(L"<ExtendedData>");	
	Write(extendedData.GetBSTR());
	Write(L"</ExtendedData>");

	Write(L"</Placemark>\n") ;

}
//
_bstr_t kmlLogger::getFilename() 
{ 
	return fileName;
}
//
_bstr_t kmlLogger::getLLA() 
{
	return coordinate;
}
//
void kmlLogger::getExtendedData(BSTR *xmlout)
{
	*xmlout = extendedData;
}
//
_bstr_t kmlLogger::getDate()
{
	return data_day;
}
//
_bstr_t kmlLogger::getTime()
{
	return data_time;
}
//
void kmlLogger::setFilename(BSTR fname) 
{ 
	if (fname != _bstr_t(L""))
	{
		fileBaseName = fname + _bstr_t("FlightLog");
	}
}
//
void kmlLogger::setFilepath(BSTR path) 
{ 
	filePath = _bstr_t(path);
}
//
void kmlLogger::setDivedeSize(BSTR size)
{
	divideSizeInByte = KLOG_FILE_SIZE;
	if (size != _bstr_t(L""))
	{
		char *div = _com_util::ConvertBSTRToString(size);
		divideSizeInByte = atoi(div);
		delete[] div;
	}
}

//
//　ファイル解析
//
//
int kmlLogger::InitRead()
{
	VARIANT_BOOL init=VARIANT_TRUE;
	std::string str;
	struct tm wtime = { 0 };
	double time_ms  =0.0;

	setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
	char *path = _com_util::ConvertBSTRToString(filePath);
	ifs.open(path, ios_base::in);
	delete[] path;
	if (!ifs)
	{
		cout << "Error: cannot open file(" << filePath << ")" << endl;
		info->totalTime = 0;
		return VARIANT_FALSE;

	}
	ifstream::pos_type pre_pos = ifs.tellg();
	_bstr_t  cFlightMode, cControlMode;
	while (getline(ifs, str))
	{
		if(strstr(str.c_str(), "<Placemark><TimeStamp><when>")!=NULL)
		{
			std::string header = "<?xml version='1.0' encoding='UTF-8'?><root><message type='set'><Data>";
			std::string footer = "</Data></message></root>";
			_bstr_t ContorlMode, Mode;
			{
				std::string xmlString = header + str + footer;
				BSTR xmlBstr = _com_util::ConvertStringToBSTR(xmlString.c_str());

				StrXml *xmlTxt = new StrXml(xmlBstr);

				//ContorlMode
				ContorlMode = xmlTxt->getData(_bstr_t(KLOG_PATH_EXTENDED) + _bstr_t(KLOG_PATH_CMODE));
				//Mode
				Mode = xmlTxt->getData(_bstr_t(KLOG_PATH_EXTENDED) + _bstr_t(KLOG_PATH_MODE));
				//GCS
				wtime = { 0 };
				wtime.tm_year = atoi(xmlTxt->getData( _bstr_t(KLOG_PATH_EXTENDED) + _bstr_t(KLOG_PATH_YEAR))) - 1900;
				wtime.tm_mon = atoi(xmlTxt->getData(_bstr_t(KLOG_PATH_EXTENDED) + _bstr_t(KLOG_PATH_MON))) - 1;
				wtime.tm_mday = atoi(xmlTxt->getData(_bstr_t(KLOG_PATH_EXTENDED) + _bstr_t(KLOG_PATH_DAY)));
				wtime.tm_hour = atoi(xmlTxt->getData(_bstr_t(KLOG_PATH_EXTENDED) + _bstr_t(KLOG_PATH_HR)));
				wtime.tm_min = atoi(xmlTxt->getData( _bstr_t(KLOG_PATH_EXTENDED) + _bstr_t(KLOG_PATH_MIN)));
				wtime.tm_sec = atoi(xmlTxt->getData(_bstr_t(KLOG_PATH_EXTENDED) + _bstr_t(KLOG_PATH_SEC)));
				time_ms = atof(xmlTxt->getData(_bstr_t(KLOG_PATH_EXTENDED) + _bstr_t(KLOG_PATH_MSEC)));

				delete xmlTxt;
				SysFreeString(xmlBstr);

			}
			//ファイルポインタ位置
			currrent_pos = ifs.tellg();	////ファイルポインタ位置

			//初回？
			if(init)
			{
				init = VARIANT_FALSE;

				//初回ならば初期時刻の保存
				info->time0    = wtime;
				info->time0_ms = time_ms;

				//FlightModeとControlMode保存
				cFlightMode  = Mode;
				cControlMode = ContorlMode;

				//モード切替え
				info->vfpos.push_back(pre_pos);
				info->vtime.push_back(0.0);
				info->vFlightMode.push_back(Mode);
				info->vControlMode.push_back(ContorlMode);

				//全データ
				info->all_fpos.push_back(pre_pos);
				info->all_time.push_back(0.0);
			}
			else
			{
				double t = getTime(&wtime, time_ms, &info->time0, info->time0_ms);

				//FlightModeかControlModeが前回値から変更あり？
				if (cFlightMode != Mode || cControlMode != ContorlMode)
				{
					//変更ありならばvectorに保存
					info->vfpos.push_back(pre_pos);
					info->vtime.push_back(t);
					info->vFlightMode.push_back(Mode);
					info->vControlMode.push_back(ContorlMode);
				}

				//現在値の保存
				cFlightMode.~_bstr_t();
				cControlMode.~_bstr_t();
				cFlightMode = Mode;
				cControlMode = ContorlMode;

				info->all_fpos.push_back(pre_pos);
				info->all_time.push_back(t);
			}
		}
		pre_pos = ifs.tellg();

	}

    //ファイル終端
	info->totalTime = getTime(&wtime, time_ms, &info->time0, info->time0_ms);

    //データ個数
	info->data_num = info->all_fpos.size();
	info->mode_num = info->vfpos.size();

	//ファイルの先頭に戻す
	moveToTopOfFile();

	return VARIANT_TRUE;
 }

 //
 // 1データセット（xml）の取得
 //
 _bstr_t kmlLogger::getOneData()
 {

	 ifs.clear();
	 ifs.seekg(currrent_pos);

	 const char    *sp, *ep;
	 std::string str;
	 while (getline(ifs, str))
	 {
		 sp = strstr(str.c_str(), "<ExtendedData>");
		 if (sp != NULL)
		 {
			 //ExtendedDatatタグの内容を抽出
			 size_t s_size = sizeof("<ExtendedData>") -1;
			 ep = strstr(sp, "</ExtendedData>");
			 size_t len = ep - sp - s_size;
			 char *tmp = (char *)malloc(sizeof(char)*(len+1));
			
			 strncpy_s(tmp, len+1, sp + s_size, len);
			 std::string content = tmp;
			 std::string header = "<?xml version='1.0' encoding='UTF-8'?><root><message type='set'>";
			 std::string footer = "</message></root>";

			 //カレントタイムの更新
			 std::string xmlStringRaw = header + content + footer;
			 struct tm wtime;
			 BSTR  extend = _com_util::ConvertStringToBSTR(xmlStringRaw.c_str());
			 info->cTime = getDltTime(extend,KLOG_PATH_DATA, &wtime);
			 SysFreeString(extend);

			 //カレントタイム付きでデータセット設定
			 std::string xmlString = header+getCurrentTm()+content + footer;
			 BSTR bstr = _com_util::ConvertStringToBSTR(xmlString.c_str());
			 _bstr_t xml = _bstr_t(bstr);
			 SysFreeString(bstr);

			 currrent_pos = ifs.tellg();//ファイルポインタ位置

			 delete[] tmp;

             return xml;
		 }
	 }
	 if (ifs.eof())	eof = VARIANT_TRUE; //ファイルEOF

	 return _bstr_t(L"");
 }

 //
 // バイナリへ変換
 //
 _bstr_t kmlLogger::getOneBinData()
 {
	 if (oneDataSet == _bstr_t(L"")) return oneDataSet;

	 //base64変換
	 StrXml *xmlTxt = new StrXml(oneDataSet.GetBSTR());
	 _bstr_t xmlBin = xmlTxt->convertTxt2Bin();
	 delete xmlTxt;
	 return xmlBin;
 }

 //
 //　現時刻直近のモードインデックス取得
 //　
 int kmlLogger::getVecIndex(double ctm)
 {
	 int vecpos = 0;	//仮設定

	 for (unsigned int i = 1; i < info->mode_num; i++)
	 {
		 vecpos = i - 1;	//vectorインデックス設定
		 if (ctm < info->vtime.at(i))
		 {
			 break;
		 }
	 }

	 currrent_pos = info->vfpos.at(vecpos);

	 return vecpos;
 }
 //
 //　ファイル再生時間
 //　
 void kmlLogger::getDataByTime(double ctm, VARIANT_BOOL lessThan)
 {
	 size_t vpos = 0;	//仮設定

	 ifstream::pos_type pre_pos = currrent_pos;
	 ifstream::pos_type pre2_pos = currrent_pos;

	 size_t size = info->all_fpos.size();

	 while (vpos < size)
	 {
		 currrent_pos = info->all_fpos.at(vpos);
		 double dt = info->all_time.at(vpos);
	
		 if (dt >= ctm)
		 {
			 if(lessThan)
			 {
				 if (dt > ctm)
				 {
					 //前回値をカレントとする
					 currrent_pos = pre_pos;
					 break;
				 }
				 else
				 {
					 //現在値をカレントとする
					 break;
				 }
			 }
			 else
			 {
				 //現在値をカレントとする
				 break;
			 }
		 }
		 pre_pos = currrent_pos;

		 vpos++;
	 }
 }
 //
 //　ファイルEOF
 //　
 VARIANT_BOOL kmlLogger::isEof()
 {
	 return eof;
 }
 //
 //　再生操作モードON
 //　
 void kmlLogger::setOpePlayOn(int sw)
 {
	 if (!playOn)
	 {
		 opePlayOn = 0;
	 }
	 else
	 {
		 opePlayOn = sw;
	 }

 }
 //
 //　再生操作モード取得
 //　
int kmlLogger::isPlaying()
 {
	 return opePlayOn;
 }
//
//　次のデータセットを設定
//　
void  kmlLogger::setNextData()
{
	_bstr_t next = getOneData(); 
	oneDataSet = next;

}
 //
 //　再生
 //　
 _bstr_t  kmlLogger::getData()
 {
	 _bstr_t current;

	 if (opePlayOn)//再生ON?
	 {
		 if (isEof())
		 {
            //STOP処理
			 playOperation(OPE_MODE::STOP, NULL);
			 //カレント位置のデータ（バイナリ変換）を取得
			 current = getOneBinData();
             //停止
			 setOpePlayOn(KLOG_PAUSE);
		 }
		 else if (opePlayOn == KLOG_PLAY_ONCE)
		 {
            //カレント位置のデータ（バイナリ変換）を取得
			 current = getOneBinData();
			 //停止
			 setOpePlayOn(KLOG_PAUSE);
		 }
		 else
		 {
	    	 //カレント位置のデータ（バイナリ変換）を取得
    		 current = getOneBinData();
		     //ファイル読込みを進める
		    setNextData(); 
		 }

		 return current;
	 }
	 else
	 {
		 return L"";
	 }
 }
 //
 // setメッセージに応じた
 //    Playモード、Opeモード設定とデータ設定
 //
 VARIANT_BOOL kmlLogger::setPlayOpeMode(BSTR xml, struct flightLog *log)
 {
	 setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
//	 struct flightLog log = FlightLog::getFlightLogInfo(xml);
	 *log = FlightLog::getFlightLogInfo(xml);
	 VARIANT_BOOL cmdResult = VARIANT_FALSE;

	 if (log->play == PLAY_MODE::OPEN)	//再生
	 {
		 if (!playOn)//ファイルが閉じていたら開く
		 {
			 info = new fileInfo();

			 //ファイルパス取得
			 StrXml *guiMsg = new StrXml(log->guiMsg);
			 setFilepath(guiMsg->getData(L"root/message/FlightLog/Play/FilePath"));
			 delete guiMsg;


			 //KMZからKMLへ変換
			 char *kmzfile = _com_util::ConvertBSTRToString(filePath.GetBSTR());
			 //ファイルの拡張子判定
			 fileIsKmz = isKmzFile(kmzfile);
			 if (fileIsKmz)
			 {
				 if (!kmzToKml(kmzfile))
				 {
					 delete[] kmzfile;
					 playOn = VARIANT_FALSE;//操作モードをFALSEにする
					 return VARIANT_FALSE;
				 }
				 //OPENファイル名
				 size_t  len = strlen(kmzfile);
				 char *filename_try = (char *)malloc(sizeof(char)*(len + 1));
				 strncpy_s(filename_try, len + 1, kmzfile, _TRUNCATE);
				 filename_try[len - 1] = 'l';//拡張子修正

				 BSTR bpath = _com_util::ConvertStringToBSTR(filename_try);
				 setlocale(LC_ALL, "Japanese_Japan.932");	//<-追加
				 kmlFilePath = _bstr_t(bpath);
				 setFilepath(kmlFilePath.GetBSTR());
				 SysFreeString(bpath);
				 delete[] filename_try;
			 }
			 else //読み込むファイルはKML
			 {
				 setFilepath(filePath.GetBSTR());
				 kmlFilePath = filePath;
			 }
			 delete[] kmzfile;

			 //ファイル読込み初期化
			 if (InitRead() == VARIANT_TRUE)
			 {
				 playOn = VARIANT_TRUE;
				 oneDataSet = getOneData();
				 cmdResult = VARIANT_TRUE;

			 }
		 }
	 }
	 else if (log->play == PLAY_MODE::CLOSE)
	 {
		 if (playOn)//ファイルが開いていたら閉じる
		 {
			 ifs.close();

			 if (fileIsKmz) //
			 {
				 //KMLファイルは再生が終了したら不要なので消す
				 remove(kmlFilePath);
			 }

			 playOperation(OPE_MODE::STOP, NULL);
			 cmdResult = VARIANT_TRUE;

			 delete info;
		 }
		 playOn = VARIANT_FALSE;//操作モードをFALSEにする

		 info->cTime = 0.0;//カレントタイムのリセット
		 eof = VARIANT_FALSE;   

	 }
	 else if (log->play == PLAY_MODE::EDIT)
	 {
		 if (playOn)//ファイルが開いていたら編集
		 {
			 cmdResult = editFile(*log);
		 }
	 }

	 if (log->ope >= OPE_MODE::STOP) //操作コマンド
	 {
		 cmdResult = playOperation(log->ope, log->guiMsg);
	 }

	 return cmdResult;
 }
 //
 // 再生/操作モード
 //
 VARIANT_BOOL kmlLogger::playOperation(int opemode, BSTR xml)
 {
	 VARIANT_BOOL result = VARIANT_FALSE;

	 if (!playOn) return result;	//再生モードがCLOSEならばすぐにリターン

	 double tm;

	 switch (opemode)
	 {
	 case OPE_MODE::STOP: //ログ停止
		
		 //ファイル先頭に戻す
		 moveToTopOfFile();
         //先頭データ取得
		 oneDataSet = getOneData();
		 //1回だけPLAY
		 setOpePlayOn(KLOG_PLAY_ONCE);

		 //モードリストのインデックスを0クリア
		 info->vpos = 0;   

		 result = VARIANT_TRUE;	//操作有効
		 break;

	 case OPE_MODE::PLAY: //ログ再生

		 if (opePlayOn) break;	//既に再生中ならば何もしない

		 //モード切り替え判定。次モードならば、vposをインクリメント
		 if (info->mode_num > (info->vpos + 1))
		 {
			 if (info->vfpos.at(info->vpos + 1) < currrent_pos)	info->vpos++;
		 }
		 //Player ON
		 setOpePlayOn(KLOG_PLAY_FILE);

		 result = VARIANT_TRUE;	//操作有効
		 break;

	 case OPE_MODE::PAUSE: //ログ一時停止

		 if (!opePlayOn) break;	//再生中でないならば何もしない

		 //Player OFF
		 setOpePlayOn(KLOG_PAUSE);

		 result = VARIANT_TRUE;	//操作有効

		 break;

	 case OPE_MODE::NEXT: //モード飛び

		 if (info->mode_num > (info->vpos + 1)) //vectorリストを超えない？
		 {
			 info->vpos++;
			 currrent_pos = info->vfpos.at(info->vpos);

			 //カレントタイムの更新
			 info->cTime = info->vtime.at(info->vpos);
			 result = VARIANT_TRUE;	//操作有効

			 setOpePlayOn(KLOG_PLAY_FILE);
			 getData();

			 //1回だけPLAY
			 setOpePlayOn(KLOG_PLAY_ONCE);
		 }
		 else
		 {
			 //超えた場合は次に進めず、現在値キープ

		 }
		 break;

	 case OPE_MODE::PREV: //モード戻り

		 if (1 <= info->vpos) //vectorリスト範囲を逸脱しない？
		 {
			 info->vpos--;
			 currrent_pos = info->vfpos.at(info->vpos);

			 //カレントタイムの更新
			 info->cTime = info->vtime.at(info->vpos);
			 result = VARIANT_TRUE;	//操作有効

			 setOpePlayOn(KLOG_PLAY_FILE);
			 getData();

			 //1回だけPLAY
			 setOpePlayOn(KLOG_PLAY_ONCE);
		 }
		 else
		 {
			 //逸脱する場合は戻さず、現在値キープ

		 }
		 break;

	 case OPE_MODE::SLIDEBAR: //スライドバー

		 //モード出し
		 tm   = FlightLog::getPlayCurrentTm(xml);
		 if (tm<0 || tm > info->totalTime) break;

		 info->vpos = getVecIndex(tm);
		 
         //データ頭出し
		 getDataByTime(tm, VARIANT_TRUE);
		 oneDataSet = getOneData();

		 //カレントタイムの更新
		 info->cTime = tm;

		 //1回だけPLAY
		 setOpePlayOn(KLOG_PLAY_ONCE);
		 result = VARIANT_TRUE;	//操作有効

		 break;

	 default:
		 break;
	 }

	 return result;
 }

 //
 //　再生ファイルの総時間
 //　
std::string  kmlLogger::getTotalTm()
 {
	 char buf[256] = { 0 };
	 sprintf_s(buf, 256, "%.3f", info->totalTime);
	 char *totaltime = _strdup(buf);
	 std::string xmlMsg = std::string(totaltime);
	 delete[] totaltime;
	 return xmlMsg;
}

 
 //
 //　再生ファイルの現再生時刻
 //　
 std::string  kmlLogger::getCurrentTm()
 {
	 char buf[256] = { 0 };
	 sprintf_s(buf, 256, "%.3f", info->cTime);
	 char *tmp = _strdup(buf);

	 std::string xmlMsg1 = "<FlightLog><Play><CurrentTime>";
	 std::string xmlMsg2 = "</CurrentTime></Play></FlightLog>";
	 std::string contents(tmp);


	 std::string xmlMsg = xmlMsg1 + std::string(tmp) + xmlMsg2;
     
	 delete[] tmp;

	 return xmlMsg;
 }


 //
 // 経過秒（ファイルの先頭データからの経過秒）の計算
 //
 double kmlLogger::getTime(struct tm *t, double ms, struct tm *t0, double ms0)
 {

	 time_t tt = mktime(t);
	 time_t tt0 = mktime(t0);

	 double dt = difftime(tt, tt0);
	 double dms = (ms - ms0) / 1000;

	 return dt + dms;
 }
 //
 //　XML文から経過秒を算出
 //
 double kmlLogger::getDltTime(BSTR xml, BSTR path, struct tm *wtime)
 {
	 if (xml == L"") return 0.0;

	 StrXml *xmlTxt = new StrXml(xml);
	 wtime->tm_year = atoi(xmlTxt->getData(_bstr_t(path) + _bstr_t(KLOG_PATH_YEAR))) - 1900;
	 wtime->tm_mon = atoi(xmlTxt->getData(_bstr_t(path) + _bstr_t(KLOG_PATH_MON))) - 1;
	 wtime->tm_mday = atoi(xmlTxt->getData(_bstr_t(path) + _bstr_t(KLOG_PATH_DAY)));
	 wtime->tm_hour = atoi(xmlTxt->getData(_bstr_t(path) + _bstr_t(KLOG_PATH_HR)));
	 wtime->tm_min = atoi(xmlTxt->getData(_bstr_t(path) + _bstr_t(KLOG_PATH_MIN)));
	 wtime->tm_sec = atoi(xmlTxt->getData(_bstr_t(path) + _bstr_t(KLOG_PATH_SEC)));
	 double ms = atof(xmlTxt->getData(_bstr_t(path) + _bstr_t(KLOG_PATH_MSEC)));
	 delete xmlTxt;

	 return getTime(wtime, ms, &info->time0, info->time0_ms);

 }

 //
 // 再生ファイルのカレントタイム
 //
 double kmlLogger::getCtime()
 {
	 return info->cTime;
 }

 //
 // トリミング
 //
 VARIANT_BOOL kmlLogger::editFile(struct flightLog log)
 {

	//現在の操作動作をバックアップ
	ifstream::pos_type currrent_pos_bkup = currrent_pos;
	size_t vpos_bkup = info->vpos;

	//FlightLogメッセージの解析
	StrXml *guiMsg    = new StrXml(log.guiMsg);
	_bstr_t savePath  = guiMsg->getData(L"root/message/FlightLog/Play/Edit/SaveFilePath");
	double startTime  = atof(guiMsg->getData(L"root/message/FlightLog/Play/Edit/StartTime"));
	double endTime    = atof(guiMsg->getData(L"root/message/FlightLog/Play/Edit/EndTime"));
	delete guiMsg;

	//トリミング範囲チェック
	if (startTime <0.0 || startTime > info->totalTime)
	{
		return VARIANT_FALSE; //設定値不良
	}
	if (endTime <0.0 || endTime > info->totalTime)
	{
		return VARIANT_FALSE; //設定値不良
	}

	//再生ファイルのトリミング終了位置seek
	info->vpos = getVecIndex(endTime);
	getDataByTime(endTime, VARIANT_TRUE);
	ifstream::pos_type trim_end = currrent_pos;

	//再生ファイルのトリミング開始位置seek
	info->vpos = getVecIndex(startTime);
	getDataByTime(startTime, VARIANT_FALSE);
	ifs.seekg(currrent_pos);

	//ファイルオープン
	std::ofstream ifedit;

	//ファイル名
	char *savefile = _com_util::ConvertBSTRToString(savePath.GetBSTR());
	VARIANT_BOOL editKmzFile = isKmzFile(savefile);

	size_t len = strlen(savefile);
	char *kml = (char *)malloc(sizeof(char)*(len + 1));
	strncpy_s(kml, len + 1, savefile, _TRUNCATE);
	if (editKmzFile)
	{
		kml[len - 1] = 'l';
	}

	//kmlファイルオープン
	ifedit.open(kml, ios_base::out);
	//ファイル名
	char *filename = (char *)malloc(sizeof(char)*(len + 1));
	strncpy_s(filename, len + 1, kml, len - 4);

    //ヘッダ部
	ifedit<<"<?xml version=\"1.0\" encoding=\"shift_jis\"?>\n";
	ifedit<<"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
	ifedit<<"<Document>\n";
	ifedit << "<name>" << filename << "</name>\n";
	delete[] filename;

    //データ部
	std::string str;
	while (getline(ifs, str))
	{
		//トリミング終了判定
		if (currrent_pos > trim_end) break;

		//ファイルの書き出し
		ifedit.write(str.c_str(), strlen(str.c_str()));
		ifedit.put('\n');

		currrent_pos = ifs.tellg();	//ファイルポインタ位置

		str.clear();
	}

	//フッタ部
	ifedit << "</Document>\n</kml>\n";

	//ファイルクローズ
	ifedit.close();

	//保存ファイルがKMZの場合
	if (editKmzFile == VARIANT_TRUE)
	{
		char *kmz = (char *)malloc(sizeof(char)*(len + 1));
		strncpy_s(kmz, len + 1, savefile, _TRUNCATE);
		kmz[len - 3] = 'k';
		kmz[len - 2] = 'm';
		kmz[len - 1] = 'z';

		kmlToKmz(kmz, kml);
		// 圧縮したので不要なKMLファイルを削除
		remove(kml);
		//メモリ解放
		delete[] kmz;
	}

	//メモリ解放
	delete[] savefile;
	delete[] kml;



	//元に戻す
	currrent_pos = currrent_pos_bkup;
	info->vpos = vpos_bkup;

	return VARIANT_TRUE;

 }

 //
 // ファイルの先頭に移動
 //
 void kmlLogger::moveToTopOfFile()
 {
	 ifs.clear();
	 ifs.seekg(0, ios_base::beg);
	 currrent_pos = ifs.tellg();	//ファイルポインタ位置

	 info->cTime = 0.0;//カレントタイムのリセット
	 eof          = VARIANT_FALSE;

 }

 VARIANT_BOOL kmlLogger::isKmzFile(char *file)
 {
	 size_t  len = strlen(file);
	 char *fileExt = &file[len - 4];
	 if (strstr(fileExt, "kmz"))
	 {
		 return VARIANT_TRUE;
	 }
	 else
	 {
		 return VARIANT_FALSE;
	 }

 }

 VARIANT_BOOL kmlLogger::kmlToKmz(char *kmz, char *kml)
 {
	 //// 起動時のカレントディレクトリを保存
	 //DWORD length = ::GetCurrentDirectory(0, NULL);
	 //TCHAR* curdir = new TCHAR[length];
	 //::GetCurrentDirectory(length, curdir);

	 ////　カレントディレクトリを移動
	 //char  *ret = strrchr(kmz, '/');
	 //if (ret == NULL) return VARIANT_FALSE;
	 //
	 //size_t len = ret - kmz;
	 //TCHAR *tdirname = new TCHAR[len + 1];
	 //size_t temp;
	 //mbstowcs_s(&temp, tdirname, len + 1, kmz, _TRUNCATE);
	 //SetCurrentDirectory(tdirname);

	 //char* kmzfilename, *kmlfilename;
	 //kmzfilename = ret + 1;
	 //ret = strrchr(kml, '/');
	 //kmlfilename = ret + 1;

	 // KMZ化
	 //int result = minizip(kmzfilename, kmlfilename);

	 // カレントディレクトリを起動ディレクトリに移動
	 //SetCurrentDirectory(curdir);

	 //メモリ解放
	 //delete[] curdir;
	 //delete[] tdirname;
	 int result = minizip(kmz, kml);
	 if (!result)
	 {
		 return VARIANT_TRUE;
	 }
	 else
	 {
		 return VARIANT_FALSE;
	 }

 }

 VARIANT_BOOL kmlLogger::kmzToKml(char *kmz)
 {
	 // 起動時のカレントディレクトリを保存
	 DWORD length = ::GetCurrentDirectory(0, NULL);
	 TCHAR* curdir = new TCHAR[length];
	 ::GetCurrentDirectory(length, curdir);

	 // カレントディレクトリを移動
	 char  *ret = strrchr(kmz, '/');
	 if (ret == NULL) return VARIANT_FALSE;

	 size_t len = ret - kmz;
	 TCHAR *tdirname = new TCHAR[len + 1];
	 size_t temp;
	 setlocale(LC_ALL, "Japanese_Japan.932");//<-追加
	 mbstowcs_s(&temp, tdirname, len + 1, kmz, _TRUNCATE);
	 SetCurrentDirectory(tdirname);

	 char* kmzfilename;
	 kmzfilename = ret + 1;

	 // KML化
	 int result = miniunz(kmz);//miniunz(kmzfilename);　//TODO:解凍ファイル名が不明/日本語対応/保存したディレクトリでないとエラー

	 // カレントディレクトリを起動ディレクトリに移動
	 SetCurrentDirectory(curdir);

	 //メモリ解放
	 delete[] curdir;
	 delete[] tdirname;

	 if (!result)
	 {
		 return VARIANT_TRUE;
	 }
	 else
	 {
		 return VARIANT_FALSE;
	 }

 }
