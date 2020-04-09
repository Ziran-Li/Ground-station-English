//
//		DataConv.h
//
//		�ʐM�p�f�[�^�ϊ��֐�
//
//		'08/10/08	Ver 1.00	����                                      by iwakura
//		'08/12/10	Ver 1.00.1	cpp�t�@�C���Ɉꕔ�ڍs                     by iwakura
//		'09/04/09	Ver 1.01	ConvRadTo2Byte, Dec2ByteToRad �֐���ǉ�  by iwakura
//		'12/05/03	Ver 1.02	Conv1To2Byte, Dec2ByteTo1 �֐���ǉ�      by iwakura
//
//

#ifndef INCLUDED_DATACONV_H
#define INCLUDED_DATACONV_H

#include "StandardDef.h"

struct PropoData;

//*******************************************************************************
//	�p�x�l��5�o�C�g�̃f�[�^�ɕϊ�����D
//
//	�����F
//		degree - �p�x�l�D-180�`180�͈̔͂Ŏw��D
//
//	Note:
//		�ܓx�o�x�̊p�x���̕ۑ��ɂ�5�o�C�g�ŏ\���D
//		�������鋗���̌덷�͑傫�����ς����Ă� 100�}�C�N�����[�g���D
//*******************************************************************************
inline _SQWORD ConvDegTo5Byte(double degree)
{
	return (_SQWORD)((degree + 180.0) * 3054198966.0444444444444444444444) & (_SQWORD)0x000000FFFFFFFFFF;
}

//*******************************************************************************
//	5�o�C�g�̃f�[�^����p�x�l�𕜌�����
//
//	�����F
//		data - ConvDegTo5Byte�ŕϊ�����5�o�C�g�̃f�[�^
//*******************************************************************************
inline double Dec5ByteToDeg(_SQWORD data)
{
	return (data & (_SQWORD)0x000000FFFFFFFFFF) * 3.274180926382541656494140625e-10 - 180.0;
}

//*******************************************************************************
//	�p�x�l��4�o�C�g�̃f�[�^�ɕϊ�����D
//
//	�����F
//		degree - �p�x�l�D-180�`180�͈̔͂Ŏw��D
//*******************************************************************************
inline _SDWORD ConvDegTo4Byte(double degree)
{
	return (_SDWORD)(degree * (0x7FFFFFFF / 180.0));
}

//*******************************************************************************
//	4�o�C�g�̃f�[�^����p�x�l�𕜌�����
//
//	�����F
//		data - ConvDegTo4Byte�ŕϊ�����4�o�C�g�̃f�[�^
//*******************************************************************************
inline double Dec4ByteToDeg(_SDWORD data)
{
	return (data * (180.0 / 0x7FFFFFFF));
}

//*******************************************************************************
//	�p�x�l��2�o�C�g�̃f�[�^�ɕϊ�����D
//
//	�����F
//		degree - �p�x�l�D-180�`180�͈̔͂Ŏw��D
//
//	Note:
//		�ő�덷�͖�0.00549
//*******************************************************************************
inline _SWORD ConvDegTo2Byte(double degree)
{
	return (_SWORD)(degree * 182.04444444444444444444444444444);
}

//*******************************************************************************
//	2�o�C�g�̃f�[�^����p�x�l�𕜌�����
//
//	�����F
//		data - ConvDegTo2Byte�ŕϊ�����2�o�C�g�̃f�[�^
//*******************************************************************************
inline double Dec2ByteToDeg(_SWORD data)
{
	return data * 0.0054931640625;
}

//*******************************************************************************
//	���W�A���P�ʂ̊p�x�l��2�o�C�g�̃f�[�^�ɕϊ�����D
//
//	�����F
// 		radian - �p�x�l�D-�΁`�΂͈̔͂Ŏw��D
//
//	Note:
//		�ő�덷�͖�0.0000959
//*******************************************************************************
inline _SWORD ConvRadTo2Byte(double radian)
{
	return (_SWORD)(radian * 10430.378350470452724949566316381);
}

