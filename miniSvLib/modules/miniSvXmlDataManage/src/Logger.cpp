#include "stdafx.h"
#include <stdio.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib") 
#include <windows.h>
#include <time.h>

#import  <msxml6.dll>  // MSXML���C�u�����̃C���|�[�g  
#include "Logger.h"
#include "StrXml.h"

enum  LOGGER_ERR{ NOERR, ERR_FILENAME_NULL, ERR_FILEOPEN, etc };
#define ERROR_MSG_SIZE   (256)
#define READ_BUFFER_SIZE (256)

//
// ������
//
//�@�߂�l�F=0;����I��
//          ��0�G�G���[�R�[�h
//
int Logger::RecStart()
{
	if (openmode == _bstr_t(L"w"))
	{
		//�f�B���N�g����������������
		if (PathIsDirectory(dirPath) == false)
		{
			CreateDirectory(dirPath, NULL);
		}

		//���ݓ����̎擾
		char buff[16] = "";
		time_t now = time(NULL);
		fileCreate = now;
		struct tm now_time;          // �|�C���^����A�ϐ����̂ɕύX
		localtime_s(&now_time, &now);
		sprintf_s(buff, "_%04d%02d%02d%02d%02d%02d", now_time.tm_year + 1900,
			now_time.tm_mon + 1, now_time.tm_mday, now_time.tm_hour, now_time.tm_min, now_time.tm_sec);

		fileBaseName = dirPath + "\\" + filename;
		filePath = fileBaseName + _bstr_t(buff);

		return Open(filePath, openmode);

	}
	else
	{
		//���[�h�G���[
	}
	return S_FALSE;
}

