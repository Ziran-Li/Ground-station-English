using System.Collections.Generic;
using GMap.NET;                         //GMap.NET Core

namespace MulitWaypoint
{
    public class MulitWaypointData
    {
        public static readonly MulitWaypointData Empty = new MulitWaypointData();

        private int _mcnt;
        private PointLatLng _Origin;
        private List<int> _mid = new List<int>();
        private List<double> _x = new List<double>();
        private List<double> _y = new List<double>();
        private List<PointLatLng> _multi = new List<PointLatLng>();
        private List<double> _mpsi = new List<double>();
        private List<double> _malt = new List<double>();
        private List<double> _mspeed = new List<double>();
        private List<double> _maccuracy = new List<double>();
        private List<double> _mwait = new List<double>();
        private List<int> _mfunc_id = new List<int>();

        #region プロパティ
        public int mCount
        {
            get { return this._mcnt; }
            set { this._mcnt = value; }
        }
        public PointLatLng Origin
        {
            get { return this._Origin; }
            set { this._Origin = value; }
        }
        public List<int> mid
        {
            get { return this._mid; }
            set { this._mid = value; }
        }
        public List<double> x
        {
            get { return this._x; }
            set { this._x = value; }
        }
        public List<double> y
        {
            get { return this._y; }
            set { this._y = value; }
        }
        public List<PointLatLng> multi
        {
            get { return this._multi; }
            set { this._multi = value; }
        }

        public List<double> mpsi
        {
            get { return this._mpsi; }
            set { this._mpsi = value; }
        }
        public List<double> malt
        {
            get { return this._malt; }
            set { this._malt = value; }
        }

        public List<double> mspeed
        {
            get { return this._mspeed; }
            set { this._mspeed = value; }
        }
        public List<double> maccuracy
        {
            get { return this._maccuracy; }
            set { this._maccuracy = value; }
        }
        public List<double> mwait
        {
            get { return this._mwait; }
            set { this._mwait = value; }
        }
        public List<int> mfunc_id
        {
            get { return this._mfunc_id; }
            set { this._mfunc_id = value; }
        }
        #endregion

    }
}
