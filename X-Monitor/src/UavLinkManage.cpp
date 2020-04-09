#include "stdafx.h"
#include <sstream>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "UavLinkManage.h"
#include "UavLink.h"			// 无人机链接
#include "DataManage.h"			// XML数据管理
#include "StrXml.h"
#include "Common/Commondef_High.h"
#include "Common/Protocol_2GDef.h"
#include "Codec.h"

UavLinkManage::UavLinkManage()
{
	m_flagSendReady = false;
}

UavLinkManage::~UavLinkManage()
{
}

// 初始化
BOOL UavLinkManage::Initialize(CUavLink *wl, dataManage *dm)
{
	if (wl == NULL || dm == NULL){
		return FALSE;
	}

	m_mainLink = wl;
	m_xmlDataDb = dm;

	m_commonPath.push_back(L"message");
	m_commonPath.push_back(L"Data");
	m_commonPath.push_back(L"UAV");

	m_recvTime = 0;

	// for debug
	//m_mainLink->LogInitialize(TRUE, L".\\");

	return TRUE;
}


// 连接到MainLink
BOOL UavLinkManage::Connect(SCIPORT comPortNo)
{
	if (ISOK(m_mainLink->Initialize(comPortNo))) //串口初始化成功  设置串口号  波特率  校验位  停止位
	{
		// 成功打开

		// 注册每个数据包的大小（接收）
		m_mainLink->RxRegisterFormat(PACKETID_COMMAND, PACKETSIZE_COMMAND);
		m_mainLink->RxRegisterFormat(PACKETID_REPLY, PACKETSIZE_REPLY);
		m_mainLink->RxRegisterFormat(PACKETID_INPUTDATA, PACKETSIZE_INPUTDATA);
		m_mainLink->RxRegisterFormat(PACKETID_INPUTDATA_6R, PACKETSIZE_INPUTDATA_6R);
		m_mainLink->RxRegisterFormat(PACKETID_INPUTDATA_12R, PACKETSIZE_INPUTDATA_12R);
		m_mainLink->RxRegisterFormat(PACKETID_INPUTDATA_MIN, PACKETSIZE_INPUTDATA_MIN);
		m_mainLink->RxRegisterFormat(PACKETID_IMU_PS_DATA, PACKETSIZE_IMU_PS_DATA);
		m_mainLink->RxRegisterFormat(PACKETID_MAGDATA, PACKETSIZE_MAGDATA);
		m_mainLink->RxRegisterFormat(PACKETID_GPSDATA, PACKETSIZE_GPSDATA);
		m_mainLink->RxRegisterFormat(PACKETID_GPSINFO, PACKETSIZE_GPSINFO);
		m_mainLink->RxRegisterFormat(PACKETID_NAVDATA, PACKETSIZE_NAVDATA);
		m_mainLink->RxRegisterFormat(PACKETID_NAVDATA_EX, PACKETSIZE_NAVDATA_EX);
		m_mainLink->RxRegisterFormat(PACKETID_REFDATA, PACKETSIZE_REFDATA);
		m_mainLink->RxRegisterFormat(PACKETID_WAYPOINTDATA, PACKETSIZE_WAYPOINTDATA);
		m_mainLink->RxRegisterFormat(PACKETID_HOMEDATA, PACKETSIZE_HOMEDATA);
		m_mainLink->RxRegisterFormat(PACKETID_SYSTEMDATA, PACKETSIZE_SYSTEMDATA);
		m_mainLink->RxRegisterFormat(PACKETID_ERRORDATA, PACKETSIZE_ERRORDATA);
		m_mainLink->RxRegisterFormat(PACKETID_ACTIONPOINT, PACKETSIZE_ACTIONPOINT);
		m_mainLink->RxRegisterFormat(PACKETID_ACTIONPOINT_ACK, PACKETSIZE_ACTIONPOINT_ACK);
		m_mainLink->RxRegisterFormat(PACKETID_MULTIWAYPOINTDATA, PACKETSIZE_MULTIWAYPOINTDATA);
		m_mainLink->RxRegisterFormat(PACKETID_MULTIWAYPOINT_ACK, PACKETSIZE_MULTIWAYPOINT_ACK);
		m_mainLink->RxRegisterFormat(PACKETID_FIRMWARE_INFO, PACKETSIZE_FIRMWARE_INFO);
		m_mainLink->RxRegisterFormat(PACKETID_PROPODATA, PACKETSIZE_PROPODATA);
		m_mainLink->RxRegisterFormat(PACKETID_EEPROMDATA, PACKETSIZE_EEPROMDATA);
		m_mainLink->RxRegisterFormat(PACKETID_EEPROM_COMMAND, PACKETSIZE_EEPROM_COMMAND);
		m_mainLink->RxRegisterFormat(PACKETID_DEBUGDATA_H, PACKETSIZE_DEBUGDATA_H);
		m_mainLink->RxRegisterFormat(PACKETID_DEBUGDATA_L, PACKETSIZE_DEBUGDATA_L);
		m_mainLink->RxRegisterFormat(PACKETID_WAYPOINTDATA2, PACKETSIZE_WAYPOINTDATA2);
		m_mainLink->RxRegisterFormat(PACKETID_GENELALPURPOSEDATA, PACKETSIZE_GENELALPURPOSEDATA);

		//创建一个命令缓冲区（用于传输）
		std::vector<BOOL> continuousSend(CMD_BUF_NUM, FALSE);
		continuousSend[CMD_BUF_GO_HOME]		= TRUE;	// //按钮长度，连续地发送一个命令（以下相同）之间
		continuousSend[CMD_BUF_LAND]		= TRUE;
		continuousSend[CMD_BUF_PAUSE]		= TRUE;
		continuousSend[CMD_BUF_RESUME]		= TRUE;
		continuousSend[CMD_BUF_WAYPOINT]	= TRUE;
		m_mainLink->MakeCmdBuf(continuousSend, true, 0);

		//获取多路命令的重试间隔
		//（往返时间+ 40毫秒（最多接收延迟的最大差值⇒无人机侧传动））
		m_multiwayInfo.retryInterval = m_mainLink->GetRoundTripTime(PACKETSIZE_MULTIWAYPOINTDATA, 
																	PACKETSIZE_MULTIWAYPOINT_ACK) + 40;

		return TRUE;
	}
	else {
		// 失败
		return FALSE;
	}
}

// 已接收数据和解码要在XML数据管理单元登记到
//将接收到的数据
int UavLinkManage::DecodeAndRegister()
{
	PACKETID PacketID; // 接收数据包的ID
	int packetCount = 0;	// //收到的数据包数量（包ID的数量可以被识别）

	while (ISOK(m_mainLink->RxBegin(&PacketID))) {
		//接收数据包
		switch (PacketID) {
		case PACKETID_COMMAND:
			DecodeCommand();
			break;

		case PACKETID_REPLY:
			DecodeReply();
			break;

		case PACKETID_INPUTDATA:
			DecodeInputData();
			break;

		case PACKETID_INPUTDATA_6R:
			//DecodeInputData_6R();
			DecodeInputData_12R();
			break;

		case PACKETID_INPUTDATA_12R:
			DecodeInputData_12R();
			break;

		case PACKETID_INPUTDATA_MIN:
			DecodeInputData_Min();
			break;

		case PACKETID_IMU_PS_DATA:
			DecodeImuPsData();
			break;

		case PACKETID_MAGDATA:
			DecodeMagData();
			break;

		case PACKETID_GPSDATA:
			DecodeGpsData();
			break;

		case PACKETID_GPSINFO:
			DecodeGpsInfo();
			break;

		case PACKETID_NAVDATA:
			DecodeNavData();
			break;

		case PACKETID_NAVDATA_EX:
			DecodeNavDataEx();
			break;

		case PACKETID_REFDATA:
			DecodeRefData();
			break;

		case PACKETID_WAYPOINTDATA:
			DecodeWayPointData();
			break;

		case PACKETID_HOMEDATA:
			DecodeHomeData();
			break;

		case PACKETID_SYSTEMDATA:
			DecodeSystemData();
			break;

		case PACKETID_ERRORDATA:
			DecodeErrorData();
			break;

		case PACKETID_ACTIONPOINT:
			DecodeActionPoint();
			break;

		case PACKETID_ACTIONPOINT_ACK:
			DecodeActionPointAck();
			break;

		case PACKETID_MULTIWAYPOINTDATA:
			DecodeMultiWayPointData();
			break;

		case PACKETID_MULTIWAYPOINT_ACK:
			DecodeMultiWayPointAck();
			break;

		case PACKETID_FIRMWARE_INFO:
			DecodeFirmwareInfo();
			break;

		case PACKETID_PROPODATA:
			DecodePropoData();
			break;

		case PACKETID_EEPROMDATA:
			DecodeEepromData();
			break;

		case PACKETID_EEPROM_COMMAND:
			DecodeEepromCommand();
			break;

		case PACKETID_DEBUGDATA_H:
			DecodeDebugData_H();
			break;

		case PACKETID_DEBUGDATA_L:
			DecodeDebugData_L();
			break;

		//case PACKETID_SERIALNUMBER:
		//	DecodeSerialNumber();
		//	break;
		case PACKETID_WAYPOINTDATA2:
			DecodeWayPointData2();
			break;

		case PACKETID_GENELALPURPOSEDATA:
			DecodeGeneralPurposeData();
			break;

		default:
			break;
		}

		++packetCount;

		_UBYTE rssi;
		m_mainLink->RxEnd(&rssi);

		RegisterRssi(rssi);	// 信号强度为XML数据管理单元
	}

	return packetCount;
}

//*******************************************************************************
//	要注册的XML数据管理单元的本地时间
//*******************************************************************************
void UavLinkManage::RegisterLocalTime()
{
	// 获取本地时间
	SYSTEMTIME st;
	GetLocalTime(&st);

	// /注册的XML数据管理单元（错误检查省略
	vector<_bstr_t> path;
	HRESULT hr;
	ELEMATTRIBUTES elem_attr = ::setAttributeACI(NULL, NULL, NULL); // 元素的属性
	MSGATTRIBUTES msg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	//消息属性

	path.push_back(L"message");
	path.push_back(L"Data");
	path.push_back(L"GCS");
	path.push_back(L"SystemTime");

	path.push_back(L"wYear");
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, 0, st.wYear );

	path[4] = L"wMonth";
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, 0, st.wMonth );

	path[4] = L"wDay";
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, 0, st.wDay );

	path[4] = L"wHour";
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, 0, st.wHour );

	path[4] = L"wMinute";
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, 0, st.wMinute );

	path[4] = L"wSecond";
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, 0, st.wSecond );

	path[4] = L"wMilliseconds";
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, 0, st.wMilliseconds );
}

