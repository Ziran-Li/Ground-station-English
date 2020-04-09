#include "stdafx.h"
#include <stdio.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib") 
#include <windows.h>
#include <time.h>

#import  <msxml6.dll>  // MSXMLライブラリのインポート  
#include "Logger.h"
#include "StrXml.h"

enum  LOGGER_ERR{ NOERR, ERR_FILENAME_NULL, ERR_FILEOPEN, etc };
#define ERROR_MSG_SIZE   (256)
#define READ_BUFFER_SIZE (256)

//
// 初期化
//
//　戻り値：=0;正常終了
//          ≠0；エラーコード
//
int Logger::RecStart()
{
	if (openmode == _bstr_t(L"w"))
	{
		//ディレクトリが無かったら作る
		if (PathIsDirectory(dirPath) == false)
		{
			CreateDirectory(dirPath, NULL);
		}

		//現在日時の取得
		char buff[16] = "";
		time_t now = time(NULL);
		fileCreate = now;
		struct tm now_time;          // ポインタから、変数実体に変更
		localtime_s(&now_time, &now);
		sprintf_s(buff, "_%04d%02d%02d%02d%02d%02d", now_time.tm_year + 1900,
			now_time.tm_mon + 1, now_time.tm_mday, now_time.tm_hour, now_time.tm_min, now_time.tm_sec);

		fileBaseName = dirPath + "\\" + filename;
		filePath = fileBaseName + _bstr_t(buff);

		return Open(filePath, openmode);

	}
	else
	{
		//モードエラー
	}
	return S_FALSE;
}

//
// 初期化
//
//　戻り値：=0;正常終了
//          ≠0；エラーコード
//
int Logger::PlayStart()
{
	int ret;
	if (openmode == _bstr_t(L"r"))
	{
		filePath = dirPath + "\\" + filename;
		ret = Open(filePath, openmode);
		if (ret == 0)
		{
			ret = goNext();
		}
	}
	return ret;
}

//
// 初期化
//
//　戻り値：=0;正常終了
//          ≠0；エラーコード
//
int Logger::Initialize()
{
	if (openmode == _bstr_t(L"w"))
	{
		//ディレクトリが無かったら作る
		if (PathIsDirectory(dirPath) == false)
		{
			CreateDirectory(dirPath, NULL);
		}
	}

	filePath = dirPath + "\\" + filename;

	char    fileNostr[9];
	sprintf_s(fileNostr, "%04d.txt", fileNo++);

	_bstr_t filePathTemp = filePath + _com_util::ConvertStringToBSTR(fileNostr);
	return Open(filePathTemp, openmode);
}

//
// ファイルサイズ取得
//
//　戻り値：ファイルサイズ
//
fpos_t Logger::getFileSize()
{
	fpos_t fsize = 0;
	fpos_t fsizeb = fseek(fp, 0, SEEK_END);
	fgetpos(fp, &fsize);

	return fsize;
}

//
// ファイル書込み(ファイルサイズチェック有り)
//
//  戻り値      : =0           ；正常終了
// 　　         : <0           ；ファイル書込みエラー
//              : >0           ；setDBからデータ取得エラー
//
int Logger::Write(dataManage *db)
{

	int    iRet;

	if (getFileSize() > fileSize)
	{

		Close(); //現在開いているファイルのクローズ

		char    fileNostr[9];
		sprintf_s(fileNostr, "%04d.txt", fileNo++);
		_bstr_t filePathTemp = filePath + _com_util::ConvertStringToBSTR(fileNostr);

		iRet = Open(filePathTemp, openmode);
		if (iRet != 0) return  iRet;
	}

	//setDBからデータを取得してファイルに書込み
	return Write(db, DataID, clientID);

}

//
// ファイルオープン
//
//　戻り値：=0;ファイルオープン成功
//          ≠0；エラーコード
//
int Logger::Open(BSTR filePath, BSTR mode)
{
	char error_msg[ERROR_MSG_SIZE];
	errno_t errorcode = _wfopen_s(&fp, filePath, mode);

	if (errorcode != 0)
	{
		if (openmode == _bstr_t(L"r") && PathIsDirectory(filePath) == false)
		{
			//読み込むファイルが元々無いのでエラーではない.
			//ただし、errorcodeには0以外の値を入れて、
		}
		else
		{
			strerror_s(error_msg, ERROR_MSG_SIZE, errorcode);
			printf("ファイルオープンエラー（%s）。FILE = %s , LINE = %d\n",
				error_msg, __FILE__, __LINE__);
		}
	}

	prePos = 0;
	return errorcode;
}

