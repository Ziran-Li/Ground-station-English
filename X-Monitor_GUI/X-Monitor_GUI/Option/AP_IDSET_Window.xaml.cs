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
    /// AP_IDSET_Window.xaml 的交互逻辑
    /// </summary>
    public partial class AP_IDSET_Window : UserControl
    {
        public AP_IDSET_Window(int id)
        {
            InitializeComponent();
        }


        private void AP_Code_Set_Click(object sender, RoutedEventArgs e)
        {
            byte[] String_TxData = new byte[8];
            string String_Used = "";
            for (int i = 0; i < 8; i++)
            {
                String_TxData[i] = 0;
            }
            if (Edit_AP_Name.Text != " ")
            {
                if (Edit_AP_Name.Text.Length != 8)
                {
                    System.Windows.MessageBox.Show("飞控编号必须是8个字符");
                    return;
                }
                else
                {
                    StringReader apid_stringreader = new StringReader(Edit_AP_Name.Text);
                    for (int j = 0; j < Edit_AP_Name.Text.Length; j++)
                    {
                        String_TxData[j] = (byte)apid_stringreader.Read();
                        if (String_TxData[j] >= 'a' && String_TxData[j] <= 'z')
                        {
                            String_TxData[j] = (byte)(String_TxData[j] - 32);
                        }
                    }
                }
                String_Used = System.Text.Encoding.Default.GetString(String_TxData);
                Constants.AP_SETID_String = String_Used;
                if (Constants.AP_SETID_String.Length == 8)
                {
                    MainWindow.pWin.Function_Wait_ExternShow();
                    Constants.Set_APID_Flag = 1;//启动发送
                }
                else
                {
                    System.Windows.MessageBox.Show("飞控编号只能是8个字符");
                }
            }
            else
            {
                System.Windows.MessageBox.Show("飞控编号不能为空");
            }
        }

        private void Edit_AP_Name_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberletter(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        public static bool isNumberletter(string _string)
        {
            if (string.IsNullOrEmpty(_string))
            {
                return false;
            }
            foreach (char c in _string)
            {
                if (!char.IsLetterOrDigit(c))
                {
                    return false;
                }
            }
            return true;
        }
        public void Function_APID_Show()
        {
            current_AP_ID.Content = Constants.AP_SHOWID_String;
        }

        private void Edit_AP_Name_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Space)
            {
                e.Handled = true;
            }
        }

    }
}
