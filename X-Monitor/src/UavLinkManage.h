#pragma once

#include <vector>
#include "SCI.h"

//#import "msxml3.dll" named_guids
#import "msxml6.dll" named_guids

class CUavLink;
class dataManage;

class UavLinkManage
{
	//// ����

	// ����RSSI��Healthֵ
	enum{
		PSSI_PARAM_A		= -100,		// RSSI���Ϊ��ʱ�Ľ��
		PSSI_PARAM_B		= -80,		// RSSI���Ϊ15ʱ�Ľ��
		PSSI_PARAM_TIMEOUT	= 1000,		// ��ʱʱ��[ms]
	};

	// ���Ա�Ӷ��㷢�����
	enum CmdBufId{
		CMD_BUF_SLEEP_REPLY	= 0,		// �� COMMAND_SLEEP ����
		CMD_BUF_GO_HOME,				// ����ָ��
		CMD_BUF_LAND,					// ��½ָ��
		CMD_BUF_PAUSE,					// PAUSEָ��
		CMD_BUF_RESUME,					// RESUMEָ��
		CMD_BUF_WAYPOINT,				// ·������
		CMD_BUF_ACTIONOINT_ACK,			// �ж�������ACK
		CMD_BUF_MULTIWAYPOINT,			// ��·������
		CMD_BUF_REQ_TIME_REPLY,			// ��COMMAND_REQUEST_TIME����
		CMD_BUF_NUM						// ���������Ԫ����Ŀ	
	};

	//// �ṹ��

	// ACK��·������
public:
	struct MultiwayAck{
		unsigned long wptId;			// �����ʶ��
		double lat;						// γ��
		double lon;						// ����
		double alt;						// �߶�
		double yaw;						// ����
		double spd;						// �ٶ�
		double tol;						// ����ָ����뾶
		double wait;					// �ȴ�ʱ��
		unsigned long  funcId;			// ����ID

		MultiwayAck():wptId(0),lat(0),lon(0),alt(0),yaw(0),spd(0),tol(0),wait(0),funcId(0){}
	};

private:
	// ��·����Ϣ
	struct MultiWayInfo{
		unsigned long nextWptId;		// ����·���ʶ��
		unsigned long prevSendTime;		// ��������һ��[ms]
		unsigned long randVal;			// �յ������ֵ
		BOOL isTransferring;			// �Ƿ�������;��
		std::vector<MultiwayAck> ack;	// �յ���ACK������ͨ���Ĳ������������һ����
		unsigned short retryInterval;	// ���·���������[ms]
		unsigned int dbID;				// �������������DBID
		MultiWayInfo():nextWptId(0),prevSendTime(0),randVal(0),isTransferring(FALSE),ack(),retryInterval(0),dbID(0){}
	};

	////��Ա����
	CUavLink	*m_mainLink;			// ��Ҫ����
	dataManage	*m_xmlDataDb;			// XML���ݹ���
	std::vector<_bstr_t> m_commonPath;	// ����XML���ݹ���ע��·����·����Ϊ���õ�
	DWORD m_recvTime;					// ������Ч���ݰ��������RSSI�Ľ���ֵ[ms]
	MultiWayInfo m_multiwayInfo;		// ��·����Ϣ

	// ���ͺ���ִ��Ԥ�� COMMAND_SLEEP
	bool m_flagSendReady;

public:
	CString m_dbgMsg;	// ���ڵ���


	//// ��Ա����
public:
	UavLinkManage();
	~UavLinkManage();

	// ��ʼ��
	BOOL Initialize(CUavLink *wl, dataManage *dm);

	// ����MainLink
	BOOL Connect(SCIPORT comPortNo);

	// ���Ӽ��
	BOOL IsConnected();

	// �������ݽ��뼰ע��xml������
	int DecodeAndRegister();

	// ������ע��xml�����ʱ��
	void RegisterLocalTime();

	// MultiWayPointDataע�� XML ���ݹ���
	void RegisterMultiWayPointData(bool flag, BSTR value);
	void RegisterData(MSXML2::IXMLDOMDocument2Ptr& pDoc);

