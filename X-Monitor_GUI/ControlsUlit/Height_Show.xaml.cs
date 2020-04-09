using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;

namespace ControlsUnit
{
    /// <summary>
    /// ControlModeView.xaml の相互作用ロジック
    /// </summary>
    public partial class Height_Show : UserControl
    {
        public Height_Show()
        {
            InitializeComponent();
        }

        #region イベント

        #endregion

        #region プロパティ
        /// <summary>
        /// 数値データの依存プロパティです。
        /// </summary>
        public static readonly DependencyProperty TextProperty =
            DependencyProperty.Register(
                "Text", typeof(string), typeof(Height_Show),
                new FrameworkPropertyMetadata(
                    default(string),
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    OnTextChanged));


        /// <summary>
        /// Slider Zoom
        /// </summary>         
        [Category("ControlsUnit")]
        [Description("UAV Height Value")]
        [Browsable(true)]
        [Bindable(true)]
        public string Text
        {
            get { return (string)GetValue(TextProperty); }
            set { SetValue(TextProperty, value); }
        }
        #endregion

        #region プロパティメソッド
        /// <summary>
        /// 値の変更時に呼ばれます。
        /// </summary>
        private static void OnTextChanged(DependencyObject d,
                                           DependencyPropertyChangedEventArgs e)
        {
            Height_Show control = (Height_Show)d;
            control.ConName.Text = (string)e.NewValue;
        }

        #endregion
    }
}
