#include "WirelessDevice_FEP.h"
#include "Common/SystemTime.h"
#include "Common/ConnectionCreator.h"
#include <string.h>

//!< �R�}���h���g���C��
#define N_COMMAND_RETRY 5


//!< �R�}���h���̃V���A���ʐM����
#define COMMANDCOMMUNICATIONTIME (12.0f * 1000.0f * 10.0f / DEFAULT_BAUDRATE)

//!< 1�o�C�g������̖����ʐM���� + �V���A���ʐM����
#define COMMUNICATIONTIMEPERBYTE (0.16f + 1000.0f * 10.0f / DEFAULT_BAUDRATE)   

//!< �Œ�ʐM��������
#define CONSTANTCOMMUNICATIONTIME (33.0f + 2.0f + 21.0f + 2.0f)
//#define CONSTANTCOMMUNICATIONTIME 33.0f

//!< �ŏ��̋x�~���� + �R�}���h���̃V���A���ʐM����
#define MINIMUMCOMMUNICATIONTIME (51.0f + 12 * 1000.0f * 10.0f / DEFAULT_BAUDRATE)

enum
{
	DEFAULT_BAUDRATE     = 115200,
	WIRELESSPACKET_LIMIT = 128, //!< ��̖����p�P�b�g�ő����f�[�^�̍ő�l
};

enum FLAGS
{
	FLAG_READY   = 1, //!< �f�o�C�X���p����OK
	FLAG_TX_BUSY = 2, //!< ���M�r�W�[���
};

enum
{
	INITSEQ_START,     //!< �����t�F�[�Y
	INITSEQ_REG_CHECK, //!< ���W�X�^�`�F�b�N
	INITSEQ_REG_SET,   //!< ���W�X�^�Đݒ�
	INITSEQ_END,       //!< ����
};

enum RECEIVE_MODE
{
	RXMODE_SLEEP,
	RXMODE_RECEIVE_WAITING,     //!< �擪����'R'�̎�M�҂����
	RXMODE_HEADER_DECODING,     //!< RBN����M��
	RXMODE_HEADER_DECODING2,    //!< TBN����M��
	RXMODE_ADDR_DECODING,       //!< �A�h���X��(***)��M��
	RXMODE_ADDR_DECODING2,      //!< �A�h���X��(TBN��̎��g�̃A�h���X)��M��
	RXMODE_DATALENGTH_DECODING, //!< �f�[�^��(###)��M��
	RXMODE_PAYLOAD_DECODING,    //!< �o�C�i���f�[�^����M��
	RXMODE_FOOTER_DECODING,     //!< �Ō�̕�������M��
	RXMODE_RSSIDATA_DECODING,   //!< [�d��]����M��
	RXMODE_COMMAND_ARG,
	RXMODE_COMMAND_ARG_RESPONCE_HEADER_DECODING,  //!< @ARG�R�}���h�ɑ΂��鉞���f�[�^�҂�
	RXMODE_COMMAND_ARG_RESPONCE_REGDATA_DECODING,
};


// �C�����C���錾���w�b�_�ł��Ȃ����߂̂������}�N��
#define _IsRxOk() (m_Flags & FLAG_READY)
#define _IsTxOk() ((m_Flags & FLAG_READY) && !(m_Flags & FLAG_TX_BUSY))


// 2������16�i���𐔒l�ɕϊ�����֐�
static bool _xtoi(const _UBYTE c[2], _UBYTE* pReturn)
{
	*pReturn = 0;

	if (c[0] - '0' <= 9)
	{
		*pReturn += (c[0] - '0') * 16;
	}
	else if (c[0] - 'A' <= 5)
	{
		*pReturn += (c[0] - 'A' + 10) * 16;
	}
	else
	{
		return false;
	}

	if (c[1] - '0' <= 9)
	{
		*pReturn += (c[1] - '0');
	}
	else if (c[1] - 'A' <= 5)
	{
		*pReturn += (c[1] - 'A' + 10);
	}
	else
	{
		return false;
	}

	return true;
}

// ���l��������3�����̕�����ɕϊ�����֐�
static void _atoi128(unsigned char n, unsigned char ptr[3])
{
	// x��0�`128�͈̔͂ł���΁Ay = int(103 * x) / 1024 �� y = x / 10 �̌��ʂɓ�����
	unsigned char m;

	m = (103 * (unsigned short)n) >> 10;
	ptr[2] = n - m * 10 + '0';
	n = (103 * (unsigned short)m) >> 10;
	ptr[1] = m - n * 10 + '0';
	m = (103 * (unsigned short)n) >> 10;
	ptr[0] = n - m * 10 + '0';
}

