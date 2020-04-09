//
//		cpuinfo.h
//
//		Win32��p �n�[�h�E�F�A��`�t�@�C��
//
//		'09/12/21	Ver 0.01	�Ƃ肠�������� by iwakura
//
//		�n�[�h�E�F�A���ς��Ȃ�����ύX����Ȃ�CPU�ŗL�̏��������ɋL�q����
//
//

#ifndef INCLUDED_CPUINFO_H
#define INCLUDED_CPUINFO_H

// �G���f�B�A���֘A�̒�`
#define BIG_ENDIAN        0 // BYTEORDER �̂��߂̒�`
#define LITTLE_ENDIAN     1 // BYTEORDER �̂��߂̒�`
#define SELECTABLE_ENDIAN 2 // BYTEORDER �̂��߂̒�`�i�\��D�{���Ɏg�����͖���j

#ifndef BYTEORDER                // CPU�ŗL���
#define BYTEORDER  LITTLE_ENDIAN // Intel x86
#endif

#endif