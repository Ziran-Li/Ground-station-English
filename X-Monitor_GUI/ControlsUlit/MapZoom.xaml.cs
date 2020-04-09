using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;


namespace ControlsUnit
{
    /// <summary>
    /// MapZoom.xaml の相互作用ロジック
    /// </summary>
    public partial class MapZoom : UserControl
    {
        public MapZoom()
        {
            InitializeComponent();
        }

        #region イベント
        /// <summary>
        /// 値変更時に呼ばれるイベントです。
        /// </summary>
        public static readonly RoutedEvent ValueChangedEvent =
            EventManager.RegisterRoutedEvent(
                "ValueChanged", RoutingStrategy.Bubble,
                typeof(RoutedPropertyChangedEventHandler<double>),
                typeof(MapZoom));

        /// <summary>
        /// 値変更時に呼ばれるイベントです。
        /// </summary>
        public event RoutedPropertyChangedEventHandler<double> ValueChanged
        {
            add { AddHandler(ValueChangedEvent, value); }
            remove { RemoveHandler(ValueChangedEvent, value); }
        }
        #endregion

        #region プロパティ
        /// <summary>
        /// 数値データの依存プロパティです。
        /// </summary>
        public static readonly DependencyProperty ValueProperty =
            DependencyProperty.Register(
                "Value", typeof(double), typeof(MapZoom),
                new FrameworkPropertyMetadata(
                    default(double),
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    OnValueChanged, OnCoerceValue));
        /// <summary>
        /// 最小値を扱う依存プロパティです。
        /// </summary>
        public static readonly DependencyProperty MinimumProperty =
            DependencyProperty.Register(
                "Minimum", typeof(double), typeof(MapZoom),
                new FrameworkPropertyMetadata(
                    (double)0, OnMinimumChanged, OnCoerceMinimum));

        /// <summary>
        /// 最大値を扱う依存プロパティです。
        /// </summary>
        public static readonly DependencyProperty MaximumProperty =
            DependencyProperty.Register(
                "Maximum", typeof(double), typeof(MapZoom),
                new FrameworkPropertyMetadata(
                    (double)100, OnMaximumChanged, OnCoerceMaximum));


        /// <summary>
        /// Slider Zoom
        /// </summary>         
        [Category("ControlsUnit")]
        [Description("map zoom value")]
        [Browsable(true)]
        [Bindable(true)]
        public double Value
        {
            get { return (double)GetValue(ValueProperty); }
            set { SetValue(ValueProperty, value); }
        }

        /// <summary>
        /// 設定できる最小値を取得または設定します。
        /// </summary>
        [Browsable(true)]
        [Bindable(true)]
        public double Minimum
        {
            get { return (double)GetValue(MinimumProperty); }
            set { SetValue(MinimumProperty, value); }
        }

        /// <summary>
        /// 設定できる最大値を取得または設定します。
        /// </summary>
        [Browsable(true)]
        [Bindable(true)]
        public double Maximum
        {
            get { return (double)GetValue(MaximumProperty); }
            set { SetValue(MaximumProperty, value); }
        }


        public static readonly DependencyProperty ZBackgroundProperty = DependencyProperty.Register(
      "Background", typeof(SolidColorBrush), typeof(MapZoom), new FrameworkPropertyMetadata(OnBackgroundChanged));
        /// <summary>
        /// 
        /// </summary>
        [Category("ControlsUnit")]
        [Description("Zoom Background")]
        [Browsable(true)]
        [Bindable(true)]
        public SolidColorBrush ZoomBackground
        {
            get { return (SolidColorBrush)GetValue(ZBackgroundProperty); }
            set { SetValue(ZBackgroundProperty, value); }
        }
        #endregion