//*******************************************************************************
//注册// MultiWayPointData的XML数据管理单元
//*******************************************************************************
//push_back在作用是在容器里添加相应的元素。
//添加相应的元素名，
void UavLinkManage::RegisterMultiWayPointData(bool flag, BSTR value)
{
	// 注册的XML数据管理单元（错误检查省略）
	vector<_bstr_t> path;
	HRESULT hr;//如果遇到了异常情况，则COM系统经过判断，会返回相应的错误值。
	ELEMATTRIBUTES elem_attr = ::setAttributeACI(NULL, NULL, NULL); // 元素的属性
	MSGATTRIBUTES msg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性
	path.push_back(L"message");
	path.push_back(L"Data");
	path.push_back(L"GCS");
	path.push_back(L"WPP");
	if (flag) hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(L"O", NULL, NULL), msg_attr, L"");		// Open
	else hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(L"C", NULL, NULL), msg_attr, L"");				// Close


	XmlMessage *xml;
	xml = new XmlMessage(value, &hr);//加载xml中的字符串

	BSTR bstrNodeValueParent = NULL;
	IXMLDOMNodePtr pNodeChild = NULL;
	pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/map");
	pNodeChild->get_text(&bstrNodeValueParent); //调用此方法获取使用控件显示的文本。bstrNodeValueParent使用控件显示的文本
	path.push_back(L"map");
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, bstrNodeValueParent);//此处判断数据是否正常加载

	pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/LatOrigin"); 
	pNodeChild->get_text(&bstrNodeValueParent);
	path[4] = L"LatOrigin";
	hr = m_xmlDataDb->setOneElement(path, elem_attr, msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

	path[4] = L"LonOrigin";
	pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/LonOrigin");
	pNodeChild->get_text(&bstrNodeValueParent);
	hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr, 0,wcstod(bstrNodeValueParent, NULL));

	path[4] = L"mission";
	//pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/mission");
	//hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr, L"");

	path.push_back(L"ID");
	pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/mission/ID");
	pNodeChild->get_text(&bstrNodeValueParent);
	hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr, 0, _wtoi(bstrNodeValueParent));

	path[5] = L"NS_Transition";
	pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/mission/NS_Transition");
	pNodeChild->get_text(&bstrNodeValueParent);
	hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

	path[5] = L"EW_Transition";
	pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/mission/EW_Transition");
	pNodeChild->get_text(&bstrNodeValueParent);
	hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

	path[5] = L"V_Transition";
	pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/mission/V_Transition");
	pNodeChild->get_text(&bstrNodeValueParent);
	hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));


	path[5] = L"ScaleX";
	pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/mission/ScaleX");
	pNodeChild->get_text(&bstrNodeValueParent);
	hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

	path[5] = L"ScaleY";
	pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/mission/ScaleY");
	pNodeChild->get_text(&bstrNodeValueParent);
	hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

	path[5] = L"ScaleZ";
	pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/mission/ScaleZ");
	pNodeChild->get_text(&bstrNodeValueParent);
	hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

	path[5] = L"Rotation";
	pNodeChild = xml->pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/mission/Rotation");
	pNodeChild->get_text(&bstrNodeValueParent);
	hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr,  0,wcstod(bstrNodeValueParent, NULL));

	path[5] = L"waypoint";
	
	HRESULT hResult;
	IXMLDOMNodeListPtr pNodeList = NULL;
	pNodeList = xml->pDoc->selectNodes(L"root/message/Data/GCS/WPP/root/mission/waypoint");

	//hResult = pNodeChild->get_childNodes(&pNodeList);
	if (NULL == pNodeList)
	{
		return;
	}
	long lCountNode = 0;
	hResult = pNodeList->get_length(&lCountNode);
	if (FAILED(hResult))
	{
		return;
	}

	path.push_back(L"");
	for (int i = 0; i < lCountNode; ++i)
	{
		/*char outchar[255];

		sprintf_s(outchar, BASE64_BUFFER_SIZE, "%d", i);
*/
		BSTR _i = NULL;

		//char* str = new char[30];
		//sprintf_s(str, 30, "%d", i);
		//_i = _bstr_t(str);

		//LPSTR lstr = outchar;

		////BSTR偵昁梫側僶僢僼傽僒僀僘傪媮傔傞
		//int bstrlen = (int)MultiByteToWideChar(
		//	CP_ACP,			// 僐乕僪儁乕僕 ANSI 僐乕僪儁乕僕
		//	0,			// 暥帤偺庬椶傪巜掕偡傞僼儔僌
		//	lstr,			// 儅僢僾尦暥帤楍偺傾僪儗僗
		//	strlen(lstr),		// 儅僢僾尦暥帤楍偺僶僀僩悢
		//	NULL,			// 儅僢僾愭儚僀僪暥帤楍傪擖傟傞僶僢僼傽偺傾僪儗僗
		//	0			// 僶僢僼傽偺僒僀僘
		//	);

		////僶僢僼傽傪妋曐偡傞
		//_i = SysAllocStringLen(NULL, bstrlen);

		////BSTR偵暋惢
		//MultiByteToWideChar(
		//	CP_ACP,
		//	0,
		//	lstr,
		//	strlen(lstr),
		//	_i,
		//	bstrlen
		//	);
		////_stprintf(_i, _T("%d"), i);

		IXMLDOMNodePtr pXMLDOMNode = NULL;
		IXMLDOMNode *pNode = NULL;
		hResult = pNodeList->get_item(i, &pXMLDOMNode);
		if (FAILED(hResult))
		{
			return;
		}
		
		path[6] = L"ID";
		pXMLDOMNode->selectSingleNode(path[6], &pNode);
		pNode->get_text(&bstrNodeValueParent);
		hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, _i), msg_attr, 0, _wtoi(bstrNodeValueParent));

		path[6] = L"x";
		pXMLDOMNode->selectSingleNode(path[6], &pNode);
		pNode->get_text(&bstrNodeValueParent);
		hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, _i), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

		path[6] = L"y";
		pXMLDOMNode->selectSingleNode(path[6], &pNode);
		pNode->get_text(&bstrNodeValueParent);
		hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, _i), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

		path[6] = L"height";
		pXMLDOMNode->selectSingleNode(path[6], &pNode);
		pNode->get_text(&bstrNodeValueParent);
		hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, _i), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

		path[6] = L"psi";
		pXMLDOMNode->selectSingleNode(path[6], &pNode);
		pNode->get_text(&bstrNodeValueParent);
		hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, _i), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

		path[6] = L"accuracy";
		pXMLDOMNode->selectSingleNode(path[6], &pNode);
		pNode->get_text(&bstrNodeValueParent);
		hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, _i), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

		path[6] = L"wait";
		pXMLDOMNode->selectSingleNode(path[6], &pNode);
		pNode->get_text(&bstrNodeValueParent);
		hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, _i), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

		path[6] = L"func_id";
		pXMLDOMNode->selectSingleNode(path[6], &pNode);
		pNode->get_text(&bstrNodeValueParent);
		hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

		path[6] = L"speed";
		pXMLDOMNode->selectSingleNode(path[6], &pNode);
		pNode->get_text(&bstrNodeValueParent);
		hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr, 0, wcstod(bstrNodeValueParent, NULL));

	}
}

void UavLinkManage::RegisterData(MSXML2::IXMLDOMDocument2Ptr& pDoc)
{
	vector<_bstr_t> path;
	HRESULT hr;
	ELEMATTRIBUTES elem_attr = ::setAttributeACI(NULL, NULL, NULL);  //元素属性
	MSGATTRIBUTES msg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	 //消息属性
	path.push_back(L"message");
	path.push_back(L"Data");
	path.push_back(L"GCS");
	path.push_back(L"WPP");
	hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(L"O", NULL, NULL), msg_attr, L"");
//	if (flag) hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(L"O", NULL, NULL), msg_attr, L"");		// Open
//	else hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(L"C", NULL, NULL), msg_attr, L"");				// Close

	BSTR bstrNodeValueParent = NULL;
	IXMLDOMNodePtr pNodeChild = NULL;
	
	pNodeChild = pDoc->selectSingleNode(L"root/message/Data/GCS/WPP/root/LatOrigin");
	pNodeChild->get_text(&bstrNodeValueParent);
	path[4] = L"LatOrigin";
	hr = m_xmlDataDb->setOneElement(path, ::setAttributeACI(NULL, NULL, NULL), msg_attr, bstrNodeValueParent);

}

//*******************************************************************************
//	数据传输（根据数据的内容，调用适当的传输功能）
//*******************************************************************************
BOOL UavLinkManage::SendData(UINT dbID, const _bstr_t &sendMsg)
{
	////确定什么命令
	vector<_bstr_t> path;	//搜索路径
	path.push_back(L"Data");
	path.push_back(L"UAV");

	vector<_bstr_t> nodeName;	// 子节点的名称

	StrXml *strXml = new StrXml(sendMsg);//加载xml中的字符串
	VARIANT_BOOL vbRet = strXml->getChileNodeName(path, &nodeName);
	BOOL bRet = FALSE;
	if(vbRet){	// 发送开始命令
		if(_tcscmp(nodeName[0], L"Wpt") == 0){
			bRet = SendWayPointData(dbID);				//航点
		}
		else if(_tcscmp(nodeName[0], L"MltWpt") == 0){
			bRet = SendMultiWayPointData(dbID);			 //多路命令
		}
		else if (_tcscmp(nodeName[0], L"Firm") == 0){
			bRet = SendCmdRequestFirmwareinfo(dbID);	//固件命令
		}
		else if(_tcscmp(nodeName[0], L"Home") == 0){
			bRet = SendCmdGoHome(dbID);					 // GO_HOME命令
		}
		else if(_tcscmp(nodeName[0], L"Land") == 0){
			bRet = SendCmdLand(dbID);					// LAND命令
		}
		else if(_tcscmp(nodeName[0], L"Pause") == 0){
			bRet = SendCmdPause(dbID);					//暂停命令
		}
		else if(_tcscmp(nodeName[0], L"Resume") == 0){
			bRet = SendCmdResume(dbID);					// 恢复命令
		}
		else if (_tcscmp(nodeName[0], L"SerialNum") == 0){
			bRet = SendCmdRequestSerialNumber(dbID);	//序列号命令
		}
		else if (_tcscmp(nodeName[0], L"MltWptE") == 0){
			bRet = SendCmdPlannedFlight(dbID);			//多路命令
		}
		else if (_tcscmp(nodeName[0], L"Error") == 0){
			bRet = SendError(dbID);			//多路错误命令
		}
	}
	delete strXml;
	
	return bRet;
}

//*******************************************************************************
//	庴怣偟偨"儅儖僠僂僃僀億僀儞僩僐儅儞僪偺ACK"偺拞恎傪庢傝弌偡
//*******************************************************************************
BOOL UavLinkManage::PopMultiwayAckData(MultiwayAck &ack)
{
	if(m_multiwayInfo.ack.empty()){
		return FALSE;
	}
	else{
		//基本元素1应答
		ack = m_multiwayInfo.ack.back();
		m_multiwayInfo.ack.pop_back();
	
		return TRUE;
	}
}