//
// ������
//
//�@�߂�l�F=0;����I��
//          ��0�G�G���[�R�[�h
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
// ������
//
//�@�߂�l�F=0;����I��
//          ��0�G�G���[�R�[�h
//
int Logger::Initialize()
{
	if (openmode == _bstr_t(L"w"))
	{
		//�f�B���N�g����������������
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
// �t�@�C���T�C�Y�擾
//
//�@�߂�l�F�t�@�C���T�C�Y
//
fpos_t Logger::getFileSize()
{
	fpos_t fsize = 0;
	fpos_t fsizeb = fseek(fp, 0, SEEK_END);
	fgetpos(fp, &fsize);

	return fsize;
}

//
// �t�@�C��������(�t�@�C���T�C�Y�`�F�b�N�L��)
//
//  �߂�l      : =0           �G����I��
// �@�@         : <0           �G�t�@�C�������݃G���[
//              : >0           �GsetDB����f�[�^�擾�G���[
//
int Logger::Write(dataManage *db)
{

	int    iRet;

	if (getFileSize() > fileSize)
	{

		Close(); //���݊J���Ă���t�@�C���̃N���[�Y

		char    fileNostr[9];
		sprintf_s(fileNostr, "%04d.txt", fileNo++);
		_bstr_t filePathTemp = filePath + _com_util::ConvertStringToBSTR(fileNostr);

		iRet = Open(filePathTemp, openmode);
		if (iRet != 0) return  iRet;
	}

	//setDB����f�[�^���擾���ăt�@�C���ɏ�����
	return Write(db, DataID, clientID);

}

//
// �t�@�C���I�[�v��
//
//�@�߂�l�F=0;�t�@�C���I�[�v������
//          ��0�G�G���[�R�[�h
//
int Logger::Open(BSTR filePath, BSTR mode)
{
	char error_msg[ERROR_MSG_SIZE];
	errno_t errorcode = _wfopen_s(&fp, filePath, mode);

	if (errorcode != 0)
	{
		if (openmode == _bstr_t(L"r") && PathIsDirectory(filePath) == false)
		{
			//�ǂݍ��ރt�@�C�������X�����̂ŃG���[�ł͂Ȃ�.
			//�������Aerrorcode�ɂ�0�ȊO�̒l�����āA
		}
		else
		{
			strerror_s(error_msg, ERROR_MSG_SIZE, errorcode);
			printf("�t�@�C���I�[�v���G���[�i%s�j�BFILE = %s , LINE = %d\n",
				error_msg, __FILE__, __LINE__);
		}
	}

	prePos = 0;
	return errorcode;
}

//
// �t�@�C���N���[�Y
//
//�@�߂�l�F=0;����I��
//          ��0�G�G���[�R�[�h
//
int Logger::Close()
{
	char error_msg[ERROR_MSG_SIZE];
	errno_t errorcode = fclose(fp);
	if (errorcode != 0)
	{
		strerror_s(error_msg, ERROR_MSG_SIZE, errorcode);
		printf("�t�@�C���N���[�Y�G���[�i%s�j�BFILE = %s , LINE = %d\n",
			error_msg, __FILE__, __LINE__);
	}
	return errorcode;
}

//
// �I������
//
//�@�߂�l�F=0;����I��
//          ��0�G�G���[�R�[�h
//
int Logger::RecEnd(time_t now)
{

	Close(); //���݊J���Ă���t�@�C���̃N���[�Y

	char buff[20] = "";
	struct tm now_time;
	localtime_s(&now_time, &now);
	sprintf_s(buff, "-%04d%02d%02d%02d%02d%02d.xml", now_time.tm_year + 1900,
		now_time.tm_mon + 1, now_time.tm_mday, now_time.tm_hour, now_time.tm_min, now_time.tm_sec);

	//�t�@�C���̃��l�[��
	_bstr_t filePathTemp = filePath + _bstr_t(buff);

	rename(filePath, filePathTemp);

	return 0;
}

//
// �I������
//
//�@�߂�l�F=0;����I��
//          ��0�G�G���[�R�[�h
//
int Logger::Finalize()
{
	return Close();
}


//
// �t�@�C��������
//
//�@�߂�l�F=0�G����I��
//          <0�G�t�@�C�������݃G���[
//
int Logger::WriteFile_YYYYMMDDHHMMSS(BSTR bstr)
{

	//���ݎ���
	time_t now = time(NULL);

	double dt = difftime(now, fileCreate);

	//�t�@�C�������`�F�b�N
	int    iRet;

	if (dt > interval)
	{
		RecEnd(now);

		fileCreate = now;  //�t�@�C���쐬�����X�V
		struct tm now_time;
		localtime_s(&now_time, &now);

		//���̃t�@�C���I�[�v��
		char buff[16] = "";
		sprintf_s(buff, "_%04d%02d%02d%02d%02d%02d", now_time.tm_year + 1900,
			now_time.tm_mon + 1, now_time.tm_mday, now_time.tm_hour, now_time.tm_min, now_time.tm_sec);

		filePath = fileBaseName + _bstr_t(buff);

		iRet = Open(filePath, openmode);
		if (iRet != 0) return  iRet;
	}

	//�t�@�C���̏�����
	/*
	StrXml *xmlTxt = new StrXml(bstr);
	_bstr_t xmlBin = xmlTxt->convertBin2Txt();
	delete xmlTxt;

	return Write(xmlBin.copy());
	*/
	return Write(bstr);
}

// �t�@�C��������
//
//�@�߂�l�F=0�G����I��
//          <0�G�t�@�C�������݃G���[
//
int Logger::Write(BSTR bstr)
{

	int iRet = fwprintf_s(fp, L"%s", bstr);
	if (iRet < 0)  //�߂�l�����Ȃ�΁A�G���[
	{
		printf("�t�@�C�������݃G���[ FILE = %s , LINE = %d\n",
			__FILE__, __LINE__);
		return iRet;
	}

	return  0;
}

//
// �t�@�C��������
//
//  �߂�l      : =0           �G����I��
// �@�@         : <0           �G�t�@�C�������݃G���[
//              : >0           �GsetDB����f�[�^�擾�G���[
//
int Logger::Write(dataManage *db, int dataID, int clientID)
{
	BSTR   bstr;
	int    iRet;

	//�f�[�^�Z�b�g
	iRet = db->getMsgOnMainSetDB(dataID, clientID, &bstr);

	if (!iRet) //�f�[�^�擾�����Ȃ�΃t�@�C���֏o��
	{
		iRet = Write(bstr);
		::SysFreeString(bstr);
	}

	return iRet;
}

//
// �t�@�C���Ǎ���
//
//�@�߂�l�F=0 ;�P�f�[�^�Z�b�g�Ǎ��ݏI��
//          =-1; �S�t�@�C���̓Ǎ��ݏI��
//          =-2; NULL
//          =1�G�t�@�C���ǂݍ��݃G���[ 
//          =2�G�G���[�R�[�h
//
int Logger::Read(BSTR* bstr)
{
	wchar_t buffer[READ_BUFFER_SIZE];
	size_t  size = READ_BUFFER_SIZE; //while���ɓ��邽�߂ɁAREAD_BUFFER_SIZE�ŏ�����
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
			Close();    //�J�����g�t�@�C�����N���[�Y

			char fileNostr[9];
			sprintf_s(fileNostr, "%04d.txt", fileNo++); //�t�@�C���ԍ���i�߂�
			_bstr_t filePathTemp = filePath + _com_util::ConvertStringToBSTR(fileNostr);
			int iRet = Open(filePathTemp, openmode);
			if (iRet != 0)
			{
				return  -1; //�I��
			}
			else
			{
				return Read(bstr); //���̃t�@�C����ǂݍ���               
			}
		}
		size = wcslen(buffer) + 1; //�k���������܂߂��T�C�Y
		if (size > READ_BUFFER_SIZE)  size = READ_BUFFER_SIZE;
		if (size > 1)
		{
			oneLine = SysAllocStringLen(buffer, size * sizeof(wchar_t));
			temp += oneLine;
			if (wcsstr((wchar_t *)oneLine, L"</root>") != NULL)
			{
				SysFreeString(oneLine);
				break; //while���𔲂���
			}
			SysFreeString(oneLine);
		}
	}

	if (while_num == INT_MAX)
	{
		printf("�Ǎ��ރf�[�^�s���z��𒴂��܂��� FILE = %s , LINE = %d\n",
			__FILE__, __LINE__);
		return  1;
	}
	else if (!temp.length())
	{
		printf("�t�@�C���Ǎ��݃G���[ FILE = %s , LINE = %d\n",
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
// �f�[�^�iXML�j�̎擾
//
_bstr_t Logger::getXml()
{
	return  xml;
}


// ���̃f�[�^�̓Ǎ���
//
//�@�߂�l�F=0 ;�P�f�[�^�Z�b�g�Ǎ��ݏI��
//          =-1; EOF
//          =-2; NULL
//          =1�G�t�@�C���ǂݍ��݃G���[ 
//          =2�G�G���[�R�[�h
//
int Logger::goNext()
{
	wchar_t buffer[READ_BUFFER_SIZE];
	size_t  size = READ_BUFFER_SIZE; //while���ɓ��邽�߂ɁAREAD_BUFFER_SIZE�ŏ�����
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
			Close();				//�J�����g�t�@�C�����N���[�Y
			return  -1;				//�I��
		}
		size = wcslen(buffer) + 1; //�k���������܂߂��T�C�Y
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
				break; //while���𔲂���
			}
			SysFreeString(preStr);
			SysFreeString(oneLine);
		}
	}

	if (while_num == INT_MAX)
	{
		xml = _bstr_t(L"");
		printf("�Ǎ��ރf�[�^�s���z��𒴂��܂��� FILE = %s , LINE = %d\n",
			__FILE__, __LINE__);
		return  1;
	}
	else if (!temp.length())
	{
		xml = _bstr_t(L"");
		printf("�t�@�C���Ǎ��݃G���[ FILE = %s , LINE = %d\n",
			__FILE__, __LINE__);
		return  -2;
	}

	xml = _bstr_t(result);
	return  0;
}

//
// �t�@�C���̍폜
//
void Logger::del()
{
	remove(filePath);

}

// �O�ɖ߂�
//
//�@�߂�l�F=0 ;�P�f�[�^�Z�b�g�Ǎ��ݏI��
//          =1�G�t�@�C���t�H�[�}�b�g�G���[�i<?xml>�^�O���Ȃ��j
//          =2�G�t�@�C���̐擪�ɓ��B
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
		if (strstr(buffer, "<?xml") != NULL) //�����񌟍�
		{
			fseek(fp, current, SEEK_SET); 
			goNext();
			break;
		}
		if (current <= 0)
		{
			printf("�t�@�C���̐擪�ɒB���܂����BFILE = %s , LINE = %d\n",
				__FILE__, __LINE__);
			return  2;
		}
	}

	if (while_num == INT_MAX)
	{
		printf("<?xml��������܂��� FILE = %s , LINE = %d\n",
			__FILE__, __LINE__);
		return  1;
	}
	return  0;
}

