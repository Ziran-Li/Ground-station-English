using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Pipe
{
	// ************************************
	// 共通クラス
	// ************************************
	class Common
	{
	}

	// ************************************
	// ストリーム制御クラス
	// ************************************
	public class AccessStream
	{
		//============
		// メンバー変数


		// 制御対象のストリーム
		private System.IO.Stream ioStream;


		//============
		// コンストラクタ
		// 第１引数: 制御対象のストリーム
		public AccessStream(System.IO.Stream io_Stream)
		{
			this.ioStream = io_Stream;
		}



		//============
		// バイト型配列（バイナリー）読み込みメソッド
		// ストリームからデーターを読み込む。なお、その読み込むデーター
		// は、バイト型配列（バイナリー）として読み込む。
		// 戻り値: 取得データー（バイト型配列）
		public byte[] ReadBytes()
		{

			//------------
			// ヘッダー読み込み
			//------------


			// Int32型のデーターのサイズ（Int32型サイズ）
			Int32 Int_Size = sizeof(Int32);


			// Int32型データー用バイト型配列
			// 後にint型データーを、バイト型配列に格納する時に使う。
			byte[] Int32Bytes = new Byte[Int_Size];


			// ストリームからヘッダー（先頭に格納する情報）を読み込む。
			// なお、ヘッダーは、データー本体のサイズの情報（Int32型）と
			// する仕様にしてあります。
			ioStream.Read(Int32Bytes, 0, Int_Size);



			// 取得したデーターをバイト型配列からint型に変換
			int len = BitConverter.ToInt32(Int32Bytes, 0);


			//------------
			// データー本体読み込み
			//------------


			// データー本体用バイト型配列
			// 後にデーター本体を、バイト型配列に格納する時に使う。
			byte[] inBuffer = new byte[len];


			// ストリームからデーター本体を読み込む。
			ioStream.Read(inBuffer, 0, len);


			// 取得したデーター値（バイト型配列）を返却
			return inBuffer;

		}



		//============
		// バイト型配列（バイナリー）書き込みメソッド
		// ストリームへデーターを書き込む。なお、その書き込むデーター
		// は、バイト型配列（バイナリー）を書き込む。
		// 第１引数: 設定データー（バイト型配列）
		public void WriteBytes(byte[] outBuffer)
		{

			//------------
			// ヘッダー書き込み
			//------------


			// ヘッダーとする情報（先頭に格納する情報）を取得。
			// なお、ヘッダーは、データー本体のサイズの情報（Int32型）と
			// する仕様にしてあります。
			int len = outBuffer.Length;


			// 設定データー（ヘッダー）をバイト型配列に変換
			byte[] Int32Bytes = BitConverter.GetBytes(len);


			// ストリームへヘッダー（先頭に格納する情報）を書き込む。
			ioStream.Write(Int32Bytes, 0, Int32Bytes.Length);


			//------------
			// データー本体書き込み
			//------------


			// ストリームへデーター本体を書き込む。
			ioStream.Write(outBuffer, 0, len);


			// ストリームをフラッシュ
			ioStream.Flush();

		}


		//============
		// string型（文字列）読み込みメソッド
		// ストリームからデーターを読み込む。なお、その読み込むデーター
		// は、string型（文字列）として読み込む。
		// 第１引数: 受信電文のｴﾝｺｰﾄﾞを入力指定(ｼﾌﾄJISｺｰﾄﾞ時は、"shift_jis")
		// 戻り値: 取得データー（string型）
		//
		// ｴﾝｺｰﾃﾞｨﾝｸﾞを示すｺｰﾄﾞﾍﾟｰｼﾞ名(第2引数指定値)
		//   ｼﾌﾄJIS: "shift_jis"
		//   JIS: "iso-2022-jp"
		//   日本語EUC: "euc-jp"
		//   Unicode (UTF-8): "utf-8"
		public string ReadString(string BeforeEncod)
		{

			// バイト型配列（バイナリー）読み込みメソッドを実行
			//  --- ストリームからデーターを取得する。ただし、下記メソッド
			//      では、バイト型配列（バイナリー）として読み込む。
			byte[] inBuffer = ReadBytes();


			// 取得したデーターをバイト型配列からstring型に変換
			// （ﾊﾞｲﾄ型配列 ---> ﾕﾆｺｰﾄﾞstring 変換）
			string data;
			StringBytesController.ChangeBinaryToUnicodeStr(
				inBuffer,
				BeforeEncod,
				out data);


			// 取得したデーター値（string型）を返却
			return data;

		}



		//============
		// string型（文字列）書き込みメソッド
		// ストリームへデーターを書き込む。なお、その書き込むデーター
		// は、string型（文字列）を書き込む。
		// 第１引数: 設定データー（string型）
		// 第２引数: 電文のｴﾝｺｰﾄﾞを入力指定(ｼﾌﾄJISｺｰﾄﾞ時は、"shift_jis")
		//
		// ｴﾝｺｰﾃﾞｨﾝｸﾞを示すｺｰﾄﾞﾍﾟｰｼﾞ名(第2引数指定値)
		//   ｼﾌﾄJIS: "shift_jis"
		//   JIS: "iso-2022-jp"
		//   日本語EUC: "euc-jp"
		//   Unicode (UTF-8): "utf-8"
		public void WriteString(
			string outString,
			string AfterEncod)
		{

			// 設定データーをバイト型配列に変換
			// （ﾕﾆｺｰﾄﾞstring ---> ﾊﾞｲﾄ型配列 変換）
			byte[] outBuffer;
			StringBytesController.ChangeUnicodeStrToBinary(
				outString,
				AfterEncod,
				out outBuffer);


			// バイト型配列（バイナリー）書き込みメソッドを実行
			//  --- ストリームへデーターを設定する。下記メソッド
			//      では、バイト型配列（バイナリー）を書き込む。
			WriteBytes(outBuffer);

		}


	}

	// ************************************
	// 文字列用バイト型配列制御クラス
	// ************************************
	public class StringBytesController
	{


		//============
		// ﾕﾆｺｰﾄﾞstring ---> 指定ｺｰﾄﾞﾊﾞｲﾄ型配列 変換ﾒｿｯﾄﾞ
		//
		// 内容: ﾕﾆｺｰﾄﾞの文字列を指定ｺｰﾄﾞ(例: ｼﾌﾄJISｺｰﾄﾞ)のﾊﾞｲﾄ型配列に変換する。
		//
		// 第1引数: 変換前の文字列を入力指定(string型のﾕﾆｺｰﾄﾞの文字列)
		// 第2引数: 変換後のｴﾝｺｰﾄﾞを入力指定(ｼﾌﾄJISｺｰﾄﾞ時は、"shift_jis")
		// 第3引数: 変換後の文字列を出力(byte型配列のｼﾌﾄJISｺｰﾄﾞの文字列)
		//
		// 戻り値: ｴﾗｰ情報(0から1: 成功、2以上: 失敗)
		// _______________(1: 警告(ﾌｫｰﾙﾊﾞｯｸ発生))
		//
		// ｴﾝｺｰﾃﾞｨﾝｸﾞを示すｺｰﾄﾞﾍﾟｰｼﾞ名(第2引数指定値)
		//   ｼﾌﾄJIS: "shift_jis"
		//   JIS: "iso-2022-jp"
		//   日本語EUC: "euc-jp"
		//   Unicode (UTF-8): "utf-8"
		public static int ChangeUnicodeStrToBinary(string UnicodeText, string AfterEncod, out byte[] AfterBytes)
		{

			// 初期化
			AfterBytes = null;
			int ErrorInfo = 0; // ｴﾗｰ情報


			// 入力がnull時のｴﾗｰ処理
			if (UnicodeText == null || AfterEncod == null) return 2;

			// 文字ｺｰﾄﾞを示すｴﾝｺｰﾄﾞを生成
			System.Text.Encoding AfterCodeEncodObj = System.Text.Encoding.GetEncoding(AfterEncod); // 変換後ｴﾝｺｰﾄﾞ
			System.Text.Encoding BeforeEncodObj = System.Text.Encoding.Unicode; // 変換前ｴﾝｺｰﾄﾞ

			// ﾕﾆｺｰﾄﾞ文字列をﾊﾞｲﾅﾘ格納用配列に設定
			byte[] UnicodeBytes = BeforeEncodObj.GetBytes(UnicodeText);

			// ﾕﾆｺｰﾄﾞを指定ｺｰﾄﾞに変換
			try
			{
				AfterBytes = System.Text.Encoding.Convert(BeforeEncodObj, AfterCodeEncodObj, UnicodeBytes);
			}
			catch (DecoderFallbackException)
			{
				// ﾌｫｰﾙﾊﾞｯｸが発生
				ErrorInfo = 1;
			}
			catch (EncoderFallbackException)
			{
				// ﾌｫｰﾙﾊﾞｯｸが発生
				ErrorInfo = 1;
			}
			catch (ArgumentNullException)
			{
				// 入出力文字列格納用ｵﾌﾞｼﾞｪｸﾄ または encoding が null を参照し、失敗
				ErrorInfo = 2;
			}

			return ErrorInfo;
		}



		//============
		// 指定ｺｰﾄﾞﾊﾞｲﾄ型配列 ---> ﾕﾆｺｰﾄﾞstring 変換ﾒｿｯﾄﾞ
		//
		// 内容: 指定ｺｰﾄﾞ(例: ｼﾌﾄJISｺｰﾄﾞ)のﾊﾞｲﾄ型配列をﾕﾆｺｰﾄﾞの文字列に変換する。
		//
		// 第1引数: 変換前の文字列を入力指定(byte型配列のｼﾌﾄJISｺｰﾄﾞの文字列)
		// 第2引数: 変換前のｴﾝｺｰﾄﾞを入力指定(ｼﾌﾄJISｺｰﾄﾞ時は、"shift_jis")
		// 第3引数: 変換後の文字列を出力(string型のﾕﾆｺｰﾄﾞの文字列)
		//
		// 戻り値: ｴﾗｰ情報(0から1: 成功、2以上: 失敗)
		// _______________(1: 警告(ﾌｫｰﾙﾊﾞｯｸ発生))
		//
		// ｴﾝｺｰﾃﾞｨﾝｸﾞを示すｺｰﾄﾞﾍﾟｰｼﾞ名(第2引数指定値)
		//   ｼﾌﾄJIS: "shift_jis"
		//   JIS: "iso-2022-jp"
		//   日本語EUC: "euc-jp"
		//   Unicode (UTF-8): "utf-8"
		public static int ChangeBinaryToUnicodeStr(byte[] BeforeBytes, string BeforeEncod, out string UnicodeText)
		{

			// 初期化
			UnicodeText = "";
			int ErrorInfo = 0; // ｴﾗｰ情報

			// 入力がnull時のｴﾗｰ処理
			if (BeforeBytes == null || BeforeEncod == null) return 2;

			// 文字ｺｰﾄﾞを示すｴﾝｺｰﾄﾞを生成
			System.Text.Encoding AfterEncodObj = System.Text.Encoding.Unicode; // 変換後ｴﾝｺｰﾄﾞ
			System.Text.Encoding BeforeEncodObj = System.Text.Encoding.GetEncoding(BeforeEncod); // 変換前ｴﾝｺｰﾄﾞ

			// 指定ｺｰﾄﾞをﾕﾆｺｰﾄﾞに変換
			byte[] AfterBytes = null;
			try
			{
				AfterBytes = System.Text.Encoding.Convert(BeforeEncodObj, AfterEncodObj, BeforeBytes);
			}
			catch (DecoderFallbackException)
			{
				// ﾌｫｰﾙﾊﾞｯｸが発生
				ErrorInfo = 1;
			}
			catch (EncoderFallbackException)
			{
				// ﾌｫｰﾙﾊﾞｯｸが発生
				ErrorInfo = 1;
			}
			catch (ArgumentNullException)
			{
				// 入出力文字列格納用ｵﾌﾞｼﾞｪｸﾄ または encoding が null を参照し、失敗
				ErrorInfo = 2;
			}


			// ﾊﾞｲﾄ型配列のﾕﾆｺｰﾄﾞ文字列をstring型に設定
			char[] AfterChars = new char[AfterEncodObj.GetCharCount(AfterBytes, 0, AfterBytes.Length)];
			AfterEncodObj.GetChars(AfterBytes, 0, AfterBytes.Length, AfterChars, 0);
			UnicodeText = new string(AfterChars);

			return ErrorInfo;
		}


	}
}
