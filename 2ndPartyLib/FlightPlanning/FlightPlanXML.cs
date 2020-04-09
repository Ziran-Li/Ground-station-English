using System;
using System.Collections.Generic;
using System.Xml.Linq;              // XDocument オブジェクト
using System.Xml.XPath;
// ファイルの保存・読み出し時
using System.Xml;
//Process関連

// GMap.NETライブラリ
using GMap.NET;                         //GMap.NET Core

namespace FlightPlanning
{
    static class ConstantsGPS
    {
        public const double ELLIPSOID_A = 6378137.0;
        public const double ELLIPSOID_F = 0.00335281066475;
        public const double ELLIPSOID_B = (ELLIPSOID_A * (1.0 - ELLIPSOID_F));
    }
	// WPP形式
    public static class ConstantsWPP
    {
		
        public const string root = "root";
        public const string map = "map";
        public const string LatOrigin = "LatOrigin";
        public const string LonOrigin = "LonOrigin";
        public const string mission = "mission";
        public const string ID = "ID";
        public const string NS_Transition = "NS_Transition";
        public const string EW_Transition = "EW_Transition";
        public const string V_Transition = "V_Transition";
        public const string ScaleX = "ScaleX";
        public const string ScaleY = "ScaleY";
        public const string ScaleZ = "ScaleZ";
        public const string Rotation = "Rotation";
        public const string waypoint = "waypoint";
        //public const string ID = "ID";
        public const string x = "x";
        public const string y = "y";
        public const string height = "height";
        public const string psi = "psi";
        public const string speed = "speed";
        public const string accuracy = "accuracy";
        public const string wait = "wait";
        public const string func_id = "func_id";
    }
	// YPP形式
	public static class ConstantsYPP
	{
		
		public const string root = "root";
		public const string Mission = "Mission";
		public const string enabled = "enabled";
		public const string mName = "Name";
		public const string Origin = "Origin";
		public const string Latitude = "Latitude";
		public const string Longitude = "Longitude";
		public const string Altitude = "Altitude";
		public const string Rotation = "Rotation";
		public const string ScaleX = "ScaleX";
		public const string ScaleY = "ScaleY";
		public const string ScaleZ = "ScaleZ";
		public const string AltitudeMode = "AltitudeMode";
		public const string Waypoint = "Waypoint";
		public const string id = "id";
		public const string wName = "Name";
		public const string x = "x";
		public const string y = "y";
		public const string Height = "Height";
		public const string Heading = "Heading";
		public const string ThreasholdH = "ThreasholdH";
		public const string ThreasholdV = "ThreasholdV";
		public const string Wait = "Wait";
		public const string Function = "Function";
		public const string FunctionParam = "FunctionParam";
		public const string Path = "Path";
		public const string Waypoint1 = "Waypoint1";
		public const string Waypoint2 = "Waypoint2";
		public const string Speed = "Speed";
		public const string Shape = "Shape";
	}

