using System;
using System.Windows.Controls;
using System.ComponentModel;
using System.Windows;
using System.Collections.Generic;
using System.Linq;

using System.Diagnostics;
using System.Windows.Media;
using System.Windows.Input;

namespace ControlsUnit
{
	static class Constants
	{
		public const int settingMax = 150;			//Home高度を基準 +150m
		public const int settingMin = -50;			//Home高度を基準 -50m
		public const int viewMax = 30;				//表示範囲の最大値 +30m
		public const int viewMin = -10;				//表示範囲の最小値 -10m

		public const int interval = 10;				// 長押し開始時間
		public const int commandInterval = 500;		//Command送信間隔 500msec

		// コマンドボタン
		public const string commandName1 = "WaypointUp10";
		public const string commandName2 = "WaypointUp";
		public const string commandName3 = "WaypointDown";
		public const string commandName4 = "WaypointDown10";
	}
    /// <summary>
    /// UAVAltitude.xaml の相互作用ロジック
    /// </summary>
    public partial class UAVAltitude : UserControl
    {
		private int _mode;		//Controlモード
		private int _fmode;		//Flightモード
		private double _baseAlt;
		private double _homeAlt;
		private double _nwaypointAlt;
		private bool _deviceMode = true;

		private System.Windows.Forms.Timer CommandTimer;        // 長押し処理用（押し間違い防止）
		private int CommandFlag;								// Commandボタン複数押し防止
		private bool _waypointUpdate = false;