//*******************************************************************************
//	2�o�C�g�̃f�[�^���烉�W�A���P�ʂ̊p�x�l�𕜌�����
//
//	�����F
//		data - ConvRadTo2Byte�ŕϊ�����2�o�C�g�̃f�[�^
//*******************************************************************************
inline double Dec2ByteToRad(_SWORD data)
{
	return data * 9.5873799242852576857380474343247e-5;
}

//*******************************************************************************
//	�}1�͈̔͂�double�^�f�[�^��2�o�C�g�̃f�[�^�ɕϊ�����
//
//	�����F
//		value - double�^�̃f�[�^
//
//	Note:
//		�ő�덷�͖�0.0000305
//*******************************************************************************
inline _SWORD Conv1To2Byte(double value)
{
	return (_SWORD)(value * 32768.0);
}

//*******************************************************************************
//	2�o�C�g�̃f�[�^����}1�͈̔͂̒l�𕜌�����
//
//	�����F
//		data - Conv100To2Byte�ŕϊ�����2�o�C�g�̃f�[�^
//*******************************************************************************
inline double Dec2ByteTo1(_SWORD data)
{
	return data / 32768.0;
}

//*******************************************************************************
//	�}10�͈̔͂�double�^�f�[�^��2�o�C�g�̃f�[�^�ɕϊ�����
//
//	�����F
//		value - double�^�̃f�[�^
//
//	Note:
//		�ő�덷�͖�0.000305
//*******************************************************************************
inline _SWORD Conv10To2Byte(double value)
{
	return (_SWORD)(value * 3276.8);
}

//*******************************************************************************
//	2�o�C�g�̃f�[�^����}10�͈̔͂̒l�𕜌�����
//
//	�����F
//		data - Conv10To2Byte�ŕϊ�����2�o�C�g�̃f�[�^
//*******************************************************************************
inline double Dec2ByteTo10(_SWORD data)
{
	return data / 3276.8;
}

//*******************************************************************************
//	�}100�͈̔͂�double�^�f�[�^��2�o�C�g�̃f�[�^�ɕϊ�����
//
//	�����F
//		value - double�^�̃f�[�^
//
//	Note:
//		�ő�덷�͖�0.00305
//*******************************************************************************
inline _SWORD Conv100To2Byte(double value)
{
	return (_SWORD)(value * 327.68);
}

//*******************************************************************************
//	2�o�C�g�̃f�[�^����}100�͈̔͂̒l�𕜌�����
//
//	�����F
//		data - Conv100To2Byte�ŕϊ�����2�o�C�g�̃f�[�^
//*******************************************************************************
inline double Dec2ByteTo100(_SWORD data)
{
	return data / 327.68;
}

//*******************************************************************************
//	�}1000�͈̔͂�double�^�f�[�^��2�o�C�g�̃f�[�^�ɕϊ�����
//
//	�����F
//		value - double�^�̃f�[�^
//
//	Note:
//		�ő�덷�͖�0.0305
//*******************************************************************************
inline _SWORD Conv1000To2Byte(double value)
{
	return (_SWORD)(value * 32.768);
}

//*******************************************************************************
//	2�o�C�g�̃f�[�^����}1000�͈̔͂̒l�𕜌�����
//
//	�����F
//		data - Conv1000To2Byte�ŕϊ�����2�o�C�g�̃f�[�^
//*******************************************************************************
inline double Dec2ByteTo1000(_SWORD data)
{
	return data * (1.0 / 32.768);
}

//*******************************************************************************
//	�}10000�͈̔͂�double�^�f�[�^��3�o�C�g�̃f�[�^�ɕϊ�����
//
//	�����F
//		value - double�^�̃f�[�^
//
//	Note:
//		�ő�덷�͖�0.0012
//*******************************************************************************
inline _SDWORD Conv10000To3Byte(double value)
{
	return (_SDWORD)((value + 10000.0) * 838.8608) & (_SDWORD)0xFFFFFF;
}