    public class FlightPlanXML
    {
        // wpp書き込み
        public static string WppWrite(string _filePath, List<Project> _projectList)
        {
			try
			{
				if (_projectList.Count > 0)
				{
					var _xml = new XDocument();
					var _root = new XElement(ConstantsWPP.root, "");

					_root.Add(new XElement(ConstantsWPP.map, ""));
					_root.Add(new XElement(ConstantsWPP.LatOrigin, _projectList[0].MissionData.Latitude));
					_root.Add(new XElement(ConstantsWPP.LonOrigin, _projectList[0].MissionData.Longitude));

					int j = 0;
					foreach (var _project in _projectList)
					{
						var _mission = new XElement(ConstantsWPP.mission, "");
						_mission.Add(new XElement(ConstantsWPP.ID, ""));

						PointLatLng _p1 = new PointLatLng(_projectList[0].MissionData.Latitude,_projectList[0].MissionData.Longitude);
						PointLatLng _p2 = new PointLatLng(_project.MissionData.Latitude, _project.MissionData.Longitude);
						System.Windows.Point _p = MeterPerLatLng(_p1,_p2);
						_mission.Add(new XElement(ConstantsWPP.NS_Transition, _p.X));
						_mission.Add(new XElement(ConstantsWPP.EW_Transition, _p.Y));
						_mission.Add(new XElement(ConstantsWPP.V_Transition, 0));
						_mission.Add(new XElement(ConstantsWPP.ScaleX, _project.MissionData.ScaleX));
						_mission.Add(new XElement(ConstantsWPP.ScaleY, _project.MissionData.ScaleY));
						_mission.Add(new XElement(ConstantsWPP.ScaleZ, _project.MissionData.ScaleZ));
						_mission.Add(new XElement(ConstantsWPP.Rotation, _project.MissionData.Rotation * Math.PI / 180));
						// Waypoint
						int i = 0;
						foreach (var _waypointData in _project.WaypointData)
						{
							var _waypoint = new XElement(ConstantsWPP.waypoint, "");
							_waypoint.Add(new XElement(ConstantsWPP.ID, _waypointData.ID));
							_waypoint.Add(new XElement(ConstantsWPP.x, _waypointData.x));
							_waypoint.Add(new XElement(ConstantsWPP.y, _waypointData.y));
							_waypoint.Add(new XElement(ConstantsWPP.height, _waypointData.Height));
							_waypoint.Add(new XElement(ConstantsWPP.psi, (_waypointData.Heading * Math.PI / 180)));	//  rad->deg 変換));
							//_waypoint.Add(new XElement(ConstantsWPP.speed, _waypointData));
							_waypoint.Add(new XElement(ConstantsWPP.accuracy, _waypointData.ThreasholdH));
							_waypoint.Add(new XElement(ConstantsWPP.wait, _waypointData.Wait));
							_waypoint.Add(new XElement(ConstantsWPP.func_id, _waypointData.Function));

							//_waypoint.Add(new XElement(ConstantsWPP.speed, _project.PathData[i].Speed));
							_waypoint.Add(new XElement(ConstantsWPP.speed, _waypointData.Speed));

							_mission.Add(_waypoint);
							i++;
						}
						_root.Add(_mission);
						j++;
					}
					_xml.Add(_root);
					_xml.Save(_filePath);

					return _xml.ToString();
				}

				return "";
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(401) MultiWaypoint FileOpen Error: " + ex.Message);
				return "";
			}
        }
        // wpp読み込み
        public static List<Project> WppRead(string _filePath )
        {
            List<Project> _projectList = new List<Project>();

            // wpp読み込み 
            XDocument _xml = XDocument.Load(_filePath);
            var _xmlRoot = _xml.Root;

			PointLatLng _ori = new PointLatLng();
			PointLatLng _point = new PointLatLng();
            int i = 0;
            foreach (var _element in _xmlRoot.XPathSelectElements(ConstantsWPP.mission))
            {
                var _project = new Project();
                //var _a = _xml.Root.Element(ConstantsWPP.LatOrigin);
                //_a.Value

				//LatLngPerMeter

				if(i == 0)
				{
					_project.MissionData.Latitude = double.Parse(_xmlRoot.XPathSelectElement(ConstantsWPP.LatOrigin).Value);
					_project.MissionData.Longitude = double.Parse(_xmlRoot.XPathSelectElement(ConstantsWPP.LonOrigin).Value);
					_ori = new PointLatLng(_project.MissionData.Latitude,_project.MissionData.Longitude);
					_point =  LatLngPerMeter(_ori);
				}
				else
				{
					var x = double.Parse(_element.XPathSelectElement(ConstantsWPP.NS_Transition).Value);
					var y = double.Parse(_element.XPathSelectElement(ConstantsWPP.EW_Transition).Value);
					_project.MissionData.Latitude = (x / _point.Lat) + _ori.Lat;
					_project.MissionData.Longitude = (y / _point.Lng) + _ori.Lng;
				}


 
                _project.MissionData.ScaleX = double.Parse(_element.XPathSelectElement(ConstantsWPP.ScaleX).Value);
                _project.MissionData.ScaleY = double.Parse(_element.XPathSelectElement(ConstantsWPP.ScaleY).Value);
                _project.MissionData.ScaleZ = double.Parse(_element.XPathSelectElement(ConstantsWPP.ScaleZ).Value);

                _project.MissionData.Rotation = double.Parse(_element.XPathSelectElement(ConstantsWPP.Rotation).Value);
				_project.MissionData.Rotation *= 180 / Math.PI;	//  rad->deg 変換
                

                // TODO
				_project.MissionData.Name = "Mission" + (i + 1).ToString();
                _project.MissionData.enabled = true;
                _project.MissionData.Altitude = 0;
                _project.MissionData.AltitudeMode = "relativeToHome";

                int j = 0;
                foreach (var _element2 in _element.XPathSelectElements(ConstantsWPP.waypoint))
                {
                    var _watpoint = new Waypoint();
                    string a = _element2.XPathSelectElement(ConstantsWPP.ID).Value;                    
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

                    var _path = new Path();
					_path.Speed = _watpoint.Speed;
                    _path.Shape = "Linear";
                    _path.Waypoint1 = j + 1;
                    _path.Waypoint2 = i + 2;

                    _project.WaypointData.Add(_watpoint);
                    _project.PathData.Add(_path);
                    j++;
                }
                
                //　座標変換
                Translation(ref _project);

                // 追加
                _projectList.Add(_project);
				++i;
            }

            return _projectList;
        }