static void _atoi256(unsigned char n, unsigned char ptr[3])
{
	// x��0�`256�͈̔͂ł���΁Ay = int(51 * (x+1)) / 512 �� y = x / 10 �̌��ʂɓ�����
	unsigned char m;

	m = (51 * (unsigned short)(n + 1)) >> 9;
	ptr[2] = n - m * 10 + '0';
	n = (51 * (unsigned short)(m + 1)) >> 9;
	ptr[1] = m - n * 10 + '0';
	m = (51 * (unsigned short)(n + 1)) >> 9;
	ptr[0] = n - m * 10 + '0';
}

Wireless_FEP::Wireless_FEP(_UBYTE PortNo) : WirelessDevice(PortNo), m_TempBuffer(WIRELESSPACKET_LIMIT)
{
	m_pSci = ConnectionCreator::Create(PSci(PortNo)); // �C���X�^���X�����ۏ�

	m_Flags = 0;

	memset(&m_Initializer, 0, sizeof(m_Initializer));
	m_Initializer.InitSequaence = INITSEQ_START;

	m_TimeNextSend = 0;

	m_TempBufferCount = 0;
	m_ReceiveMode = RXMODE_SLEEP;
	m_ReceiveStateCount = 0;
	m_IsReceived = false;
	m_Rssi = 0;
}

Wireless_FEP::~Wireless_FEP() 
{
	ConnectionCreator::Release(m_pSci);
}

CRESULT Wireless_FEP::Initialize(_UDWORD Baudrate)
{
	if (Baudrate == 0) Baudrate = DEFAULT_BAUDRATE;

	CRESULT res = C_OK;

	if (ISOK(res)) res = m_pSci->SetBaudrate(Baudrate);
	if (ISOK(res)) res = m_pSci->SetParity(SCI_PARITY_NONE);
	if (ISOK(res)) res = m_pSci->SetStopBits(SCI_STOPBIT_ONE);
	if (ISOK(res)) res = m_pSci->Open();

	return res;
}

void Wireless_FEP::Update()
{
	// ���������������ꍇ
	//if (m_pSci->) return;

	// �f�o�C�X�̗��p�������܂��������ꍇ�A�f�o�C�X�𗘗p�\�ɂ��鏈�������s����
	if ((m_Flags & FLAG_READY) == 0)
	{
		_InitSequence();
		return;
	}

	// �f�o�C�X�̗��p�����������Ă���ꍇ�A
#if 0
	if ((_SWORD)(m_TimeNextSend - ::GetSystemTimeShort()) <= 0)
	{
		// �r�W�[���Ԍo�߁I

		// ���M�r�W�[��Ԃ̉���
		_ResetBusy();
	}
#else
	_UWORD nowTime = ::GetSystemTimeShort();
	_UWORD diffTime;
	
	if (nowTime <= m_TimeNextSend)
	{
		diffTime = m_TimeNextSend - nowTime;

		if (diffTime <= 0x7FFF)
		{
			if (nowTime == m_TimeNextSend)
			{
				// �r�W�[���Ԍo�߁I

				// ���M�r�W�[��Ԃ̉���
				_ResetBusy();
			}
		}
		else
		{
			// 0x10000�܂���
			// �r�W�[���Ԍo�߁I

			// ���M�r�W�[��Ԃ̉���
			_ResetBusy();
		}
	}
	else
	{
		diffTime = nowTime - m_TimeNextSend;

		if (diffTime <= 0x7FFF)
		{
			// �r�W�[���Ԍo�߁I

			// ���M�r�W�[��Ԃ̉���
			_ResetBusy();
		}
		else
		{
			// 0x10000�܂���
			// �r�W�[���Ԗ��o��

		}
	}
#endif
}

bool Wireless_FEP::IsReady()
{
	return m_Flags & FLAG_READY;
}

bool Wireless_FEP::IsTxBusy()
{
	return !_IsTxOk();
}


CRESULT Wireless_FEP::SendBegin(size_t TotalLength)
{
	// ���������������ꍇ
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_BUSY;

	if (m_TempBufferCount + TotalLength > WIRELESSPACKET_LIMIT) return C_OUTOFBUFFER;

	return C_OK;
}

