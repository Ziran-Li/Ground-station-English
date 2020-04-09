#ifndef _INCLUDED_FILE_TRANSFER_STORAGE_DEF_H_
#define _INCLUDED_FILE_TRANSFER_STORAGE_DEF_H_

#include "Common/StandardDef.h"			// ��{�f�[�^�^
#include "Common/SDCardStorageDef.h"	// SD�t�@�C�����^

//--------
//�萔��`
//--------
// �������ʃR�[�h
#define PARAM_RESULT_CODE_OK           (0x00) // ����
#define PARAM_RESULT_CODE_NG           (0xFF) // ���s
#define PARAM_RESULT_CODE_STATE_ERR    (0xFE) // ��Ԉُ�
#define PARAM_RESULT_CODE_PARAM_ERR    (0xFD) // �p�����[�^�ُ�
#define PARAM_RESULT_CODE_CRC_ERR      (0xFC) // CRC�ُ�
#define PARAM_RESULT_CODE_SDCARD_BUSY  (0xFB) // SD�J�[�h�r�W�[

#define TRANS_MODE_NORMAL              (0x00) // �f�t�H���g���[�h(�ʏ�^�p���)
#define TRANS_MODE_BOOT                (0x01) // �u�[�g���[�h

// CRC32�v�Z�֘A
#define CRC32_POLYNOMIAL               (0x04C11DB7UL)  // CRC32�v�Z�p(����������X32+X26+X23+X22+X16+X12+X11+X10+X8+X7+X5+X4+X2+X1+1)
#define CRC32_INITIAL_VAL              (0xFFFFFFFFUL)  // �����l
#define CRC32_OUT_XOR_VAL              (0xFFFFFFFFUL)  // �o��XOR

// ��M�f�[�^�Ǘ��r�b�g�}�b�v�֘A
#define DATA_BITMAP_SIZE               (32)    // ��M�f�[�^�Ǘ��r�b�g�}�b�v�̈�T�C�Y
#define DATA_BITMAP_VALID_VAL          (0xFF)  // ��M�ς݂�\��1byte�P�ʂ̐���l
#define DATA_BITMAP_DIV32_SHIFT        (5)     // 32���Z���̉E�V�t�g��
#define DIV32_MOD_MASK                 (0x1F)  // ��]�Z�o�p�}�X�N�l

// �f�[�^�T�C�Y�֘A
#define DATA_TRANS_USER_DATA_LEN       (32)    // �o�[�X�g�]���f�[�^��(�b��ŌŒ�l)
#define DATA_BLOCK_SIZE                (8192)  // �f�[�^�u���b�N�T�C�Y
#define VALID_DATA_LEN_MIN             (1)     // �L���f�[�^���ŏ��l(0�͋��e���Ȃ�)
//#define VALID_DATA_LEN_MAX           (536870912)  // �S�f�[�^���ő�T�C�Y(512MB)(�g���p)
#define VALID_DATA_LEN_MAX             (512*1024*1024)  // �S�f�[�^���ő�T�C�Y(512MB)(�g���p)

// �f�B���N�g���ړ��A�\���֘A��`
#define FILE_PATH_LENGTH               (256)  // �t�@�C���p�X�T�C�Y

#define STR_SPACE                      " "        // �X�y�[�X
#define STR_PERIOD                     "."        // �s���I�h
#define STR_UP                         "../"      // ���
#define STR_NULL                       "\0"       // �I�[����
#define STR_SLASH                      "/"        // �J�����g�t�H���_
#define STR_FIRMWARE                   "FIRMWARE" // �J�����g�t�H���_
#define STR_SYSTEM                     "SYSTEM"   // �J�����g�t�H���_
	

//-------------
// �d�l��`�֘A
//-------------

// RW�]�����
#define SPECDEF_TRANSFER_KIND_READ     (0x00)  // �ǂݍ���
#define	SPECDEF_TRANSFER_KIND_WRITE    (0x01)  // ��������

// 201506xx TCS.Y.Nakamura ADD_S [�t�@�C���]���@�\�g���Ή�]
// �p�P�b�g���M���(UAV -> GCS)
typedef enum
{
	SENDTYPE_UAV_NONE                    = 0x00, // ���M����
	SENDTYPE_UAV_MODE_CHANGE_RES         = 0x01, // �f�[�^�]�����[�h�ύX����
	SENDTYPE_UAV_START_NTF               = 0x02, // �f�[�^�]���J�n�ʒm
	SENDTYPE_UAV_STOP_NTF                = 0x03, // �f�[�^�]����~�ʒm
	SENDTYPE_UAV_MULTI_NTF               = 0x04, // �ėp�ʒm
	SENDTYPE_UAV_RESEND_REQ              = 0x05, // �f�[�^�đ��v��
	SENDTYPE_UAV_USERDATA                = 0x06, // ���[�U�[�f�[�^
	SENDTYPE_UAV_MULTI_REQ               = 0x07, // �ėp�v��
	SENDTYPE_UAV_UPDATE_FILE_LIST_RSP    = 0x08, // �t�@�C�����X�g�X�V����
	SENDTYPE_UAV_FILE_LIST_DATA          = 0x09, // �t�@�C�����X�g�f�[�^
	SENDTYPE_UAV_BUSY                    = 0xFE, // �r�W�[��
	SENDTYPE_UAV_INVALID                 = 0xFF, // �����l(��)
} SENDTYPE_UAV_E;

