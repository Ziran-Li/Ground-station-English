using GMap.NET.WindowsPresentation;
using System;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
// line
using System.Windows.Shapes;
using System.Windows.Media.Imaging;

namespace Y_Planner.CustomMarkers
{
    /// <summary>
    /// Waypoint.xaml の相互作用ロジック
    /// </summary>
    public partial class BasePoint : UserControl
    {
        Popup Popup;
        Label Label;

        GMapMarker Marker;
        MainWindow MainWindow;

		bool EndFlag = true;    //スレッドループ
        bool DeviceFlag = false; // true:Touch   false:Mouse

        public BasePoint(MainWindow window, GMapMarker marker, bool mode, int _no, bool _icon)
        {
            InitializeComponent();

            DeviceFlag = mode;
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
				Label.Content = "No/" + this.Marker.ID.ToString() +
								"\n纬度/" + this.Marker.Position.Lat.ToString("0.0000000") +
								"\n经度/" + this.Marker.Position.Lng.ToString("0.0000000");
            }
            Popup.Child = Label;

			var task = Task.Factory.StartNew(() =>
			{

				while (EndFlag)
				{
					Dispatcher.BeginInvoke((Action)(() =>
					{
						if (!Marker.IsMouseOver && Marker.Mode == 1)
						{
							Line.Stroke = Brushes.Gold;
							Marker.Mode = 0;
						}
						if (!Marker.IsMouseOver && Marker.Mode == -1)
						{
							Line.Stroke = Brushes.White;
							Marker.Mode = 0;
						}

					}));
					System.Threading.Thread.Sleep(10);	// Wait
				}
			});
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
        
        // マウス操作　マーカー移動と方位角指定
        void UAVWayPoint_MouseMove(object sender, MouseEventArgs e)
        {
			try
			{
				if (e.LeftButton == MouseButtonState.Pressed && IsMouseCaptured)
				{
					System.Windows.Point p = e.GetPosition(MainWindow.MainMap);
					p.X -= Constants.moveOffsetX;
					p.Y -= Constants.moveOffsetY;
					Marker.Position = MainWindow.MainMap.FromLocalToLatLng((int)(p.X), (int)(p.Y));

					// PopUp表示の値更新
					if (DeviceFlag)
					{
						Label.Content = "No/" + this.Marker.ID.ToString() +
										"\n纬度/" + this.Marker.Position.Lat.ToString("0.0000000") +
										"\n经度/" + this.Marker.Position.Lng.ToString("0.0000000");
					}
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
				if (!IsMouseCaptured)
				{
					Mouse.Capture(this);
				}
				Marker.IsMouseOver = true;
				Line.Stroke = Brushes.Gold;
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
				Marker.IsMouseOver = false;
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
			}
			catch
			{

			}

        }

        // タッチ操作　マーカー移動と方位角指定
        private void MarkerControl_TouchDown(object sender, TouchEventArgs e)
        {
			try
			{
				this.CaptureTouch(e.TouchDevice);
				Marker.IsMouseOver = true;
				Line.Stroke = Brushes.Gold;
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
				Marker.IsMouseOver = false;
			}
			catch 
			{
			}

        }
        private void MarkerControl_TouchMove(object sender, TouchEventArgs e)
        {
			try
			{
				TouchPoint pt = e.GetTouchPoint(MainWindow.MainMap);
				System.Windows.Point p = new System.Windows.Point();
				p.X = pt.Position.X;
				p.Y = pt.Position.Y;
				p.X += Constants.moveOffsetX;
				p.Y += Constants.moveOffsetY;
				Marker.Position = MainWindow.MainMap.FromLocalToLatLng((int)(p.X), (int)(p.Y));

				// PopUp表示の値更新
				if (!DeviceFlag)
				{
					Label.Content = "No/" + this.Marker.ID.ToString() +
                                    "\n纬度/" + this.Marker.Position.Lat.ToString("0.0000000") +
									"\n经度/" + this.Marker.Position.Lng.ToString("0.0000000");
				}

			}
			catch
			{

			}
          
        }
        private void MarkerControl_TouchEnter(object sender, TouchEventArgs e)
        {
			try
			{
				Marker.ZIndex += 10000;
				Popup.IsOpen = true;
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
			}
			catch
			{

			}

        }        

    }
}