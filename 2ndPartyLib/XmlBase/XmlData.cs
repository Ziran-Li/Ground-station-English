using System;
using System.Collections.Generic;

// 
using GMap.NET;                         //GMap.NET Core

//
using SysTime;
using MulitWaypoint;
using Point;
using Health;
using Plot;

namespace XmlBase
{
    public class XmlData
    {
        public static readonly XmlData Empty = new XmlData();

        public XmlData()
        {
            _time = new SystemTime();
            _MulitWaypoint = new MulitWaypointData();
            _UAVpoint = new PointData();
            _Homepoint = new PointData();
            _Waypoint = new PointData();
            _NWaypoint = new PointData();
			_Subpoint = new PointData();
            // メイン画面表示HealthData
            _battery = 0;                 // バッテリ電圧残量
            _nSat = 0;                      // GPSの有効衛星数
            _IsPingCommandReplying = false;    // Connected/Disconnected
            _nControlMode = 0;              // Controlモード
            _nControlModeName = "";
            _nReset = false;
            _nFlightMode = 0;               // Flightモード
            _nFlightModeName = "";
            _nDriveMode = -1;
			_commandReplay = -1;

            // MulitWaypoint転送状況
            _Percent = 0;

            //　再生時間
            _currentTime = 0;

            _Port = "";						// 通信ポート
            _FilePath = "";
            _FlightLogFilePath = "";		//「飛行ログを開く」の絶対パス
            _FlightLogFileSavePath = "";	//「飛行ログを保存」の絶対パス
			_FlightPlanFilePath = "";		//  飛行計画ファイルの絶対パス
			_FlightPlan = "";				// 飛行計画　XML
            // Plot Data
            _PlotWindow = new List<PlotData>();

            // HealthData
            _Health = new HealthData();

			_errorID = new int[] { -1, -1};
			_errorLevel = new int[] { -1, -1 };
			_errorTicks = new int[] { -1, -1 };
			_errorCategory = new int[] { -1, -1 };
			_errorParam = new int[] { -1, -1 };
			_errorGCSTimer = new int[] { 0, 0};
        }

        // ================================================================================
        //getメッセージ
        #region getメッセージ

        // 時刻
        private SystemTime _time;

        // メイン画面表示HealthData
        private double _battery;                 // バッテリ電圧残量
        private int _nSat;                      // GPSの有効衛星数
        private bool _IsPingCommandReplying;    // Connected/Disconnected
        private int _nControlMode;              // Controlモード
        private string _nControlModeName;
        private bool _nReset;
        private int _nFlightMode;               // Flightモード
        private string _nFlightModeName;
        private int _nDriveMode;				// Driveモード(ロータ回転) 0:停止 1:アイドリング 2:飛行中

        // UAV現在位置
        private PointData _UAVpoint;

        // Home位置
        private PointData _Homepoint;

        // Waypoint設定中
        private PointData _NWaypoint;

		// Sub
		private PointData _Subpoint;

        // MulitWaypoint転送状況
        private int _Percent;

        // UAV詳細画面表示HealthData
        private HealthData _Health;

        //　再生時間
        private double _currentTime;

		// コマンド到達確認
		private int _commandReplay;

        #endregion

        // ================================================================================
        //setメッセージ
        #region setメッセージ

        private string _Port;					// 通信ポート
        private string _FlightLogFilePath;		//「飛行ログを開く」の絶対パス
        private string _FlightLogFileSavePath;	//「飛行ログを保存」の絶対パス
        private string _FilePath;
		private string _FlightPlanFilePath;		// 飛行計画ファイルの絶対パス
        private string _FlightPlan = "";		// 飛行計画　XML    
		#endregion

        // ================================================================================
        //mainlinksetメッセージ
        #region mainlinksetメッセージ

        // Waypoint UAV目標位置
        private PointData _Waypoint;

        // MulitWaypoint
        private MulitWaypointData _MulitWaypoint;

        #endregion

        // ================================================================================
        // Plot Data
        private List<PlotData> _PlotWindow;

		private int[] _errorID;
		private int[] _errorLevel;
		private int[] _errorTicks;
		private int[] _errorCategory;
		private int[] _errorParam;
		private int[] _errorGCSTimer;

