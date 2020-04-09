using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;

namespace ControlsUnit
{
    /// <summary>
    /// TrimmingSlide.xaml の相互作用ロジック
    /// </summary>
    public partial class TrimmingSlide : UserControl
    {
		private System.Windows.Controls.Primitives.ScrollEventType _scrollEventType;
		private bool _playSlider;
        public TrimmingSlide()
        {
            InitializeComponent();
			_scrollEventType = System.Windows.Controls.Primitives.ScrollEventType.ThumbPosition;
			_playSlider = false;
        }

        #region 内部イベント
        //再生用スライド
		private void _PlaySlider_Scroll(object sender, System.Windows.Controls.Primitives.ScrollEventArgs e)
		{
			_playSlider = true;
			_scrollEventType = e.ScrollEventType;
			SetValue(PlayValueProperty, (double)e.NewValue);
			//System.Diagnostics.Debug.WriteLine("_PlaySlider_Scroll：" + e.NewValue + "/" + e.ScrollEventType);
		}
        // トリミングスライド上限
        private void _RangeSlider_HigherValueChanged(object sender, RoutedEventArgs e)
        {
            SetValue(HigherValueProperty, (double)_RangeSlider.HigherValue);
        }
        // トリミングスライド下限
        private void _RangeSlider_LowerValueChanged(object sender, RoutedEventArgs e)
        {
            SetValue(LowerValueProperty, (double)_RangeSlider.LowerValue);
        }
        #endregion

        #region 外部イベント

        #region トリミング用　LowerValue
        public static readonly RoutedEvent LowerValueChangedEvent =
            EventManager.RegisterRoutedEvent(
                "LowerValueChanged", RoutingStrategy.Bubble,
                typeof(RoutedPropertyChangedEventHandler<double>),
                typeof(TrimmingSlide));
        public event RoutedPropertyChangedEventHandler<double> LowerValueChanged
        {
            add { AddHandler(LowerValueChangedEvent, value); }
            remove { RemoveHandler(LowerValueChangedEvent, value); }
        }

        #endregion

        #region トリミング用　HigherValue
        public static readonly RoutedEvent HigherValueChangedEvent =
            EventManager.RegisterRoutedEvent(
                "HigherValueChanged", RoutingStrategy.Bubble,
                typeof(RoutedPropertyChangedEventHandler<double>),
                typeof(TrimmingSlide));

        public event RoutedPropertyChangedEventHandler<double> HigherValueChanged
        {
            add { AddHandler(HigherValueChangedEvent, value); }
            remove { RemoveHandler(HigherValueChangedEvent, value); }
        }
        #endregion

        #region 再生用　PlayValue
		public static readonly RoutedEvent ValueChangedEvent =
													EventManager.RegisterRoutedEvent(
													"ValueChanged", RoutingStrategy.Bubble,
													typeof(RoutedPropertyChangedEventHandler<double>),
													typeof(TrimmingSlide));

		/// <summary>
		/// 値変更時に呼ばれるイベントです。
		/// </summary>
		public event RoutedPropertyChangedEventHandler<double> ValueChanged
		{
			add { AddHandler(ValueChangedEvent, value); }
			remove { RemoveHandler(ValueChangedEvent, value); }
		}
        #endregion
        #endregion

        #region プロパティ
        #region トリミング用　LowerValue
        public static readonly DependencyProperty LowerValueProperty =
            DependencyProperty.Register(
            "LowerValue", typeof(double), typeof(TrimmingSlide),
            new FrameworkPropertyMetadata(
            default(double),
            FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
            OnLowerValueChanged));
     
        [Category("ControlsUnit")]
        [Description("Trimming LowerValue")]
        [Browsable(true)]
        [Bindable(true)]
        public double LowerValue
        {
            get { return (double)GetValue(LowerValueProperty); }
            set { SetValue(LowerValueProperty, value); }
        }
        #endregion
        
        #region トリミング用　HigherValue
        public static readonly DependencyProperty HigherValueProperty =
            DependencyProperty.Register(
            "HigherValue", typeof(double), typeof(TrimmingSlide),
            new FrameworkPropertyMetadata(
            default(double),
            FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
            OnHigherValueChanged));

        [Category("ControlsUnit")]
        [Description("Trimming HigherValue")]
        [Browsable(true)]
        [Bindable(true)]
        public double HigherValue
        {
            get { return (double)GetValue(HigherValueProperty); }
            set { SetValue(HigherValueProperty, value); }
        }
        #endregion
    
        #region 再生用　PlayValue
		public static readonly DependencyProperty PlayValueProperty =
														DependencyProperty.Register(
														"Value", typeof(double), typeof(TrimmingSlide),
														new FrameworkPropertyMetadata(
														default(double),
														FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
														OnPlayValueChanged));