//
// ファイルクローズ
//
//　戻り値：=0;正常終了
//          ≠0；エラーコード
//
int Logger::Close()
{
	char error_msg[ERROR_MSG_SIZE];
	errno_t errorcode = fclose(fp);
	if (errorcode != 0)
	{
		strerror_s(error_msg, ERROR_MSG_SIZE, errorcode);
		printf("ファイルクローズエラー（%s）。FILE = %s , LINE = %d\n",
			error_msg, __FILE__, __LINE__);
	}
	return errorcode;
}

//
// 終了処理
//
//　戻り値：=0;正常終了
//          ≠0；エラーコード
//
int Logger::RecEnd(time_t now)
{

	Close(); //現在開いているファイルのクローズ

	char buff[20] = "";
	struct tm now_time;
	localtime_s(&now_time, &now);
	sprintf_s(buff, "-%04d%02d%02d%02d%02d%02d.xml", now_time.tm_year + 1900,
		now_time.tm_mon + 1, now_time.tm_mday, now_time.tm_hour, now_time.tm_min, now_time.tm_sec);

	//ファイルのリネーム
	_bstr_t filePathTemp = filePath + _bstr_t(buff);

	rename(filePath, filePathTemp);

	return 0;
}

//
// 終了処理
//
//　戻り値：=0;正常終了
//          ≠0；エラーコード
//
int Logger::Finalize()
{
	return Close();
}


//
// ファイル書込み
//
//　戻り値：=0；正常終了
//          <0；ファイル書込みエラー
//
int Logger::WriteFile_YYYYMMDDHHMMSS(BSTR bstr)
{

	//現在時刻
	time_t now = time(NULL);

	double dt = difftime(now, fileCreate);

	//ファイル分割チェック
	int    iRet;

	if (dt > interval)
	{
		RecEnd(now);

		fileCreate = now;  //ファイル作成日時更新
		struct tm now_time;
		localtime_s(&now_time, &now);

		//次のファイルオープン
		char buff[16] = "";
		sprintf_s(buff, "_%04d%02d%02d%02d%02d%02d", now_time.tm_year + 1900,
			now_time.tm_mon + 1, now_time.tm_mday, now_time.tm_hour, now_time.tm_min, now_time.tm_sec);

		filePath = fileBaseName + _bstr_t(buff);

		iRet = Open(filePath, openmode);
		if (iRet != 0) return  iRet;
	}

	//ファイルの書込み
	/*
	StrXml *xmlTxt = new StrXml(bstr);
	_bstr_t xmlBin = xmlTxt->convertBin2Txt();
	delete xmlTxt;

	return Write(xmlBin.copy());
	*/
	return Write(bstr);
}

// ファイル書込み
//
//　戻り値：=0；正常終了
//          <0；ファイル書込みエラー
//
int Logger::Write(BSTR bstr)
{

	int iRet = fwprintf_s(fp, L"%s", bstr);
	if (iRet < 0)  //戻り値が負ならば、エラー
	{
		printf("ファイル書込みエラー FILE = %s , LINE = %d\n",
			__FILE__, __LINE__);
		return iRet;
	}

	return  0;
}

//
// ファイル書込み
//
//  戻り値      : =0           ；正常終了
// 　　         : <0           ；ファイル書込みエラー
//              : >0           ；setDBからデータ取得エラー
//
int Logger::Write(dataManage *db, int dataID, int clientID)
{
	BSTR   bstr;
	int    iRet;

	//データセット
	iRet = db->getMsgOnMainSetDB(dataID, clientID, &bstr);

	if (!iRet) //データ取得成功ならばファイルへ出力
	{
		iRet = Write(bstr);
		::SysFreeString(bstr);
	}

	return iRet;
}

