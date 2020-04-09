#ifndef BASE64DATA_H
#define BASE64DATA_H

#include <Windows.h>
#include <Wincrypt.h>
using namespace std; 
#include <map>
#include <string.h>

#define BASE64_BUFFER_SIZE (256)

extern	map<int, string> dataTypeMap;

class Base64Data
{
public:
	enum DATA_TYPE_KEYS{ R, SR, LR, I, SI, LI, UI, USI, ULI, X, B, S , type_num,}; 

	static void encode(void *value, int dataType, LPTSTR base64out, size_t *size );
	static void decode(LPCTSTR value,  char *out, size_t *out_size);
	static void decodeString(LPCTSTR value, int dataType, char *out );
	static int  getAttTID(BSTR value);
	static VARIANT_BOOL convertBintoText(BSTR value_base64, BSTR attT ,BSTR *value_text);

	static void datatypeMap( )
	{		
		dataTypeMap.insert(make_pair( DATA_TYPE_KEYS::R  ,"R"  ));
		dataTypeMap.insert(make_pair( DATA_TYPE_KEYS::SR ,"SR" ));
		dataTypeMap.insert(make_pair( DATA_TYPE_KEYS::LR ,"LR" ));
		dataTypeMap.insert(make_pair( DATA_TYPE_KEYS::I  ,"I"  ));
		dataTypeMap.insert(make_pair( DATA_TYPE_KEYS::SI ,"SI" ));
		dataTypeMap.insert(make_pair( DATA_TYPE_KEYS::LI ,"LI" ));
		dataTypeMap.insert(make_pair( DATA_TYPE_KEYS::UI ,"UI" ));
		dataTypeMap.insert(make_pair( DATA_TYPE_KEYS::USI,"USI"));
		dataTypeMap.insert(make_pair( DATA_TYPE_KEYS::ULI,"ULI"));
		dataTypeMap.insert(make_pair( DATA_TYPE_KEYS::X  ,"X"  ));
		dataTypeMap.insert(make_pair( DATA_TYPE_KEYS::B  ,"B"  ));
		dataTypeMap.insert(make_pair( DATA_TYPE_KEYS::S  ,"S"  ));
		
	};

private:
	static int base64encoding(const BYTE* pByte, int nLen, LPTSTR pResult, int bufer_size);
	static int base64decoding( const BYTE* pStr, int nLen, BYTE* pResult, int bufer_size);

public:
	static void encodeArray(double value[], int num, LPTSTR base64out);
	static void encodeArray(float value[], int num, LPTSTR base64out);
	static void encodeArray(long double value[], int num, LPTSTR base64out);
	static void encodeArray(int value[], int num, LPTSTR base64out);
	static void encodeArray(short value[], int num, LPTSTR base64out);
	static void encodeArray(long long value[], int num, LPTSTR base64out);
	static void encodeArray(unsigned int value[], int num, LPTSTR base64out);
	static void encodeArray(unsigned short int value[], int num, LPTSTR base64out);
	static void encodeArray(unsigned long long value[], int num, LPTSTR base64out);
	static void encodeArray(unsigned char value[], int num, LPTSTR base64out);

	static void convTxtValue(int attTID, char *data, LPTSTR pstr);
	static void convBinValue(int attTID, LPTSTR val, char test_decode[BASE64_BUFFER_SIZE]);

};

#endif /**/