		#region 初期化
		public UAVAltitude()
		{
			InitializeComponent();

			CommandFlag = 0;
			_fmode = _mode = 0;
			_baseAlt = 0;
			// コマンド操作　長押し機能
			CommandTimer = new System.Windows.Forms.Timer();
			CommandTimer.Tick += new EventHandler(CommandTimer_Tick);
		}
		private void UserControl_Loaded(object sender, RoutedEventArgs e)
		{
			ChangeTouchMouse(_deviceMode);
		}
		//　マウス・タッチ操作イベント切替（タッチ操作時のマウスイベント発生防止）
		private void ChangeTouchMouse(bool f)
		{
			if (f)
			{
				// コマンドボタン
				this.WaypointUp10.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseDown);
				this.WaypointUp10.MouseLeftButtonUp += new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseUp);
				this.WaypointUp.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseDown);
				this.WaypointUp.MouseLeftButtonUp += new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseUp);
				this.WaypointDown.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseDown);
				this.WaypointDown.MouseLeftButtonUp += new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseUp);
				this.WaypointDown10.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseDown);
				this.WaypointDown10.MouseLeftButtonUp += new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseUp);

				this.WaypointUp10.TouchDown -= new EventHandler<TouchEventArgs>(this.Command_TouchDown);
				this.WaypointUp10.TouchUp -= new EventHandler<TouchEventArgs>(this.Command_TouchUp);
				this.WaypointUp.TouchDown -= new EventHandler<TouchEventArgs>(this.Command_TouchDown);
				this.WaypointUp.TouchUp -= new EventHandler<TouchEventArgs>(this.Command_TouchUp);
				this.WaypointDown.TouchDown -= new EventHandler<TouchEventArgs>(this.Command_TouchDown);
				this.WaypointDown.TouchUp -= new EventHandler<TouchEventArgs>(this.Command_TouchUp);
				this.WaypointDown10.TouchDown -= new EventHandler<TouchEventArgs>(this.Command_TouchDown);
				this.WaypointDown10.TouchUp -= new EventHandler<TouchEventArgs>(this.Command_TouchUp);
			}
			else
			{
				// コマンドボタン
				this.WaypointUp10.MouseLeftButtonDown -= new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseDown);
				this.WaypointUp10.MouseLeftButtonUp -= new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseUp);
				this.WaypointUp.MouseLeftButtonDown -= new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseDown);
				this.WaypointUp.MouseLeftButtonUp -= new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseUp);
				this.WaypointDown.MouseLeftButtonDown -= new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseDown);
				this.WaypointDown.MouseLeftButtonUp -= new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseUp);
				this.WaypointDown10.MouseLeftButtonDown -= new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseDown);
				this.WaypointDown10.MouseLeftButtonUp -= new System.Windows.Input.MouseButtonEventHandler(this.Command_MouseUp);

				this.WaypointUp10.TouchDown += new EventHandler<TouchEventArgs>(this.Command_TouchDown);
				this.WaypointUp10.TouchUp += new EventHandler<TouchEventArgs>(this.Command_TouchUp);
				this.WaypointUp.TouchDown += new EventHandler<TouchEventArgs>(this.Command_TouchDown);
				this.WaypointUp.TouchUp += new EventHandler<TouchEventArgs>(this.Command_TouchUp);
				this.WaypointDown.TouchDown += new EventHandler<TouchEventArgs>(this.Command_TouchDown);
				this.WaypointDown.TouchUp += new EventHandler<TouchEventArgs>(this.Command_TouchUp);
				this.WaypointDown10.TouchDown += new EventHandler<TouchEventArgs>(this.Command_TouchDown);
				this.WaypointDown10.TouchUp += new EventHandler<TouchEventArgs>(this.Command_TouchUp);
			}
		}
		#endregion

		#region 表示範囲の調整
		// Waypoint上限下限設定の制限
		private double WaypointAltValueLimit(double _wAlt)
		{
			if (_homeAlt + Constants.settingMax <= _wAlt)		
			{
				Waypoint.Value = _homeAlt + Constants.settingMax;
			}
			if (_homeAlt + Constants.settingMin >= _wAlt)		
			{
				Waypoint.Value = _homeAlt + Constants.settingMin;
			}

			return Waypoint.Value;
		}
		// 高度表示の自動調整
		// _hAlt = ホーム高度
		// _nAlt =機体内部Waypoint
		// _wAlt =基地局Waypoint
		private void AltitudeView(double _hAlt, double _nAlt, double _wAlt)
		{
			List<double> _alt = new List<double>();

			_alt.Add(_hAlt - _baseAlt);
			_alt.Add(_nAlt - _baseAlt);
			_alt.Add(_wAlt - _baseAlt);

			int _viewMax = (int)_alt.Max();
			int _viewMin = (int)_alt.Min();
			int _settingMax = (int)(_hAlt + Constants.settingMax);
			int _settingMin = (int)(_hAlt + Constants.settingMin);

			if (_settingMax > _viewMax)										//最大上限値
			{
				int _max = _viewMax + Constants.viewMax;
				if (_settingMax < _max)
				{
					Dummy.Maximum = WaypointAct.Maximum = Home.Maximum = altval.Maximum = Waypoint.Maximum = Uav.Maximum = _settingMax;
				}
				else
				{
					Dummy.Maximum = WaypointAct.Maximum = Home.Maximum = altval.Maximum = Waypoint.Maximum = Uav.Maximum = _max;		//表示上限
				}
				Debug.WriteLine("高度MAX：" + Waypoint.Maximum.ToString());
			}
			if (_settingMin < _viewMin)										//最小下限値
			{
				int _min = _viewMin + Constants.viewMin;
				if (_settingMin > _min)
				{
					Dummy.Minimum = WaypointAct.Minimum = Home.Minimum = altval.Minimum = Waypoint.Minimum = Uav.Minimum = _settingMin;
				}
				else
				{
					Dummy.Minimum = WaypointAct.Minimum = Home.Minimum = altval.Minimum = Waypoint.Minimum = Uav.Minimum = _min;		//表示下限
				}
				Debug.WriteLine("高度MIN：" + Waypoint.Minimum.ToString());
			}
		}
		#endregion

		#region コマンドボタン
		// コマンドボタン長押し時
		private void CommandButtonDown()
		{
			switch (CommandFlag)
			{
				case 1:
					#region AltUp
					WaypointUp10.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
					WaypointUp10Border.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
					break;
					#endregion
				case 2:
					#region AltDown
					WaypointUp.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
					WaypointUpBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
					break;
					#endregion
				case 3:
					#region AziUp
					WaypointDownBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
					WaypointDown.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
					break;
					#endregion
				case 4:
					#region AziDown
					WaypointDown10Border.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
					WaypointDown10.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
					break;
					#endregion
				default:
					break;

			}
		}
		private void CommandButtonUp()
		{
			switch (CommandFlag)
			{
				case 1:
					#region AltUp
					WaypointUp10.Background = Brushes.White;
					WaypointUp10Border.BorderBrush = Brushes.White;
					break;
					#endregion
				case 2:
					#region AltDown
					WaypointUpBorder.BorderBrush = Brushes.White;
					WaypointUp.Background = Brushes.White;
					break;
					#endregion
				case 3:
					#region AziUp
					WaypointDownBorder.BorderBrush = Brushes.White;
					WaypointDown.Background = Brushes.White;
					break;
					#endregion
				case 4:
					#region AziDown
					WaypointDown10Border.BorderBrush = Brushes.White;
					WaypointDown10.Background = Brushes.White;
					break;
					#endregion
				default:
					break;

			}
		}

		// 押されたコマンドボタンを取得
		private int CommandButtonMode(string str)
		{
			int mode = 0;

			switch (str)
			{
				case Constants.commandName1:
					mode = 1;
					break;
				case Constants.commandName2:
					mode = 2;
					break;
				case Constants.commandName3:
					mode = 3;
					break;
				case Constants.commandName4:
					mode = 4;
					break;
				default:
					break;
			}

			return mode;
		}
		// コマンドボタンイベント
		private void Command_MouseDown(object sender, MouseButtonEventArgs e)
		{
			System.Windows.Controls.Label _command = sender as System.Windows.Controls.Label;

			if (_command != null)
			{
				string str = (string)_command.Content;

				CommandFlag = CommandButtonMode(str);

				CommandTimer.Interval = Constants.interval;   //wait
				CommandTimer.Enabled = true;
				CommandTimer.Start();
				if (!_command.IsMouseCaptured)
				{
					_command.CaptureMouse();
				}
			}
		}
		private void Command_MouseUp(object sender, MouseButtonEventArgs e)
		{
			System.Windows.Controls.Label _command = sender as System.Windows.Controls.Label;

			if (_command != null)
			{
				CommandButtonUp();  // ボタンの色を戻す

				if (_command.IsMouseCaptured)
				{
					_command.ReleaseMouseCapture();
				}
				if (CommandTimer.Enabled)
				{
					CommandTimer.Enabled = false;
					CommandTimer.Stop();
					CommandFlag = 0;
				}
			}
		}
		private void Command_TouchDown(object sender, TouchEventArgs e)
		{
			System.Windows.Controls.Label _command = sender as System.Windows.Controls.Label;

			if (_command != null)
			{
				string str = (string)_command.Content;

				CommandFlag = CommandButtonMode(str);

				CommandTimer.Interval = Constants.interval;   //wait
				CommandTimer.Enabled = true;
				CommandTimer.Start();
				_command.CaptureTouch(e.TouchDevice);
			}
		}
		private void Command_TouchUp(object sender, TouchEventArgs e)
		{
			System.Windows.Controls.Label _command = sender as System.Windows.Controls.Label;

			if (_command != null)
			{
				CommandButtonUp();  // ボタンの色を戻す
				_command.ReleaseTouchCapture(e.TouchDevice);
				if (CommandTimer.Enabled)
				{
					CommandTimer.Enabled = false;
					CommandTimer.Stop();
					CommandFlag = 0;
				}
			}
		}

		// 長押し処理
		private void CommandTimer_Tick(object sender, EventArgs e)
		{
			try
			{
				if (CommandTimer.Enabled)
				{
					// Commandボタン長押しを色変更で表示
					CommandButtonDown();

					switch (CommandFlag)
					{
						case 1:
							Waypoint.Value = (int)(Waypoint.Value + 10);
							break;
						case 2:
							Waypoint.Value = (int)(Waypoint.Value + 1);
							break;
						case 3:
							Waypoint.Value = (int)(Waypoint.Value - 1);
							break;
						case 4:
							Waypoint.Value = (int)(Waypoint.Value - 10);
							break;
						default:
							break;
					}

					if (CommandTimer.Interval == Constants.interval)
					{
						CommandTimer.Interval = Constants.commandInterval;
					}
				}
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("Command: " + ex.Message);
			}

		}
		#endregion

        #region イベント
		/// <summary>
		/// 値変更時に呼ばれるイベントです。
		/// </summary>
		public static readonly RoutedEvent TargetValueChangedEvent =
			EventManager.RegisterRoutedEvent(
				"TargetValueChanged", RoutingStrategy.Bubble,
				typeof(RoutedPropertyChangedEventHandler<double>),
				typeof(RotateAzimuthal));

		/// <summary>
		/// 値変更時に呼ばれるイベントです。
		/// </summary>
		public event RoutedPropertyChangedEventHandler<double> TargetValueChanged
		{
			add { AddHandler(TargetValueChangedEvent, value); }
			remove { RemoveHandler(TargetValueChangedEvent, value); }
		}
        #endregion

		#region プロパティ
		public double HomeAlt
        {
            get { return this._homeAlt; }
			set
			{
				this._homeAlt = value;
				Home.Value = this._homeAlt - _baseAlt;
			}
        }
		public double NWaypointAlt
        {
            get { return this._nwaypointAlt; }
            set { this._nwaypointAlt = value;
			WaypointAct.Value = this._nwaypointAlt;
			}
        }
		public int ControlMode
		{
			get { return this._mode; }
			set { this._mode = value;
				// 留意点：モード変更に伴い変更が必要
				// 自律ナビゲーション
				if (_mode == 4)
				{
					//if (Home.Visibility != Visibility.Visible)
					//{
					//	Home.Visibility = Visibility.Visible;
					//}
					if (WaypointAct.Visibility != Visibility.Visible)
					{
						WaypointAct.Visibility = Visibility.Visible;
					}
					if (_fmode == 1)
					{
						if (Waypoint.Visibility != Visibility.Visible)
						{
							Waypoint.Visibility = Visibility.Visible;
						}
					}
					//if (Dummy.Visibility != Visibility.Visible)
					//{
					//	Dummy.Visibility = Visibility.Visible;
					//}
				}
				else
				{
					//if (Home.Visibility != Visibility.Hidden)
					//{
					//	Home.Visibility = Visibility.Hidden;
					//}
					if (WaypointAct.Visibility != Visibility.Hidden)
					{
						WaypointAct.Visibility = Visibility.Hidden;
					}
					if (Waypoint.Visibility != Visibility.Hidden)
					{
						Waypoint.Visibility = Visibility.Hidden;
					}
					//if (Dummy.Visibility != Visibility.Hidden)
					//{
					//	Dummy.Visibility = Visibility.Hidden;
					//}
				}
			}
		}
		public int FlightMode
		{
			get { return this._fmode; }
			set
			{
				int f = value;
				if (this._fmode != f)
				{
					this._fmode = value;
					if (Waypoint.Visibility != Visibility.Hidden)
					{
						Waypoint.Visibility = Visibility.Hidden;
					}
					// 留意点：モード変更に伴い変更が必要
					// 計画飛行
					//if (_fmode == 1)
					//{
					//	if (Waypoint.Visibility != Visibility.Hidden)
					//	{
					//		Waypoint.Visibility = Visibility.Hidden;
					//	}
					//}
					//else
					//{
					//	if (Waypoint.Visibility != Visibility.Visible)
					//	{
					//		Waypoint.Visibility = Visibility.Visible;
					//	}
					//}
				}

				
			}
		}
		#endregion

		#region プロパティ タッチ/マウス
		public bool DeviceMode
		{
			get { return this._deviceMode; }
			set { this._deviceMode = value;
				ChangeTouchMouse(_deviceMode);
				}
		}
		#endregion
		
		#region プロパティ 目標高度（Waypoint高度）
		/// <summary>
        /// 数値データの依存プロパティです。
        /// </summary>
        public static readonly DependencyProperty TargetAltitudeProperty =
            DependencyProperty.Register(
                "TargetAltitude", typeof(double), typeof(UAVAltitude),
                new FrameworkPropertyMetadata(
                    default(double),
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    OnTargetAltitudeChanged));
        /// <summary>
        ///
        /// </summary>         
        [Category("ControlsUnit")]
        [Description("")]
        [Browsable(true)]
        [Bindable(true)]
        public double WaypointAlt
        {
            get { return (double)GetValue(TargetAltitudeProperty); }
            set 
			{ 
				SetValue(TargetAltitudeProperty, value);
				Waypoint.Value = value;
				Waypoint.Value -= _baseAlt;

				int a = (int)(((double)value) * 10);
				NNuav.Content = "(" + (a * 0.1).ToString() + ")";
			}
        }
		/// <summary>
		/// 値の変更時に呼ばれます。
		/// </summary>
		private static void OnTargetAltitudeChanged(DependencyObject d,
										   DependencyPropertyChangedEventArgs e)
		{
			UAVAltitude control = (UAVAltitude)d;

			//control.Waypoint.Value = (double)e.NewValue;

			RoutedPropertyChangedEventArgs<double> args = new RoutedPropertyChangedEventArgs<double>(
				(double)e.OldValue, (double)e.NewValue, TargetValueChangedEvent);
			control.OnTargetAltitudeChanged(args);
		}
		protected virtual void OnTargetAltitudeChanged(RoutedPropertyChangedEventArgs<double> args)
		{
			RaiseEvent(args);
		}
        #endregion

        #region プロパティ　現在高度（UAV高度）
		private static void OnAltitudeChanged(DependencyObject d,
										  DependencyPropertyChangedEventArgs e)
		{
			UAVAltitude control = (UAVAltitude)d;
			control.Uav.Value = ((double)e.NewValue - control._baseAlt);
			int a = (int)((double)e.NewValue * 10);
			double target = a;

			control.Nalt.Content = (target * 0.1).ToString();

			a = (int)(((double)e.NewValue - control._baseAlt - control.altval.Value) * 10);
			target = a;

			control.NNalt.Content = "(" + (target * 0.1).ToString() + ")";
		}
        /// <summary>
        /// 数値データの依存プロパティです。
        /// </summary>
        public static readonly DependencyProperty AltitudeProperty =
            DependencyProperty.Register(
                "Altitude", typeof(double), typeof(UAVAltitude),
                new FrameworkPropertyMetadata(
                    default(double),
                    FrameworkPropertyMetadataOptions.None,
                    OnAltitudeChanged));
        /// <summary>
        ///
        /// </summary>         
        [Category("ControlsUnit")]
        [Description("")]
        [Browsable(true)]
        [Bindable(true)]
        public double UavAlt
        {
            get { return (double)GetValue(AltitudeProperty); }
            set { SetValue(AltitudeProperty, value); }
        }
        #endregion

        #region プロパティ　標高バー
        /// <summary>
        /// 値の変更時に呼ばれます。
        /// </summary>

        public static readonly DependencyProperty ElevationValueProperty = DependencyProperty.Register(
        "ElevationValue", typeof(double), typeof(UAVAltitude), new UIPropertyMetadata(
        0.0d,
        (d, e) =>
        {
			(d as UAVAltitude).altval.Value = (double)e.NewValue;
        }));

        /// <summary>
        /// 
        /// </summary>         
        [Category("ControlsUnit")]
        [Description("Elevation Value")]
        public double ElevationValue
        {
            get { return (double)this.GetValue(ElevationValueProperty); }
            set { this.SetValue(ElevationValueProperty, (double)value); }
        }
        #endregion

		// 目標高度指定スライドのイベント
		private void Waypoint_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			int a;

			a = (int)((e.NewValue + _baseAlt) * 10);
			Waypoint.Value = (double)e.NewValue;

			double target = a;

			Nuav.Content = (target * 0.1).ToString();				// 数値表示

			a = (int)(((double)e.NewValue) * 10);
			target = a;
			NNuav.Content = "(" + (target * 0.1).ToString() + ")";

			SetValue(TargetAltitudeProperty, (double)e.NewValue + _baseAlt);

			this.AltitudeView(_homeAlt - _baseAlt, _nwaypointAlt - _baseAlt, Waypoint.Value);	//表示範囲の調整
		}

		// オフセット設定
		private void Offset_Click(object sender, RoutedEventArgs e)
		{
			_baseAlt = UavAlt;
			//Waypoint.Value -= _baseAlt;
			//Uav.Value -= _baseAlt;
			//Home.Value -= _baseAlt;
		}
		
    }
}