        #region スライダ　MAX-MIN値
        [Category("ControlsUnit")]
        [Description("Zoom Minimum")]
        public double ZoomMinimum
        {
            get { return (double)ZoomSlider.Minimum; }
            set
            {
                ZoomSlider.Minimum = value;
            }
        }
        [Category("ControlsUnit")]
        [Description("Zoom Maximum")]
        public double ZoomMaximum
        {
            get { return (double)ZoomSlider.Maximum; }
            set
            {
                ZoomSlider.Maximum = value;
            }
        }
        #endregion
        #region プロパティメソッド
        /// <summary>
        /// 値の変更時に呼ばれます。
        /// </summary>
        private static void OnValueChanged(DependencyObject d,
                                           DependencyPropertyChangedEventArgs e)
        {
            MapZoom control = (MapZoom)d;
            control.ZoomSlider.Value = (double)e.NewValue;

            RoutedPropertyChangedEventArgs<double> args = new RoutedPropertyChangedEventArgs<double>(
                (double)e.OldValue, (double)e.NewValue, ValueChangedEvent);
            control.OnValueChanged(args);
        }
        protected virtual void OnValueChanged(RoutedPropertyChangedEventArgs<double> args)
        {
            RaiseEvent(args);
        }

        /// <summary>
        /// 値を[MinValue, MaxValue]の間に納めます。
        /// </summary>
        private static object OnCoerceValue(DependencyObject d,
                                            object baseValue)
        {
            var self = (MapZoom)d;
            var value = (double)baseValue;

            return Math.Min(self.Maximum, Math.Max(value, self.Minimum));
        }
        
        private static void OnBackgroundChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            var element = (MapZoom)sender;
            element.InvalidateVisual();
        }

        protected override void OnRender(DrawingContext drawingContext)
        {
            SolidColorBrush brush = ZoomBackground;
            if (brush != null)
            {
             //   slider.Opacity = brush.Opacity;
              //  ZoomBtn.Opacity = brush.Opacity;
            //    PlusBtn.Opacity = brush.Opacity;
            //    MinusBtn.Opacity = brush.Opacity;
                slider.Background = new SolidColorBrush( brush.Color );
                ZoomBtn.Background = new SolidColorBrush(brush.Color);
             //   PlusBtn.Background = new SolidColorBrush(brush.Color);
             //   MinusBtn.Background = new SolidColorBrush( brush.Color );
            } 
        }
        
        /// <summary>
        /// 最小値が変わったときに呼ばれます。
        /// </summary>
        private static void OnMinimumChanged(DependencyObject d,
                                            DependencyPropertyChangedEventArgs e)
        {
            d.CoerceValue(MaximumProperty);
            d.CoerceValue(ValueProperty);
        }

        /// <summary>
        /// 必要なら設定された最小値を修正します。
        /// </summary>
        private static object OnCoerceMinimum(DependencyObject d,
                                              object baseValue)
        {
            var self = (MapZoom)d;
            var value = (double)baseValue;

            // 当然、最大値よりも小さくないとダメです。
            return Math.Min(value, self.Maximum);
        }

        /// <summary>
        /// 最大値が変わったときに呼ばれます。
        /// </summary>
        private static void OnMaximumChanged(DependencyObject d,
                                             DependencyPropertyChangedEventArgs e)
        {
            d.CoerceValue(MinimumProperty);
            d.CoerceValue(ValueProperty);
        }

        /// <summary>
        /// 必要なら設定された最大値を修正します。
        /// </summary>
        private static object OnCoerceMaximum(DependencyObject d,
                                              object baseValue)
        {
            var self = (MapZoom)d;
            var value = (double)baseValue;

            // 当然、最小値よりも大きくないとダメです。
            return Math.Max(value, self.Minimum);
        }
        #endregion

        private void ZoomSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            SetValue(ValueProperty, (double)e.NewValue);
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            ZoomSlider.Value = ((int)ZoomSlider.Value) + 1;
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            ZoomSlider.Value = ((int)(ZoomSlider.Value + 0.99)) - 1;
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            
            if (slider.Visibility == Visibility.Visible)
            {
                slider.Visibility = Visibility.Hidden;
            }
            else
            {
                slider.Visibility = Visibility.Visible;
            }
        }

    }
}
