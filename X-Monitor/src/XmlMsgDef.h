// X-Monitor�R�A���Ŏg�p����XML���̒�^��

// �ۑ��J�n
#define XML_MSG_REC_START1 ("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='FlightLog'><FlightLog><Rec><FilePath t='S'>")
#define XML_MSG_REC_START2 ("</FilePath><DivideSizeInByte t='UI'>")
#define XML_MSG_REC_START3 ("</DivideSizeInByte><FileMode t='S'>SAVE</FileMode></Rec></FlightLog></message></root>")

// �ۑ��I��
#define XML_MSG_REC_END	("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='FlightLog'><FlightLog><Rec><FileMode t='S'>END</FileMode></Rec></FlightLog></message></root>")

// COM�ݒ胊�v���C
#define XML_MSG_COM_CONFIG_REPLY1 ("<?xml version='1.0' encoding='utf-8'?><root><message type='set' a='ConfigReply'><ConfigReply><COM><Result t='S'>")
#define XML_MSG_COM_CONFIG_REPLY2 ("</Result></COM></ConfigReply></message></root>")

// �Đ����v���CNG
#define XML_MSG_PLAY_REPLY_NG ("<?xml version='1.0' encoding='utf-8'?><root><message type='set' a='FlightLogReply'><FlightLogReply><Play><Result t='S'>NG</Result></Play></FlightLogReply></message></root>")

// �}���`�E�F�C�|�C���g�R�}���h�ɑ΂���ACK
#define XML_MSG_MLTWAY_ACK1 ("<?xml version='1.0' encoding='UTF-8'?><root><message type='mainlink send'><Data><UAV><MltWptAck><WptId>")
#define XML_MSG_MLTWAY_ACK2 ("</WptId><Lat>")
#define XML_MSG_MLTWAY_ACK3 ("</Lat><Lon>")
#define XML_MSG_MLTWAY_ACK4 ("</Lon><Alt>")
#define XML_MSG_MLTWAY_ACK5 ("</Alt><Yaw>")
#define XML_MSG_MLTWAY_ACK6 ("</Yaw><Spd>")
#define XML_MSG_MLTWAY_ACK7 ("</Spd><Tol>")
#define XML_MSG_MLTWAY_ACK8 ("</Tol><Wait>")
#define XML_MSG_MLTWAY_ACK9 ("</Wait><FuncId>")
#define XML_MSG_MLTWAY_ACK10 ("</FuncId></MltWptAck></UAV></Data></message></root>")
