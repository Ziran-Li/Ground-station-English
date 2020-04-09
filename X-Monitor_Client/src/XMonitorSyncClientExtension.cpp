#include "stdafx.h"
#include "XMonitorSyncClientExtension.h"

#include "Main.h"

void XMonitorSyncClientExt::OnSync()
{
	if( CWnd* pMainWnd = theApp.GetMainWnd() ) {
		if( pMainWnd->GetSafeHwnd() ) {
			pMainWnd->SendMessage(WM_DATA_RECEIVE);
		}
	}
}