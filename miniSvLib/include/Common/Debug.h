///
/// @file       Debug.h
///
/// @brief      �R���\�[���f�o�b�O�p�@�\�iHEW�ŃR���p�C������Ƌ�̊֐��ɂȂ�܂��j
///

#ifndef INCLUDED_DEBUG_H
#define INCLUDED_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

// ��ʕ\���n�֐�
void debug_puts(char* string);
void debug_printf(char* pszFormat, ...);

// ��ʏ���
void debug_cls(); // �x���̂Ŏg�����舫��

#ifdef __cplusplus
}
#endif

#endif