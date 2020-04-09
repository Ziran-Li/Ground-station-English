using System.ComponentModel;
using System;

namespace FlightPlanning
{
    /// <summary>
    /// Rootパラメータクラス
    /// </summary>
	[Serializable()]
	public class Path
    {
        [Category("1.Waypoint")]
        public int Waypoint1 { get; set; }              //前のウェイポイントID
        [Category("1.Waypoint")]
        public int Waypoint2 { get; set; }              //次のウェイポイントID
        [Category("2.最大速度")]
        public double Speed { get; set; }               //パス上での最大速度 [m/s]
        [Category("3.軌道の形状")]
        public string Shape { get; set; }               //軌道の形状
    }
}