//*******************************************************************************
//	要确定是否重发多路点命令
//*******************************************************************************
BOOL UavLinkManage::DecideMultiwayCmdRetry()
{	
	if(!m_multiwayInfo.isTransferring || m_multiwayInfo.prevSendTime == 0) return FALSE;
	
	DWORD diff = ::timeGetTime() - m_multiwayInfo.prevSendTime;	//以前的命令的传输的运行时间[毫秒] 
	
	//它从以前的命令的发送，如果你不是一个特定的时间过去了，重试（重发）
	if(diff > m_multiwayInfo.retryInterval){
		return SendMultiWayPointData(m_multiwayInfo.dbID, TRUE);	// 发送COMMAND_SLEEP无需等待
	}

	return FALSE;
}

//*******************************************************************************
//	/解码（命令无人机/ GCS）
//*******************************************************************************
void UavLinkManage::DecodeCommand()
{
	_UBYTE ub[4];
	_UWORD uw;

	m_mainLink->RxPop(&uw);			// 命令ID
	m_mainLink->RxPop(&ub[0], 1);	// 参数1
	m_mainLink->RxPop(&ub[1], 1);	// 参数2
	m_mainLink->RxPop(&ub[2], 1);	// 参数3
	m_mainLink->RxPop(&ub[3], 1);	// 参数4

	// //命令ID处理响应
	switch(uw){

	case COMMAND_SLEEP:
		{
			 m_flagSendReady = true;                 // /发送准备标志设置
			BOOL res = ProcCmdSleep();              // /开始数据传输
			if (res)
			{
				m_flagSendReady = false;		// 数据传输标志
			}
		}
		break;

	case COMMAND_REQUEST_TIME:
		ProcCmdReqTime();
		break;

	default:
		break;
	}
}

//*******************************************************************************
//解码（回复COMMAND_PACKET）													
//*******************************************************************************
void UavLinkManage::DecodeReply()
{
	_UBYTE ub;
	_UWORD uw;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性

	m_mainLink->RxPop(&uw);
	m_mainLink->RxPop(&ub);
	m_mainLink->RxPop(&ub);
	m_mainLink->RxPop(&ub);
	m_mainLink->RxPop(&ub);
	// Command
	m_xmlDataDb->setOneElement(GetXmlPath("Command", "Response"),
		::setAttributeACI(NULL, NULL, NULL),
		setmsg_attr, 0, static_cast<COMMAND>(uw));
}

//*******************************************************************************
//	解码（控制​​系统的控制输入数据）
//*******************************************************************************
void UavLinkManage::DecodeInputData()
{
	_SWORD sw;
	_UWORD uw;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// //消息属性

	//更高级别的控制输入（油门，滚动，俯仰，偏航
	DecodeInputData_Min();

	// //控制输入（油门）
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Input"), 
								::setAttributeACI(L"L", NULL, L"0"),
								setmsg_attr, 4, (double)uw );

	// 下控制输入Roll
	m_mainLink->RxPop(&sw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Input"), 
								::setAttributeACI(L"L", NULL, L"1"),
								setmsg_attr, 4, (double)sw );

	// 下控制输入Pitch
	m_mainLink->RxPop(&sw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Input"), 
								::setAttributeACI(L"L", NULL, L"2"),
								setmsg_attr, 4, (double)sw );

	// 下控制输入Yaw
	m_mainLink->RxPop(&sw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Input"), 
								::setAttributeACI(L"L", NULL, L"3"),
								setmsg_attr, 4, (double)sw );
}

//*******************************************************************************
//解码（控制​​系统，其中输入数据被提供到电机的六个控制输入数据）
//*******************************************************************************
void UavLinkManage::DecodeInputData_6R()
{
	_UWORD uw;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性

	// 更高级别的控制输入（油门，滚动，俯仰，偏航）
	// 控制输入（油门，滚动，俯仰，偏航）
	DecodeInputData();

	// 输入值到电机1号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"0"),
								setmsg_attr, 6, uw );

	// 输入值到电机2号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"1"),
								setmsg_attr, 6, uw );

	// 输入值到电机3号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"2"),
								setmsg_attr, 6, uw );

	// 输入值到电机4号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"3"),
								setmsg_attr, 6, uw );

	// 输入值到电机5号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"4"),
								setmsg_attr, 6, uw );

	// 输入值到电机6号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"5"),
								setmsg_attr, 6, uw );
}

//*******************************************************************************
//	解码（其中输入数据被提供到转子的控制系统的12个的控制输入数据）
//*******************************************************************************
void UavLinkManage::DecodeInputData_12R()
{
	_UWORD uw;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	 //消息属性

	//更高级别的控制输入（油门，滚动，俯仰，偏航）
	//下控制输入（油门，滚动，俯仰，偏航）
	DecodeInputData();

	//输入值到电机1号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"0"),
								setmsg_attr, 12, uw );

	// 输入值到电机2号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"1"),
								setmsg_attr, 12, uw );

	// 输入值到电机3号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"2"),
								setmsg_attr, 12, uw );

	// 输入值到电机4号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"3"),
								setmsg_attr, 12, uw );

	// 输入值到电机5号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"4"),
								setmsg_attr, 12, uw );

	// 输入值到电机6号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"5"),
								setmsg_attr, 12, uw );

	// 输入值到电机7号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"6"),
								setmsg_attr, 12, uw );

	// 输入值到电机8号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"7"),
								setmsg_attr, 12, uw );

	// 输入值到电机9号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"8"),
								setmsg_attr, 12, uw );

	// 输入值到电机10号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"9"),
								setmsg_attr, 12, uw );

	//输入值到电机11号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"10"),
								setmsg_attr, 12, uw );

	// 输入值到电机12号
	m_mainLink->RxPop(&uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Rotor"), 
								::setAttributeACI(NULL, NULL, L"11"),
								setmsg_attr, 12, uw );
}

//*******************************************************************************
//	解码（最低控制系统的控制输入数据）
//*******************************************************************************
void UavLinkManage::DecodeInputData_Min()
{
	_UWORD uw;
	double val;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// //消息属性

	// 更高级别的控制输入（油门）
	m_mainLink->RxPop(&uw);
	val = MiniSvCodec::DecHighLevelInputData(uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Input"), 
								::setAttributeACI(L"H", NULL, L"0"),
								setmsg_attr, 4, val );

	// 更高级别的控制输入（Roll）
	m_mainLink->RxPop(&uw);
	val = MiniSvCodec::DecHighLevelInputData(uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Input"), 
								::setAttributeACI(L"H", NULL, L"1"),
								setmsg_attr, 4, val );

	//更高级别的控制输入（Pitch）
	m_mainLink->RxPop(&uw);
	val = MiniSvCodec::DecHighLevelInputData(uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Input"), 
								::setAttributeACI(L"H", NULL, L"2"),
								setmsg_attr, 4, val );

	//更高级别的控制输入（Yaw）
	m_mainLink->RxPop(&uw);
	val = MiniSvCodec::DecHighLevelInputData(uw);
	m_xmlDataDb->setOneElement(	GetXmlPath("Ctrl", "Input"), 
								::setAttributeACI(L"H", NULL, L"3"),
								setmsg_attr, 4, val );
}

//*******************************************************************************
//	解码（IMU传感器压力传感器的数据）
//*******************************************************************************
void UavLinkManage::DecodeImuPsData()
{
	_SWORD sw;
	_SDWORD sdw;
	double val;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	//消息属性

	// 角速度（X轴/Roll)
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo1000(sw);
	m_xmlDataDb->setOneElement(	GetXmlPath("IMU", "Gyro"), 
								::setAttributeACI(NULL, NULL, L"0"),
								setmsg_attr, 3, val );

	// 角速度（Y轴/Pitch）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo1000(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Gyro"),
								::setAttributeACI(NULL, NULL, L"1"),
								setmsg_attr, 3, val );

	// 角速度（Z轴/Yaw）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo1000(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Gyro"),
								::setAttributeACI(NULL, NULL, L"2"),
								setmsg_attr, 3, val );

	// 加速度（X轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Acc"),
								::setAttributeACI(NULL, NULL, L"0"),
								setmsg_attr, 3, val );

	//加速度（Y轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Acc"),
								::setAttributeACI(NULL, NULL, L"1"),
								setmsg_attr, 3, val );

	// 加速度（Z轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Acc"),
								::setAttributeACI(NULL, NULL, L"2"),
								setmsg_attr, 3, val );

	// 磁通密度的大小
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo1(sw)*100000;
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "MagNorm"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 姿态角Roll乯
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Att"),
								::setAttributeACI(NULL, NULL, L"0"),
								setmsg_attr, 3, val );

	// 姿态角Pitch乯
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Att"),
								::setAttributeACI(NULL, NULL, L"1"),
								setmsg_attr, 3, val );

	// 姿态角Yaw乯
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Att"),
								::setAttributeACI(NULL, NULL, L"2"),
								setmsg_attr, 3, val );

	//高度
	m_mainLink->RxPop(&sdw, 3);
	val = MiniSvCodec::Dec3ByteTo10000(sdw);
	m_xmlDataDb->setOneElement( GetXmlPath("PS", "Alt"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );
}

//*******************************************************************************
//	解码（磁场IMU的数据）
//*******************************************************************************
void UavLinkManage::DecodeMagData()
{
	_SWORD sw;
	double val;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	////消息属性

	// 地磁X轴
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo1(sw)*100000;
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Mag"),
								::setAttributeACI(NULL, NULL, L"0"),
								setmsg_attr, 3, val );

	// 地磁Y轴
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo1(sw)*100000;
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Mag"),
								::setAttributeACI(NULL, NULL, L"1"),
								setmsg_attr, 3, val );

	// 地磁Z轴
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo1(sw)*100000;
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Mag"),
								::setAttributeACI(NULL, NULL, L"2"),
								setmsg_attr, 3, val );
}

//*******************************************************************************
//	解码（GPS原始数据）
//*******************************************************************************
void UavLinkManage::DecodeGpsData()
{
	_SQWORD sqw;
	_SDWORD sdw;
	_SWORD sw;
	double val;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性

	// 纬度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement( GetXmlPath("GPS", "Lat"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 经度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement( GetXmlPath("GPS", "Lon"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 高度
	m_mainLink->RxPop(&sdw, 3);
	val = MiniSvCodec::Dec3ByteTo10000(sdw);
	m_xmlDataDb->setOneElement( GetXmlPath("GPS", "Alt"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 速度（北/ X轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("GPS", "Vel"),
								::setAttributeACI(NULL, L"r", L"0"),
								setmsg_attr, 3, val );

	// 速度（东/ Y轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("GPS", "Vel"),
								::setAttributeACI(NULL, L"r", L"1"),
								setmsg_attr, 3, val );

	//速度（下/ Z轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("GPS", "Vel"),
								::setAttributeACI(NULL, L"r", L"2"),
								setmsg_attr, 3, val );
}

