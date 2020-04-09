using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace ControlsUnit
{
    /// <summary>
    /// AlarmMessage.xaml の相互作用ロジック
    /// </summary>
    public partial class AlarmMessage : UserControl
    {
        public AlarmMessage()
        {
            InitializeComponent();
        }
		// 文字の出現回数をカウント
		private int CountChar(string s, char c)
		{
			return s.Length - s.Replace(c.ToString(), "").Length;
		}

        #region イベント

        #endregion

        #region プロパティ
        /// <summary>
        /// 
        /// </summary>
        public static readonly DependencyProperty AlarmFlagProperty =
            DependencyProperty.Register(
                "AlarmFlag", typeof(int), typeof(AlarmMessage),
                new FrameworkPropertyMetadata(
                    default(int),
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    OnValueChanged));

        /// <summary>
        /// 警告メッセージ
        /// </summary>
        public static readonly DependencyProperty AlarmMessageProperty =
            DependencyProperty.Register(
                "Message", typeof(string), typeof(AlarmMessage),
                new FrameworkPropertyMetadata(
                    default(string),
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    OnAlarmMessageChanged));

        /// <summary>
        /// 
        /// </summary>         
        [Category("ControlsUnit")]
        [Description("Alarm Message")]
        [Browsable(true)]
        [Bindable(true)]
        public int AlarmFlag
        {
            get { return (int)GetValue(AlarmFlagProperty); }
            set { SetValue(AlarmFlagProperty, value); }
        }

        /// <summary>
        /// 警告メッセージ
        /// </summary>         
        [Category("ControlsUnit")]
        [Description("Alarm Message")]
        [Browsable(true)]
        [Bindable(true)]
        public string Message
        {
            get { return (string)GetValue(AlarmMessageProperty); }
            set { SetValue(AlarmMessageProperty, value); }
        }

        #endregion

        #region プロパティメソッド
        /// <summary>
        /// 
        /// </summary>
        private static void OnValueChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            AlarmMessage control = (AlarmMessage)d;
            SolidColorBrush ColorBrush = new SolidColorBrush();

			int _count = control.CountChar(control.AlarmMessage0.Text, '\n');
			if (_count > 3)
			{
				int _hi = (_count - 3) * 18;
				control.grid.Height += _hi;
			}
			else
			{
				control.grid.Height = 100;
			}
			
            // 警告レベル
            // 1:    　　   　　　  注意【Caution】
			// 2:注意  Warning      警告【Warning】
			// 3:緊急  Emergency    危険【Danger】
            // default:その他のエラー
            switch ((int)e.NewValue)
            {
                case 1:		//水色
                    control.AlarmState.Content = "Message";
                    ColorBrush.Color = Color.FromArgb(255, 0, 176, 240);
                    ColorBrush.Opacity = 0.6;
                    control.AlarmColor.Fill = ColorBrush;//Color.FromArgb(255, 255, 255, 0);
					control.AlarmMessage1.Text = "";
                    break;
                case 2:		//黄色
                    control.AlarmState.Content = "Warning";
                    ColorBrush.Color = Color.FromArgb(255, 255, 192, 0);
                    ColorBrush.Opacity = 0.6;
                    control.AlarmColor.Fill = ColorBrush;

					if (control.AlarmMessage0.Text.IndexOf("バッテリ") != -1)
					{
						control.AlarmMessage1.Text = "电量不足，请注意电池电量。";

					}
					else
					{
						control.AlarmMessage1.Text = "请注意当前电脑负荷太重。";
					}
                    break;
                case 3:		//赤
                    control.AlarmState.Content = "Emergency";//"Danger";
                    ColorBrush.Color = Color.FromArgb(255, 192, 0, 0);
                    ColorBrush.Opacity = 0.80;
                    control.AlarmColor.Fill = ColorBrush;

					if (control.AlarmMessage0.Text.IndexOf("セーフティ") != -1)
					{
						control.AlarmMessage1.Text = "安全工作中，请注意正确操作。";
					}
					else
					{
						control.AlarmMessage1.Text = "出现故障请返回。";
					}
                    break;
                default:　//白
                    control.AlarmState.Content = "ERROR";
                    ColorBrush.Color = Colors.White;
                    ColorBrush.Opacity = 0.6;
                    control.AlarmColor.Fill = ColorBrush;
					control.AlarmMessage1.Text = "";
                    break;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        private static void OnAlarmMessageChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            AlarmMessage control = (AlarmMessage)d;

            control.AlarmMessage0.Text = (string)e.NewValue;
        }

        #endregion

    }
}
