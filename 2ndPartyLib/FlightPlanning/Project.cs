using System;
using System.Collections.Generic;

using GMap.NET;                         //GMap.NET Core
using GMap.NET.WindowsPresentation;     //GMap.NET C# WPF用
namespace FlightPlanning
{
	[Serializable()]
    public class Project
    {
        private Mission _mission;					// ミッション管理クラス
        private List<Waypoint> _waypoint;			// ウェイポイント管理クラス(List)
        private List<Path> _path;					// 飛行ルートパス管理クラス(List)
		private GMapMarker _baseMarker;				// 基準点管理クラス
		private List<GMapMarker> _waypointMarker;	// Waypoint管理クラス
		private GMapRoute _waypointPath;			// ルート管理クラス
		private PointLatLng _oldBasePoint;			// 
		private GMapRoute _projectPath;

        public Project()
        {
            _mission = new Mission();
            _waypoint = new List<Waypoint>();
            _path = new List<Path>();
			_waypointMarker = new List<GMapMarker>();
			_waypointPath = new GMapRoute(new List<PointLatLng>());
			_oldBasePoint = new PointLatLng();
			_projectPath = new GMapRoute(new List<PointLatLng>());
        }

        #region プロパティ
        public Mission MissionData
        {
            get { return this._mission; }
            set { this._mission = value; }
        }
        public List<Path> PathData
        {
            get { return this._path; }
            set { this._path = value; }
        }
        public List<Waypoint> WaypointData
        {
            get { return this._waypoint; }
            set { this._waypoint = value; }
        }
		public GMapMarker BaseMarker
		{
			get { return this._baseMarker; }
			set { this._baseMarker = value; }
		}
		public  List<GMapMarker> WaypointMarker
		{
			get { return this._waypointMarker; }
			set { this._waypointMarker = value; }
		}
		public GMapRoute WaypointPath
		{
			get { return this._waypointPath; }
			set { this._waypointPath = value; }
		}
		public PointLatLng OldBasePoint
		{
			get { return this._oldBasePoint; }
			set { this._oldBasePoint = value; }
		}
		public GMapRoute ProjectPath
		{
			get { return this._projectPath; }
			set { this._projectPath = value; }
		}
        #endregion
    }
}
