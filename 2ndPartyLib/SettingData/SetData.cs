using System.Collections.Generic;
using System.Windows.Media;

using GMap.NET;
using GMap.NET.MapProviders;

namespace SettingData
{
    public class SetData
    {
        public static readonly SetData Empty = new SetData();

        private bool _Set;									// true:设置ＯＫ false:设置失败

		public bool SendCommandFlag { get; set; }
		public int Mode { get; set; }						//0:没有收到数据　1:收到的数据
		public string FlightLogFilePath { get; set; }
		public int DivideSizeInByte { get; set; }
		public int StoragePeriodInDay { get; set; }
		public int RecStopTimeFromDataStop { get; set; }

        //功能设置
        private string SerialPortSet;		//串行通信端口设置
        private string ModeSet;				//用户操作模式设置
        private string _coreIPaddress;
        private int _corePort;
        private bool Deviceflag;			// true:Mouse   false:Touch

        //监视相关设置
        private bool AlertLinkView;			//设置/解除 通信警告消息
        private bool AlertLinkSound;		//设置/解除 通信警告音消息
        private bool AlertVoltageView;		//设置/解除 电压警告量消息
        private bool AlertVoltageSound;		//设置/解除 电压警告音消息
        private bool AlertGPSView;			//设置/解除 GPS警告消息
        private bool AlertGPSSound;			//设置/解除 GPS警告音消息

        //地图相关
        private string Copyright;			//原始地图版权
        private PointLatLng LatLng;			//地图经纬度
        private GMapProvider Provider;		//地图提供商
        private bool Trajectory;			//显示/隐藏路径
        private double Trajectory_length;	//
        private double ElevationValue;
        private bool OnlineFlag;

        //屏幕
        private int Trajectory_mode;		// 1:地图中心坐标 2:机体中心坐标 3: Waypoint中心坐标

        // 选项屏幕显示和隐藏
        private bool PostionWindow;			//PostionWindowSet
        private bool PlotWindow;			//PlotWindowSet
        private List<int> window_no;		//选项屏幕数量

        private SolidColorBrush WindodColor;//TODO:窗口背景调整

        //　初始化
        public SetData()
        {
			SendCommandFlag = false;
			Mode = 0;
            _Set = false;

            _coreIPaddress = string.Empty;
            _corePort = 9999;

            SerialPortSet = string.Empty;
            ModeSet = string.Empty;
            AlertLinkView = false;
            AlertLinkSound = false;
            AlertVoltageView = false;
            AlertVoltageSound = false;
			AlertGPSView = false;
			AlertGPSSound = false;
            Trajectory_mode = 1;
            Trajectory_length = 0;
            ElevationValue = 0;
            OnlineFlag = false;

            LatLng = new PointLatLng();
            Provider = GMapProviders.OpenStreetMap;       //默认：OpenStreetMap
            Copyright = GMapProviders.OpenStreetMap.Copyright;

            Trajectory = false;
            Deviceflag = false;

            PostionWindow = false;
            PlotWindow = false;
            window_no = new List<int>();

            WindodColor = new SolidColorBrush();
        }

        #region 属性
        public bool Set
        {
            get { return this._Set; }
            set { this._Set = value; }
        }
        public string CoreIPaddress
        {
            get { return this._coreIPaddress; }
            set { this._coreIPaddress = value; }
        }
        public int CorePort
        {
            get { return this._corePort; }
            set { this._corePort = value; }
        }
        public string SerialPortSetting
        {
            get { return this.SerialPortSet; }
            set { this.SerialPortSet = value; }
        }
        public string ModeSetting
        {
            get { return this.ModeSet; }
            set { this.ModeSet = value; }
        }
        public bool AlertLinkViewFlag
        {
            get { return this.AlertLinkView; }
            set { this.AlertLinkView = value; }
        }
        public bool AlertLinkSoundFlag
        {
            get { return this.AlertLinkSound; }
            set { this.AlertLinkSound = value; }
        }
		public bool AlertGPSViewFlag
		{
			get { return this.AlertGPSView; }
			set { this.AlertGPSView = value; }
		}
		public bool AlertGPSSoundFlag
		{
			get { return this.AlertGPSSound; }
			set { this.AlertGPSSound = value; }
		}
        public bool AlertVoltageViewFlag
        {
            get { return this.AlertVoltageView; }
            set { this.AlertVoltageView = value; }
        }
        public bool AlertVoltageSoundFlag
        {
            get { return this.AlertVoltageSound; }
            set { this.AlertVoltageSound = value; }
        }

        public int TrajectoryMode
        {
            get { return this.Trajectory_mode; }
            set { this.Trajectory_mode = value; }
        }
        public double TrajectoryLength
        {
            get { return this.Trajectory_length; }
            set { this.Trajectory_length = value; }
        }
        public double Elevation
        {
            get { return this.ElevationValue; }
            set { this.ElevationValue = value; }
        }
        public bool Online
        {
            get { return this.OnlineFlag; }
            set { this.OnlineFlag = value; }
        }

        public string ProviderCopyright
        {
            get { return this.Copyright; }
            set { this.Copyright = value; }
        }
        public PointLatLng LatLngPoint
        {
            get { return this.LatLng; }
            set { this.LatLng = value; }
        }
        public GMapProvider MapProvider
        {
            get { return this.Provider; }
            set
            {
                this.Provider = value;
                Copyright = this.Provider.Copyright;
            }
        }

        public bool TrajectoryFlag
        {
            get { return this.Trajectory; }
            set { this.Trajectory = value; }
        }

        public bool DeviceMode
        {
            get { return this.Deviceflag; }
            set { this.Deviceflag = value; }
        }

        public bool PostionWindowSet
        {
            get { return this.PostionWindow; }
            set { this.PostionWindow = value; }
        }
        public bool PlotWindowSet
        {
            get { return this.PlotWindow; }
            set { this.PlotWindow = value; }
        }

        public SolidColorBrush WindodBackground
        {
            get { return this.WindodColor; }
            set { this.WindodColor = value; }
        }

        public List<int> WindowNo
        {
            get { return this.window_no; }
            set { this.window_no = value; }
        }
        #endregion
    }
}