CRESULT Wireless_FEP::Send(_UBYTE data)
{
	// ���������������ꍇ
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_FAIL;

	m_TempBuffer[m_TempBufferCount++] = data;

	return C_OK;
}

CRESULT Wireless_FEP::Send(const void* pData, size_t length)
{
	// ���������������ꍇ
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_FAIL;

	const _UBYTE* pSrc = reinterpret_cast<const _UBYTE*>(pData);
	const _UBYTE* pEnd = pSrc + length;
	_UBYTE* pDest = &m_TempBuffer[m_TempBufferCount];

	while (pSrc < pEnd)
	{
		*pDest++ = *pSrc++;
 	}

	m_TempBufferCount += length;

	return C_OK;
}

CRESULT Wireless_FEP::SendEnd()
{
	// ���������������ꍇ
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_FAIL;

	return C_OK;
}

CRESULT Wireless_FEP::Flush()
{
	// ���������������ꍇ
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsTxOk()) return C_FAIL;

	if (m_TempBufferCount)
	{
		// ���M�o�C�g���𕶎���ɕϊ�����
		_UBYTE len[3];
		_atoi128(m_TempBufferCount, len);

		// �V���A���ʐM�f�[�^�o��
		m_pSci->WriteString((const _UBYTE*)"@TBN", 4);
		m_pSci->WriteString(m_DestAddr, 3);
		m_pSci->WriteString(len, 3);
		m_pSci->WriteString(&m_TempBuffer[0], m_TempBufferCount);
		m_pSci->WriteString((const _UBYTE*)"\r\n", 2);

		// ���M�r�W�[��Ԃɐݒ�
		_SetBusy(m_TempBufferCount);
		
		m_TempBufferCount = 0; // �o�b�t�@�N���A
	}

	return C_OK;
}
	
CRESULT Wireless_FEP::Receive(_UBYTE* pData)
{
	// ���������������ꍇ
	//if (!IsInitSci()) return C_FAIL;

	if (!_IsRxOk()) return C_FAIL;

	_Rx(); // m_ReceiveData �� m_IsReceived �̏�Ԃ��X�V����

	if (m_IsReceived)
	{
		*pData = m_ReceiveData;
		m_IsReceived = false;

		return C_OK;
	}
	else
	{
		return C_FAIL;
	}
}
	
bool Wireless_FEP::IsDataReceived()
{
	// ���������������ꍇ
	//if (!IsInitSci()) return false;

	if (!_IsRxOk()) return false;

	_Rx(); // m_ReceiveData �� m_IsReceived �̏�Ԃ��X�V����

	return m_IsReceived;
}
	
void Wireless_FEP::ProcessUnusedCharacter(_UBYTE c)
{
}
	
CRESULT Wireless_FEP::GetRssi(_UBYTE* pRssi) const
{
	if (m_Rssi == 0)
	{
		return C_NODATA;
	}
	else
	{
		*pRssi = m_Rssi;
		return C_OK;
	}
}

_UWORD Wireless_FEP::GetRoundTripTime(size_t TxBytes, size_t RxBytes) const
{
	if (TxBytes > WIRELESSPACKET_LIMIT) TxBytes = WIRELESSPACKET_LIMIT;
	if (RxBytes > WIRELESSPACKET_LIMIT) RxBytes = WIRELESSPACKET_LIMIT;

	return _GetBusyTime(TxBytes) + _GetBusyTime(RxBytes);
}

