#include "ConnectionCreator.h"
#include "McuLib/SCI.h"

///
/// @brief  ���̋@�\�������Ȃ��V���A���ʐM�N���X
///
class CSciNull : public ISci
{
	virtual CRESULT SetBaudrate(_UDWORD Baudrate) { return C_FAIL; }
	virtual CRESULT SetParity(PARITY Parity)      { return C_FAIL; }
	virtual CRESULT SetStopBits(STOPBIT StopBits) { return C_FAIL; }

	virtual CRESULT Open()  { return C_FAIL; }
	virtual void    Close() {}
	
	virtual bool    IsEnable() const { return true; }

	// �������i������COM�|�[�g���I�[�v���j
	virtual CRESULT Initialize(_UDWORD Baudrate, _UBYTE Parity, _UBYTE StopBits) { return C_FAIL; }

	// �f�[�^���M�֐�
	virtual CRESULT WriteElement(_UBYTE chr)                   { return C_FAIL; }
	virtual CRESULT WriteString(const _UBYTE* ptr, _UWORD len) { return C_FAIL; }

	// �f�[�^��M�֐�
	virtual bool    IsReceiveData()          { return false; }
	virtual CRESULT ReadElement(_UBYTE* ptr) { return C_FAIL; }

	virtual _UDWORD IsTransmittable(void)    { return 0; }

	virtual int     GetPortNumber() const    { return -1; }

	// ���v���
	virtual _UDWORD GetTxCount() const       { return 0; }
	virtual _UDWORD GetRxCount() const       { return 0; }
	virtual _UDWORD GetParityErrors() const  { return 0; }
	virtual _UDWORD GetFramingErrors() const { return 0; }
	virtual _UDWORD GetOverrunErrors() const { return 0; }
};


static CSciNull SciNull; // �G���[����NULL�|�C���^��Ԃ��Ȃ����Ƃ�ۏႷ�邽�߂̃C���X�^���X

ISci* ConnectionCreator::Create(const PSci& param)
{
	ISci* pSci;
	
	switch( param.Port ) 
	{
	case 0:
		pSci = new CSci0;
		break;
	case 1:
		pSci = new CSci1;
		break;
	case 2:
		pSci = new CSci2;
		break;
	case 3:
		pSci = new CSci3;
		break;
	case 4:
		pSci = new CSci4;
		break;
	default:
		pSci = &SciNull;
		break;
	}

	if (!isAllocationOk(pSci)) pSci = &SciNull;

	pSci->SetBaudrate(param.Baudrate);
	pSci->SetParity(param.Parity);
	pSci->SetStopBits(param.StopBits);

	return pSci;
}

ISci* ConnectionCreator::Create(const PSciNull& param)
{
	return &SciNull;
}

IUdp* ConnectionCreator::Create(const PUdp& param)
{
	return NULL;
}

void ConnectionCreator::Release(IConnection* pConnection)
{
	if (pConnection == &SciNull) return; // delete���Ȃ�

	if (pConnection) delete pConnection;
}