//*******************************************************************************
//	解码（附加信息数据，GPS）
//*******************************************************************************
void UavLinkManage::DecodeGpsInfo()
{
	_UBYTE ub;
	_SWORD sw;
	double val;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	 //消息属性

	//卫星的补充数量
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("GPS", "Sats"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, ub );

	//水平定位精度
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("GPS", "hPosAcc"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	//垂直定位精度
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("GPS", "vPosAcc"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	//水平速度精度
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("GPS", "hVelAcc"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	//垂直速度精度
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("GPS", "vVelAcc"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );
}

//*******************************************************************************
//	//解码（导航计算数据）
//*******************************************************************************
void UavLinkManage::DecodeNavData()
{
	_SQWORD sqw;
	_SDWORD sdw;
	_SWORD sw;
	double val;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性

	// 纬度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement( GetXmlPath("Nav", "Lat"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 经度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement( GetXmlPath("Nav", "Lon"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 高度
	m_mainLink->RxPop(&sdw, 3);
	val = MiniSvCodec::Dec3ByteTo10000(sdw);
	m_xmlDataDb->setOneElement( GetXmlPath("Nav", "Alt"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 速度（北/ X轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Nav", "Vel"),
								::setAttributeACI(NULL, L"r", L"0"),
								setmsg_attr, 3, val );

	// 速度（东/ Y轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Nav", "Vel"),
								::setAttributeACI(NULL, L"r", L"1"),
								setmsg_attr, 3, val );

	// 速度（下/ Z轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Nav", "Vel"),
								::setAttributeACI(NULL, L"r", L"2"),
								setmsg_attr, 3, val );

	//姿态角Yaw
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Att"),
								::setAttributeACI(NULL, NULL, L"2"),
								setmsg_attr, 3, val );
}

//*******************************************************************************
//	解码（带导航计算数据和方向数据）
//*******************************************************************************
void UavLinkManage::DecodeNavDataEx()
{
	_SQWORD sqw;
	_SDWORD sdw;
	_SWORD sw;
	double val;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性

	// 纬度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement( GetXmlPath("Nav", "Lat"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 经度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement( GetXmlPath("Nav", "Lon"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 高度
	m_mainLink->RxPop(&sdw, 3);
	val = MiniSvCodec::Dec3ByteTo10000(sdw);
	m_xmlDataDb->setOneElement( GetXmlPath("Nav", "Alt"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 速度（北/ X轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Nav", "Vel"),
								::setAttributeACI(NULL, L"r", L"0"),
								setmsg_attr, 3, val );

	// 速度（东/ Y轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Nav", "Vel"),
								::setAttributeACI(NULL, L"r", L"1"),
								setmsg_attr, 3, val );

	// 速度（下/ Z轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Nav", "Vel"),
								::setAttributeACI(NULL, L"r", L"2"),
								setmsg_attr, 3, val );

	// 姿态角Roll
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Att"),
								::setAttributeACI(NULL, NULL, L"0"),
								setmsg_attr, 3, val );

	// 姿态角Pitch
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Att"),
								::setAttributeACI(NULL, NULL, L"1"),
								setmsg_attr, 3, val );

	// 姿态角Yaw
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("IMU", "Att"),
								::setAttributeACI(NULL, NULL, L"2"),
								setmsg_attr, 3, val );
}

//*******************************************************************************
//	解码（控制​​目标值的数据）
//*******************************************************************************
void UavLinkManage::DecodeRefData()
{
	_SWORD sw;
	_SDWORD sdw;
	double val;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性

	// 方向目标值（X轴/Roll）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Ref", "Att"),
								::setAttributeACI(NULL, NULL, L"0"),
								setmsg_attr, 3, val );

	// 方向目标值（Y轴/Pitch）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Ref", "Att"),
								::setAttributeACI(NULL, NULL, L"1"),
								setmsg_attr, 3, val );

	// 方向目标值（Z轴/Yaw）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Ref", "Att"),
								::setAttributeACI(NULL, NULL, L"2"),
								setmsg_attr, 3, val );

	//速度目标值（X轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Ref", "Vel"),
								::setAttributeACI(NULL, L"n", L"0"),
								setmsg_attr, 3, val );

	// 速度目标值（Y轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Ref", "Vel"),
								::setAttributeACI(NULL, L"n", L"1"),
								setmsg_attr, 3, val );

	// 速度目标值（Z轴）
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Ref", "Vel"),
								::setAttributeACI(NULL, L"n", L"2"),
								setmsg_attr, 3, val );

	// 高度目标值
	m_mainLink->RxPop(&sdw, 3);
	val = MiniSvCodec::Dec3ByteTo10000(sdw);
	m_xmlDataDb->setOneElement( GetXmlPath("Ref", "Alt"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );
}

//*******************************************************************************
//解码（航点的数据和配置命令）
//*******************************************************************************
void UavLinkManage::DecodeWayPointData()
{
	_SQWORD sqw;
	_SDWORD sdw;
	_SWORD sw;
	_UBYTE ub;
	double val;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	//消息属性

	// 纬度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement( GetXmlPath("Wpt", "Lat"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 经度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement( GetXmlPath("Wpt", "Lon"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 高度
	m_mainLink->RxPop(&sdw, 3);
	val = MiniSvCodec::Dec3ByteTo10000(sdw);
	m_xmlDataDb->setOneElement( GetXmlPath("Wpt", "Alt"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 方向
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Wpt", "Yaw"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 飞行速度
	m_mainLink->RxPop(&ub);
	val = MiniSvCodec::DecAirSpeed(ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Wpt", "Spd"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );
}
//*******************************************************************************
//解码（与航点数量的数据和配置命令）
//*******************************************************************************
void UavLinkManage::DecodeWayPointData2()
{
	_SQWORD sqw;
	_SDWORD sdw;
	_SWORD sw;
	_UBYTE ub;
	double val;
	_UWORD no;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	//消息属性

	// 纬度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement(GetXmlPath("Wpt", "Lat"),
		::setAttributeACI(NULL, NULL, NULL),
		setmsg_attr, 0, val);

	// 经度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement(GetXmlPath("Wpt", "Lon"),
		::setAttributeACI(NULL, NULL, NULL),
		setmsg_attr, 0, val);

	// 高度
	m_mainLink->RxPop(&sdw, 3);
	val = MiniSvCodec::Dec3ByteTo10000(sdw);
	m_xmlDataDb->setOneElement(GetXmlPath("Wpt", "Alt"),
		::setAttributeACI(NULL, NULL, NULL),
		setmsg_attr, 0, val);

	// 方向
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement(GetXmlPath("Wpt", "Yaw"),
		::setAttributeACI(NULL, NULL, NULL),
		setmsg_attr, 0, val);

	// 飞行速度
	m_mainLink->RxPop(&ub);
	val = MiniSvCodec::DecAirSpeed(ub);
	m_xmlDataDb->setOneElement(GetXmlPath("Wpt", "Spd"),
		::setAttributeACI(NULL, NULL, NULL),
		setmsg_attr, 0, val);

	// 航点数量
	m_mainLink->RxPop(&no);
	m_xmlDataDb->setOneElement(GetXmlPath("Wpt", "No"),
		::setAttributeACI(NULL, NULL, NULL),
		setmsg_attr, 0, no);
}

//*******************************************************************************
//	解码（反馈点数据）
//*******************************************************************************
void UavLinkManage::DecodeHomeData()
{
	_SQWORD sqw;
	_SDWORD sdw;
	_SWORD sw;
	double val;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 儊僢僙乕僕偺懏惈

	//纬度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement( GetXmlPath("Home", "Lat"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 经度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement( GetXmlPath("Home", "Lon"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 高度
	m_mainLink->RxPop(&sdw, 3);
	val = MiniSvCodec::Dec3ByteTo10000(sdw);
	m_xmlDataDb->setOneElement( GetXmlPath("Home", "Alt"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 方向
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Home", "Yaw"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );
}

//*******************************************************************************
//	解码（数据指示系统的内部状态） 系统的数据
//*******************************************************************************
void UavLinkManage::DecodeSystemData()
{
	_UDWORD udw;
	_SWORD sw;
	_UBYTE ub;
	double val;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性

	// 启动模式和驱动模式
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Sys", "Mode", "Boot"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)((ub&0xF8)>>3) );		// 高5位
	m_xmlDataDb->setOneElement( GetXmlPath("Sys", "Mode", "Drv"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)(ub&0x07) );			// 低3位

	// 控制模式，自动模式
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Sys", "Mode", "Ctrl"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)((ub&0xF0)>>4) );		// 高4位
	m_xmlDataDb->setOneElement( GetXmlPath("Sys", "Mode", "Auto"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)(ub&0x0F) );			// 低4位

	// 内部标志（下）
	m_mainLink->RxPop(&udw);
	m_xmlDataDb->setOneElement( GetXmlPath("Sys", "Flag"),
								::setAttributeACI(L"L", NULL, NULL),
								setmsg_attr, 0, (unsigned int)udw );

	// 内部标志（下）
	m_mainLink->RxPop(&udw);
	m_xmlDataDb->setOneElement( GetXmlPath("Sys", "Flag"),
								::setAttributeACI(L"H", NULL, NULL),
								setmsg_attr, 0, (unsigned int)udw );

	//CPU利用率（低级）
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Sys", "CPU"),
								::setAttributeACI(L"L", NULL, NULL),
								setmsg_attr, 0, ub );

	//CPU占用率（较高的）
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Sys", "CPU"),
								::setAttributeACI(L"H", NULL, NULL),
								setmsg_attr, 0, ub );

	// 平均供电电压
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteTo100(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Voltage"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 遥控接收器-MCU通信稳健·遥控接收器接收稳健
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "ReceiverComm"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)((ub&0xF0)>>4) );		// 高4位
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "ReceiverRecep"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)(ub&0x0F) );			// 低4位

	// 陀螺仪传感器-MCU通信稳健陀螺仪传感器测量稳健
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "GyroComm"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)((ub&0xF0)>>4) );		// 高4位
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "GyroMeas"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)(ub&0x0F) );			// 低4位

	// MCU-MCU通信稳健（下机匣）·MCU-MCU通信稳健性（上接收）
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "McuComm"),
								::setAttributeACI(L"L", NULL, NULL),
								setmsg_attr, 0, (_UBYTE)((ub&0xF0)>>4) );		// 高4位
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "McuComm"),
								::setAttributeACI(L"H", NULL, NULL),
								setmsg_attr, 0, (_UBYTE)(ub&0x0F) );			// 低4位

	//  IMU通信稳健·IMU陀螺测量稳健
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "ImuComm"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)((ub&0xF0)>>4) );		// 高4位
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "ImuGyroMeas"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)(ub&0x0F) );			// 低4位

	//  IMU加速度测量稳健·IMU磁测稳健
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "ImuAccMeas"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)((ub&0xF0)>>4) );		// 高4位
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "ImuMagMeas"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)(ub&0x0F) );			// 低4位

	// 压力传感器-MCU通信压力传感器测量稳健性
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "PsComm"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)((ub&0xF0)>>4) );		// 高4位
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "PsMeas"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)(ub&0x0F) );			// 低4位

	// GPS-MCU通信健全·GPS测量稳健
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "GpsComm"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)((ub&0xF0)>>4) );		// 高4位
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "GpsMeas"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)(ub&0x0F) );			// 低4位

	// 数据链路通信稳健（UAV接收）
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "Datalink"),
								::setAttributeACI(L"U", NULL, NULL),
								setmsg_attr, 0, (_UBYTE)((ub&0xF0)>>4) );		// 高4位

	// 数据链路通信的接收强度（UAV接收）
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("RSSI", "Datalink"),
								::setAttributeACI(L"U", NULL, NULL),
								setmsg_attr, 0, (_SWORD)(-ub) );
}