void Wireless_FEP::_InitSequence()
{
	// ���p�������ł��Ă��Ȃ�

	_RxCommandResult();

	if (m_Initializer.InitSequaence == INITSEQ_START)
	{
		//----------------------------------
		// �����@ ���W�X�^�f�[�^�S�擾
		//----------------------------------
		if (m_ReceiveMode == RXMODE_SLEEP && m_Initializer.RegisterReadCount == 0)
		{
			// ���W�X�^�f�[�^���擾
			m_pSci->WriteString((const _UBYTE*)"@ARG\r\n", 6);
			m_ReceiveMode = RXMODE_COMMAND_ARG;
			m_Initializer.TimeoutCounter = 25;
		}
		else if (m_Initializer.RegisterReadCount == Wireless_FEP_Initializer::N_REGISTER)
		{
			// ���W�X�^�f�[�^�擾����
			//m_Initializer.InitSequaence = INITSEQ_REG_CHECK;
			m_Initializer.InitSequaence = INITSEQ_END;
			m_Initializer.RetryCounter = 0;

			_atoi256(m_Initializer.Reg[2], m_DestAddr); // ���M��̃A�h���X���擾
		}

		if (m_Initializer.TimeoutCounter == 0)
		{
			if (m_Initializer.RetryCounter < N_COMMAND_RETRY)
			{
				// �^�C���A�E�g �� ���Ȃ���
				m_ReceiveMode = RXMODE_SLEEP;
				m_Initializer.RegisterReadCount = 0;
				m_Initializer.RetryCounter++;
			}
			else
			{
				// ���g���C�A�E�g
				m_Initializer.InitSequaence = INITSEQ_END;
				m_Initializer.RetryCounter = 0;

				m_DestAddr[0] = '9'; // �ݒ�s�\�ȃA�h���X�𑗐M��ɂ���
				m_DestAddr[1] = '9';
				m_DestAddr[2] = '9';
			}
		}
		else
		{
			m_Initializer.TimeoutCounter--;
		}
	}

	if (m_Initializer.InitSequaence == INITSEQ_REG_CHECK)
	{
		//----------------------------------
		// �����A ���W�X�^�f�[�^�`�F�b�N
		//----------------------------------

		if (m_ReceiveMode == RXMODE_SLEEP)
		{
			if (m_Initializer.Reg[13] != 0x81)
			{
				m_pSci->WriteString((const _UBYTE*)"@REG13:81H\r\n", 12);

			}
		}

		//m_Initializer.RegisterVal[2]
	}

	if (m_Initializer.InitSequaence == INITSEQ_REG_SET)
	{
		//----------------------------------
		// �����B ���W�X�^�Đݒ�
		//----------------------------------

	}

	if (m_Initializer.InitSequaence == INITSEQ_END)
	{
		//----------------------------------
		// �����C ����������
		//----------------------------------

		// delete pInitializer;

		m_Flags |= FLAG_READY | FLAG_TX_BUSY;
		m_ReceiveMode = RXMODE_RECEIVE_WAITING;
		m_TimeNextSend = ::GetSystemTimeShort() + _GetBusyTime(0);
	}
}

