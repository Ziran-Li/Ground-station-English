using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;

namespace ControlsUnit
{
    /// <summary>
    /// RCrecView.xaml の相互作用ロジック
    /// </summary>
    public partial class RCrecView : UserControl
    {
		private double _alertValue = 50.0;

        public int rcmin = 0;
        public int rcmax = 0;

        public RCrecView()
        {
            InitializeComponent();
        }


        /// <summary>
        /// RCレシーバ接続状態のヘルス値表示　MAX-MIN値
        /// </summary>    
        [Category("ControlsUnit")]
        [Description("UAV Link MinValue")]
        public int ValueMin
        {
            get { return (int)rcmin; }
            set { rcmin = value; }
        }
        [Category("ControlsUnit")]
        [Description("UAV Link MaxValue")]
        public int ValueMax
        {
            get { return (int)rcmax; }
            set { rcmax = value; }
        }

        #region イベント

        #endregion

        #region プロパティ
        /// <summary>
        /// 数値データの依存プロパティです。
        /// </summary>
        public static readonly DependencyProperty RCrecValueProperty =
            DependencyProperty.Register(
                "RCrecValue", typeof(double), typeof(RCrecView),
                new FrameworkPropertyMetadata(
                    default(double),
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    OnRCrecValueChanged));
        /// <summary>
        ///
        /// </summary>         
        [Category("ControlsUnit")]
        [Description("UAV RCrec Value")]
        [Browsable(true)]
        [Bindable(true)]
        public double RCrecValue
        {
            get { return (double)GetValue(RCrecValueProperty); }
            set 
			{
				SetValue(RCrecValueProperty, value);
				//var _val = (int)value;
				//if (_val <= _alertValue)
				//{
				//	ControlsUnitBorder.BorderBrush = System.Windows.Media.Brushes.Red;
				//	ControlsUnitBorder.BorderThickness = new System.Windows.Thickness(3);
				//}
				//else
				//{
				//	ControlsUnitBorder.BorderBrush = System.Windows.Media.Brushes.White;
				//	ControlsUnitBorder.BorderThickness = new System.Windows.Thickness(1);
				//}
			}
        }
		[Category("ControlsUnit")]
		[Description("UAV RCrec AlertValue")]
		public double ValueAlert
		{
			get { return (double)_alertValue; }
			set { _alertValue = value; }
		}
        #endregion

        #region プロパティメソッド
        /// <summary>
        /// 値の変更時に呼ばれます。
        /// </summary>
        private static void OnRCrecValueChanged(DependencyObject d,
                                           DependencyPropertyChangedEventArgs e)
        {
            RCrecView control = (RCrecView)d;
            control.RCrecVal.Content = Math.Round((double)Math.Min(Math.Max((double)e.NewValue, control.rcmin), control.rcmax),
                                                    0, 
                                                    MidpointRounding.AwayFromZero).ToString();
        }

        #endregion

    }
}
