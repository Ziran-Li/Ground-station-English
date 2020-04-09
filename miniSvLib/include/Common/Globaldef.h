//
//	    Globaldef.h
//
//		�\���́E�}�N���E�^��`
//
//		'08/01/06	Ver 1.00	����                                             by S-Suzuki
//		~08/10/08	Ver 1.01	�萔�̒�`�����낢��ƒǉ�                       by iwakura
//		'08/10/16	Ver 1.02	�t���C�g���[�h�ǉ�                               by iwakura
//		'08/10/28	Ver 1.02.1	�f�o�b�O�p�ϐ��ǉ�                               by iwakura
//		'09/01/22	Ver 1.03	�p�P�b�g���ǉ�                                 by iwakura
//		'09/02/27	Ver 1.04	�t���C�g���[�h�ǉ�(USER1~5)                      by iwakura
//		         	        	�p�P�b�gID�ǉ�(USER1~5)
//		'09/03/12	Ver 1.05	MAVInfo�̒ǉ�                                    by iwakura
//		'09/03/15	Ver 1.06	MAVInfo�̃����o�ǉ�                              by iwakura
//		'09/04/02	Ver 1.07	EstData�̒ǉ�
//		'09/09/11	Ver 1.08	�t�@�C���̏ꏊ�ύX�ɔ������C��                   by iwakura
//		         	        	�\���̖��̕ύX(MavData)
//		         	        	�p�P�b�gID�ǉ�(MAV_CONDITION)
//		'09/09/23	Ver 1.09	PosData �ɃR���X�g���N�^�ǉ�                     by iwakura
//		         	        	PosData �ɒl�ݒ�p�̊֐��ǉ�
//		'09/12/21	Ver 1.09.1	�t�@�C�����E�t�@�C���̏ꏊ�ύX�ɔ������C��       by iwakura
//		'10/01/04	Ver 1.10	�p�P�b�gID�ǉ� (PACKETID_ATTREF2, PACKETID_VELREF2, PACKETID_GPSREF2)
//		         	        	                                                 by iwakura
//		'10/01/05	Ver 1.11	PI, EXP, PACKETID_PROPO_SIMPLE ��ǉ�            by iwakura
//		'10/01/30	Ver 1.12	PACKETID_GENERALINT, PACKETID_GENERALREAL ���K�� by iwakura
//		'10/02/07	Ver 1.13	XUFO_COMMAND��MAV_COMMAND �ɉ���                 by iwakura
//								COMMAND_PING ��ǉ�
//		'10/05/20	Ver 1.14	�\���̖��̕ύX(PPmdata -> CtrlData)              by iwakura
//		'10/06/20	Ver 1.15	�\���́EPACKETID�EPACKETSIZE�̖��O�ύX           by iwakura
//								PropoData������������悤�ɂ���
//		'11/02/10	Ver 1.16	gDebug�̏������^�C�~���O��ύX                   by iwakura
//		�����������������������������������������
//			�������牺�̃o�[�W�����̓v���W�F�N�g�̃o�[�W����
//		�����������������������������������������
//		'13/??/??	Ver 2.0.X	�����낢��ƕύX                                 by iwakura
//		'13/06/09	Ver 2.1.0	Data3D�\���̂�ǉ�                               by iwakura
//		'12/07/06	Ver 2.1.1	EulerAngle, Quaternion�\���̂�ǉ�               by iwakura
//		'13/08/15	Ver 3.2.0	Commondef_High �Ɉړ��F
//		         	         	    "LowLevelMCUData"
//		         	         	Commondef_Low �Ɉړ��F
//		         	         	    "QUADWORD"
//
//

#ifndef INCLUDED_GLOBALDEF_H
#define INCLUDED_GLOBALDEF_H

#include "Common/StandardDef.h"

// �ėp��ԃx�N�g��
class StateVector
{
protected:
	size_t  m_Length;
	
	double* m_pX;

public:
	StateVector();
	~StateVector();
	
	CRESULT SetLength(size_t length);                      // �̈�m�ۗp�D
	size_t  GetLength() const;                             // �x�N�g�����̎擾�D
	double* GetPointer(unsigned int index, size_t length); // �|�C���^�̎擾�Dlength ����v���Ȃ��ꍇ NULL ���Ԃ�D
	double* GetPointer(size_t length) { return GetPointer(0, length); }
};



#endif
