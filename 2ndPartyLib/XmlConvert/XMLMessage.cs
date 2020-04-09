
namespace XmlBase
{
	/*　
	 * 　XMLファイルの読み込み
	 * 
	 * （1）ソリューション「XML/Message」にXMLファイルを追加すること
	 * （2）追加したXMLファイルはプロパティより、ビルドアクション「埋め込まれたリソース」出力ディレクトリにコピー「常にコピーする」に設定すること
	 * （3）Xml/Message内のXMLファイル名のまま追記すること
	 * 
	 */
	enum XmlMessage
	{
		Close,							//Closeメッセージ
		Get_UAVData,					//Getメッセージ　UAVデータ取得リスト
		Get_Initial,					//Getメッセージ  コア部のCOMポート番号とディレクトリの取得
		Get_COMPort,					//Getメッセージ　コア部のCOMポート番号の取得
		MainlinkSend_Home,				//MainlinkSendメッセージ　HomeデータをUAVへ送信　
		MainlinkSend_Land,				//MainlinkSendメッセージ　LandデータをUAVへ送信　
		MainlinkSend_Pause,				//MainlinkSendメッセージ　PauseデータをUAVへ送信
		MainlinkSend_Resume,			//MainlinkSendメッセージ　LandデータをUAVへ送信
		MainlinkSend_Waypoint,			//MainlinkSendメッセージ　ResumeデータをUAVへ送信
		MainlinkSend_MultiWaypoint,		//MainlinkSendメッセージ　MultiWaypointデータをUAVへ送信
		MainlinkSend_MultiWaypointAck,	//MainlinkSendメッセージ　MultiWaypointデータをリプライ
		Set_COMPort,					//Setメッセージ　コア部のCOMポート番号を設定
		Set_FlightLogEdit,				//Setメッセージ　飛行ログの切り出し保存
		Set_FlightLogOpenClose,			//Setメッセージ　飛行ログを開く/閉じる
		Set_FlightLogPlay,				//Setメッセージ　飛行ログの再生/停止/一時停止/モード送り/モード戻し
		Set_FlightLogSlidebar			//Setメッセージ　飛行ログのスライドバー
	}
}