	// ���ݴ���(���ݵ�DBID�����������)
	BOOL SendData(UINT dbID, const _bstr_t &sendMsg);

	// ��·���Ƿ�����ִ�й�����
	BOOL IsMultiwayTransferring() const { return m_multiwayInfo.isTransferring; }

	// �����Ѿ����յ������ݡ���·��ACK���
	BOOL PopMultiwayAckData(MultiwayAck &ack);

	// ��·���������ԣ���ȷ��
	BOOL DecideMultiwayCmdRetry();

	BOOL Send();

private:
	// ����
	void DecodeCommand();			// UAV/GCS�������
	void DecodeReply();				// COMMAND_PACKET����
	void DecodeInputData();			// kong
	void DecodeInputData_6R();		// ����ϵͳ����������ת�ӵĿ�����������
	void DecodeInputData_12R();		// ����ϵͳ������12��ת�ӵĿ�����������
	void DecodeInputData_Min();		// ���Ƶ���С����
	void DecodeImuPsData();			// IMU��ѹ������
	void DecodeMagData();			// IMU�ش�����
	void DecodeGpsData();			// ԭʼGPS����
	void DecodeGpsInfo();			// GPS������Ϣ
	void DecodeNavData();			// ��������
	void DecodeNavDataEx();			// ������������̬����
	void DecodeRefData();			// Ŀ���������
	void DecodeWayPointData();		// �������ݺ�����
	void DecodeWayPointData2();		// �����뺽����������Ŀ
	void DecodeHomeData();			// ����������
	void DecodeSystemData();		// ��Ӧϵͳ�ڲ�״̬������
	void DecodeErrorData();			// UAV�Ϸ����Ĵ�������
	void DecodeActionPoint();		// �ж���ִ�е�����
	void DecodeActionPointAck();	// ��ȡ�ж�ִ������ ACK �� [s/w ��վû���յ�]
	void DecodeMultiWayPointData();	// ��㴫������ [s/w ��վû���յ�]
	void DecodeMultiWayPointAck();	// ��·�㽫��Ӧ���ݴ���
	void DecodeFirmwareInfo();		// ֪ͨ�̼���Ϣ��
	void DecodePropoData();			// ң������
	void DecodeEepromData();		// EEPROM д���� /EEPROM [s/w ��վ������] ���ݲɼ�
	void DecodeEepromCommand();		// EEPROM ���� [s/w ��վû���յ�]
	void DecodeDebugData_H();		// ��λ�����Ƶ�Ƭ��ֵ
	void DecodeDebugData_L();		// ��λ�����Ƶ�Ƭ��ֵ
	void DecodeGeneralPurposeData();// ͨ������

	// ��ȡһ���ַ�����������
	std::string GetActionString(_UBYTE type) const;

	// ��������
	BOOL SendWayPointData(UINT dbID);
	BOOL SendMultiWayPointData(UINT dbID, BOOL forcedFlush = FALSE);
	BOOL SendCmdRequestFirmwareinfo(UINT dbID);
	BOOL SendCmdRequestSendCmdRequestSerialNum(UINT dbID);
	BOOL SendCmdGoHome(UINT dbID);
	BOOL SendCmdLand(UINT dbID);
	BOOL SendCmdPause(UINT dbID);
	BOOL SendCmdResume(UINT dbID);
	BOOL SendActionPointAck(_UBYTE actionType, _UWORD count);

	BOOL SendCmdRequestSerialNumber(UINT dbID);
	void DecodeSerialNumber();

	BOOL SendCmdPlannedFlight(UINT dbID);
	BOOL SendError(UINT dbID);

	// �����˻���Ӧ����
	BOOL ProcCmdSleep();
	BOOL ProcCmdReqTime();

	// Ҫע��XML���ݹ�����
	BOOL RegisterRssi(_UBYTE rssi);

	// ʵ�ó���
	std::vector<_bstr_t> GetXmlPath(_bstr_t path1, _bstr_t path2 = L"", _bstr_t path3 = L"");	// ���ɵ�xml·��
};