//
// ファイル読込み
//
//　戻り値：=0 ;１データセット読込み終了
//          =-1; 全ファイルの読込み終了
//          =-2; NULL
//          =1；ファイル読み込みエラー 
//          =2；エラーコード
//
int Logger::Read(BSTR* bstr)
{
	wchar_t buffer[READ_BUFFER_SIZE];
	size_t  size = READ_BUFFER_SIZE; //while文に入るために、READ_BUFFER_SIZEで初期化
	bstr_t  temp;
	BSTR  oneLine;
	int     while_num = 0;

	prePos = ftell(fp);

	while (while_num++ < INT_MAX)
	{
		wmemset(buffer, 0, READ_BUFFER_SIZE);
		fgetws(buffer, READ_BUFFER_SIZE, fp);
		if (feof(fp) != 0)
		{
			Close();    //カレントファイルをクローズ

			char fileNostr[9];
			sprintf_s(fileNostr, "%04d.txt", fileNo++); //ファイル番号を進める
			_bstr_t filePathTemp = filePath + _com_util::ConvertStringToBSTR(fileNostr);
			int iRet = Open(filePathTemp, openmode);
			if (iRet != 0)
			{
				return  -1; //終了
			}
			else
			{
				return Read(bstr); //次のファイルを読み込む               
			}
		}
		size = wcslen(buffer) + 1; //ヌル文字を含めたサイズ
		if (size > READ_BUFFER_SIZE)  size = READ_BUFFER_SIZE;
		if (size > 1)
		{
			oneLine = SysAllocStringLen(buffer, size * sizeof(wchar_t));
			temp += oneLine;
			if (wcsstr((wchar_t *)oneLine, L"</root>") != NULL)
			{
				SysFreeString(oneLine);
				break; //while文を抜ける
			}
			SysFreeString(oneLine);
		}
	}

	if (while_num == INT_MAX)
	{
		printf("読込むデータ行が想定を超えました FILE = %s , LINE = %d\n",
			__FILE__, __LINE__);
		return  1;
	}
	else if (!temp.length())
	{
		printf("ファイル読込みエラー FILE = %s , LINE = %d\n",
			__FILE__, __LINE__);
		return  -2;
	}
	else
	{
		*bstr = temp.copy();
	}
	return  0;
}


//
// データ（XML）の取得
//
_bstr_t Logger::getXml()
{
	return  xml;
}


// 次のデータの読込み
//
//　戻り値：=0 ;１データセット読込み終了
//          =-1; EOF
//          =-2; NULL
//          =1；ファイル読み込みエラー 
//          =2；エラーコード
//
int Logger::goNext()
{
	wchar_t buffer[READ_BUFFER_SIZE];
	size_t  size = READ_BUFFER_SIZE; //while文に入るために、READ_BUFFER_SIZEで初期化
	bstr_t  temp(L"");
	BSTR    oneLine;
	int     while_num = 0;
	BSTR    result =L"";

	prePos = ftell(fp);

	while (while_num++ < INT_MAX)
	{
		wmemset(buffer, 0, READ_BUFFER_SIZE);
		fgetws(buffer, READ_BUFFER_SIZE, fp);
		if (feof(fp) != 0)
		{
			xml = _bstr_t(L"");
			Close();				//カレントファイルをクローズ
			return  -1;				//終了
		}
		size = wcslen(buffer) + 1; //ヌル文字を含めたサイズ
		if (size > READ_BUFFER_SIZE)  size = READ_BUFFER_SIZE;
		if (size > 1)
		{
			oneLine = SysAllocStringLen(buffer, size * sizeof(wchar_t));

			BSTR preStr = SysAllocString(temp);
			temp = _bstr_t(preStr) + _bstr_t(oneLine);
			if (wcsstr((wchar_t *)temp, L"</root>") != NULL)
			{	
				result = wcsstr(temp.GetBSTR(), L"<?xml version");
				SysFreeString(preStr);
				SysFreeString(oneLine);
				break; //while文を抜ける
			}
			SysFreeString(preStr);
			SysFreeString(oneLine);
		}
	}

	if (while_num == INT_MAX)
	{
		xml = _bstr_t(L"");
		printf("読込むデータ行が想定を超えました FILE = %s , LINE = %d\n",
			__FILE__, __LINE__);
		return  1;
	}
	else if (!temp.length())
	{
		xml = _bstr_t(L"");
		printf("ファイル読込みエラー FILE = %s , LINE = %d\n",
			__FILE__, __LINE__);
		return  -2;
	}

	xml = _bstr_t(result);
	return  0;
}

//
// ファイルの削除
//
void Logger::del()
{
	remove(filePath);

}

// 前に戻る
//
//　戻り値：=0 ;１データセット読込み終了
//          =1；ファイルフォーマットエラー（<?xml>タグがない）
//          =2；ファイルの先頭に到達
//
int Logger::goBack()
{
	const int read_size = 6;
	char buffer[read_size];
	int  while_num = 0;

	long current = prePos;
	while (while_num++ < INT_MAX)
	{
		current = current - 1L;
		if (current < 0) current = 0;
		fseek(fp, current, SEEK_SET);
		fread(buffer, sizeof(char), read_size - 1, fp);
		if (strstr(buffer, "<?xml") != NULL) //文字列検索
		{
			fseek(fp, current, SEEK_SET); 
			goNext();
			break;
		}
		if (current <= 0)
		{
			printf("ファイルの先頭に達しました。FILE = %s , LINE = %d\n",
				__FILE__, __LINE__);
			return  2;
		}
	}

	if (while_num == INT_MAX)
	{
		printf("<?xmlが見つかりません FILE = %s , LINE = %d\n",
			__FILE__, __LINE__);
		return  1;
	}
	return  0;
}

