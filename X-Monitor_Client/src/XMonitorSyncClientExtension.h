#pragma once

//// Network Communication
//#include "../X-MonitorClientLib/X-MonitorClientLib.h"
//#pragma comment(lib, "../Inter Process Communication/X-MonitorClientLib" DEBUG_LIB_CHAR ".lib")

#include "miniSv.h"

class XMonitorSyncClientExt : public XMonitorSyncClient
{
protected:
	virtual void OnSync();
};