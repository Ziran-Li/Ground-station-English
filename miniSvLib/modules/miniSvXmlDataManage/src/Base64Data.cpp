#pragma comment (lib, "Crypt32.lib")

#include "stdafx.h"
#include <comutil.h>
#include <tchar.h>
#include "Base64Data.h" 

//
// BASE64エンコーディング
//
//   pByte   　　：バイト列
//   nLen    　　：バイト列長さ
//   pResult 　　：エンコード結果
//　 bufer_size 　:バッファサイズ
//
int Base64Data::base64encoding(const BYTE* pByte, int nLen, LPTSTR pResult, int bufer_size)
{
	DWORD   cstr   = bufer_size;
 
	BOOL bRet = 
	   CryptBinaryToString((const BYTE*)pByte, nLen, 
							CRYPT_STRING_BASE64| CRYPT_STRING_NOCRLF,
							pResult, &cstr);

   if (!bRet) cstr=0; //変換fail時は0とする

   return( cstr );	  //変換後の文字の長さをリターン
}

//
// BASE64デコーディング
//
//   pByte   　　：バイト列
//   nLen    　　：バイト列長さ
//   pResult 　　：エンコード結果
//　 bufer_size 　:バッファサイズ
//
int Base64Data::base64decoding( const BYTE* pStr, int nLen, BYTE* pResult, int bufer_size)
{

	DWORD cstr   = bufer_size;

	BOOL bRet = 
	   CryptStringToBinary((LPCWSTR)pStr, nLen, 
							CRYPT_STRING_BASE64,
							pResult, &cstr,
							NULL,NULL);

   if (!bRet) cstr=0;   //変換fail時は0とする
   return( cstr );	    //変換後の文字の長さをリターン
}

