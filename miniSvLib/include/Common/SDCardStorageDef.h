// 201412xx TCS.Y.Nakamura ADD_S
#ifndef INCLUDED_SDCARDSTORAGEDEF_H
#define INCLUDED_SDCARDSTORAGEDEF_H

#include "Common/StandardDef.h"

// 201506xx TCS.Y.Nakamura ADD_S [�t�@�C���]���@�\�g���Ή�]
// Lib/SDCard.h����ړ����܂���.
#define FILE_NAME_LEN	11	// �t�@�C�����̍ő�T�C�Y�i�P�ʂ̓o�C�g�j
// �������ł͒��l�g���Ă��̂ňꉞ��`.�K�v�ȉӏ��Ŏg�p���ĉ�����.�����ӏ��͏C�����܂���.
#define FILE_NAME_LEN_L	8	// 8:3�`���̍���
#define FILE_NAME_LEN_R	3	// 8:3�`���̉E��
// 201506xx TCS.Y.Nakamura ADD_E

typedef struct _tag_SDCardFileInfo{		// SD�t�@�C�����(20)
// 201506xx TCS.Y.Nakamura MOD_S [�t�@�C���]���@�\�g���Ή�]
	_UBYTE	DirName[FILE_NAME_LEN];		// �t�@�C����(11)
// 201506xx TCS.Y.Nakamura MOD_E
	_UBYTE	DirAttr;					// ����(1)
	_UWORD	DirCrtTime;					// �t�@�C���쐬����(2)
	_UWORD	DirCrtDate;					// �t�@�C���쐬���t(2)
	_UWORD	DirWrtTime;					// �Ō�Ƀt�@�C���쐬��������(2)
	_UWORD	DirWrtDate;					// �Ō�Ƀt�@�C���쐬�������t(2)
} SD_FILE_INFO_T;

#endif	// INCLUDED_SDCARDSTORAGEDEF_H
// 201412xx TCS.Y.Nakamura ADD_E
