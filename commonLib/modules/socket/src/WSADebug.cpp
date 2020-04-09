
#include "stdafx.h"

#include "WSADebug.h"

#ifdef CDEBUG_DEBUGMODE
#ifndef _DEBUG
#define _DEBUG
#endif
#endif

void CWSADebug::WSAPrintLastError(LPCTSTR pAdditionalMessage)
{
	CWSADebug::WSAFormatMessage(::WSAGetLastError(), pAdditionalMessage);
}

void CWSADebug::WSAFormatMessage(int WSAErrorCode, LPCTSTR pAdditionalMessage)
{
#ifdef _DEBUG

	LPCTSTR errmessage;

	switch( WSAErrorCode ) {
#ifdef CWSADEBUG_ENGLISHMODE
		case WSAEINTR:              errmessage = _T("Interrupted function call."); break;
		case WSAEACCES:             errmessage = _T("Permission denied."); break;
		case WSAEFAULT:             errmessage = _T("Bad address."); break;
		case WSAEINVAL:             errmessage = _T("Invalid argument."); break;
		case WSAEMFILE:             errmessage = _T("Too many open files."); break;
		case WSAEWOULDBLOCK:        errmessage = _T("Resource temporarily unavailable."); break;
		case WSAEINPROGRESS:        errmessage = _T("Operation now in progress."); break;
		case WSAEALREADY:           errmessage = _T("Operation already in progress."); break;
		case WSAENOTSOCK:           errmessage = _T("Socket operation on nonsocket."); break;
		case WSAEDESTADDRREQ:       errmessage = _T("Destination address required."); break;
		case WSAEMSGSIZE:           errmessage = _T("Message too long."); break;
		case WSAEPROTOTYPE:         errmessage = _T("Protocol wrong type for socket."); break;
		case WSAENOPROTOOPT:        errmessage = _T("Bad protocol option."); break;
		case WSAEPROTONOSUPPORT :   errmessage = _T("Protocol not supported."); break;
		case WSAESOCKTNOSUPPORT:    errmessage = _T("Socket type not supported."); break;
		case WSAEOPNOTSUPP:         errmessage = _T("Operation not supported."); break;
		case WSAEPFNOSUPPORT:       errmessage = _T("Protocol family not supported."); break;
		case WSAEAFNOSUPPORT:       errmessage = _T("Address family not supported by protocol family."); break;
		case WSAEADDRINUSE:         errmessage = _T("Address already in use."); break;
		case WSAEADDRNOTAVAIL:      errmessage = _T("Cannot assign requested address."); break;
		case WSAENETDOWN:           errmessage = _T("Network is down."); break;
		case WSAENETUNREACH:        errmessage = _T("Network is unreachable."); break;
		case WSAENETRESET:          errmessage = _T("Network dropped connection on reset."); break;
		case WSAECONNABORTED:       errmessage = _T("Software caused connection abort."); break;
		case WSAECONNRESET:         errmessage = _T("Connection reset by peer."); break;
		case WSAENOBUFS:            errmessage = _T("No buffer space available."); break;
		case WSAEISCONN:            errmessage = _T("Socket is already connected."); break;
		case WSAENOTCONN:           errmessage = _T("Socket is not connected."); break;
		case WSAESHUTDOWN:          errmessage = _T("Cannot send after socket shutdown."); break;
		case WSAETIMEDOUT:          errmessage = _T("Connection timed out."); break;
		case WSAECONNREFUSED:       errmessage = _T("Connection refused."); break;
		case WSAEHOSTDOWN:          errmessage = _T("Host is down."); break;
		case WSAEHOSTUNREACH:       errmessage = _T("No route to host."); break;
		case WSAEPROCLIM:           errmessage = _T("Too many processes."); break;
		case WSASYSNOTREADY:        errmessage = _T("Network subsystem is unavailable."); break;
		case WSAVERNOTSUPPORTED:    errmessage = _T("Winsock.dll version out of range."); break;
		case WSANOTINITIALISED:     errmessage = _T("Successful WSAStartup not yet performed."); break;
		case WSAEDISCON:            errmessage = _T("Graceful shutdown in progress."); break;
		case WSATYPE_NOT_FOUND:     errmessage = _T("Class type not found."); break;
		case WSAHOST_NOT_FOUND:     errmessage = _T("Host not found. No such host is known."); break;
		case WSATRY_AGAIN:          errmessage = _T("Nonauthoritative host not found."); break;
		case WSANO_RECOVERY:        errmessage = _T("This is a nonrecoverable error."); break;
		case WSANO_DATA:            errmessage = _T("Valid name, no data record of requested type."); break;
#ifdef WSA_INVALID_HANDLE
		case WSA_INVALID_HANDLE:    errmessage = _T("Specified event object handle is invalid."); break;
#endif
#ifdef WSA_INVALID_PARAMETER
		case WSA_INVALID_PARAMETER: errmessage = _T("One or more parameters are invalid."); break;
#endif
#ifdef WSA_IO_INCOMPLETE
		case WSA_IO_INCOMPLETE:     errmessage = _T("Overlapped I/O event object not in signaled state."); break;
#endif
#ifdef WSA_IO_PENDING
		case WSA_IO_PENDING:        errmessage = _T("Overlapped operations will complete later."); break;
#endif
#ifdef WSA_NOT_ENOUGH_MEMORY
		case WSA_NOT_ENOUGH_MEMORY: errmessage = _T("Insufficient memory available."); break;
#endif
#ifdef WSA_OPERATION_ABORTED
		case WSA_OPERATION_ABORTED: errmessage = _T("Overlapped operation aborted."); break;
#endif
#ifdef WSAINVALIDPROCTABLE
		case WSAINVALIDPROCTABLE:   errmessage = _T("Invalid procedure table from service provider."); break;
#endif
#ifdef WSAINVALIDPROVIDER
		case WSAINVALIDPROVIDER:    errmessage = _T("Invalid service provider version number."); break;
#endif
#ifdef WSAPROVIDERFAILEDINIT
		case WSAPROVIDERFAILEDINIT: errmessage = _T("Unable to initialize a service provider."); break;
#endif
#ifdef WSASYSCALLFAILURE
		case WSASYSCALLFAILURE:     errmessage = _T("System call failure."); break;
#endif
		default:                    errmessage = _T("Corresponding error message not found."); break;

#else // #ifdef CWSADEBUG_ENGLISHMODE

		case WSAEINTR:              errmessage = _T("�֐��Ăяo���Ɋ��荞�݂�����܂����B"); break;
		case WSAEACCES:             errmessage = _T("�A�N�Z�X�͋��ۂ���܂����B"); break;
		case WSAEFAULT:             errmessage = _T("�A�h���X������������܂���B"); break;
		case WSAEINVAL:             errmessage = _T("�����Ȉ����ł��B"); break;
		case WSAEMFILE:             errmessage = _T("�J���Ă���t�@�C�����������܂��B"); break;
		case WSAEWOULDBLOCK:        errmessage = _T("���\�[�X���ꎞ�I�ɗ��p�ł��Ȃ��Ȃ��Ă��܂��B"); break;
		case WSAEINPROGRESS:        errmessage = _T("����͌��ݎ��s���ł��B"); break;
		case WSAEALREADY:           errmessage = _T("����͊��Ɏ��s���ł��B"); break;
		case WSAENOTSOCK:           errmessage = _T("��\�P�b�g�ɑ΂��ă\�P�b�g��������݂܂����B"); break;
		case WSAEDESTADDRREQ:       errmessage = _T("���M��̃A�h���X���K�v�ł��B"); break;
		case WSAEMSGSIZE:           errmessage = _T("���b�Z�[�W���������܂��B"); break;
		case WSAEPROTOTYPE:         errmessage = _T("�v���g�R���̎�ނ��\�P�b�g�ɑ΂��Đ���������܂���B"); break;
		case WSAENOPROTOOPT:        errmessage = _T("�v���g�R���̃I�v�V����������������܂���B"); break;
		case WSAEPROTONOSUPPORT :   errmessage = _T("�v���g�R�����T�|�[�g����Ă��܂���B"); break;
		case WSAESOCKTNOSUPPORT:    errmessage = _T("�T�|�[�g����Ă��Ȃ��\�P�b�g�̎�ނł��B"); break;
		case WSAEOPNOTSUPP:         errmessage = _T("���삪�\�P�b�g��ŃT�|�[�g����Ă��܂���B"); break;
		case WSAEPFNOSUPPORT:       errmessage = _T("�v���g�R�� �t�@�~�����T�|�[�g����Ă��܂���B"); break;
		case WSAEAFNOSUPPORT:       errmessage = _T("�v���g�R�� �t�@�~���̓A�h���X �t�@�~�����T�|�[�g���Ă��܂���B"); break;
		case WSAEADDRINUSE:         errmessage = _T("�A�h���X�͊��Ɏg�p���ł��B"); break;
		case WSAEADDRNOTAVAIL:      errmessage = _T("�v�����ꂽ�A�h���X�����蓖�Ă��܂���B"); break;
		case WSAENETDOWN:           errmessage = _T("�l�b�g���[�N���_�E�����Ă��܂��B"); break;
		case WSAENETUNREACH:        errmessage = _T("ICMP �l�b�g���[�N�ɓ��B�ł��܂���B"); break;
		case WSAENETRESET:          errmessage = _T("�l�b�g���[�N�����Z�b�g���ꂽ���ߐؒf����܂����B"); break;
		case WSAECONNABORTED:       errmessage = _T("�\�t�g�E�F�A�ɂ���Đڑ������~����܂����B"); break;
		case WSAECONNRESET:         errmessage = _T("�s�A�ɂ���Đڑ������Z�b�g����܂����B"); break;
		case WSAENOBUFS:            errmessage = _T("�o�b�t�@�̈悪�T�|�[�g����Ă��܂���B"); break;
		case WSAEISCONN:            errmessage = _T("�\�P�b�g�͊��ɐڑ�����Ă��܂��B"); break;
		case WSAENOTCONN:           errmessage = _T("�\�P�b�g���ڑ�����Ă��܂���B"); break;
		case WSAESHUTDOWN:          errmessage = _T("�\�P�b�g�̃V���b�g�_�E����ɑ��M�ł��܂���B"); break;
		case WSAETIMEDOUT:          errmessage = _T("�ڑ����^�C���A�E�g�ɂȂ�܂����B"); break;
		case WSAECONNREFUSED:       errmessage = _T("�ڑ������ۂ���܂����B"); break;
		case WSAEHOSTDOWN:          errmessage = _T("�z�X�g���_�E�����Ă��܂��B"); break;
		case WSAEHOSTUNREACH:       errmessage = _T("�z�X�g�ɓ��B���邽�߂̃��[�g������܂���B"); break;
		case WSAEPROCLIM:           errmessage = _T("�v���Z�X���������܂��B"); break;
		case WSASYSNOTREADY:        errmessage = _T("�l�b�g���[�N �T�u�V�X�e�������p�ł��܂���B"); break;
		case WSAVERNOTSUPPORTED:    errmessage = _T("���̃o�[�W������ Winsock.dll �̓T�|�[�g����Ă��܂���B"); break;
		case WSANOTINITIALISED:     errmessage = _T("WSAStartup �̎��s�ɐ������Ă��܂���B"); break;
		case WSAEDISCON:            errmessage = _T("����ȃV���b�g�_�E���������i�s���ł��B"); break;
		case WSATYPE_NOT_FOUND:     errmessage = _T("���̎�ނ̃N���X��������܂���B"); break;
		case WSAHOST_NOT_FOUND:     errmessage = _T("�z�X�g��������܂���B���̂悤�ȃz�X�g�͂���܂���B"); break;
		case WSATRY_AGAIN:          errmessage = _T("�z�X�g��������܂���B���������T�[�o�[����̉���������܂���B"); break;
		case WSANO_RECOVERY:        errmessage = _T("�񕜕s�\�ȃG���[���������܂����B"); break;
		case WSANO_DATA:            errmessage = _T("�v�����ꂽ���O�͗L���ł����A�v�����ꂽ��ނ̃f�[�^ ���R�[�h������܂���B"); break;
#ifdef WSA_INVALID_HANDLE
		case WSA_INVALID_HANDLE:    errmessage = _T("�w�肳�ꂽ�C�x���g �I�u�W�F�N�g�̃n���h���������ł��B"); break;
#endif
#ifdef WSA_INVALID_PARAMETER
		case WSA_INVALID_PARAMETER: errmessage = _T("1 �܂��͕����̃p�����[�^�������ł��B"); break;
#endif
#ifdef WSA_IO_INCOMPLETE
		case WSA_IO_INCOMPLETE:     errmessage = _T("�d������ I/O �C�x���g�̓V�O�i�����ꂽ��Ԃł͂���܂���B"); break;
#endif
#ifdef WSA_IO_PENDING
		case WSA_IO_PENDING:        errmessage = _T("�d����������͌�Ɋ������܂��B"); break;
#endif
#ifdef WSA_NOT_ENOUGH_MEMORY
		case WSA_NOT_ENOUGH_MEMORY: errmessage = _T("���������s�����Ă��܂��B"); break;
#endif
#ifdef WSA_OPERATION_ABORTED
		case WSA_OPERATION_ABORTED: errmessage = _T("�d���������삪��������܂����B"); break;
#endif
#ifdef WSAINVALIDPROCTABLE
		case WSAINVALIDPROCTABLE:   errmessage = _T("�T�[�r�X �v���o�C�_����Ԃ��ꂽ�v���V�[�W�� �e�[�u���������ł��B"); break;
#endif
#ifdef WSAINVALIDPROVIDER
		case WSAINVALIDPROVIDER:    errmessage = _T("�T�[�r�X �v���o�C�_�̃o�[�W�����ԍ��������ł��B"); break;
#endif
#ifdef WSAPROVIDERFAILEDINIT
		case WSAPROVIDERFAILEDINIT: errmessage = _T("�T�[�r�X �v���o�C�_���������ł��܂���B"); break;
#endif
#ifdef WSASYSCALLFAILURE
		case WSASYSCALLFAILURE:     errmessage = _T("�V�X�e�� �R�[�������s���܂����B"); break;
#endif
		default:                    errmessage = _T("�Ή�����G���[���b�Z�[�W�͂���܂���B"); break;

#endif // #ifdef CWSADEBUG_ENGLISHMODE
	}

	if( pAdditionalMessage ) {
		CDebug::printf(_T("ErrorCode: %d ErrorMessage: %s %s\n"), WSAErrorCode, errmessage, pAdditionalMessage);
	} else {
		CDebug::printf(_T("ErrorCode: %d ErrorMessage: %s\n"), WSAErrorCode, errmessage);
	}
#endif
}

