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
using System.Windows.Forms;     //添加引用
namespace XMonitor_GUI
{
    /// <summary>
    /// Version_Info_Show.xaml 的交互逻辑
    /// </summary>
    public partial class Version_Info_Show : System.Windows.Controls.UserControl
    {
        public Version_Info_Show(int id)
        {
            InitializeComponent();
            Function_AllVersion_Show();
        }
        public void Function_AllVersion_Show()
        {
            /////////////ap信息显示///////////////////////////
            m_ap_hard.Content = Constants.AP_HardWare_VersionShow.Replace("_", "__");
            m_ap_soft.Content = Constants.AP_SoftWare_VersionShow.Replace("_", "__");
            //      m_ap_id.Content = Constants.AP_EquipmentID_VersionShow.Replace("_", "__");
            m_ap_id.Content = "";
            //////////////////IMU信息显示///////////////////////
            m_imu_hard.Content = Constants.IMU_HardWare_VersionShow.Replace("_", "__");
            m_imu_soft.Content = Constants.IMU_SoftWare_VersionShow.Replace("_", "__");
            m_imu_id.Content = Constants.IMU_EquipmentID_VersionShow.Replace("_", "__");
            /////////////////GPS信息显示////////////////////////
            m_gps_hard.Content = Constants.GPS_HardWare_VersionShow.Replace("_", "__");
            m_gps_soft.Content = Constants.GPS_SoftWare_VersionShow.Replace("_", "__");
            m_gps_id.Content = Constants.GPS_EquipmentID_VersionShow.Replace("_", "__");
            ////////////////HUB信息显示/////////////////////////
            m_hub_hard.Content = Constants.HUB_HardWare_VersionShow.Replace("_", "__");
            m_hub_soft.Content = Constants.HUB_SoftWare_VersionShow.Replace("_", "__");
            m_hub_id.Content = Constants.HUB_EquipmentID_VersionShow.Replace("_", "__");
            ///////////////mag信息显示////////////////////////
            m_mag_hard.Content = Constants.MAG_HardWare_VersionShow.Replace("_", "__");
            m_mag_soft.Content = Constants.MAG_SoftWare_VersionShow.Replace("_", "__");
            m_mag_id.Content = Constants.MAG_EquipmentID_VersionShow.Replace("_", "__");
            ////////////////led信息显示////////////////////////
            m_led_hard.Content = Constants.LED_HardWare_VersionShow.Replace("_", "__");
            m_led_soft.Content = Constants.LED_SoftWare_VersionShow.Replace("_", "__");
            m_led_id.Content = Constants.LED_EquipmentID_VersionShow.Replace("_", "__");
            ////////////////FDR信息显示////////////////////////
            m_fdr_hard.Content = Constants.FDR_HardWare_VersionShow.Replace("_", "__");
            m_fdr_soft.Content = Constants.FDR_SoftWare_VersionShow.Replace("_", "__");
            m_fdr_id.Content = Constants.FDR_EquipmentID_VersionShow.Replace("_", "__");
            ////////////////DTU信息显示////////////////////////
            m_dtu_hard.Content = Constants.DTU_HardWare_VersionShow.Replace("_", "__");
            m_dtu_soft.Content = Constants.DTU_SoftWare_VersionShow.Replace("_", "__");
            m_dtu_id.Content = Constants.DTU_EquipmentID_VersionShow.Replace("_", "__");

            ////////////////RTK信息显示////////////////////////
            m_rtk_hard.Content = Constants.RTK_HardWare_VersionShow.Replace("_", "__");
            m_rtk_soft.Content = Constants.RTK_SoftWare_VersionShow.Replace("_", "__");
            m_rtk_id.Content = Constants.RTK_EquipmentID_VersionShow.Replace("_", "__");

        }

        private void FireWare_Btn_Click(object sender, RoutedEventArgs e)
        {
            if(AP_CHECK.IsChecked==true)
            {
                Constants.Update_Equipment_ID = 1;
            }
            else if (IMU_CHECK.IsChecked==true)
            {
                Constants.Update_Equipment_ID = 2;
            }
            else if (GPS_CHECK.IsChecked==true)
            {
                Constants.Update_Equipment_ID = 3;
            }
            else if (HUB_CHECK.IsChecked==true)
            {
                Constants.Update_Equipment_ID = 4;
            }
            else if (MAG_CHECK.IsChecked==true)
            {
                Constants.Update_Equipment_ID = 5;
            }
            else if (LED_CHECK.IsChecked==true)
            {
                Constants.Update_Equipment_ID = 6;
            }
            else
            {
                System.Windows.MessageBox.Show("请先选择需要升级的设备");
                return;
            }

            if (System.Windows.Forms.MessageBox.Show("是否确定固件升级", "提示", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.Yes)
            {
                MainWindow.pWin.Function_Wait_ExternShow();
                Constants.Fireware_index_flag = 1;
            }

        }
    }
}
