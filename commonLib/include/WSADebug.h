//
//	Winsock�p�f�o�b�M���O�N���X
//
//	X-Monitor �̃\�����[�V�����ŊJ�����D
//	�J���������惉�C�u�����Ɉړ�
//
//	stdafx.h �� CWSADEBUG_ENGLISHMODE ���`����Ɖp��̃��b�Z�[�W��\�����܂��D
//

#pragma once

#include <winsock2.h>
#include "debug.h"

class CWSADebug : public CDebug
{
public:
	static void WSAPrintLastError(LPCTSTR pAdditionalMessage = NULL);
	static void WSAPrintNetworkEvents(WSANETWORKEVENTS events);
	static void WSAFormatMessage(int WSAErrorCode, LPCTSTR pAdditionalMessage = NULL);
};