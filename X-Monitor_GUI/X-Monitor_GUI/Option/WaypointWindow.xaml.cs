using System.Windows.Controls;
using System.Windows;
using System;
using System.Diagnostics;
using System.Windows.Media;
using System.ComponentModel;
using System.Windows.Input;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

using Point;

namespace XMonitor_GUI
{
    static class ConstantsGPS
    {
        public const double ELLIPSOID_A = 6378137.0;
        public const double ELLIPSOID_F = 0.00335281066475;
        public const double ELLIPSOID_B = (ELLIPSOID_A * (1.0 - ELLIPSOID_F));
    }
    /// <summary>
    /// WaypointWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class WaypointWindow : UserControl
    {
        bool EndFlag = false;							//スレッドループ
        private int Window_Id;							// オプション画面ID
        private PointData _WaypointData;				// WaypointData
        private PointData _UAVData;						// WaypointData
        private PointData _HomeData;
        private PointData _GpsData;

        private bool _WaypointUpdate;					//
        System.Windows.Forms.Timer CommandTimer;        // 長押し処理用（押し間違い防止）
        int CommandFlag;								// Commandボタン複数押し防止
        private const int interval = 10;				// 長押し開始時間
        public int _mode;										// 操作モード 0:マウス/タッチ操作 1:GPS

        public WaypointWindow(int id, PointData _waypoint)
        {
            InitializeComponent();

            Width = double.NaN;
            Height = double.NaN;

            Window_Id = id;
            _WaypointData = new PointData();
            _WaypointData = _waypoint;
            _UAVData = new PointData();
            _HomeData = new PointData();
            _GpsData = new PointData();

            _WaypointUpdate = false;
            CommandFlag = 0;
            _mode = 0;

            M_IfPaizhao_Action.Items.Add("No");
            M_IfPaizhao_Action.Items.Add("Yes");
            M_IfPaizhao_Action.SelectedIndex = 0;
            M_IfHangxiangzizhu_Action.Items.Add("No");
            M_IfHangxiangzizhu_Action.Items.Add("Yes");
            M_IfHangxiangzizhu_Action.SelectedIndex = 0;
            M_IfGaoduzizhu_Action.Items.Add("No");
            M_IfGaoduzizhu_Action.Items.Add("Yes");
            M_IfGaoduzizhu_Action.SelectedIndex = 0;
            M_IfRenwuxunhuan_Action.Items.Add("No");
            M_IfRenwuxunhuan_Action.Items.Add("Yes");
            M_IfRenwuxunhuan_Action.SelectedIndex = 0;

            M_JiaoduAuto_Make.Items.Add("No");
            M_JiaoduAuto_Make.Items.Add("Yes");
            M_JiaoduAuto_Make.SelectedIndex = 0;

            M_Ifreback_Action.Items.Add("No");
            M_Ifreback_Action.Items.Add("Yes");
            M_Ifreback_Action.SelectedIndex = 0;

            var _image = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/mouse.png"), UriKind.Relative));
            ImageBrush _imageBrush = new ImageBrush(_image);
            //	MouseTouchSet.OpacityMask = _imageBrush;
            //	MouseTouchBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
            //	MouseTouchSet.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));

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

            Popup = new System.Windows.Controls.Primitives.Popup();							//ポップアップ表示
            Label = new System.Windows.Controls.Label();    //ポップアップのコメント表示
            Popup.Placement = System.Windows.Controls.Primitives.PlacementMode.Mouse;
            {
                Label.Background = Brushes.Blue;
                Label.Foreground = Brushes.White;
                Label.BorderBrush = Brushes.WhiteSmoke;
                Label.BorderThickness = new Thickness(2);
                Label.Padding = new Thickness(5);
                Label.FontSize = 12;
                Label.Content = "";
            }
            Popup.Child = Label;

            PostionAlt.Maximum = int.MaxValue;
            PostionAlt.Minimum = int.MinValue;

            if (Properties.Settings.Default.Function3 == "GpsMonitor")
            {
                Mode.Width = 210;
                Mode.Height = 50;
            }
            else
            {
                Mode.Width = 0;
                Mode.Height = 0;
            }
        }
        private void Main_Unloaded(object sender, RoutedEventArgs e)
        {
            EndFlag = false;
        }
        public void Worker(int data)
        {
            if (!_WaypointUpdate)
            {
                if (!PostionLat.IsFocused) PostionLat.Value = _WaypointData.Lat;
                if (!PostionLng.IsFocused) PostionLng.Value = _WaypointData.Lon;
                if (!PostionAlt.IsFocused) PostionAlt.Value = _WaypointData.mmWayPoint_HeightEdit;
                if (!PostionAzi.IsFocused) PostionAzi.Value = _WaypointData.mmWayPoint_YawEdit;
                if (!PostionSpeed.IsFocused) PostionSpeed.Value = _WaypointData.mmWayPoint_HoverTimerEdit;//悬停时间
                if (!Hengxiangsudu.IsFocused) Hengxiangsudu.Value = _WaypointData.mmWayPoint_HengxiangSuduEdit;
                if (!Xiangshangsudu_Set.IsFocused) Xiangshangsudu.Value = _WaypointData.mmWayPoint_ZongshangSuduEdit;
                if (!Xiangxiasudu_Set.IsFocused) Xiangxiasudu.Value = _WaypointData.mmWayPoint_ZongxiaSuduEdit;
                M_IfPaizhao_Action.SelectedIndex = ((int)_WaypointData.mmWayPoint_SetIndexEdit & 0x01);
                M_IfHangxiangzizhu_Action.SelectedIndex = ((int)_WaypointData.mmWayPoint_SetIndexEdit>>1 & 0x01);
                M_IfGaoduzizhu_Action.SelectedIndex = ((int)_WaypointData.mmWayPoint_SetIndexEdit>>2 & 0x01);
             //   M_IfRenwuxunhuan_Action.SelectedIndex = ((int)_WaypointData.mmWayPoint_SetIndexEdit>>3 & 0x01);
                    
                        


                var _uav = new System.Windows.Point(_UAVData.Lat, _UAVData.Lon);
                var _waypoint = new System.Windows.Point(_WaypointData.Lat, _WaypointData.Lon);
                var _p = MeterPerLatLng2(_uav, _waypoint);

                if (!PostionLatDis.IsFocused) PostionLatDis.Value = Math.Round(_p.X, 3, MidpointRounding.AwayFromZero);
                if (!PostionLngDis.IsFocused) PostionLngDis.Value = Math.Round(_p.Y, 3, MidpointRounding.AwayFromZero);

                /////////2016 5.3显示相关的数据/////////////////////////////////////////////
                //M_Imu_Roll.Content = Math.Round(_WaypointData.mm_IMUDataRoll, 3);
                //M_Imu_Pitch.Content = Math.Round(_WaypointData.mm_IMUDataPitch, 3);
                //M_Imu_Yaw.Content = Math.Round(_WaypointData.mm_IMUDataYaw, 3);
                //M_Gyro_RollRate.Content = Math.Round(_WaypointData.mm_GyroDataRollRate, 3);
                //M_Gyro_PitchRate.Content = Math.Round(_WaypointData.mm_GyroDataPitchRate, 3);
                //M_Gyro_YawRate.Content = Math.Round(_WaypointData.mm_GyroDataYawRate, 3);
                //M_Acc_X.Content = Math.Round(_WaypointData.mm_AccDataX, 3);
                //M_Acc_Y.Content = Math.Round(_WaypointData.mm_AccDataY, 3);
                //M_Acc_Z.Content = Math.Round(_WaypointData.mm_AccDataZ, 3);
                //M_Mag_X.Content = Math.Round(_WaypointData.mm_MagDataX, 3);
                //M_Mag_Y.Content = Math.Round(_WaypointData.mm_MagDataY, 3);
                //M_Mag_Z.Content = Math.Round(_WaypointData.mm_MagDataZ, 3);


            }
        }

        // コマンドボタン長押し時
        private void CommandButtonDown()
        {
            switch (CommandFlag)
            {
                case 1:
                    #region AltUp
                    //AltUp.Foreground = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    //AltUpBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 2:
                    #region AltDown
                    //AltDown.Foreground = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    // AltDownBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 3:
                    #region AziUp
                    //AziUpBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    // AziUp.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 4:
                    #region AziDown
                    //AziDownBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    //AziDown.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 5:
                    #region LatUp
                    //LatUpBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    // LatUp.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 6:
                    #region LatDown
                    //LatDownBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    // LatDown.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 7:
                    #region LngUp
                    //LngUpBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    //LngUp.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 8:
                    #region LngDown
                    //LngDownBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    // LngDown.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 9:
                    #region MarkerSet
                    //MarkerSetBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    //MarkerSet.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    break;
                    #endregion
                case 10:
                    #region  MouseTouch
                    //MouseTouchBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    //MouseTouchSet.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    //GpsBorder.BorderBrush = null;
                    //GpsSet.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    _mode = 0;
                    break;
                    #endregion
                case 11:
                    #region GpsSet
                    //MouseTouchBorder.BorderBrush = null;
                    //MouseTouchSet.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    //GpsBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    //GpsSet.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    _mode = 1;
                    break;
                    #endregion
                case 12:
                    #region  HomeSet
                    //HomeBorder.BorderBrush = new SolidColorBrush(Color.FromRgb(255, 192, 0));
                    //HomeSet.Background = new SolidColorBrush(Color.FromRgb(255, 192, 0));
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
                    //AltUp.Foreground = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    // AltUpBorder.BorderBrush = null;
                    break;
                    #endregion
                case 2:
                    #region AltDown
                    //AltDownBorder.BorderBrush = null;
                    //AltDown.Foreground = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 3:
                    #region AziUp
                    //AziUpBorder.BorderBrush = null;
                    // AziUp.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 4:
                    #region AziDown
                    //AziDownBorder.BorderBrush = null;
                    //  AziDown.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 5:
                    #region LatUp
                    //LatUpBorder.BorderBrush = null;
                    // LatUp.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 6:
                    #region LatDown
                    //LatDownBorder.BorderBrush = null;
                    // LatDown.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 7:
                    #region LngUp
                    //LngUpBorder.BorderBrush = null;
                    //LngUp.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 8:
                    #region LngDown
                    //LngDownBorder.BorderBrush = null;
                    //LngDown.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 9:
                    #region MarkerSet
                    //MarkerSetBorder.BorderBrush = null;
                    //MarkerSet.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 10:
                    #region  MouseTouch
                    //MouseTouchBorder.BorderBrush = null;
                    //MouseTouchSet.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 11:
                    #region GpsSet
                    //GpsBorder.BorderBrush = null;
                    //GpsSet.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                case 12:
                    #region  HomeSet
                    //HomeBorder.BorderBrush = null;
                    //HomeSet.Background = new SolidColorBrush(Color.FromRgb(80, 80, 80));
                    break;
                    #endregion
                default:
                    break;

            }
        }

        // Popup表示
        System.Windows.Controls.Primitives.Popup Popup;									// コマンドボタン等の説明メッセージPopup
        System.Windows.Controls.Label Label;			// コマンドボタン等の説明メッセージLabel
        // 押されたコマンドボタンを取得
        private int CommandButtonMode(string str)
        {
            switch (str)
            {
                case "AltUp":
                    #region AltUp
                    return 1;
                    #endregion
                case "AltDown":
                    #region AltDown
                    return 2;
                    #endregion
                case "AziUp":
                    #region AziUp
                    return 3;
                    #endregion
                case "AziDown":
                    #region AziDown
                    return 4;
                    #endregion
                case "LatUp":
                    #region LatUp
                    return 5;
                    #endregion
                case "LatDown":
                    #region LatDown
                    return 6;
                    #endregion
                case "LngUp":
                    #region LngUp
                    return 7;
                    #endregion
                case "LngDown":
                    #region LngDown
                    return 8;
                    #endregion
                case "MarkerSet":
                    #region MarkerSet
                    return 9;
                    #endregion
                case "MouseTouchSet":
                    #region  MouseTouch
                    return 10;
                    #endregion
                case "GpsSet":
                    #region GpsSet
                    return 11;
                    #endregion
                case "HomeSet":
                    #region  HomeSet
                    return 12;
                    #endregion
                default:
                    break;

            }
            return 0;
        }
        private void Command_MouseEnter(object sender, System.Windows.Input.MouseEventArgs e)
        {
            System.Windows.Controls.Label _command = sender as System.Windows.Controls.Label;

            if (_command != null)
            {
                CommandFlag = CommandButtonMode(_command.Name);
                switch (CommandFlag)
                {
                    case 1:
                        #region AltUp
                        Label.Content = "高度+1";
                        break;
                        #endregion
                    case 2:
                        #region AltDown
                        Label.Content = "高度-1";
                        break;
                        #endregion
                    case 3:
                        #region AziUp
                        Label.Content = "方位角+1";
                        break;
                        #endregion
                    case 4:
                        #region AziDown
                        Label.Content = "方位角-1";
                        break;
                        #endregion
                    case 5:
                        #region LatUp
                        Label.Content = "纬度+1";
                        break;
                        #endregion
                    case 6:
                        #region LatDown
                        Label.Content = "纬度-1";
                        break;
                        #endregion
                    case 7:
                        #region LngUp
                        Label.Content = "经度+1";
                        break;
                        #endregion
                    case 8:
                        #region LngDown
                        Label.Content = "经度-1";
                        break;
                        #endregion
                    case 9:
                        #region MarkerSet
                        Label.Content = "Waypoint信息清零";
                        break;
                        #endregion
                    case 10:
                        #region  MouseTouch
                        Label.Content = "Waypoint情報をユーザ指定に設定します";
                        break;
                        #endregion
                    case 11:
                        #region GpsSet
                        Label.Content = "Waypoint情報を基地局のGPS情報に設定します";
                        break;
                        #endregion
                    case 12:
                        #region  HomeSet
                        Label.Content = "Waypoint情報をHome情報に設定します";
                        break;
                        #endregion
                    default:
                        break;

                }
                Popup.IsOpen = true;
            }
        }
        private void Command_MouseLeave(object sender, System.Windows.Input.MouseEventArgs e)
        {
            Popup.IsOpen = false;
        }
        private void Command_TouchEnter(object sender, TouchEventArgs e)
        {
            System.Windows.Controls.Label _command = sender as System.Windows.Controls.Label;

            if (_command != null)
            {
                CommandFlag = CommandButtonMode(_command.Name);
                switch (CommandFlag)
                {
                    case 1:
                        #region AltUp
                        Label.Content = "高度を+1します";
                        break;
                        #endregion
                    case 2:
                        #region AltDown
                        Label.Content = "高度を-1します";
                        break;
                        #endregion
                    case 3:
                        #region AziUp
                        Label.Content = "方位を+1します";
                        break;
                        #endregion
                    case 4:
                        #region AziDown
                        Label.Content = "方位を-1します";
                        break;
                        #endregion
                    case 5:
                        #region LatUp
                        Label.Content = "緯度を+1します";
                        break;
                        #endregion
                    case 6:
                        #region LatDown
                        Label.Content = "緯度を-1します";
                        break;
                        #endregion
                    case 7:
                        #region LngUp
                        Label.Content = "経度を+1します";
                        break;
                        #endregion
                    case 8:
                        #region LngDown
                        Label.Content = "経度を-1します";
                        break;
                        #endregion
                    case 9:
                        #region MarkerSet
                        Label.Content = "Waypoint情報を機体情報に設定します";
                        break;
                        #endregion
                    case 10:
                        #region  MouseTouch
                        Label.Content = "Waypoint情報をユーザ指定に設定します";
                        break;
                        #endregion
                    case 11:
                        #region GpsSet
                        Label.Content = "Waypoint情報を基地局のGPS情報に設定します";
                        break;
                        #endregion
                    case 12:
                        #region  HomeSet
                        Label.Content = "Waypoint情報をHome情報に設定します";
                        break;
                        #endregion
                    default:
                        break;

                }
                Popup.IsOpen = true;
            }
        }
        private void Command_TouchLeave(object sender, TouchEventArgs e)
        {
            Popup.IsOpen = false;
        }


        private void MouseDown(object sender, int _flag)
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
        private void Command_LostMouseCapture(object sender, System.Windows.Input.MouseEventArgs e)
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
        private void Command_LostTouchCapture(object sender, TouchEventArgs e)
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
            TouchDown(sender, e, 1);
        }
        private void AltUp_TouchUp(object sender, TouchEventArgs e)
        {
            TouchUp(sender, e);
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
            TouchDown(sender, e, 3);
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

        #region MouseTouchSet ボタン
        // MarkerSetボタン
        private void MouseTouchSet_MouseDown(object sender, MouseButtonEventArgs e)
        {
            MouseDown(sender, 10);
        }
        private void MouseTouchSet_MouseUp(object sender, MouseButtonEventArgs e)
        {
            MouseUp(sender);
        }
        private void MouseTouchSet_TouchDown(object sender, TouchEventArgs e)
        {
            TouchDown(sender, e, 10);
        }
        private void MouseTouchSet_TouchUp(object sender, TouchEventArgs e)
        {
            TouchUp(sender, e);
        }
        #endregion
        #region HomeSet ボタン
        // MarkerSetボタン
        private void HomeSet_MouseDown(object sender, MouseButtonEventArgs e)
        {
            MouseDown(sender, 12);
        }
        private void HomeSet_MouseUp(object sender, MouseButtonEventArgs e)
        {
            MouseUp(sender);
        }
        private void HomeSet_TouchDown(object sender, TouchEventArgs e)
        {
            TouchDown(sender, e, 12);
        }
        private void HomeSet_TouchUp(object sender, TouchEventArgs e)
        {
            TouchUp(sender, e);
        }
        #endregion
        #region GpsSet ボタン
        // MarkerSetボタン
        private void GpsSet_MouseDown(object sender, MouseButtonEventArgs e)
        {
            MouseDown(sender, 11);
        }
        private void GpsSet_MouseUp(object sender, MouseButtonEventArgs e)
        {
            MouseUp(sender);
        }
        private void GpsSet_TouchDown(object sender, TouchEventArgs e)
        {
            TouchDown(sender, e, 11);
        }
        private void GpsSet_TouchUp(object sender, TouchEventArgs e)
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
                            val += 5;
                            if (val > 180)
                            {	//姿勢入力制限 MAX値
                                val -= 360;
                            }
                            PostionAzi.Value = val;
                            break;
                        case 4:			// 姿勢（-）
                            val = Math.Floor((double)PostionAzi.Value);
                            val -= 5;
                            if (val > 180)
                            {	//姿勢入力制限 MIN値
                                val -= 360;
                            }
                            if (val < -180)
                            {
                                val += 360;
                            }

                            PostionAzi.Value = val;
                            break;
                        case 5:			// 緯度（+）
                            {
                                //int _x = (int)Math.Floor((double)PostionLatDis.Value);
                                //int _y = (int)Math.Floor((double)PostionLngDis.Value);
                                double _x = (double)PostionLatDis.Value;
                                double _y = (double)PostionLngDis.Value;
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
                                //int _x = (int)Math.Floor((double)PostionLatDis.Value);
                                //int _y = (int)Math.Floor((double)PostionLngDis.Value);
                                double _x = (double)PostionLatDis.Value;
                                double _y = (double)PostionLngDis.Value;
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
                                //int _x = (int)Math.Floor((double)PostionLatDis.Value);
                                //int _y = (int)Math.Floor((double)PostionLngDis.Value);
                                double _x = (double)PostionLatDis.Value;
                                double _y = (double)PostionLngDis.Value;
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
                                //int _x = (int)Math.Floor((double)PostionLatDis.Value);
                                //int _y = (int)Math.Floor((double)PostionLngDis.Value);
                                double _x = (double)PostionLatDis.Value;
                                double _y = (double)PostionLngDis.Value;
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
                        case 10:

                            break;
                        case 11:
                            PostionLat.Value = _GpsData.Lat;
                            PostionLng.Value = _GpsData.Lon;
                            //PostionAlt.Value = _GpsData.Alt;
                            //PostionAzi.Value = _GpsData.Yaw;
                            PostionLatDis.Value = 0;
                            PostionLngDis.Value = 0;
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
                        case 12:
                            PostionLat.Value = _HomeData.Lat;
                            PostionLng.Value = _HomeData.Lon;
                            //PostionAlt.Value = _HomeData.Alt;
                            //PostionAzi.Value = _HomeData.Yaw;
                            //PostionLatDis.Value = 0;
                            //PostionLngDis.Value = 0;
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
                _WaypointUpdate = false;
            }
        }
        public PointData Home
        {
            get { return (PointData)_HomeData; }
            set
            {
                _HomeData = value;
            }
        }
        public PointData GpsModule
        {
            get { return (PointData)_GpsData; }
            set
            {
                _GpsData = value;
                if (_mode == 1)
                {
                    PostionLat.Value = _GpsData.Lat;
                    PostionLng.Value = _GpsData.Lon;
                }
            }
        }

