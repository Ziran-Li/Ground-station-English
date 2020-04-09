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
#define KLOG_PATH_CMODE  (L"/UAV/Sys/Mode/Ctrl")	// ���䃂�[�h
#define KLOG_PATH_MODE  (L"/UAV/Sys/Mode/Auto")		// �������[�h
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

	size_t data_num;    //�f�[�^����
	size_t mode_num;    //���[�h����
	   
    size_t vpos;        //���[�h���X�g�C���f�b�N�X
	double cTime;       //���ݎ��� (sec)
	double totalTime;   //������ (sec)
	struct tm  time0;	//�J�n����
	double time0_ms;	//�J�n�����@�~���Z�b�N

    //���[�h���X�g
	deque<ifstream::pos_type> vfpos;//�t�@�C���ʒu
	deque<double>vtime;			//�o�ߕb
	deque<_bstr_t>vFlightMode;	//��s���[�h
	deque<_bstr_t>vControlMode;	//���䃂�[�h

	//�f�[�^���X�g
	deque<ifstream::pos_type> all_fpos;//�t�@�C���ʒu
	deque<double> all_time;		//�o�ߕb


	fileInfo(){ vpos = 0; data_num = 0; mode_num = 0; totalTime = 0.0; cTime = 0.0; };
};

class kmlLogger
{
private:

	FILE     *fp;
	_bstr_t  openmode;			//�t�@�C���I�[�v�����[�h
	_bstr_t  dirName;			//�f�B���N�g����
	_bstr_t  fileName;			//�t�@�C����
	_bstr_t  fileBaseName;
	_bstr_t  filePath;
	_bstr_t  kmlFilePath;		//�t�@�C����
	VARIANT_BOOL fileIsKmz;		//��TRUE�Gkmz�A=FALSE;kml


	_bstr_t  coordinate;		//lon,lat,alt
	_bstr_t  extendedData;		//extenededDat
	_bstr_t  data_day;          //Year,Month,Day
	_bstr_t  data_time;         //Hour,Minute,Second

	_bstr_t oneDataSet;			//1�f�[�^�Z�b�g
	unsigned int  divideSizeInByte; //�t�@�C���T�C�Y(Byte)
    
	VARIANT_BOOL playOn;		//=TRUE�GON ,=FALSE;OFF
	VARIANT_BOOL opePlayOn;		//=TRUE�GON ,=FALSE;OFF
	VARIANT_BOOL eof;           //

	HRESULT loadStrXml(BSTR bstrXML);

	/* �ۑ��֌W*/
	_bstr_t getFilename();
	_bstr_t getLLA();
	void    getExtendedData(BSTR *xmlout);
	_bstr_t getDate();
	_bstr_t getTime();
	fpos_t  getFileSize();

    
	/* �Đ��֌W*/
	std::ifstream ifs;
	ifstream::pos_type currrent_pos; //���݂̃t�@�C���X�g���[���ʒu

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
    	
	void    moveToTopOfFile(); //�t�@�C���̐擪�Ɉړ�

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
