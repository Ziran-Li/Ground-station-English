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
using System.IO;
namespace XMonitor_GUI
{
    /// <summary>
    /// Wifi_Set_Window.xaml 的交互逻辑
    /// </summary>
    public partial class Wifi_Set_Window : UserControl
    {
        public Wifi_Set_Window(int id)
        {
            InitializeComponent();
            if (File.Exists("C:\\AutoWing\\AutoWing.txt"))
            {
                StreamReader sr = File.OpenText("C:\\AutoWing\\AutoWing.txt");
                int k = 0;//当前读取的是第几行
                String[] str = new String[2];
                while(sr.Peek()!=-1)
                {
                    str[k] = sr.ReadLine();
                    k++;
                }
                Wifi_IP_SET.Text = str[0];//显示ip相关的值
                Wifi_Set_Port.Text = str[1];//显示端口号相关的值
                sr.Close();
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
///////////////////////////////////存储设置好的ip和端口号///////////////////////////////////////////////////////////
            try
            {
                Constants.User_Wifi_Set_IP = Wifi_IP_SET.Text;
                Constants.User_Wifi_Set_Port = Wifi_Set_Port.Text;
                if (Wifi_IP_SET.Text == "")//如果ip里面为空
                {
                    System.Windows.MessageBox.Show("IP Can Not Be Null");
                    return;
                }
                if (Wifi_Set_Port.Text == "")
                {
                    System.Windows.MessageBox.Show("Port Can Not Be Null");
                    return;
                }
                StreamWriter sw = new StreamWriter("C:\\AutoWing\\AutoWing.txt");
                sw.WriteLine(Constants.User_Wifi_Set_IP);
                sw.WriteLine(Constants.User_Wifi_Set_Port);
                sw.Flush();
                sw.Close();
                /////////////////////////////////////存储设置好的ip和端口号///////////////////////////////////////////////////////////

                //////////////////////////////////////连接wifi调用主函数里面的连接函数///////////////////////////////////////////////
                MainWindow.pWin.Open_Wifi_Function();
            }
            catch (Exception)
            {
                
            }

        }
    }
}
