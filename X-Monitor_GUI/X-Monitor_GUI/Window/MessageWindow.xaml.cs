using System.Text;
using System.Windows;
using Socket;
using System.Net;
using System.IO;
using System.Windows.Forms;     //参照追加

namespace XMonitor_GUI
{
	/// <summary>
	/// MessageWindow.xaml の相互作用ロジック
	/// </summary>
	public partial class MessageWindow
	{
		public bool flag;
		public bool Online; // true:Online　False：Offline
		string MessageNumber="";

		public MessageWindow()
		{
			InitializeComponent();
		}

		// 1.9.1「×」ボタン
		private void Button_Click(object sender, RoutedEventArgs e)
		{
			// Dialog box canceled
			this.DialogResult = false;
			//Close();
		}

		// 「OK」ボタン
		private void Button_Click_1(object sender, RoutedEventArgs e)
		{
			this.DialogResult = true;
		}

        public bool Initialize()
        {
			try
			{
				//var _nowTime = System.DateTime.Now;										//現在日時
				//var _newDay = Properties.Settings.Default.ACSLMessageDate.AddMonths(1);	//１か月後の日時
				//int compareValue = _nowTime.CompareTo(_newDay);

				//if (compareValue < 0)
				//{
				//	// １か月前
				//	if (!Properties.Settings.Default.ACSLMessageFlag)
				//	{
				//		return Messagere();
				//	}
				//}
				//else 
				//{
				//	// １か月後
				//	return Messagere();
				//}

				return Messagere();
			}
			catch
			{
                return false;
			}
        }

		private bool Messagere()
		{
			try
			{
				string _message;

				WebClient wc = new WebClient();
				Stream st = wc.OpenRead("http://acsl.co.jp/ACSL043_Message287_5702.xml");
				StreamReader sr = new StreamReader(st, Encoding.GetEncoding("utf-8"));

				_message = sr.ReadToEnd();

				sr.Close();
				st.Close();
				wc.Dispose();
				Online = true;
				UpdataText.Text = _message;

				if (_message.IndexOf("message") != -1)
				{
					var _m = XmlBase.XmlConvert.GetMessage(_message, "X-Monitor");
					MessageNumber = _m.MessageVersion;
					//UpdataText.Text = "Version [" + Properties.Settings.Default.XMonitorVersion + "] -> [" + _m.UpadataVersion + "]";
					//UpdataText.Text += _m.UpadataMessage;
					//MessageText.Text = _m.InformationMessage;

					if (_m.MessageVersion != Properties.Settings.Default.ACSLMessageNumber.ToString())
					{
						UpdataText.Text = "Version [" + Properties.Settings.Default.XMonitorVersion + "] -> [" + _m.UpadataVersion + "]";
						UpdataText.Text += _m.UpadataMessage;
						MessageText.Text = _m.InformationMessage;
					}
					else
					{
						if (flag)
						{
							UpdataText.Text = "Version [" + Properties.Settings.Default.XMonitorVersion + "] -> [" + _m.UpadataVersion + "]";
							UpdataText.Text += _m.UpadataMessage;
							MessageText.Text = _m.InformationMessage;
						}
						else
						{
							return false;
						}
					}
				}

				return true;
			}
			catch
			{
				Online = false;
				return false;
			}
		}

        private void Window_Loaded(object sender, RoutedEventArgs e)
        { 
        }

		private void CheckBox_Checked(object sender, RoutedEventArgs e)
		{
			try
			{
				// 次回更新まで表示しない
				if (MessageNumber != "")
				{
					Properties.Settings.Default.ACSLMessageDate = System.DateTime.Now;
					Properties.Settings.Default.ACSLMessageFlag = true;
					Properties.Settings.Default.ACSLMessageNumber = long.Parse(MessageNumber);
					Properties.Settings.Default.Save();
				}

			}
			catch
			{

			}
		}
	}
}
