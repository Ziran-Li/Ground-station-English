#pragma comment (lib, "Crypt32.lib")

#include "stdafx.h"
#include <comutil.h>
#include <tchar.h>
#include "Base64Data.h" 

//
// BASE64�G���R�[�f�B���O
//
//   pByte   �@�@�F�o�C�g��
//   nLen    �@�@�F�o�C�g�񒷂�
//   pResult �@�@�F�G���R�[�h����
//�@ bufer_size �@:�o�b�t�@�T�C�Y
//
int Base64Data::base64encoding(const BYTE* pByte, int nLen, LPTSTR pResult, int bufer_size)
{
	DWORD   cstr   = bufer_size;
 
	BOOL bRet = 
	   CryptBinaryToString((const BYTE*)pByte, nLen, 
							CRYPT_STRING_BASE64| CRYPT_STRING_NOCRLF,
							pResult, &cstr);

   if (!bRet) cstr=0; //�ϊ�fail����0�Ƃ���

   return( cstr );	  //�ϊ���̕����̒��������^�[��
}

//
// BASE64�f�R�[�f�B���O
//
//   pByte   �@�@�F�o�C�g��
//   nLen    �@�@�F�o�C�g�񒷂�
//   pResult �@�@�F�G���R�[�h����
//�@ bufer_size �@:�o�b�t�@�T�C�Y
//
int Base64Data::base64decoding( const BYTE* pStr, int nLen, BYTE* pResult, int bufer_size)
{

	DWORD cstr   = bufer_size;

	BOOL bRet = 
	   CryptStringToBinary((LPCWSTR)pStr, nLen, 
							CRYPT_STRING_BASE64,
							pResult, &cstr,
							NULL,NULL);

   if (!bRet) cstr=0;   //�ϊ�fail����0�Ƃ���
   return( cstr );	    //�ϊ���̕����̒��������^�[��
}

