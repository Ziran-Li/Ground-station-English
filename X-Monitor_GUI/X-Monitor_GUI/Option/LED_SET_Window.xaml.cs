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

namespace XMonitor_GUI
{
    /// <summary>
    /// LED_SET_Window.xaml 的交互逻辑
    /// </summary>
    public partial class LED_SET_Window : UserControl
    {
        public LED_SET_Window(int id)
        {
            InitializeComponent();
            DarktoLightSlider.AddHandler(Slider.MouseLeftButtonDownEvent, new MouseButtonEventHandler(this.DarktoLightSlider_MouseLeftButtonDown), true);
            DarktoLightSlider.AddHandler(Slider.MouseLeftButtonUpEvent, new MouseButtonEventHandler(this.DarktoLightSlider_MouseLeftButtonUp), true);
        }
/// <summary>
/// ////////////////夜视灯
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void NightCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            System.Windows.Controls.CheckBox chk = (System.Windows.Controls.CheckBox)sender;
            if (chk.IsChecked == true)
            {
                bool m_ifstartforward;//前置开关
                bool m_ifstartback;//后置开关
                bool m_ifturndirection;//夜视开关
                if (Forward_Switch.IsChecked == true)//获取前置开关
                {
                    m_ifstartforward = true;
                }
                else
                {
                    m_ifstartforward = false;
                }
                if (Back_Switch.IsChecked ==true)//获取后置开关
                {
                    m_ifstartback = true;
                }
                else
                {
                    m_ifstartback = false;
                }
                if (Night_Switch.IsChecked ==true)//获取夜视开关
                {
                    m_ifturndirection = true;
                }
                else
                {
                    m_ifturndirection = false;
                }

                if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == false)//都关闭
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000000);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == false)//前置开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000004);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == false)//后置开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000008);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == true)//夜视开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000010);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == false)//前置和后置打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000000C);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == true)//前置和转向打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000014);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == true)//后置和夜视打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000018);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == true)//都打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000001C);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
            }
        }
        private void NightUncheckBox_UnChecked(object sender, RoutedEventArgs e)
        {
            System.Windows.Controls.CheckBox chk = (System.Windows.Controls.CheckBox)sender;
            if (chk.IsChecked == false)
            {
                bool m_ifstartforward;//前置开关
                bool m_ifstartback;//后置开关
                bool m_ifturndirection;//夜视开关
                if (Forward_Switch.IsChecked == true)//获取前置开关
                {
                    m_ifstartforward = true;
                }
                else
                {
                    m_ifstartforward = false;
                }
                if (Back_Switch.IsChecked == true)//获取后置开关
                {
                    m_ifstartback = true;
                }
                else
                {
                    m_ifstartback = false;
                }
                if (Night_Switch.IsChecked == true)//获取夜视开关
                {
                    m_ifturndirection = true;
                }
                else
                {
                    m_ifturndirection = false;
                }

                if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == false)//都关闭
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000000);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == false)//前置开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000004);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == false)//后置开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000008);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == true)//夜视开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000010);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == false)//前置和后置打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000000C);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == true)//前置和转向打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000014);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == true)//后置和夜视打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000018);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == true)//都打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000001C);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
            }
        }

/// <summary>
/// ///前视灯
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void ForwardCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            System.Windows.Controls.CheckBox chk = (System.Windows.Controls.CheckBox)sender;
            if (chk.IsChecked == true)
            {
                bool m_ifstartforward;//前置开关
                bool m_ifstartback;//后置开关
                bool m_ifturndirection;//夜视开关
                if (Forward_Switch.IsChecked == true)//获取前置开关
                {
                    m_ifstartforward = true;
                }
                else
                {
                    m_ifstartforward = false;
                }
                if (Back_Switch.IsChecked == true)//获取后置开关
                {
                    m_ifstartback = true;
                }
                else
                {
                    m_ifstartback = false;
                }
                if (Night_Switch.IsChecked == true)//获取夜视开关
                {
                    m_ifturndirection = true;
                }
                else
                {
                    m_ifturndirection = false;
                }

                if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == false)//都关闭
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000000);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == false)//前置开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000004);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == false)//后置开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000008);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == true)//夜视开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000010);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == false)//前置和后置打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000000C);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == true)//前置和转向打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000014);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == true)//后置和夜视打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000018);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == true)//都打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000001C);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
            }
        }
        private void ForwardUncheckBox_UnChecked(object sender, RoutedEventArgs e)
        {
            System.Windows.Controls.CheckBox chk = (System.Windows.Controls.CheckBox)sender;
            if (chk.IsChecked == false)
            {
                bool m_ifstartforward;//前置开关
                bool m_ifstartback;//后置开关
                bool m_ifturndirection;//夜视开关
                if (Forward_Switch.IsChecked == true)//获取前置开关
                {
                    m_ifstartforward = true;
                }
                else
                {
                    m_ifstartforward = false;
                }
                if (Back_Switch.IsChecked == true)//获取后置开关
                {
                    m_ifstartback = true;
                }
                else
                {
                    m_ifstartback = false;
                }
                if (Night_Switch.IsChecked == true)//获取夜视开关
                {
                    m_ifturndirection = true;
                }
                else
                {
                    m_ifturndirection = false;
                }

                if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == false)//都关闭
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000000);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == false)//前置开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000004);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == false)//后置开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000008);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == true)//夜视开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000010);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == false)//前置和后置打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000000C);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == true)//前置和转向打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000014);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == true)//后置和夜视打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000018);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == true)//都打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000001C);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
            }
        }