//*******************************************************************************
//	3�o�C�g�̃f�[�^����}10000�͈̔͂̒l�𕜌�����
//
//	�����F
//		data - Conv10000To3Byte�ŕϊ�����3�o�C�g�̃f�[�^
//*******************************************************************************
inline double Dec3ByteTo10000(_SDWORD data)
{
	return (data & (_SDWORD)0xFFFFFF) / 838.8608 - 10000.0;
}

//*******************************************************************************
//	�v���|�f�[�^��5�o�C�g�̃f�[�^�ɕϊ�����
//
//	�����F
//		pPropo	�v���|�f�[�^�̃|�C���^
//
//	Note:
//		�X�e�B�b�N���9�r�b�g�~4 + �X�C�b�`���4�r�b�g
//		�r�b�g�t�B�[���h�͏����n�Ɉˑ�����̂Ŏg��Ȃ�
//*******************************************************************************
#ifdef __cplusplus
_UQWORD ConvPropoPacket(const PropoData* pPropo);
#endif

//*******************************************************************************
//	5�o�C�g�̃f�[�^����v���|�f�[�^�𕜌�����
//
//	�����F
//		data   - ConvPropoPacket�ŕϊ�����5�o�C�g�̃f�[�^
//		pPropo - ���������f�[�^�̊i�[��
//*******************************************************************************
#ifdef __cplusplus
void DecPropoPacket(_UQWORD data, PropoData* pPropo);
#endif

//*******************************************************************************
//	�N�����f�[�^��2�o�C�g�̃f�[�^�ɕϊ�����
//
//	�����F
//		Year  �N�̉�2���̒l �i��F2014�N �� 14�j
//		Month ��
//		Day   ��
//*******************************************************************************
inline _UWORD ConvDateTo2Byte(_UBYTE Year, _UBYTE Month, _UBYTE Day)
{
	return ((Year & 0x3F) << 9) | ((Month & 0xF) << 5) | (Day & 0x1F);
}

//*******************************************************************************
//	2�o�C�g�̃f�[�^����N�����ɕϊ�����
//
//	�����F
//		data   ConvDateTo2Byte�ŕϊ�����2�o�C�g�̃f�[�^
//		pYear  �N�̉�2���̒l �i��F2014�N �� 14�j
//		pMonth ��
//		pDay   ��
//*******************************************************************************
inline void Dec2ByteToDate(_UWORD data, _UBYTE* pYear, _UBYTE* pMonth, _UBYTE* pDay)
{
	*pYear  = (data >> 9) & 0x3F;
	*pMonth = (data >> 5) & 0x0F;
	*pDay   = (data     ) & 0x1F;
}

//*******************************************************************************
//	�N���������b�f�[�^��4�o�C�g�̃f�[�^�ɕϊ�����
//
//	�����F
//		Year   �N�̉�2���̒l �i��F2014�N �� 14�j
//		Month  ��
//		Day    ��
//      Hour   ��
//      Minute ��
//      Second �b
//*******************************************************************************
inline _UDWORD ConvDateAndTimeTo4Byte(_UBYTE Year, _UBYTE Month, _UBYTE Day, _UBYTE Hour, _UBYTE Minute, _UBYTE Second)
{
	return Year      * ((_UDWORD)60*60*24*31*12) +
		   (Month-1) * ((_UDWORD)60*60*24*31) +
		   (Day-1)   * ((_UDWORD)60*60*24) +
		   Hour      * (60*60) +
		   Minute    * 60 +
		   Second;
}