		// ypp書き込み
		public static void YppWrite(string _filePath, List<Project> _projectList)
		{
			try
			{
				if (_projectList.Count > 0)
				{
					var _xml = new XDocument();
					var _root = new XElement(ConstantsYPP.root, "");



					foreach (var _project in _projectList)
					{
						var _mission = new XElement(ConstantsYPP.Mission, "");
						_mission.SetAttributeValue(ConstantsYPP.enabled, _project.MissionData.enabled);					// enabled
						_mission.Add(new XElement(ConstantsYPP.mName, _project.MissionData.Name));						// Name

						var _origin = new XElement(ConstantsYPP.Origin, "");
						_origin.Add(new XElement(ConstantsYPP.Latitude, _project.MissionData.Latitude));							// Latitude
						_origin.Add(new XElement(ConstantsYPP.Longitude, _project.MissionData.Longitude));				// Longitude
						_origin.Add(new XElement(ConstantsYPP.Altitude, _project.MissionData.Altitude));				// Altitude
						_origin.Add(new XElement(ConstantsYPP.Rotation, _project.MissionData.Rotation));				// Rotation
						_mission.Add(_origin);

						_mission.Add(new XElement(ConstantsYPP.ScaleX, _project.MissionData.ScaleX));					// ScaleX
						_mission.Add(new XElement(ConstantsYPP.ScaleY, _project.MissionData.ScaleY));					// ScaleY
						_mission.Add(new XElement(ConstantsYPP.ScaleZ, _project.MissionData.ScaleZ));					// ScaleZ

						_mission.Add(new XElement(ConstantsYPP.AltitudeMode, _project.MissionData.AltitudeMode));

						// Waypoint
						foreach (var _waypointData in _project.WaypointData)
						{
							var _waypoint = new XElement(ConstantsYPP.Waypoint, "");
							_waypoint.SetAttributeValue(ConstantsYPP.id, _waypointData.ID);						// ID

							_waypoint.Add(new XElement(ConstantsYPP.wName, _waypointData.Name));
							_waypoint.Add(new XElement(ConstantsYPP.x, _waypointData.x));
							_waypoint.Add(new XElement(ConstantsYPP.y, _waypointData.y));
							_waypoint.Add(new XElement(ConstantsYPP.Height, _waypointData.Height));
							_waypoint.Add(new XElement(ConstantsYPP.Heading, _waypointData.Heading));
							_waypoint.Add(new XElement(ConstantsYPP.ThreasholdH, _waypointData.ThreasholdH));
							_waypoint.Add(new XElement(ConstantsYPP.ThreasholdV, _waypointData.ThreasholdV));
							_waypoint.Add(new XElement(ConstantsYPP.Wait, _waypointData.Wait));
							_waypoint.Add(new XElement(ConstantsYPP.Function, _waypointData.Function));
							//_waypoint.Add(new XElement(ConstantsYPP.FunctionParam, _waypointData.Function));
							_mission.Add(_waypoint);
						}
						// Path
						foreach (var _pathData in _project.PathData)
						{
							var _path = new XElement(ConstantsYPP.Path, "");

							_path.Add(new XElement(ConstantsYPP.Waypoint1, _pathData.Waypoint1));
							_path.Add(new XElement(ConstantsYPP.Waypoint2, _pathData.Waypoint2));
							_path.Add(new XElement(ConstantsYPP.Speed, _pathData.Speed));
							_path.Add(new XElement(ConstantsYPP.Shape, _pathData.Shape));

							_mission.Add(_path);
						}
						_root.Add(_mission);
					}
					_xml.Add(_root);
					_xml.Save(_filePath);
				}
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(401) MultiWaypoint FileOpen Error: " + ex.Message);
			}
		}
		// ypp読み込み
		public static List<Project> YppRead(string _filePath)
		{
			List<Project> _projectList = new List<Project>();
			try
			{
				// wpp読み込み 
				XDocument _xml = XDocument.Load(_filePath);
				var _xmlRoot = _xml.Root;

				int i = 0;
				foreach (var _element in _xmlRoot.XPathSelectElements(ConstantsYPP.Mission))
				{
					var _project = new Project();

					_project.MissionData.enabled = bool.Parse(_element.Attribute(ConstantsYPP.enabled).Value);
					_project.MissionData.Name = "Mission" + (i+1).ToString();

					_project.MissionData.Latitude = double.Parse(_element.XPathSelectElement(ConstantsYPP.Origin +"/"+ ConstantsYPP.Latitude).Value);
					_project.MissionData.Longitude = double.Parse(_element.XPathSelectElement(ConstantsYPP.Origin + "/" + ConstantsYPP.Longitude).Value);
					_project.MissionData.Altitude = double.Parse(_element.XPathSelectElement(ConstantsYPP.Origin + "/" + ConstantsYPP.Altitude).Value);
					_project.MissionData.Rotation = double.Parse(_element.XPathSelectElement(ConstantsYPP.Origin + "/" + ConstantsYPP.Rotation).Value);

					_project.MissionData.ScaleX = double.Parse(_element.XPathSelectElement(ConstantsYPP.ScaleX).Value);
					_project.MissionData.ScaleY = double.Parse(_element.XPathSelectElement(ConstantsYPP.ScaleY).Value);
					_project.MissionData.ScaleZ = double.Parse(_element.XPathSelectElement(ConstantsYPP.ScaleZ).Value);


					_project.MissionData.AltitudeMode = _element.XPathSelectElement(ConstantsYPP.AltitudeMode).Value;

					int j = 0;
					foreach (var _waypointData in _element.XPathSelectElements(ConstantsYPP.Waypoint))
					{
						var _watpoint = new Waypoint();
						string a = _waypointData.Attribute(ConstantsYPP.id).Value;
						if (a != "")
						{
							_watpoint.ID = int.Parse(a);
						}
						else
						{
							_watpoint.ID = j + 1;
						}

						_watpoint.Name = _waypointData.XPathSelectElement(ConstantsYPP.wName).Value;
						_watpoint.x = double.Parse(_waypointData.XPathSelectElement(ConstantsYPP.x).Value);
						_watpoint.y = double.Parse(_waypointData.XPathSelectElement(ConstantsYPP.y).Value);
						_watpoint.Height = double.Parse(_waypointData.XPathSelectElement(ConstantsYPP.Height).Value);
						_watpoint.Heading = double.Parse(_waypointData.XPathSelectElement(ConstantsYPP.Heading).Value);
						_watpoint.ThreasholdH = double.Parse(_waypointData.XPathSelectElement(ConstantsYPP.ThreasholdH).Value);
						_watpoint.ThreasholdV = double.Parse(_waypointData.XPathSelectElement(ConstantsYPP.ThreasholdV).Value);
						_watpoint.Wait = double.Parse(_waypointData.XPathSelectElement(ConstantsYPP.Wait).Value);
						_watpoint.Function = double.Parse(_waypointData.XPathSelectElement(ConstantsYPP.Function).Value);

						

						_project.WaypointData.Add(_watpoint);
						j++;
					}
					foreach (var _pathData in _element.XPathSelectElements(ConstantsYPP.Path))
					{
						var _path = new Path();
						_path.Speed = double.Parse(_pathData.XPathSelectElement(ConstantsYPP.Speed).Value);
						_path.Shape = _pathData.XPathSelectElement(ConstantsYPP.Shape).Value;
						_path.Waypoint1 = int.Parse(_pathData.XPathSelectElement(ConstantsYPP.Waypoint1).Value);
						_path.Waypoint2 = int.Parse(_pathData.XPathSelectElement(ConstantsYPP.Waypoint1).Value);

						_project.PathData.Add(_path);
					}
					//　座標変換
					Translation(ref _project);

					// 追加
					_projectList.Add(_project);

					++i;
				}
				
				return _projectList;
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(403) FlightLog Edit Error : " + ex.Message);
				return _projectList;
			}
			
		}