void CWSADebug::WSAPrintNetworkEvents(WSANETWORKEVENTS events)
{
#ifdef _DEBUG
	CDebug::puts(_T("------------------------------"));
	CDebug::printf(_T("lNetworkEvents = %d\n"), events.lNetworkEvents);
	if( events.lNetworkEvents & FD_ACCEPT ) {
		CDebug::puts(_T("FD_ACCEPT - incoming connections"));
	}
	if( events.lNetworkEvents & FD_ADDRESS_LIST_CHANGE ) {
		CDebug::puts(_T("FD_ADDRESS_LIST_CHANGE - local address list changes for the address family of the socket"));
	}
	if( events.lNetworkEvents & FD_GROUP_QOS ) {
		CDebug::puts(_T("FD_GROUP_QOS - socket group QOS changes"));
	}
	if( events.lNetworkEvents & FD_QOS ) {
		CDebug::puts(_T("FD_QOS - socket QOS changes"));
	}
	if( events.lNetworkEvents & FD_OOB ) {
		CDebug::puts(_T("FD_OOB - arrival of OOB (out of band) data"));
	}
	if( events.lNetworkEvents & FD_READ ) {
		CDebug::puts(_T("FD_READ - readiness for reading"));
	}
	if( events.lNetworkEvents & FD_WRITE ) {
		CDebug::puts(_T("FD_WRITE - readiness for writing"));
	}
	if( events.lNetworkEvents & FD_CONNECT ) {
		CDebug::puts(_T("FD_CONNECT - completed connection or multipoint join operation"));
	}
	if( events.lNetworkEvents & FD_CLOSE ) {
		CDebug::puts(_T("FD_CLOSE - socket closure"));
	}
	if( events.lNetworkEvents & FD_ROUTING_INTERFACE_CHANGE ) {
		CDebug::puts(_T("FD_ROUTING_INTERFACE_CHANGE - routing interface changes for the specified destination"));
	}
	CDebug::puts(_T("------------------------------"));

#endif
}