/// <summary>
/// 后视灯
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void BackCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            System.Windows.Controls.CheckBox chk = (System.Windows.Controls.CheckBox)sender;
            if (chk.IsChecked == true)
            {
                bool m_ifstartforward;//前置开关
                bool m_ifstartback;//后置开关
                bool m_ifturndirection;//夜视开关
                if (Forward_Switch.IsChecked == true)//获取前置开关
                {
                    m_ifstartforward = true;
                }
                else
                {
                    m_ifstartforward = false;
                }
                if (Back_Switch.IsChecked == true)//获取后置开关
                {
                    m_ifstartback = true;
                }
                else
                {
                    m_ifstartback = false;
                }
                if (Night_Switch.IsChecked == true)//获取夜视开关
                {
                    m_ifturndirection = true;
                }
                else
                {
                    m_ifturndirection = false;
                }
                if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == false)//都关闭
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000000);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == false)//前置开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000004);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == false)//后置开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000008);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == true)//夜视开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000010);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == false)//前置和后置打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000000C);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == true)//前置和转向打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000014);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == true)//后置和夜视打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000018);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == true)//都打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000001C);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
            }
        }
        private void BackUncheckBox_UnChecked(object sender, RoutedEventArgs e)
        {
            System.Windows.Controls.CheckBox chk = (System.Windows.Controls.CheckBox)sender;
            if (chk.IsChecked == false)
            {
                bool m_ifstartforward;//前置开关
                bool m_ifstartback;//后置开关
                bool m_ifturndirection;//夜视开关
                if (Forward_Switch.IsChecked == true)//获取前置开关
                {
                    m_ifstartforward = true;
                }
                else
                {
                    m_ifstartforward = false;
                }
                if (Back_Switch.IsChecked == true)//获取后置开关
                {
                    m_ifstartback = true;
                }
                else
                {
                    m_ifstartback = false;
                }
                if (Night_Switch.IsChecked == true)//获取夜视开关
                {
                    m_ifturndirection = true;
                }
                else
                {
                    m_ifturndirection = false;
                }

                if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == false)//都关闭
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000000);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == false)//前置开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000004);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == false)//后置开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000008);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == true)//夜视开关打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000010);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == false)//前置和后置打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000000C);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == true)//前置和转向打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000014);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == true)//后置和夜视打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000018);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
                else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == true)//都打开
                {
                    Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000001C);
                    Constants.AutoWing_Switch_Open_Flag = 1;
                }
            }
        }

        private void DarktoLightSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            int value = (Int32)DarktoLightSlider.Value;
            bool m_ifstartforward;//前置开关
            bool m_ifstartback;//后置开关
            bool m_ifturndirection;//夜视开关

            if (Forward_Switch.IsChecked == true)//获取前置开关
            {
                m_ifstartforward = true;
            }
            else
            {
                m_ifstartforward = false;
            }
            if (Back_Switch.IsChecked == true)//获取后置开关
            {
                m_ifstartback = true;
            }
            else
            {
                m_ifstartback = false;
            }
            if (Night_Switch.IsChecked == true)//获取夜视开关
            {
                m_ifturndirection = true;
            }
            else
            {
                m_ifturndirection = false;
            }
   /*         if (value < 30)
            {
                Constants.Led_nighttolight_state = 0;//低亮
            }
            else if ((value >= 30) && (value < 60))
            {
                Constants.Led_nighttolight_state = 1;//中亮
            }
            else
            {
                Constants.Led_nighttolight_state = 2;//高亮
            }*/
            Constants.Led_nighttolight_state = value;


            if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == false)//都关闭
            {
                Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000000);
            }
            else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == false)//前置开关打开
            {
                Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000004);
            }
            else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == false)//后置开关打开
            {
                Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000008);
            }
            else if (m_ifstartforward == false && m_ifstartback == false && m_ifturndirection == true)//夜视开关打开
            {
                Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000010);
            }
            else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == false)//前置和后置打开
            {
                Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000000C);
            }
            else if (m_ifstartforward == true && m_ifstartback == false && m_ifturndirection == true)//前置和转向打开
            {
                Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000014);
            }
            else if (m_ifstartforward == false && m_ifstartback == true && m_ifturndirection == true)//后置和夜视打开
            {
                Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x00000018);
            }
            else if (m_ifstartforward == true && m_ifstartback == true && m_ifturndirection == true)//都打开
            {
                Constants.Led_nighttolight_state = (Constants.Led_nighttolight_state | 0x0000001C);
            }
        }

        private void DarktoLightSlider_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Constants.AutoWing_Led_Flag = 1;
        }

        private void DarktoLightSlider_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Constants.AutoWing_Led_Flag = 0;
        }
    }
}
