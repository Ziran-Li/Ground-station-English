//
// 時刻クラス
//


namespace SysTime
{
    public class SystemTime
    {
        private int _Year;
        private int _Month;
        private int _Day;
        private int _Hour;
        private int _Minute;
        private int _Second;
        private int _Milliseconds;
        private double _time;      //経過秒（Minute+Second+Milliseconds）
        private string _Gmt;

        public SystemTime()
        {
            _Year = 0;
            _Month = 0;
            _Day = 0;
            _Hour = 0;
            _Minute = 0;
            _Second = 0;
            _Milliseconds = 0;
            _time = 0;
            _Gmt = "";
        }

        #region プロパティ
        public int Year
        {
            get { return this._Year; }
            set { this._Year = value; }
        }
        public int Month
        {
            get { return this._Month; }
            set { this._Month = value; }
        }
        public int Day
        {
            get { return this._Day; }
            set { this._Day = value; }
        }
        public int Hour
        {
            get { return this._Hour; }
            set { this._Hour = value; }
        }
        public int Minute
        {
            get { return this._Minute; }
            set
            {
                this._Minute = value;
                this._time = TimeCalc();
            }
        }
        public int Second
        {
            get { return this._Second; }
            set
            {
                this._Second = value;
                this._time = TimeCalc();
            }
        }
        public int Milliseconds
        {
            get { return this._Milliseconds; }
            set
            {
                this._Milliseconds = value;
                this._time = TimeCalc();
            }
        }
        public double Time
        {
            get { return this._time; }
            set { this._time = value; }
        }
        public string GMT
        {
            get { return this._Gmt; }
            set { this._Gmt = value; }
        }
        #endregion

        public double TimeCalc()
        {
            double time = 0;
            time += _Minute * 60;
            time += _Second;
            time += _Milliseconds / 1000;
            _Gmt = GMTCalc();
            return time;
        }
        public string GMTCalc()
        {
            string time = "";

            time = _Year.ToString() + ":" + _Month.ToString() + ":" + _Day.ToString() + ":" + _Hour.ToString() + ":" + _Minute.ToString() + ":" + _Second.ToString() + ":" + _Milliseconds.ToString();

            return time;
        }
    }
}
