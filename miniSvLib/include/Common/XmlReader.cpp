#include "XmlReader.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
//#include <boost/exception/all.hpp>

struct XmlReader::myData
{
	boost::property_tree::ptree tree;
};

XmlReader::XmlReader()
{
	m_pData = new myData;
}

XmlReader::~XmlReader()
{
	delete m_pData;
}

bool XmlReader::Open(const char* FileName)
{
	try
	{
		boost::property_tree::read_xml(FileName, m_pData->tree);
	}
	catch (...)
	{
		printf("XMLファイルオープンエラー:%s\n", FileName);
	}

	return true;
}

bool XmlReader::Read(const char* Path, int& Value)
{
	int val;
	bool ret = false;

	try 
	{
		val = m_pData->tree.get<int>(Path);
		Value = val;
		ret = true;
	}
	catch (...)
	{
		printf("XML読取エラー:%s\n", Path);
	}

	return ret;
}

bool XmlReader::Read(const char* Path, double& Value)
{
	double val;
	bool ret = false;

	try
	{
		val = m_pData->tree.get<double>(Path);
		Value = val;
		ret = true;
	}
	catch (...)
	{
		printf("XML読取エラー:%s\n", Path);
	}

	return ret;
}

//extern int ReadIntFromXml(const char* Path, const char* XmlNode);

extern "C" int ReadIntFromXml(const char* Path, const char* XmlNode)
{
	int ret = 0;

	XmlReader Reader;
	Reader.Open(Path);
	Reader.Read(XmlNode, ret);

	return ret;
}