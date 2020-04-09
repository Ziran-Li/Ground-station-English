///
///
/// @file       Udp.h
///
/// @brief      UDP�ʐM�N���X��`
///
/// @author     iwakura
///
///

#pragma once

#include <basetsd.h>
#include "Common/IUdp.h"

class Udp : public IUdp
{
	static _UDWORD s_Count;

	typedef UINT_PTR SOCKET;
	typedef struct sockaddr_in SOCKADDR_IN;

	SOCKET*      m_pSock;
	SOCKADDR_IN* m_pAddrIn;
	SOCKADDR_IN* m_pAddrOut;

	_UBYTE m_RxBuffer[65536];
	_UWORD m_RxReaded; // �ǂݎ��σo�C�g��
	_UWORD m_RxLength; // �S�o�C�g��

public:
	Udp(const char* HostName, _UWORD PortSend, _UWORD PortRecv);
	virtual ~Udp();

	// �ڑ��^�ؒf
	virtual CRESULT Open();
	virtual void    Close();

	// �ʐM�\���`�F�b�N
	virtual bool    IsEnable() const;

	// ��M�`�F�b�N
	virtual bool    IsReceiveData();

	// ���M�\�`�F�b�N
	virtual _UDWORD IsTransmittable(void);

	// ���o�͊֐�
	virtual	CRESULT WriteElement(_UBYTE chr);                   // 1Byte���M
	virtual	CRESULT ReadElement(_UBYTE* ptr);                   // 1Byte��M

	virtual CRESULT ReadString(void* ptr, _UWORD len);          // �A���f�[�^���M(UDP��p)
	virtual	CRESULT WriteString(const _UBYTE* ptr, _UWORD len); // �A���f�[�^���M

	// ���v���
	virtual _UDWORD GetTxCount() const { return 0; }
	virtual _UDWORD GetRxCount() const { return 0; }

private:
	void _Receive();
};