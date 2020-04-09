using GMap.NET.WindowsPresentation;
using System;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
//line
using System.Windows.Shapes;
using System.Windows.Media.Imaging;

namespace Y_Planner.CustomMarkers
{
	static class Constants
	{
		// フリックボールのOffset
		public const int flickOffsetX = 25;		//
		public const int flickOffsetY = 35;		//

		// マーカ移動時のOffset
		public const int moveOffsetX = 0;		//
		public const int moveOffsetY = 0;		//

	}
    /// <summary>
    /// Waypoint.xaml の相互作用ロジック
    /// </summary>
    public partial class Waypoint : UserControl
    {
        Popup Popup;
        Label Label;

        GMapMarker Marker;
        MainWindow MainWindow;

        bool EndFlag = true;    //スレッドループ
        bool UavAngSw = false;  //UAVマーカー回転（長押し）
        bool angFlag = false;
        bool modeFlag = false;
        bool DeviceFlag = false; // true:Touch   false:Mouse
        int ang = 0;
        double Nang = 0;
		double _oldAng = 0;
        int cnt = 0;
        int PushTime = 100;
        System.Windows.Point TouchPoint;       //初期タッチ位置
		private bool moveFlag = false;
       
        public Waypoint(MainWindow window, GMapMarker marker, bool mode, int _no, bool _icon)
        {
            InitializeComponent();

            DeviceFlag = mode;


            //アイコンを変更
            if (_icon)
            {
                icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/UavWayPoint.png")));
            }
            else
            {
                icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/MultiWayPoint.png")));
				Main.RenderTransform = new ScaleTransform(0.5, 0.5);	//マーカーサイズ縮小
            }
            /*
             * //イベント管理
            if (DeviceFlag)
            {
                this.MouseLeave -= new System.Windows.Input.MouseEventHandler(this.MarkerControl_MouseLeave);
                this.MouseMove -= new System.Windows.Input.MouseEventHandler(this.UAVWayPoint_MouseMove);
                this.MouseLeftButtonUp -= new System.Windows.Input.MouseButtonEventHandler(this.UAVWayPoint_MouseLeftButtonUp);
                this.MouseLeftButtonDown -= new System.Windows.Input.MouseButtonEventHandler(this.UAVWayPoint_MouseLeftButtonDown);
            }
            else
            {
                
                this.MouseLeave += new System.Windows.Input.MouseEventHandler(this.MarkerControl_MouseLeave);
                this.MouseMove += new System.Windows.Input.MouseEventHandler(this.UAVWayPoint_MouseMove);
                this.MouseLeftButtonUp += new System.Windows.Input.MouseButtonEventHandler(this.UAVWayPoint_MouseLeftButtonUp);
                this.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(this.UAVWayPoint_MouseLeftButtonDown);
                
            }
            */
            this.MainWindow = window;
            this.Marker = marker;
            this.Marker.ID = _no;
            this.Marker.Mode = 0;

            Popup = new Popup();    //ポップアップ表示
            Label = new Label();    //ポップアップのコメント表示

            Popup.Placement = PlacementMode.Mouse;
            {
                Label.Background = Brushes.Blue;
                Label.Foreground = Brushes.White;
                Label.BorderBrush = Brushes.WhiteSmoke;
                Label.BorderThickness = new Thickness(2);
                Label.Padding = new Thickness(5);
                Label.FontSize = 12;
                Label.Content = "ID/" + this.Marker.ID.ToString() +
                                "\n纬度/" + this.Marker.Position.Lat.ToString("0.0000000") +
								"\n经度/" + this.Marker.Position.Lng.ToString("0.0000000") +
								"\n角度/" + this.Marker.Azimuth.ToString("0.0000000") +
								"\n高度/" + this.Marker.Altitude.ToString("0.0000000") +
								"\n速度/" + this.Marker.Speed.ToString("0.0000000");
            }
            Popup.Child = Label;

			Number.Content = this.Marker.ID.ToString();

            var task = Task.Factory.StartNew(() =>
            {

                while (EndFlag)
                {
                    if (UavAngSw && cnt > PushTime)   //cnt（長押し）
                    {
                        modeFlag = true;
                    }
                    else if (!UavAngSw)
                    {
                        modeFlag = false;
                        cnt = 0;
                    }

                    Dispatcher.BeginInvoke(new Action<int>(Worker), ang);
                    System.Threading.Thread.Sleep(10);   //1msec
                    ++cnt;

                }
            });
        }

