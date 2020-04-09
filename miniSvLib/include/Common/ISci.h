///
/// @file       ISci.h
///
/// @brief      �V���A���ʐM���ʃC���^�[�t�F�[�X��`
///
/// @author     iwakura
///

#ifndef INCLUDED_ISCI_H
#define INCLUDED_ISCI_H

#include "Common/IConnection.h"

enum PARITY 
{
	SCI_PARITY_NONE    = 0,   //!< �p���e�B�Ȃ�
	SCI_PARITY_ODD     = 1,   //!< ��p���e�B
	SCI_PARITY_EVEN    = 2,   //!< �����p���e�B
	SCI_PARITY_DEFAULT = 255, //!< �I�u�W�F�N�g�̋K��l
};

enum STOPBIT
{
	SCI_STOPBIT_ONE     = 0,   //!< 1�X�g�b�v�r�b�g
	SCI_STOPBIT_ONE5    = 1,   //!< 1.5�X�g�b�v�r�b�g
	SCI_STOPBIT_TWO     = 2,   //!< 2�X�g�b�v�r�b�g
	SCI_STOPBIT_DEFAULT = 255, //!< �I�u�W�F�N�g�̋K��l
};

//*******************************************************************************
/// @brief  �V���A���ʐM���ʃC���^�[�t�F�[�X
//*******************************************************************************
class ISci : public IConnection
{
public:
	virtual ~ISci() {}

	virtual int     GetPortNumber() const = 0;

	virtual CRESULT SetBaudrate(_UDWORD Baudrate) = 0; // Open���Ă���ԂɕύX���邱�Ƃ͂ł��Ȃ�
	virtual CRESULT SetParity(PARITY Parity)      = 0; // Open���Ă���ԂɕύX���邱�Ƃ͂ł��Ȃ�
	virtual CRESULT SetStopBits(STOPBIT StopBits) = 0; // Open���Ă���ԂɕύX���邱�Ƃ͂ł��Ȃ�
	
	virtual _UDWORD GetParityErrors() const = 0;
	virtual _UDWORD GetFramingErrors() const = 0;
	virtual _UDWORD GetOverrunErrors() const = 0;
};

#endif