//*******************************************************************************
//	解码（无人机生成的错误数据）
//*******************************************************************************
void UavLinkManage::DecodeErrorData()
{
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性


	wchar_t index[2][3] = { L"0", L"1"};
	// 0-15个
	for (int i = 0; i<2; ++i){
		_UBYTE err[5] = { 0 };
		m_mainLink->RxPop(&err);

		// 错误的类ID
		m_xmlDataDb->setOneElement(GetXmlPath("Error", "ID"), ::setAttributeACI(NULL, NULL, index[i]), setmsg_attr, 2, (_UBYTE)err[4]);

		// 错误水平
		m_xmlDataDb->setOneElement(GetXmlPath("Error", "Level"), ::setAttributeACI(NULL, NULL, index[i]), setmsg_attr, 2, static_cast<_UBYTE>((err[3] & 0xC0) >> 6));

		// 时间信息
		m_xmlDataDb->setOneElement(GetXmlPath("Error", "Ticks"), ::setAttributeACI(NULL, NULL, index[i]), setmsg_attr, 2,
			(_UWORD)((0x0FC0 & ((_UWORD)err[3] << 6)) | ((0x00FC & (_UWORD)err[2]) >> 2)));

		// 错误类型
		m_xmlDataDb->setOneElement(GetXmlPath("Error", "Category"), ::setAttributeACI(NULL, NULL, index[i]), setmsg_attr, 2,
			(static_cast<_UWORD>(err[2] & 0x03) << 8) | static_cast<_UWORD>(err[1]));

		// 错误的具体参数
		m_xmlDataDb->setOneElement(GetXmlPath("Error", "Param"), ::setAttributeACI(NULL, NULL, index[i]), setmsg_attr, 2, (_UWORD)err[0]);
	}


}

//*******************************************************************************
//	解码（有关执行的动作点数据）
//*******************************************************************************
void UavLinkManage::DecodeActionPoint()
{
	_SQWORD sqw;
	_SDWORD sdw;
	_UDWORD udw;
	_SWORD sw;
	_UWORD uwCount;
	_UBYTE ubActionType;
	double val;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性

	//操作类型
	m_mainLink->RxPop(&ubActionType);
	BSTR bstrActionType = A2BSTR(GetActionString(ubActionType).c_str());

	// 计数
	m_mainLink->RxPop(&uwCount);
	m_xmlDataDb->setOneElement( GetXmlPath("Action", bstrActionType, "Count"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, uwCount );

	// 纬度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement( GetXmlPath("Action", bstrActionType, "Lat"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 经度
	m_mainLink->RxPop(&sqw, 5);
	val = MiniSvCodec::Dec5ByteToDeg(sqw);
	m_xmlDataDb->setOneElement( GetXmlPath("Action", bstrActionType, "Lon"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 高度
	m_mainLink->RxPop(&sdw, 3);
	val = MiniSvCodec::Dec3ByteTo10000(sdw);
	m_xmlDataDb->setOneElement( GetXmlPath("Action", bstrActionType, "Alt"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, val );

	// 姿态角Roll
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Action", bstrActionType, "Att"),
								::setAttributeACI(NULL, NULL, L"0"),
								setmsg_attr, 3, val );

	// 姿态角Pitch
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Action", bstrActionType, "Att"),
								::setAttributeACI(NULL, NULL, L"1"),
								setmsg_attr, 3, val );

	// 姿态角Yaw
	m_mainLink->RxPop(&sw);
	val = MiniSvCodec::Dec2ByteToDeg(sw);
	m_xmlDataDb->setOneElement( GetXmlPath("Action", bstrActionType, "Att"),
								::setAttributeACI(NULL, NULL, L"2"),
								setmsg_attr, 3, val );

	// GPS周期·GPS周秒
	m_mainLink->RxPop(&udw);
	_UWORD gpsWeek;
	float gpsWeekSec;
	MiniSvCodec::Dec4ByteToGpsTime(udw, &gpsWeek, &gpsWeekSec);
	m_xmlDataDb->setOneElement( GetXmlPath("Action", bstrActionType, "GpsWeek"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, gpsWeek );
	m_xmlDataDb->setOneElement( GetXmlPath("Action", bstrActionType, "GpsWeekSec"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (double)gpsWeekSec );

	::SysFreeString(bstrActionType);

	// ACK返回
	SendActionPointAck(ubActionType, uwCount);
}

//*******************************************************************************
//解码（[未在基站接收的S / W]左右的动作进行点数据的ACK）
//*******************************************************************************
void UavLinkManage::DecodeActionPointAck()
{
	// 没有处理
}

//*******************************************************************************
//	解码（多路点传送数据[在基站S / W被未收到]）
//*******************************************************************************
void UavLinkManage::DecodeMultiWayPointData()
{
	// 没有处理
}

//*******************************************************************************
//	解码（响应数据给多路传输点）
//*******************************************************************************
void UavLinkManage::DecodeMultiWayPointAck()
{
	_SQWORD sqw;
	_SDWORD sdw;
	_SWORD sw;
	MultiwayAck ack;

	// 航点的ID
	m_mainLink->RxPop(&ack.wptId);	
	
	//纬度
	m_mainLink->RxPop(&sqw, 5);
	ack.lat = MiniSvCodec::Dec5ByteToDeg(sqw);
	
	// 经度
	m_mainLink->RxPop(&sqw, 5);
	ack.lon = MiniSvCodec::Dec5ByteToDeg(sqw);

	// 高度
	m_mainLink->RxPop(&sdw, 3);
	ack.alt = MiniSvCodec::Dec3ByteTo10000(sdw);

	// 方向
	m_mainLink->RxPop(&sw);
	ack.yaw = MiniSvCodec::Dec2ByteToDeg(sw);

	// 飞行速度
	m_mainLink->RxPop(&sw);
	ack.spd = MiniSvCodec::Dec2ByteTo100(sw);
	
	// 到达确定半径
	m_mainLink->RxPop(&sw);
	ack.tol = MiniSvCodec::Dec2ByteTo100(sw);
	
	// 等待时间
	m_mainLink->RxPop(&sdw, 3);
	ack.wait = MiniSvCodec::Dec3ByteTo10000(sdw);
	
	// 功能ID
	m_mainLink->RxPop(&ack.funcId);	

	// 航点ID”登记等
	if(ack.wptId == 0){
		m_multiwayInfo.nextWptId = ack.funcId;	// 接收的随机值
		m_multiwayInfo.randVal = ack.funcId;
	}
	else{
		//“接收航点编号”+ 1
		//（然而，它被设置为“1”，如果你达到为0x80000000（= 2 ^ 31）
		m_multiwayInfo.nextWptId = (ack.wptId + 1) == 0x80000000 ? 1 : ack.wptId + 1;	
	}

	// 如果传输完成，更新传输状态//
	if(ack.wptId == 0xFFFFFFFF){
		m_multiwayInfo.isTransferring = FALSE;
	}

	//复位命令传输时间（做不进行重发处理，直到下一个命令发送）
	m_multiwayInfo.prevSendTime = 0;	

	// 僋儔僀傾儞僩偵憲傞ACK僨乕僞梡偵僂僃僀億僀儞僩ID傪曇廤
	if(ack.wptId != 0 && ack.wptId != 0xFFFFFFFF){
		if(ack.wptId < m_multiwayInfo.randVal){	// 2^31偵払偟偰1偵栠偭偨応崌
			ack.wptId = 0x80000000 - m_multiwayInfo.randVal + ack.wptId;
		}
		else{
			ack.wptId = ack.wptId - m_multiwayInfo.randVal + 1;
		}
	}

	// 注册该ACK数据被发送到客户端
	m_multiwayInfo.ack.push_back(ack);
}

//*******************************************************************************
//	解码（数据包用于通知固件信息
//*******************************************************************************
void UavLinkManage::DecodeFirmwareInfo()
{
	_UWORD uw;
	_UBYTE ub[8];
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// //消息属性

	//主号，次号，版本号（下控制MCU）
	m_mainLink->RxPop(&ub[0]);
	m_mainLink->RxPop(&ub[1]);
	m_mainLink->RxPop(&ub[2]);
	wchar_t version[16];	// "XXX.XXX.XXX"格式
	wsprintf(version, L"%03d.%03d.%03d", ub[0], ub[1], ub[2]);
	m_xmlDataDb->setOneElement( GetXmlPath("Firm", "Ver"),
								::setAttributeACI(L"L", NULL, NULL),
								setmsg_attr, version );

	// 生成日期（下控制MCU）
	m_mainLink->RxPop(&uw);
	_UBYTE year, month, day;
	MiniSvCodec::Dec2ByteToDate(uw, &year, &month, &day);
	wchar_t buildDate[16];	// "YYYY.MM.DD"格式
	wsprintf(buildDate, L"20%02d.%02d.%02d", year, month, day);
	m_xmlDataDb->setOneElement( GetXmlPath("Firm", "BuildDate"),
								::setAttributeACI(L"L", NULL, NULL),
								setmsg_attr, buildDate );

	//主号，次号，版本号（上控制MCU）
	m_mainLink->RxPop(&ub[0]);
	m_mainLink->RxPop(&ub[1]);
	m_mainLink->RxPop(&ub[2]);
	m_mainLink->RxPop(&ub[3]);
	ub[4] = '\0';
	BSTR bstrEditionL = A2BSTR((LPCSTR)ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Firm", "Edition"),
								::setAttributeACI(L"L", NULL, NULL),
								setmsg_attr, (LPCTSTR)bstrEditionL );
	::SysFreeString(bstrEditionL);

	//生成日期（上控制MCU）
	m_mainLink->RxPop(&ub[0]);
	m_mainLink->RxPop(&ub[1]);
	m_mainLink->RxPop(&ub[2]);
	wsprintf(version, L"%03d.%03d.%03d", ub[0], ub[1], ub[2]);
	m_xmlDataDb->setOneElement( GetXmlPath("Firm", "Ver"),
								::setAttributeACI(L"H", NULL, NULL),
								setmsg_attr, version );

	// 生成日期（上控制MCU）
	m_mainLink->RxPop(&uw);
	MiniSvCodec::Dec2ByteToDate(uw, &year, &month, &day);
	wsprintf(buildDate, L"20%02d.%02d.%02d", year, month, day);
	m_xmlDataDb->setOneElement( GetXmlPath("Firm", "BuildDate"),
								::setAttributeACI(L"H", NULL, NULL),
								setmsg_attr, buildDate );

	// Edition代码（上控制MCU）
	m_mainLink->RxPop(&ub[0]);
	m_mainLink->RxPop(&ub[1]);
	m_mainLink->RxPop(&ub[2]);
	m_mainLink->RxPop(&ub[3]);
	ub[4] = '\0';
	BSTR bstrEditionH = A2BSTR((LPCSTR)ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Firm", "Edition"),
								::setAttributeACI(L"H", NULL, NULL),
								setmsg_attr, (LPCTSTR)bstrEditionH );
	::SysFreeString(bstrEditionH);

}

//*******************************************************************************
//解码（PROPO数据）
//*******************************************************************************
void UavLinkManage::DecodePropoData()
{
	_UQWORD uqw;
	_UBYTE ub;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	//消息属性

	// //通道1〜4
	m_mainLink->RxPop(&uqw, 5);
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"0"),
								setmsg_attr, 16, (_UWORD)(((uqw>>30)&0x03FF)+1020));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"1"),
								setmsg_attr, 16, (_UWORD)(((uqw>>20)&0x03FF)+1020));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"2"),
								setmsg_attr, 16, (_UWORD)(((uqw>>10)&0x03FF)+1020));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"3"),
								setmsg_attr, 16, (_UWORD)((uqw&0x03FF)+1020));

	// 通道5~8
	m_mainLink->RxPop(&uqw, 5);
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"4"),
								setmsg_attr, 16, (_UWORD)(((uqw>>30)&0x03FF)+1020));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"5"),
								setmsg_attr, 16, (_UWORD)(((uqw>>20)&0x03FF)+1020));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"6"),
								setmsg_attr, 16, (_UWORD)(((uqw>>10)&0x03FF)+1020));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"7"),
								setmsg_attr, 16, (_UWORD)((uqw&0x03FF)+1020));

	// Ch9~12
	m_mainLink->RxPop(&uqw, 5);
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"8"),
								setmsg_attr, 16, (_UWORD)(((uqw>>30)&0x03FF)+1020));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"9"),
								setmsg_attr, 16, (_UWORD)(((uqw>>20)&0x03FF)+1020));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"10"),
								setmsg_attr, 16, (_UWORD)(((uqw>>10)&0x03FF)+1020));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"11"),
								setmsg_attr, 16, (_UWORD)((uqw&0x03FF)+1020));

	// Ch13~16
	m_mainLink->RxPop(&uqw, 5);
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"12"),
								setmsg_attr, 16, (_UWORD)(((uqw>>30)&0x03FF)+1020));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"13"),
								setmsg_attr, 16, (_UWORD)(((uqw>>20)&0x03FF)+1020));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"14"),
								setmsg_attr, 16, (_UWORD)(((uqw>>10)&0x03FF)+1020));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "An"),
								::setAttributeACI(NULL, NULL, L"15"),
								setmsg_attr, 16, (_UWORD)((uqw&0x03FF)+1020));

	// Ch17,18,帧丢失，故障安全
	m_mainLink->RxPop(&ub);
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "Dg"),
								::setAttributeACI(NULL, NULL, L"0"),
								setmsg_attr, 2, (_UBYTE)((ub>>0)&0x01));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "Dg"),
								::setAttributeACI(NULL, NULL, L"1"),
								setmsg_attr, 2, (_UBYTE)((ub>>1)&0x01));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "Lost"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)((ub>>2)&0x01));
	m_xmlDataDb->setOneElement( GetXmlPath("Propo", "FS"),
								::setAttributeACI(NULL, NULL, NULL),
								setmsg_attr, 0, (_UBYTE)((ub>>3)&0x01));	
}

