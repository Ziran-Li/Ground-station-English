#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
#import  <msxml6.dll>  // MSXMLライブラリのインポート  
#include "DataManage.h"
#include "XmlMsgAttribute.h"

#define LOG_FILE_NAME    (L"log")

//
//　Loggerクラス
//
class Logger
{
private:
	FILE     *fp;
	long   prePos;
	_bstr_t  dirPath;			//ディレクトリパス
	_bstr_t  filename;			//ファイル名
	fpos_t   fileSize;		    //ファイルサイズ
	long long   interval;		    //時間
	unsigned int fileNo;		//ファイル番号
	_bstr_t  openmode;			//ファイルオープンモード
	_bstr_t  filePath;
	_bstr_t  fileBaseName;

	time_t  fileCreate;

	unsigned int clientID;		//クライアントID
	unsigned int DataID;		//データID

	int Write(dataManage *db, int dataID, int clientID);

	_bstr_t  xml;

public:
	Logger()
	{
		fp = NULL;
		fileNo = 0;
	};
	Logger(BSTR dir_Path, BSTR file_name, long long dtime)
	{
		fp = NULL;
		fileNo = 0;

		openmode = L"w";

		dirPath = dir_Path;
		filename = file_name;
		fileSize = LLONG_MAX;

		interval = dtime;

	};
	Logger(BSTR dir_Path, fpos_t fileSizeMax, unsigned int client_ID, unsigned int Data_ID)
	{
		fp = NULL;
		fileNo = 0;

		openmode = L"w";

		dirPath = dir_Path;
		filename = _bstr_t(LOG_FILE_NAME);
		fileSize = fileSizeMax;

		clientID = client_ID;
		DataID = Data_ID;
	};
	Logger(BSTR dir_Path)
	{
		fp = NULL;
		fileNo = 0;

		openmode = L"r";

		dirPath = dir_Path;
		filename = _bstr_t(LOG_FILE_NAME);
		fileSize = 0;

		clientID = 0;
		DataID = 0;
	};
	Logger(BSTR dir_Path, BSTR file_name)
	{
		fp = NULL;
		fileNo = 0;

		openmode = L"r";

		dirPath = dir_Path;
		filename = file_name;
		fileSize = 0;

		clientID = 0;
		DataID = 0;
	};

	int RecStart();
	int PlayStart();
	int Initialize();
	int Finalize();
	_bstr_t getXml();
	int goNext();
	int goBack();

	int Open(BSTR filePath, BSTR mode);
	int Close();
	int Write(BSTR bstr);
	int Write(dataManage *db);
	int Read(BSTR* bstr);

	int WriteFile_YYYYMMDDHHMMSS(BSTR bstr);
	int RecEnd(time_t now);
	fpos_t getFileSize();

	void del();
};

#endif /**/
