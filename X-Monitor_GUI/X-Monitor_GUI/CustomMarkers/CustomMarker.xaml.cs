using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
using System.Diagnostics;

using System.Threading;
using System.Windows.Media.Imaging;

//line
using System.Windows.Shapes;
using System.ComponentModel;
using GMap.NET;
using GMap.NET.WindowsPresentation;

namespace XMonitor_GUI.CustomMarkers
{
	/// <summary>
	/// CustomMarker.xaml の相互作用ロジック
	/// </summary>
	public partial class CustomMarker : UserControl
	{
		Popup Popup;
		Label Label;
		GMapMarker Marker;

		private string _name;
		bool EndFlag = true;    //スレッドループ
		double ang = 0;

		public CustomMarker(GMapMarker marker, string name, string path)
		{
			InitializeComponent();

			// CustomMarkerのアイコンを変更
			icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath(path)));
			this.Marker = marker;
			this.Marker.Mode = 1;

			Popup = new Popup();    //ポップアップ表示
			Label = new Label();    //ポップアップのコメント表示

			_name = name;

			Number.Content = "";

			Popup.Placement = PlacementMode.Mouse;
			{
				Label.Background = Brushes.Blue;
				Label.Foreground = Brushes.White;
				Label.BorderBrush = Brushes.WhiteSmoke;
				Label.BorderThickness = new Thickness(2);
				Label.Padding = new Thickness(5);
				Label.FontSize = 12;
				// PopUp表示の値更新
				if (_name == "Home")
				{
					if (Label != null)
					{
						Label.Content = _name +
									"\nLat/" + this.Marker.Position.Lat.ToString("0.0000000") +
									"\nLng/" + this.Marker.Position.Lng.ToString("0.0000000") +
									"\nAng/" + this.Marker.Azimuth.ToString("0.0000000") +
									"\nAlt/" + this.Marker.Altitude.ToString("0.0000000");
					}
				}
				else
				{
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
										Label.Content = _name +
														"\nLat/" + this.Marker.Position.Lat.ToString("0.0000000") +
														"\nLng/" + this.Marker.Position.Lng.ToString("0.0000000") +
														"\nAng/" + this.Marker.Azimuth.ToString("0.0000000") +
														"\nAlt/" + this.Marker.Altitude.ToString("0.0000000") +
														"\nSpd/" + this.Marker.Speed.ToString("0.0000000");
									}
									else
									{
										Label = new Label();
									}

									// UAVマーカーの方位角の更新
									if (Marker.Azimuth != ang)
									{
										ang = this.Marker.Azimuth;
										if (ang < 0)
										{
											ang += 360;
										}
										icon.RenderTransform = new RotateTransform { Angle = ang };
									}
									if (Marker.ID != 0)
									{
										Number.Content = Marker.ID.ToString();
									}
								}));
							}
							catch
							{

							}
							
							System.Threading.Thread.Sleep(100);   //100msec
						}
					});


				}

			}
			Popup.Child = Label;

		}

		void UAVWayPoint_Unloaded(object sender, RoutedEventArgs e)
		{
			try
			{
				Marker.ZIndex -= 10000;
				Popup.IsOpen = false;

				this.Unloaded -= new RoutedEventHandler(UAVWayPoint_Unloaded);
				this.MouseEnter -= new MouseEventHandler(MarkerControl_MouseEnter);
				this.MouseLeave -= new MouseEventHandler(MarkerControl_MouseLeave);
				Popup = null;
				Label = null;

				Marker.Shape = null;

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
				// PopUp表示の値更新
				if (_name == "Home")
				{
					Label.Content = _name +
									"\nLat/" + this.Marker.Position.Lat.ToString("0.0000000") +
									"\nLng/" + this.Marker.Position.Lng.ToString("0.0000000") +
									"\nAng/" + this.Marker.Azimuth.ToString("0.0000000") +
									"\nAlt/" + this.Marker.Altitude.ToString("0.0000000");
				}
				else
				{
					Label.Content = _name +
									"\nLat/" + this.Marker.Position.Lat.ToString("0.0000000") +
									"\nLng/" + this.Marker.Position.Lng.ToString("0.0000000") +
									"\nAng/" + this.Marker.Azimuth.ToString("0.0000000") +
									"\nAlt/" + this.Marker.Altitude.ToString("0.0000000") +
									"\nSpd/" + this.Marker.Speed.ToString("0.0000000");
				}
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
				// PopUp表示の値更新
				if (_name == "Home")
				{
					Label.Content = _name +
									"\nLat/" + this.Marker.Position.Lat.ToString("0.0000000") +
									"\nLng/" + this.Marker.Position.Lng.ToString("0.0000000") +
									"\nAng/" + this.Marker.Azimuth.ToString("0.0000000") +
									"\nAlt/" + this.Marker.Altitude.ToString("0.0000000");
				}
				else
				{
					Label.Content = _name +
									"\nLat/" + this.Marker.Position.Lat.ToString("0.0000000") +
									"\nLng/" + this.Marker.Position.Lng.ToString("0.0000000") +
									"\nAng/" + this.Marker.Azimuth.ToString("0.0000000") +
									"\nAlt/" + this.Marker.Altitude.ToString("0.0000000") +
									"\nSpd/" + this.Marker.Speed.ToString("0.0000000");
				}
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