        public void Worker(int data)
        {
			// マーカー方位変更
            if(Marker.Azimuth != Nang){
                Nang = Marker.Azimuth;
                ang = (int)Nang;    //値の更新
                if (ang < 0)
                {
                    ang += 360;
                }
                icon.RenderTransform = new RotateTransform { Angle = ang };
            }

			// フリックエリア表示
            if (modeFlag)
            {
                if (FlickArea.Visibility != Visibility.Visible)
                {
                    FlickArea.Visibility = Visibility.Visible;
                    myCanvas.Children.Clear();
                    
                }
            }
            else
            {
                if (FlickArea.Visibility != Visibility.Hidden)
                {
                    FlickArea.Visibility = Visibility.Hidden;
                    
                }
            }

			//
            if(!Marker.IsMouseOver && Marker.Mode == -1)
            {
                icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/MultiWayPoint.png")));	
                Marker.Mode = 0;
            }

			if (!Marker.IsMouseOver && Marker.Mode == 11)
			{
				//アイコンを変更
				icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/UavWayPoint.png")));
				Marker.Mode = 0;
			}
			
        }

        void UAVWayPoint_Loaded(object sender, RoutedEventArgs e)
        {

        }
        void UAVWayPoint_Unloaded(object sender, RoutedEventArgs e)
        {
			try
			{
				Marker.ZIndex -= 10000;
				Popup.IsOpen = false;

				this.Unloaded -= new RoutedEventHandler(UAVWayPoint_Unloaded);
				this.Loaded -= new RoutedEventHandler(UAVWayPoint_Loaded);
				this.MouseEnter -= new MouseEventHandler(MarkerControl_MouseEnter);
				this.MouseLeave -= new MouseEventHandler(MarkerControl_MouseLeave);
				this.MouseMove -= new MouseEventHandler(UAVWayPoint_MouseMove);
				this.MouseLeftButtonUp -= new MouseButtonEventHandler(UAVWayPoint_MouseLeftButtonUp);
				this.MouseLeftButtonDown -= new MouseButtonEventHandler(UAVWayPoint_MouseLeftButtonDown);

				Marker.Shape = null;
				Popup = null;
				Label = null;

				EndFlag = false;
			}
			catch
			{

			}

        }

