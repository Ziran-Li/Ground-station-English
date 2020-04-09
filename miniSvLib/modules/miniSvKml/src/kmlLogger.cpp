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
// ������
//
//�@�߂�l�F=0;����I��
//          ��0�G�G���[�R�[�h
//
int kmlLogger::Initialize()
{

	if(openmode == _bstr_t(L"w"))
	{
		//���ݓ����̎擾
		time_t now = time(NULL);
		char buff[128]="";
		setlocale(LC_ALL, "Japanese_Japan.932");	//<-�ǉ�
		struct tm now_time;                 // �|�C���^����A�ϐ����̂ɕύX
		localtime_s(&now_time, &now);
		sprintf_s(buff,"_%04d%02d%02d_%02d%02d_%02d",now_time.tm_year+1900,
			now_time.tm_mon + 1,now_time.tm_mday,now_time.tm_hour,now_time.tm_min,now_time.tm_sec);

		dirName= _bstr_t("./") ;
		fileName = fileBaseName + _bstr_t(buff);

		kmlFilePath = fileName + _bstr_t(".kml");
		//�t�@�C��OPEN
		return Open(kmlFilePath, openmode);
	}
	else //read���[�h
	{
		return Open(filePath, openmode);
	}
}

//
// Xml������̓Ǎ���
//
HRESULT  kmlLogger::loadStrXml(BSTR bstrXML)
{
	MSXML2::IXMLDOMDocument2Ptr pDoc;	//DOM�h�L�������g�|�C���^

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
        printf("ExtendedData�f�[�^���Ȃ�");
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
// �t�@�C��������
//
//�@�߂�l�F=0�G����I��
//          <0�G�t�@�C�������݃G���[
//
int kmlLogger::Write(BSTR bstr)
{

	int iRet =fwprintf_s(fp, L"%s", bstr);
	if(iRet < 0 )  //�߂�l�����Ȃ�΁A�G���[
	{
		printf("�t�@�C�������݃G���[ FILE = %s , LINE = %d\n",
				 __FILE__,__LINE__);
		return iRet;
	}
	
	return  0;
}

//
// �t�@�C���I�[�v��
//
//�@�߂�l�F=0;�t�@�C���I�[�v������
//          ��0�G�G���[�R�[�h
//
int kmlLogger::Open(BSTR filePath, BSTR mode)
{
	char error_msg[ERROR_MSG_SIZE];
	setlocale(LC_ALL, "Japanese_Japan.932");	//<-�ǉ�
	errno_t errorcode = _wfopen_s(&fp, filePath, mode);

	if(errorcode != 0 )
	{
        if(openmode == _bstr_t(L"r") && PathIsDirectory(filePath)==false)
		{
            //�ǂݍ��ރt�@�C�������X�����̂ŃG���[�ł͂Ȃ�.
            //�������Aerrorcode�ɂ�0�ȊO�̒l�������Ă���B
		}
		else
		{
    		strerror_s(error_msg , ERROR_MSG_SIZE, errorcode);
	    	printf("�t�@�C���I�[�v���G���[�i%s�j�BFILE = %s , LINE = %d\n",
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
// �t�@�C���N���[�Y
//
//�@�߂�l�F=0;����I��
//          ��0�G�G���[�R�[�h
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

		// KML����KMZ�֕ϊ�
		char *kmlfile = _com_util::ConvertBSTRToString(kmlFilePath.GetBSTR());
		char *filename_try = _com_util::ConvertBSTRToString(kmlFilePath.GetBSTR());
		size_t  len = strlen(filename_try);
		filename_try[len - 1] = 'z';

		// TODO:KMZ�𓀎��̃t�@�C������Ǖs�̉ۑ肪��������܂ŕۗ�
		//// ���k
		//if (kmlToKmz(filename_try, kmlfile))
		//{
		//	// ���k�����̂ŕs�v��KML�t�@�C�����폜
		//	remove(kmlfile);

		//	ret = VARIANT_TRUE;
		//}
		//else
		//{
		//	//���k���s
		//	ret = VARIANT_FALSE;
		//}
		ret = VARIANT_TRUE;

		delete[] kmlfile;
		delete[] filename_try;
		
	}

	return ret;
}

//
// �t�@�C���T�C�Y�擾
//
//�@�߂�l�F�t�@�C���T�C�Y
//
fpos_t kmlLogger::getFileSize()
{
	fpos_t fsize = 0;
	fpos_t fsizeb = fseek(fp, 0, SEEK_END);
	fgetpos(fp, &fsize);

	return fsize;
}
//
// �P�Z�b�g�f�[�^�쐬
//
void kmlLogger::WriteOneData(BSTR bstrSetXML)
{

	if (getFileSize() > divideSizeInByte)
	{

		Close(); //���݊J���Ă���t�@�C���̃N���[�Y

		Initialize();  //�t�@�C���̏������i�I�[�v�����w�b�_���o�́j
	}

	//�P�f�[�^�Z�b�g���t�@�C���ɏ�����
	OneData(bstrSetXML);

}
//
// �P�Z�b�g�f�[�^�쐬
//
void kmlLogger::OneData(BSTR bstrSetXML)
{	
	loadStrXml(bstrSetXML);

	Write(L"<Placemark>") ;

	Write(L"<TimeStamp><when>");	////TODO �^�C���X�^���v���ǂ����邩�H				 
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
//�@�t�@�C�����
//
//
int kmlLogger::InitRead()
{
	VARIANT_BOOL init=VARIANT_TRUE;
	std::string str;
	struct tm wtime = { 0 };
	double time_ms  =0.0;

	setlocale(LC_ALL, "Japanese_Japan.932");	//<-�ǉ�
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
			//�t�@�C���|�C���^�ʒu
			currrent_pos = ifs.tellg();	////�t�@�C���|�C���^�ʒu

			//����H
			if(init)
			{
				init = VARIANT_FALSE;

				//����Ȃ�Ώ��������̕ۑ�
				info->time0    = wtime;
				info->time0_ms = time_ms;

				//FlightMode��ControlMode�ۑ�
				cFlightMode  = Mode;
				cControlMode = ContorlMode;

				//���[�h�ؑւ�
				info->vfpos.push_back(pre_pos);
				info->vtime.push_back(0.0);
				info->vFlightMode.push_back(Mode);
				info->vControlMode.push_back(ContorlMode);

				//�S�f�[�^
				info->all_fpos.push_back(pre_pos);
				info->all_time.push_back(0.0);
			}
			else
			{
				double t = getTime(&wtime, time_ms, &info->time0, info->time0_ms);

				//FlightMode��ControlMode���O��l����ύX����H
				if (cFlightMode != Mode || cControlMode != ContorlMode)
				{
					//�ύX����Ȃ��vector�ɕۑ�
					info->vfpos.push_back(pre_pos);
					info->vtime.push_back(t);
					info->vFlightMode.push_back(Mode);
					info->vControlMode.push_back(ContorlMode);
				}

				//���ݒl�̕ۑ�
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

    //�t�@�C���I�[
	info->totalTime = getTime(&wtime, time_ms, &info->time0, info->time0_ms);

    //�f�[�^��
	info->data_num = info->all_fpos.size();
	info->mode_num = info->vfpos.size();

	//�t�@�C���̐擪�ɖ߂�
	moveToTopOfFile();

	return VARIANT_TRUE;
 }

 //
 // 1�f�[�^�Z�b�g�ixml�j�̎擾
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
			 //ExtendedDatat�^�O�̓��e�𒊏o
			 size_t s_size = sizeof("<ExtendedData>") -1;
			 ep = strstr(sp, "</ExtendedData>");
			 size_t len = ep - sp - s_size;
			 char *tmp = (char *)malloc(sizeof(char)*(len+1));
			
			 strncpy_s(tmp, len+1, sp + s_size, len);
			 std::string content = tmp;
			 std::string header = "<?xml version='1.0' encoding='UTF-8'?><root><message type='set'>";
			 std::string footer = "</message></root>";

			 //�J�����g�^�C���̍X�V
			 std::string xmlStringRaw = header + content + footer;
			 struct tm wtime;
			 BSTR  extend = _com_util::ConvertStringToBSTR(xmlStringRaw.c_str());
			 info->cTime = getDltTime(extend,KLOG_PATH_DATA, &wtime);
			 SysFreeString(extend);

			 //�J�����g�^�C���t���Ńf�[�^�Z�b�g�ݒ�
			 std::string xmlString = header+getCurrentTm()+content + footer;
			 BSTR bstr = _com_util::ConvertStringToBSTR(xmlString.c_str());
			 _bstr_t xml = _bstr_t(bstr);
			 SysFreeString(bstr);

			 currrent_pos = ifs.tellg();//�t�@�C���|�C���^�ʒu

			 delete[] tmp;

             return xml;
		 }
	 }
	 if (ifs.eof())	eof = VARIANT_TRUE; //�t�@�C��EOF

	 return _bstr_t(L"");
 }

 //
 // �o�C�i���֕ϊ�
 //
 _bstr_t kmlLogger::getOneBinData()
 {
	 if (oneDataSet == _bstr_t(L"")) return oneDataSet;

	 //base64�ϊ�
	 StrXml *xmlTxt = new StrXml(oneDataSet.GetBSTR());
	 _bstr_t xmlBin = xmlTxt->convertTxt2Bin();
	 delete xmlTxt;
	 return xmlBin;
 }

 //
 //�@���������߂̃��[�h�C���f�b�N�X�擾
 //�@
 int kmlLogger::getVecIndex(double ctm)
 {
	 int vecpos = 0;	//���ݒ�

	 for (unsigned int i = 1; i < info->mode_num; i++)
	 {
		 vecpos = i - 1;	//vector�C���f�b�N�X�ݒ�
		 if (ctm < info->vtime.at(i))
		 {
			 break;
		 }
	 }

	 currrent_pos = info->vfpos.at(vecpos);

	 return vecpos;
 }
 //
 //�@�t�@�C���Đ�����
 //�@
 void kmlLogger::getDataByTime(double ctm, VARIANT_BOOL lessThan)
 {
	 size_t vpos = 0;	//���ݒ�

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
					 //�O��l���J�����g�Ƃ���
					 currrent_pos = pre_pos;
					 break;
				 }
				 else
				 {
					 //���ݒl���J�����g�Ƃ���
					 break;
				 }
			 }
			 else
			 {
				 //���ݒl���J�����g�Ƃ���
				 break;
			 }
		 }
		 pre_pos = currrent_pos;

		 vpos++;
	 }
 }
 //
 //�@�t�@�C��EOF
 //�@
 VARIANT_BOOL kmlLogger::isEof()
 {
	 return eof;
 }
 //
 //�@�Đ����샂�[�hON
 //�@
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
 //�@�Đ����샂�[�h�擾
 //�@
