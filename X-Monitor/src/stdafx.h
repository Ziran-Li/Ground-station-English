
// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �ꕔ�� CString �R���X�g���N�^�[�͖����I�ł��B

// ��ʓI�Ŗ������Ă����S�� MFC �̌x�����b�Z�[�W�̈ꕔ�̔�\�����������܂��B
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �̃R�A����ѕW���R���|�[�l���g
#include <afxext.h>         // MFC �̊g������





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �R���� �R���g���[�� �T�|�[�g
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �R���� �R���g���[�� �T�|�[�g
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC �ɂ����郊�{���ƃR���g���[�� �o�[�̃T�|�[�g



//*******************************************************************************
// ���[�U�[��`���b�Z�[�W
//*******************************************************************************
#define WM_UPDATE_DIALOGVIEW    (WM_APP + 1)    // �ĕ`�惁�b�Z�[�W (wParam : ���b�Z�[�W�̊Ԋu[ms])
#define WM_UPDATE_CONTROL       (WM_APP + 2)    // �R���g���[���̐ݒ�X�V���b�Z�[�W
#define WM_APPEND_LOG           (WM_APP + 3)    // ���C���E�B���h�E�̃��O�ɕ������\�����邽�߂̃��b�Z�[�W�i������́CtheApp �̃L���[�ɕۑ����Ď󂯓n������D�j
#define WM_DESTROY_MODELESS     (WM_APP + 4)    // �q�̃��[�h���X�_�C�A���O���j�����ꂽ�Ƃ��ɐe�E�B���h�E�ɒʒm���郁�b�Z�[�W (wParam : �j�����ꂽ�_�C�A���O�̃|�C���^)
#define WM_SERVER_DATA_RECEIVE  (WM_APP + 5)    // X-Monitor Server ���f�[�^�̎�M���X���b�h�ɒʒm���邽�߂Ɏg�p���郁�b�Z�[�W (wParam : ��M�����f�[�^�̎��)
#define WM_CLIENT_ACCEPT        (WM_APP + 6)    // X-Monitor Server ���N���C�A���g�̐ڑ����X���b�h�ɒʒm���邽�߂Ɏg�p���郁�b�Z�[�W (wParam : IP�A�h���X, lParam : ���v)
#define WM_CLIENT_CLOSE         (WM_APP + 7)    // X-Monitor Server ���N���C�A���g�̐ؒf���X���b�h�ɒʒm���邽�߂Ɏg�p���郁�b�Z�[�W (wParam : IP�A�h���X, lParam : ���v)
#define WM_NOTIFY_SYSTEM_RESUME (WM_APP + 8)    // �V�X�e���̍ĊJ���X���b�h�ɒʒm���邽�߂Ɏg�p���郁�b�Z�[�W
#define WM_DIALOG_MESSAGE       (WM_APP + 1000) // ����ȍ~�́C�e�_�C�A���O���Œ�`����郁�b�Z�[�W�Ƃ���







