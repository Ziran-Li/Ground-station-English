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

namespace ControlsUnit
{
    /// <summary>
    /// 双滑块Slider 
    /// By lekko
    /// </summary>
    public partial class SilderArrange : UserControl
    {
        #region 私有变量

        private static int _width = 150;  // 拖动条初始宽度
        private static int _height = 30;  // 高度
        private static int _min = 0;      // 最小值
        private static int _max = 100;    // 最大值
        private static int _freq = 20;    // 出现刻度的间距

        #endregion

        // 构造函数
        public SilderArrange()
        {
            InitializeComponent();
        }

        #region 私有属性

        /// <summary>
        /// 裁剪矩阵（头）
        /// </summary>
        private Rect StartRect
        {
            get { return (Rect)GetValue(StartRectProperty); }
            set { SetValue(StartRectProperty, value); }
        }
        private static readonly DependencyProperty StartRectProperty =
            DependencyProperty.Register("StartRect", typeof(Rect), typeof(SilderArrange));

        /// <summary>
        /// 裁剪矩阵（尾）
        /// </summary>
        private Rect EndRect
        {
            get { return (Rect)GetValue(EndRectProperty); }
            set { SetValue(EndRectProperty, value); }
        }
        private static readonly DependencyProperty EndRectProperty =
            DependencyProperty.Register("EndRect", typeof(Rect), typeof(SilderArrange));

        #endregion

        #region 公开依赖属性

        /// <summary>
        /// 刻度间距，默认为10
        /// </summary>
        public int SliderTickFrequency
        {
            get { return (int)GetValue(SliderTickFrequencyProperty); }
            set { SetValue(SliderTickFrequencyProperty, value); }
        }
        public static readonly DependencyProperty SliderTickFrequencyProperty =
            DependencyProperty.Register("SliderTickFrequency", typeof(int), typeof(SilderArrange), new PropertyMetadata(_freq));

        /// <summary>
        /// 控件高度，默认为30
        /// </summary>
        public int SilderHeight
        {
            get { return (int)GetValue(SilderHeightProperty); }
            set { SetValue(SilderHeightProperty, value); }
        }
        public static readonly DependencyProperty SilderHeightProperty =
            DependencyProperty.Register("SilderHeight", typeof(int), typeof(SilderArrange), new PropertyMetadata(_height));

        /// <summary>
        /// 拖动条宽度，默认为150
        /// </summary>
        public int SilderWidth
        {
            get { return (int)GetValue(SilderWidthProperty); }
            set { SetValue(SilderWidthProperty, value); }
        }
        public static readonly DependencyProperty SilderWidthProperty =
            DependencyProperty.Register("SilderWidth", typeof(int), typeof(SilderArrange), new PropertyMetadata(_width));

        /// <summary>
        /// 最小值，默认为0
        /// </summary>
        public int Minimum
        {
            get { return (int)GetValue(MinimumProperty); }
            set { SetValue(MinimumProperty, value); }
        }
        public static readonly DependencyProperty MinimumProperty =
            DependencyProperty.Register("Minimum", typeof(int), typeof(SilderArrange), new PropertyMetadata(_min));

        /// <summary>
        /// 最大值，默认为100
        /// </summary>
        public int Maximum
        {
            get { return (int)GetValue(MaximumProperty); }
            set { SetValue(MaximumProperty, value); }
        }
        public static readonly DependencyProperty MaximumProperty =
            DependencyProperty.Register("Maximum", typeof(int), typeof(SilderArrange), new PropertyMetadata(_max));

        /// <summary>
        /// 选中开始值，默认为0
        /// </summary>
        public int StartValue
        {
            get { return (int)GetValue(StartValueProperty); }
            set { SetValue(StartValueProperty, value); }
        }
        public static readonly DependencyProperty StartValueProperty =
            DependencyProperty.Register("StartValue", typeof(int), typeof(SilderArrange));

        /// <summary>
        /// 选中结束值，默认为100
        /// </summary>
        public int EndValue
        {
            get { return (int)GetValue(EndValueProperty); }
            set { SetValue(EndValueProperty, value); }
        }
        public static readonly DependencyProperty EndValueProperty =
            DependencyProperty.Register("EndValue", typeof(int), typeof(SilderArrange), new PropertyMetadata(_max));

        #endregion

        #region 前台交互

        /// <summary>
        /// 对两个拖动条进行裁剪
        /// </summary>
        private void ClipSilder()
        {
            int selectedValue = EndValue - StartValue;
            int totalValue = Maximum - Minimum;
            double sliderClipWidth = SilderWidth * (StartValue - Minimum + selectedValue / 2) / totalValue;
            // 对第一个拖动条进行裁剪
            StartRect = new Rect(0, 0, sliderClipWidth, SilderHeight);
            // 对第二个拖动条进行裁剪
            EndRect = new Rect(sliderClipWidth, 0, SilderWidth, SilderHeight);
        }

        /// <summary>
        /// 初始化裁剪
        /// </summary>
        private void UC_Arrange_Loaded(object sender, RoutedEventArgs e)
        {
            ClipSilder();
        }

        private void SL_Bat1_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (e.NewValue > EndValue)    // 检查值范围
                StartValue = EndValue;    // 超出，重设为最大值
            ClipSilder();
        }

        private void SL_Bat2_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (e.NewValue < StartValue)
                EndValue = StartValue;
            ClipSilder();
        }

        private void TextBox_KeyUp1(object sender, System.Windows.Input.KeyEventArgs e)
        {
            try
            {
                if (e.Key == Key.Enter)    // 按回车时确认输入
                    StartValue = Convert.ToInt32(((TextBox)sender).Text);
            }
            catch
            {
            }
        }
        public void SetMAxValue(int index_value)
        {
            EndValue = index_value;
        }
        public void SetMInValue(int index_value)
        {
            StartValue = index_value;
        }
        private void TextBox_KeyUp2(object sender, KeyEventArgs e)
        {
            try
            {
                if (e.Key == Key.Enter)
                    EndValue = Convert.ToInt32(((TextBox)sender).Text);
            }
            catch
            {
            }
        }

        #endregion

    }
}