		public static void KmlWrite(string _filePath, List<Project> _projectList)
		{
			try
			{
				if (_projectList.Count > 0)
				{
					//XDocument _xml = XDocument.Parse(_str);
					XDocument _xml = XDocument.Load(@"Xml/FlightPlanning.kml");
					XmlNamespaceManager xnm = new XmlNamespaceManager(new NameTable());
					xnm.AddNamespace("x", "http://www.opengis.net/kml/2.2");

					foreach (var _project in _projectList)
					{

						_xml.XPathSelectElement("//x:Document/x:Folder/x:Folder/x:LookAt/x:longitude", xnm).Value = _project.MissionData.Longitude.ToString();
						_xml.XPathSelectElement("//x:Document/x:Folder/x:Folder/x:LookAt/x:latitude", xnm).Value = _project.MissionData.Latitude.ToString();
						// Waypoint
						string _text = "\n";
						foreach (var _waypointData in _project.WaypointData)
						{
							// 高度方向にオフセット
							_text += _waypointData.Longitude.ToString() + "," + _waypointData.Latitude.ToString() + "," + (_waypointData.Height + 2).ToString() + "\n";
						}
						_xml.XPathSelectElement("//x:Document/x:Folder/x:Folder/x:Placemark/x:LineString/x:coordinates", xnm).Value = _text;
						//_xml.XPathSelectElement("//x:Document/x:Folder/x:Folder/x:Placemark/x:Polygon/x:outerBoundaryIs/x:LinearRing/x:coordinates", xnm).Value = _text;
					}
					_xml.Save(_filePath);
				}
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(401) MultiWaypoint FileOpen Error: " + ex.Message);
			}
		}

