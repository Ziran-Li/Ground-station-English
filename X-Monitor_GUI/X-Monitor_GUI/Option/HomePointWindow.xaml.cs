using System.Windows.Controls;
using System.Windows;
using System;
using System.Diagnostics;
using System.Windows.Media;
using System.ComponentModel;
using System.Windows.Input;
using System.Threading.Tasks;

using Point;

namespace XMonitor_GUI
{

    /// <summary>
    /// WaypointWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class HomePointWindow : UserControl
    {
		bool EndFlag = false;							//スレッドループ
        private int Window_Id;							// オプション画面ID
        private PointData _WaypointData;				// WaypointData
        private PointData _UAVData;						// WaypointData
        private bool _WaypointUpdate;					//
        System.Windows.Forms.Timer CommandTimer;        // 長押し処理用（押し間違い防止）
        int CommandFlag;								// Commandボタン複数押し防止
        private const int interval = 10;				// 長押し開始時間

		public HomePointWindow(int id)
        {
            InitializeComponent();

            Width = double.NaN;
            Height = double.NaN;

            Window_Id = id;
            _WaypointData = new PointData();
			_UAVData = new PointData();
			_WaypointUpdate = false;
			CommandFlag = 0;

            // コマンド操作　長押し機能
            CommandTimer = new System.Windows.Forms.Timer();
            CommandTimer.Tick += new EventHandler(CommandTimer_Tick);

			var task = Task.Factory.StartNew(() =>
			{
				EndFlag = true;
				while (EndFlag)
				{
					Dispatcher.BeginInvoke(new Action<int>(Worker), 1);
					System.Threading.Thread.Sleep(100);   //1msec
				}
			});
        }
		private void Main_Unloaded(object sender, RoutedEventArgs e)
		{
			EndFlag = false;
		}
		public void Worker(int data)
		{
			if (!_WaypointUpdate)
			{
				PostionLat.Value = _WaypointData.Lat;
				PostionLng.Value = _WaypointData.Lon;
				PostionAlt.Value = _WaypointData.Alt;
				PostionAzi.Value = _WaypointData.Yaw;
				PostionSpeed.Value = _WaypointData.Speed;

				var _uav = new System.Windows.Point(_UAVData.Lat, _UAVData.Lon);
				var _waypoint = new System.Windows.Point(_WaypointData.Lat, _WaypointData.Lon);
				var _p = MeterPerLatLng2(_uav, _waypoint);

				PostionLatDis.Value = _p.X;
				PostionLngDis.Value = _p.Y;
			}
		}

        // コマンドボタン長押し時
        private void CommandButtonDown()
        {
            switch (CommandFlag)
            {
                case 1:
                    #region AltUp
                    AltUp.Foreground = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    AltUpBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 2:
                    #region AltDown
                    AltDown.Foreground = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    AltDownBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 3:
                    #region AziUp
                    AziUpBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    AziUp.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 4:
                    #region AziDown
                    AziDownBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    AziDown.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 5:
                    #region LatUp
                    LatUpBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    LatUp.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 6:
                    #region LatDown
                    LatDownBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    LatDown.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 7:
                    #region LngUp
                    LngUpBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    LngUp.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 8:
                    #region LngDown
                    LngDownBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    LngDown.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 9:
                    #region MarkerSet
					MarkerSetBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
					MarkerSet.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
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
                    AltUp.Foreground = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    AltUpBorder.BorderBrush = null;
                    break;
                    #endregion
                case 2:
                    #region AltDown
                    AltDownBorder.BorderBrush = null;
                    AltDown.Foreground = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 3:
                    #region AziUp
                    AziUpBorder.BorderBrush = null;
                    AziUp.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 4:
                    #region AziDown
                    AziDownBorder.BorderBrush = null;
                    AziDown.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 5:
                    #region LatUp
                    LatUpBorder.BorderBrush = null;
                    LatUp.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 6:
                    #region LatDown
                    LatDownBorder.BorderBrush = null;
                    LatDown.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 7:
                    #region LngUp
                    LngUpBorder.BorderBrush = null;
                    LngUp.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 8:
                    #region LngDown
                    LngDownBorder.BorderBrush = null;
                    LngDown.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 9:
                    #region MarkerSet
					MarkerSetBorder.BorderBrush = null;
					MarkerSet.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
					break;
                    #endregion
                default:
                    break;

            }
        }

		private void MouseDown(object sender,int _flag)
		{
			System.Windows.Controls.Label _command = sender as System.Windows.Controls.Label;

			CommandTimer.Interval = 100;   //wait
			CommandTimer.Enabled = true;
			CommandTimer.Start();
			CommandFlag = _flag;

			if (!_command.IsMouseCaptured)
			{
				_command.CaptureMouse();
			}
		}
		private void MouseUp(object sender)
		{
			System.Windows.Controls.Label _command = sender as System.Windows.Controls.Label;

			CommandButtonUp();  // ボタンの色を戻す
			if (CommandTimer.Enabled)
			{
				CommandTimer.Enabled = false;
				CommandTimer.Stop();
				CommandFlag = 0;
			}

			if (_command.IsMouseCaptured)
			{
				_command.ReleaseMouseCapture();
			}
		}
		private void TouchDown(object sender, TouchEventArgs e, int _flag)
		{
			System.Windows.Controls.Label _command = sender as System.Windows.Controls.Label;

			CommandTimer.Interval = 100;   //wait
			CommandTimer.Enabled = true;
			CommandTimer.Start();
			CommandFlag = _flag;

			_command.CaptureTouch(e.TouchDevice);

		}
		private void TouchUp(object sender, TouchEventArgs e)
		{
			System.Windows.Controls.Label _command = sender as System.Windows.Controls.Label;

			CommandButtonUp();  // ボタンの色を戻す
			if (CommandTimer.Enabled)
			{
				CommandTimer.Enabled = false;
				CommandTimer.Stop();
				CommandFlag = 0;
			}

			_command.ReleaseTouchCapture(e.TouchDevice);

		}

        #region Alt ボタン
        // AltUpボタン
        private void AltUp_MouseDown(object sender, MouseButtonEventArgs e)
        {
			MouseDown(sender, 1);
        }
        private void AltUp_MouseUp(object sender, MouseButtonEventArgs e)
        {
			MouseUp(sender);
        }
        private void AltUp_TouchDown(object sender, TouchEventArgs e)
        {
			TouchDown(sender,e, 1);
        }
        private void AltUp_TouchUp(object sender, TouchEventArgs e)
        {
			TouchUp(sender,e);
        }
        // AltDownボタン
        private void AltDown_MouseDown(object sender, MouseButtonEventArgs e)
        {
			MouseDown(sender, 2);
        }
        private void AltDown_MouseUp(object sender, MouseButtonEventArgs e)
        {
			MouseUp(sender);
        }
        private void AltDown_TouchDown(object sender, TouchEventArgs e)
        {
			TouchDown(sender, e, 2);
        }
        private void AltDown_TouchUp(object sender, TouchEventArgs e)
        {
			TouchUp(sender, e);
        }
        #endregion

        #region Azi ボタン
        // AziUpボタン
        private void AziUp_MouseDown(object sender, MouseButtonEventArgs e)
        {
			MouseDown(sender, 3);
        }
        private void AziUp_MouseUp(object sender, MouseButtonEventArgs e)
        {
			MouseUp(sender);
        }
        private void AziUp_TouchDown(object sender, TouchEventArgs e)
        {
			TouchDown(sender,e, 3);
        }
        private void AziUp_TouchUp(object sender, TouchEventArgs e)
        {
			TouchUp(sender, e);
        }
        // AziDownボタン
        private void AziDown_MouseDown(object sender, MouseButtonEventArgs e)
        {
			MouseDown(sender, 4);
        }
        private void AziDown_MouseUp(object sender, MouseButtonEventArgs e)
        {
			MouseUp(sender);
        }
        private void AziDown_TouchDown(object sender, TouchEventArgs e)
        {
			TouchDown(sender, e, 4);
        }
        private void AziDown_TouchUp(object sender, TouchEventArgs e)
        {
			TouchUp(sender, e);
        }
        #endregion

        #region Lat ボタン
        // LatUpボタン
        private void LatUp_MouseDown(object sender, MouseButtonEventArgs e)
        {
			MouseDown(sender, 5);
        }
        private void LatUp_MouseUp(object sender, MouseButtonEventArgs e)
        {
			MouseUp(sender);
        }
        private void LatUp_TouchDown(object sender, TouchEventArgs e)
        {
			TouchDown(sender, e, 5);
        }
        private void LatUp_TouchUp(object sender, TouchEventArgs e)
        {
			TouchUp(sender, e);
        }
        // LatDownボタン
        private void LatDown_MouseDown(object sender, MouseButtonEventArgs e)
        {
			MouseDown(sender, 6);
        }
        private void LatDown_MouseUp(object sender, MouseButtonEventArgs e)
        {
			MouseUp(sender);
        }
        private void LatDown_TouchDown(object sender, TouchEventArgs e)
        {
			TouchDown(sender, e, 6);
        }
        private void LatDown_TouchUp(object sender, TouchEventArgs e)
        {
			TouchUp(sender, e);
        }
        #endregion

        #region Lng ボタン
        // LngUpボタン
        private void LngUp_MouseDown(object sender, MouseButtonEventArgs e)
        {
			MouseDown(sender, 7);
        }
        private void LngUp_MouseUp(object sender, MouseButtonEventArgs e)
        {
			MouseUp(sender);
        }
        private void LngUp_TouchDown(object sender, TouchEventArgs e)
        {
			TouchDown(sender, e, 7);
        }
        private void LngUp_TouchUp(object sender, TouchEventArgs e)
        {
			TouchUp(sender, e);
        }
        // LngDownボタン
        private void LngDown_MouseDown(object sender, MouseButtonEventArgs e)
        {
			MouseDown(sender, 8);
        }
        private void LngDown_MouseUp(object sender, MouseButtonEventArgs e)
        {
			MouseUp(sender);
        }
        private void LngDown_TouchDown(object sender, TouchEventArgs e)
        {
			TouchDown(sender, e, 8);
        }
        private void LngDown_TouchUp(object sender, TouchEventArgs e)
        {
			TouchUp(sender, e);
        }
        #endregion

        #region MarkerSet ボタン
        // MarkerSetボタン
        private void MarkerSet_MouseDown(object sender, MouseButtonEventArgs e)
        {
			MouseDown(sender, 9);
        }
        private void MarkerSet_MouseUp(object sender, MouseButtonEventArgs e)
        {
			MouseUp(sender);
        }
        private void MarkerSet_TouchDown(object sender, TouchEventArgs e)
        {
			TouchDown(sender, e, 9);
        }
        private void MarkerSet_TouchUp(object sender, TouchEventArgs e)
        {
			TouchUp(sender, e);
        }
        #endregion

        // 長押し処理
        private void CommandTimer_Tick(object sender, EventArgs e)
        {
            try
            {
				_WaypointUpdate = true;
                if (CommandTimer.Enabled)
                {
                    // Commandボタン長押しを色変更で表示
                    CommandButtonDown();

                    double val;
                    switch (CommandFlag)
                    {
						case 1:			// 高度（+）
                           val = Math.Floor((double)PostionAlt.Value * 10);
							val = val / 10;
                            val += 0.1;
							//if(val > 300){	//高度入力制限 MAX値
							//	val = 300;
							//}
                            PostionAlt.Value = val;
                            break;
						case 2:			// 高度（-）
                            val = Math.Floor((double)PostionAlt.Value * 10);
							val = val / 10;
                            val -= 0.1;
							//if (val < 0)
							//{	//高度入力制限 MIN値
							//	val = 0;
							//}
                            PostionAlt.Value = val;
                            break;
						case 3:			// 姿勢（+）
							val = Math.Floor((double)(double)PostionAzi.Value);
                            val += 1;
							if (val > 180)
							{	//姿勢入力制限 MAX値
								val -= 360;
							}
                            PostionAzi.Value = val;
                            break;
						case 4:			// 姿勢（-）
							val = Math.Floor((double)PostionAzi.Value);
                            val -= 1;
							if (val > 180)
							{	//姿勢入力制限 MIN値
								val -= 360;
							}
							if(val < -180)
							{
								val += 360;
							}

                            PostionAzi.Value = val;
                            break;
						case 5:			// 緯度（+）
							{
								int _x = (int)Math.Floor((double)PostionLatDis.Value);
								int _y = (int)Math.Floor((double)PostionLngDis.Value);
								_x += 1;
								var _ori = MeterPerLatLng(new System.Windows.Point(_UAVData.Lat, _UAVData.Lon));

								double lat = _UAVData.Lat + _x / _ori.X;
								double lng = _UAVData.Lon + _y / _ori.Y;

								PostionLat.Value = lat;
								PostionLng.Value = lng;
								PostionLatDis.Value = _x;
								PostionLngDis.Value = _y;
							}
                            break;
						case 6: 
							{
								int _x = (int)Math.Floor((double)PostionLatDis.Value);
								int _y = (int)Math.Floor((double)PostionLngDis.Value);
								_x -= 1;
								var _ori = MeterPerLatLng(new System.Windows.Point(_UAVData.Lat, _UAVData.Lon));

								double lat = _UAVData.Lat + _x / _ori.X;
								double lng = _UAVData.Lon + _y / _ori.Y;

								PostionLat.Value = lat;
								PostionLng.Value = lng;
								PostionLatDis.Value = _x;
								PostionLngDis.Value = _y;
							}
                            break;
                        case 7:
							{
								int _x = (int)Math.Floor((double)PostionLatDis.Value);
								int _y = (int)Math.Floor((double)PostionLngDis.Value);
								_y += 1;
								var _ori = MeterPerLatLng(new System.Windows.Point(_UAVData.Lat, _UAVData.Lon));

								double lat = _UAVData.Lat + _x / _ori.X;
								double lng = _UAVData.Lon + _y / _ori.Y;

								PostionLat.Value = lat;
								PostionLng.Value = lng;
								PostionLatDis.Value = _x;
								PostionLngDis.Value = _y;
							}

                            break;
                        case 8:
							{
								int _x = (int)Math.Floor((double)PostionLatDis.Value);
								int _y = (int)Math.Floor((double)PostionLngDis.Value);
								_y -= 1;
								var _ori = MeterPerLatLng(new System.Windows.Point(_UAVData.Lat, _UAVData.Lon));

								double lat = _UAVData.Lat + _x / _ori.X;
								double lng = _UAVData.Lon + _y / _ori.Y;

								PostionLat.Value = lat;
								PostionLng.Value = lng;
								PostionLatDis.Value = _x;
								PostionLngDis.Value = _y;
							}
                            break;
                        case 9:
								PostionLat.Value = _UAVData.Lat;
								PostionLng.Value = _UAVData.Lon;
								PostionAlt.Value = _UAVData.Alt;
								PostionAzi.Value = _UAVData.Yaw;
								PostionLatDis.Value = 0;
								PostionLngDis.Value = 0;
                            break;
                        default:
                            break;
                    }
                    PostionSetMarkerUpdata();
                }
            }
            catch (Exception ex)
            {
				System.Windows.MessageBox.Show("(702) OptionWindow. Waypoint Input Error : " + ex.Message);
            }

        }
		// 飛行計画データのXYをLatLng変換 ()
		private System.Windows.Point MeterPerLatLng(System.Windows.Point orilat)
		{
			double GPS_ELLIPSOID_A = 6378137.0;
			double GPS_ELLIPSOID_F = 0.00335281066475;
			double GPS_ELLIPSOID_B = (GPS_ELLIPSOID_A * (1.0 - GPS_ELLIPSOID_F));

			double dPseudoLatitudeRad, dDistanceFromCE;
			double dSin, dCos;

			// 変換定数の計算
			System.Windows.Point r = new System.Windows.Point();

			//
			//Lat
			//
			// 擬似緯度の計算
			if (orilat.X == 90.0 || orilat.X == -90.0)
			{
				// tan の値が特異となる場合
				dPseudoLatitudeRad = orilat.X * Math.PI / 180.0; // この場合は擬似緯度と緯度が等しい。
			}
			else
			{
				// tan の値が特異とならない場合
				dPseudoLatitudeRad = Math.Atan((1.0 - GPS_ELLIPSOID_F) * (1.0 - GPS_ELLIPSOID_F) * Math.Tan(orilat.X * Math.PI / 180.0));
			}

			// 三角関数の計算
			dSin = Math.Sin(dPseudoLatitudeRad);
			dCos = Math.Cos(dPseudoLatitudeRad);

			// 地球中心と現在地点との距離の計算
			dDistanceFromCE = GPS_ELLIPSOID_A * GPS_ELLIPSOID_B / Math.Sqrt(GPS_ELLIPSOID_A * GPS_ELLIPSOID_A * dSin * dSin + GPS_ELLIPSOID_B * GPS_ELLIPSOID_B * dCos * dCos);

			r.X = (Math.PI / 180.0) * dDistanceFromCE;

			//
			//Lng
			//
			r.Y = (Math.PI / 180.0) * dDistanceFromCE * dCos;


			return r;
		}
		private System.Windows.Point MeterPerLatLng2(System.Windows.Point _orilat, System.Windows.Point _postion)
		{
			System.Windows.Point _point = new System.Windows.Point();

			double dPseudoLatitudeRad, dDistanceFromCE;
			double dSin, dCos;

			// 変換定数の計算
			System.Windows.Point r = new System.Windows.Point();

			//
			//Lat
			//
			// 擬似緯度の計算
			if (_orilat.X == 90.0 || _orilat.X == -90.0)
			{
				// tan の値が特異となる場合
				dPseudoLatitudeRad = _orilat.X * Math.PI / 180.0; // この場合は擬似緯度と緯度が等しい。
			}
			else
			{
				// tan の値が特異とならない場合
				dPseudoLatitudeRad = Math.Atan((1.0 - ConstantsGPS.ELLIPSOID_F) * (1.0 - ConstantsGPS.ELLIPSOID_F) * Math.Tan(_orilat.X * Math.PI / 180.0));
			}

			// 三角関数の計算
			dSin = Math.Sin(dPseudoLatitudeRad);
			dCos = Math.Cos(dPseudoLatitudeRad);

			// 地球中心と現在地点との距離の計算
			dDistanceFromCE = ConstantsGPS.ELLIPSOID_A * ConstantsGPS.ELLIPSOID_B / Math.Sqrt(ConstantsGPS.ELLIPSOID_A * ConstantsGPS.ELLIPSOID_A * dSin * dSin + ConstantsGPS.ELLIPSOID_B * ConstantsGPS.ELLIPSOID_B * dCos * dCos);

			r.X = (Math.PI / 180.0) * dDistanceFromCE;

			//
			//Lng
			//
			r.Y = (Math.PI / 180.0) * dDistanceFromCE * dCos;


			_point.X = ((_postion.X - _orilat.X) * r.X);
			_point.Y = ((_postion.Y - _orilat.Y) * r.Y);

			return _point;
		}
        
		#region 内部イベント
        // Watpointデータの反映
        private void PostionSetMarkerUpdata()
        {
			_WaypointData.Lat = (double)PostionLat.Value;//double.Parse(PostionLat.Text);
			_WaypointData.Lon = (double)PostionLng.Value;//double.Parse(PostionLng.Text);
			_WaypointData.Yaw = (double)PostionAzi.Value;//double.Parse(PostionAzi.Text);
			_WaypointData.Alt = (double)PostionAlt.Value;// double.Parse(PostionAlt.Text);
			_WaypointData.Speed = (double)PostionSpeed.Value;

            SetValue(WaypointDataProperty, (PointData)_WaypointData);
        }
		// 画面サイズの変更
		private void Main_SizeChanged(object sender, SizeChangedEventArgs e)
		{
			double r = e.NewSize.Width / e.NewSize.Height;

			//　縦横比
			if (r < 1)
			{
				// 縦表示
				Window.Orientation = Orientation.Vertical;
			}
			else
			{
				// 横表示
				Window.Orientation = Orientation.Horizontal;
			}
		}
		#endregion

        #region プロパティ
		public bool isEnable
		{
			get { return (bool)this.IsEnabled; }
			set { this.IsEnabled = value; }
		}
        public int ID
        {
            get { return (int)Window_Id; }
            set { Window_Id = value; }
        }
        public bool WaypointUpdate
        {
            get { return (bool)_WaypointUpdate; }
            set { _WaypointUpdate = value; }
        }
        public static readonly DependencyProperty WaypointDataProperty = DependencyProperty.Register(
																		"PlotDataSelect", typeof(PointData), 
																		typeof(WaypointWindow),
																		new FrameworkPropertyMetadata(
																			default(PointData),
																			FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
																			OnWaypointDataChanged)
																		);

        [Category("Window")]
        [Description("WaypointWindow")]
        [Browsable(true)]
        [Bindable(true)]
        public PointData Waypoint
        {
            get { return (PointData)_WaypointData; }
            set
            {
                _WaypointData = value;
            }
        }

        public PointData UAVData
        {
            get { return (PointData)_UAVData; }
            set { _UAVData = value;           }
        }
        #endregion

        #region プロパティメソッド
        private static void OnWaypointDataChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            WaypointWindow control = (WaypointWindow)d;
        }

