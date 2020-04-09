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
    /// Imu_Info_Window.xaml 的交互逻辑
    /// </summary>
    public partial class Imu_Info_Window : UserControl
    {
        public Imu_Info_Window(int id)
        {
            InitializeComponent();
        }
        public void Function_ImuData_Refresh()
        {
            M_Imu_Roll.Content = Math.Round(Constants.mm_IMUDataRoll, 3);
            M_Imu_Pitch.Content = Math.Round(Constants.mm_IMUDataPitch, 3);
            M_Imu_Yaw.Content = Math.Round(Constants.mm_IMUDataYaw, 3);
            M_Gyro_RollRate.Content = Math.Round(Constants.mm_GyroDataRollRate, 3);
            M_Gyro_PitchRate.Content = Math.Round(Constants.mm_GyroDataPitchRate, 3);
            M_Gyro_YawRate.Content = Math.Round(Constants.mm_GyroDataYawRate, 3);
            M_Acc_X.Content = Math.Round(Constants.mm_AccDataX, 3);
            M_Acc_Y.Content = Math.Round(Constants.mm_AccDataY, 3);
            M_Acc_Z.Content = Math.Round(Constants.mm_AccDataZ, 3);
            M_Mag_X.Content = Math.Round(Constants.mm_MagDataX, 3);
            M_Mag_Y.Content = Math.Round(Constants.mm_MagDataY, 3);
            M_Mag_Z.Content = Math.Round(Constants.mm_MagDataZ, 3);
        }
    }
}
