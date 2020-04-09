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
    /// Position.xaml の相互作用ロジック
    /// </summary>
    public partial class Position : UserControl
    {
        double first_la = 0.0;
        double first_lo = 0.0;
        double second_la = 0.0;
        double second_lo = 0.0;
        int design_flag = 0;
        float angle = (float)0.0;
        float _ang = (float)0.0;
        Popup Popup;
        Label Label;
        GMapMarker Marker;

        bool EndFlag = true;    //スレッドループ
        double ang = 0;

        public Position(GMapMarker marker)
        {
            InitializeComponent();
            
            this.Marker = marker;
			this.Marker.Mode = 1;

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
				Label.Content = "UAV" +
								"\nLat/" + this.Marker.Position.Lat.ToString("0.0000000") +
								"\nLng/" + this.Marker.Position.Lng.ToString("0.0000000") +
								"\nAng/" + this.Marker.Azimuth.ToString("0.0000000") +
								"\nAlt/" + this.Marker.Altitude.ToString("0.0000000");
                //Label.Content = "Ang/0" + "\nLat/" + this.Marker.Position.Lat.ToString() + "\nLng/" + this.Marker.Position.Lng.ToString();
            }
            Popup.Child = Label;

            var task = Task.Factory.StartNew(() =>
            {

                while (EndFlag)
                {
					try
					{
						if (this.Marker.Mode == int.MaxValue) break;
						Dispatcher.BeginInvoke((Action)(() =>
						{
							// PopUp表示の値更新
                            if (Label != null)
                            {
                                Label.Content = "UAV" +
                                          "\nLat/" + this.Marker.Position.Lat.ToString("0.0000000") +
                                                "\nLng/" + this.Marker.Position.Lng.ToString("0.0000000") +
                                                "\nAng/" + this.Marker.Azimuth.ToString("0.0000000") +
                                                "\nAlt/" + this.Marker.Altitude.ToString("0.0000000");
                            }

                            if (Marker.Azimuth != ang)
                            {
                                ang = this.Marker.Azimuth;
                                if (ang < 0)
                                {
                                    ang += 360;
                                }
                                icon.RenderTransform = new RotateTransform { Angle = ang };
                            }

                           _ang = Destination_Angle_Function();
                            FlickArea.RenderTransform = new RotateTransform { Angle = _ang };

                            //// UAVマーカーの方位角の更新
                            //if (Marker.Azimuth != ang)
                            //{
                            //    ang = this.Marker.Azimuth;
                            //    if (ang < 0)
                            //    {
                            //        ang += 360;
                            //    }
                            //    icon.RenderTransform = new RotateTransform { Angle = ang };
                            //}

                            //// Home方向
                            //System.Windows.Point p = new System.Windows.Point();

                            //p.X = this.Marker.SubPosition.Lat - this.Marker.Position.Lat;
                            //p.Y = this.Marker.SubPosition.Lng - this.Marker.Position.Lng;
                            //if ((p.X > 0.000001 || p.X < -0.000001) && (p.Y > 0.000001 || p.Y < -0.000001))
                            //{
                            //    double _ang = (Math.Atan2(p.Y, p.X) * 180 / Math.PI);
                            //    //_ang += ang;
                            //    if (_ang < 0)
                            //    {
                            //        _ang += 360;
                            //    }
                            //    FlickArea.RenderTransform = new RotateTransform { Angle = _ang };
                            //}
						}));

					}
					catch
					{

					}
					
                    System.Threading.Thread.Sleep(20);   //100msec
                }
            });
        }

        float Destination_Angle_Function()
        {

            if(design_flag==0)
            {
                if (this.Marker.Position.Lat > 10)
                {
                    design_flag = 1;
                    first_la = second_la;
                    first_lo = second_lo;
                }

            }
            else
            {
                design_flag = 0;
                second_la = this.Marker.Position.Lat;
                second_lo = this.Marker.Position.Lng;
                if (System.Math.Abs((first_la - second_la)) > 0 || System.Math.Abs((first_lo - second_lo)) > 0)
                {
                    angle = Rotation_Angle(first_la, first_lo, second_la, second_lo);
                }
            }
            return angle;
        }
        float Rotation_Angle(double first_destination_la, double first_destination_lo, double second_destination_la, double second_destination_lo)
        {
            float Actual_angle = (float)0.0;
            double Actual_C = 0.0;
            double Actual_A = 0.0;

            Actual_C = Math.Cos((90 - second_destination_la) * 3.1415926 / 180.0) * Math.Cos((90 - first_destination_la) * 3.1415926 / 180.0) + Math.Sin((90 - second_destination_la) * 3.1415926 / 180.0) * Math.Sin((90 - first_destination_la) * 3.1415926 / 180.0) * Math.Cos((second_destination_lo - first_destination_lo) * 3.1415926 / 180.0);
            Actual_A = Math.Sqrt((1 - Actual_C * Actual_C));
            if (Actual_A != 0)
            {
                Actual_angle = (float)Math.Asin(((Math.Sin((90 - second_destination_la) * 3.1415926 / 180.0)) * Math.Sin((second_destination_lo - first_destination_lo) * 3.1415926 / 180.0)) / Actual_A);

                Actual_angle = (float)(Actual_angle * 180.0 / 3.1415926);
                ////////////////////////////////四个象限/////////////////////////////////////////////////////////////
                if (second_destination_la > first_destination_la && second_destination_lo > first_destination_lo)
                {
                    Actual_angle = -Actual_angle;
                }
                if (second_destination_la < first_destination_la && second_destination_lo > first_destination_lo)
                {
                    Actual_angle = -180 + Actual_angle;
                }
                if (second_destination_la < first_destination_la && second_destination_lo < first_destination_lo)
                {
                    Actual_angle = 180 + Actual_angle;
                }
                if (second_destination_la > first_destination_la && second_destination_lo < first_destination_lo)
                {
                    Actual_angle = -Actual_angle;
                }
                ///////////////////////////////四个象限////////////////////////////////////////////////////////////////
            }
            if (second_destination_la == first_destination_la && second_destination_lo >= first_destination_lo)
            {
                //Actual_angle = -90;
                Actual_angle = -90;
            }
            if (second_destination_la == first_destination_la && second_destination_lo <= first_destination_lo)
            {
               // Actual_angle = 90;
                Actual_angle = 90;
            }
            if (second_destination_la >= first_destination_la && second_destination_lo == first_destination_lo)
            {
                Actual_angle = 0;
            }
            if (second_destination_la <= first_destination_la && second_destination_lo == first_destination_lo)
            {
                Actual_angle = 180;
            }
            if (Actual_angle > 0 && Actual_angle<180)
            {
                Actual_angle = 360 - Actual_angle;
            }
            if (Actual_angle > -180 && Actual_angle<0)
            {
                Actual_angle = -Actual_angle;
            }
            if (Actual_angle == -180 || Actual_angle==180)
            {
                Actual_angle = 180;
            }
            return Actual_angle;
            //return (float)(Actual_angle * 3.1415926 / 180);
        }
        void UAVWayPoint_Loaded(object sender, RoutedEventArgs e)
        {
            if (icon.Source.CanFreeze)
             {
                 icon.Source.Freeze();
             }
        }
        void UAVWayPoint_Unloaded(object sender, RoutedEventArgs e)
        {
			try
			{
				Marker.ZIndex -= 10000;
				Popup.IsOpen = false;

				this.Unloaded -= new RoutedEventHandler(UAVWayPoint_Unloaded);
				this.Loaded -= new RoutedEventHandler(UAVWayPoint_Loaded);
				Popup = null;
				Label = null;

				EndFlag = false;
			}
			catch
			{

			}
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
			}
			catch
			{

			}
        }
    }
}