// �p�P�b�g���M���(UAV <- GCS)
typedef enum
{
	SENDTYPE_GCS_NONE                    = 0x00, // ���M����
	SENDTYPE_GCS_MODE_CHANGE_REQ         = 0x01, // �f�[�^�]�����[�h�ύX�v��
	SENDTYPE_GCS_START_REQ               = 0x02, // �f�[�^�]���J�n�v��
	SENDTYPE_GCS_STOP_REQ                = 0x03, // �f�[�^�]����~�v��
	SENDTYPE_GCS_MULTI_REQ               = 0x04, // �ėp�v��
	SENDTYPE_GCS_RESEND_REQ              = 0x05, // �f�[�^�đ��v��
	SENDTYPE_GCS_USERDATA                = 0x06, // ���[�U�[�f�[�^
	SENDTYPE_GCS_MULTI_NTF               = 0x07, // �ėp�ʒm
	SENDTYPE_GCS_UPDATE_FILE_LIST_REQ    = 0x08, // �t�@�C�����X�g�X�V�v��
	SENDTYPE_GCS_FILE_LIST_DATA_SEND_REQ = 0x09, // �t�@�C�����X�g�f�[�^���M�v��
	SENDTYPE_GCS_INVALID                 = 0xFF, // �����l(��)
} SENDTYPE_GCS_E;
// 201506xx TCS.Y.Nakamura ADD_E

#define SPECDEF_FILE_LIST_DATA_REQ             (0x01)  // �t�@�C�����X�g�f�[�^���M�v��
#define SPECDEF_FILE_LIST_DATA_NTF             (0x02)  // �t�@�C�����X�g�f�[�^���M�ʒm

// �R�}���h���
// �v���n(PC��UAV)
#define PARAM_CMD_KIND_START_BLOCK_SET_REQ       (0x01)  // �f�[�^�]���J�n�u���b�N�ݒ�v��
#define PARAM_CMD_KIND_COMP_QUERY_REQ            (0x02)  // �f�[�^�]�������₢���킹�v��
#define PARAM_CMD_KIND_SDCARD_WRITE_REQ          (0x03)  // SD�J�[�h�������ݗv��
#define PARAM_CMD_KIND_SDCARD_READ_REQ           (0x04)  // SD�J�[�h�ǂݍ��ݗv��
#define PARAM_CMD_KIND_SDDATA_SEND_REQ           (0x05)  // SD�f�[�^���M�v��
// �ʒm�n(UAV��PC)
#define PARAM_CMD_KIND_START_BLOCK_SET_COMP_NTF  (0x01)  // �f�[�^�]���J�n�u���b�N�ݒ芮���ʒm
#define PARAM_CMD_KIND_TRANS_COMP_NTF            (0x02)  // �f�[�^�]�������ʒm
#define PARAM_CMD_KIND_SDCARD_WRITE_COMP_NTF     (0x03)  // SD�J�[�h�������݊����ʒm
#define PARAM_CMD_KIND_SDCARD_READ_COMP_NTF      (0x04)  // SD�J�[�h�ǂݍ��݊����ʒm


//-------------
// ��M�f�[�^�^
//-------------
typedef struct
{
	_UBYTE					PacketKind;			// �p�P�b�g���
	void*					pData;				// �f�[�^�|�C���^
} FILE_TRANSFER_RX_PACKET_T;

//--------------------------------------
// �f�[�^�]�����[�h�ύX�v�����(�S1byte)
//--------------------------------------
typedef struct
{
	_UBYTE					Mode;				// �]�����[�h(1)	1:�u�[�g���[�h�A1�ȊO:����
} MODE_CHG_REQ_INFO_T;

//--------------------------------------
// �f�[�^�]�����[�h�ύX�������(�S1byte)
//--------------------------------------
typedef struct
{
	_UBYTE					Result;				// ����
} MODE_CHG_RES_INFO_T;