        // 数字/文字チェック
        private string ValueCheck(string value)
        {
            string result = string.Empty;

            double d;
            if (double.TryParse(value, out d))
            {
                int i;
                if (int.TryParse(value, out i))
                {
                    return value;
                }
                else
                {
                    return value;
                }

            }
            else
            {
                return "0";
            }
        }
        // XMLデータ内の数値取得
        private string XmlDataGet(XElement xmlRoot, string tag, string att = "", string attValue = "")
        {
            try
            {
                //タグがない場合
                if (xmlRoot.XPathSelectElement(tag) == null)
                {
                    return "0";
                }

                // 空の場合
                if (String.IsNullOrEmpty(xmlRoot.XPathSelectElement(tag).Value))
                {
                    return "0";
                }

                string val = xmlRoot.XPathSelectElement(tag).Value;
                if (att != "")
                {
                    foreach (var element in xmlRoot.XPathSelectElements(tag))
                    {
                        string _attVal = element.Attribute(att).Value;
                        if (_attVal == attValue)
                        {
                            //System.Diagnostics.Debug.WriteLine("XmlDataGet: " + _attVal + "/" + element.Value);
                            return ValueCheck(element.Value);
                        }
                    }
                    val = "0";
                }

                return ValueCheck(val);
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("(802) XmlDataGet Error : " + ex.Message);
                return "0";
            }
        }

        // 飛行計画データのXYをLatLng変換 (経度緯度→メートルの計算)
        public static System.Windows.Point MeterPerLatLng(PointLatLng _orilat, PointLatLng _postion)
        {
            System.Windows.Point _point = new System.Windows.Point();

            double dPseudoLatitudeRad, dDistanceFromCE;
            double dSin, dCos;

            // 変換定数の計算
            PointLatLng r = new PointLatLng();

            //
            //Lat
            //
            // 擬似緯度の計算
            if (_orilat.Lat == 90.0 || _orilat.Lat == -90.0)
            {
                // tan の値が特異となる場合
                dPseudoLatitudeRad = _orilat.Lat * Math.PI / 180.0; // この場合は擬似緯度と緯度が等しい。
            }
            else
            {
                // tan の値が特異とならない場合
                dPseudoLatitudeRad = Math.Atan((1.0 - ConstantsGPS.ELLIPSOID_F) * (1.0 - ConstantsGPS.ELLIPSOID_F) * Math.Tan(_orilat.Lat * Math.PI / 180.0));
            }

            // 三角関数の計算
            dSin = Math.Sin(dPseudoLatitudeRad);
            dCos = Math.Cos(dPseudoLatitudeRad);

            // 地球中心と現在地点との距離の計算
            dDistanceFromCE = ConstantsGPS.ELLIPSOID_A * ConstantsGPS.ELLIPSOID_B / Math.Sqrt(ConstantsGPS.ELLIPSOID_A * ConstantsGPS.ELLIPSOID_A * dSin * dSin + ConstantsGPS.ELLIPSOID_B * ConstantsGPS.ELLIPSOID_B * dCos * dCos);

            r.Lat = (Math.PI / 180.0) * dDistanceFromCE;

            //
            //Lng
            //
            r.Lng = (Math.PI / 180.0) * dDistanceFromCE * dCos;


            _point.X = ((_postion.Lat - _orilat.Lat) * r.Lat);
            _point.Y = ((_postion.Lng - _orilat.Lng) * r.Lng);

            return _point;
        }