		#region マウス操作　マーカー移動と方位角指定
        void UAVWayPoint_MouseMove(object sender, MouseEventArgs e)
        {
			try
			{
				var pe = e.GetPosition(this);
				double x = TouchPoint.X - pe.X;
				double y = TouchPoint.Y - pe.Y;

				double dis_area = Math.Sqrt((x * x) + (y * y));

				//   Point p = e.GetPosition(MainWindow.MainMap);
				// p.Y += 15; //オフセット
				if ((e.LeftButton == MouseButtonState.Pressed && IsMouseCaptured) &&
					(modeFlag == true))
				{


					System.Windows.Point p = e.GetPosition(this);
					p.X -= Constants.flickOffsetX;
					p.Y -= Constants.flickOffsetY;
					int dis = (int)Math.Sqrt(((p.X * p.X) + (p.Y * p.Y)));


					Nang = (Math.Atan2(p.Y, p.X));

					myCanvas.Children.Clear();
					Line line = new Line()
					{

						Stroke = Brushes.Red,
						X1 = 150,
						Y1 = 150,
						X2 = p.X + 150,
						Y2 = p.Y + 150,
						StrokeThickness = 2,
						StrokeStartLineCap = PenLineCap.Flat
					};
					Ellipse en = new Ellipse()
					{
						Fill = new SolidColorBrush(Color.FromArgb(0x80, 0xFF, 0, 0)),
						//Stroke = Brushes.Yellow,
						Width = 30,
						Height = 30
					};


					if (dis >= 150)
					{
						dis = 150;
						p.X = dis * Math.Cos(Nang);
						p.Y = dis * Math.Sin(Nang);
						angFlag = true;

						line.X2 = p.X + 150;
						line.Y2 = p.Y + 150;
						//en.Fill = new SolidColorBrush(Color.FromArgb(0x80, 0xFF, 0, 0));
						en.Fill = line.Stroke = Brushes.Red;
					}
					else
					{
						en.Fill = line.Stroke = Brushes.Blue;
						angFlag = false;

					}


					Canvas.SetLeft(en, p.X + 150 - 15);
					Canvas.SetTop(en, p.Y + 150 - 15);

					//　描画
					myCanvas.Children.Add(en);
					myCanvas.Children.Add(line);

					Nang = (Math.Atan2(-p.X, -p.Y) * 180 / Math.PI);
					Nang *= -1;
					Marker.Azimuth = Math.Round(Nang, 0, MidpointRounding.AwayFromZero);
				}
				else if (e.LeftButton == MouseButtonState.Pressed && IsMouseCaptured)
				{
					System.Windows.Point p = e.GetPosition(MainWindow.MainMap);
					p.X -= Constants.moveOffsetX;
					p.Y -= Constants.moveOffsetY;
					System.Diagnostics.Debug.WriteLine(">>>>>>>>>>>>>>dis_area:" + dis_area);
					if (dis_area > 10 && moveFlag)
					{
						Marker.Position = MainWindow.MainMap.FromLocalToLatLng((int)(p.X), (int)(p.Y));
					}
					moveFlag = true;
					cnt = 0;
				}
				// PopUp表示の値更新
				if (DeviceFlag)
				{
					Label.Content = "ID/" + this.Marker.ID.ToString() +
									"\n纬度/" + this.Marker.Position.Lat.ToString("0.0000000") +
									"\n经度/" + this.Marker.Position.Lng.ToString("0.0000000") +
									"\n角度/" + this.Marker.Azimuth.ToString("0.0000000") +
									"\n高度/" + this.Marker.Altitude.ToString("0.0000000") +
									"\n速度/" + this.Marker.Speed.ToString("0.0000000");
				}
			}
			catch
			{

			}
			
            
        }
        void UAVWayPoint_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
			try
			{
				UavAngSw = true;
				PushTime = 100;
				if (!IsMouseCaptured)
				{
					Mouse.Capture(this);
				}
				_oldAng = Marker.Azimuth;
				Marker.IsMouseOver = true;
				//アイコンを変更
				icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/UavWayPoint.png")));
			}
			catch
			{

			}
           
        }
        void UAVWayPoint_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
			try
			{
				if (IsMouseCaptured)
				{
					Mouse.Capture(null);
				}
				if (angFlag == true)
				{
					ang = (int)Nang;    //値の更新
					if (ang < 0)
					{
						ang += 360;
					}
					icon.RenderTransform = new RotateTransform { Angle = ang };
				}
				else
				{
					Marker.Azimuth = _oldAng;
				}
				UavAngSw = false;
				modeFlag = false;

				////アイコンを変更
				//if (Marker.Mode != 1)
				//{
				//	icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/MultiWayPoint.png")));
				//}
				Marker.IsMouseOver = false;
				moveFlag = false;
			}
			catch
			{

			}
           
        }
        void MarkerControl_MouseLeave(object sender, MouseEventArgs e)
        {
			try
			{
				Marker.ZIndex -= 10000;
				Popup.IsOpen = false;

				Main.RenderTransform = new ScaleTransform(0.5, 0.5);	//マーカーサイズ縮小
			}
			catch
			{

			}

        }
        void MarkerControl_MouseEnter(object sender, MouseEventArgs e)
        {
			try
			{
				Marker.ZIndex += 10000;
				Popup.IsOpen = true;

				Main.RenderTransform = new ScaleTransform(1, 1);	//マーカーサイズ標準
			}
			catch
			{

			}

        }
		#endregion

		#region タッチ操作　マーカー移動と方位角指定
		private void MarkerControl_TouchEnter(object sender, TouchEventArgs e)
        {
			try
			{
				Marker.ZIndex += 10000;
				Popup.IsOpen = true;

				Main.RenderTransform = new ScaleTransform(1, 1);	//マーカーサイズ標準
				//アイコンを変更
				//icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/UavWayPoint.png")));	
			}
			catch
			{

			}

        }
        private void MarkerControl_TouchLeave(object sender, TouchEventArgs e)
        {
			try
			{
				Marker.ZIndex -= 10000;
				Popup.IsOpen = false;

				Main.RenderTransform = new ScaleTransform(0.5, 0.5);	//マーカーサイズ縮小
				//アイコンを変更
				//icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/MultiWayPoint.png")));	
			}
			catch
			{

			}

        }        
        private void MarkerControl_TouchDown(object sender, TouchEventArgs e)
        {
			try
			{
				this.CaptureTouch(e.TouchDevice);

				UavAngSw = true;
				PushTime = 50;

				var pt = e.GetTouchPoint(this);
				TouchPoint.X = pt.Position.X;
				TouchPoint.Y = pt.Position.Y;
				_oldAng = Marker.Azimuth;
				//Marker.ControlInput = true;
				Marker.IsMouseOver = true;
			}
			catch
			{

			}

        }
        private void MarkerControl_TouchUp(object sender, TouchEventArgs e)
        {
			try
			{
				this.ReleaseTouchCapture(e.TouchDevice);

				myCanvas.Children.Clear();

				if (angFlag == true)
				{
					ang = (int)Nang;    //値の更新
					if (ang < 0)
					{
						ang += 360;
					}
					icon.RenderTransform = new RotateTransform { Angle = ang };
				}
				else
				{
					Marker.Azimuth = _oldAng;
				}
				UavAngSw = false;
				modeFlag = false;
				//Marker.ControlInput = false;
				Marker.IsMouseOver = false;
				moveFlag = false;
			}
			catch
			{

			}
           
        }
        private void MarkerControl_TouchMove(object sender, TouchEventArgs e)
        {
			try
			{
				TouchPoint pe = e.GetTouchPoint(this);
				double x = TouchPoint.X - pe.Position.X;
				double y = TouchPoint.Y - pe.Position.Y;

				double dis_area = Math.Sqrt((x * x) + (y * y));

				/*
				if (dis_area >310)
				{
					UavAngSw = false;
					modeFlag = false;
				}
				*/

				if ((modeFlag == true))
				{
					var pt = e.GetTouchPoint(this);
					System.Windows.Point p = new System.Windows.Point();
					p.X = pt.Position.X;
					p.Y = pt.Position.Y;
					p.X -= Constants.flickOffsetX;
					p.Y -= Constants.flickOffsetY;
					int dis = (int)Math.Sqrt(((p.X * p.X) + (p.Y * p.Y)));


					Nang = (Math.Atan2(p.Y, p.X));

					myCanvas.Children.Clear();
					Line line = new Line()
					{
						Stroke = Brushes.Red,
						X1 = 150,
						Y1 = 150,
						X2 = p.X + 150,
						Y2 = p.Y + 150,
						StrokeThickness = 2,
						StrokeStartLineCap = PenLineCap.Flat
					};
					Ellipse en = new Ellipse()
					{
						Fill = new SolidColorBrush(Color.FromArgb(0x80, 0xFF, 0, 0)),
						//Stroke = Brushes.Yellow,
						Width = 30,
						Height = 30
					};


					if (dis >= 150)
					{
						dis = 150;
						p.X = dis * Math.Cos(Nang);
						p.Y = dis * Math.Sin(Nang);
						angFlag = true;

						line.X2 = p.X + 150;
						line.Y2 = p.Y + 150;
						//en.Fill = new SolidColorBrush(Color.FromArgb(0x80, 0xFF, 0, 0));
						en.Fill = line.Stroke = Brushes.Red;

					}
					else
					{
						en.Fill = line.Stroke = Brushes.Blue;
						angFlag = false;

					}


					Canvas.SetLeft(en, p.X + 150 - 15);
					Canvas.SetTop(en, p.Y + 150 - 15);

					//　描画
					myCanvas.Children.Add(en);
					myCanvas.Children.Add(line);

					Nang = (Math.Atan2(-p.X, -p.Y) * 180 / Math.PI);
					Nang *= -1;
					Marker.Azimuth = Math.Round(Nang, 0, MidpointRounding.AwayFromZero);
				}
				else
				{
					TouchPoint pt = e.GetTouchPoint(MainWindow.MainMap);
					System.Windows.Point p = new System.Windows.Point();
					p.X = pt.Position.X;
					p.Y = pt.Position.Y;
					p.X += Constants.moveOffsetX;
					p.Y += Constants.moveOffsetY;

					System.Diagnostics.Debug.WriteLine("dis_area:" + dis_area);
					if (dis_area > 10 && moveFlag)
					{
						Marker.Position = MainWindow.MainMap.FromLocalToLatLng((int)(p.X), (int)(p.Y));
					}
					moveFlag = true;
				}
				// PopUp表示の値更新
				if (!DeviceFlag)
				{
					Label.Content = "ID/" + this.Marker.ID.ToString() +
									"\n纬度/" + this.Marker.Position.Lat.ToString("0.0000000") +
									"\n经度/" + this.Marker.Position.Lng.ToString("0.0000000") +
									"\n角度/" + this.Marker.Azimuth.ToString("0.0000000") +
									"\n高度/" + this.Marker.Altitude.ToString("0.0000000") +
									"\n速度/" + this.Marker.Speed.ToString("0.0000000");
				}
			}
			catch
			{

			}
           
            
        }
		#endregion
    }
}