//---------------------------------
// �f�[�^�]���J�n�v�����(�S36byte)
//---------------------------------
typedef struct
{
	_UBYTE					RWKind;				// RW�v�����(1) �L���l:0,1
	_UDWORD					DataLen;			// �L���f�[�^��(4)	��Read����Don't Care
// 201506xx TCS.Y.Nakamura ADD_S [�t�@�C���]���@�\�g���Ή�]
	_UBYTE			DirName[FILE_NAME_LEN];	// �t�H���_��(11)
	_UDWORD			FileSize;		// �t�@�C���T�C�Y
// 201506xx TCS.Y.Nakamura ADD_E
	SD_FILE_INFO_T			stSdFileInfo;		// �t�@�C�����(20)	��Read����DirName�ȊODon't Care
} START_REQ_SFT_INFO_T;

//---------------------------------
// �f�[�^�]���J�n�ʒm���(�S26byte)
//---------------------------------
typedef struct
{
	_UBYTE					RWKind;				// RW�ʒm���(1) �L���l:0,1
	_UBYTE					Result;				// �f�[�^�]���J�n�v���ɑ΂��錋��(1)
	_UDWORD					DataLen;			// �L���f�[�^��(4)
	SD_FILE_INFO_T			stSdFileInfo;		// SD�t�@�C�����(20)
} START_NTF_SFT_INFO_T;
	
//----------------------
// �ėp�v�����(�S7byte)
//----------------------
typedef struct
{
	_UBYTE					CmdReqKind;			// �v�����(1) �L���l:1�`5
	_UWORD					BlockNo;			// �u���b�N�ԍ�(2) �L���l:0�`65535
	_UDWORD					CRC32Data;			// CRC32�f�[�^(4)
} MULTI_REQ_SFT_INFO_T;

//----------------------
// �ėp�ʒm���(�S4byte)
//----------------------
typedef struct
{
	_UBYTE					CmdNtfKind;			// �ʒm���(1) �L���l:1�`4
	_UBYTE					Result;				// �ʒm����(1)
	_UWORD					BlockNo;			// �u���b�N�ԍ�(2) �L���l:0�`65535
} MULTI_NTF_INFO_T;

//--------------------------------
// �f�[�^�]����~�v�����(�S1byte)
//--------------------------------
typedef struct
{
	_UBYTE					Flag;				// �f�[�^�]����~���[�h(1) ��~����:1
} STOP_REQ_SFT_INFO_T;

//--------------------------------
// �f�[�^�]����~�v�����(�S1byte)
//--------------------------------
typedef struct
{
	_UBYTE					Result;				// �f�[�^�]����~�v���ɑ΂��錋��(1)
} STOP_NTF_SFT_INFO_T;

//-----------------------------
// ���[�U�[�f�[�^���(�S33byte)
//-----------------------------
typedef struct
{
	_UBYTE					DataNo;				// �f�[�^�ԍ�(1) �L���l:0�`255
	_UBYTE	UserData[DATA_TRANS_USER_DATA_LEN];	// ���[�U�[�f�[�^�̈�(32)
} USER_DATA_SFT_INFO_T;

//-----------------------------
// �f�[�^�đ��v�����(�S32byte)
//-----------------------------
typedef struct
{
	_UBYTE	ResendData[DATA_TRANS_USER_DATA_LEN];	// �đ��v���f�[�^(32)
} RESEND_REQ_INFO_T;

// 201506xx TCS.Y.Nakamura ADD_S [�t�@�C���]���@�\�g���Ή�]
//---------------------------------
// �t�@�C�����X�g�X�V�v��(�S11byte)
//---------------------------------
typedef struct
{
	_UBYTE	CurDirName[FILE_NAME_LEN];	// �J�����g�f�B���N�g����
} UPDATE_FILE_LIST_REQ_INFO_T;

//--------------------------------
// �t�@�C�����X�g�X�V����(�S3byte)
//--------------------------------
typedef struct
{
	_UBYTE					Result;				// ����(1)
	_UWORD					nFileCnt;			// �t�@�C����(�t�H���_)���(2)
} UPDATE_FILE_LIST_RSP_INFO_T;

//-------------------------------------
// �t�@�C��(�t�H���_)�ŗL���(�S24byte)
//-------------------------------------
typedef struct
{
	_UDWORD					nSize;				// �t�@�C���T�C�Y(4)
	SD_FILE_INFO_T			stSdFileInfo;		// �t�@�C��(�t�H���_)���(20)
} FILE_UNIQUE_INFO_T;


//-------------------------------
// �t�@�C�����X�g�f�[�^(�S27byte)
//-------------------------------
typedef struct
{
	_UBYTE					nMode;				// ���[�h(1)
	_UWORD					nDataNo;			// �f�[�^�ԍ�(2)
	FILE_UNIQUE_INFO_T		stFileUniqueInfo;	// �t�@�C��(�t�H���_)�ŗL���(24)
} FILE_LIST_DATA_T;
// 201506xx TCS.Y.Nakamura ADD_E

#endif	// _INCLUDED_FILE_TRANSFER_STORAGE_DEF_H_
