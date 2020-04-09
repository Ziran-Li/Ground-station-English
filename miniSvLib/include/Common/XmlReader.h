#pragma once

class XmlReader
{
	struct myData;

	myData* m_pData;

public:
	XmlReader();
	virtual ~XmlReader();

	bool Open(const char* FileName);
	void Close();

	bool Read(const char* Path, int& Value);
	bool Read(const char* Path, double& Value);
};
