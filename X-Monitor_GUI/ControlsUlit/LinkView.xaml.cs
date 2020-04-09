using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace ControlsUnit
{
    /// <summary>
    /// LinkView.xaml の相互作用ロジック
    /// </summary>
    public partial class LinkView : UserControl
    {
		private double _alertValue = 50.0;

        public int linkmin = 0;
        public int linkmax = 0;
        public int linkhealth = 0;

        public LinkView()
        {
            InitializeComponent();
        }

        public static readonly DependencyProperty LinkValueProperty = DependencyProperty.Register(
         "LinkValue", typeof(int), typeof(LinkView), new UIPropertyMetadata(
            1,
            (d, e) =>
            {
                (d as LinkView).linkhealth = (int)Math.Min(Math.Max((int)e.NewValue, (d as LinkView).linkmin), (d as LinkView).linkmax);
                if ((d as LinkView).linkhealth>0)
                {
                    (d as LinkView).LinkSta.Content = "Connected";
                    (d as LinkView).LinkSta.Foreground = new SolidColorBrush(Color.FromArgb(0xFF, 0xFF, 0xFF, 0xFF));
                    (d as LinkView).Linkimg.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/connect.png")));
                }
                else
                {
                    (d as LinkView).LinkSta.Content = "Disconnected";
                    (d as LinkView).LinkSta.Foreground = new SolidColorBrush(Color.FromArgb(0xFF, 201, 30, 16));
                    (d as LinkView).Linkimg.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/disconnect.png")));
                }

                (d as LinkView).LinkVal.Content = (d as LinkView).linkhealth.ToString();
            }));

        /// <summary>
        /// 接続状態のヘルス値を表示
        /// </summary>         
        [Category("ControlsUnit")]
        [Description("UAV Link Value")]
        public int LinkValue
        {
            get { return (int)this.GetValue(LinkValueProperty); }
            set { this.SetValue(LinkValueProperty, (int)value);
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

        /// <summary>
        /// 接続状態のヘルス値表示　MAX-MIN値
        /// </summary>    
        [Category("ControlsUnit")]
        [Description("UAV Link MinValue")]
        public int ValueMin
        {
            get { return (int)linkmin; }
            set { linkmin = value; }
        }
        [Category("ControlsUnit")]
        [Description("UAV Link MaxValue")]
        public int ValueMax
        {
            get { return (int)linkmax; }
            set { linkmax = value; }
        }
		[Category("ControlsUnit")]
		[Description("UAV Link AlertValue")]
		public double ValueAlert
		{
			get { return (double)_alertValue; }
			set { _alertValue = value; }
		}
    }
}