void Wireless_FEP::_Rx()
{
	_UBYTE c;

	if (m_IsReceived) return;

	// �v���g�R�����̃f�[�^���ǂݏo����܂ŌJ��Ԃ��B
	// �ǂݏo���Ȃ������� false ��Ԃ�
	while (ISOK(m_pSci->ReadElement(&c)))
	{
		// 1�o�C�g��M

		if (m_ReceiveMode == RXMODE_PAYLOAD_DECODING)
		{
			//----------------------------------
			// �D ��M�f�[�^�{�̎�M���c
			//----------------------------------
			m_ReceiveData = c;
			m_IsReceived = true;
			m_ReceiveLength--;

			if (m_ReceiveLength == 0)
			{
				m_ReceiveMode = RXMODE_FOOTER_DECODING;
			}

			// �v���g�R�����̃f�[�^����M�ł����I
			return;
		}
		else if (m_ReceiveMode == RXMODE_RECEIVE_WAITING)
		{
			//----------------------------------
			// �@ ��M�f�[�^�҂����
			// 'R'     - RBN***###[�o�C�i���f�[�^][�d��]<CR><LF> �� 1�o�C�g�ڂ���M�H
			// 'T'     - @TBN999###[�o�C�i���f�[�^][�d��]<CR><LF> �� 2�o�C�g�ڂ���M�H�i���ڐڑ����j
			// ��L�ȊO - �S�~�Ƃ݂Ȃ�
			//----------------------------------
			if (c == 'R') 
			{
				m_ReceiveMode = RXMODE_HEADER_DECODING;
				m_ReceiveStateCount = 0;
				m_ReceiveStringChecker = 0;
			}
			else if (c == 'T')
			{
				// ���ڐڑ������Ƃ��ɑΉ����邽�߂̃R�[�h
				m_ReceiveMode = RXMODE_HEADER_DECODING2;
				m_ReceiveStateCount = 0;
				m_ReceiveStringChecker = 0;
			}
		}
		else if (m_ReceiveMode == RXMODE_HEADER_DECODING || m_ReceiveMode == RXMODE_HEADER_DECODING2)
		{
			//----------------------------------
			// �A �w�b�_"RBN"��"BN"������M���c�ior "TBN"��"BN"�����j
			//----------------------------------
			m_ReceiveStringChecker <<= 8;
			m_ReceiveStringChecker |= c;
			m_ReceiveStateCount++;

			if (m_ReceiveStateCount == 2)
			{
				if (m_ReceiveStringChecker == (('B' << 8) | 'N'))
				{
					if (m_ReceiveMode == RXMODE_HEADER_DECODING)
					{
						// "RBN"��A���Ŏ�M����
						m_ReceiveMode = RXMODE_ADDR_DECODING;
						m_ReceiveStateCount = 0;
						m_ReceiveStringChecker = 0;
					}
					else
					{
						// "TBN"��A���Ŏ�M����
						m_ReceiveMode = RXMODE_ADDR_DECODING2;
						m_ReceiveStateCount = 0;
						m_ReceiveStringChecker = 0;
					}
				}
				else
				{
					// "RBN"�ł͂Ȃ�����
					m_ReceiveMode = RXMODE_RECEIVE_WAITING;
				}
			}
		}
		else if (m_ReceiveMode == RXMODE_ADDR_DECODING || m_ReceiveMode == RXMODE_ADDR_DECODING2)
		{
			//----------------------------------
			// �B ���M���A�h���X��M���c
			//----------------------------------
			m_ReceiveStringChecker <<= 8;
			m_ReceiveStringChecker |= c;
			m_ReceiveStateCount++;

			if (m_ReceiveStateCount == 3)
			{
				if ((m_ReceiveMode == RXMODE_ADDR_DECODING && m_ReceiveStringChecker == ((m_DestAddr[0] << 16) | (m_DestAddr[1] << 8) | m_DestAddr[2])) ||
					m_ReceiveMode == RXMODE_ADDR_DECODING2)
				{
					// �A�h���X�����M��̃f�o�C�X�̃A�h���X�ƈ�v�����ior TBN���3�o�C�g�̎�M�������j
					m_ReceiveMode = RXMODE_DATALENGTH_DECODING;
					m_ReceiveStateCount = 0;
					m_ReceiveLength = 0;
				}
				else
				{
					// �A�h���X�s��v
					m_ReceiveMode = RXMODE_RECEIVE_WAITING;
				}
			}
		}
		else if (m_ReceiveMode == RXMODE_DATALENGTH_DECODING)
		{
			//----------------------------------
			// �C �o�C�g����M���c
			//----------------------------------
			m_ReceiveLength *= 10;
			m_ReceiveLength += c - '0';
			m_ReceiveStateCount++;

			if (m_ReceiveStateCount == 3)
			{
				if (m_ReceiveLength != 0 && m_ReceiveLength <= 128)
				{
					// �o�C�g���f�[�^�f�R�[�h����
					m_ReceiveMode = RXMODE_PAYLOAD_DECODING;
					m_ReceiveStateCount = 0;
				}
				else
				{
					// �s���Ȓl
					m_ReceiveMode = RXMODE_RECEIVE_WAITING;
				}
			}
		}
		else if (m_ReceiveMode == RXMODE_FOOTER_DECODING)
		{
			//----------------------------------
			// �E �t�b�^��M���c
			// ���̂����ꂩ
			// <CR><LF>
			// [�d��]<CR><LF>
			//----------------------------------
			if (static_cast<_UBYTE>(c - '0') <= 1) // 1�����ڂ�'0' �� '1' �������Ȃ�
			{
				// [�d��]<CR><LF> �̃p�^�[��
				m_RssiDataTemp = c - '0';
				m_ReceiveMode = RXMODE_RSSIDATA_DECODING;
				m_ReceiveStateCount = 1;
			}
			else
			{
				// <CR><LF> �̃p�^�[�� �������� ��O
				m_ReceiveMode = RXMODE_RECEIVE_WAITING;
			}
		}
		else if (m_ReceiveMode == RXMODE_RSSIDATA_DECODING)
		{
			//----------------------------------
			// �F�d���f�[�^��M���c
			//----------------------------------
			if (static_cast<_UBYTE>(c - '0') <= 9) // '0' �` '9'
			{
				m_RssiDataTemp *= 10;
				m_RssiDataTemp += c - '0';
				m_ReceiveStateCount++;
			}
			else
			{
				// ��O
				m_ReceiveMode = RXMODE_RECEIVE_WAITING;
			}

			if (m_ReceiveStateCount == 3)
			{
				m_Rssi = m_RssiDataTemp;
				m_ReceiveMode = RXMODE_RECEIVE_WAITING;
			}
		}
	}

	// �ǂݏo���Ȃ������ꍇ
	return;
}