        // ================================================================================
        //プロパティ
        #region プロパティ
		public int CommandReplay
		{
			get { return this._commandReplay; }
			set { this._commandReplay = value; }
		}
        public SystemTime Time
        {
            get { return this._time; }
            set { this._time = value; }
        }

        #region メイン画面表示HealthData
        public double BatteryVoltage
        {
            get { return this._battery; }
            set { this._battery = value; }
        }
        public int nSat
        {
            get { return this._nSat; }
            set { this._nSat = value; }
        }
        public bool IsPingCommandReplying
        {
            get { return this._IsPingCommandReplying; }
            set { this._IsPingCommandReplying = value; }
        }
        public int nControlMode
        {
            get { return this._nControlMode; }
            set { this._nControlMode = value; }
        }
        public string nControlModeName
        {
            get { return this._nControlModeName; }
            set { this._nControlModeName = value; }
        }
        public bool nReset
        {
            get { return this._nReset; }
            set { this._nReset = value; }
        }
        public int nFlightMode
        {
            get { return this._nFlightMode; }
            set { this._nFlightMode = value; }
        }
        public string nFlightModeName
        {
            get { return this._nFlightModeName; }
            set { this._nFlightModeName = value; }
        }
        public int nDriveMode
        {
            get { return this._nDriveMode; }
            set { this._nDriveMode = value; }
        }
        public int Percent
        {
            get { return this._Percent; }
            set { this._Percent = value; }
        }
        #endregion

        public double CurrentTime
        {
            get { return this._currentTime; }
            set { this._currentTime = value; }
        }
        public PointData UAVpoint
        {
            get { return this._UAVpoint; }
            set { this._UAVpoint = value; }
        }
        public PointData Homepoint
        {
            get { return this._Homepoint; }
            set { this._Homepoint = value; }
        }
        public PointData NWaypoint
        {
            get { return this._NWaypoint; }
            set { this._NWaypoint = value; }
        }
        public HealthData Health
        {
            get { return this._Health; }
            set { this._Health = value; }
        }

        #region setメッセージ
        public string Port
        {
            get { return this._Port; }
            set { this._Port = value; }
        }
        public string FilePath
        {
            get { return this._FilePath; }
            set { this._FilePath = value; }
        }
        public string FlightLogFilePath
        {
            get { return this._FlightLogFilePath; }
            set { this._FlightLogFilePath = value; }
        }
        public string FlightLogFileSavePath
        {
            get { return this._FlightLogFileSavePath; }
            set { this._FlightLogFileSavePath = value; }
        }
		public string FlightPlanFilePath
		{
			get { return this._FlightPlanFilePath; }
			set { this._FlightPlanFilePath = value; }
		}
		public string FlightPlan
		{
			get { return this._FlightPlan; }
			set { this._FlightPlan = value; }
		}
        #endregion

        #region mainlinksetメッセージ

        public MulitWaypointData MulitWaypoint
        {
            get { return this._MulitWaypoint; }
            set { this._MulitWaypoint = value; }
        }
        public PointData Waypoint
        {
            get { return this._Waypoint; }
            set { this._Waypoint = value; }
        }
        #endregion

		public PointData SubPoint
		{
			get { return this._Subpoint; }
			set { this._Subpoint = value; }
		}
        public List<PlotData> PlotWindow
        {
            get { return this._PlotWindow; }
            set { this._PlotWindow = value; }
        }
		public int[] ErrorID
		{
			get { return this._errorID; }
			set { this._errorID = value; }
		}
		public int[] ErrorLevel
		{
			get { return this._errorLevel; }
			set { this._errorLevel = value; }
		}
		public int[] ErrorTicks
		{
			get { return this._errorTicks; }
			set { this._errorTicks = value; }
		}
		public int[] ErrorCategory
		{
			get { return this._errorCategory; }
			set { this._errorCategory = value; }
		}
		public int[] ErrorParam
		{
			get { return this._errorParam; }
			set { this._errorParam = value; }
		}
		public int[] ErrorGCSTimer
		{
			get { return this._errorGCSTimer; }
			set { this._errorGCSTimer = value; }
		}
        #endregion
    }
}
