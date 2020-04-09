using System;
//Process関連
using System.Windows;
using System.Xml.Linq;
using System.Xml.XPath;

using FlightPlanning;

namespace XMonitor_GUI
{
	/// <summary>
	/// MultiWaypointWindow.xaml の相互作用ロジック
	/// </summary>
	public partial class MultiWaypointWindow
	{
		private int _waypointNumber;
		Waypoint _homePotion;
		GMap.NET.PointLatLng _ori = new GMap.NET.PointLatLng();

		public MultiWaypointWindow()
		{
			InitializeComponent();
			_waypointNumber = 0;
			_homePotion = new Waypoint();
			_homePotion.Sub = "H";
			_homePotion.ID = 0;
			_homePotion.Name = "Home";
			_homePotion.Latitude = 0;
			_homePotion.Longitude = 0;
			_homePotion.x = 0;
			_homePotion.y = 0;
			_homePotion.Height = 0;
			_homePotion.Heading = 0;
			_homePotion.ThreasholdH = 0;
			_homePotion.ThreasholdV = 0;
			_homePotion.Wait = 0;
			_homePotion.Function = 0;
			_homePotion.Speed = 2;

		}
		// 「×」（閉じる）ボタン 
		private void Button_Click(object sender, RoutedEventArgs e)
		{
			Hide();     //非表示
			//Close();
		}

		public static readonly DependencyProperty MessageProperty = DependencyProperty.Register(
			"Message", typeof(string), typeof(MultiWaypointWindow),
			new FrameworkPropertyMetadata(
			default(string),
			FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
			OnMessageChanged));

		private static void OnMessageChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			MultiWaypointWindow control = (MultiWaypointWindow)d;
			string _getMessage = (string)e.NewValue;
			control.WaypointListView.Items.Clear();

			if (_getMessage != "")
			{
				// wpp読み込み 
				XDocument _xml = XDocument.Parse(_getMessage);
				var _xmlRoot = _xml.Root;

				control._ori.Lat = double.Parse(_xmlRoot.XPathSelectElement(ConstantsWPP.LatOrigin).Value);
				control._ori.Lng = double.Parse(_xmlRoot.XPathSelectElement(ConstantsWPP.LonOrigin).Value);
				foreach (var _element in _xmlRoot.XPathSelectElements(ConstantsWPP.mission))
				{
					var _project = new Project();
					//var _a = _xml.Root.Element(ConstantsWPP.LatOrigin);
					//_a.Value
					_project.MissionData.Latitude = double.Parse(_xmlRoot.XPathSelectElement(ConstantsWPP.LatOrigin).Value);
					_project.MissionData.Longitude = double.Parse(_xmlRoot.XPathSelectElement(ConstantsWPP.LonOrigin).Value);

					_project.MissionData.ScaleX = double.Parse(_element.XPathSelectElement(ConstantsWPP.ScaleX).Value);
					_project.MissionData.ScaleY = double.Parse(_element.XPathSelectElement(ConstantsWPP.ScaleY).Value);
					_project.MissionData.ScaleZ = double.Parse(_element.XPathSelectElement(ConstantsWPP.ScaleZ).Value);

					_project.MissionData.Rotation = double.Parse(_element.XPathSelectElement(ConstantsWPP.Rotation).Value);

					// TODO
					_project.MissionData.Name = "";
					_project.MissionData.enabled = true;
					_project.MissionData.Altitude = 0;
					_project.MissionData.AltitudeMode = "relativeToHome";

					int j = 0;
					foreach (var _element2 in _element.XPathSelectElements(ConstantsWPP.waypoint))
					{
						var _watpoint = new Waypoint();
						string a = _element2.XPathSelectElement(ConstantsWPP.ID).Value;
						_watpoint.Sub = "■";
						_watpoint.ID = j + 1;
						_watpoint.Name = "";
						_watpoint.x = double.Parse(_element2.XPathSelectElement(ConstantsWPP.x).Value);
						_watpoint.y = double.Parse(_element2.XPathSelectElement(ConstantsWPP.y).Value);
						_watpoint.Height = double.Parse(_element2.XPathSelectElement(ConstantsWPP.height).Value);
						_watpoint.Heading = double.Parse(_element2.XPathSelectElement(ConstantsWPP.psi).Value);
						_watpoint.Heading *= 180 / Math.PI;	//  rad->deg 変換
						_watpoint.ThreasholdH = double.Parse(_element2.XPathSelectElement(ConstantsWPP.accuracy).Value);
						_watpoint.ThreasholdV = 0.01;
						_watpoint.Wait = double.Parse(_element2.XPathSelectElement(ConstantsWPP.wait).Value);
						_watpoint.Function = double.Parse(_element2.XPathSelectElement(ConstantsWPP.func_id).Value);
						_watpoint.Speed = double.Parse(_element2.XPathSelectElement(ConstantsWPP.speed).Value);

						control.WaypointListView.Items.Add(_watpoint);     //WaypointListViewに追加

						j++;
					}
					break;
				}
				//

			}
		}

		public string Message
		{
			get { return (string)GetValue(MessageProperty); }
			set { SetValue(MessageProperty, value); }
		}

		public int WaypointNumber
		{
			get { return (int)_waypointNumber; }
			set 
			{
				_waypointNumber = value;

				if (WaypointListView.Items.Count > 0)
				{
					if (_waypointNumber > 0)
					{
						var _watpoint1 = new Waypoint();
						if (_waypointNumber > 1)
						{
							_watpoint1 = (Waypoint)WaypointListView.Items[_waypointNumber - 2];
							_watpoint1.Sub = "―";
							WaypointListView.Items[_waypointNumber - 2] = _watpoint1;
						}
						var _watpoint2 = (Waypoint)WaypointListView.Items[_waypointNumber - 1];
						_watpoint2.Sub = "▶";
						WaypointListView.Items[_waypointNumber - 1] = _watpoint2;
						WaypointListView.Items.Refresh();
					}
					if(_waypointNumber == int.MinValue)
					{
						var _watpoint = (Waypoint)WaypointListView.Items[WaypointListView.Items.Count - 2];
						_watpoint.Sub = "―";
						WaypointListView.Items[WaypointListView.Items.Count - 2] = _watpoint;
						WaypointListView.Items.Refresh();
					}
				}
			}
		}

		public Waypoint HomePotion
		{
			get { return _homePotion; }
			set
			{
				_homePotion = value;
				System.Windows.Point _p = FlightPlanXML.MeterPerLatLng(_ori, new GMap.NET.PointLatLng(_homePotion.Latitude,_homePotion.Latitude));
				_homePotion.Sub = "H";
				_homePotion.ID = WaypointListView.Items.Count + 1;
				_homePotion.x = _p.X;
				_homePotion.y = _p.Y;
				WaypointListView.Items.Add(_homePotion);     //WaypointListViewに追加
			}
		}
	}
}