//
// BASE64エンコーディング
//
//   value   　　：被エンコード対象データ
//   dataType    ：valueのデータ型
//   base64out 　：エンコード結果 （char配列）
//　 size 　　　 : base64outのサイズ
//
// 【注意】 エンコード結果がBASE64_BUFFER_SIZEを超える場合は、
//          BASE64_BUFFER_SIZEを超える分は切り捨てられるため、完全ではない。
//
void Base64Data::encode(void *value, int dataType, LPTSTR base64out, size_t *size)
{

	//TODO: typedef.hのキーワード使う？

	DWORD   cstr = BASE64_BUFFER_SIZE;
	char *p = (char *)value;

	memset(base64out, 0, BASE64_BUFFER_SIZE);
	char temp[BASE64_BUFFER_SIZE]={0};

	switch (dataType)
	{
	case DATA_TYPE_KEYS::R :	//倍精度実数型
		*size = base64encoding( (BYTE*)p, sizeof(double), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::SR :	//単精度実数型
		*size = base64encoding( (BYTE*)p, sizeof(float), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::LR :	//倍々精度実数型 
		*size = base64encoding( (BYTE*)p, sizeof(long double), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::I :	//符号付き32bit整数型
		*size = base64encoding( (BYTE*)p, sizeof(int), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::SI :	//符号付き16bit整数型
		*size = base64encoding( (BYTE*)p, sizeof(short), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::LI :	//符号付き64bit整数型
		*size = base64encoding( (BYTE*)p, sizeof(long long), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::UI :	//符号なし32bit整数型
		*size = base64encoding( (BYTE*)p, sizeof(unsigned int), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::USI :	//符号なし16bit整数型
		*size = base64encoding( (BYTE*)p, sizeof(unsigned short), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::ULI:	//符号なし64bit整数型
		*size = base64encoding( (BYTE*)p, sizeof(unsigned long long), base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::X:	//符号なし8ビット整数型
		*size = base64encoding( (BYTE*)p, 1, base64out, cstr);
		 break;

	case DATA_TYPE_KEYS::B:	//ブール代数型（1文字）
		 //バッファサイズを超えてコピーすることのないよう、一旦tempに格納
		strncpy_s(temp , BASE64_BUFFER_SIZE, (char *)value ,1); 
		mbstowcs_s(size,base64out,BASE64_BUFFER_SIZE,temp  ,1);
		break;

	case DATA_TYPE_KEYS::S:	//文字列
		 //バッファサイズを超えてコピーすることのないよう、一旦tempに格納
		strncpy_s(temp , BASE64_BUFFER_SIZE, (char *)value ,_TRUNCATE); 
		mbstowcs_s(size,base64out,BASE64_BUFFER_SIZE,temp,_TRUNCATE);
		break;

	default: 
		*size = base64encoding( (BYTE*)p, sizeof(double), base64out, cstr);
		break;
	}

}

//
// BASE64デコーディング
//
//   value   　　：バイト列
//   out 　　    ：エンコード結果（char配列のポインタ）
//　 out_size 　 : out配列格納サイズ
//
void Base64Data::decode(LPCTSTR value, char *out, size_t *out_size)
{

	DWORD   cstr = BASE64_BUFFER_SIZE;

	int    len = lstrlen(value);	//valueの配列サイズmaxはBASE64_BUFFER_SIZE
	*out_size = base64decoding((BYTE*)value, len, (BYTE*)out, cstr);
}

//
// BASE64デコーディング(文字列出力)
//
//   value   　　：バイト列
//   dataType    ：データ型ID
//　 out_size　　：変換後文字列
//
void Base64Data::decodeString(LPCTSTR value, int dataType, char *outchar )
{
	char out[BASE64_BUFFER_SIZE]={};
	char    temp[BASE64_BUFFER_SIZE]={};
	size_t out_size;

	memset(outchar, 0, BASE64_BUFFER_SIZE);

	switch (dataType)
	{
	case DATA_TYPE_KEYS::R :	//倍精度実数型
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%15.8e", *((double*)temp));//TODO：この書式子でよい？
		 break;

	case DATA_TYPE_KEYS::SR :	//単精度実数型
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%f", *((float*)temp));
		 break;

	case DATA_TYPE_KEYS::LR :	//倍々精度実数型 
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%15.8e", *((long double*)temp));	//TODO：この書式子でよい？
		 break;

	case DATA_TYPE_KEYS::I :	//符号付き32bit整数型
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%d", *((int*)temp));	
		 break;

	case DATA_TYPE_KEYS::SI :	//符号付き16bit整数型
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%d", *((short*)temp));
		 break;

	case DATA_TYPE_KEYS::LI :	//符号付き64bit整数型
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%lld",  *((long long*)temp));
		 break;

	case DATA_TYPE_KEYS::UI :	//符号なし32bit整数型
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%u",  *((unsigned int *)temp));	
		 break;

	case DATA_TYPE_KEYS::USI :	//符号なし16bit整数型
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%u", *((unsigned short *)temp));	
		 break;

	case DATA_TYPE_KEYS::ULI:	//符号なし64bit整数型
		 decode(value, temp, &out_size);
		 sprintf_s(outchar,BASE64_BUFFER_SIZE, "%llu",  *((unsigned long long *)temp));	
		 break;

	case DATA_TYPE_KEYS::X:	//符号なし8ビット整数型
		 decode(value, temp, &out_size);
		 sprintf_s(outchar, BASE64_BUFFER_SIZE, "%u", (unsigned char)temp[0]);
		 break;

	case DATA_TYPE_KEYS::B:	//ブール代数型（1文字）
		 size_t size;
		 wcstombs_s(&size, outchar, BASE64_BUFFER_SIZE, value, 1);
		 break;

	case DATA_TYPE_KEYS::S:	//文字列
  		 wcstombs_s(&size, outchar, BASE64_BUFFER_SIZE, value, _TRUNCATE);
		 break;

	default://倍精度実数型
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
	for(size1=0; size1< strlen(c); size1++)	//アルファベット部を抜き出す
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
			if (strncmp((it->second).c_str(), c, size1) == 0) //文字列一致
			{
				delete[] c;
				return it->first;
			}
		}
		++it;
	}
	delete[] c;
	return DATA_TYPE_KEYS::R; //属性tがない場合はデフォルトを返す
}


VARIANT_BOOL Base64Data::convertBintoText(BSTR value_base64, BSTR attT ,BSTR *value_text)
{

	char pstr[BASE64_BUFFER_SIZE];

	//属性t ID
	int tID = Base64Data::getAttTID(attT);

	Base64Data::decodeString(value_base64, tID, pstr);

	*value_text = _com_util::ConvertStringToBSTR(pstr);

	return VARIANT_TRUE; //
}

//
// BASE64エンコーディング（配列）【double】
//
//   value   　　：被エンコード対象データ(配列)
//　 num  　　　 : 配列の要素数
//   base64out 　：エンコード結果 （char配列）
//
void Base64Data::encodeArray(double value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64エンコード用
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
// BASE64エンコーディング（配列）【float】
//
//   value   　　：被エンコード対象データ(配列)
//　 num  　　　 : 配列の要素数
//   base64out 　：エンコード結果 （char配列）
//
void Base64Data::encodeArray(float value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64エンコード用
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
// BASE64エンコーディング（配列）【long double】
//
//   value   　　：被エンコード対象データ(配列)
//　 num  　　　 : 配列の要素数
//   base64out 　：エンコード結果 （char配列）
//
void Base64Data::encodeArray(long double value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64エンコード用
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
// BASE64エンコーディング（配列）【int】
//
//   value   　　：被エンコード対象データ(配列)
//　 num  　　　 : 配列の要素数
//   base64out 　：エンコード結果 （char配列）
//
void Base64Data::encodeArray(int value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64エンコード用
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
// BASE64エンコーディング（配列）【short】
//
//   value   　　：被エンコード対象データ(配列)
//　 num  　　　 : 配列の要素数
//   base64out 　：エンコード結果 （char配列）
//
void Base64Data::encodeArray(short value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64エンコード用
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
// BASE64エンコーディング（配列）【long long】
//
//   value   　　：被エンコード対象データ(配列)
//　 num  　　　 : 配列の要素数
//   base64out 　：エンコード結果 （char配列）
//
void Base64Data::encodeArray(long long value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64エンコード用
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
// BASE64エンコーディング（配列）【unsigned int】
//
//   value   　　：被エンコード対象データ(配列)
//　 num  　　　 : 配列の要素数
//   base64out 　：エンコード結果 （char配列）
//
void Base64Data::encodeArray(unsigned int value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64エンコード用
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
// BASE64エンコーディング（配列）【unsigned short int】
//
//   value   　　：被エンコード対象データ(配列)
//　 num  　　　 : 配列の要素数
//   base64out 　：エンコード結果 （char配列）
//
void Base64Data::encodeArray(unsigned short int value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64エンコード用
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
// BASE64エンコーディング（配列）【unsigned long long】
//
//   value   　　：被エンコード対象データ(配列)
//　 num  　　　 : 配列の要素数
//   base64out 　：エンコード結果 （char配列）
//
void Base64Data::encodeArray(unsigned long long value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64エンコード用
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
// BASE64エンコーディング（配列）【unsigned char】
//
//   value   　　：被エンコード対象データ(配列)
//　 num  　　　 : 配列の要素数
//   base64out 　：エンコード結果 （char配列）
//
void Base64Data::encodeArray(unsigned char value[], int num, LPTSTR base64out)
{
	TCHAR pstr[BASE64_BUFFER_SIZE]; //base64エンコード用
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
	case Base64Data::R :	//倍精度実数型
		d1 = atof(data);
		Base64Data::encode(&d1, Base64Data::R, pstr, &outsize);
		break;

	case Base64Data::SR :	//単精度実数型
		 d2 =(float)atof(data);
		 Base64Data::encode(&d2, Base64Data::SR, pstr, &outsize);
		 break;

	case Base64Data::LR :	//倍々精度実数型 
		 d3 =atof(data);
		 Base64Data::encode(&d3, Base64Data::LR, pstr, &outsize);
		 break;

	case Base64Data::I :	//符号付き32bit整数型
		 d4 =atoi(data);
		 Base64Data::encode(&d4, Base64Data::I, pstr, &outsize);
		 break;

	case Base64Data::SI :	//符号付き16bit整数型
		 d5 =atoi(data);
		 Base64Data::encode(&d5, Base64Data::SI, pstr, &outsize);
		 break;

	case Base64Data::LI :	//符号付き64bit整数型
		//d6 = std::stroll(data, NULL, 10);
		d6 = atoll(data);
		Base64Data::encode(&d6, Base64Data::LI, pstr, &outsize);
		 break;

	case Base64Data::UI :	//符号なし32bit整数型
 		 d7 = std::strtoul(data , NULL, 10);
		 Base64Data::encode(&d7, Base64Data::UI, pstr, &outsize);
		 break;

	case Base64Data::USI :	//符号なし16bit整数型
		 d8 = (unsigned short)std::strtoul(data , NULL, 10);
		 Base64Data::encode(&d8, Base64Data::USI, pstr, &outsize);
		 break;

	case Base64Data::ULI:	//符号なし64bit整数型
		 d9 = std::strtoull(data, NULL, 10);
		 Base64Data::encode(&d9, Base64Data::ULI, pstr, &outsize);
		 break;

	case Base64Data::X:	//符号なし8ビット整数型
		 d10 = atoi(data);
		 Base64Data::encode(&d10, Base64Data::X, pstr, &outsize);
		 break;

	case Base64Data::B:	//ブール代数型（1文字）
		 c = data[0];		
		 Base64Data::encode(&c, Base64Data::B, pstr, &outsize);
		 break;

	case Base64Data::S:	//文字列
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
	case Base64Data::R:	//倍精度実数型
		Base64Data::decodeString(val, Base64Data::R, test_decode);
		break;

	case Base64Data::SR:	//単精度実数型
		Base64Data::decodeString(val, Base64Data::SR, test_decode);
		break;

	case Base64Data::LR:	//倍々精度実数型 
		Base64Data::decodeString(val, Base64Data::LR, test_decode);
		break;

	case Base64Data::I:	//符号付き32bit整数型
		Base64Data::decodeString(val, Base64Data::I, test_decode);
		break;

	case Base64Data::SI:	//符号付き16bit整数型
		Base64Data::decodeString(val, Base64Data::SI, test_decode);
		break;

	case Base64Data::LI:	//符号付き64bit整数型
		Base64Data::decodeString(val, Base64Data::LI, test_decode);
		break;

	case Base64Data::UI:	//符号なし32bit整数型
		Base64Data::decodeString(val, Base64Data::UI, test_decode);
		break;

	case Base64Data::USI:	//符号なし16bit整数型
		Base64Data::decodeString(val, Base64Data::USI, test_decode);
		break;

	case Base64Data::ULI:	//符号なし64bit整数型
		Base64Data::decodeString(val, Base64Data::ULI, test_decode);
		break;

	case Base64Data::X:	//符号なし8ビット整数型
		Base64Data::decodeString(val, Base64Data::X, test_decode);
		break;

	case Base64Data::B:	//ブール代数型（1文字）
		Base64Data::decodeString(val, Base64Data::B, test_decode);
		break;

	case Base64Data::S:	//文字列
		Base64Data::decodeString(val, Base64Data::S, test_decode);
		break;

	default:
		Base64Data::decodeString(val, Base64Data::R, test_decode);
		break;
	}

}