		[Category("ControlsUnit")]
		[Description("Trimming PlayValue")]
		[Browsable(true)]
		[Bindable(true)]
		public double Value
		{
			get { return (double)GetValue(PlayValueProperty); }
			set
			{
				SetValue(PlayValueProperty, value);
			}
		}

		public double PlayValue
		{
			get { return (double)_PlaySlider.Value; }
			set {
				if (_scrollEventType == System.Windows.Controls.Primitives.ScrollEventType.ThumbPosition)
				{
					_PlaySlider.Value = value;
					double _time = _PlaySlider.Value;
					int m = (int)(_time / 60);
					int s = (int)(_time - (m * 60));
					int ms = (int)((_time - (int)(_time)) * 1000);
					_Time.Content = m.ToString() + ":" + s.ToString() + "." + ms.ToString();
				}
				_scrollEventType = System.Windows.Controls.Primitives.ScrollEventType.ThumbPosition;
			}
		}
        #endregion

        #region スライダ　MAX-MIN値
        [Category("ControlsUnit")]
        [Description("TrimmingSlide MinValue")]
        public double Minimum
        {
            get { return (double)_PlaySlider.Minimum; }
            set { _PlaySlider.Minimum = value;        }
        }
        [Category("ControlsUnit")]
        [Description("TrimmingSlide MaxValue")]
        public double Maximum
        {
            get { return (double)_PlaySlider.Maximum; }
            set {
                 _PlaySlider.Maximum = value;
                double _time = _PlaySlider.Maximum;
                int m = (int)(_time / 60);
                int s = (int)(_time - (m * 60));
				int ms = (int)((_time - (int)(_time)) * 1000);
                _TotalTime.Content = m.ToString() + ":" + s.ToString() + "." + ms.ToString();
            }
        }
		[Category("ControlsUnit")]
		[Description("TrimmingSlide MinValue")]
		public double RangeMinimum
		{
			get { return (double)_RangeSlider.Minimum; }
			set { _RangeSlider.Minimum = value;		}
		}
		[Category("ControlsUnit")]
		[Description("TrimmingSlide MaxValue")]
		public double RangeMaximum
		{
			get { return (double)_RangeSlider.Maximum; }
			set
			{
				_RangeSlider.Maximum = value;
				System.Diagnostics.Debug.WriteLine("Maximum: " + _PlaySlider.Maximum + "/" + _RangeSlider.Maximum);
			}
		}
        #endregion

		#region ScrollEventType
		[Category("ControlsUnit")]
		[Description("TrimmingSlide ScrollEventType")]
		public System.Windows.Controls.Primitives.ScrollEventType ScrollType
		{
			get { return _scrollEventType; }
			set { _scrollEventType = value; }
		}
		#endregion

		public bool PlaySlider
		{
			get { return _playSlider; }
			set { _playSlider = value; }
		}
		public System.Windows.Media.Brush TimeForeground
		{
			get { return _Time.Foreground; }
			set { 
				_Time.Foreground = value;
				_TotalTime.Foreground = _Time.Foreground;
			}
		}
        #endregion

        #region プロパティメソッド
        #region トリミング用　LowerValue
        private static void OnLowerValueChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            TrimmingSlide control = (TrimmingSlide)d;
            control._RangeSlider.LowerValue = (double)e.NewValue;

            RoutedPropertyChangedEventArgs<double> args = new RoutedPropertyChangedEventArgs<double>(
                                                                (double)e.OldValue, (double)e.NewValue, LowerValueChangedEvent);
            control.OnLowerValueChanged(args);
        }
        protected virtual void OnLowerValueChanged(RoutedPropertyChangedEventArgs<double> args)
        {
            RaiseEvent(args);
        }
        #endregion

        #region トリミング用　HigherValue
        private static void OnHigherValueChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            TrimmingSlide control = (TrimmingSlide)d;
            control._RangeSlider.HigherValue = (double)e.NewValue;

            RoutedPropertyChangedEventArgs<double> args = new RoutedPropertyChangedEventArgs<double>(
                                                    (double)e.OldValue, (double)e.NewValue, HigherValueChangedEvent);
            control.OnHigherValueChanged(args);
        }
        protected virtual void OnHigherValueChanged(RoutedPropertyChangedEventArgs<double> args)
        {
            RaiseEvent(args);
        }
        #endregion

        #region 再生用　PlayValue
		private static void OnPlayValueChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			TrimmingSlide control = (TrimmingSlide)d;
			control._PlaySlider.Value = (double)e.NewValue;
			RoutedPropertyChangedEventArgs<double> args = new RoutedPropertyChangedEventArgs<double>(
						(double)e.OldValue, (double)e.NewValue, ValueChangedEvent);
			control.OnPlayValueChanged(args);
		}
		protected virtual void OnPlayValueChanged(RoutedPropertyChangedEventArgs<double> args)
		{
			RaiseEvent(args);
		}
        #endregion

        #endregion
    }
}