//*******************************************************************************
//	解码（[在基站没有接收到的S / W] EEPROM写入请求/ EEPROM的所获得的数据）
//*******************************************************************************
void UavLinkManage::DecodeEepromData()
{
	//没有处理
}

//*******************************************************************************
//解码（EEPROM操作命令[未在基站S / W接收]）
//*******************************************************************************
void UavLinkManage::DecodeEepromCommand()
{
	//没有处理
}

//*******************************************************************************
//	解码（上层控制MCU的调试面积的值）
//*******************************************************************************
void UavLinkManage::DecodeDebugData_H()
{
	_UBYTE ub;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性

	wchar_t index[16][3] = {	L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", 
								L"8", L"9", L"10", L"11", L"12", L"13", L"14", L"15"	};
	// 0-15
	for(int i=0; i<16; ++i){
		m_mainLink->RxPop(&ub);
		m_xmlDataDb->setOneElement( GetXmlPath("Debug"),
									::setAttributeACI(L"H", NULL, index[i]),
									setmsg_attr, 16, ub );
	}
}

//*******************************************************************************
//	解码（下控制MCU的调试面积的值）
//*******************************************************************************
void UavLinkManage::DecodeDebugData_L()
{
	_UBYTE ub;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性

	wchar_t index[16][3] = {	L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", 
								L"8", L"9", L"10", L"11", L"12", L"13", L"14", L"15"	};
	// 0-15
	for(int i=0; i<16; ++i){
		m_mainLink->RxPop(&ub);
		m_xmlDataDb->setOneElement( GetXmlPath("Debug"),
									::setAttributeACI(L"L", NULL, index[i]),
									setmsg_attr, 16, ub );
	}
}
//*******************************************************************************
//	解码（通用数据）
//*******************************************************************************
void UavLinkManage::DecodeGeneralPurposeData()
{
	_UBYTE type;
	//int type;
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 儊僢僙乕僕偺懏惈
	wchar_t index[4][3] = { L"0", L"1", L"2", L"3"};

	// 通用的数据格式
	m_mainLink->RxPop(&type);
	m_xmlDataDb->setOneElement(GetXmlPath("GP","Type"),::setAttributeACI(NULL, NULL, NULL),setmsg_attr, 0, type);

	// 通用数据
	switch (type)
	{
		case 0x01:
			// FLOAT[4]
			for (int i = 0; i < 4; i++)
			{
				float d;
				m_mainLink->RxPop(&d);
				double data = (double)d;
				m_xmlDataDb->setOneElement(GetXmlPath("GP", "Data"), ::setAttributeACI(NULL, NULL, index[i]), setmsg_attr, 4, data);
			}
			break;

		case 0x02:
			// FLOAT[2] + DOUBLE
			for (int i = 0; i < 2; i++)
			{
				float d;
				m_mainLink->RxPop(&d);
				double data = (double)d; 
				m_xmlDataDb->setOneElement(GetXmlPath("GP", "Data"), ::setAttributeACI(NULL, NULL, index[i]), setmsg_attr, 4, data);
			}
			double data;
			m_mainLink->RxPop(&data);
			m_xmlDataDb->setOneElement(GetXmlPath("GP", "Data"), ::setAttributeACI(NULL, NULL, L"2"), setmsg_attr, 4, data);
			m_xmlDataDb->setOneElement(GetXmlPath("GP", "Data"), ::setAttributeACI(NULL, NULL, L"3"), setmsg_attr, 4, (double)0);
			break;

		case 0x03:
			// DOUBLE[2]
			for (int i = 0; i < 2; i++)
			{
				double data;
				m_mainLink->RxPop(&data);
				m_xmlDataDb->setOneElement(GetXmlPath("GP", "Data"), ::setAttributeACI(NULL, NULL, index[i]), setmsg_attr, 4, data);
			}
			m_xmlDataDb->setOneElement(GetXmlPath("GP", "Data"), ::setAttributeACI(NULL, NULL, L"2"), setmsg_attr, 4, (double)0);
			m_xmlDataDb->setOneElement(GetXmlPath("GP", "Data"), ::setAttributeACI(NULL, NULL, L"3"), setmsg_attr, 4, (double)0);
			break;

		default:
			// DOUBLE[2]
			for (int i = 0; i < 2; i++)
			{
				double data;
				m_mainLink->RxPop(&data);
				m_xmlDataDb->setOneElement(GetXmlPath("GP", "Data"), ::setAttributeACI(NULL, NULL, index[i]), setmsg_attr, 4, data);
			}
			m_xmlDataDb->setOneElement(GetXmlPath("GP", "Data"), ::setAttributeACI(NULL, NULL, L"2"), setmsg_attr, 4, (double)0);
			m_xmlDataDb->setOneElement(GetXmlPath("GP", "Data"), ::setAttributeACI(NULL, NULL, L"3"), setmsg_attr, 4, (double)0);
			break;
	}
}

//*******************************************************************************
//	获取动作类型的字符串
//*******************************************************************************
string UavLinkManage::GetActionString(_UBYTE type) const
{
	string str;
	stringstream ss;

	switch(type)
	{
	case 1:	// 拍摄
		str = "Shot";
		break;

	default: // 未定义操作类型
		ss << "Undefined_" << static_cast<int>(type);
		str = ss.str();
		break;		
	}

	return str;
}

//*******************************************************************************
//	要发送的无人机航点//
//*******************************************************************************
BOOL UavLinkManage::SendWayPointData(UINT dbID)
{
	// 发送标识（“T”或“F”）
	char sendFlag[1];

	// 憲怣僨乕僞崁栚
	double lat;		// 纬度
	double lon;		// 经度
	double alt;		// 高度
	double yaw;		// 方向
	double spd;		// 飞行速度

	//从XML数据管理类，抓取要发送的数据
	vector<_bstr_t> path;
	path.push_back(L"UAV");	// 堦偮忋偺奒憌偵<Data>僞僌偑偁傞慜採
	path.push_back(L"Wpt");

	// 获取发送标志
	path.push_back(L"SendFlag");
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, sendFlag) != S_OK) return FALSE;

	BOOL bRet = FALSE;
	if(sendFlag[0] == 'T'){			// 开始传递
		BOOL bAllOk = TRUE;
		path[2] = L"Lat";
		if (m_xmlDataDb->getOneElement(dbID, path, NULL, &lat) != S_OK) bAllOk = FALSE; 
		path[2] = L"Lon";	
		if (m_xmlDataDb->getOneElement(dbID, path, NULL, &lon) != S_OK) bAllOk = FALSE; 
		path[2] = L"Alt";	
		if (m_xmlDataDb->getOneElement(dbID, path, NULL, &alt) != S_OK) bAllOk = FALSE; 
		path[2] = L"Yaw";	
		if (m_xmlDataDb->getOneElement(dbID, path, NULL, &yaw) != S_OK) bAllOk = FALSE; 
		path[2] = L"Spd";	
		if (m_xmlDataDb->getOneElement(dbID, path, NULL, &spd) != S_OK) bAllOk = FALSE; 

		// 仅当所有的数据是完整的，并且将数据发送
		if(bAllOk)
		{
			m_mainLink->TxBegin(PACKETID_WAYPOINTDATA);
			m_mainLink->TxPush(MiniSvCodec::ConvDegTo5Byte(lat), 5);
			m_mainLink->TxPush(MiniSvCodec::ConvDegTo5Byte(lon), 5);
			m_mainLink->TxPush(MiniSvCodec::Conv10000To3Byte(alt), 3);
			m_mainLink->TxPush(MiniSvCodec::ConvDegTo2Byte(yaw));
			m_mainLink->TxPush(MiniSvCodec::ConvAirSpeed(spd));
			m_mainLink->TxEnd(CMD_BUF_WAYPOINT);
			bRet = TRUE;
		}
	}
	else if(sendFlag[0] == 'F'){	// 传输的结束
		m_mainLink->ClearCmdBuf(CMD_BUF_WAYPOINT);
		bRet = TRUE;

		// Command命令传输完成响应清除
		MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// //消息属性
		m_xmlDataDb->setOneElement(GetXmlPath("Command", "Response"),
			::setAttributeACI(NULL, NULL, NULL),
			setmsg_attr, 0, static_cast<COMMAND>(COMMAND_SLEEP));
	}

	return bRet;
}

