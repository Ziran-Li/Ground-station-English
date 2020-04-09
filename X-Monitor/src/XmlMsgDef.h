// X-Monitorコア部で使用するXML文の定型句

// 保存開始
#define XML_MSG_REC_START1 ("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='FlightLog'><FlightLog><Rec><FilePath t='S'>")
#define XML_MSG_REC_START2 ("</FilePath><DivideSizeInByte t='UI'>")
#define XML_MSG_REC_START3 ("</DivideSizeInByte><FileMode t='S'>SAVE</FileMode></Rec></FlightLog></message></root>")

// 保存終了
#define XML_MSG_REC_END	("<?xml version='1.0' encoding='UTF-8'?><root><message type='set' a='FlightLog'><FlightLog><Rec><FileMode t='S'>END</FileMode></Rec></FlightLog></message></root>")

// COM設定リプライ
#define XML_MSG_COM_CONFIG_REPLY1 ("<?xml version='1.0' encoding='utf-8'?><root><message type='set' a='ConfigReply'><ConfigReply><COM><Result t='S'>")
#define XML_MSG_COM_CONFIG_REPLY2 ("</Result></COM></ConfigReply></message></root>")

// 再生リプライNG
#define XML_MSG_PLAY_REPLY_NG ("<?xml version='1.0' encoding='utf-8'?><root><message type='set' a='FlightLogReply'><FlightLogReply><Play><Result t='S'>NG</Result></Play></FlightLogReply></message></root>")

// マルチウェイポイントコマンドに対するACK
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
