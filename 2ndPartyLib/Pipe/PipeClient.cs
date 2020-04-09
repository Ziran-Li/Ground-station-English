using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Pipe
{
	//// ************************************
	//// クライアントクラス
	////  --- 名前付きパイプにおけるクライアントを構成するクラス。
	//// ************************************
	//public class PipeClient
	//{

	//	//========================
	//	// メンバーメソッド
	//	//========================


	//	//============
	//	// サーバー接続
	//	// 本メソッドで、サーバーへの接続を行ないます。
	//	// 第１引数: サーバーへ要求するコマンド（命令）
	//	public void Connect(string Command_Message)
	//	{
	//		// 名前付きパイプ操作用のオブジェクト（クライアント用）を生成
	//		using (System.IO.Pipes.NamedPipeClientStream pipeClient = new System.IO.Pipes.NamedPipeClientStream(".", "acsl-pipe", System.IO.Pipes.PipeDirection.InOut))
	//		{

				
	//			try
	//			{
	//				// サーバー接続
	//				// --- 待機中のサーバーに接続します。
	//				// 定したタイムアウト期限内に、待機中のサーバーに接続します。
	//				pipeClient.Connect(5000);

	//				if (!pipeClient.IsConnected)
	//				{
	//					return;
	//				}

	//				// ストリームに対して、文字データーを読み書きするオブジェクトを生成
	//				AccessStream ss = new AccessStream(pipeClient);


	//				string temp1;    // 取得文字を格納するオブジェクト


	//				// サーバーに接続されたか確認。
	//				if ((temp1 = ss.ReadString("shift_jis")) == "A server requires the client to present an order.")
	//				{

	//					//------------
	//					// サーバーに接続されたので、以下の処理で、サーバーに要求内容
	//					// を示すコマンド（命令）を送信する。
	//					//------------


	//					// コマンド（命令）をサーバーへ送信。
	//					ss.WriteString(Command_Message, "shift_jis");



	//					//------------
	//					// サーバーから返信（処理結果）を受け取る。
	//					//------------


	//					// サーバーから取得する文字を格納するオブジェクト
	//					string temp2;


	//					// 電文受信
	//					// サーバープロセスから通達されたメッセージを取得する。
	//					temp2 = ss.ReadString("shift_jis");


	//					// 電文受信時に起動させるﾒｿｯﾄﾞを実行
	//					// （本クラスを利用する側のプログラムに、イベントを伝達
	//					//   するメソッドを実行）
	//					OnReceiveCall(DateTime.Now, temp2);

	//				}
	//			}
	//			catch
	//			{

	//			}

	//		}

	//	}

	//	//========================
	//	// イベント発生関連の定義
	//	//========================


	//	// 電文受信ﾒｿｯﾄﾞ用ﾃﾞﾘｹﾞｰﾄ
	//	//  --- ｲﾍﾞﾝﾄﾌﾟﾛｼｰｼﾞｬをどのような引数のﾒｿｯﾄﾞにしたいのか、それを宣言
	//	// 第１引数: 本イベントプロシージャの呼び出し元のオブジェクト
	//	// 第２引数: 返信電文を受信した時刻
	//	// 第３引数: 受信メッセージ
	//	public delegate void ReceiveCallDelegate(object sender, DateTime Event_Time, string Reply_Message);


	//	// 電文受信ﾒｯｾｰｼﾞ用ｲﾍﾞﾝﾄの定義
	//	//  --- 宣言された物をｲﾍﾞﾝﾄとして登録
	//	public event ReceiveCallDelegate ReceiveCallEvent; // 電文受信ﾒｯｾｰｼﾞ伝達ｲﾍﾞﾝﾄ


	//	//============
	//	// 電文受信ｲﾍﾞﾝﾄ処理
	//	//  --- 電文を受信した時の処理内容を定義したﾒｿｯﾄﾞ。
	//	// 第１引数: 返信電文を受信した時刻
	//	// 第２引数: 受信メッセージ
	//	public void OnReceiveCall(DateTime Event_Time, string Reply_Message)
	//	{
	//		if (ReceiveCallEvent != null)
	//		{
	//			// ｸﾗｽｵﾌﾞｼﾞｪｸﾄ生成後に登録されたﾒｿｯﾄﾞ(ｲﾍﾞﾝﾄﾊﾝﾄﾞﾗ)を実行
	//			ReceiveCallEvent(this, Event_Time, Reply_Message);
	//		}
	//	}
	//}
	// ************************************
	// クライアントクラス
	//  --- 名前付きパイプにおけるクライアントを構成するクラス。
	// ************************************
	public class PipeClient
	{
		//============
		// メンバー変数

		// 名前付きパイプ操作用のオブジェクト（クライアント用）を生成
		public System.IO.Pipes.NamedPipeClientStream pipeClient;


		// サーバーへ要求するコマンド（命令）
		public string CommandMessage;


		// データー類
		public byte[] HeaderBuffer; // ヘッダー





		//============
		// コンストラクタ
		// 第１引数: 制御対象のストリーム
		public PipeClient()
		{

			// Int32型のデーターのサイズ（Int32型サイズ）
			Int32 Int_Size = sizeof(Int32);


			// Int32型データー用バイト型配列
			// 後にint型データーを、バイト型配列に格納する時に使う。
			HeaderBuffer = new byte[Int_Size];


			// パイプストリームとタイマーの初期化（解法状態で初期化）
			this.pipeClient = null;

		}


		//========================
		// メンバーメソッド
		//========================



		//============
		// サーバー接続
		// 本メソッドで、サーバーへの接続を行ないます。
		// 第１引数: サーバーへ要求するコマンド（命令）
		public bool Connect(string Command_Message)
		{
			try
			{
				// 名前付きパイプ操作用のオブジェクト（クライアント用）を生成
				pipeClient = new System.IO.Pipes.NamedPipeClientStream(".", "acsl-pipe", System.IO.Pipes.PipeDirection.InOut);


				// サーバー接続
				// --- 待機中のサーバーに接続します。
				//pipeClient.Connect();
				pipeClient.Connect(5000);

				// サーバーへ送るコマンドを保存
				CommandMessage = Command_Message;


				// 接続通知受信
				ReceiveConnectionNotice(this);

				//System.Windows.MessageBox.Show("データを転送に成功しました。");
				return true;
			}
			catch
			{
				System.Windows.MessageBox.Show("データを転送に失敗しました。");
				return false;
			}
		}



		//============
		// 接続通知受信メソッド
		// サーバーへ接続すると、無事に接続されたことを知らせる通知用電文が、
		// サーバーから送られて来る仕様にしてあります。その通知を本メソッド
		// で受信します。
		// 第１引数: クライアントクラス（各スレッド間の情報伝達に使用）
		private void ReceiveConnectionNotice(PipeClient client)
		{

			// 非同期操作のステータスを表すオブジェクト。
			IAsyncResult result;


			try
			{
				// 接続通知用受信電文のヘッダーを取得する。
				// かつ、その後の処理を行なうメソッドを指定。
				result = client.pipeClient.BeginRead(client.HeaderBuffer, 0, client.HeaderBuffer.Length, null, null);
			}
			catch (InvalidOperationException) //disconnected/disposed
			{
				return;
			}



			//------------
			// 非同期操作完了
			//------------


			// 非同期処理が完了するまで待つ。
			result.AsyncWaitHandle.WaitOne();


			int bytesRead;

			try
			{
				// 非同期読み取りを完了するまで待機する操作を終了します。
				bytesRead = client.pipeClient.EndRead(result);
			}
			catch (System.IO.IOException) //closed
			{
				return;
			}


			// 以下の処理は、サーバーから来た接続通知を読み始めてから実行
			// されます。正確には、接続通知電文のヘッダーを読み終えた直後
			// に、以下の処理の実行が開始されます。
			// （それまで待機中となり、実行が開始されない）




			//------------
			// ヘッダーの復元
			//  --- ヘッダーは、既に読み込み済みなので、後はそれをInt32型に復元する。
			//------------




			// 取得したデーターをバイト型配列からint型に変換
			Int32 len = BitConverter.ToInt32(client.HeaderBuffer, 0);





			//------------
			// データー本体読み込み
			//------------


			// データー本体用バイト型配列
			// 後にデーター本体を、バイト型配列に格納する時に使う。
			byte[] inBuffer = new byte[len];


			// ストリームからデーター本体を読み込む。
			client.pipeClient.Read(inBuffer, 0, len);



			// 取得したデーターをバイト型配列からstring型に変換
			// （ﾊﾞｲﾄ型配列 ---> ﾕﾆｺｰﾄﾞstring 変換）
			string temp;    // 取得文字を格納するオブジェクト
			StringBytesController.ChangeBinaryToUnicodeStr(
				inBuffer,
				"shift_jis",
				out temp);


			// サーバーに接続されたか確認。
			if (temp == "A connection succeeded.")
			{

				// ストリームに対して、文字データーを読み書きするオブジェクトを生成
				AccessStream ss = new AccessStream(pipeClient);


				// コマンド（命令）をサーバーへ送信。
				ss.WriteString(client.CommandMessage, "shift_jis");


				// サーバープロセスから通達されたメッセージを取得する。
				client.ReceiveReply(client);
			}


		}



		//============
		// 返信受信
		// サーバーから来る返信電文を受信する（すなわち、サーバーへ要求
		// していた処理結果を、本メソッドで取得します）。
		// 第１引数: クライアントクラス（各スレッド間の情報伝達に使用）
		public void ReceiveReply(PipeClient client)
		{

			//------------
			// サーバーから返信（処理結果）を受け取る。
			//------------


			// 非同期操作のステータスを表すオブジェクト。
			IAsyncResult result;


			try
			{

				// 返信（受信電文）のヘッダーを取得する。
				// かつ、その後の処理を行なうメソッドを指定。
				result = client.pipeClient.BeginRead(client.HeaderBuffer, 0, client.HeaderBuffer.Length, null, null);

			}
			catch (InvalidOperationException) //disconnected/disposed
			{
				return;
			}



			//------------
			// 非同期操作完了
			//------------


			// 非同期処理が完了するまで待つ。
			result.AsyncWaitHandle.WaitOne();


			int bytesRead;

			try
			{
				// 非同期読み取りを完了するまで待機する操作を終了します。
				bytesRead = client.pipeClient.EndRead(result);
			}
			catch (System.IO.IOException) //closed
			{
				return;
			}


			// 以下の処理は、サーバーから来た返信を読み始めてから実行
			// されます。正確には、返信電文のヘッダーを読み終えた直後
			// に、以下の処理の実行が開始されます。
			// （それまで待機中となり、実行が開始されない）



			//------------
			// ヘッダーの復元
			//  --- ヘッダーは、既に読み込み済みなので、後はそれをInt32型に復元する。
			//------------




			// 取得したデーターをバイト型配列からint型に変換
			Int32 len = BitConverter.ToInt32(client.HeaderBuffer, 0);




			//------------
			// データー本体読み込み
			//------------


			// データー本体用バイト型配列
			// 後にデーター本体を、バイト型配列に格納する時に使う。
			byte[] inBuffer = new byte[len];


			try
			{
				// ストリームからデーター本体を読み込む。
				client.pipeClient.Read(inBuffer, 0, len);
			}
			catch (System.ObjectDisposedException)  // ストリームが破棄されている場合
			{
				return;
			}


			// 取得したデーターをバイト型配列からstring型に変換
			// （ﾊﾞｲﾄ型配列 ---> ﾕﾆｺｰﾄﾞstring 変換）
			string temp;    // 取得文字を格納するオブジェクト
			StringBytesController.ChangeBinaryToUnicodeStr(
				inBuffer,
				"shift_jis",
				out temp);


			// パイプストリーム解法
			client.pipeClient.Close();
			client.pipeClient.Dispose();
			client.pipeClient = null;


			// 電文受信時に起動させるﾒｿｯﾄﾞを実行
			OnReceiveCall(DateTime.Now, temp);

		}



		//========================
		// イベント発生関連の定義
		//========================


		// 電文受信ﾒｿｯﾄﾞ用ﾃﾞﾘｹﾞｰﾄ
		//  --- ｲﾍﾞﾝﾄﾌﾟﾛｼｰｼﾞｬをどのような引数のﾒｿｯﾄﾞにしたいのか、それを宣言
		// 第１引数: 本イベントプロシージャの呼び出し元のオブジェクト
		// 第２引数: 返信電文を受信した時刻
		// 第３引数: 受信メッセージ
		public delegate void ReceiveCallDelegate(object sender, DateTime Event_Time, string Reply_Message);


		// 電文受信ﾒｯｾｰｼﾞ用ｲﾍﾞﾝﾄの定義
		//  --- 宣言された物をｲﾍﾞﾝﾄとして登録
		public event ReceiveCallDelegate ReceiveCallEvent; // 電文受信ﾒｯｾｰｼﾞ伝達ｲﾍﾞﾝﾄ


		//============
		// 電文受信ｲﾍﾞﾝﾄ処理
		//  --- 電文を受信した時の処理内容を定義したﾒｿｯﾄﾞ。
		// 第１引数: 返信電文を受信した時刻
		// 第２引数: 受信メッセージ
		public void OnReceiveCall(DateTime Event_Time, string Reply_Message)
		{
			if (ReceiveCallEvent != null)
			{
				// ｸﾗｽｵﾌﾞｼﾞｪｸﾄ生成後に登録されたﾒｿｯﾄﾞ(ｲﾍﾞﾝﾄﾊﾝﾄﾞﾗ)を実行
				ReceiveCallEvent(this, Event_Time, Reply_Message);
			}
		}

	}
}