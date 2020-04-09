
namespace Point
{
    public class PointData
    {
        public static readonly PointData Empty = new PointData();

        private double _nlat;   // 現在緯度
        private double _nlon;   // 現在経度
        private double _nalt;   // 現在高度
        private double _nroll;   // Roll（現在方位角）　
        private double _npitch;  // Pitch
        private double _nyaw;    // Yaw
        private double _speed;	// speed
		private int _no;

//////////航拍增加的东西////////////////////////
        private double WayPoint_SetIndexEdit;
        private double WayPoint_KeepIndex1Edit;
        private double WayPoint_HeightEdit;
        private double WayPoint_YawEdit;
        private double WayPoint_HengxiangSuduEdit;
        private double WayPoint_ZongshangSuduEdit;
        private double WayPoint_ZongxiaSuduEdit;
        private double WayPoint_HoverTimerEdit;
        private double WayPoint_KeepIndex2Edit;
///////////航拍增加的东西////////////////////////
/// <summary>
/// /增加的数据显示部分
/// </summary>
        private double _IMUDataRoll;
        private double _IMUDataPitch;
        private double _IMUDataYaw;
        private double _GyroDataRollRate;
        private double _GyroDataPitchRate;
        private double _GyroDataYawRate;
        private double _AccDataX;
        private double _AccDataY;
        private double _AccDataZ;
        private double _MagDataX;
        private double _MagDataY;
        private double _MagDataZ;

        public PointData()
        {
            _nlat = 0;		// 現在緯度
            _nlon = 0;		// 現在経度
            _nalt = 0;		// 現在高度
            _nroll = 0;		// Roll（現在方位角）　
            _npitch = 0;	// Pitch
            _nyaw = 0;		// Yaw
            _speed = 0;
			_no = 0;

            _IMUDataRoll = 0;
            _IMUDataPitch = 0;
            _IMUDataYaw = 0;
            _GyroDataRollRate = 0;
            _GyroDataPitchRate =0;
            _GyroDataYawRate =0;
            _AccDataX =0;
            _AccDataY =0;
            _AccDataZ =0;
            _MagDataX =0;
            _MagDataY =0;
            _MagDataZ =0;


            WayPoint_SetIndexEdit=0;
            WayPoint_KeepIndex1Edit=0;
            WayPoint_HeightEdit=0;
            WayPoint_YawEdit=0;
            WayPoint_HengxiangSuduEdit=0;
            WayPoint_ZongshangSuduEdit=0;
            WayPoint_ZongxiaSuduEdit=0;
            WayPoint_HoverTimerEdit=0;
            WayPoint_KeepIndex2Edit=0;

        }

        #region プロパティ
        public double Lon
        {
            get { return this._nlon; }
            set { this._nlon = value; }
        }
        public double Lat
        {
            get { return this._nlat; }
            set { this._nlat = value; }
        }
        public double Alt
        {
            get { return this._nalt; }
            set { this._nalt = value; }
        }
        public double Roll
        {
            get { return this._nroll; }
            set { this._nroll = value; }
        }
        public double Pitch
        {
            get { return this._npitch; }
            set { this._npitch = value; }
        }
        public double Yaw
        {
            get { return this._nyaw; }
            set { this._nyaw = value; }
        }
        public double Speed
        {
            get { return this._speed; }
            set { this._speed = value; }
        }
		public int No
		{
			get { return this._no; }
			set { this._no = value; }
		}
/// <summary>
/// /新添加的数据部分函数
/// </summary>
        public double mm_IMUDataRoll
        {
            get { return this._IMUDataRoll; }
            set { this._IMUDataRoll = value; }
        }
        public double mm_IMUDataPitch
        {
            get { return this._IMUDataPitch; }
            set { this._IMUDataPitch = value; }
        }
        public double mm_IMUDataYaw
        {
            get { return this._IMUDataYaw; }
            set { this._IMUDataYaw = value; }
        }
        public double mm_GyroDataRollRate
        {
            get { return this._GyroDataRollRate; }
            set { this._GyroDataRollRate = value; }
        }
        public double mm_GyroDataPitchRate
        {
            get { return this._GyroDataPitchRate; }
            set { this._GyroDataPitchRate = value; }
        }
        public double mm_GyroDataYawRate
        {
            get { return this._GyroDataYawRate; }
            set { this._GyroDataYawRate = value; }
        }

        public double mm_AccDataX
        {
            get { return this._AccDataX; }
            set { this._AccDataX = value; }
        }
        public double mm_AccDataY
        {
            get { return this._AccDataY; }
            set { this._AccDataY = value; }
        }
        public double mm_AccDataZ
        {
            get { return this._AccDataZ; }
            set { this._AccDataZ = value; }
        }
        public double mm_MagDataX
        {
            get { return this._MagDataX; }
            set { this._MagDataX = value; }
        }
        public double mm_MagDataY
        {
            get { return this._MagDataY; }
            set { this._MagDataY = value; }
        }
        public double mm_MagDataZ
        {
            get { return this._MagDataZ; }
            set { this._MagDataZ = value; }
        }
///////////////////////////////航拍新添加的地方////////////////////////////////


        public double mmWayPoint_SetIndexEdit
        {
            get { return this.WayPoint_SetIndexEdit; }
            set { this.WayPoint_SetIndexEdit = value; }
        }
        public double mmWayPoint_KeepIndex1Edit
        {
            get { return this.WayPoint_KeepIndex1Edit; }
            set { this.WayPoint_KeepIndex1Edit = value; }
        }
        public double mmWayPoint_HeightEdit
        {
            get { return this.WayPoint_HeightEdit; }
            set { this.WayPoint_HeightEdit = value; }
        }
        public double mmWayPoint_YawEdit
        {
            get { return this.WayPoint_YawEdit; }
            set { this.WayPoint_YawEdit = value; }
        }
        public double mmWayPoint_HengxiangSuduEdit
        {
            get { return this.WayPoint_HengxiangSuduEdit; }
            set { this.WayPoint_HengxiangSuduEdit = value; }
        }
        public double mmWayPoint_ZongshangSuduEdit
        {
            get { return this.WayPoint_ZongshangSuduEdit; }
            set { this.WayPoint_ZongshangSuduEdit = value; }
        }
        public double mmWayPoint_ZongxiaSuduEdit
        {
            get { return this.WayPoint_ZongxiaSuduEdit; }
            set { this.WayPoint_ZongxiaSuduEdit = value; }
        }
        public double mmWayPoint_HoverTimerEdit
        {
            get { return this.WayPoint_HoverTimerEdit; }
            set { this.WayPoint_HoverTimerEdit = value; }
        }
        public double mmWayPoint_KeepIndex2Edit
        {
            get { return this.WayPoint_KeepIndex2Edit; }
            set { this.WayPoint_KeepIndex2Edit = value; }
        }
        #endregion

    }
}