        public PointData UAVData
        {
            get { return (PointData)_UAVData; }
            set
            {
                _UAVData = value;
                _WaypointUpdate = false;
            }
        }
        #endregion

        #region プロパティメソッド
        private static void OnWaypointDataChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            WaypointWindow control = (WaypointWindow)d;
        }

        #endregion

        #region ValueChanged
        private void PostionLat_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            if (EndFlag)
            {
                if (PostionLat.Value != null)
                {
                    _WaypointUpdate = true;
                    _WaypointData.Lat = (double)PostionLat.Value;
                    SetValue(WaypointDataProperty, (PointData)_WaypointData);

                    if (Constants.Turn_Lock_flag == 0)
                    {
                        MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                    }
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
                    if (Constants.Turn_Lock_flag == 0)
                    {
                        MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                    }
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
                    if ((double)PostionAlt.Value < -1000 || (double)PostionAlt.Value>3000)
                    {
                        if ((double)PostionAlt.Value<-1000)
                        {
                            PostionAlt.Value = -1000;
                        }
                        if ((double)PostionAlt.Value > 3000)
                        {
                            PostionAlt.Value = 3000;
                        }
                        System.Windows.MessageBox.Show("请保证输入值在-1000m-3000m之间");
                        return;
                    }
                    else
                    {
                        _WaypointUpdate = true;
                        _WaypointData.mmWayPoint_HeightEdit = (double)PostionAlt.Value;
                        SetValue(WaypointDataProperty, (PointData)_WaypointData);
                        if (Constants.Turn_Lock_flag == 0)
                        {
                            MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                        }
                    }
                }
            }
        }

        private void PostionAzi_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            if (EndFlag)
            {
                if (PostionAzi.Value != null)
                {

                    if ((double)PostionAzi.Value < 0 || (double)PostionAzi.Value > 360)
                    {
                        if ((double)PostionAzi.Value < 0)
                        {
                            PostionAzi.Value = 0;
                        }
                        if ((double)PostionAzi.Value > 360)
                        {
                            PostionAzi.Value = 360;
                        }

                        System.Windows.MessageBox.Show("请保证角度在0°-360°之间");
                        return;
                    }
                    else
                    {
                        _WaypointUpdate = true;
                        _WaypointData.mmWayPoint_YawEdit = (double)PostionAzi.Value;
                        SetValue(WaypointDataProperty, (PointData)_WaypointData);
                        if (Constants.Turn_Lock_flag == 0)
                        {
                            MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                        }
                    }

                }
            }
        }

        private void PostionSpeed_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            if (EndFlag)
            {
                if (PostionSpeed.Value != null)
                {
                    if ((double)PostionSpeed.Value < 0 || (double)PostionSpeed.Value > 255)
                    {
                        if ((double)PostionSpeed.Value<0)
                        {
                            PostionSpeed.Value = 0;
                        }
                        if ((double)PostionSpeed.Value > 255)
                        {
                            PostionSpeed.Value = 255;
                        }
                        System.Windows.MessageBox.Show("请保证悬停时间在0s-255s之间");
                        return;
                    }
                    else
                    {
                        _WaypointUpdate = true;
                        _WaypointData.mmWayPoint_HoverTimerEdit = (double)PostionSpeed.Value;
                        SetValue(WaypointDataProperty, (PointData)_WaypointData);
                        if (Constants.Turn_Lock_flag == 0)
                        {
                            MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                        }
                    }
                }
            }
        }

        private void PostionHengxiangsudu_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            if (EndFlag)
            {
                if ((double)Hengxiangsudu.Value < 0 || (double)Hengxiangsudu.Value > 25)
                {
                    if ((double)Hengxiangsudu.Value<1)
                    {
                        Hengxiangsudu.Value = 1;
                    }
                    if ((double)Hengxiangsudu.Value>25)
                    {
                        Hengxiangsudu.Value = 25;
                    }
                    System.Windows.MessageBox.Show("请保证横向速度在1m/s-25m/s之间");
                    return;
                }
                else
                {
                    if (Hengxiangsudu.Value != null)
                    {
                        _WaypointUpdate = true;
                        _WaypointData.mmWayPoint_HengxiangSuduEdit = (double)Hengxiangsudu.Value;
                        SetValue(WaypointDataProperty, (PointData)_WaypointData);
                        if (Constants.Turn_Lock_flag == 0)
                        {
                            MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                        }
                    }
                }

            }
        }
         private void PostionZongshanggsudu_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            if (EndFlag)
            {
                if (Xiangshangsudu.Value != null)
                {

                    if ((double)Xiangshangsudu.Value < 0 || (double)Xiangshangsudu.Value > 6.1)
                    {
                        if ((double)Xiangshangsudu.Value<0)
                        {
                            Xiangshangsudu.Value = 0;
                        }
                        if ((double)Xiangshangsudu.Value > 6)
                        {
                            Xiangshangsudu.Value = 6;
                        }
                        System.Windows.MessageBox.Show("请保证向上速度在0m/s-6m/s之间");
                        return;
                    }
                    else
                    {
                        _WaypointUpdate = true;
                        _WaypointData.mmWayPoint_ZongshangSuduEdit = (double)Xiangshangsudu.Value;
                        SetValue(WaypointDataProperty, (PointData)_WaypointData);
                        if (Constants.Turn_Lock_flag == 0)
                        {
                            MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                        }
                    }
                }
            }
        }
         private void PostionZongxiasudu_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
         {
             if (EndFlag)
             {

                 if ((double)Xiangxiasudu.Value < 0 || (double)Xiangxiasudu.Value > 4.1)
                 {
                     if ((double)Xiangxiasudu.Value < 0)
                     {
                         Xiangxiasudu.Value = 0;
                     }
                     if ((double)Xiangxiasudu.Value > 4)
                     {
                         Xiangxiasudu.Value = 4;
                     }
                     System.Windows.MessageBox.Show("请保证向下速度在0m/s-4m/s之间");
                     return;
                 }
                 else
                 {
                     if (Xiangxiasudu.Value != null)
                     {
                         _WaypointUpdate = true;
                         _WaypointData.mmWayPoint_ZongxiaSuduEdit = (double)Xiangxiasudu.Value;
                         SetValue(WaypointDataProperty, (PointData)_WaypointData);
                         if (Constants.Turn_Lock_flag == 0)
                         {
                             MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                         }
                     }
                 }
             }
         }
        #endregion

         private void M_IfPaizhao_Action_SelectionChanged(object sender, SelectionChangedEventArgs e)
         {
             if (EndFlag)
             {

                     if (Constants.Turn_Lock_flag == 0)
                     {
                         _WaypointUpdate = true;
                         _WaypointData.mmWayPoint_SetIndexEdit = ((M_IfPaizhao_Action.SelectedIndex & 0x01) | ((M_IfHangxiangzizhu_Action.SelectedIndex << 1) & 0x02) | ((M_IfGaoduzizhu_Action.SelectedIndex << 2) & 0x04) | ((M_IfRenwuxunhuan_Action.SelectedIndex << 3) & 0x08) | ((M_Ifreback_Action.SelectedIndex << 4) & 0x10));
                         SetValue(WaypointDataProperty, (PointData)_WaypointData);
                         MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                     }
             }
         }

         private void M_IfHangxiangzizhu_Action_SelectionChanged(object sender, SelectionChangedEventArgs e)
         {
             if (EndFlag)
             {

                     if (Constants.Turn_Lock_flag == 0)
                     {
                         _WaypointUpdate = true;
                         _WaypointData.mmWayPoint_SetIndexEdit = ((M_IfPaizhao_Action.SelectedIndex & 0x01) | ((M_IfHangxiangzizhu_Action.SelectedIndex << 1) & 0x02) | ((M_IfGaoduzizhu_Action.SelectedIndex << 2) & 0x04) | ((M_IfRenwuxunhuan_Action.SelectedIndex << 3) & 0x08) | ((M_Ifreback_Action.SelectedIndex << 4) & 0x10));
                         SetValue(WaypointDataProperty, (PointData)_WaypointData);
                         MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                     }
             }
         }

         private void M_IfGaoduzizhu_Action_SelectionChanged(object sender, SelectionChangedEventArgs e)
         {
             if (EndFlag)
             {

                     if (Constants.Turn_Lock_flag == 0)
                     {
                         _WaypointUpdate = true;
                         _WaypointData.mmWayPoint_SetIndexEdit = ((M_IfPaizhao_Action.SelectedIndex & 0x01) | ((M_IfHangxiangzizhu_Action.SelectedIndex << 1) & 0x02) | ((M_IfGaoduzizhu_Action.SelectedIndex << 2) & 0x04) | ((M_IfRenwuxunhuan_Action.SelectedIndex << 3) & 0x08) | ((M_Ifreback_Action.SelectedIndex << 4) & 0x10));
                         SetValue(WaypointDataProperty, (PointData)_WaypointData);
                         MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                     }
             }
         }

         private void M_IfRenwuxunhuan_Action_SelectionChanged(object sender, SelectionChangedEventArgs e)
         {
             if (EndFlag)
             {
                     if (Constants.Turn_Lock_flag == 0)
                     {

                         if(M_IfRenwuxunhuan_Action.SelectedIndex==0)
                         {
                             Constants.If_WaypointReAction_Flag = 0;
                         }
                         else
                         {
                             Constants.If_WaypointReAction_Flag = 1;
                         }
                         _WaypointUpdate = true;
                         _WaypointData.mmWayPoint_SetIndexEdit = ((M_IfPaizhao_Action.SelectedIndex & 0x01) | ((M_IfHangxiangzizhu_Action.SelectedIndex << 1) & 0x02) | ((M_IfGaoduzizhu_Action.SelectedIndex << 2) & 0x04) | ((M_IfRenwuxunhuan_Action.SelectedIndex << 3) & 0x08) | ((M_Ifreback_Action.SelectedIndex << 4) & 0x10));
                         SetValue(WaypointDataProperty, (PointData)_WaypointData);
                         MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                     }
             }
         }
        public void Function_Reset()
         {
             M_IfRenwuxunhuan_Action.SelectedIndex = 0;
         }
        public void Function_Reset1()
        {
            M_IfRenwuxunhuan_Action.SelectedIndex = 1;
        }

        private void M_JiaoduAuto_Make_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if(M_JiaoduAuto_Make.SelectedIndex==0)
            {
                Constants.Angle_Auto_Make_Flag = 0;
                MainWindow.pWin.Function_JiaoduAutoMake_ActiveOrNagtive();
            }
            else
            {
                Constants.Angle_Auto_Make_Flag = 1;
                MainWindow.pWin.Function_JiaoduAutoMake_ActiveOrNagtive();
            }

        }

        private void M_Ifreback_Action_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (EndFlag)
            {
                if (Constants.Turn_Lock_flag == 0)
                {

                    if (M_Ifreback_Action.SelectedIndex == 0)
                    {
                        Constants.If_Reback_Flag = 0;
                    }
                    else
                    {
                        Constants.If_Reback_Flag = 1;
                    }
                    _WaypointUpdate = true;
                    _WaypointData.mmWayPoint_SetIndexEdit = ((M_IfPaizhao_Action.SelectedIndex & 0x01) | ((M_IfHangxiangzizhu_Action.SelectedIndex << 1) & 0x02) | ((M_IfGaoduzizhu_Action.SelectedIndex << 2) & 0x04) | ((M_IfRenwuxunhuan_Action.SelectedIndex << 3) & 0x08) | ((M_Ifreback_Action.SelectedIndex << 4) & 0x10));
                    SetValue(WaypointDataProperty, (PointData)_WaypointData);
                    MainWindow.pWin.Function_Modify_Marker((PointData)_WaypointData);
                }
            }
        }
        public void Function_Back()
        {
            M_Ifreback_Action.SelectedIndex = 0;
        }
        public void Function_Back1()
        {
            M_Ifreback_Action.SelectedIndex = 1;
        }
    }
}