        // 飛行計画データのXYをLatLng変換 (メートル→経度緯度の計算)
        public static PointLatLng LatLngPerMeter(PointLatLng orilat)
        {
            double dPseudoLatitudeRad, dDistanceFromCE;
            double dSin, dCos;

            // 変換定数の計算
            PointLatLng r = new PointLatLng();

            //
            //Lat
            //
            // 擬似緯度の計算
            if (orilat.Lat == 90.0 || orilat.Lat == -90.0)
            {
                // tan の値が特異となる場合
                dPseudoLatitudeRad = orilat.Lat * Math.PI / 180.0; // この場合は擬似緯度と緯度が等しい。
            }
            else
            {
                // tan の値が特異とならない場合
                dPseudoLatitudeRad = Math.Atan((1.0 - ConstantsGPS.ELLIPSOID_F) * (1.0 - ConstantsGPS.ELLIPSOID_F) * Math.Tan(orilat.Lat * Math.PI / 180.0));
            }

            // 三角関数の計算
            dSin = Math.Sin(dPseudoLatitudeRad);
            dCos = Math.Cos(dPseudoLatitudeRad);

            // 地球中心と現在地点との距離の計算
            dDistanceFromCE = ConstantsGPS.ELLIPSOID_A * ConstantsGPS.ELLIPSOID_B / Math.Sqrt(ConstantsGPS.ELLIPSOID_A * ConstantsGPS.ELLIPSOID_A * dSin * dSin + ConstantsGPS.ELLIPSOID_B * ConstantsGPS.ELLIPSOID_B * dCos * dCos);

            r.Lat = (Math.PI / 180.0) * dDistanceFromCE;

            //
            //Lng
            //
            r.Lng = (Math.PI / 180.0) * dDistanceFromCE * dCos;


            return r;
        }

        // 飛行計画ファイル読み込み時の座標変換の反映
        private static void Translation(ref Project _project)
        {
            PointLatLng OriginMeter = LatLngPerMeter(new PointLatLng(_project.MissionData.Latitude,_project.MissionData.Longitude));

            // 水平面の変換
			double _ang = _project.MissionData.Rotation * Math.PI / 180;
			double c = Math.Cos(_ang);
			double s = Math.Sin(_ang);
            double sx = _project.MissionData.ScaleX; ;
            double sy = _project.MissionData.ScaleX; ;
            double T11 = c * sx;
            double T12 = -s * sy;
            double T21 = s * sx;
            double T22 = c * sy;

            // Waypoint
            foreach(var _waypoint in _project.WaypointData)
            {
                double xx = T11 * (_waypoint.x) + T12 * (_waypoint.y);
                double yy = T21 * (_waypoint.x) + T22 * (_waypoint.y);

                _waypoint.Latitude = _project.MissionData.Latitude + xx / OriginMeter.Lat;
                _waypoint.Longitude = _project.MissionData.Longitude + yy / OriginMeter.Lng;
                _waypoint.Height = _waypoint.Height * _project.MissionData.ScaleZ;
            }

        }
        // ユーザ操作時の座標変換の反映
		public static Waypoint InverseTranslation(Mission _mission, Waypoint _waypoint)
        {
			PointLatLng OriginMeter = LatLngPerMeter(new PointLatLng(_mission.Latitude, _mission.Longitude));

			// 水平面の変換
			double _ang = _mission.Rotation * Math.PI / 180;
			double c = Math.Cos(_ang);
			double s = Math.Sin(_ang);
			double sx = _mission.ScaleX;
			double sy = _mission.ScaleY;
			double T11 = c * sx;
			double T12 = -s * sy;
			double T21 = s * sx;
			double T22 = c * sy;

			double xx = T11 * (_waypoint.x) + T12 * (_waypoint.y);
			double yy = T21 * (_waypoint.x) + T22 * (_waypoint.y);

			_waypoint.Latitude = _mission.Latitude + xx / OriginMeter.Lat;
			_waypoint.Longitude = _mission.Longitude + yy / OriginMeter.Lng;
			_waypoint.Height = _waypoint.Height * _mission.ScaleZ;

			return _waypoint;
        }
    }
}
