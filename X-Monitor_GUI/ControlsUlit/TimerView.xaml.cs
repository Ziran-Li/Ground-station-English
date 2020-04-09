using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace ControlsUnit
{
	/// <summary>
	/// TimerView.xaml の相互作用ロジック
	/// </summary>
	public partial class TimerView : UserControl
	{

		private System.Windows.Threading.DispatcherTimer mTimer;	//タイマー
		private System.Diagnostics.Stopwatch sw ;					//時間
		private System.Diagnostics.Stopwatch csw;					//時間
		private System.Diagnostics.Stopwatch _totalSW;				//時間
		private double mTime = 0.0;
		private int _mode;
		private bool _connect;

		public TimerView()
		{
			InitializeComponent();

			_mode = 0;
			_connect = false;

			//
			sw = new System.Diagnostics.Stopwatch();
			csw = new System.Diagnostics.Stopwatch();
			_totalSW = new System.Diagnostics.Stopwatch();
			sw.Start();
			csw.Start();
			_totalSW.Start();
			mTimer = new System.Windows.Threading.DispatcherTimer();
			mTimer.Interval = TimeSpan.FromMilliseconds(100); //100ミリ秒間隔に設定
			mTimer.Tick += new EventHandler(TickTimer);
			mTimer.Start();
		}

		public bool Connect
		{
			get { return (bool)_connect; }
			set
			{
				if ((bool)value != _connect)
				{
					if(_connect != (bool)value)
					{
						csw.Reset();
						csw.Start();
						_connect = value;
						if (_connect)
						{
							ConnectName.Content = "Connect";
						}
						else
						{
							ConnectName.Content = "Disconnect";
						}
					}
				}
			}
		}

		public int Mode
		{
			get { return (int)_mode; }
			set {
			if ((int)value != _mode)
			{
				if (_mode == 0)
				{
					_totalSW.Restart();
				}
				_mode = value;
				mTime = 0.0;
				sw.Restart();
			}
			}
		}

		//定期実行の間隔を設定、コールバックされるメソッド
		void TickTimer(object sender, EventArgs e)
		{
			mTime += 0.1;

			TimeSpan ts = sw.Elapsed;
			string elapsedTime = String.Format("{0:00}:{1:00}.{2:00}",
			ts.Minutes, ts.Seconds,
			ts.Milliseconds / 10);
			mTimerVal.Content = elapsedTime;

			TimeSpan ts2 = csw.Elapsed;
			string elapsedTime2 = String.Format("{0:00}:{1:00}.{2:00}",
			ts2.Minutes, ts2.Seconds,
			ts2.Milliseconds / 10);
			cTimerVal.Content = elapsedTime2;

			//TimeSpan ts2 = _totalSW.Elapsed;
			//string elapsedTime2 = String.Format("{0:00}:{1:00}.{2:00}",
			//ts2.Minutes, ts2.Seconds,
			//ts2.Milliseconds / 10);
			//TotalVal.Content = elapsedTime2;
			TotalVal.Content = DateTime.Now.ToString("HH:mm:ss");		//現在時刻
		}

		private void TotalVal_MouseDoubleClick(object sender, MouseButtonEventArgs e)
		{
			//_totalSW.Restart();
		}
	
		private void TimerVal_MouseDoubleClick(object sender, MouseButtonEventArgs e)
		{
			sw.Restart();
		}
		private void cTimerVal_MouseDoubleClick(object sender, MouseButtonEventArgs e)
		{
			csw.Restart();
		}

	}
}
