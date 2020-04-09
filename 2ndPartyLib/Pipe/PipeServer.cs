using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace Pipe
{
	#region Comment
	//// ************************************
	//// サーバークラス
	////  --- 名前付きパイプにおけるサーバーを構成するクラス。サーバーとして
	////      の固有の定義をここで行ないます。なお、本サーバーでは、クライア
	////      ントからの命令（コマンド）に対して、それに対応したメッセージを
	////      返却する仕様とします。
	//// ************************************
	//public class PipeServer
	//{//========================
	//	// メンバーメソッド
	//	//  --- 主となる処理（受付、受信、送信）
	//	//========================



	//	//============
	//	// 受け付け処理用メソッド
	//	// クライアントからの接続の受け付けを、本メソッドを実行することで開始されます。
	//	public void Listen()
	//	{
	//		string _message = "";

	//		// 名前付きパイプ操作用のオブジェクト（サーバー用）を生成
	//		using (System.IO.Pipes.NamedPipeServerStream pipeServer = new System.IO.Pipes.NamedPipeServerStream("acsl-pipe", System.IO.Pipes.PipeDirection.InOut))
	//		{

	//			// 受付開始時に起動させるメソッドを実行
	//			// （本クラスを利用する側のプログラムに、イベントを伝達
	//			//   するメソッドを実行）
	//			OnStartListen(DateTime.Now, _message);


	//			// 接続待機
	//			//  --- クライアントが本サーバー（NamedPipeServerStream オブジェクト）に接続するのを待機します。
	//			try
	//			{
	//				pipeServer.WaitForConnection();
	//			}
	//			catch //(Exception ex)
	//			{
	//				// ここで２回目以降の受信時に例外を拾ってしまう
	//			}



	//			// 以下の処理は、本サーバーにクライアントが接続された後に実行されます。
	//			// （それまで待機中となり、実行が開始されない）


	//			//label1.Text = "クライアントが接続しまいた。";


	//			try
	//			{

	//				// ユーザーが入力した要求電文を読んで、それに応じた返信をクライアン
	//				// トプロセスに送信する。


	//				// ストリームに対して、文字データーを読み書きするオブジェクトを生成
	//				AccessStream ss = new AccessStream(pipeServer);


	//				// 送信データーを書き込み
	//				//  --- 「サーバーはクライアントに命令を提出するよう要求している」
	//				//      と言う意味の英文メッセージを名前付きパイプに書き込む。
	//				ss.WriteString(
	//					"A server requires the client to present an order.",
	//					"shift_jis");

	//				//ss.WriteString("A1A1A1", "shift_jis");


	//				string OrderMessage = ss.ReadString("shift_jis");


	//				//ss.WriteString("Reply:", "shift_jis");


	//				// 返信用電文作成
	//				// クライアントに返す返信用電文を作成します。なお、今までの
	//				// 流れでは、
	//				// ss.WriteString("返信内容", "shift_jis");
	//				// で返信したいところです。しかし、マイクロソフトのホームペ
	//				// ージ（MSDNホームページ）を見ますと、返信はRunAsClient()
	//				// と言うメソッドで行なうようなので、下記コードもそのように
	//				// します。


	//				// 実際にサーバーで行なう処理内容（返信用処理）を定義しているクラスを生成します。
	//				ServerService Server_Service = new ServerService(ss, OrderMessage);
	//				_message = OrderMessage;

	//				// 返信用処理の実行
	//				pipeServer.RunAsClient(Server_Service.SendMessage);

	//			}
	//			// 名前付きパイプが壊れているなどの異常が発生した場合
	//			catch //(System.IO.IOException ex)
	//			{
	//				// エラー処理
	//				//MessageBox.Show(ex.Message, "エラー通知");
	//			}


	//			// 返信送信完了時に起動させるメソッドを実行
	//			// （本クラスを利用する側のプログラムに、イベントを伝達
	//			//   するメソッドを実行）
	//			OnFinishTransmission(DateTime.Now, _message);

	//		}

	//	}

	//	// ************************************
	//	// イベント発生関連の定義
	//	// ************************************


	//	//========================
	//	// 受け付け開始イベント発生の定義
	//	//========================


	//	// 受け付け開始メソッド用デリゲート
	//	//  --- ｲﾍﾞﾝﾄﾌﾟﾛｼｰｼﾞｬをどのような引数のﾒｿｯﾄﾞにしたいのか、それを宣言
	//	// 第１引数: 本イベントプロシージャの呼び出し元のオブジェクト
	//	// 第２引数: 受け付けを開始した時刻
	//	public delegate void StartListenMethodDelegate(object sender, DateTime Event_Time, string Message);

	//	// 受付開始メッセージ用イベントの定義
	//	//  --- 宣言された物をｲﾍﾞﾝﾄとして登録
	//	public event StartListenMethodDelegate StartListenEvent; // ﾒｯｾｰｼﾞ伝達ｲﾍﾞﾝﾄ

	//	//------------------------
	//	// 受付開始イベント処理
	//	//  --- 受け付けを開始した時、それに応じる処理内容を定義したメソッドです。
	//	// 第１引数: 受け付けを開始した時刻
	//	public void OnStartListen(DateTime Event_Time, string Message)
	//	{
	//		if (StartListenEvent != null)
	//		{
	//			// ｸﾗｽｵﾌﾞｼﾞｪｸﾄ生成後に登録されたﾒｿｯﾄﾞ(ｲﾍﾞﾝﾄﾊﾝﾄﾞﾗ)を実行
	//			StartListenEvent(this, Event_Time, Message);
	//		}
	//	}

	//	//========================
	//	// 返信電文送信完了イベント発生の定義
	//	//========================


	//	// 返信送信完了ﾒｿｯﾄﾞ用ﾃﾞﾘｹﾞｰﾄ
	//	//  --- ｲﾍﾞﾝﾄﾌﾟﾛｼｰｼﾞｬをどのような引数のﾒｿｯﾄﾞにしたいのか、それを宣言
	//	// 第１引数: 本イベントプロシージャの呼び出し元のオブジェクト
	//	// 第２引数: 返信電文を送信した時刻
	//	public delegate void FinishTransmissionDelegate(object sender, DateTime Transmit_Time, string Message);


	//	// 返信送信完了ﾒｯｾｰｼﾞ用ｲﾍﾞﾝﾄの定義
	//	//  --- 宣言された物をｲﾍﾞﾝﾄとして登録
	//	public event FinishTransmissionDelegate FinishTransmissionEvent; // ﾒｯｾｰｼﾞ伝達ｲﾍﾞﾝﾄ


	//	//------------------------
	//	// 返信送信完了ｲﾍﾞﾝﾄ処理
	//	//  --- 返信電文送信を完了した時の処理内容を定義したﾒｿｯﾄﾞ。
	//	// 第１引数: 返信電文を送信した時刻
	//	public void OnFinishTransmission(DateTime Transmit_Time, string Message)
	//	{
	//		if (FinishTransmissionEvent != null)
	//		{
	//			// ｸﾗｽｵﾌﾞｼﾞｪｸﾄ生成後に登録されたﾒｿｯﾄﾞ(ｲﾍﾞﾝﾄﾊﾝﾄﾞﾗ)を実行
	//			FinishTransmissionEvent(this, Transmit_Time, Message);
	//		}
	//	}

	//}

	//// ************************************
	//// サービス内容定義クラス
	////  --- 本サーバーが提供する機能（サービス）を定義するクラスです。
	//// ************************************
	//public class ServerService
	//{

	//	//============
	//	// メンバー変数


	//	// ストリーム制御クラス
	//	private AccessStream ss;


	//	// クライアントからの要求電文によるコマンド
	//	private string OrderMessage;


	//	//============
	//	// コンストラクタ
	//	// 第１引数: ストリーム制御クラス
	//	// 第２引数: クライアントからの要求電文によるコマンド
	//	public ServerService(AccessStream str, string Order_Message)
	//	{
	//		ss = str;
	//		OrderMessage = Order_Message;
	//	}

	//	//============
	//	// メッセージ返信メソッド
	//	// クライアントから来た要求電文には、コマンド（命令）が書かれ
	//	// ています。そのコマンドに対応したメッセージを、本メソッドに
	//	// よりクライアントへ返信します。
	//	public void SendMessage()
	//	{

	//		// コマンドが"command1"の場合
	//		if (OrderMessage == "command1")
	//		{
	//			// 返信用メッセージをストリームに書き込む。
	//			ss.WriteString("あいうえお", "shift_jis");
	//		}
	//		// コマンドが"command2"の場合
	//		else if (OrderMessage == "command2")
	//		{
	//			// 返信用メッセージをストリームに書き込む。
	//			ss.WriteString("abcdefg", "shift_jis");
	//		}
	//		// コマンドが"command3"の場合
	//		else if (OrderMessage == "command3")
	//		{
	//			// 返信用メッセージをストリームに書き込む。
	//			ss.WriteString("1234567", "shift_jis");
	//		}
	//		// その他
	//		else
	//		{
	//			// 返信用メッセージをストリームに書き込む。
	//			ss.WriteString("command error", "shift_jis");
	//		}
	//	}
	//}
	#endregion

	// http://note.chiebukuro.yahoo.co.jp/detail/n182212
	 // ************************************
    // サーバークラス
    //  --- 名前付きパイプにおけるサーバーを構成するクラス。サーバーとして
    //      の固有の定義をここで行ないます。なお、本サーバーでは、クライア
    //      ントからの命令（コマンド）に対して、それに対応したメッセージを
    //      返却する仕様とします。
    // ************************************
    public class PipeServer
    {

        // ************************
        // メンバー変数とコンストラクタ
        // ************************

 

        // 名前付きパイプ操作用のオブジェクト（クライアント用）を生成
        public System.IO.Pipes.NamedPipeServerStream pipeServer;


        // 送信データー
        public string SendData;

 

        //============
        // コンストラクタ
        // 第１引数: 送信データー
        public PipeServer()
        {
            // 名前付きパイプ操作用のオブジェクトをnullで初期化
            pipeServer = null;


           
        }

 

        // ************************
        // メンバーメソッド
        //  --- 主となる処理（受付、受信、送信）
        // ************************

 

        //============
        // 受け付け処理用メソッド
        // クライアントからの接続の受け付けを、本メソッドを実行することで開始されます。
		public void Listen(string Send_Data)
        {
			// 送信データー
			SendData = Send_Data;
            // 受付開始時に起動させるメソッドを実行
            // （本クラスを利用する側のプログラムに、イベントを伝達
            //   するメソッドを実行）
            OnStartListen(DateTime.Now);


            try
            {

                // 名前付きパイプ操作用のオブジェクト（サーバー用）を生成
                pipeServer = new System.IO.Pipes.NamedPipeServerStream("acsl-pipe", System.IO.Pipes.PipeDirection.InOut, 1, System.IO.Pipes.PipeTransmissionMode.Byte, System.IO.Pipes.PipeOptions.Asynchronous);


                // 接続待機
                // クライアントが接続して来るのを待機する。
                // かつ、その後の処理を行なうメソッドを指定。
                pipeServer.BeginWaitForConnection(new AsyncCallback(WaitForConnectionCallBack), pipeServer);
            }
            catch (Exception ex)
            {
                // エラー処理
                System.Windows.Forms.MessageBox.Show(ex.Message, "エラー通知");
            }
        }

 

        //============
        // クライアント接続後処理メソッド
        // 本サーバーにクライアントが接続してきたら、本メソッドの処理を
        // 実行します。具体的には、まず、クライアントに要求電文（命令）
        // を提出するよう通知します（すなわち、これは、サーバーに接続さ
        // れたことをクライアントに通知することを意味しています）。次に、
        // クライアントから要求電文が送られてくるのを待機します。
        // 第１引数: 非同期操作のステータス
        private void WaitForConnectionCallBack(IAsyncResult iar)
        {

            try
            {

                // パイプの取得
                System.IO.Pipes.NamedPipeServerStream pipe_Server = (System.IO.Pipes.NamedPipeServerStream)iar.AsyncState;


                //------------
                // 非同期操作完了
                //------------


                // クライアントが接続するのを待機する非同期操作を終了します。
                pipe_Server.EndWaitForConnection(iar);


                // 以下の処理は、本サーバーにクライアントが接続された後に実行されます。
                // （それまで待機中となり、実行が開始されない）


                //------------
                // クライアントへ接続通知を送信
                //------------


                // ストリームに対して、文字データーを読み書きするオブジェクトを生成
                AccessStream ss = new AccessStream(pipe_Server);


                // 送信データーを書き込み
                //  --- クライアントに要求電文（命令）を提出するよう通知します。
                //      具体的には、
                //      「サーバーはクライアントに命令を提出するよう要求している」
                //      と言う意味の英文メッセージを名前付きパイプに書き込む。
                ss.WriteString(
                    "A connection succeeded.",
                    "shift_jis");


                //------------
                // クライアントから要求電文を受信
                //------------


                // 要求電文受信待機
                ReceiveOrder(this);

            }
            // 名前付きパイプが壊れているなどの異常が発生した場合
            catch (System.IO.IOException ex)
            {
                // エラー処理
                System.Windows.Forms.MessageBox.Show(ex.Message, "エラー通知");
            }
        }


        //============
        // 要求電文受信待機
        // クライアントから要求電文（命令）を受信します。
        // 第１引数: サーバークラス（各スレッド間の情報伝達に使用）
        void ReceiveOrder(PipeServer p_server)
        {

            // Int32型のデーターのサイズ（Int32型サイズ）
            Int32 Int_Size = sizeof(Int32);


            // ヘッダー
            // Int32型データー用バイト型配列
            // 後にint型データーを、バイト型配列に格納する時に使う。
            byte[] HeaderBuffer = new byte[Int_Size];


            // 非同期操作のステータスを表すオブジェクト。
            IAsyncResult result;


            try
            {
                // 要求用受信電文のヘッダーを取得する。
                // かつ、その後の処理を行なうメソッドを指定。
                result = p_server.pipeServer.BeginRead(HeaderBuffer, 0, HeaderBuffer.Length, null, null);
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
                bytesRead = p_server.pipeServer.EndRead(result);
            }
            catch (System.IO.IOException) //closed
            {
                return;
            }


            // 以下の処理は、クライアントから来た要求電文を読み始めてから
            // 実行されます。正確には、要求電文のヘッダーを読み終えた直後
            // に、以下の処理の実行が開始されます。
            // （それまで待機中となり、実行が開始されない）

 

            //------------
            // ヘッダーの復元
            //  --- ヘッダーは、既に読み込み済みなので、後はそれをInt32型に復元する。
            //------------


            // 取得したデーターをバイト型配列からint型に変換
            Int32 len = BitConverter.ToInt32(HeaderBuffer, 0);

 

 

            //------------
            // データー本体読み込み
            //------------


            // データー本体用バイト型配列
            // 後にデーター本体を、バイト型配列に格納する時に使う。
            byte[] inBuffer = new byte[len];


            // ストリームからデーター本体を読み込む。
            p_server.pipeServer.Read(inBuffer, 0, len);

 

            // 取得したデーターをバイト型配列からstring型に変換
            // （ﾊﾞｲﾄ型配列 ---> ﾕﾆｺｰﾄﾞstring 変換）
            string OrderMessage;    // 取得文字を格納するオブジェクト
            StringBytesController.ChangeBinaryToUnicodeStr(
                inBuffer,
                "shift_jis",
                out OrderMessage);


            // 電文受信時に起動させるﾒｿｯﾄﾞを実行
            OnReceiveCall(DateTime.Now, OrderMessage);

 

            // 返信用電文作成
            // クライアントに返す返信用電文を作成します。なお、今までの
            // 流れでは、
            // ss.WriteString("返信内容", "shift_jis");
            // で返信したいところです。しかし、マイクロソフトのホームペ
            // ージ（MSDNホームページ）を見ますと、返信はRunAsClient()
            // と言うメソッドで行なうようなので、下記コードもそのように
            // します。


            // ストリームに対して、文字データーを読み書きするオブジェクトを生成
            AccessStream ss = new AccessStream(p_server.pipeServer);


            // 返信用メッセージをストリームに書き込む。
            ss.WriteString(SendData, "shift_jis");

 

            // 名前付きパイプのクローズ
            p_server.pipeServer.Close();
            p_server.pipeServer.Dispose();
            p_server.pipeServer = null;


            // 返信送信完了時に起動させるメソッドを実行
            OnFinishTransmission(DateTime.Now);


        }

 

        // ************************************
        // イベント発生関連の定義
        // ************************************


        //========================
        // 受付開始イベント発生の定義


        // 受付開始メソッド用デリゲート
        //  --- ｲﾍﾞﾝﾄﾌﾟﾛｼｰｼﾞｬをどのような引数のﾒｿｯﾄﾞにしたいのか、それを宣言
        // 第１引数: 本イベントプロシージャの呼び出し元のオブジェクト
        // 第２引数: 受け付けを開始した時刻
        public delegate void StartListenMethodDelegate(object sender, DateTime Event_Time);

 

        // 受付開始メッセージ用イベントの定義
        //  --- 宣言された物をｲﾍﾞﾝﾄとして登録
        public event StartListenMethodDelegate StartListenEvent; // ﾒｯｾｰｼﾞ伝達ｲﾍﾞﾝﾄ

 

        //------------------------
        // 受付開始イベント処理
        //  --- 受け付けを開始した時、それに応じる処理内容を定義したメソッドです。
        // 第１引数: 受け付けを開始した時刻
        public void OnStartListen(DateTime Event_Time)
        {
            if (StartListenEvent != null)
            {
                // ｸﾗｽｵﾌﾞｼﾞｪｸﾄ生成後に登録されたﾒｿｯﾄﾞ(ｲﾍﾞﾝﾄﾊﾝﾄﾞﾗ)を実行
                StartListenEvent(this, Event_Time);
            }
        }

 

        //========================
        // 電文受信イベント発生関連の定義


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

 

        //========================
        // 返信電文送信完了イベント発生の定義


        // 返信送信完了ﾒｿｯﾄﾞ用ﾃﾞﾘｹﾞｰﾄ
        //  --- ｲﾍﾞﾝﾄﾌﾟﾛｼｰｼﾞｬをどのような引数のﾒｿｯﾄﾞにしたいのか、それを宣言
        // 第１引数: 本イベントプロシージャの呼び出し元のオブジェクト
        // 第２引数: 返信電文を送信した時刻
        public delegate void FinishTransmissionDelegate(object sender, DateTime Transmit_Time);


        // 返信送信完了ﾒｯｾｰｼﾞ用ｲﾍﾞﾝﾄの定義
        //  --- 宣言された物をｲﾍﾞﾝﾄとして登録
        public event FinishTransmissionDelegate FinishTransmissionEvent; // ﾒｯｾｰｼﾞ伝達ｲﾍﾞﾝﾄ


        //------------------------
        // 返信送信完了ｲﾍﾞﾝﾄ処理
        //  --- 返信電文送信を完了した時の処理内容を定義したﾒｿｯﾄﾞ。
        // 第１引数: 返信電文を送信した時刻
        public void OnFinishTransmission(DateTime Transmit_Time)
        {
            if (FinishTransmissionEvent != null)
            {
                // ｸﾗｽｵﾌﾞｼﾞｪｸﾄ生成後に登録されたﾒｿｯﾄﾞ(ｲﾍﾞﾝﾄﾊﾝﾄﾞﾗ)を実行
                FinishTransmissionEvent(this, Transmit_Time);
            }
        }

    }
}