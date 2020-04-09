using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Diagnostics;

namespace ControlsUnit
{
    /// <summary>
    /// RotateAzimuthal.xaml の相互作用ロジック
    /// </summary>
    public partial class RotateAzimuthal : UserControl
    {
        public RotateAzimuthal()
        {
            InitializeComponent();
        }

        private Point FCenter;           // 回転の中心座標（コントロールの左上隅を原点とする座標値）
		private bool FIsRotating;        // 回転操作中は true
		private double FMouseDownAngle;  // マウスダウン時の角度
		private Vector FMouseDownVector; // 同、方向

		//public static readonly DependencyProperty AngleProperty;

        #region イベント
        /// <summary>
        /// 値変更時に呼ばれるイベントです。
        /// </summary>
        public static readonly RoutedEvent ValueChangedEvent =
            EventManager.RegisterRoutedEvent(
                "ValueChanged", RoutingStrategy.Bubble,
                typeof(RoutedPropertyChangedEventHandler<double>),
                typeof(RotateAzimuthal));

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
        public static readonly DependencyProperty AngleProperty =
            DependencyProperty.Register(
                "Angle", typeof(double), typeof( RotateAzimuthal),
                new FrameworkPropertyMetadata(
                    default(double),
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    OnValueChanged));
        public double Angle
        {
            get { return (double)this.GetValue(AngleProperty); }
            set { this.SetValue(AngleProperty, value); }
        }
        #endregion

        #region プロパティメソッド
        /// <summary>
        /// 値の変更時に呼ばれます。
        /// </summary>
        private static void OnValueChanged(DependencyObject d,
                                           DependencyPropertyChangedEventArgs e)
        {
             RotateAzimuthal control = ( RotateAzimuthal)d;
             control.Angle = (double)e.NewValue % 360;
             // 角度は 0 ～ 360 の範囲内におさめる
             if (control.Angle < 0){
                 control.Angle += 360;
             }
                 
            RoutedPropertyChangedEventArgs<double> args = new RoutedPropertyChangedEventArgs<double>( 
                (double)e.OldValue, (double)e.NewValue, ValueChangedEvent);
            control.OnValueChanged(args);
        }
        protected virtual void OnValueChanged(RoutedPropertyChangedEventArgs<double> args)
        {
            RaiseEvent(args);
        }
        #endregion

		//---------------------------------------------------------------------------------------------
        static RotateAzimuthal()
		{
			//AngleProperty = DependencyProperty.Register("Angle", typeof(double), typeof(RotateAzimuthal  ),
			//	new FrameworkPropertyMetadata(0.0, FrameworkPropertyMetadataOptions.AffectsRender | FrameworkPropertyMetadataOptions.BindsTwoWayByDefault, null, coerceValueCallback));
		}

        //---------------------------------------------------------------------------------------------
        private static object coerceValueCallback(DependencyObject d, object baseValue)
        {
            var angle = (double)baseValue % 360;

            // 角度は 0 ～ 360 の範囲内におさめる
            if (angle < 0)
                return angle + 360;
            else
                return angle;
        }

        //---------------------------------------------------------------------------------------------
        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            var position = e.GetPosition(this);
            FMouseDownVector = position - FCenter;
            FMouseDownAngle = this.Angle;

            e.MouseDevice.Capture(this);
            FIsRotating = true;
        }

        //---------------------------------------------------------------------------------------------
        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (FIsRotating)
            {
                Point position = e.GetPosition(this);
                Vector currentVector = position - FCenter;
                this.Angle = Vector.AngleBetween(FMouseDownVector, currentVector) + FMouseDownAngle;
            }
        }

        //---------------------------------------------------------------------------------------------
        protected override void OnMouseUp(MouseButtonEventArgs e)
        {
            if (FIsRotating)
            {
                e.MouseDevice.Capture(null);
                FIsRotating = false;
            }
        }

        //---------------------------------------------------------------------------------------------
        protected override void OnLostMouseCapture(MouseEventArgs e)
        {
            FIsRotating = false;
        }

        //---------------------------------------------------------------------------------------------
        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            base.OnRenderSizeChanged(sizeInfo);

            // コントロールの中心を回転の中心とする
            FCenter = new Point(this.ActualWidth / 2, this.ActualHeight / 2);
        }
    }
}
