using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;

namespace ControlsUnit
{
    /// <summary>
    /// ControlModeView.xaml の相互作用ロジック
    /// </summary>
    public partial class ControlModeView : UserControl
    {
        public ControlModeView()
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
                "Text", typeof(string), typeof(ControlModeView),
                new FrameworkPropertyMetadata(
                    default(string),
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    OnTextChanged));


        /// <summary>
        /// Slider Zoom
        /// </summary>         
        [Category("ControlsUnit")]
        [Description("UAV Control Mode")]
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
            ControlModeView control = (ControlModeView)d;
            control.ConName.Text = (string)e.NewValue;
        }

        #endregion
    }
}
