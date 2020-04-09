using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;

using System;
using System.Threading.Tasks;

//line
using GMap.NET.WindowsPresentation;

namespace Y_Planner.CustomMarkers
{
	/// <summary>
	/// Position.xaml の相互作用ロジック
	/// </summary>
	public partial class Position : UserControl
	{
		Popup Popup;
		Label Label;
		GMapMarker Marker;

		bool EndFlag = true;    //スレッドループ
		double ang = 0;

		public Position(GMapMarker marker)
		{
			InitializeComponent();

			this.Marker = marker;

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
				Label.Content = "纬度/" + this.Marker.Position.Lat.ToString("0.0000000") +
								"\n经度/" + this.Marker.Position.Lng.ToString("0.0000000") +
								"\n角度/" + this.Marker.Azimuth.ToString("0.0000000") +
								"\n高度/" + this.Marker.Altitude.ToString("0.0000000");
				//Label.Content = "Ang/0" + "\nLat/" + this.Marker.Position.Lat.ToString() + "\nLng/" + this.Marker.Position.Lng.ToString();
			}
			Popup.Child = Label;

			var task = Task.Factory.StartNew(() =>
			{

				while (EndFlag)
				{
					Dispatcher.BeginInvoke((Action)(() =>
					{
						if (Marker.IsMouseOver)
						{
							// PopUp表示の値更新
							Label.Content = "纬度/" + this.Marker.Position.Lat.ToString("0.0000000") +
											"\n经度/" + this.Marker.Position.Lng.ToString("0.0000000") +
											"\n角度/" + this.Marker.Azimuth.ToString("0.0000000") +
											"\n高度/" + this.Marker.Altitude.ToString("0.0000000");
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
					}));

					System.Threading.Thread.Sleep(20);   //100msec
				}
			});
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
				Marker.ZIndex -= 10000;
				Popup.IsOpen = false;
				Marker.IsMouseOver = false;
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
				Marker.IsMouseOver = true;
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
				Marker.ZIndex += 10000;
				Popup.IsOpen = true;
				Marker.IsMouseOver = true;
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
				Marker.IsMouseOver = false;
			}
			catch
			{

			}

		}
	}
}
