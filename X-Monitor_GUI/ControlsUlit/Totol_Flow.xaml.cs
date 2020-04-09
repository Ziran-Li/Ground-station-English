using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel;
namespace ControlsUnit
{
    /// <summary>
    /// Totol_Flow.xaml 的交互逻辑
    /// </summary>
    public partial class Totol_Flow : UserControl
    {
        public Totol_Flow()
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
                "Text", typeof(string), typeof(Totol_Flow),
                new FrameworkPropertyMetadata(
                    default(string),
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    OnTextChanged));
        /// <summary>
        /// Slider Zoom
        /// </summary>         
        [Category("ControlsUnit")]
        [Description("UAV ToTalFlow Value")]
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
            Totol_Flow control = (Totol_Flow)d;
            control.m_total_flow.Text = (string)e.NewValue;
        }
        #endregion
    }
}
