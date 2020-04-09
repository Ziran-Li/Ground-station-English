#ifndef KMLLOGGER_H
#define KMLLOGGER_H
#include <fstream>
#include <iostream>
#include <time.h>
#include <tchar.h>
#include <string>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib") 

#include <deque>
#include <list>
#include <map>
#include "XmlMessage.h"
#include "DataManage.h"
#include "Base64Data.h"

#include "FlightLog.h"

#define KLOG_PATH_EXTENDED (L"root/message/Data/Placemark/ExtendedData/Data")
#define KLOG_PATH_DATA     (L"root/message/Data")
#define KLOG_PATH_YEAR (L"/GCS/SystemTime/wYear")
#define KLOG_PATH_MON  (L"/GCS/SystemTime/wMonth")
#define KLOG_PATH_DAY  (L"/GCS/SystemTime/wDay")
#define KLOG_PATH_HR   (L"/GCS/SystemTime/wHour")
#define KLOG_PATH_MIN  (L"/GCS/SystemTime/wMinute")
#define KLOG_PATH_SEC  (L"/GCS/SystemTime/wSecond")
#define KLOG_PATH_MSEC (L"/GCS/SystemTime/wMilliseconds")
#define KLOG_PATH_CMODE  (L"/UAV/Sys/Mode/Ctrl")	// 制御モード
#define KLOG_PATH_MODE  (L"/UAV/Sys/Mode/Auto")		// 自律モード
#define KLOG_PATH_LON  (L"/UAV/Nav/Lon")
#define KLOG_PATH_LAT  (L"/UAV/Nav/Lat")
#define KLOG_PATH_ALT  (L"/UAV/Nav/Alt")

#define KLOG_PLAY_FILE (1)
#define KLOG_PAUSE     (0)
#define KLOG_PLAY_ONCE (2)

#define KLOG_FILE_SIZE (2560000)


class fileInfo
{
   public:

	size_t data_num;    //データ総数
	size_t mode_num;    //モード総数
	   
    size_t vpos;        //モードリストインデックス
	double cTime;       //現在時刻 (sec)
	double totalTime;   //総時間 (sec)
	struct tm  time0;	//開始時刻
	double time0_ms;	//開始時刻　ミリセック

    //モードリスト
	deque<ifstream::pos_type> vfpos;//ファイル位置
	deque<double>vtime;			//経過秒
	deque<_bstr_t>vFlightMode;	//飛行モード
	deque<_bstr_t>vControlMode;	//制御モード

	//データリスト
	deque<ifstream::pos_type> all_fpos;//ファイル位置
	deque<double> all_time;		//経過秒


	fileInfo(){ vpos = 0; data_num = 0; mode_num = 0; totalTime = 0.0; cTime = 0.0; };
};

class kmlLogger
{
private:

	FILE     *fp;
	_bstr_t  openmode;			//ファイルオープンモード
	_bstr_t  dirName;			//ディレクトリ名
	_bstr_t  fileName;			//ファイル名
	_bstr_t  fileBaseName;
	_bstr_t  filePath;
	_bstr_t  kmlFilePath;		//ファイル名
	VARIANT_BOOL fileIsKmz;		//＝TRUE；kmz、=FALSE;kml


	_bstr_t  coordinate;		//lon,lat,alt
	_bstr_t  extendedData;		//extenededDat
	_bstr_t  data_day;          //Year,Month,Day
	_bstr_t  data_time;         //Hour,Minute,Second

	_bstr_t oneDataSet;			//1データセット
	unsigned int  divideSizeInByte; //ファイルサイズ(Byte)
    
	VARIANT_BOOL playOn;		//=TRUE；ON ,=FALSE;OFF
	VARIANT_BOOL opePlayOn;		//=TRUE；ON ,=FALSE;OFF
	VARIANT_BOOL eof;           //

	HRESULT loadStrXml(BSTR bstrXML);

	/* 保存関係*/
	_bstr_t getFilename();
	_bstr_t getLLA();
	void    getExtendedData(BSTR *xmlout);
	_bstr_t getDate();
	_bstr_t getTime();
	fpos_t  getFileSize();

    
	/* 再生関係*/
	std::ifstream ifs;
	ifstream::pos_type currrent_pos; //現在のファイルストリーム位置

	int     getVecIndex(double ctm);
	void    setFilepath(BSTR path);
	double	getTime(struct tm *t, double ms, struct tm *t0, double ms0);
	VARIANT_BOOL playOperation(int opemode, BSTR xml);

	int     InitRead();
	double  getDltTime(BSTR xml, BSTR path, struct tm *wtime);
	void    getDataByTime(double ctm, VARIANT_BOOL lessThan);

    _bstr_t getOneData();
	_bstr_t getOneBinData();
	void    setNextData();
    	
	void    moveToTopOfFile(); //ファイルの先頭に移動

	VARIANT_BOOL editFile(struct flightLog log);
	VARIANT_BOOL kmlToKmz(char *kmz, char *kml);
	VARIANT_BOOL kmzToKml(char *kmz);

	class fileInfo *info;

public:
	kmlLogger(void);
	kmlLogger(BSTR mode);
	~kmlLogger(void);

	int  Initialize();
	int  Write(BSTR bstr);
	int  Open(BSTR filePath, BSTR mode);
	int  Close();

	void setFilename(BSTR fname);   //Rec
	void setDivedeSize(BSTR size);  //Rec
	void OneData(BSTR bstrSetXML);
	void WriteOneData(BSTR bstrSetXML);    //Rec
    _bstr_t getData(MSXML2::IXMLDOMDocument2Ptr psDoc, _bstr_t path );
	_bstr_t getData();

	std::string  getTotalTm();
	std::string  getCurrentTm();
	VARIANT_BOOL setPlayOpeMode(BSTR xml, struct flightLog *logo);

	void   setOpePlayOn(int sw);
	int    isPlaying();
	double getCtime();

	VARIANT_BOOL isEof();
	VARIANT_BOOL isKmzFile(char *file);
};

#endif