//*******************************************************************************
//	4�o�C�g�̃f�[�^��N���������b�𕜌�����
//
//	�����F
//		pYear   �N�̉�2���̒l �i��F2014�N �� 14�j
//		pMonth  ��
//		pDay    ��
//      pHour   ��
//      pMinute ��
//      pSecond �b
//*******************************************************************************
inline void Dec4ByteToDateAndTime(_UDWORD data, _UBYTE* pYear, _UBYTE* pMonth, _UBYTE* pDay, _UBYTE* pHour, _UBYTE* pMinute, _UBYTE* pSecond)
{
	_UDWORD tmp;
	tmp = data / 60; *pSecond = (_UBYTE)(data - tmp * 60);     data = tmp;
	tmp = data / 60; *pMinute = (_UBYTE)(data - tmp * 60);     data = tmp;
	tmp = data / 24; *pHour   = (_UBYTE)(data - tmp * 24);     data = tmp;
	tmp = data / 31; *pDay    = (_UBYTE)(data - tmp * 31 + 1); data = tmp;
	tmp = data / 12; *pMonth  = (_UBYTE)(data - tmp * 12 + 1); data = tmp;
	*pYear = (_UBYTE)(data);
}

// 20140925 TCS.Y.Nakamura ADD_S
#include <string.h>
//*******************************************************************************
//	2�o�C�g�̕b�f�[�^���玞�ԁA���A�b�f�[�^�ɕϊ�����
//
//	�����F
//		time    �ϊ��Ώۃf�[�^�ւ̃|�C���^
//		pHour   ���Ԃւ̃|�C���^
//		pMinute ���ւ̃|�C���^
//		pSecond �b�ւ̃|�C���^
//		GetByte �ϊ��ΏۃT�C�Y
//*******************************************************************************
inline int ConvSecToHMS(void* time, _UDWORD* pHour, _UBYTE* pMinute, _UBYTE* pSecond, int GetByte)
{
	int b_time = 0;
	int *temp = (int*)time;
	
	// �p�����[�^�`�F�b�N
	if((NULL == time) || (NULL == pHour) || (NULL == pMinute) || (NULL == pSecond)
		|| (sizeof(b_time) <= GetByte)){
		return -1;
	}
	
	// �w��T�C�Y���A�f�[�^�R�s�[
	memcpy(&b_time, temp, GetByte);
	
	*pHour = b_time / 3600;
	*pMinute = (b_time % 3600) / 60;
	*pSecond = b_time % 60;
	
	return 0;
}
// 20140925 TCS.Y.Nakamura ADD_E

//*******************************************************************************
//	GPS�T�AGPS�T�b�̃f�[�^��2�o�C�g�̃f�[�^�ɕϊ�����
//
//	�����F
//		GpsWeek       GPS�T
//      GpsWeekSecond GPS�T�b (�����_�ȉ����܂�)
//
//  ���l�F
//      2042�N���Ɍv�Z���ʂ��I�[�o�[�t���[���܂�
//      ���ԉ𑜓x��0.2�b
//*******************************************************************************
inline _UDWORD ConvGpsTimeTo4Byte(_UWORD GpsWeek, float GpsWeekSecond)
{
	return (GpsWeek - 1800) * 3024000 + (_UDWORD)(GpsWeekSecond * 5);
}

//*******************************************************************************
//	4�o�C�g�̃f�[�^����GPS�T�AGPS�T�b�̃f�[�^�𕜌�����
//
//	�����F
//		GpsWeek       GPS�T
//      GpsWeekSecond GPS�T�b (�����_�ȉ����܂�)
//
//  ���l�F
//      2042�N���Ɍv�Z���ʂ��I�[�o�[�t���[���܂�
//*******************************************************************************
inline void Dec4ByteToGpsTime(_UDWORD data, _UWORD* pGpsWeek, float* pGpsWeekSecond)
{
	_UWORD tmp = (_UWORD)(data / 3024000);
	*pGpsWeek = tmp + 1800;
	*pGpsWeekSecond = (data - (tmp * 3024000)) * 0.2f;
}

#endif