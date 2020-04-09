using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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
using System.Text.RegularExpressions;

namespace TestIPBox.Controls
{
    /// <summary>
    /// IPBox.xaml 的交互逻辑-
    /// </summary>
    public partial class IPBox : UserControl
    {
        //
        // 摘要：
        //     构造函数
        /// <summary>
        /// 构造函数
        /// </summary>
        public IPBox()
        {
            InitializeComponent();

            // 处理粘贴
            DataObject.AddPastingHandler(this, new DataObjectPastingEventHandler(IPTextBox_Paste));

            // 设置textBox次序¨°
            ipTextBox1.setNeighbour(null, ipTextBox2);
            ipTextBox2.setNeighbour(ipTextBox1, ipTextBox3);
            ipTextBox3.setNeighbour(ipTextBox2, ipTextBox4);
            ipTextBox4.setNeighbour(ipTextBox3, null);
        }

        // 处理粘贴 类似IP格式的才能粘贴
        private void IPTextBox_Paste(object sender, DataObjectPastingEventArgs e)
        {

            if (e.DataObject.GetDataPresent(typeof(string)))
            {
                string value = e.DataObject.GetData(typeof(string)).ToString();
                this.Text = value;
            }
            e.CancelCommand();
        }

        #region DP

        public static readonly DependencyProperty TextProperty =
            DependencyProperty.Register("Text", typeof(String), typeof(IPBox),
            new FrameworkPropertyMetadata(String.Empty, new PropertyChangedCallback(TextPropertyChangedCallback)));

        [Description("IPAddress IP地址")]
        [Category("Common Properties")]
        public String Text
        {
            get
            {
                return (String)this.GetValue(TextProperty);
            }
            set
            {
                this.SetValue(TextProperty, value);
            }
        }

        private static void TextPropertyChangedCallback(DependencyObject sender, DependencyPropertyChangedEventArgs arg)
        {
            if (sender != null && sender is IPBox)
            {
                IPBox ipbox = sender as IPBox;
                ipbox.OnTextUpdated((String)arg.OldValue, (String)arg.NewValue);
            }
        }

        #endregion

        #region Event

        public static readonly RoutedEvent TextUpdatedEvent =
            EventManager.RegisterRoutedEvent("TextUpdated",
             RoutingStrategy.Bubble, typeof(RoutedPropertyChangedEventHandler<String>), typeof(IPBox));

        [Description("IP地址被更新后发生")]
        public event RoutedPropertyChangedEventHandler<String> TextUpdated
        {
            add
            {
                this.AddHandler(TextUpdatedEvent, value);
            }
            remove
            {
                this.RemoveHandler(TextUpdatedEvent, value);
            }
        }

        protected virtual void OnTextUpdated(String oldValue, String newValue)
        {
            RoutedPropertyChangedEventArgs<String> arg =
                new RoutedPropertyChangedEventArgs<String>(oldValue, newValue, TextUpdatedEvent);
            try
            {
                string[] ipnum = newValue.Split('.');
                if (ipnum.Length == 4)
                {
                    ipTextBox1.Text = ipnum[0];
                    ipTextBox2.Text = ipnum[1];
                    ipTextBox3.Text = ipnum[2];
                    ipTextBox4.Text = ipnum[3];
                }
                this.RaiseEvent(arg);
            }
            catch (Exception ex)
            {
            }

        }

        #endregion

        #region Validate

        /// <summary>
        /// IP地址格式正确返回 true
        /// </summary>
        /// <returns></returns>
        public bool ValidateIPAddress()
        {
            string pattern = @"^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$";

            //判断IP地址格式
            return Regex.IsMatch(this.Text, pattern);
        }

        #endregion

        private void ipTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (
                !string.IsNullOrWhiteSpace(ipTextBox1.Text)
                || !string.IsNullOrWhiteSpace(ipTextBox2.Text)
                || !string.IsNullOrWhiteSpace(ipTextBox3.Text)
                || !string.IsNullOrWhiteSpace(ipTextBox4.Text)
                )
            {
                this.Text = ipTextBox1.Text + "." + ipTextBox2.Text + "." + ipTextBox3.Text + "." + ipTextBox4.Text;
            }
            else
            {
                this.Text = string.Empty;
            }
        }
    }
}