//
// BASE64�G���R�[�f�B���O
//
//   value   �@�@�F��G���R�[�h�Ώۃf�[�^
//   dataType    �Fvalue�̃f�[�^�^
//   base64out �@�F�G���R�[�h���� �ichar�z��j
//�@ size �@�@�@ : base64out�̃T�C�Y
//
// �y���Ӂz �G���R�[�h���ʂ�BASE64_BUFFER_SIZE�𒴂���ꍇ�́A
//          BASE64_BUFFER_SIZE�𒴂��镪�͐؂�̂Ă��邽�߁A���S�ł͂Ȃ��B
//
void Base64Data::encode(void *value, int dataType, LPTSTR base64out, size_t *size)
{

	//TODO: typedef.h�̃L�[���[�h�g���H

	DWORD   cstr = BASE64_BUFFER_SIZE;
	char *p = (char *)value;

	memset(base64out, 0, BASE64_BUFFER_SIZE);
	char temp[BASE64_BUFFER_SIZE]={0};

	switch (dataType)
	{
	case DATA_TYPE_KEYS::R :	//�{���x�����^
		*size = base64encoding( (BYTE*)p, sizeof(double), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::SR :	//�P���x�����^
		*size = base64encoding( (BYTE*)p, sizeof(float), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::LR :	//�{�X���x�����^ 
		*size = base64encoding( (BYTE*)p, sizeof(long double), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::I :	//�����t��32bit�����^
		*size = base64encoding( (BYTE*)p, sizeof(int), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::SI :	//�����t��16bit�����^
		*size = base64encoding( (BYTE*)p, sizeof(short), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::LI :	//�����t��64bit�����^
		*size = base64encoding( (BYTE*)p, sizeof(long long), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::UI :	//�����Ȃ�32bit�����^
		*size = base64encoding( (BYTE*)p, sizeof(unsigned int), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::USI :	//�����Ȃ�16bit�����^
		*size = base64encoding( (BYTE*)p, sizeof(unsigned short), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::ULI:	//�����Ȃ�64bit�����^
		*size = base64encoding( (BYTE*)p, sizeof(unsigned long long), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::X:	//�����Ȃ�8�r�b�g�����^
		*size = base64encoding( (BYTE*)p, 1, base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::B:	//�u�[���㐔�^�i1�����j
		 //�o�b�t�@�T�C�Y�𒴂��ăR�s�[���邱�Ƃ̂Ȃ��悤�A��Utemp�Ɋi�[
		strncpy_s(temp , BASE64_BUFFER_SIZE, (char *)value ,1); 
		mbstowcs_s(size,base64out,BASE64_BUFFER_SIZE,temp  ,1);
		break;

	case DATA_TYPE_KEYS::S:	//������
		 //�o�b�t�@�T�C�Y�𒴂��ăR�s�[���邱�Ƃ̂Ȃ��悤�A��Utemp�Ɋi�[
		strncpy_s(temp , BASE64_BUFFER_SIZE, (char *)value ,_TRUNCATE); 
		mbstowcs_s(size,base64out,BASE64_BUFFER_SIZE,temp,_TRUNCATE);
		break;

	default: 
		*size = base64encoding( (BYTE*)p, sizeof(double), base64out, cstr);
		break;
	}

}

//
// BASE64�f�R�[�f�B���O
//
//   value   �@�@�F�o�C�g��
//   out �@�@    �F�G���R�[�h���ʁichar�z��̃|�C���^�j
//�@ out_size �@ : out�z��i�[�T�C�Y
//
void Base64Data::decode(LPCTSTR value, char *out, size_t *out_size)
{

	DWORD   cstr = BASE64_BUFFER_SIZE;

	int    len = lstrlen(value);	//value�̔z��T�C�Ymax��BASE64_BUFFER_SIZE
	*out_size = base64decoding((BYTE*)value, len, (BYTE*)out, cstr);
}

//
// BASE64�f�R�[�f�B���O(������o��)
//
//   value   �@�@�F�o�C�g��
//   dataType    �F�f�[�^�^ID
//�@ out_size�@�@�F�ϊ��㕶����
//
void Base64Data::decodeString(LPCTSTR value, int dataType, char *outchar )
{
	char out[BASE64_BUFFER_SIZE]={};
	char    temp[BASE64_BUFFER_SIZE]={};
	size_t out_size;

	memset(outchar, 0, BASE64_BUFFER_SIZE);

	switch (dataType)
	{
	case DATA_TYPE_KEYS::R :	//�{���x�����^
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%15.8e", *((double*)temp));//TODO�F���̏����q�ł悢�H
		 break;

	case DATA_TYPE_KEYS::SR :	//�P���x�����^
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%f", *((float*)temp));
		 break;

	case DATA_TYPE_KEYS::LR :	//�{�X���x�����^ 
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%15.8e", *((long double*)temp));	//TODO�F���̏����q�ł悢�H
		 break;

	case DATA_TYPE_KEYS::I :	//�����t��32bit�����^
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%d", *((int*)temp));	
		 break;

	case DATA_TYPE_KEYS::SI :	//�����t��16bit�����^
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%d", *((short*)temp));
		 break;

	case DATA_TYPE_KEYS::LI :	//�����t��64bit�����^
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%lld",  *((long long*)temp));
		 break;

	case DATA_TYPE_KEYS::UI :	//�����Ȃ�32bit�����^
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%u",  *((unsigned int *)temp));	
		 break;

	case DATA_TYPE_KEYS::USI :	//�����Ȃ�16bit�����^
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%u", *((unsigned short *)temp));	
		 break;

	case DATA_TYPE_KEYS::ULI:	//�����Ȃ�64bit�����^
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%llu",  *((unsigned long long *)temp));	
		 break;

	case DATA_TYPE_KEYS::X:	//�����Ȃ�8�r�b�g�����^
		 decode(value, temp, &out_size);
		 sprintf_s(outchar, BASE64_BUFFER_SIZE, "%u", (unsigned char)temp[0]);
		 break;

	case DATA_TYPE_KEYS::B:	//�u�[���㐔�^�i1�����j
		 size_t size;
		 wcstombs_s(&size, outchar, BASE64_BUFFER_SIZE, value, 1);
		 break;

	case DATA_TYPE_KEYS::S:	//������
  		 wcstombs_s(&size, outchar, BASE64_BUFFER_SIZE, value, _TRUNCATE);
		 break;

	default://�{���x�����^
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%15.8e", *((double*)temp));
		 break;
	}
	
}

int Base64Data::getAttTID(BSTR attT)
{
	map<int,string>::iterator it = dataTypeMap.begin();
	char* c = _com_util::ConvertBSTRToString(attT);
	size_t size1 = 0;
	for(size1=0; size1< strlen(c); size1++)	//�A���t�@�x�b�g���𔲂��o��
	{
		if(!isalpha(c[size1]))
		{
			break;
		}
	}

	while( it != dataTypeMap.end() ) 
	{
		size_t size2 = strlen((it->second).c_str());
		if(size1 == size2)
		{
			if (strncmp((it->second).c_str(), c, size1) == 0) //�������v
			{
				delete[] c;
				return it->first;
			}
		}
		++it;
	}
	delete[] c;
	return DATA_TYPE_KEYS::R; //����t���Ȃ��ꍇ�̓f�t�H���g��Ԃ�
}


VARIANT_BOOL Base64Data::convertBintoText(BSTR value_base64, BSTR attT ,BSTR *value_text)
{

	char pstr[BASE64_BUFFER_SIZE];

	//����t ID
	int tID = Base64Data::getAttTID(attT);

	Base64Data::decodeString(value_base64, tID, pstr);

	*value_text = _com_util::ConvertStringToBSTR(pstr);

	return VARIANT_TRUE; //
}

//
// BASE64�G���R�[�f�B���O�i�z��j�ydouble�z
//
//   value   �@�@�F��G���R�[�h�Ώۃf�[�^(�z��)
//�@ num  �@�@�@ : �z��̗v�f��
//   base64out �@�F�G���R�[�h���� �ichar�z��j
//
void Base64Data::encodeArray(double value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64�G���R�[�h�p
	size_t  outsize;

	TCHAR tmp[BASE64_BUFFER_SIZE] = {0};
	memset(base64out, 0, BASE64_BUFFER_SIZE);

	for (int i = 0; i < num-1; i++){
		encode(&value[i], Base64Data::R, pstr, &outsize);
		_tcsncat_s(tmp, pstr, outsize);
		_tcsncat_s(tmp, L" ", 1);
	}
	encode(&value[num - 1], Base64Data::R, pstr, &outsize);
	_tcsncat_s(tmp, pstr, outsize);

	_tcsncpy_s(base64out, BASE64_BUFFER_SIZE, tmp, sizeof(tmp));
}
//
// BASE64�G���R�[�f�B���O�i�z��j�yfloat�z
//
//   value   �@�@�F��G���R�[�h�Ώۃf�[�^(�z��)
//�@ num  �@�@�@ : �z��̗v�f��
//   base64out �@�F�G���R�[�h���� �ichar�z��j
//
void Base64Data::encodeArray(float value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64�G���R�[�h�p
	size_t  outsize;

	TCHAR tmp[BASE64_BUFFER_SIZE] = { 0 };
	memset(base64out, 0, BASE64_BUFFER_SIZE);

	for (int i = 0; i < num - 1; i++){
		encode(&value[i], Base64Data::SR, pstr, &outsize);
		_tcsncat_s(tmp, pstr, outsize);
		_tcsncat_s(tmp, L" ", 1);
	}
	encode(&value[num - 1], Base64Data::SR, pstr, &outsize);
	_tcsncat_s(tmp, pstr, outsize);

	_tcsncpy_s(base64out, BASE64_BUFFER_SIZE, tmp, sizeof(tmp));
}
//
// BASE64�G���R�[�f�B���O�i�z��j�ylong double�z
//
//   value   �@�@�F��G���R�[�h�Ώۃf�[�^(�z��)
//�@ num  �@�@�@ : �z��̗v�f��
//   base64out �@�F�G���R�[�h���� �ichar�z��j
//
void Base64Data::encodeArray(long double value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64�G���R�[�h�p
	size_t  outsize;

	TCHAR tmp[BASE64_BUFFER_SIZE] = { 0 };
	memset(base64out, 0, BASE64_BUFFER_SIZE);

	for (int i = 0; i < num - 1; i++){
		encode(&value[i], Base64Data::LR, pstr, &outsize);
		_tcsncat_s(tmp, pstr, outsize);
		_tcsncat_s(tmp, L" ", 1);
	}
	encode(&value[num - 1], Base64Data::LR, pstr, &outsize);
	_tcsncat_s(tmp, pstr, outsize);

	_tcsncpy_s(base64out, BASE64_BUFFER_SIZE, tmp, sizeof(tmp));
}
//
// BASE64�G���R�[�f�B���O�i�z��j�yint�z
//
//   value   �@�@�F��G���R�[�h�Ώۃf�[�^(�z��)
//�@ num  �@�@�@ : �z��̗v�f��
//   base64out �@�F�G���R�[�h���� �ichar�z��j
//
void Base64Data::encodeArray(int value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64�G���R�[�h�p
	size_t  outsize;

	TCHAR tmp[BASE64_BUFFER_SIZE] = { 0 };
	memset(base64out, 0, BASE64_BUFFER_SIZE);

	for (int i = 0; i < num - 1; i++){
		encode(&value[i], Base64Data::I, pstr, &outsize);
		_tcsncat_s(tmp, pstr, outsize);
		_tcsncat_s(tmp, L" ", 1);
	}
	encode(&value[num - 1], Base64Data::I, pstr, &outsize);
	_tcsncat_s(tmp, pstr, outsize);

	_tcsncpy_s(base64out, BASE64_BUFFER_SIZE, tmp, sizeof(tmp));
}
//
// BASE64�G���R�[�f�B���O�i�z��j�yshort�z
//
//   value   �@�@�F��G���R�[�h�Ώۃf�[�^(�z��)
//�@ num  �@�@�@ : �z��̗v�f��
//   base64out �@�F�G���R�[�h���� �ichar�z��j
//
void Base64Data::encodeArray(short value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64�G���R�[�h�p
	size_t  outsize;

	TCHAR tmp[BASE64_BUFFER_SIZE] = { 0 };
	memset(base64out, 0, BASE64_BUFFER_SIZE);

	for (int i = 0; i < num - 1; i++){
		encode(&value[i], Base64Data::SI, pstr, &outsize);
		_tcsncat_s(tmp, pstr, outsize);
		_tcsncat_s(tmp, L" ", 1);
	}
	encode(&value[num - 1], Base64Data::SI, pstr, &outsize);
	_tcsncat_s(tmp, pstr, outsize);

	_tcsncpy_s(base64out, BASE64_BUFFER_SIZE, tmp, sizeof(tmp));
}
//
// BASE64�G���R�[�f�B���O�i�z��j�ylong long�z
//
//   value   �@�@�F��G���R�[�h�Ώۃf�[�^(�z��)
//�@ num  �@�@�@ : �z��̗v�f��
//   base64out �@�F�G���R�[�h���� �ichar�z��j
//
void Base64Data::encodeArray(long long value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64�G���R�[�h�p
	size_t  outsize;

	TCHAR tmp[BASE64_BUFFER_SIZE] = {0};
	memset(base64out, 0, BASE64_BUFFER_SIZE);

	for (int i = 0; i < num-1; i++){
		encode(&value[i], Base64Data::LI, pstr, &outsize);
		_tcsncat_s(tmp, pstr, outsize);
		_tcsncat_s(tmp, L" ", 1);
	}
	encode(&value[num - 1], Base64Data::LI, pstr, &outsize);
	_tcsncat_s(tmp, pstr, outsize);

	_tcsncpy_s(base64out, BASE64_BUFFER_SIZE, tmp, sizeof(tmp));
}
//
// BASE64�G���R�[�f�B���O�i�z��j�yunsigned int�z
//
//   value   �@�@�F��G���R�[�h�Ώۃf�[�^(�z��)
//�@ num  �@�@�@ : �z��̗v�f��
//   base64out �@�F�G���R�[�h���� �ichar�z��j
//
void Base64Data::encodeArray(unsigned int value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64�G���R�[�h�p
	size_t  outsize;

	TCHAR tmp[BASE64_BUFFER_SIZE] = { 0 };
	memset(base64out, 0, BASE64_BUFFER_SIZE);

	for (int i = 0; i < num - 1; i++){
		encode(&value[i], Base64Data::UI, pstr, &outsize);
		_tcsncat_s(tmp, pstr, outsize);
		_tcsncat_s(tmp, L" ", 1);
	}
	encode(&value[num - 1], Base64Data::UI, pstr, &outsize);
	_tcsncat_s(tmp, pstr, outsize);

	_tcsncpy_s(base64out, BASE64_BUFFER_SIZE, tmp, sizeof(tmp));
}
//
// BASE64�G���R�[�f�B���O�i�z��j�yunsigned short int�z
//
//   value   �@�@�F��G���R�[�h�Ώۃf�[�^(�z��)
//�@ num  �@�@�@ : �z��̗v�f��
//   base64out �@�F�G���R�[�h���� �ichar�z��j
//
void Base64Data::encodeArray(unsigned short int value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64�G���R�[�h�p
	size_t  outsize;

	TCHAR tmp[BASE64_BUFFER_SIZE] = { 0 };
	memset(base64out, 0, BASE64_BUFFER_SIZE);

	for (int i = 0; i < num - 1; i++){
		encode(&value[i], Base64Data::USI, pstr, &outsize);
		_tcsncat_s(tmp, pstr, outsize);
		_tcsncat_s(tmp, L" ", 1);
	}
	encode(&value[num - 1], Base64Data::USI, pstr, &outsize);
	_tcsncat_s(tmp, pstr, outsize);

	_tcsncpy_s(base64out, BASE64_BUFFER_SIZE, tmp, sizeof(tmp));
}
//
// BASE64�G���R�[�f�B���O�i�z��j�yunsigned long long�z
//
//   value   �@�@�F��G���R�[�h�Ώۃf�[�^(�z��)
//�@ num  �@�@�@ : �z��̗v�f��
//   base64out �@�F�G���R�[�h���� �ichar�z��j
//
void Base64Data::encodeArray(unsigned long long value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64�G���R�[�h�p
	size_t  outsize;

	TCHAR tmp[BASE64_BUFFER_SIZE] = { 0 };
	memset(base64out, 0, BASE64_BUFFER_SIZE);

	for (int i = 0; i < num - 1; i++){
		encode(&value[i], Base64Data::ULI, pstr, &outsize);
		_tcsncat_s(tmp, pstr, outsize);
		_tcsncat_s(tmp, L" ", 1);
	}
	encode(&value[num - 1], Base64Data::ULI, pstr, &outsize);
	_tcsncat_s(tmp, pstr, outsize);

	_tcsncpy_s(base64out, BASE64_BUFFER_SIZE, tmp, sizeof(tmp));
}
//
// BASE64�G���R�[�f�B���O�i�z��j�yunsigned char�z
//
//   value   �@�@�F��G���R�[�h�Ώۃf�[�^(�z��)
//�@ num  �@�@�@ : �z��̗v�f��
//   base64out �@�F�G���R�[�h���� �ichar�z��j
//
void Base64Data::encodeArray(unsigned char value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64�G���R�[�h�p
	size_t  outsize;

	TCHAR tmp[BASE64_BUFFER_SIZE] = { 0 };
	memset(base64out, 0, BASE64_BUFFER_SIZE);

	for (int i = 0; i < num - 1; i++){
		encode(&value[i], Base64Data::X, pstr, &outsize);
		_tcsncat_s(tmp, pstr, outsize);
		_tcsncat_s(tmp, L" ", 1);
	}
	encode(&value[num - 1], Base64Data::X, pstr, &outsize);
	_tcsncat_s(tmp, pstr, outsize);

	_tcsncpy_s(base64out, BASE64_BUFFER_SIZE, tmp, sizeof(tmp));
}

void Base64Data::convTxtValue(int attTID, char *data, LPTSTR pstr)
{
	size_t outsize;
	double		d1;
	float		d2;
	long double d3;
	int			d4;
	short		d5;
	long long int      d6;
	unsigned int       d7;
	unsigned short     d8;
	unsigned long long d9;
	unsigned char      d10;
	char c;

	char temp[BASE64_BUFFER_SIZE]={0};

	switch (attTID)
	{
	case Base64Data::R :	//�{���x�����^
		d1 = atof(data);
		Base64Data::encode(&d1, Base64Data::R, pstr, &outsize);
		break;

	case Base64Data::SR :	//�P���x�����^
		 d2 =(float)atof(data);
		 Base64Data::encode(&d2, Base64Data::SR, pstr, &outsize);
		 break;

	case Base64Data::LR :	//�{�X���x�����^ 
		 d3 =atof(data);
		 Base64Data::encode(&d3, Base64Data::LR, pstr, &outsize);
		 break;

	case Base64Data::I :	//�����t��32bit�����^
		 d4 =atoi(data);
		 Base64Data::encode(&d4, Base64Data::I, pstr, &outsize);
		 break;

	case Base64Data::SI :	//�����t��16bit�����^
		 d5 =atoi(data);
		 Base64Data::encode(&d5, Base64Data::SI, pstr, &outsize);
		 break;

	case Base64Data::LI :	//�����t��64bit�����^
		//d6 = std::stroll(data, NULL, 10);
		d6 = atoll(data);
		Base64Data::encode(&d6, Base64Data::LI, pstr, &outsize);
		 break;

	case Base64Data::UI :	//�����Ȃ�32bit�����^
 		 d7 = std::strtoul(data , NULL, 10);
		 Base64Data::encode(&d7, Base64Data::UI, pstr, &outsize);
		 break;

	case Base64Data::USI :	//�����Ȃ�16bit�����^
		 d8 = (unsigned short)std::strtoul(data , NULL, 10);
		 Base64Data::encode(&d8, Base64Data::USI, pstr, &outsize);
		 break;

	case Base64Data::ULI:	//�����Ȃ�64bit�����^
		 d9 = std::strtoull(data, NULL, 10);
		 Base64Data::encode(&d9, Base64Data::ULI, pstr, &outsize);
		 break;

	case Base64Data::X:	//�����Ȃ�8�r�b�g�����^
		 d10 = atoi(data);
		 Base64Data::encode(&d10, Base64Data::X, pstr, &outsize);
		 break;

	case Base64Data::B:	//�u�[���㐔�^�i1�����j
		 c = data[0];		
		 Base64Data::encode(&c, Base64Data::B, pstr, &outsize);
		 break;

	case Base64Data::S:	//������
         Base64Data::encode(data, Base64Data::S, pstr, &outsize);	
		break;

	default: 
		double d =atof(data);
		Base64Data::encode(&d, Base64Data::R, pstr, &outsize);
		break; 
	}

}

void Base64Data::convBinValue(int attTID, LPTSTR val, char test_decode[BASE64_BUFFER_SIZE])
{
	char out[BASE64_BUFFER_SIZE] = {};
	char    temp[BASE64_BUFFER_SIZE] = {};

	memset(test_decode, 0, BASE64_BUFFER_SIZE);

	switch (attTID)
	{
	case Base64Data::R:	//�{���x�����^
		Base64Data::decodeString(val, Base64Data::R, test_decode);
		break;

	case Base64Data::SR:	//�P���x�����^
		Base64Data::decodeString(val, Base64Data::SR, test_decode);
		break;

	case Base64Data::LR:	//�{�X���x�����^ 
		Base64Data::decodeString(val, Base64Data::LR, test_decode);
		break;

	case Base64Data::I:	//�����t��32bit�����^
		Base64Data::decodeString(val, Base64Data::I, test_decode);
		break;

	case Base64Data::SI:	//�����t��16bit�����^
		Base64Data::decodeString(val, Base64Data::SI, test_decode);
		break;

	case Base64Data::LI:	//�����t��64bit�����^
		Base64Data::decodeString(val, Base64Data::LI, test_decode);
		break;

	case Base64Data::UI:	//�����Ȃ�32bit�����^
		Base64Data::decodeString(val, Base64Data::UI, test_decode);
		break;

	case Base64Data::USI:	//�����Ȃ�16bit�����^
		Base64Data::decodeString(val, Base64Data::USI, test_decode);
		break;

	case Base64Data::ULI:	//�����Ȃ�64bit�����^
		Base64Data::decodeString(val, Base64Data::ULI, test_decode);
		break;

	case Base64Data::X:	//�����Ȃ�8�r�b�g�����^
		Base64Data::decodeString(val, Base64Data::X, test_decode);
		break;

	case Base64Data::B:	//�u�[���㐔�^�i1�����j
		Base64Data::decodeString(val, Base64Data::B, test_decode);
		break;

	case Base64Data::S:	//������
		Base64Data::decodeString(val, Base64Data::S, test_decode);
		break;

	default:
		Base64Data::decodeString(val, Base64Data::R, test_decode);
		break;
	}

}