        #endregion

		private void PostionLat_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			if (EndFlag)
			{
				if (PostionLat.Value != null)
				{
					_WaypointUpdate = true;
					_WaypointData.Lat = (double)PostionLat.Value;
					SetValue(WaypointDataProperty, (PointData)_WaypointData);
				}
			}
		}

		private void PostionLatDis_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{

			if (EndFlag)
			{
				if (PostionLatDis.Value != null && PostionLngDis.Value != null)
				{
					var _x = (double)PostionLatDis.Value;
					var _y = (double)PostionLngDis.Value;

					var _ori = MeterPerLatLng(new System.Windows.Point(_UAVData.Lat, _UAVData.Lon));

					double lat = _UAVData.Lat + _x / _ori.X;
					double lng = _UAVData.Lon + _y / _ori.Y;

					PostionLat.Value = lat;
					PostionLng.Value = lng;
					PostionLatDis.Value = _x;

				}
			}

		}

		private void PostionLng_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			if (EndFlag)
			{
				if (PostionLng.Value != null)
				{
					_WaypointUpdate = true;
					_WaypointData.Lon = (double)PostionLng.Value;
					SetValue(WaypointDataProperty, (PointData)_WaypointData);
				}
			}
		}

		private void PostionLngDis_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			if (EndFlag)
			{
				if (PostionLatDis.Value != null && PostionLngDis.Value != null)
				{
					var _x = (double)PostionLatDis.Value;
					var _y = (double)PostionLngDis.Value;

					var _ori = MeterPerLatLng(new System.Windows.Point(_UAVData.Lat, _UAVData.Lon));

					double lat = _UAVData.Lat + _x / _ori.X;
					double lng = _UAVData.Lon + _y / _ori.Y;

					PostionLat.Value = lat;
					PostionLng.Value = lng;
					PostionLatDis.Value = _x;
					PostionLngDis.Value = _y;
				}
			}
		}

		private void PostionAlt_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			if (EndFlag)
			{
				if (PostionAlt.Value != null)
				{
					_WaypointUpdate = true;
					_WaypointData.Alt = (double)PostionAlt.Value;
					SetValue(WaypointDataProperty, (PointData)_WaypointData);
				}
			}
		}

		private void PostionAzi_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			if (EndFlag)
			{
				if (PostionAzi.Value != null)
				{
					_WaypointUpdate = true;
					_WaypointData.Yaw = (double)PostionAzi.Value;
					SetValue(WaypointDataProperty, (PointData)_WaypointData);
				}			
			}
		}
		
		private void PostionSpeed_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			if (EndFlag)
			{
				if (PostionSpeed.Value != null)
				{
					_WaypointUpdate = true;
					_WaypointData.Speed = (double)PostionSpeed.Value;
					SetValue(WaypointDataProperty, (PointData)_WaypointData);
				}
			}
		}		

    }
}
