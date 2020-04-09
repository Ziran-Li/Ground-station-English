using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;

namespace ControlsUnit
{
    /// <summary>
    /// GPSView.xaml の相互作用ロジック
    /// </summary>
    public partial class GPSView : UserControl
    {
		private double _alertValue = 7.0;

        public int gpsmin = 0;
        public int gpsmax = 0;

        public GPSView()
        {
            InitializeComponent();
        }

        public static readonly DependencyProperty GPSValueProperty = DependencyProperty.Register(
            "GPSValue", typeof(int), typeof(GPSView), new UIPropertyMetadata(
            1,
            (d, e) =>
        {
            (d as GPSView).GpsVal.Content = ((int)Math.Min(Math.Max((int)e.NewValue, (d as GPSView).gpsmin), (d as GPSView).gpsmax)).ToString() ; 
        }));

        /// <summary>
        /// 
        /// </summary>         
        [Category("ControlsUnit")]
        [Description("UAV GPS Value")]
        public int GPSValue
        {
            get { return (int)this.GetValue(GPSValueProperty); }
            set { 
				this.SetValue(GPSValueProperty, (int)value);
				//var _val = (int)value;
				//if (_val <= _alertValue)
				//{
				//	System.Windows.Media.SolidColorBrush _brush = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(192, 0, 0));
				//	_brush.Opacity = 0.7;
				//	ControlsUnitBorder.Background = _brush;
				//}
				//else
				//{
				//	System.Windows.Media.SolidColorBrush _brush = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(31, 26, 80));
				//	_brush.Opacity = 0.7;
				//	ControlsUnitBorder.Background = _brush;
				//}
			}
        }
        public void RTK_Hide_Function()
        {
            RTK_Value.Visibility = System.Windows.Visibility.Collapsed;
        }
        public void RTK_Show_Function()
        {
            RTK_Value.Visibility = Visibility;
        }
        /// <summary>
        /// GPS表示　MAX-MIN値
        /// </summary>    
        [Category("ControlsUnit")]
        [Description("UAV GPS MinValue")]
        public int ValueMin
        {
            get { return (int)gpsmin; }
            set { gpsmin = value; }
        }
        [Category("ControlsUnit")]
        [Description("UAV GPS MaxValue")]
        public int ValueMax
        {
            get { return (int)gpsmax; }
            set { gpsmax = value; }
        }
		[Category("ControlsUnit")]
		[Description("UAV GPS AlertValue")]
		public double ValueAlert
		{
			get { return (double)_alertValue; }
			set { _alertValue = value; }
		}
    }
}
