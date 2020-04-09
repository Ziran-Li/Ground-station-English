using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;

namespace ControlsUnit
{
    /// <summary>
    /// BatteryView.xaml の相互作用ロジック
    /// </summary>
    public partial class BatteryView : UserControl
    {
		private double _alertValue = 3.0;

        public BatteryView()
        {
            InitializeComponent();
        }

        /// <summary>
        /// バッテリー残量表示
        /// </summary>      
        public static readonly DependencyProperty BatteryValueProperty = DependencyProperty.Register(
            "BatteryValue", typeof(double), typeof(BatteryView), new UIPropertyMetadata(
            1.0d,
            (d, e) =>
            {
                // MAX-Minの判定
                (d as BatteryView).ProBar.Value = (double)Math.Min(Math.Max((double)e.NewValue, (d as BatteryView).ProBar.Minimum), (d as BatteryView).ProBar.Maximum);
                // 小数点第3位を四捨五入
				(d as BatteryView).BatVal.Content = ((double)e.NewValue).ToString("0.00") + "V";

            }));
        [Category("ControlsUnit")]
        [Description("UAV Battery Value")]
        public double BatteryValue
        {
            get { return (double)this.GetValue(BatteryValueProperty); }
            set 
			{ 
				this.SetValue(BatteryValueProperty, (double)value);
				if (ProBar.Value <= _alertValue)
				{
					ProBar.Foreground = System.Windows.Media.Brushes.Red;
					//ControlsUnitBorder.BorderBrush = System.Windows.Media.Brushes.Red;
					//ControlsUnitBorder.BorderThickness = new System.Windows.Thickness(3);
					//System.Windows.Media.SolidColorBrush _brush = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(192, 0, 0));
					//_brush.Opacity = 0.7;
					//ControlsUnitBorder.Background = _brush;
				}
				else
				{
					ProBar.Foreground = System.Windows.Media.Brushes.Green;//new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(31, 26, 80));
					//ControlsUnitBorder.BorderBrush = System.Windows.Media.Brushes.White;
					//ControlsUnitBorder.BorderThickness = new System.Windows.Thickness(1);
					//System.Windows.Media.SolidColorBrush _brush = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(31, 26, 80));
					//_brush.Opacity = 0.7;
					//ControlsUnitBorder.Background = _brush;
				}
			}
        }


        /// <summary>
        /// バッテリー残量表示　MAX-MIN値
        /// </summary>    
        [Category("ControlsUnit")]
        [Description("UAV Battery MinValue")]
        public double ValueMin
        {
            get { return (double)ProBar.Minimum; }
            set { ProBar.Minimum = value; }
        }
        [Category("ControlsUnit")]
        [Description("UAV Battery MaxValue")]
        public double ValueMax
        {
            get { return (double)ProBar.Maximum; }
            set { ProBar.Maximum=value; }
        }
		[Category("ControlsUnit")]
		[Description("UAV Battery AlertValue")]
		public double ValueAlert
		{
			get { return (double)_alertValue; }
			set { _alertValue = value; }
		}

    }
}