void Wireless_FEP::_RxCommandResult()
{
	_UBYTE c;

	while (ISOK(m_pSci->ReadElement(&c)))
	{
		if (m_ReceiveMode == RXMODE_COMMAND_ARG)
		{
			//----------------------------------
			// �����@-(1) �����l��ݒ肵�Ă�������
			//----------------------------------
			m_ReceiveMode = RXMODE_COMMAND_ARG_RESPONCE_HEADER_DECODING;
			m_ReceiveStateCount = 0;
			m_ReceiveStringChecker = 0;
			m_Initializer.RegisterReadCount = 0;
		}


		if (m_ReceiveMode == RXMODE_COMMAND_ARG_RESPONCE_HEADER_DECODING)
		{
			//----------------------------------
			// �����@-(2) @ARG�R�}���h�̉����iREG##:$$H<CR><LF> �~29�s ��"REG"���j����M���c
			//----------------------------------
			m_ReceiveStringChecker <<= 8;
			m_ReceiveStringChecker |= c;

			if ((m_ReceiveStringChecker & 0xFFFFFF) == (('R' << 16) | ('E' << 8) | 'G'))
			{
				m_ReceiveMode = RXMODE_COMMAND_ARG_RESPONCE_REGDATA_DECODING;
				m_ReceiveStateCount = 0;
			}
		}
		else if (m_ReceiveMode == RXMODE_COMMAND_ARG_RESPONCE_REGDATA_DECODING)
		{
			//----------------------------------
			// �����@-(3) @ARG�R�}���h�̉����iREG##:$$H<CR><LF> �~29�s ��"##:$$H"<CR><LF>���j����M���c
			//----------------------------------
			m_Initializer.Workspace[m_ReceiveStateCount++] = c;

			if (m_ReceiveStateCount == 8)
			{
				_UBYTE RegNo = (m_Initializer.Workspace[0] - '0') * 10 + (m_Initializer.Workspace[1] - '0'); // 4, 5�����ڂ̓��W�X�^�ԍ�
				_UBYTE Value;

				// �t�H�[�}�b�g�`�F�b�N
				if (m_Initializer.Workspace[2] == ':' &&
					m_Initializer.Workspace[5] == 'H' &&
					m_Initializer.Workspace[6] == '\r' &&
					m_Initializer.Workspace[7] == '\n' &&
					RegNo == m_Initializer.RegisterReadCount &&
					_xtoi(&m_Initializer.Workspace[3], &Value)) // �� �����Ń��W�X�^�l�̃f�R�[�h������B7, 8�����ڂ����W�X�^�l
				{
					// ����OK

					m_Initializer.Reg[m_Initializer.RegisterReadCount++] = Value;

					if (m_Initializer.RegisterReadCount >= Wireless_FEP_Initializer::N_REGISTER)
					{
						// �S���W�X�^�擾����
						m_ReceiveMode = RXMODE_SLEEP;
					}
					else
					{
						// �܂��S���擾���Ă��Ȃ� �� ���̃��W�X�^��ǂ݂ɂ���
						m_ReceiveMode = RXMODE_COMMAND_ARG_RESPONCE_HEADER_DECODING;
						m_ReceiveStringChecker = 0;
					}
				}
				else
				{
					// ����NG �� ���Ȃ���(@ARG�R�}���h�đ�)

					m_Initializer.RegisterReadCount = 0;
					m_ReceiveMode = RXMODE_SLEEP;
				}
			}
		}
	}
}

void Wireless_FEP::_SetBusy(_UWORD length)
{
	_UWORD BusyTime = _GetBusyTime(length);

	m_Flags |= FLAG_TX_BUSY;
	m_TimeNextSend = ::GetSystemTimeShort() + BusyTime;
}

void Wireless_FEP::_ResetBusy()
{
	m_Flags &= ~FLAG_TX_BUSY;
}

_UWORD Wireless_FEP::_GetBusyTime(_UWORD length) const
{
	float time = CONSTANTCOMMUNICATIONTIME + COMMANDCOMMUNICATIONTIME + length * COMMUNICATIONTIMEPERBYTE;

	// time �̒l�͍ŏ��l�ȉ��ɂȂ�Ȃ�
	if (time < MINIMUMCOMMUNICATIONTIME) time = MINIMUMCOMMUNICATIONTIME;

	return -static_cast<_SWORD>(-time); // �����ɕϊ�����Ɠ����ɌJ�グ���s��
}