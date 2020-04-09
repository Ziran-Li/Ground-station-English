using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
using System.Diagnostics;

using System;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

//line
using System.Windows.Shapes;
using System.ComponentModel;
using GMap.NET;
using GMap.NET.WindowsPresentation;

namespace XMonitor_GUI.CustomMarkers
{
    /// <summary>
    /// MultiWaypoint.xaml の相互作用ロジック
    /// </summary>
    public partial class MultiWaypoint : UserControl
    {
        Popup Popup;
        Label Label;
        GMapMarker Marker;
		Brush _brush = Brushes.Red;

        public MultiWaypoint(GMapMarker marker, int id, double ang, double alt, double speed, double accuracy, double wait, double _dis1, double _dis2, double _ang1, double _ang2, Brush color)
        {
            InitializeComponent();

			this._brush = color;
            this.Marker = marker;
            this.Marker.Azimuth = ang;
            this.Marker.Altitude = alt;
            this.Marker.Speed = speed;
            if (ang < 0)
            {
                ang += 360;
            }
            icon.RenderTransform = new RotateTransform { Angle = ang };

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
                // PopUp表示の値更新
				Label.Content = "Waypoint" +
					"\nID/" + id.ToString() +
                    "\nLat/" + this.Marker.Position.Lat.ToString("0.0000000") +
                    "\nLng/" + this.Marker.Position.Lng.ToString("0.0000000") +
                    "\nAng/" + this.Marker.Azimuth.ToString("0.0000000") +
                    "\nAlt/" + alt.ToString("0.0000000") +
                    "\nSpd/" + speed.ToString("0.0000000") +
                    "\nAcc/" + accuracy.ToString("0.0000000") +
                    "\nWait/" + wait.ToString("0.0000000");
            }
            Popup.Child = Label;
			Number.Content = id.ToString();

            // Waypoint間の距離表示
            if (_dis1 != -1)
            {
                this.TextDistance(_dis1, _ang1);
            }
            if (_dis2 != -1)
            {
                this.TextDistance(_dis2, _ang2);
            }
        }

        void UAVWayPoint_Unloaded(object sender, RoutedEventArgs e)
        {
			try
			{
				//Marker.ZIndex -= 10000;
				Popup.IsOpen = false;

				this.Unloaded -= new RoutedEventHandler(UAVWayPoint_Unloaded);
				this.MouseEnter -= new MouseEventHandler(MarkerControl_MouseEnter);
				this.MouseLeave -= new MouseEventHandler(MarkerControl_MouseLeave);
				Popup = null;
				Label = null;

				Marker.Shape = null;
			}
			catch
			{

			}
        }

        // 2点間距離(前)
        public void TextDistance(double _dis, double _ang)
        {
            Label _label = new System.Windows.Controls.Label();
			//_label.Background = Brushes.Blue;
			_label.Foreground = _brush;
            _label.FontSize = 12;
            _label.Content = _dis.ToString("0.0") + "m";

            double _x = Math.Cos(_ang * Math.PI / 180) * (30);
            //_x += 15;   // offset
            double _y = Math.Sin(_ang * Math.PI / 180) * (30);
            _y += 12;   // offset

            Canvas.SetTop(_label, _y);
            Canvas.SetLeft(_label, _x);
            canv.Children.Add(_label);
        }

        // ================================================================================
        // マウス操作
        // PopUp表示
        void MarkerControl_MouseLeave(object sender, MouseEventArgs e)
        {
			try
			{
				//Marker.ZIndex -= 10000;
				Popup.IsOpen = false;
				canv.Visibility = Visibility.Hidden;
			}
			catch
			{

			}
        }
        void MarkerControl_MouseEnter(object sender, MouseEventArgs e)
        {
			try
			{
				//Marker.ZIndex += 10000;
				Popup.IsOpen = true;
				canv.Visibility = Visibility.Visible;
			}
			catch
			{

			}

        }

        // ================================================================================
        // タッチ操作
        // PopUp表示
        private void MarkerControl_TouchEnter(object sender, TouchEventArgs e)
        {
			try
			{
				//Marker.ZIndex += 10000;
				Popup.IsOpen = true;
				canv.Visibility = Visibility.Visible;
			}
			catch
			{

			}

        }
        private void MarkerControl_TouchLeave(object sender, TouchEventArgs e)
        {
			try
			{
				//Marker.ZIndex -= 10000;
				Popup.IsOpen = false;
				canv.Visibility = Visibility.Hidden;
			}
			catch
			{

			}

        }
    }
}