//*******************************************************************************
//	无人机希望发送多路点
//*******************************************************************************
              // SendMultiWayPointData(m_multiwayInfo.dbID, TRUE);
BOOL UavLinkManage::SendMultiWayPointData(UINT dbID, BOOL forcedFlush)
{
	// 发送数据项
	unsigned long wptId;	// 航点的ID
	double lat;				// 纬度
	double lon;				// 经度
	double alt;				// 高度
	double yaw;				// 方向
	double spd;				// 飞行速度
	double tol;				// 到来决心圆半径
	double wait;			// 等待时间
	unsigned long  funcId;	// 函数ID
	double tmp;

	// 从XML数据管理类，抓取要发送的数据
	vector<_bstr_t> path;
	path.push_back(L"UAV");	// //到一个级别有一个<数据>标签前提
	path.push_back(L"MltWpt");

	BOOL bAllOk = TRUE;
	path.push_back(L"WptId");
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, &tmp) != S_OK) bAllOk = FALSE;
	wptId = (unsigned long)(tmp + 0.5);
	path[2] = L"Lat";
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, &lat) != S_OK) bAllOk = FALSE;
	path[2] = L"Lon";
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, &lon) != S_OK) bAllOk = FALSE;
	path[2] = L"Alt";
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, &alt) != S_OK) bAllOk = FALSE;
	path[2] = L"Yaw";
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, &yaw) != S_OK) bAllOk = FALSE;
	path[2] = L"Spd";
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, &spd) != S_OK) bAllOk = FALSE;
	path[2] = L"Tol";
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, &tol) != S_OK) bAllOk = FALSE;
	path[2] = L"Wait";
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, &wait) != S_OK) bAllOk = FALSE;
	path[2] = L"FuncId";
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, &tmp) != S_OK) bAllOk = FALSE;
	funcId = (unsigned long)(tmp + 0.5);

	// 仅当所有的数据是完整的，并且将数据发送
	BOOL bRet = FALSE;
	if(bAllOk){
		// 如果“取消”就不发送到所述UAV
		if(wptId == 0xFFFFFFFF && funcId == 0xFFFFFFFF){
			m_multiwayInfo.isTransferring = FALSE;	
			m_multiwayInfo.prevSendTime = 0;		// 复位命令传输时间
			m_mainLink->ClearAllCmdBuf();			// 所有的命令缓冲区清除
			return TRUE;
		}
	
		// 初始化命令在传输过程中//
		if(wptId == 0){
			m_multiwayInfo.isTransferring = TRUE;
			m_multiwayInfo.prevSendTime = 0;	// 复位命令传输时间
			m_mainLink->ClearAllCmdBuf();		//所有的命令缓冲区清空
		}
		// 退出命令在传输过程中//
		else if(wptId == 0xFFFFFFFF){
			funcId = m_multiwayInfo.nextWptId;
		}
		// 
		else{
			wptId = m_multiwayInfo.nextWptId;
		}

		// 创建的发送数据
		m_mainLink->TxBegin(PACKETID_MULTIWAYPOINTDATA);
		m_mainLink->TxPush(wptId);
		m_mainLink->TxPush(MiniSvCodec::ConvDegTo5Byte(lat), 5);
		m_mainLink->TxPush(MiniSvCodec::ConvDegTo5Byte(lon), 5);
		m_mainLink->TxPush(MiniSvCodec::Conv10000To3Byte(alt), 3);
		m_mainLink->TxPush(MiniSvCodec::ConvDegTo2Byte(yaw));
		m_mainLink->TxPush(MiniSvCodec::Conv100To2Byte(spd));
		m_mainLink->TxPush(MiniSvCodec::Conv100To2Byte(tol));
		m_mainLink->TxPush(MiniSvCodec::Conv10000To3Byte(wait), 3);
		m_mainLink->TxPush(funcId);
		m_mainLink->TxEnd(CMD_BUF_MULTIWAYPOINT);

		    //数据将发送
			// - 对于第一命令时，将COMMAND_SLEEP后发送
			// - 第二和随后的命令将被立即发送，而无需等待COMMAND_SLEEP
			// - 然而，如果强制的传送（forcedFlush = TRUE），还发送第一命令立刻
		if(wptId != 0 || forcedFlush == TRUE){
			m_mainLink->TxFlush();
			m_multiwayInfo.prevSendTime = ::timeGetTime();	// 记录传输时间（重发处理）
		}

		//记录包含该数据（重发处理）DB的ID
		m_multiwayInfo.dbID = dbID;

		bRet = TRUE;
	}

	return bRet;
}

//*******************************************************************************
//	发送REQUEST_FIRMWAREINFO命令到无人机//
//*******************************************************************************
BOOL UavLinkManage::SendCmdRequestFirmwareinfo(UINT dbID)
{
	// 僐儅儞僪ID偼巄掕

	// 憲怣Flag乮"T" or "F"乯
	char sendFlag[1];

	// XML僨乕僞娗棟僋儔僗偐傜丄憲怣偡傞僨乕僞傪庢偭偰偔傞
	vector<_bstr_t> path;
	path.push_back(L"UAV");	// //到一个级别有一个<数据>标签前提
	path.push_back(L"Firm");

	//获取发送标志
	path.push_back(L"SendFlag");
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, sendFlag) != S_OK) return FALSE;

	BOOL bRet = FALSE;
	if (sendFlag[0] == 'T'){			//传递开始
		m_mainLink->TxBegin(PACKETID_COMMAND);
		m_mainLink->TxPush<_UWORD>(COMMAND_REQUEST_FIRMWAREINFO);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxEnd(CMD_BUF_GO_HOME);				// TODO:
		bRet = TRUE;
	}
	else if (sendFlag[0] == 'F'){	//传输的结束
		m_mainLink->ClearCmdBuf(CMD_BUF_GO_HOME);		// TODO:
		bRet = TRUE;
	}

	return bRet;
}

//*******************************************************************************
//	//无人机要发送一个命令来GO_HOME 
//*******************************************************************************
BOOL UavLinkManage::SendCmdGoHome(UINT dbID)
{
	// /发送标识（“T”或“F”）
	char sendFlag[1];

	//从XML数据管理类，取要发送的数据
	vector<_bstr_t> path;
	path.push_back(L"UAV"); //到一个级别有一个<数据>标签前提
	path.push_back(L"Home");

	//获取发送标志
	path.push_back(L"SendFlag");
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, sendFlag) != S_OK) return FALSE;

	BOOL bRet = FALSE;
	if(sendFlag[0] == 'T'){			// 开始发送
		m_mainLink->TxBegin(PACKETID_COMMAND);
		m_mainLink->TxPush<_UWORD>(COMMAND_GO_HOME);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxEnd(CMD_BUF_GO_HOME);
		bRet = TRUE;
	}
	else if(sendFlag[0] == 'F'){	// 发送结束
		m_mainLink->ClearCmdBuf(CMD_BUF_GO_HOME);
		bRet = TRUE;

		////命令传输完成响应清除
		MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// //消息属性
		m_xmlDataDb->setOneElement(GetXmlPath("Command", "Response"),
			::setAttributeACI(NULL, NULL, NULL),
			setmsg_attr, 0, static_cast<COMMAND>(COMMAND_SLEEP));
	}

	return bRet;
}

//*******************************************************************************
////无人机要发送一个命令来LAND 
//*******************************************************************************
BOOL UavLinkManage::SendCmdLand(UINT dbID)
{
	//发送标识（“T”或“F”）
	char sendFlag[1];

	// XML僨乕僞娗棟僋儔僗偐傜丄憲怣偡傞僨乕僞傪庢偭偰偔傞
	vector<_bstr_t> path;
	path.push_back(L"UAV");	// //到一个级别有一个<数据>标签前提
	path.push_back(L"Land");

	// 憲怣僼儔僌傪庢摼
	path.push_back(L"SendFlag");
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, sendFlag) != S_OK) return FALSE;

	BOOL bRet = FALSE;
	if(sendFlag[0] == 'T'){			//开始发送
		m_mainLink->TxBegin(PACKETID_COMMAND);
		m_mainLink->TxPush<_UWORD>(COMMAND_LAND);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxEnd(CMD_BUF_LAND);
		bRet = TRUE;
	}
	else if(sendFlag[0] == 'F'){	// 发送结束
		m_mainLink->ClearCmdBuf(CMD_BUF_LAND);
		bRet = TRUE;

		//传输的结束
		MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// //命令传输完成响应清
		m_xmlDataDb->setOneElement(GetXmlPath("Command", "Response"),
			::setAttributeACI(NULL, NULL, NULL),
			setmsg_attr, 0, static_cast<COMMAND>(COMMAND_SLEEP));
	}

	return bRet;
}

//*******************************************************************************
////无人机发送暂停命令
//*******************************************************************************
BOOL UavLinkManage::SendCmdPause(UINT dbID)
{
	////临命令ID

	////发送标识（“T”或“F”）
	char sendFlag[1];

	////从XML数据管理类，抓取要发送的数据
	vector<_bstr_t> path;
	path.push_back(L"UAV");	//到一个级别有一个<数据>标签前提
	path.push_back(L"Pause");

	////获取发送标志
	path.push_back(L"SendFlag");
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, sendFlag) != S_OK) return FALSE;

	BOOL bRet = FALSE;
	if(sendFlag[0] == 'T'){			//开始发送
		m_mainLink->TxBegin(PACKETID_COMMAND);
		m_mainLink->TxPush<_UWORD>(COMMAND_PAUSE);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxEnd(CMD_BUF_PAUSE);
		bRet = TRUE;
	}
	else if(sendFlag[0] == 'F'){	//传输的结束
		m_mainLink->ClearCmdBuf(CMD_BUF_PAUSE);
		bRet = TRUE;

		////命令传输完成响应清
		MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	////消息属性
		m_xmlDataDb->setOneElement(GetXmlPath("Command", "Response"),
			::setAttributeACI(NULL, NULL, NULL),
			setmsg_attr, 0, static_cast<COMMAND>(COMMAND_SLEEP));
	}

	return bRet;
}

//*******************************************************************************
//	//无人机要发送RESUME命令
//*******************************************************************************
BOOL UavLinkManage::SendCmdResume(UINT dbID)
{
	//命令ID临

	//发送标识（“T”或“F”）
	char sendFlag[1];

	//从XML数据管理类，抓取要发送的数据
	vector<_bstr_t> path;
	path.push_back(L"UAV"); //到一个级别有一个<数据>标签前提

	//获取发送标志
	path.push_back(L"SendFlag");
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, sendFlag) != S_OK) return FALSE;

	BOOL bRet = FALSE;
	if(sendFlag[0] == 'T'){			// 发送开始
		m_mainLink->TxBegin(PACKETID_COMMAND);
		m_mainLink->TxPush<_UWORD>(COMMAND_RESUME);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxEnd(CMD_BUF_RESUME);
		bRet = TRUE;
	}
	else if(sendFlag[0] == 'F'){	// 发送结束
		m_mainLink->ClearCmdBuf(CMD_BUF_RESUME);
		bRet = TRUE;

		///命令传输完成响应清除
		MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// 消息属性
		m_xmlDataDb->setOneElement(GetXmlPath("Command", "Response"),
			::setAttributeACI(NULL, NULL, NULL),
			setmsg_attr, 0, static_cast<COMMAND>(COMMAND_SLEEP));
	}

	return bRet;
}