int kmlLogger::isPlaying()
 {
	 return opePlayOn;
 }
//
//�@���̃f�[�^�Z�b�g��ݒ�
//�@
void  kmlLogger::setNextData()
{
	_bstr_t next = getOneData(); 
	oneDataSet = next;

}
 //
 //�@�Đ�
 //�@
 _bstr_t  kmlLogger::getData()
 {
	 _bstr_t current;

	 if (opePlayOn)//�Đ�ON?
	 {
		 if (isEof())
		 {
            //STOP����
			 playOperation(OPE_MODE::STOP, NULL);
			 //�J�����g�ʒu�̃f�[�^�i�o�C�i���ϊ��j���擾
			 current = getOneBinData();
             //��~
			 setOpePlayOn(KLOG_PAUSE);
		 }
		 else if (opePlayOn == KLOG_PLAY_ONCE)
		 {
            //�J�����g�ʒu�̃f�[�^�i�o�C�i���ϊ��j���擾
			 current = getOneBinData();
			 //��~
			 setOpePlayOn(KLOG_PAUSE);
		 }
		 else
		 {
	    	 //�J�����g�ʒu�̃f�[�^�i�o�C�i���ϊ��j���擾
    		 current = getOneBinData();
		     //�t�@�C���Ǎ��݂�i�߂�
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
 // set���b�Z�[�W�ɉ�����
 //    Play���[�h�AOpe���[�h�ݒ�ƃf�[�^�ݒ�
 //
 VARIANT_BOOL kmlLogger::setPlayOpeMode(BSTR xml, struct flightLog *log)
 {
	 setlocale(LC_ALL, "Japanese_Japan.932");	//<-�ǉ�
//	 struct flightLog log = FlightLog::getFlightLogInfo(xml);
	 *log = FlightLog::getFlightLogInfo(xml);
	 VARIANT_BOOL cmdResult = VARIANT_FALSE;

	 if (log->play == PLAY_MODE::OPEN)	//�Đ�
	 {
		 if (!playOn)//�t�@�C�������Ă�����J��
		 {
			 info = new fileInfo();

			 //�t�@�C���p�X�擾
			 StrXml *guiMsg = new StrXml(log->guiMsg);
			 setFilepath(guiMsg->getData(L"root/message/FlightLog/Play/FilePath"));
			 delete guiMsg;


			 //KMZ����KML�֕ϊ�
			 char *kmzfile = _com_util::ConvertBSTRToString(filePath.GetBSTR());
			 //�t�@�C���̊g���q����
			 fileIsKmz = isKmzFile(kmzfile);
			 if (fileIsKmz)
			 {
				 if (!kmzToKml(kmzfile))
				 {
					 delete[] kmzfile;
					 playOn = VARIANT_FALSE;//���샂�[�h��FALSE�ɂ���
					 return VARIANT_FALSE;
				 }
				 //OPEN�t�@�C����
				 size_t  len = strlen(kmzfile);
				 char *filename_try = (char *)malloc(sizeof(char)*(len + 1));
				 strncpy_s(filename_try, len + 1, kmzfile, _TRUNCATE);
				 filename_try[len - 1] = 'l';//�g���q�C��

				 BSTR bpath = _com_util::ConvertStringToBSTR(filename_try);
				 setlocale(LC_ALL, "Japanese_Japan.932");	//<-�ǉ�
				 kmlFilePath = _bstr_t(bpath);
				 setFilepath(kmlFilePath.GetBSTR());
				 SysFreeString(bpath);
				 delete[] filename_try;
			 }
			 else //�ǂݍ��ރt�@�C����KML
			 {
				 setFilepath(filePath.GetBSTR());
				 kmlFilePath = filePath;
			 }
			 delete[] kmzfile;

			 //�t�@�C���Ǎ��ݏ�����
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
		 if (playOn)//�t�@�C�����J���Ă��������
		 {
			 ifs.close();

			 if (fileIsKmz) //
			 {
				 //KML�t�@�C���͍Đ����I��������s�v�Ȃ̂ŏ���
				 remove(kmlFilePath);
			 }

			 playOperation(OPE_MODE::STOP, NULL);
			 cmdResult = VARIANT_TRUE;

			 delete info;
		 }
		 playOn = VARIANT_FALSE;//���샂�[�h��FALSE�ɂ���

		 info->cTime = 0.0;//�J�����g�^�C���̃��Z�b�g
		 eof = VARIANT_FALSE;   

	 }
	 else if (log->play == PLAY_MODE::EDIT)
	 {
		 if (playOn)//�t�@�C�����J���Ă�����ҏW
		 {
			 cmdResult = editFile(*log);
		 }
	 }

	 if (log->ope >= OPE_MODE::STOP) //����R�}���h
	 {
		 cmdResult = playOperation(log->ope, log->guiMsg);
	 }

	 return cmdResult;
 }
 //
 // �Đ�/���샂�[�h
 //
 VARIANT_BOOL kmlLogger::playOperation(int opemode, BSTR xml)
 {
	 VARIANT_BOOL result = VARIANT_FALSE;

	 if (!playOn) return result;	//�Đ����[�h��CLOSE�Ȃ�΂����Ƀ��^�[��

	 double tm;

	 switch (opemode)
	 {
	 case OPE_MODE::STOP: //���O��~
		
		 //�t�@�C���擪�ɖ߂�
		 moveToTopOfFile();
         //�擪�f�[�^�擾
		 oneDataSet = getOneData();
		 //1�񂾂�PLAY
		 setOpePlayOn(KLOG_PLAY_ONCE);

		 //���[�h���X�g�̃C���f�b�N�X��0�N���A
		 info->vpos = 0;   

		 result = VARIANT_TRUE;	//����L��
		 break;

	 case OPE_MODE::PLAY: //���O�Đ�

		 if (opePlayOn) break;	//���ɍĐ����Ȃ�Ή������Ȃ�

		 //���[�h�؂�ւ�����B�����[�h�Ȃ�΁Avpos���C���N�������g
		 if (info->mode_num > (info->vpos + 1))
		 {
			 if (info->vfpos.at(info->vpos + 1) < currrent_pos)	info->vpos++;
		 }
		 //Player ON
		 setOpePlayOn(KLOG_PLAY_FILE);

		 result = VARIANT_TRUE;	//����L��
		 break;

	 case OPE_MODE::PAUSE: //���O�ꎞ��~

		 if (!opePlayOn) break;	//�Đ����łȂ��Ȃ�Ή������Ȃ�

		 //Player OFF
		 setOpePlayOn(KLOG_PAUSE);

		 result = VARIANT_TRUE;	//����L��

		 break;

	 case OPE_MODE::NEXT: //���[�h���

		 if (info->mode_num > (info->vpos + 1)) //vector���X�g�𒴂��Ȃ��H
		 {
			 info->vpos++;
			 currrent_pos = info->vfpos.at(info->vpos);

			 //�J�����g�^�C���̍X�V
			 info->cTime = info->vtime.at(info->vpos);
			 result = VARIANT_TRUE;	//����L��

			 setOpePlayOn(KLOG_PLAY_FILE);
			 getData();

			 //1�񂾂�PLAY
			 setOpePlayOn(KLOG_PLAY_ONCE);
		 }
		 else
		 {
			 //�������ꍇ�͎��ɐi�߂��A���ݒl�L�[�v

		 }
		 break;

	 case OPE_MODE::PREV: //���[�h�߂�

		 if (1 <= info->vpos) //vector���X�g�͈͂���E���Ȃ��H
		 {
			 info->vpos--;
			 currrent_pos = info->vfpos.at(info->vpos);

			 //�J�����g�^�C���̍X�V
			 info->cTime = info->vtime.at(info->vpos);
			 result = VARIANT_TRUE;	//����L��

			 setOpePlayOn(KLOG_PLAY_FILE);
			 getData();

			 //1�񂾂�PLAY
			 setOpePlayOn(KLOG_PLAY_ONCE);
		 }
		 else
		 {
			 //��E����ꍇ�͖߂����A���ݒl�L�[�v

		 }
		 break;

	 case OPE_MODE::SLIDEBAR: //�X���C�h�o�[

		 //���[�h�o��
		 tm   = FlightLog::getPlayCurrentTm(xml);
		 if (tm<0 || tm > info->totalTime) break;

		 info->vpos = getVecIndex(tm);
		 
         //�f�[�^���o��
		 getDataByTime(tm, VARIANT_TRUE);
		 oneDataSet = getOneData();

		 //�J�����g�^�C���̍X�V
		 info->cTime = tm;

		 //1�񂾂�PLAY
		 setOpePlayOn(KLOG_PLAY_ONCE);
		 result = VARIANT_TRUE;	//����L��

		 break;

	 default:
		 break;
	 }

	 return result;
 }

 //
 //�@�Đ��t�@�C���̑�����
 //�@
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
 //�@�Đ��t�@�C���̌��Đ�����
 //�@
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
 // �o�ߕb�i�t�@�C���̐擪�f�[�^����̌o�ߕb�j�̌v�Z
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
 //�@XML������o�ߕb���Z�o
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
 // �Đ��t�@�C���̃J�����g�^�C��
 //
 double kmlLogger::getCtime()
 {
	 return info->cTime;
 }

 //
 // �g���~���O
 //
 VARIANT_BOOL kmlLogger::editFile(struct flightLog log)
 {

	//���݂̑��쓮����o�b�N�A�b�v
	ifstream::pos_type currrent_pos_bkup = currrent_pos;
	size_t vpos_bkup = info->vpos;

	//FlightLog���b�Z�[�W�̉��
	StrXml *guiMsg    = new StrXml(log.guiMsg);
	_bstr_t savePath  = guiMsg->getData(L"root/message/FlightLog/Play/Edit/SaveFilePath");
	double startTime  = atof(guiMsg->getData(L"root/message/FlightLog/Play/Edit/StartTime"));
	double endTime    = atof(guiMsg->getData(L"root/message/FlightLog/Play/Edit/EndTime"));
	delete guiMsg;

	//�g���~���O�͈̓`�F�b�N
	if (startTime <0.0 || startTime > info->totalTime)
	{
		return VARIANT_FALSE; //�ݒ�l�s��
	}
	if (endTime <0.0 || endTime > info->totalTime)
	{
		return VARIANT_FALSE; //�ݒ�l�s��
	}

	//�Đ��t�@�C���̃g���~���O�I���ʒuseek
	info->vpos = getVecIndex(endTime);
	getDataByTime(endTime, VARIANT_TRUE);
	ifstream::pos_type trim_end = currrent_pos;

	//�Đ��t�@�C���̃g���~���O�J�n�ʒuseek
	info->vpos = getVecIndex(startTime);
	getDataByTime(startTime, VARIANT_FALSE);
	ifs.seekg(currrent_pos);

	//�t�@�C���I�[�v��
	std::ofstream ifedit;

	//�t�@�C����
	char *savefile = _com_util::ConvertBSTRToString(savePath.GetBSTR());
	VARIANT_BOOL editKmzFile = isKmzFile(savefile);

	size_t len = strlen(savefile);
	char *kml = (char *)malloc(sizeof(char)*(len + 1));
	strncpy_s(kml, len + 1, savefile, _TRUNCATE);
	if (editKmzFile)
	{
		kml[len - 1] = 'l';
	}

	//kml�t�@�C���I�[�v��
	ifedit.open(kml, ios_base::out);
	//�t�@�C����
	char *filename = (char *)malloc(sizeof(char)*(len + 1));
	strncpy_s(filename, len + 1, kml, len - 4);

    //�w�b�_��
	ifedit<<"<?xml version=\"1.0\" encoding=\"shift_jis\"?>\n";
	ifedit<<"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
	ifedit<<"<Document>\n";
	ifedit << "<name>" << filename << "</name>\n";
	delete[] filename;

    //�f�[�^��
	std::string str;
	while (getline(ifs, str))
	{
		//�g���~���O�I������
		if (currrent_pos > trim_end) break;

		//�t�@�C���̏����o��
		ifedit.write(str.c_str(), strlen(str.c_str()));
		ifedit.put('\n');

		currrent_pos = ifs.tellg();	//�t�@�C���|�C���^�ʒu

		str.clear();
	}

	//�t�b�^��
	ifedit << "</Document>\n</kml>\n";

	//�t�@�C���N���[�Y
	ifedit.close();

	//�ۑ��t�@�C����KMZ�̏ꍇ
	if (editKmzFile == VARIANT_TRUE)
	{
		char *kmz = (char *)malloc(sizeof(char)*(len + 1));
		strncpy_s(kmz, len + 1, savefile, _TRUNCATE);
		kmz[len - 3] = 'k';
		kmz[len - 2] = 'm';
		kmz[len - 1] = 'z';

		kmlToKmz(kmz, kml);
		// ���k�����̂ŕs�v��KML�t�@�C�����폜
		remove(kml);
		//���������
		delete[] kmz;
	}

	//���������
	delete[] savefile;
	delete[] kml;



	//���ɖ߂�
	currrent_pos = currrent_pos_bkup;
	info->vpos = vpos_bkup;

	return VARIANT_TRUE;

 }

 //
 // �t�@�C���̐擪�Ɉړ�
 //
 void kmlLogger::moveToTopOfFile()
 {
	 ifs.clear();
	 ifs.seekg(0, ios_base::beg);
	 currrent_pos = ifs.tellg();	//�t�@�C���|�C���^�ʒu

	 info->cTime = 0.0;//�J�����g�^�C���̃��Z�b�g
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
	 //// �N�����̃J�����g�f�B���N�g����ۑ�
	 //DWORD length = ::GetCurrentDirectory(0, NULL);
	 //TCHAR* curdir = new TCHAR[length];
	 //::GetCurrentDirectory(length, curdir);

	 ////�@�J�����g�f�B���N�g�����ړ�
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

	 // KMZ��
	 //int result = minizip(kmzfilename, kmlfilename);

	 // �J�����g�f�B���N�g�����N���f�B���N�g���Ɉړ�
	 //SetCurrentDirectory(curdir);

	 //���������
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
	 // �N�����̃J�����g�f�B���N�g����ۑ�
	 DWORD length = ::GetCurrentDirectory(0, NULL);
	 TCHAR* curdir = new TCHAR[length];
	 ::GetCurrentDirectory(length, curdir);

	 // �J�����g�f�B���N�g�����ړ�
	 char  *ret = strrchr(kmz, '/');
	 if (ret == NULL) return VARIANT_FALSE;

	 size_t len = ret - kmz;
	 TCHAR *tdirname = new TCHAR[len + 1];
	 size_t temp;
	 setlocale(LC_ALL, "Japanese_Japan.932");//<-�ǉ�
	 mbstowcs_s(&temp, tdirname, len + 1, kmz, _TRUNCATE);
	 SetCurrentDirectory(tdirname);

	 char* kmzfilename;
	 kmzfilename = ret + 1;

	 // KML��
	 int result = miniunz(kmz);//miniunz(kmzfilename);�@//TODO:�𓀃t�@�C�������s��/���{��Ή�/�ۑ������f�B���N�g���łȂ��ƃG���[

	 // �J�����g�f�B���N�g�����N���f�B���N�g���Ɉړ�
	 SetCurrentDirectory(curdir);

	 //���������
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