//*******************************************************************************
//	/无人机一个ACK它会发送动作执行点的数据/
//*******************************************************************************
BOOL UavLinkManage::SendActionPointAck(_UBYTE actionType, _UWORD count)
{
	m_mainLink->TxBegin(PACKETID_ACTIONPOINT_ACK);
	m_mainLink->TxPush(actionType);
	m_mainLink->TxPush(count);
	m_mainLink->TxEnd(CMD_BUF_ACTIONOINT_ACK);

	return TRUE;
}

//*******************************************************************************
////无人机它确实处理COMMAND_SLEEP从
//*******************************************************************************
BOOL UavLinkManage::ProcCmdSleep()
{
	// //或包含多路点命令发送缓冲器
	BOOL existsCmd = m_mainLink->ExistsCmd(CMD_BUF_MULTIWAYPOINT);

	// 它会发送数据（命令）//无人机
//（如果你发送的数据是不是什么都不做）
	if(m_mainLink->TxFlush() != C_OK){
		return FALSE;
	}

	if(existsCmd){
		m_multiwayInfo.prevSendTime = ::timeGetTime();	// 记录一个多路传输命令的时间（重发处理）
	}

	return TRUE;
}
BOOL UavLinkManage::Send()
{
	if (m_flagSendReady)
	{
		// 如果它包含一个多路点命令发送缓冲器
		BOOL existsCmd = m_mainLink->ExistsCmd(CMD_BUF_MULTIWAYPOINT);

		// 它会发送数据（命令）//无人机
//（如果你发送的数据是不是什么都不做）
		if (m_mainLink->TxFlush() != C_OK){
			return FALSE;
		}

		if (existsCmd){
			m_multiwayInfo.prevSendTime = ::timeGetTime();	// /记录一个多路传输命令的时间（重发处理）
		}
		m_flagSendReady = false;	///当你的数据传输标志清楚
		return TRUE;
	}
	return FALSE;
}
//*******************************************************************************
//	确实处理COMMAND_REQUEST_TIME从//无人机
//*******************************************************************************
BOOL UavLinkManage::ProcCmdReqTime()
{
	///获取本地时间
	SYSTEMTIME st;
	GetSystemTime(&st);
	
	_UDWORD dataAndTime = MiniSvCodec::ConvDateAndTimeTo4Byte(	_UBYTE(st.wYear-2000),
																_UBYTE(st.wMonth),
																_UBYTE(st.wDay),
																_UBYTE(st.wHour),
																_UBYTE(st.wMinute),
																_UBYTE(st.wSecond) );
	//数据包生成
	m_mainLink->TxBegin(PACKETID_REPLY);
	m_mainLink->TxPush(_UWORD(COMMAND_REQUEST_TIME));
	m_mainLink->TxPush(_UBYTE((dataAndTime>>0)&0xFF));		////从低字节
	m_mainLink->TxPush(_UBYTE((dataAndTime>>8)&0xFF));
	m_mainLink->TxPush(_UBYTE((dataAndTime>>16)&0xFF));
	m_mainLink->TxPush(_UBYTE((dataAndTime>>24)&0xFF));
	m_mainLink->TxEnd(CMD_BUF_REQ_TIME_REPLY);

	return TRUE;
}

//*******************************************************************************
//	你要注册的XML数据管理单元的信号强度
//*******************************************************************************
BOOL UavLinkManage::RegisterRssi(_UBYTE rssi)
{
	if(rssi == 0) return FALSE;

	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	// //消息属性

	// RSSI抣
	m_xmlDataDb->setOneElement( GetXmlPath("RSSI", "Datalink"),
								::setAttributeACI(L"D", NULL, NULL),
								setmsg_attr, 0, (_SWORD)(-1*rssi));

	// Health抣
	DWORD nowTime = ::timeGetTime();
	_UBYTE healthA = max((_UBYTE)((-1*rssi - PSSI_PARAM_A)*(15.0 / (PSSI_PARAM_B - PSSI_PARAM_A))+0.5), 1);	////健康值
	_UBYTE healthB = (nowTime - m_recvTime) < PSSI_PARAM_TIMEOUT ? 15 : 0;
	_UBYTE health = min(healthA, healthB);
	m_xmlDataDb->setOneElement( GetXmlPath("Health", "Datalink"),
								::setAttributeACI(L"D", NULL, NULL),
								setmsg_attr, 0, health);

	////更新时收到一个有效的数据包
	m_recvTime = nowTime;	

	return TRUE;
}

//*******************************************************************************
//生成XML路径
//*******************************************************************************
vector<_bstr_t> UavLinkManage::GetXmlPath(_bstr_t path1, _bstr_t path2, _bstr_t path3)
{
	vector<_bstr_t> path;

	path = m_commonPath;
	if(path1.length() != 0) path.push_back(path1);
	if(path2.length() != 0) path.push_back(path2);
	if(path3.length() != 0) path.push_back(path3);

	return path;
}

//*******************************************************************************
////无人机发送的SerialNumber命令
//*******************************************************************************
BOOL UavLinkManage::SendCmdRequestSerialNumber(UINT dbID)
{
	// 临命令ID

	// 发送标识（“T”或“F”）
	char sendFlag[1];

	//从XML数据管理类，抓取要发送的数据
	vector<_bstr_t> path;
	path.push_back(L"UAV");	// //到一个级别有一个<数据>标签前提
	path.push_back(L"SerialNum");

	// /获取发送标志
	path.push_back(L"SendFlag");
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, sendFlag) != S_OK) return FALSE;

	BOOL bRet = FALSE;
	if (sendFlag[0] == 'T'){			//传递开始
		m_mainLink->TxBegin(PACKETID_EEPROM_COMMAND);						// E2PDEF_RW_KIND_READ 0x01  
		m_mainLink->TxPush<_UBYTE>(0x01);
		m_mainLink->TxPush<_UWORD>(100);							// E2P_BID_SERIALNUMBER 100制造业信息化
		m_mainLink->TxEnd(1);						
		bRet = TRUE;
	}
	else if (sendFlag[0] == 'F'){	//传输的结束
		m_mainLink->ClearCmdBuf(CMD_BUF_NUM);
		bRet = TRUE;
	}

	return bRet;
}

//*******************************************************************************
//	
//*******************************************************************************
void UavLinkManage::DecodeSerialNumber()
{
	MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	 //消息属性


	_UBYTE SendNo;
	_UWORD BlockID;
	_UBYTE byte[29];

	m_mainLink->RxPop<_UBYTE>(&SendNo);
	m_mainLink->RxPop<_UWORD>(&BlockID);
	for (_UDWORD i = 0; i < 29; i++)
	{
		m_mainLink->RxPop<_UBYTE>(&byte[i]);
	}

	//-------------
	////飞机管理信息
	//-------------
	CString str;

	//型号和序列号
	str.Empty();
	str.Format(_T("%s"), byte);
	
	char* pSerialNumber = new char[sizeof(byte)+1];
	ZeroMemory(pSerialNumber, sizeof(byte)+1);
	CopyMemory(pSerialNumber, byte, sizeof(byte));
	str = pSerialNumber;
	
	m_xmlDataDb->setOneElement(GetXmlPath("E2P", "SerialNum"),
		::setAttributeACI(NULL, NULL, NULL),
		setmsg_attr, str);

	delete[] pSerialNumber;
}

//*******************************************************************************
////无人机	发送GO_HOME命令
//*******************************************************************************
BOOL UavLinkManage::SendCmdPlannedFlight(UINT dbID)
{
	// /发送标识（“T”或“F”）
	char sendFlag[1];

	// /发送标识（“T”或“F”）
	vector<_bstr_t> path;
	path.push_back(L"UAV"); ///到一个级别有一个<数据>标签前提
	path.push_back(L"MltWptE");

	//获取发送标志
	path.push_back(L"SendFlag");
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, sendFlag) != S_OK) return FALSE;

	BOOL bRet = FALSE;
	if (sendFlag[0] == 'T'){			//传递开始
		m_mainLink->TxBegin(PACKETID_COMMAND);
		m_mainLink->TxPush<_UWORD>(COMMAND_EXIT_PLANNEDFLIGHT);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxPush<_UBYTE>(0);
		m_mainLink->TxEnd(CMD_BUF_GO_HOME);
		bRet = TRUE;
	}
	else if (sendFlag[0] == 'F'){	// 传输的结束
		m_mainLink->ClearCmdBuf(CMD_BUF_GO_HOME);
		bRet = TRUE;
	}

	return bRet;
}

//*******************************************************************************
//	
//*******************************************************************************
BOOL UavLinkManage::SendError(UINT dbID)
{
	double sendNum = -1;

	// 从XML数据管理类，抓取要发送的数据
	vector<_bstr_t> path;
	path.push_back(L"UAV");	// /到一个级别有一个<数据>标签前提
	path.push_back(L"Error");

	// 获取发送标志
	path.push_back(L"SendFlag");
	if (m_xmlDataDb->getOneElement(dbID, path, NULL, &sendNum) != S_OK) return FALSE;

	BOOL bRet = FALSE;
	int _num = (int)sendNum;
	if (_num >= 0 && _num  < 2){	//传输的结束
		bRet = TRUE;

		// 明确的错误
		MSGATTRIBUTES setmsg_attr = ::setMsgAttribute(L"set", NULL, NULL, NULL);	////消息属性

		wchar_t index[2][3] = { L"0", L"1" };
		//错误的类ID
		m_xmlDataDb->setOneElement(GetXmlPath("Error", "ID"), ::setAttributeACI(NULL, NULL, index[_num]), setmsg_attr, 2, (_UBYTE)(0));

		// 错误水平
		m_xmlDataDb->setOneElement(GetXmlPath("Error", "Level"), ::setAttributeACI(NULL, NULL, index[_num]), setmsg_attr, 2, static_cast<_UBYTE>(0));

		// 时间信息
		m_xmlDataDb->setOneElement(GetXmlPath("Error", "Ticks"), ::setAttributeACI(NULL, NULL, index[_num]), setmsg_attr, 2, (_UWORD)(0));

		// 错误类型
		m_xmlDataDb->setOneElement(GetXmlPath("Error", "Category"), ::setAttributeACI(NULL, NULL, index[_num]), setmsg_attr, 2, (static_cast<_UWORD>(0)));

		// 错误的具体参数
		m_xmlDataDb->setOneElement(GetXmlPath("Error", "Param"), ::setAttributeACI(NULL, NULL, index[_num]), setmsg_attr, 2, (_UWORD)(0));
	}

	return bRet;
}
