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
using System.Windows.Forms;
using System.IO;
namespace XMonitor_GUI
{
    /// <summary>
    /// Index_SoftWare_Set.xaml 的交互逻辑
    /// </summary>
    public partial class Index_SoftWare_Set : System.Windows.Controls.UserControl
    {
        int lock_pid_flag = 0;
        ////////////遥控器校准///////////////////
        byte roll_if_reverse = 0;
        byte pitch_if_reverse = 0;
        byte yaw_if_reverse = 0;
        byte throttle_if_reverse = 0;
        byte switcha_reverse = 0;
        byte switchb_reverse = 0;
        byte switchc_reverse = 0;
        byte switchd_reverse = 0;



        ///////文件名字//////////////////////////
        String Insert_File_Name = "";
        UInt64 Data_All_LineNums = 0;
        String line;
        /// <summary>
        /// 机型
        /// </summary>
        int Ay_FlightModel_Index = 1;
        int Ay_UsedFlightModel_Index = 1;
        /// <summary>
        /// 电池
        /// </summary>
        int Ay_BatteryModel_Index = 1;
        int Ay_UsedBatteryModel_Index = 0;
        /// <summary>
        /// 遥控器
        /// </summary>
        /// <param name="id"></param>
        int Ay_TelModel_Index = 1;
        int Ay_UsedTelModel_Index = 0;
        /// <summary>
        /// 遥控器是否回中
        /// </summary>
        /// <param name="id"></param>
        int Ay_UsedIfBack_Index = 0;
        /// <summary>
        /// 怠速值
        /// </summary>
        /// <param name="id"></param>
        int Ay_UsedIdleSpeed_Index = 5;
        /// <summary>
        /// 遥控器什么手
        /// </summary>
        /// <param name="id"></param>
        int Which_Country_Flag = 0;

        System.Windows.Point LeftCenter;
        System.Windows.Point RightCenter;

        System.Windows.Point Middle_Modify_LeftCenter;
        System.Windows.Point Middle_Modify_RightCenter;
        public Index_SoftWare_Set(int id)
        {
            InitializeComponent();
            ///////////////////////作业参数设置初始化///////////////////////////////////////
            Constants.Jump_To5or50Hz_Flag = 0;
            Function_WorkIndexSet_Init();//作业参数设置初始化
            Function_Combox_init();//遥控器是否回中初始化以及安全策略的动作
            //   Function_Motors_init();//电机初始化
            if (Constants.Show_DataSelect_QUADE != 0)
            {
                Function_Reback_ShowInfo();
            }
        }
        //public Function_Motors_init()
        //{

        //}
        public void Function_AnQuanCeLue_ShowInfo()
        {
            Edit_FirstNotice_Volt.Text = Constants.Show_UAV_Noticefirstvolt.ToString("#0.00");
            Edit_SecondNotice_Volt.Text = Constants.Show_UAV_Noticesecondvolt.ToString("#0.00");
            M_Lowvolt_Action.SelectedIndex = (int)Constants.Show_UAV_Noticesecondaction;
            M_Losecontrol_Action.SelectedIndex = (int)Constants.Show_UAV_Noticelosecontrolaction;
            Edit_Enclosuree_Distance.Text = Constants.Show_UAV_Noticedistance.ToString("#0.0");
            Edit_Enclosuree_Height.Text = Constants.Show_UAV_Noticeheight.ToString("#0.0");
            M_FangHangHeight.Text = Constants.Show_UAV_Noticefanhanggaodu.ToString("#0.0");
        }
        public void Function_ZhiBaiZuoYe_ShowInfo()
        {
            Edit_Zhibao_Speed.Text = Constants.Show_UAV_ZhiBaoSpeed.ToString("#0.0");
            Edit_Zhibao_Range.Text = Constants.Show_UAV_ZhiBaoFuDu.ToString("#0.0");
            m_TerrainFollowYesOrNo.SelectedIndex = (int)Constants.Show_UAV_ZhiBaoAction;
        }
        public void Function_Hangpai_ShowInfo()
        {
            Edit_Photo_Speed.Text = Constants.Show_UAV_HangPaiSpeed.ToString("#0.0");
            Edit_Photo_upSpeed.Text = Constants.Show_UAV_HangPaiUpSpeed.ToString("#0.0");
            Edit_Photo_downSpeed.Text = Constants.Show_UAV_HangPaiDownSpeed.ToString("#0.0");
            Edit_M9Low_Position.Text = Constants.Show_UAV_M9Low.ToString("#0.0");
            Edit_M9Middle_Position.Text = Constants.Show_UAV_M9Middle.ToString("#0.0");
            Edit_M9High_Position.Text = Constants.Show_UAV_M9High.ToString("#0.0");
            Edit_M10Low_Position.Text = Constants.Show_UAV_M10Low.ToString("#0.0");
            Edit_M10Middle_Position.Text = Constants.Show_UAV_M10Middle.ToString("#0.0");
            Edit_M10High_Position.Text = Constants.Show_UAV_M10High.ToString("#0.0");
        }
        public void Function_YKQJZ()
        {
            ///////////////////////////roll////////////////////////////////////////////////
            if ((Constants.Show_New_YK_roll / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_roll / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Maxroll / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Maxroll / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Minroll / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Minroll / 3000.0 * 150) <= 150)
            {
                m_label_RollValue.Height = (Constants.Show_New_YK_roll / 3000.0 * 150);
                Canvas.SetTop(m_label_RollValue, (150 - m_label_RollValue.Height));//roll中的最大值显示
                Canvas.SetTop(m_lable_RollMax, (150 - Constants.Show_New_YK_Maxroll / 3000.0 * 150));//roll中的最大值显示
                Canvas.SetTop(m_lable_RollMin, (150 - Constants.Show_New_YK_Minroll / 3000.0 * 150));//roll中的最小值显示
            }
            /////////////////////////pitch///////////////////////////////////////////////////
            if ((Constants.Show_New_YK_pitch / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_pitch / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Maxpitch / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Maxpitch / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Minpitch / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Minpitch / 3000.0 * 150) <= 150)
            {
                m_label_PitchValue.Height = (Constants.Show_New_YK_pitch / 3000.0 * 150);
                Canvas.SetTop(m_label_PitchValue, (150 - m_label_PitchValue.Height));//roll中的最大值显示
                Canvas.SetTop(m_lable_PitchMax, (150 - Constants.Show_New_YK_Maxpitch / 3000.0 * 150));//roll中的最大值显示
                Canvas.SetTop(m_lable_PitchMin, (150 - Constants.Show_New_YK_Minpitch / 3000.0 * 150));//roll中的最小值显示
            }
            //////////////////////////yaw//////////////////////////////////////////////////
            if ((Constants.Show_New_YK_yaw / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_yaw / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_MaxYaw / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_MaxYaw / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_MinYaw / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_MinYaw / 3000.0 * 150) <= 150)
            {
                m_label_YawValue.Height = (Constants.Show_New_YK_yaw / 3000.0 * 150);
                Canvas.SetTop(m_label_YawValue, (150 - m_label_YawValue.Height));//roll中的最大值显示
                Canvas.SetTop(m_lable_YawMax, (150 - Constants.Show_New_YK_MaxYaw / 3000.0 * 150));//roll中的最大值显示
                Canvas.SetTop(m_lable_YawMin, (150 - Constants.Show_New_YK_MinYaw / 3000.0 * 150));//roll中的最小值显示
            }
            ////////////////////////////throttle////////////////////////////////////////////
            if ((Constants.Show_New_YK_throttle / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_throttle / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Maxthrottle / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Maxthrottle / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Minthrottle / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Minthrottle / 3000.0 * 150) <= 150)
            {
                m_label_throttleValue.Height = (Constants.Show_New_YK_throttle / 3000.0 * 150);
                Canvas.SetTop(m_label_throttleValue, (150 - m_label_throttleValue.Height));//roll中的最大值显示
                Canvas.SetTop(m_lable_ThrottleMax, (150 - Constants.Show_New_YK_Maxthrottle / 3000.0 * 150));//roll中的最大值显示
                Canvas.SetTop(m_lable_ThrottleMin, (150 - Constants.Show_New_YK_Minthrottle / 3000.0 * 150));//roll中的最小值显示
            }
            //////////////////////////sw1//////////////////////////////////////////////////
            if ((Constants.Show_New_YK_switch1 / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_switch1 / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Maxswitch1 / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Maxswitch1 / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Minswitch1 / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Minswitch1 / 3000.0 * 150) <= 150)
            {
                m_label_Sw1Value.Height = (Constants.Show_New_YK_switch1 / 3000.0 * 150);
                Canvas.SetTop(m_label_Sw1Value, (150 - m_label_Sw1Value.Height));//roll中的最大值显示
                Canvas.SetTop(m_lable_sw1Max, (150 - Constants.Show_New_YK_Maxswitch1 / 3000.0 * 150));//roll中的最大值显示
                Canvas.SetTop(m_lable_sw1Min, (150 - Constants.Show_New_YK_Minswitch1 / 3000.0 * 150));//roll中的最小值显示
            }
            //////////////////////////sw2//////////////////////////////////////////////////
            if ((Constants.Show_New_YK_switch2 / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_switch2 / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Maxswitch2 / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Maxswitch2 / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Minswitch2 / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Minswitch2 / 3000.0 * 150) <= 150)
            {
                m_label_Sw2Value.Height = (Constants.Show_New_YK_switch2 / 3000.0 * 150);
                Canvas.SetTop(m_label_Sw2Value, (150 - m_label_Sw2Value.Height));//roll中的最大值显示
                Canvas.SetTop(m_lable_sw2Max, (150 - Constants.Show_New_YK_Maxswitch2 / 3000.0 * 150));//roll中的最大值显示
                Canvas.SetTop(m_lable_sw2Min, (150 - Constants.Show_New_YK_Minswitch2 / 3000.0 * 150));//roll中的最小值显示
            }
            //////////////////////////sw3/////////////////////////////////////////////////
            if ((Constants.Show_New_YK_switch3 / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_switch3 / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Maxswitch3 / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Maxswitch3 / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Minswitch3 / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Minswitch3 / 3000.0 * 150) <= 150)
            {
                m_label_Sw3Value.Height = (Constants.Show_New_YK_switch3 / 3000.0 * 150);
                Canvas.SetTop(m_label_Sw3Value, (150 - m_label_Sw3Value.Height));//roll中的最大值显示
                Canvas.SetTop(m_lable_sw3Max, (150 - Constants.Show_New_YK_Maxswitch3 / 3000.0 * 150));//roll中的最大值显示
                Canvas.SetTop(m_lable_sw3Min, (150 - Constants.Show_New_YK_Minswitch3 / 3000.0 * 150));//roll中的最小值显示
            }
            //////////////////////////sw4///////////////////////////////////////////////////
            if ((Constants.Show_New_YK_switch4 / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_switch4 / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Maxswitch4 / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Maxswitch4 / 3000.0 * 150) <= 150
                && (Constants.Show_New_YK_Minswitch4 / 3000.0 * 150) >= 0 && (Constants.Show_New_YK_Minswitch4 / 3000.0 * 150) <= 150)
            {
                m_label_Sw4Value.Height = (Constants.Show_New_YK_switch4 / 3000.0 * 150);
                Canvas.SetTop(m_label_Sw4Value, (150 - m_label_Sw4Value.Height));//roll中的最大值显示
                Canvas.SetTop(m_lable_sw4Max, (150 - Constants.Show_New_YK_Maxswitch4 / 3000.0 * 150));//roll中的最大值显示
                Canvas.SetTop(m_lable_sw4Min, (150 - Constants.Show_New_YK_Minswitch4 / 3000.0 * 150));//roll中的最小值显示
            }
            RollProp_Value.Content = Constants.Show_New_YK_roll;
            PitchProp_Value.Content = Constants.Show_New_YK_pitch;
            YawProp_Value.Content = Constants.Show_New_YK_yaw;
            ThroProp_Value.Content = Constants.Show_New_YK_throttle;
            SW1Prop_Value.Content = Constants.Show_New_YK_switch1;
            SW2Prop_Value.Content = Constants.Show_New_YK_switch2;
            SW3Prop_Value.Content = Constants.Show_New_YK_switch3;
            SW4Prop_Value.Content = Constants.Show_New_YK_switch4;
        }
        public void Function_AllStateShow(float AutoWingVoltValue, int AutoWingStarNum, int AutoWingSensorState, float Set_Pensa_Banjing, float Set_Pensa_Gaodu, float Set_Pensa_Sudu, float Set_Pensa_Juli, int Attitute_State, double Mag_Angle,int model_feixing)
        {
            byte High_Byte = (byte)((Constants.Show_DataSelect_QUADE & 0x0000ff00) >> 8);//接收到的高8位
            byte Low_Byte = (byte)(Constants.Show_DataSelect_QUADE & 0x000000ff);//接收到的低8位
            int Flight_Style_Num = High_Byte;//机型选择编码1开始的
            int YKQType_Num = (int)(Low_Byte & 0x01);//遥控器类型0开始的
            int Imu_Check_Acc = 0;
            int Imu_Check_Gyro = 0;
            int Imu_Check_Mag = 0;
            Label_Volt.Content = AutoWingVoltValue.ToString() + "V";//电压显示
            Label_Star.Content = AutoWingStarNum.ToString(); //星数显示
            Imu_Check_Acc = (AutoWingSensorState >> 6) & 0x0003;
            Imu_Check_Gyro = (AutoWingSensorState >> 8) & 0x0003;
            Imu_Check_Mag = (AutoWingSensorState) & 0x0003;
            if (Attitute_State == 1 || Attitute_State == 3)
            {
                Label_Att.Foreground = new SolidColorBrush(Colors.Red);
                Label_Att.Content = "Abnormal";
            }
            else
            {
                Label_Att.Foreground = new SolidColorBrush(Colors.White);
                Label_Att.Content = "Normal";
            }
            if (Imu_Check_Acc == 1 || Imu_Check_Acc == 3)
            {
                Label_Acc.Foreground = new SolidColorBrush(Colors.Red);
                Label_Acc.Content = "Abnormal";
                Acc_Noise.Foreground = new SolidColorBrush(Colors.Red);
                Acc_Noise.Content = "Abnormal";
            }
            else
            {
                Label_Acc.Foreground = new SolidColorBrush(Colors.White);
                Label_Acc.Content = "Normal";
                ///////////////////////////////正常显示实际的噪音值//////////////////////////////
                Acc_Noise.Foreground = new SolidColorBrush(Colors.White);
                Acc_Noise.Content = Constants.Acc_Noise_ValueShow.ToString("#0.0");
            }
            if (Imu_Check_Mag == 1 || Imu_Check_Mag == 3)
            {
                Label_Mag.Foreground = new SolidColorBrush(Colors.Red);
                Label_Mag.Content = "Abnormal";
            }
            else
            {
                //     Label_Mag.Foreground = new SolidColorBrush(Colors.White);
                //     Label_Mag.Content = "Normal";
                Label_Mag.Foreground = new SolidColorBrush(Colors.White);
                Label_Mag.Content = Mag_Angle.ToString("#0.0");
                //////////////显示正常的角度/////////////////////////////

            }
            if (Imu_Check_Gyro == 1 || Imu_Check_Gyro == 3)
            {
                Label_Gyro.Foreground = new SolidColorBrush(Colors.Red);
                Label_Gyro.Content = "Abnormal";
            }
            else
            {
                Label_Gyro.Foreground = new SolidColorBrush(Colors.White);
                Label_Gyro.Content = "Normal";
            }
            Label_Range.Content = Set_Pensa_Banjing.ToString() + "M";//幅值

            if (model_feixing == 0)//当前模式是植保的话将高度设置
            {
                fuzhistatic_height.Height = 28;
                fuzhiline_height.Visibility = Visibility.Visible;
                duanyaostatic_height.Height = 28;
                duanyaoline_height.Visibility = Visibility.Visible;
            }
            else if (model_feixing == 1)//当前模式是航拍的话将高度置0
            {
                fuzhistatic_height.Height = 0;
                fuzhiline_height.Visibility = Visibility.Hidden;
                duanyaostatic_height.Height = 0;
                duanyaoline_height.Visibility = Visibility.Hidden;
            }


            Label_LimitHeight.Content = Set_Pensa_Gaodu.ToString() + "M";//限高

            if (model_feixing==0)
            {
                Label_LimitVel.Content = Set_Pensa_Sudu.ToString() + "M/S"; //植保巡航速度
            }
            else if (model_feixing==1)
            {
                Label_LimitVel.Content = Constants.Show_UAV_HangPaiSpeed.ToString() + "M/S"; //航拍巡航速度
            }

            Label_LimitDistance.Content = Set_Pensa_Juli.ToString() + "M";//距离
            switch (Flight_Style_Num)//机型相关的
            {
                case 0://默认情况把值赋值为2
                    Label_Flight.Content = "Null";
                    break;
                case 1://4叉字顺时针
                    Label_Flight.Content = "FourForkCloc";
                    break;
                case 2://4叉字逆时针
                    Label_Flight.Content = "FourForkAnti";
                    break;
                case 3://4十字顺时针
                    Label_Flight.Content = "FourCrossCloc";
                    break;
                case 4://4十字逆时针
                    Label_Flight.Content = "FourCrossAnti";
                    break;
                case 5://6叉字顺时针
                    Label_Flight.Content = "SixForkCloc";
                    break;
                case 6://6叉字逆时针
                    Label_Flight.Content = "SixForkAnti";
                    break;
                case 7://6十字顺时针
                    Label_Flight.Content = "SixCrossCloc";
                    break;
                case 8://6十字逆时针
                    Label_Flight.Content = "SixCrossAnti";
                    break;
                case 9://8V字顺时针
                    Label_Flight.Content = "EightForkCloc";
                    break;
                case 10://8V字逆时针
                    Label_Flight.Content = "EightForkAnti";
                    break;
                case 11://8一字顺时针
                    Label_Flight.Content = "EightCrossCloc";
                    break;
                case 12://8一字逆时针
                    Label_Flight.Content = "EightCrossAnti";
                    break;
                case 13://共轴叉字顺时针
                    Label_Flight.Content = "CoaxialForkCloc";
                    break;
                case 14://共轴叉字逆时针
                    Label_Flight.Content = "CoaxialForkAnti";
                    break;
                default:
                    break;
            }
            if (Constants.Prop_Failure_Code == 0)
            {
                switch (YKQType_Num)//遥控器类型
                {
                    case 0://14SG
                        Label_Ykq.Content = "14SG";
                        break;
                    case 1://天地飞
                        Label_Ykq.Content = "Others";
                        break;
                    default:
                        break;
                }
            }
            else
            {
                switch (Constants.Prop_Failure_Code)//遥控器解锁错误代码
                {
                    case 1://1:中点不准或未连接
                        Label_Ykq.Content = "Middle Point Not Correct";
                        break;
                    case 2://2:开关D不在最小处
                        Label_Ykq.Content = "SwitchD Not On Lowest";
                        break;
                    case 3:// 3:开关C不在最小处
                        Label_Ykq.Content = "SwitchC Not On Lowest";
                        break;
                    case 4:// 4:开关B不在最小处
                        Label_Ykq.Content = "SwitchB Not On Lowest";
                        break;
                    case 5://5:GPS星数不足
                        Label_Ykq.Content = "Stars Not Enough";
                        break;
                    case 6://6:Roll轴反向
                        Label_Ykq.Content = "Roll Reverse";
                        break;
                    case 7://7:Roll轴不准
                        Label_Ykq.Content = "Roll Not Correct";
                        break;
                    case 8://8:Pitch轴反向
                        Label_Ykq.Content = "Pitch Reverse";
                        break;
                    case 9:// 9:Pitch轴不准
                        Label_Ykq.Content = "Pitch Not Correct";
                        break;
                    case 10://10:Throttle轴反向
                        Label_Ykq.Content = "Throttle Reverse";
                        break;
                    case 11://11:Throttle轴不准
                        Label_Ykq.Content = "Throttle Not Correct";
                        break;
                    case 12://12:Yaw轴反向
                        Label_Ykq.Content = "Yaw Reverse";
                        break;
                    case 13://13:Yaw轴不准
                        Label_Ykq.Content = "Yaw Not Correct";
                        break;
                    case 14://14:未知错误
                        Label_Ykq.Content = "UnKnown";
                        break;
                    default:
                        break;
                }
            }
/////////////////新添加的部分显示////////////////////////////////////////////////
            FirstLevel_volt.Content = Constants.Show_UAV_Noticefirstvolt.ToString("#0.00") + "V";
            SecondLevel_volt.Content = Constants.Show_UAV_Noticesecondvolt.ToString("#0.00") + "V";
            switch ((int)Constants.Show_UAV_Noticesecondaction)//二级报警动作
            {
                case 0://无动作
                    SecondLevel_Action.Content = "Null";
                    break;
                case 1://返航
                    SecondLevel_Action.Content = "Reback";
                    break;
                case 2://悬停
                    SecondLevel_Action.Content = "Hover";
                    break;
                case 3://迫降
                    SecondLevel_Action.Content = "Land";
                    break;
                default:
                    break;
            }
            Reback_Height.Content = Constants.Show_UAV_Noticefanhanggaodu.ToString("#0.0") + "M";
           switch ((int)Constants.Show_UAV_Noticelosecontrolaction)//失控保护动作
           {
               case 0://无动作
                   LoseControl_Action.Content = "Null";
                   break;
               case 1://返航
                   LoseControl_Action.Content = "Reback";
                   break;
               case 2://悬停
                   LoseControl_Action.Content = "Hover";
                   break;
               case 3://迫降
                   LoseControl_Action.Content = "Land";
                   break;
               default:
                   break;
           }
           Hand_Speed.Content = Constants.Show_UAV_Speed.ToString("#0.0") + "M/S";
           AutoFly_Height.Content = Constants.Show_UAV_Height.ToString("#0.0") + "M";
           MaxUp_Speed.Content = Constants.Show_UAV_UpSpeed.ToString("#0.0") + "M/S";
           MaxDown_Speed.Content = Constants.Show_UAV_DownSpeed.ToString("#0.0") + "M/S";
           Attitude_AngleLimit.Content = Constants.Show_UAV_AttitudeRange.ToString("#0") + "°";
           MaxYaw_Velocity.Content = Constants.Show_UAV_Maxhangxiangjiaosudu.ToString("#0") + "°/s";
           switch ((int)Constants.Show_shuiben_duanyao_action)//断药动作
           {
               case 0://无动作
                   Break_Action.Content = "Null";
                   break;
               case 1://返航
                   Break_Action.Content = "Reback";
                   break;
               case 2://悬停
                   Break_Action.Content = "Hover";
                   break;
               default:
                   break;
           }
            if(model_feixing==0)
            {
                CurrentFlight_Model.Content = "Plant Model";
            }
            else if (model_feixing==1)
            {
                CurrentFlight_Model.Content = "Aerial Model";
            }
            else
            {
                CurrentFlight_Model.Content = "Null";
            }
            

        }
        private void Function_Combox_init()
        {
            m_YesOrNo.Items.Add("Yes");
            m_YesOrNo.Items.Add("No");
            m_YesOrNo.SelectedIndex = 0;
            M_Lowvolt_Action.Items.Add("Null");
            M_Lowvolt_Action.Items.Add("Reback");
            M_Lowvolt_Action.Items.Add("Hover");
            M_Lowvolt_Action.Items.Add("Land");
            M_Lowvolt_Action.SelectedIndex = 0;
            M_Losecontrol_Action.Items.Add("Null");
            M_Losecontrol_Action.Items.Add("Reback");
            M_Losecontrol_Action.Items.Add("Hover");
            M_Losecontrol_Action.Items.Add("Land");
            M_Losecontrol_Action.SelectedIndex = 0;
            M_Duanyao_Action.Items.Add("Null");
            M_Duanyao_Action.Items.Add("Reback");
            M_Duanyao_Action.Items.Add("Hover");
            M_Duanyao_Action.SelectedIndex = 0;
            m_TerrainFollowYesOrNo.Items.Add("No");
            m_TerrainFollowYesOrNo.Items.Add("Yes");
            m_TerrainFollowYesOrNo.SelectedIndex = 0;
        }
        private void Button_Click_IndexSet(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 2;//参数设置界面
            Constants.Jump_To5or50Hz_Flag = 0;
            Function_WorkIndex_Show();
        }

        private void Button_Click_FlightSet(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 3;//机型电池灯光等等界面
            Constants.Jump_To5or50Hz_Flag = 0;
        }

        private void Button_Click_PIDSet(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 4;//PID设置界面
            Constants.Jump_To5or50Hz_Flag = 1;
        }

        private void Button_Click_DatePlayBack(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 5;//灯管选择界面
        }
        private void Button_Click_YKCHECKSet(object sender, RoutedEventArgs e)
        {
            Constants.AutoWing_Tel_Control_flag = 0;//遥控器校准开始标志
            Constants.AutoWing_Tel_Finish_flag = 0;//遥控器校准完成进入修正界面
            Constants.AutoWing_Tel_Modifymiddle_Flag = 0;//遥控器中点修正标志
            Constants.AutoWing_Tel_Modifynext_Flag = 0;//遥控器中点修正界面下一步
            Constants.AutoWing_Tel_Makesure_flag = 0;//遥控器校准方向确定标志
            m_TabControl_Title.SelectedIndex = 9;//遥控器校准界面
        }
        private void Button_Click_JiXingPic(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 6;//机型图片选择
            this.PhotoCanvas.Index = Ay_UsedFlightModel_Index;
            Function_List_FlightModel(Ay_UsedFlightModel_Index);
        }

        private void Button_Click_MotorTest(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 11;//点击测试
        }

        private void CanvasWithPhoto_IndexChanged(object sender, RoutedPropertyChangedEventArgs<int> e)
        {
            CanvasWithPhoto c = (CanvasWithPhoto)sender;
            Function_List_FlightModel(c.Index);
            Ay_FlightModel_Index = c.Index;
            // UpdataRadioBtn(c.Index);
        }
        private void Function_List_FlightModel(int Flight_Index)
        {
            switch (Flight_Index)//机型
            {
                case 1://四轴叉字顺
                    Btn_Flight_Model.Content = "FourForkCloc";
                    break;
                case 2://四轴叉字逆
                    Btn_Flight_Model.Content = "FourForkAnti";
                    break;
                case 3://四轴十字顺
                    Btn_Flight_Model.Content = "FourCrossCloc";
                    break;
                case 4://四轴十字逆
                    Btn_Flight_Model.Content = "FourCrossAnti";
                    break;
                case 5://六轴叉字顺
                    Btn_Flight_Model.Content = "SixForkCloc";
                    break;
                case 6://六轴叉字逆
                    Btn_Flight_Model.Content = "SixForkAnti";
                    break;
                case 7://六轴十字顺
                    Btn_Flight_Model.Content = "SixCrossCloc";
                    break;
                case 8://六轴十字逆
                    Btn_Flight_Model.Content = "SixCrossAnti";
                    break;
                case 9://八轴叉字顺
                    Btn_Flight_Model.Content = "EightForkCloc";
                    break;
                case 10://八轴叉字逆
                    Btn_Flight_Model.Content = "EightForkAnti";
                    break;
                case 11://八轴十字顺
                    Btn_Flight_Model.Content = "EightCrossCloc";
                    break;
                case 12://八轴十字逆
                    Btn_Flight_Model.Content = "EightCrossAnti";
                    break;
                case 13://共轴叉字顺
                    Btn_Flight_Model.Content = "CoaxialForkCloc";
                    break;
                case 14://共轴叉字逆
                    Btn_Flight_Model.Content = "CoaxialForkAnti";
                    break;
                default:
                    break;
            }
        }

        private void Btn_Flight_Model_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 3;//机型电池灯光等等界面
            Ay_UsedFlightModel_Index = Ay_FlightModel_Index;
            switch (Ay_FlightModel_Index)//机型
            {
                case 1://四轴叉字顺
                    Btn_FlightModel_Choose.Content = "FourForkCloc";
                    break;
                case 2://四轴叉字逆
                    Btn_FlightModel_Choose.Content = "FourForkAnti";
                    break;
                case 3://四轴十字顺
                    Btn_FlightModel_Choose.Content = "FourCrossCloc";
                    break;
                case 4://四轴十字逆
                    Btn_FlightModel_Choose.Content = "FourCrossAnti";
                    break;
                case 5://六轴叉字顺
                    Btn_FlightModel_Choose.Content = "SixForkCloc";
                    break;
                case 6://六轴叉字逆
                    Btn_FlightModel_Choose.Content = "SixForkAnti";
                    break;
                case 7://六轴十字顺
                    Btn_FlightModel_Choose.Content = "SixCrossCloc";
                    break;
                case 8://六轴十字逆
                    Btn_FlightModel_Choose.Content = "SixCrossAnti";
                    break;
                case 9://八轴叉字顺
                    Btn_FlightModel_Choose.Content = "EightForkCloc";
                    break;
                case 10://八轴叉字逆
                    Btn_FlightModel_Choose.Content = "EightForkAnti";
                    break;
                case 11://八轴十字顺
                    Btn_FlightModel_Choose.Content = "EightCrossCloc";
                    break;
                case 12://八轴十字逆
                    Btn_FlightModel_Choose.Content = "EightCrossAnti";
                    break;
                case 13://共轴叉字顺
                    Btn_FlightModel_Choose.Content = "CoaxialForkCloc";
                    break;
                case 14://共轴叉字逆
                    Btn_FlightModel_Choose.Content = "CoaxialForkAnti";
                    break;
                default:
                    break;
            }
        }

        private void Btn_BatteryModel_Choose_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 7;//电池图片选择
            this.BatteryPhotoCanvas.Index = (Ay_UsedBatteryModel_Index + 1);
            Function_List_BatteryModel((Ay_UsedBatteryModel_Index + 1));
        }
        private void Function_List_BatteryModel(int Battery_Index)
        {
            switch (Battery_Index)//机型
            {
                case 1://6s
                    Btn_Battery_Model.Content = "6S";
                    break;
                case 2://12s
                    Btn_Battery_Model.Content = "12S";
                    break;
                case 3://4s
                    Btn_Battery_Model.Content = "4S";
                    break;
                case 4://3s
                    Btn_Battery_Model.Content = "3S";
                    break;
                default:
                    break;
            }
        }

        private void BatteryCanvasWithPhoto_IndexChanged(object sender, RoutedPropertyChangedEventArgs<int> e)
        {
            CanvasWithPhoto c = (CanvasWithPhoto)sender;
            Function_List_BatteryModel(c.Index);
            Ay_BatteryModel_Index = c.Index;
        }

        private void Btn_Battery_Model_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 3;//机型电池灯光等等界面
            Ay_UsedBatteryModel_Index = (Ay_BatteryModel_Index - 1);
            switch ((Ay_BatteryModel_Index))//机型
            {
                case 1://6s
                    Btn_BatteryModel_Choose.Content = "6S";
                    break;
                case 2://12S
                    Btn_BatteryModel_Choose.Content = "12S";
                    break;
                case 3://4S
                    Btn_BatteryModel_Choose.Content = "4S";
                    break;
                case 4://3S
                    Btn_BatteryModel_Choose.Content = "3S";
                    break;
                default:
                    break;
            }
        }
        private void TelCanvasWithPhoto_IndexChanged(object sender, RoutedPropertyChangedEventArgs<int> e)
        {
            CanvasWithPhoto c = (CanvasWithPhoto)sender;
            Function_List_TelModel(c.Index);
            Ay_TelModel_Index = c.Index;
        }

        private void Btn_TelModel_Choose_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 8;//遥控图片选择
            this.TelPhotoCanvas.Index = (Ay_UsedTelModel_Index + 1);
            Function_List_TelModel((Ay_UsedTelModel_Index + 1));
        }

        private void Btn_Tel_Model_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 3;//机型电池灯光等等界面
            Ay_UsedTelModel_Index = (Ay_TelModel_Index - 1);
            switch ((Ay_TelModel_Index))//机型
            {
                case 1://14SG
                    Btn_TelModel_Choose.Content = "14SG";
                    break;
                case 2://其它
                    Btn_TelModel_Choose.Content = "Other";
                    break;
                default:
                    break;
            }
        }
        private void Function_List_TelModel(int Tel_Index)
        {
            switch (Tel_Index)//机型
            {
                case 1://14SG
                    Btn_Tel_Model.Content = "14SG";
                    break;
                case 2://其它
                    Btn_Tel_Model.Content = "Other";
                    break;
                default:
                    break;
            }
        }

        private void m_YesOrNo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Ay_UsedIfBack_Index = m_YesOrNo.SelectedIndex;
        }

        private void Idle_Speed_Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            m_idle_value.Content = Idle_Speed_Slider.Value.ToString() + "%";
            Ay_UsedIdleSpeed_Index = (int)Idle_Speed_Slider.Value;
        }
        /// <summary>
        /// //作业参数获取
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Btn_WorkIndex_Get_Click(object sender, RoutedEventArgs e)
        {
            Function_WorkIndex_Show();
        }
        private void Function_WorkIndex_Show()
        {
            Edit_Fly_Speed.Text = Constants.Show_UAV_Speed.ToString("#0.0");
            Edit_Fly_Height.Text = Constants.Show_UAV_Height.ToString("#0.0");
            Edit_Up_Speed.Text = Constants.Show_UAV_UpSpeed.ToString("#0.0");
            Edit_Down_Speed.Text = Constants.Show_UAV_DownSpeed.ToString("#0.0");
            Edit_Attitude_Value.Text = Constants.Show_UAV_AttitudeRange.ToString("#0.0");
            Edit_Maxrad_Value.Text = Constants.Show_UAV_Maxhangxiangjiaosudu.ToString("#0.0");
        }
        private void JuliCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            System.Windows.Controls.CheckBox chk = (System.Windows.Controls.CheckBox)sender;
            if (chk.IsChecked == true)
            {
                //         Edit_Work_Distance.IsReadOnly = false;
            }
        }
        private void JuliUncheckBox_UnChecked(object sender, RoutedEventArgs e)
        {
            System.Windows.Controls.CheckBox chk = (System.Windows.Controls.CheckBox)sender;
            if (chk.IsChecked == false)
            {
                //       Edit_Work_Distance.IsReadOnly = true;
                //       Edit_Work_Distance.Text = "0.0";
            }
        }
        private void GaoduCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            System.Windows.Controls.CheckBox chk = (System.Windows.Controls.CheckBox)sender;
            if (chk.IsChecked == true)
            {
                //      Edit_Work_Height.IsReadOnly = false;
            }
        }
        private void GaoduUncheckBox_UnChecked(object sender, RoutedEventArgs e)
        {
            System.Windows.Controls.CheckBox chk = (System.Windows.Controls.CheckBox)sender;
            if (chk.IsChecked == false)
            {
                //    Edit_Work_Height.IsReadOnly = true;
                //    Edit_Work_Height.Text = "0.0";
            }
        }
        private void Function_WorkIndexSet_Init()
        {
            //     Edit_Work_Distance.IsReadOnly = true;
            //     Edit_Work_Height.IsReadOnly = true;
            //     Edit_Work_Distance.Text = "0.0";
            //    Edit_Work_Height.Text = "0.0";
        }
        void Function_Reback_ShowInfo()
        {
            byte High_Byte = (byte)((Constants.Show_DataSelect_QUADE & 0x0000ff00) >> 8);//接收到的高8位
            byte Low_Byte = (byte)(Constants.Show_DataSelect_QUADE & 0x000000ff);//接收到的低8位
            int Flight_Style_Num = High_Byte;//机型选择编码1开始的
            int YKQType_Num = (int)(Low_Byte & 0x01);//遥控器类型0开始的
            int Battery_Num = (int)((Low_Byte & 0x06) >> 1);//电池类型0开始的
            int IfReback_Num = (int)((Low_Byte & 0x80) >> 7);//是否回中0开始的
            switch (Flight_Style_Num)//机型相关的
            {
                case 0://默认情况把值赋值为2
                    Ay_UsedFlightModel_Index = 2;
                    Btn_FlightModel_Choose.Content = "FourForkAnti";
                    break;
                case 1://4叉字顺时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "FourForkCloc";
                    break;
                case 2://4叉字逆时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "FourForkAnti";
                    break;
                case 3://4十字顺时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "FourCrossCloc";
                    break;
                case 4://4十字逆时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "FourCrossAnti";
                    break;
                case 5://6叉字顺时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "SixForkCloc";
                    break;
                case 6://6叉字逆时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "SixForkAnti";
                    break;
                case 7://6十字顺时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "SixCrossCloc";
                    break;
                case 8://6十字逆时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "SixCrossAnti";
                    break;
                case 9://8V字顺时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "EightForkCloc";
                    break;
                case 10://8V字逆时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "EightForkAnti";
                    break;
                case 11://8一字顺时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "EightCrossCloc";
                    break;
                case 12://8一字逆时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "EightCrossAnti";
                    break;
                case 13://共轴叉字顺时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "CoaxialForkCloc";
                    break;
                case 14://共轴叉字逆时针
                    Ay_UsedFlightModel_Index = Flight_Style_Num;
                    Btn_FlightModel_Choose.Content = "CoaxialForkAnti";
                    break;
                default:
                    break;
            }
            switch (YKQType_Num)//遥控器类型
            {
                case 0://14SG
                    Ay_UsedTelModel_Index = YKQType_Num;
                    Btn_TelModel_Choose.Content = "14SG";
                    break;
                case 1://天地飞
                    Ay_UsedTelModel_Index = YKQType_Num;
                    Btn_TelModel_Choose.Content = "Other";
                    break;
                default:
                    break;
            }
            switch (Battery_Num)//电池类型
            {
                case 0://6s电池
                    Ay_UsedBatteryModel_Index = Battery_Num;
                    Btn_BatteryModel_Choose.Content = "6S";
                    break;
                case 1://12s电池
                    Ay_UsedBatteryModel_Index = Battery_Num;
                    Btn_BatteryModel_Choose.Content = "12S";
                    break;
                case 2://4s电池
                    Ay_UsedBatteryModel_Index = Battery_Num;
                    Btn_BatteryModel_Choose.Content = "4S";
                    break;
                case 3://3s电池
                    Ay_UsedBatteryModel_Index = Battery_Num;
                    Btn_BatteryModel_Choose.Content = "3S";
                    break;
                default:
                    break;
            }
            switch (IfReback_Num)//是否回中
            {
                case 0://回中
                    m_YesOrNo.SelectedIndex = IfReback_Num;
                    Ay_UsedIfBack_Index = IfReback_Num;
                    break;
                case 1://不回中
                    m_YesOrNo.SelectedIndex = IfReback_Num;
                    Ay_UsedIfBack_Index = IfReback_Num;
                    break;
                default:
                    break;
            }
            m_idle_value.Content = Constants.Show_tape_speedvalue.ToString() + "%";
            Idle_Speed_Slider.Value = Constants.Show_tape_speedvalue;
        }

        private void Btn_PIDIndex_Get_Click(object sender, RoutedEventArgs e)
        {
            Edit_GyproKp_Rp.Text = Constants.Show_gyro_kp_rp.ToString("#0.00");
            Edit_GyproKi_Rp.Text = Constants.Show_gyro_ki_rp.ToString("#0.00");
            Edit_GyproKd_Rp.Text = Constants.Show_gyro_kd_rp.ToString("#0.00");
            Edit_GyproKp_Y.Text = Constants.Show_gyro_kp_y.ToString("#0.00");
            Edit_GyproKi_Y.Text = Constants.Show_gyro_ki_y.ToString("#0.00");
            Edit_GyproKd_Y.Text = Constants.Show_gyro_kd_y.ToString("#0.00");
            Edit_PropKp_Rp.Text = Constants.Show_prop_kp_rp.ToString("#0.00");
            Edit_PropKi_Rp.Text = Constants.Show_prop_ki_rp.ToString("#0.00");
            Edit_PropKp_Y.Text = Constants.Show_prop_kp_y.ToString("#0.00");
            Edit_PropKi_Y.Text = Constants.Show_prop_ki_y.ToString("#0.00");

            if (Constants.Show_gyro_kp_rp <= 50)
            {
                slider_ziwen_pid.Value = 0;
                label_ziwen_pid.Content = "0%";
            }
            if (Constants.Show_gyro_kp_rp > 50 && Constants.Show_gyro_kp_rp <= 58)
            {
                slider_ziwen_pid.Value = 1;
                label_ziwen_pid.Content = "10%";
            }
            if (Constants.Show_gyro_kp_rp > 58 && Constants.Show_gyro_kp_rp <= 66)
            {
                slider_ziwen_pid.Value = 2;
                label_ziwen_pid.Content = "20%";
            }
            if (Constants.Show_gyro_kp_rp > 66 && Constants.Show_gyro_kp_rp <= 74)
            {
                slider_ziwen_pid.Value = 3;
                label_ziwen_pid.Content = "30%";
            }
            if (Constants.Show_gyro_kp_rp > 74 && Constants.Show_gyro_kp_rp <= 82)
            {
                slider_ziwen_pid.Value = 4;
                label_ziwen_pid.Content = "40%";
            }
            if (Constants.Show_gyro_kp_rp > 82 && Constants.Show_gyro_kp_rp <= 90)
            {
                slider_ziwen_pid.Value = 5;
                label_ziwen_pid.Content = "50%";
            }
            if (Constants.Show_gyro_kp_rp > 90 && Constants.Show_gyro_kp_rp <= 98)
            {
                slider_ziwen_pid.Value = 6;
                label_ziwen_pid.Content = "60%";
            }
            if (Constants.Show_gyro_kp_rp > 98 && Constants.Show_gyro_kp_rp <= 106)
            {
                slider_ziwen_pid.Value = 7;
                label_ziwen_pid.Content = "70%";
            }
            if (Constants.Show_gyro_kp_rp > 106 && Constants.Show_gyro_kp_rp <= 114)
            {
                slider_ziwen_pid.Value = 8;
                label_ziwen_pid.Content = "80%";
            }
            if (Constants.Show_gyro_kp_rp > 114 && Constants.Show_gyro_kp_rp <= 122)
            {
                slider_ziwen_pid.Value = 9;
                label_ziwen_pid.Content = "90%";
            }
            if (Constants.Show_gyro_kp_rp > 122)
            {
                slider_ziwen_pid.Value = 10;
                label_ziwen_pid.Content = "100%";
            }

        }

        private void Btn_WorkIndex_Set_Click(object sender, RoutedEventArgs e)
        {
            if (Edit_Fly_Speed.Text == "")
            {
                System.Windows.MessageBox.Show("Work Speed Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_Speed = (float)Convert.ToDouble(Edit_Fly_Speed.Text);//作业速度
            }
            if (Constants.Set_UAV_Speed < 1 || Constants.Set_UAV_Speed > 25)
            {
                System.Windows.MessageBox.Show("Speed Is Between 1m/s And 25m/s");
                return;
            }
            if (Edit_Fly_Height.Text == "")
            {
                System.Windows.MessageBox.Show("Take Off Height Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_Height = (float)Convert.ToDouble(Edit_Fly_Height.Text);//自主起飞高度
            }
            if (Constants.Set_UAV_Height < 1 || Constants.Set_UAV_Height > 10)
            {
                System.Windows.MessageBox.Show("Take Off Height Is Between 1m And 10m");
                return;
            }
            if (Edit_Up_Speed.Text == "")
            {
                System.Windows.MessageBox.Show("Max Up Speed Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_UpSpeed = (float)Convert.ToDouble(Edit_Up_Speed.Text);//最大上升速度
            }
            if (Constants.Set_UAV_UpSpeed < 0 || Constants.Set_UAV_UpSpeed > 6)
            {
                System.Windows.MessageBox.Show("Max Up Speed Is Between 0m/s And 6m/s");
                return;
            }
            if (Edit_Down_Speed.Text == "")
            {
                System.Windows.MessageBox.Show("Max Down Speed Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_DownSpeed = (float)Convert.ToDouble(Edit_Down_Speed.Text);//最大下降速度
            }
            if (Constants.Set_UAV_DownSpeed < 0 || Constants.Set_UAV_DownSpeed > 4)
            {
                System.Windows.MessageBox.Show("Max Down Speed Is Between 0m/s And 4m/s");
                return;
            }

            if (Edit_Attitude_Value.Text == "")
            {
                System.Windows.MessageBox.Show("Attitude Range Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_AttitudeRange = (float)Convert.ToDouble(Edit_Attitude_Value.Text);//姿态角幅值
            }
            if (Constants.Set_UAV_AttitudeRange < 10 || Constants.Set_UAV_AttitudeRange > 45)
            {
                System.Windows.MessageBox.Show("Attitude Range Is Between 10 And 45");
                return;
            }


            if (Edit_Maxrad_Value.Text == "")
            {
                System.Windows.MessageBox.Show("Max Yaw Angular Speed Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_Maxhangxiangjiaosudu = (float)Convert.ToDouble(Edit_Maxrad_Value.Text);//最大航向角速度
            }
            if (Constants.Set_UAV_Maxhangxiangjiaosudu < 10 || Constants.Set_UAV_Maxhangxiangjiaosudu > 120)
            {
                System.Windows.MessageBox.Show("Max Yaw Angular Speed Is Between 10 And 120");
                return;
            }
            Constants.AutoWing_WorkIndex_Flag = 1;//开启参数设置发送标志
            MainWindow.pWin.Function_Wait_ExternShow();
        }

        private void Btn_IndexSet_All_Click(object sender, RoutedEventArgs e)
        {
            Constants.Set_DataSelect_QUADE = (((short)Ay_UsedFlightModel_Index << 8) & 0xff00) | (((short)Ay_UsedBatteryModel_Index << 1) & 0x0006) | (((short)Ay_UsedTelModel_Index) & 0x0001) | (((short)Ay_UsedIfBack_Index << 7) & 0x0080);//数组要进行打包
            Constants.Set_tape_speedvalue = Ay_UsedIdleSpeed_Index;//怠速值
            Constants.AutoWing_ModelFlight_Flag = 1;//开启参数设置发送标志
            MainWindow.pWin.Function_Wait_ExternShow();
        }

        private void Button_PID_SETClick(object sender, RoutedEventArgs e)
        {
            if (Edit_GyproKp_Rp.Text == "" || Edit_GyproKi_Rp.Text == "" || Edit_GyproKd_Rp.Text == "" || Edit_GyproKp_Y.Text == "" || Edit_GyproKi_Y.Text == ""
                 || Edit_GyproKd_Y.Text == "" || Edit_PropKp_Rp.Text == "" || Edit_PropKi_Rp.Text == "" || Edit_PropKp_Y.Text == "" || Edit_PropKi_Y.Text == "")
            {
                System.Windows.MessageBox.Show("Please Enter Relevant Index");
                return;
            }
            else
            {
                Constants.Set_gyro_kp_rp = (float)Convert.ToDouble(Edit_GyproKp_Rp.Text);
                Constants.Set_gyro_ki_rp = (float)Convert.ToDouble(Edit_GyproKi_Rp.Text);
                Constants.Set_gyro_kd_rp = (float)Convert.ToDouble(Edit_GyproKd_Rp.Text);
                Constants.Set_gyro_kp_y = (float)Convert.ToDouble(Edit_GyproKp_Y.Text);
                Constants.Set_gyro_ki_y = (float)Convert.ToDouble(Edit_GyproKi_Y.Text);
                Constants.Set_gyro_kd_y = (float)Convert.ToDouble(Edit_GyproKd_Y.Text);
                Constants.Set_prop_kp_rp = (float)Convert.ToDouble(Edit_PropKp_Rp.Text);
                Constants.Set_prop_ki_rp = (float)Convert.ToDouble(Edit_PropKi_Rp.Text);
                Constants.Set_prop_kp_y = (float)Convert.ToDouble(Edit_PropKp_Y.Text);
                Constants.Set_prop_ki_y = (float)Convert.ToDouble(Edit_PropKi_Y.Text);
                Constants.AutoWing_PIDSet_Flag = 1;//开启PID设置发送标志
                MainWindow.pWin.Function_Wait_ExternShow();
            }
        }

        private void Grid_Unloaded(object sender, RoutedEventArgs e)
        {
            Constants.Jump_To5or50Hz_Flag = 0;
            Constants.AutoWing_Tel_Control_flag = 0;//遥控器校准开始标志
            Constants.AutoWing_Tel_Finish_flag = 0;//遥控器校准完成进入修正界面
            Constants.AutoWing_Tel_Modifymiddle_Flag = 0;//遥控器中点修正标志
            Constants.AutoWing_Tel_Modifynext_Flag = 0;//遥控器中点修正界面下一步
            Constants.AutoWing_Tel_Makesure_flag = 0;//遥控器校准方向确定标志
            if (Constants.Play_Data_Back_Flag == true)
            {
                Constants.Play_Data_Back_Flag = false;
                Constants.History_DataReader_Back.Close();
                Constants.History_DataPlay_Back.Close();
                Constants.History_DataPlay_Back.Dispose();
                Constants.Progres_DataCount = 0;
            }
        }

        private void YKJZ_Start_Click(object sender, RoutedEventArgs e)
        {
            MainWindow.pWin.Function_Wait_ExternShow();
            Constants.AutoWing_Tel_Steps = 1;//校准开始发送的step值是1
            Constants.AutoWing_Tel_StepsValue = 0;//正反方向为0这个时候是不考虑的
            Constants.AutoWing_Tel_Finish_flag = 0;//遥控器校准完成进入修正界面
            Constants.AutoWing_Tel_Modifymiddle_Flag = 0;//遥控器中点修正标志
            Constants.AutoWing_Tel_Modifynext_Flag = 0;//遥控器中点修正界面下一步
            Constants.AutoWing_Tel_Makesure_flag = 0;//遥控器校准方向确定标志
            Constants.AutoWing_Tel_Control_flag = 1;//开启遥控器校准发送标志是
        }

        private void YKJZ_Success_Click(object sender, RoutedEventArgs e)
        {
            //        m_TabControl_Title.SelectedIndex = 14;//中点修正界面
            MainWindow.pWin.Function_Wait_ExternShow();
            Constants.AutoWing_Tel_Steps = 3;//校准下一步发送的step值是3
            Constants.AutoWing_Tel_StepsValue = 0;//正反方向为0这个时候是不考虑的
            Constants.AutoWing_Tel_Control_flag = 0;//遥控器校准开始标志
            Constants.AutoWing_Tel_Modifymiddle_Flag = 0;//遥控器中点修正标志
            Constants.AutoWing_Tel_Modifynext_Flag = 0;//遥控器中点修正界面下一步
            Constants.AutoWing_Tel_Makesure_flag = 0;//遥控器校准方向确定标志
            Constants.AutoWing_Tel_Finish_flag = 1;//下一步进入中点修正界面
        }
        public void Jump_Ykq_Sure()
        {
            YKQ_Middle_Next.Content = "Cancel";
            m_TabControl_Title.SelectedIndex = 14;//中点修正界面
        }

        private void Ykq_Dire_Sure_Click(object sender, RoutedEventArgs e)
        {
            MainWindow.pWin.Function_Wait_ExternShow();
            Constants.AutoWing_Tel_Modifynext_Flag = 0;//将方向修改的发送部分停止
            Constants.AutoWing_Tel_Steps = 10;//校准下一步发送的
            Constants.AutoWing_Tel_Control_flag = 0;//遥控器校准开始标志
            Constants.AutoWing_Tel_Finish_flag = 0;//遥控器校准完成进入修正界面
            Constants.AutoWing_Tel_Modifymiddle_Flag = 0;//遥控器中点修正标志
            Constants.AutoWing_Tel_Modifynext_Flag = 0;//遥控器中点修正界面下一步
            Constants.AutoWing_Tel_Makesure_flag = 1;//开启遥控器方向确认发送标志
        }

        private void RadioButton_Checked(object sender, RoutedEventArgs e)
        {
            Which_Country_Flag = 0;//美国手

            /////////////////////显示为美国手的界面///////////////////////////
            try
            {
                //////////////////////清空所有通道的反向////////////////////////////////
                m_leftcheck_amercia_x.IsChecked = false;
                m_leftcheck_amercia_y.IsChecked = false;
                m_rightcheck_amercia_x.IsChecked = false;
                m_rightcheck_amercia_y.IsChecked = false;
                m_check_sw1.IsChecked = false;
                m_check_sw2.IsChecked = false;
                m_check_sw3.IsChecked = false;
                m_check_sw4.IsChecked = false;
                roll_if_reverse = 0;
                pitch_if_reverse = 0;
                yaw_if_reverse = 0;
                throttle_if_reverse = 0;
                switcha_reverse = 0;
                switchb_reverse = 0;
                switchc_reverse = 0;
                switchd_reverse = 0;
                m_leftcheck_amercia_x.Content = "YawReverse";
                m_leftcheck_amercia_y.Content = "ThrReverse";
                m_rightcheck_amercia_x.Content = "RollReverse";
                m_rightcheck_amercia_y.Content = "PitchReverse";
                Constants.AutoWing_Tel_StepsValue = 0;
            }
            catch (Exception)
            {

            }

        }

        private void RadioButton_Checked_1(object sender, RoutedEventArgs e)
        {
            Which_Country_Flag = 1;//日本手
            /////////////////////显示为日本手的界面///////////////////////////
            try
            {
                //////////////////////清空所有通道的反向////////////////////////////////
                m_leftcheck_amercia_x.IsChecked = false;
                m_leftcheck_amercia_y.IsChecked = false;
                m_rightcheck_amercia_x.IsChecked = false;
                m_rightcheck_amercia_y.IsChecked = false;
                m_check_sw1.IsChecked = false;
                m_check_sw2.IsChecked = false;
                m_check_sw3.IsChecked = false;
                m_check_sw4.IsChecked = false;
                roll_if_reverse = 0;
                pitch_if_reverse = 0;
                yaw_if_reverse = 0;
                throttle_if_reverse = 0;
                switcha_reverse = 0;
                switchb_reverse = 0;
                switchc_reverse = 0;
                switchd_reverse = 0;
                m_leftcheck_amercia_x.Content = "YawReverse";
                m_leftcheck_amercia_y.Content = "PitchReverse";
                m_rightcheck_amercia_x.Content = "RollReverse";
                m_rightcheck_amercia_y.Content = "ThrReverse";
                Constants.AutoWing_Tel_StepsValue = 0;
            }
            catch (Exception)
            {

            }

        }


        /// <summary>
        /// 遥控器左边遥感
        /// </summary>
        public void Function_YKQ_LEFT()
        {

            if (Which_Country_Flag == 0)
            {
                /****************************美国手左边横纵坐标**********************/
                if (Constants.Show_guiyi_YK_yaw >= -1 && Constants.Show_guiyi_YK_yaw <= 1 && Constants.Show_guiyi_YK_throttle >= -1 && Constants.Show_guiyi_YK_throttle <= 1)
                {
                    LeftCenter.X = 200 - (100 * (1 - Constants.Show_guiyi_YK_yaw)) - 4;
                    LeftCenter.Y = 100 - (100 * (Constants.Show_guiyi_YK_throttle + 1) - 100) - 4;
                }
                else
                {
                    LeftCenter.X = 100 - 4;
                    LeftCenter.Y = 100 - 4;
                }
                /***************************美国手开关*********************************/
                Function_YK_Switch();
            }
            if (Which_Country_Flag == 1)
            {
                /****************************日本手左边横纵坐标**********************/
                if (Constants.Show_guiyi_YK_yaw >= -1 && Constants.Show_guiyi_YK_yaw <= 1 && Constants.Show_guiyi_YK_pitch >= -1 && Constants.Show_guiyi_YK_pitch <= 1)
                {
                    LeftCenter.X = 200 - (100 * (1 - Constants.Show_guiyi_YK_yaw)) - 4;
                    LeftCenter.Y = 100 - (100 * (Constants.Show_guiyi_YK_pitch + 1) - 100) - 4;
                }
                else
                {
                    LeftCenter.X = 100 - 4;
                    LeftCenter.Y = 100 - 4;
                }
                /***************************日本手开关*********************************/
                Function_YK_Switch();
            }
            Canvas.SetLeft(m_circle, LeftCenter.X);
            Canvas.SetTop(m_circle, LeftCenter.Y);
        }
        /// <summary>
        /// 遥控器右边遥杆
        /// </summary>
        public void Function_YKQ_RIGHT()
        {

            if (Which_Country_Flag == 0)
            {
                /****************************美国手右边横纵坐标**********************/
                if (Constants.Show_guiyi_YK_roll >= -1 && Constants.Show_guiyi_YK_roll <= 1 && Constants.Show_guiyi_YK_pitch >= -1 && Constants.Show_guiyi_YK_pitch <= 1)
                {
                    RightCenter.X = 200 - (100 * (1 - Constants.Show_guiyi_YK_roll)) - 4;
                    RightCenter.Y = 100 - (100 * (Constants.Show_guiyi_YK_pitch + 1) - 100) - 4;
                }
                else
                {
                    RightCenter.X = 100 - 4;
                    RightCenter.Y = 100 - 4;
                }
            }
            if (Which_Country_Flag == 1)
            {
                /****************************日本手右边横纵坐标**********************/
                if (Constants.Show_guiyi_YK_roll >= -1 && Constants.Show_guiyi_YK_roll <= 1 && Constants.Show_guiyi_YK_throttle >= -1 && Constants.Show_guiyi_YK_throttle <= 1)
                {
                    RightCenter.X = 200 - (100 * (1 - Constants.Show_guiyi_YK_roll)) - 4;
                    RightCenter.Y = 100 - (100 * (Constants.Show_guiyi_YK_throttle + 1) - 100) - 4;
                }
                else
                {
                    RightCenter.X = 100 - 4;
                    RightCenter.Y = 100 - 4;
                }
            }
            Canvas.SetLeft(m_circletwo, RightCenter.X);
            Canvas.SetTop(m_circletwo, RightCenter.Y);
        }


        /// <summary>
        /// 遥控器中点修正左边摇杆区域
        /// </summary>
        public void Function_YKQ_MiddleModify_LEFT()
        {
            if (Which_Country_Flag == 0)
            {
                /****************************美国手左边横纵坐标**********************/
                if (Constants.Show_guiyi_YK_yaw >= -1 && Constants.Show_guiyi_YK_yaw <= 1 && Constants.Show_guiyi_YK_throttle >= -1 && Constants.Show_guiyi_YK_throttle <= 1)
                {
                    Middle_Modify_LeftCenter.X = 200 - (100 * (1 - Constants.Show_guiyi_YK_yaw)) - 4;
                    Middle_Modify_LeftCenter.Y = 100 - (100 * (Constants.Show_guiyi_YK_throttle + 1) - 100) - 4;
                }
                else
                {
                    Middle_Modify_LeftCenter.X = 100 - 4;
                    Middle_Modify_LeftCenter.Y = 100 - 4;
                }
            }
            if (Which_Country_Flag == 1)
            {
                /****************************日本手左边横纵坐标**********************/
                if (Constants.Show_guiyi_YK_yaw >= -1 && Constants.Show_guiyi_YK_yaw <= 1 && Constants.Show_guiyi_YK_pitch >= -1 && Constants.Show_guiyi_YK_pitch <= 1)
                {
                    Middle_Modify_LeftCenter.X = 200 - (100 * (1 - Constants.Show_guiyi_YK_yaw)) - 4;
                    Middle_Modify_LeftCenter.Y = 100 - (100 * (Constants.Show_guiyi_YK_pitch + 1) - 100) - 4;
                }
                else
                {
                    Middle_Modify_LeftCenter.X = 100 - 4;
                    Middle_Modify_LeftCenter.Y = 100 - 4;
                }
            }
            Canvas.SetLeft(m_modifycircle, Middle_Modify_LeftCenter.X);
            Canvas.SetTop(m_modifycircle, Middle_Modify_LeftCenter.Y);
        }
        /// <summary>
        /// 遥控器中点修正右边摇杆区域
        /// </summary>
        public void Function_YKQ_MiddleModify_RIGHT()
        {
            if (Which_Country_Flag == 0)
            {
                /****************************美国手右边横纵坐标**********************/
                if (Constants.Show_guiyi_YK_roll >= -1 && Constants.Show_guiyi_YK_roll <= 1 && Constants.Show_guiyi_YK_pitch >= -1 && Constants.Show_guiyi_YK_pitch <= 1)
                {
                    Middle_Modify_RightCenter.X = 200 - (100 * (1 - Constants.Show_guiyi_YK_roll)) - 4;
                    Middle_Modify_RightCenter.Y = 100 - (100 * (Constants.Show_guiyi_YK_pitch + 1) - 100) - 4;
                }
                else
                {
                    Middle_Modify_RightCenter.X = 100 - 4;
                    Middle_Modify_RightCenter.Y = 100 - 4;
                }
            }
            if (Which_Country_Flag == 1)
            {
                /****************************日本手右边横纵坐标**********************/
                if (Constants.Show_guiyi_YK_roll >= -1 && Constants.Show_guiyi_YK_roll <= 1 && Constants.Show_guiyi_YK_throttle >= -1 && Constants.Show_guiyi_YK_throttle <= 1)
                {
                    Middle_Modify_RightCenter.X = 200 - (100 * (1 - Constants.Show_guiyi_YK_roll)) - 4;
                    Middle_Modify_RightCenter.Y = 100 - (100 * (Constants.Show_guiyi_YK_throttle + 1) - 100) - 4;
                }
                else
                {
                    Middle_Modify_RightCenter.X = 100 - 4;
                    Middle_Modify_RightCenter.Y = 100 - 4;
                }
            }
            Canvas.SetLeft(m_modifycircletwo, Middle_Modify_RightCenter.X);
            Canvas.SetTop(m_modifycircletwo, Middle_Modify_RightCenter.Y);
        }
        /// <summary>
        /// /开关方向
        /// </summary>
        private void Function_YK_Switch()
        {
            if (Constants.Show_guiyi_YK_switch1 == 0 || Constants.Show_guiyi_YK_switch1 == 1)
            {
                Switch1.IsChecked = true;
            }
            else if (Constants.Show_guiyi_YK_switch1 == 2)
            {
                Switch1.IsChecked = false;
            }
            else
            {
                Switch1.IsChecked = true;
            }

            if (Constants.Show_guiyi_YK_switch2 == 0 || Constants.Show_guiyi_YK_switch2 == 1)
            {
                Switch2.IsChecked = true;
            }
            else if (Constants.Show_guiyi_YK_switch2 == 2)
            {
                Switch2.IsChecked = false;
            }
            else
            {
                Switch2.IsChecked = true;
            }

            if (Constants.Show_guiyi_YK_switch3 == 0 || Constants.Show_guiyi_YK_switch3 == 1)
            {
                Switch3.IsChecked = true;
            }
            else if (Constants.Show_guiyi_YK_switch3 == 2)
            {
                Switch3.IsChecked = false;
            }
            else
            {
                Switch3.IsChecked = true;
            }

            if (Constants.Show_guiyi_YK_switch4 == 0 || Constants.Show_guiyi_YK_switch4 == 1)
            {
                Switch4.IsChecked = true;
            }
            else if (Constants.Show_guiyi_YK_switch4 == 2)
            {
                Switch4.IsChecked = false;
            }
            else
            {
                Switch4.IsChecked = true;
            }

        }

        private void YKQ_BackButton_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 9;//遥控器校准界面
        }

        private void Insert_File_Click(object sender, RoutedEventArgs e)
        {
            if (Constants.Play_Data_Back_Flag == true)
            {
                if (System.Windows.Forms.MessageBox.Show("If Load The New Mission", "Tip", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.Yes)
                {
                    Constants.Play_Data_Back_Flag = false;
                    Constants.History_DataReader_Back.Close();
                    Constants.History_DataPlay_Back.Close();
                }
                else
                {
                    return;
                }

            }
            string file_types = "文件(*.dat)|*.dat;||";
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = file_types;
            dlg.InitialDirectory = "C:\\AutoWing\\";
            dlg.Multiselect = false;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                Insert_File_Name = dlg.FileName;
                if (!File.Exists(Insert_File_Name))
                {
                    System.Windows.MessageBox.Show("File Is Not Exist");
                    return;
                }
                File_Path_Name.Text = Insert_File_Name;
                Constants.History_DataPlay_Back = new FileStream(Insert_File_Name, FileMode.Open, FileAccess.Read);
            }
        }
        private void Data_Play_Back_Click(object sender, RoutedEventArgs e)
        {
            if (Constants.Play_Data_Back_Flag == false)
            {
                if (Constants.History_DataPlay_Back != null && Constants.History_DataPlay_Back.CanRead == true)
                {
                    Data_All_LineNums = 0;
                    Constants.Progres_DataCount = 0;
                    Constants.History_DataReader_Back = new StreamReader(Constants.History_DataPlay_Back);
                    while ((line = Constants.History_DataReader_Back.ReadLine()) != null)
                    {
                        Data_All_LineNums++;
                    }
                    Constants.History_DataReader_Back.BaseStream.Seek(0, SeekOrigin.Begin);
                    Constants.History_DataReader_Back.DiscardBufferedData();
                    Constants.Play_Data_Back_Flag = true;
                }
                else
                {

                    System.Windows.MessageBox.Show("Please Load The New File");
                    return;
                }
            }
            else
            {
                System.Windows.MessageBox.Show("Please Not Click More Reback Button");
            }
        }
        public void Function_Data_Reback()
        {
            if (Constants.Play_Data_Back_Flag == true)
            {
                Data_Rate.Value = ((float)Constants.Progres_DataCount / (float)Data_All_LineNums * 100);//实际的回放进度
            }
            else
            {
                Data_Rate.Value = 0.0;
            }
        }

        private void Front_Speed_Click(object sender, RoutedEventArgs e)
        {
            if (Constants.Play_Data_Back_Flag == true)
            {
                if (Constants.Progres_DataCount < (int)(Data_All_LineNums - 500))
                {
                    Constants.Progres_DataCount = (Constants.Progres_DataCount + 500);
                    for (int i = 0; i < 500; i++)
                    {
                        Constants.History_DataReader_Back.ReadLine();
                    }
                }
                else
                {
                    System.Windows.MessageBox.Show("Can Not Be Forward");
                }
            }
        }

        private void Back_Speed_Click(object sender, RoutedEventArgs e)
        {
            if (Constants.Play_Data_Back_Flag == true)
            {
                if ((Constants.Progres_DataCount - 500) > 0)
                {
                    Constants.Progres_DataCount = (Constants.Progres_DataCount - 500);
                    Constants.History_DataReader_Back.BaseStream.Seek(0, SeekOrigin.Begin);
                    Constants.History_DataReader_Back.DiscardBufferedData();
                    for (int i = 0; i < Constants.Progres_DataCount; i++)
                    {
                        Constants.History_DataReader_Back.ReadLine();
                    }
                }
                else
                {
                    System.Windows.MessageBox.Show("Can Not Be Back");
                }
            }
        }

        private void Stop_Or_Reback(object sender, RoutedEventArgs e)
        {
            if (Constants.Play_Data_Back_Flag == true)
            {
                if (Constants.Stop_or_Reback_Flag == 0)
                {
                    Constants.Stop_or_Reback_Flag = 1;
                    Stop_Or_Reback_N.Content = "Recover";
                }
                else
                {
                    Constants.Stop_or_Reback_Flag = 0;
                    Stop_Or_Reback_N.Content = "Suspend";
                }
            }
            else
            {
                System.Windows.MessageBox.Show("Have No Mission");
            }
        }

        private void First_Motor_Btn_Click(object sender, RoutedEventArgs e)
        {
            if (Constants.Motor_Show_Flag == 1)
            {
                Constants.Motor1_Test_Index = 1;
                Constants.AutoWing_MotorTest_Flag = 1;
                MainWindow.pWin.Function_Wait_ExternShow();
            }
            else
            {
                System.Windows.MessageBox.Show("DisConnect");
            }

        }

        private void Second_Motor_Btn_Click(object sender, RoutedEventArgs e)
        {
            if (Constants.Motor_Show_Flag == 1)
            {
                Constants.Motor1_Test_Index = 0;
                Constants.AutoWing_MotorTest_Flag = 1;
                MainWindow.pWin.Function_Wait_ExternShow();
            }
            else
            {
                System.Windows.MessageBox.Show("DisConnect");
            }
        }
        private void Button_Click_Ganliang(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 12;//上层PID设置
            Constants.Jump_To5or50Hz_Flag = 3;//切换到50hz下的上层pid包也就是data7
        }


        private void Button_ShangCengPID_SETClick(object sender, RoutedEventArgs e)
        {
            //if (Edit_Zitai_P.Text == "" || Edit_Zitai_I.Text == "" || Edit_Zitai_D.Text == "" || Edit_HangXiang_P.Text == "" || Edit_HangXiang_I.Text == ""
            //     || Edit_HangXiang_D.Text == "" || Edit_SuDu_P.Text == "" || Edit_SuDu_I.Text == "" || Edit_SuDu_D.Text == "" || Edit_GaoDu_P.Text == "" || Edit_GaoDu_I.Text == "" || Edit_GaoDu_D.Text == "")
            //{
            //    System.Windows.MessageBox.Show("请输入相关参数值");
            //    return;
            //}
            //else
            //{
            //    Constants.Set_zitai_p = (float)Convert.ToDouble(Edit_Zitai_P.Text);
            //    Constants.Set_zitai_i = (float)Convert.ToDouble(Edit_Zitai_I.Text);
            //    Constants.Set_zitai_d = (float)Convert.ToDouble(Edit_Zitai_D.Text);
            //    Constants.Set_hangxiang_p = (float)Convert.ToDouble(Edit_HangXiang_P.Text);
            //    Constants.Set_hangxiang_i = (float)Convert.ToDouble(Edit_HangXiang_I.Text);
            //    Constants.Set_hangxiang_d = (float)Convert.ToDouble(Edit_HangXiang_D.Text);
            //    Constants.Set_sudu_p = (float)Convert.ToDouble(Edit_SuDu_P.Text);
            //    Constants.Set_sudu_i = (float)Convert.ToDouble(Edit_SuDu_I.Text);
            //    Constants.Set_sudu_d = (float)Convert.ToDouble(Edit_SuDu_D.Text);
            //    Constants.Set_gaodu_p = (float)Convert.ToDouble(Edit_GaoDu_P.Text);
            //    Constants.Set_gaodu_i = (float)Convert.ToDouble(Edit_GaoDu_I.Text);
            //    Constants.Set_gaodu_d = (float)Convert.ToDouble(Edit_GaoDu_D.Text);
            //    Constants.AutoWing_ShangCengPIDSet_Flag = 1;//开启PID设置发送标志
            //    MainWindow.pWin.Function_Wait_ExternShow();
            //}
        }
        private void Btn_ShangCengPIDIndex_Get_Click(object sender, RoutedEventArgs e)
        {
            //Edit_Zitai_P.Text = Constants.Show_zitai_p.ToString("#0");
            //Edit_Zitai_I.Text = Constants.Show_zitai_i.ToString("#0");
            //Edit_Zitai_D.Text = Constants.Show_zitai_d.ToString("#0");
            //Edit_HangXiang_P.Text = Constants.Show_hangxiang_p.ToString("#0");
            //Edit_HangXiang_I.Text = Constants.Show_hangxiang_i.ToString("#0");
            //Edit_HangXiang_D.Text = Constants.Show_hangxiang_d.ToString("#0");
            //Edit_SuDu_P.Text = Constants.Show_sudu_p.ToString("#0");
            //Edit_SuDu_I.Text = Constants.Show_sudu_i.ToString("#0");
            //Edit_SuDu_D.Text = Constants.Show_sudu_d.ToString("#0");
            //Edit_GaoDu_P.Text = Constants.Show_gaodu_p.ToString("#0");
            //Edit_GaoDu_I.Text = Constants.Show_gaodu_i.ToString("#0");
            //Edit_GaoDu_D.Text = Constants.Show_gaodu_d.ToString("#0");

            ///////////////////////////////姿态杆子///////////////////////////////////////
            if (Constants.Show_zitai_p <= 20)
            {
                slider_zitai_pid1.Value = 0;
                label_zitai_pid1.Content = "0%";
            }
            if (Constants.Show_zitai_p > 20 && Constants.Show_zitai_p <= 28)
            {
                slider_zitai_pid1.Value = 1;
                label_zitai_pid1.Content = "10%";
            }
            if (Constants.Show_zitai_p > 28 && Constants.Show_zitai_p <= 36)
            {
                slider_zitai_pid1.Value = 2;
                label_zitai_pid1.Content = "20%";
            }
            if (Constants.Show_zitai_p > 36 && Constants.Show_zitai_p <= 44)
            {
                slider_zitai_pid1.Value = 3;
                label_zitai_pid1.Content = "30%";
            }
            if (Constants.Show_zitai_p > 44 && Constants.Show_zitai_p <= 52)
            {
                slider_zitai_pid1.Value = 4;
                label_zitai_pid1.Content = "40%";
            }
            if (Constants.Show_zitai_p > 52 && Constants.Show_zitai_p <= 60)
            {
                slider_zitai_pid1.Value = 5;
                label_zitai_pid1.Content = "50%";
            }
            if (Constants.Show_zitai_p > 60 && Constants.Show_zitai_p <= 68)
            {
                slider_zitai_pid1.Value = 6;
                label_zitai_pid1.Content = "60%";
            }
            if (Constants.Show_zitai_p > 68 && Constants.Show_zitai_p <= 76)
            {
                slider_zitai_pid1.Value = 7;
                label_zitai_pid1.Content = "70%";
            }
            if (Constants.Show_zitai_p > 76 && Constants.Show_zitai_p <= 84)
            {
                slider_zitai_pid1.Value = 8;
                label_zitai_pid1.Content = "80%";
            }
            if (Constants.Show_zitai_p > 84 && Constants.Show_zitai_p <= 92)
            {
                slider_zitai_pid1.Value = 9;
                label_zitai_pid1.Content = "90%";
            }
            if (Constants.Show_zitai_p > 92)
            {
                slider_zitai_pid1.Value = 10;
                label_zitai_pid1.Content = "100%";
            }
            ///////////////////////////////////////高度杆子////////////////////////////////////////////////////////////

            if (Constants.Show_gaodu_p <= 10)
            {
                slider_zitai_pid2.Value = 0;
                label_zitai_pid2.Content = "0%";
            }
            if (Constants.Show_gaodu_p > 10 && Constants.Show_gaodu_p <= 14)
            {
                slider_zitai_pid2.Value = 1;
                label_zitai_pid2.Content = "10%";
            }
            if (Constants.Show_gaodu_p > 14 && Constants.Show_gaodu_p <= 18)
            {
                slider_zitai_pid2.Value = 2;
                label_zitai_pid2.Content = "20%";
            }
            if (Constants.Show_gaodu_p > 18 && Constants.Show_gaodu_p <= 22)
            {
                slider_zitai_pid2.Value = 3;
                label_zitai_pid2.Content = "30%";
            }
            if (Constants.Show_gaodu_p > 22 && Constants.Show_gaodu_p <= 26)
            {
                slider_zitai_pid2.Value = 4;
                label_zitai_pid2.Content = "40%";
            }
            if (Constants.Show_gaodu_p > 26 && Constants.Show_gaodu_p <= 30)
            {
                slider_zitai_pid2.Value = 5;
                label_zitai_pid2.Content = "50%";
            }
            if (Constants.Show_gaodu_p > 30 && Constants.Show_gaodu_p <= 34)
            {
                slider_zitai_pid2.Value = 6;
                label_zitai_pid2.Content = "60%";
            }
            if (Constants.Show_gaodu_p > 34 && Constants.Show_gaodu_p <= 38)
            {
                slider_zitai_pid2.Value = 7;
                label_zitai_pid2.Content = "70%";
            }
            if (Constants.Show_gaodu_p > 38 && Constants.Show_gaodu_p <= 42)
            {
                slider_zitai_pid2.Value = 8;
                label_zitai_pid2.Content = "80%";
            }
            if (Constants.Show_gaodu_p > 42 && Constants.Show_gaodu_p <= 46)
            {
                slider_zitai_pid2.Value = 9;
                label_zitai_pid2.Content = "90%";
            }
            if (Constants.Show_gaodu_p > 46)
            {
                slider_zitai_pid2.Value = 10;
                label_zitai_pid2.Content = "100%";
            }

            ///////////////////////////////////////速度杆子////////////////////////////////////////////////////////////

            if (Constants.Show_sudu_i <= 20)
            {
                slider_zitai_pid3.Value = 0;
                label_zitai_pid3.Content = "0%";
            }
            if (Constants.Show_sudu_i > 20 && Constants.Show_sudu_i <= 26)
            {
                slider_zitai_pid3.Value = 1;
                label_zitai_pid3.Content = "10%";
            }
            if (Constants.Show_sudu_i > 26 && Constants.Show_sudu_i <= 32)
            {
                slider_zitai_pid3.Value = 2;
                label_zitai_pid3.Content = "20%";
            }
            if (Constants.Show_sudu_i > 32 && Constants.Show_sudu_i <= 38)
            {
                slider_zitai_pid3.Value = 3;
                label_zitai_pid3.Content = "30%";
            }
            if (Constants.Show_sudu_i > 38 && Constants.Show_sudu_i <= 44)
            {
                slider_zitai_pid3.Value = 4;
                label_zitai_pid3.Content = "40%";
            }
            if (Constants.Show_sudu_i > 44 && Constants.Show_sudu_i <= 50)
            {
                slider_zitai_pid3.Value = 5;
                label_zitai_pid3.Content = "50%";
            }
            if (Constants.Show_sudu_i > 50 && Constants.Show_sudu_i <= 56)
            {
                slider_zitai_pid3.Value = 6;
                label_zitai_pid3.Content = "60%";
            }
            if (Constants.Show_sudu_i > 56 && Constants.Show_sudu_i <= 62)
            {
                slider_zitai_pid3.Value = 7;
                label_zitai_pid3.Content = "70%";
            }
            if (Constants.Show_sudu_i > 62 && Constants.Show_sudu_i <= 68)
            {
                slider_zitai_pid3.Value = 8;
                label_zitai_pid3.Content = "80%";
            }
            if (Constants.Show_sudu_i > 68 && Constants.Show_sudu_i <= 74)
            {
                slider_zitai_pid3.Value = 9;
                label_zitai_pid3.Content = "90%";
            }
            if (Constants.Show_sudu_i > 74)
            {
                slider_zitai_pid3.Value = 10;
                label_zitai_pid3.Content = "100%";
            }

        }

        private void Edit_Zitai_P_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberic(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }
        public static bool isNumberic(string _string)
        {
            if (string.IsNullOrEmpty(_string))
            {
                return false;
            }
            foreach (char c in _string)
            {
                if (!char.IsDigit(c))
                {
                    return false;
                }
            }
            return true;
        }

        private void Edit_Zitai_P_TextChanged(object sender, TextChangedEventArgs e)
        {
            //if (Edit_Zitai_P.Text != "")
            //{
            //    if (Convert.ToInt32(Edit_Zitai_P.Text) >= 0 && Convert.ToInt32(Edit_Zitai_P.Text) <= 100)
            //    {

            //    }
            //    else
            //    {
            //        Edit_Zitai_P.Text = "0";
            //        System.Windows.MessageBox.Show("输入的值保持在0-100之间");
            //    }
            //}
        }

        private void Edit_Zitai_I_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberic(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        private void Edit_Zitai_I_TextChanged(object sender, TextChangedEventArgs e)
        {
            //if (Edit_Zitai_I.Text != "")
            //{
            //    if (Convert.ToInt32(Edit_Zitai_I.Text) >= 0 && Convert.ToInt32(Edit_Zitai_I.Text) <= 100)
            //    {

            //    }
            //    else
            //    {
            //        Edit_Zitai_I.Text = "0";
            //        System.Windows.MessageBox.Show("输入的值保持在0-100之间");
            //    }
            //}
        }

        private void Edit_Zitai_D_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberic(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        private void Edit_Zitai_D_TextChanged(object sender, TextChangedEventArgs e)
        {
            //if (Edit_Zitai_D.Text != "")
            //{
            //    if (Convert.ToInt32(Edit_Zitai_D.Text) >= 0 && Convert.ToInt32(Edit_Zitai_D.Text) <= 100)
            //    {

            //    }
            //    else
            //    {
            //        Edit_Zitai_D.Text = "0";
            //        System.Windows.MessageBox.Show("输入的值保持在0-100之间");
            //    }
            //}
        }

        private void Edit_HangXiang_P_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberic(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        private void Edit_HangXiang_P_TextChanged(object sender, TextChangedEventArgs e)
        {
            //if (Edit_HangXiang_P.Text != "")
            //{
            //    if (Convert.ToInt32(Edit_HangXiang_P.Text) >= 0 && Convert.ToInt32(Edit_HangXiang_P.Text) <= 100)
            //    {

            //    }
            //    else
            //    {
            //        Edit_HangXiang_P.Text = "0";
            //        System.Windows.MessageBox.Show("输入的值保持在0-100之间");
            //    }
            //}
        }

        private void Edit_HangXiang_I_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberic(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        private void Edit_HangXiang_I_TextChanged(object sender, TextChangedEventArgs e)
        {
            //if (Edit_HangXiang_I.Text != "")
            //{
            //    if (Convert.ToInt32(Edit_HangXiang_I.Text) >= 0 && Convert.ToInt32(Edit_HangXiang_I.Text) <= 100)
            //    {

            //    }
            //    else
            //    {
            //        Edit_HangXiang_I.Text = "0";
            //        System.Windows.MessageBox.Show("输入的值保持在0-100之间");
            //    }
            //}
        }

        private void Edit_HangXiang_D_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberic(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        private void Edit_HangXiang_D_TextChanged(object sender, TextChangedEventArgs e)
        {
            //if (Edit_HangXiang_D.Text != "")
            //{
            //    if (Convert.ToInt32(Edit_HangXiang_D.Text) >= 0 && Convert.ToInt32(Edit_HangXiang_D.Text) <= 100)
            //    {

            //    }
            //    else
            //    {
            //        Edit_HangXiang_D.Text = "0";
            //        System.Windows.MessageBox.Show("输入的值保持在0-100之间");
            //    }
            //}
        }

        private void Edit_SuDu_P_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberic(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        private void Edit_SuDu_P_TextChanged(object sender, TextChangedEventArgs e)
        {
            //if (Edit_SuDu_P.Text != "")
            //{
            //    if (Convert.ToInt32(Edit_SuDu_P.Text) >= 0 && Convert.ToInt32(Edit_SuDu_P.Text) <= 100)
            //    {

            //    }
            //    else
            //    {
            //        Edit_SuDu_P.Text = "0";
            //        System.Windows.MessageBox.Show("输入的值保持在0-100之间");
            //    }
            //}
        }

        private void Edit_SuDu_I_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberic(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        private void Edit_SuDu_I_TextChanged(object sender, TextChangedEventArgs e)
        {
            //if (Edit_SuDu_I.Text != "")
            //{
            //    if (Convert.ToInt32(Edit_SuDu_I.Text) >= 0 && Convert.ToInt32(Edit_SuDu_I.Text) <= 100)
            //    {

            //    }
            //    else
            //    {
            //        Edit_SuDu_I.Text = "0";
            //        System.Windows.MessageBox.Show("输入的值保持在0-100之间");
            //    }
            //}
        }

        private void Edit_SuDu_D_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberic(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        private void Edit_SuDu_D_TextChanged(object sender, TextChangedEventArgs e)
        {
            //if (Edit_SuDu_D.Text != "")
            //{
            //    if (Convert.ToInt32(Edit_SuDu_D.Text) >= 0 && Convert.ToInt32(Edit_SuDu_D.Text) <= 100)
            //    {

            //    }
            //    else
            //    {
            //        Edit_SuDu_D.Text = "0";
            //        System.Windows.MessageBox.Show("输入的值保持在0-100之间");
            //    }
            //}
        }

        private void Edit_GaoDu_P_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberic(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        private void Edit_GaoDu_P_TextChanged(object sender, TextChangedEventArgs e)
        {
            //if (Edit_GaoDu_P.Text != "")
            //{
            //    if (Convert.ToInt32(Edit_GaoDu_P.Text) >= 0 && Convert.ToInt32(Edit_GaoDu_P.Text) <= 100)
            //    {

            //    }
            //    else
            //    {
            //        Edit_GaoDu_P.Text = "0";
            //        System.Windows.MessageBox.Show("输入的值保持在0-100之间");
            //    }
            //}
        }

        private void Edit_GaoDu_I_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberic(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        private void Edit_GaoDu_I_TextChanged(object sender, TextChangedEventArgs e)
        {
            //if (Edit_GaoDu_I.Text != "")
            //{
            //    if (Convert.ToInt32(Edit_GaoDu_I.Text) >= 0 && Convert.ToInt32(Edit_GaoDu_I.Text) <= 100)
            //    {

            //    }
            //    else
            //    {
            //        Edit_GaoDu_I.Text = "0";
            //        System.Windows.MessageBox.Show("输入的值保持在0-100之间");
            //    }
            //}
        }

        private void Edit_GaoDu_D_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (!isNumberic(e.Text))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        private void Edit_GaoDu_D_TextChanged(object sender, TextChangedEventArgs e)
        {
            //if (Edit_GaoDu_D.Text != "")
            //{
            //    if (Convert.ToInt32(Edit_GaoDu_D.Text) >= 0 && Convert.ToInt32(Edit_GaoDu_D.Text) <= 100)
            //    {

            //    }
            //    else
            //    {
            //        Edit_GaoDu_D.Text = "0";
            //        System.Windows.MessageBox.Show("输入的值保持在0-100之间");
            //    }
            //}
        }
        private void function_MagnetAnd_AB()//切换进来后显示当前得到的相关参数值
        {
            Edit_Mag_OffsetAngle.Text = Constants.Mag_Offset_Angle_Show.ToString();
            Edit_AB_ApproachLine_Range.Text = Constants.AB_ApproachLine_Range_Show.ToString("#0.0");
            Edit_AB_ApproachLine_DeadZone.Text = Constants.AB_ApproachLine_DeadZone_Show.ToString("#0.0");
            Edit_AB_ApproachPoint_Range.Text = Constants.AB_ApproachPoint_Range_Show.ToString("#0.0");
            Edit_AB_HorizoMove_Acc.Text = Constants.AB_HorizoMove_Acc_Show.ToString("#0.0");
            Edit_AB_HorizoMove_Range.Text = Constants.AB_HorizoMove_Range_Show.ToString("#0.0");
            Edit_AB_FowardMove_Acc.Text = Constants.AB_FowardMove_Acc_Show.ToString("#0.0");
            Edit_AB_VelToPosition_Percent.Text = Constants.AB_VelToPosition_Percent_Show.ToString("#0.00");
            Edit_AB_BToThreshold_Percent.Text = Constants.AB_BToThreshold_Percent_Show.ToString("#0.0");
            Edit_AB_AToThreshold_Percen.Text = Constants.AB_AToThreshold_Percen_Show.ToString("#0.0");
            Edit_AB_BackThreshold_Value.Text = Constants.AB_BackThreshold_Show.ToString("#0.0");
        }
        private void function_Chart_Pump()
        {
            int max_vel = (Int32)(Constants.Intelligent_Vel_MaxValue_Show);
            int min_vel = (Int32)(Constants.Intelligent_Vel_MinValue_Show);
            SLA_H.SetMAxValue(max_vel);
            SLA_H.SetMInValue(min_vel);
            SLA_V.SetMAxValue(Constants.Intelligent_Pump_MaxValue_Show);
            SLA_V.SetMInValue(Constants.Intelligent_Pump_MinValue_Show);

            ///////////////////////绘制最小速度最小泵量->最大速度最大泵量的线段//////////////////////////
            Plot_Chart.Children.Remove(Chart_Line);
            Plot_Chart.Children.Remove(Chart2_Line);
            Plot_Chart.Children.Remove(Chart_Line3);
            Plot_Chart.Children.Remove(Chart_Line4);
            Plot_Chart.Children.Remove(Chart_Line5);
            Plot_Chart.Children.Remove(Chart_Line6);
            Chart_Line.X1 = (2.5 * SLA_H.StartValue);
            Chart_Line.X2 = (2.5 * SLA_H.EndValue);
            Chart_Line.Y1 = (200 - (SLA_V.StartValue * 2));
            Chart_Line.Y2 = (200 - (SLA_V.EndValue * 2));
            Chart_Line.Stroke = Brushes.Coral;
            Chart_Line.StrokeThickness = 2;
            //          Chart_Line.StrokeDashArray = new DoubleCollection() { 2, 3 };
            Chart_Line.StrokeDashCap = PenLineCap.Triangle;
            Chart_Line.StrokeEndLineCap = PenLineCap.Square;
            Chart_Line.StrokeStartLineCap = PenLineCap.Round;
            Plot_Chart.Children.Add(Chart_Line);
            ///////////////////////绘制大速度最大泵量->终点速度（8m/s）最大泵量
            Chart2_Line.X1 = (2.5 * SLA_H.EndValue);
            Chart2_Line.X2 = 200;
            Chart2_Line.Y1 = (200 - (SLA_V.EndValue * 2));
            Chart2_Line.Y2 = (200 - (SLA_V.EndValue * 2));
            Chart2_Line.Stroke = Brushes.Coral;
            Chart2_Line.StrokeThickness = 2;
            //           Chart2_Line.StrokeDashArray = new DoubleCollection() { 2, 3 };
            Chart2_Line.StrokeDashCap = PenLineCap.Triangle;
            Chart2_Line.StrokeEndLineCap = PenLineCap.Square;
            Chart2_Line.StrokeStartLineCap = PenLineCap.Round;
            Plot_Chart.Children.Add(Chart2_Line);
            //////////////////////竖直往下的最小值对应的虚线///////////////////////////////////////
            Chart_Line3.X1 = (2.5 * SLA_H.StartValue);
            Chart_Line3.X2 = (2.5 * SLA_H.StartValue);
            Chart_Line3.Y1 = (200 - (SLA_V.StartValue * 2));
            Chart_Line3.Y2 = 200;
            Chart_Line3.Stroke = Brushes.Yellow;
            Chart_Line3.StrokeThickness = 2;
            Chart_Line3.StrokeDashArray = new DoubleCollection() { 2, 3 };
            Chart_Line3.StrokeDashCap = PenLineCap.Triangle;
            Chart_Line3.StrokeEndLineCap = PenLineCap.Square;
            Chart_Line3.StrokeStartLineCap = PenLineCap.Round;
            Plot_Chart.Children.Add(Chart_Line3);
            ////////////////////////水平往左最小值对应的虚线//////////////////////////////////////
            Chart_Line4.X1 = 0;
            Chart_Line4.X2 = (2.5 * SLA_H.StartValue);
            Chart_Line4.Y1 = (200 - (SLA_V.StartValue * 2));
            Chart_Line4.Y2 = (200 - (SLA_V.StartValue * 2));
            Chart_Line4.Stroke = Brushes.Yellow;
            Chart_Line4.StrokeThickness = 2;
            Chart_Line4.StrokeDashArray = new DoubleCollection() { 2, 3 };
            Chart_Line4.StrokeDashCap = PenLineCap.Triangle;
            Chart_Line4.StrokeEndLineCap = PenLineCap.Square;
            Chart_Line4.StrokeStartLineCap = PenLineCap.Round;
            Plot_Chart.Children.Add(Chart_Line4);
            ////////////////////////竖直往下的最大值对应的虚线//////////////////////////////////////
            Chart_Line5.X1 = (2.5 * SLA_H.EndValue);
            Chart_Line5.X2 = (2.5 * SLA_H.EndValue);
            Chart_Line5.Y1 = (200 - (SLA_V.EndValue * 2));
            Chart_Line5.Y2 = 200;
            Chart_Line5.Stroke = Brushes.Blue;
            Chart_Line5.StrokeThickness = 2;
            Chart_Line5.StrokeDashArray = new DoubleCollection() { 2, 3 };
            Chart_Line5.StrokeDashCap = PenLineCap.Triangle;
            Chart_Line5.StrokeEndLineCap = PenLineCap.Square;
            Chart_Line5.StrokeStartLineCap = PenLineCap.Round;
            Plot_Chart.Children.Add(Chart_Line5);
            ////////////////////////水平往左最大值对应的虚线//////////////////////////////////////
            Chart_Line6.X1 = 0;
            Chart_Line6.X2 = (2.5 * SLA_H.EndValue);
            Chart_Line6.Y1 = (200 - (SLA_V.EndValue * 2));
            Chart_Line6.Y2 = (200 - (SLA_V.EndValue * 2));
            Chart_Line6.Stroke = Brushes.Blue;
            Chart_Line6.StrokeThickness = 2;
            Chart_Line6.StrokeDashArray = new DoubleCollection() { 2, 3 };
            Chart_Line6.StrokeDashCap = PenLineCap.Triangle;
            Chart_Line6.StrokeEndLineCap = PenLineCap.Square;
            Chart_Line6.StrokeStartLineCap = PenLineCap.Round;
            Plot_Chart.Children.Add(Chart_Line6);
            Max_Percent.Content = SLA_V.EndValue.ToString() + "%";
            Min_Percent.Content = SLA_V.StartValue.ToString() + "%";
            double Max_Vel_f = 0.0;
            double Min_Vel_f = 0.0;
            Max_Vel_f = SLA_H.EndValue / 10.0;
            Min_Vel_f = SLA_H.StartValue / 10.0;
            Max_Vel.Content = Max_Vel_f.ToString("#0.0") + "m/s";
            Min_Vel.Content = Min_Vel_f.ToString("#0.0") + "m/s";
            Fog_Slider.Value = Constants.Intelligent_Motor_Value_Show;
            Fog_Value.Content = Fog_Slider.Value.ToString("#0") + "%";
            Fog_ConstantSlider.Value = Constants.Fixed_Motor_Value_Show;
            Fog_ConstantValue.Content = Fog_ConstantSlider.Value.ToString("#0") + "%";
            Fog_ConstantPumpSlider.Value = Constants.Fixed_Pump_Value_Show;
            Fog_ConstantPumpValue.Content = Fog_ConstantPumpSlider.Value.ToString("#0") + "%";

            if(Constants.Show_shuiben_duanyao_action==0)
            {
                M_Duanyao_Action.SelectedIndex = 0;
            }
            else
            {
                M_Duanyao_Action.SelectedIndex = 1;
            }

        }
        private void Button_Click_WaterPump(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 13;//水泵校准
            function_Chart_Pump();
        }

        /// <summary>
        /// 磁偏角及拐弯速度
        /// </summary>
        private void Button_Click_MagnetAngle_TurnSpeed(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 15;//磁偏量及拐弯界面
            function_MagnetAnd_AB();
        }

        int Get_Current_Value_Flag = 0;
        private void SLA_V_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Get_Current_Value_Flag = 1;
            Constants.Set_Pump_Flag = 1;
        }

        private void SLA_V_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Get_Current_Value_Flag = 0;
            Constants.Set_Pump_Flag = 0;
        }
        Line Chart_Line = new Line();//实际的线条
        Line Chart2_Line = new Line();//补全后面的线条
        Line Chart_Line3 = new Line();//最小速度对应竖直往下
        Line Chart_Line4 = new Line();//最小速度对应水平往左
        Line Chart_Line5 = new Line();//最大速度对应竖直往下
        Line Chart_Line6 = new Line();//最大速度对应水平往左
        private void SLA_V_PreviewMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (Get_Current_Value_Flag == 1)
            {
                if (SLA_V.StartValue >= 20 && SLA_V.EndValue >= 30)
                {
                    ///////////////////////绘制最小速度最小泵量->最大速度最大泵量的线段//////////////////////////
                    Plot_Chart.Children.Remove(Chart_Line);
                    Plot_Chart.Children.Remove(Chart2_Line);
                    Plot_Chart.Children.Remove(Chart_Line3);
                    Plot_Chart.Children.Remove(Chart_Line4);
                    Plot_Chart.Children.Remove(Chart_Line5);
                    Plot_Chart.Children.Remove(Chart_Line6);
                    Chart_Line.X1 = (2.5 * SLA_H.StartValue);
                    Chart_Line.X2 = (2.5 * SLA_H.EndValue);
                    Chart_Line.Y1 = (200 - (SLA_V.StartValue * 2));
                    Chart_Line.Y2 = (200 - (SLA_V.EndValue * 2));
                    Chart_Line.Stroke = Brushes.Coral;
                    Chart_Line.StrokeThickness = 2;
                    //      Chart_Line.StrokeDashArray = new DoubleCollection() { 2, 3 };
                    Chart_Line.StrokeDashCap = PenLineCap.Triangle;
                    Chart_Line.StrokeEndLineCap = PenLineCap.Square;
                    Chart_Line.StrokeStartLineCap = PenLineCap.Round;
                    Plot_Chart.Children.Add(Chart_Line);
                    ///////////////////////绘制大速度最大泵量->终点速度（8m/s）最大泵量
                    Chart2_Line.X1 = (2.5 * SLA_H.EndValue);
                    Chart2_Line.X2 = 200;
                    Chart2_Line.Y1 = (200 - (SLA_V.EndValue * 2));
                    Chart2_Line.Y2 = (200 - (SLA_V.EndValue * 2));
                    Chart2_Line.Stroke = Brushes.Coral;//Brushes.Coral
                    Chart2_Line.StrokeThickness = 2;
                    //      Chart2_Line.StrokeDashArray = new DoubleCollection() { 2, 3 };
                    Chart2_Line.StrokeDashCap = PenLineCap.Triangle;
                    Chart2_Line.StrokeEndLineCap = PenLineCap.Square;
                    Chart2_Line.StrokeStartLineCap = PenLineCap.Round;
                    Plot_Chart.Children.Add(Chart2_Line);

                    //////////////////////竖直往下的最小值对应的虚线///////////////////////////////////////
                    Chart_Line3.X1 = (2.5 * SLA_H.StartValue);
                    Chart_Line3.X2 = (2.5 * SLA_H.StartValue);
                    Chart_Line3.Y1 = (200 - (SLA_V.StartValue * 2));
                    Chart_Line3.Y2 = 200;
                    Chart_Line3.Stroke = Brushes.Yellow;
                    Chart_Line3.StrokeThickness = 2;
                    Chart_Line3.StrokeDashArray = new DoubleCollection() { 2, 3 };
                    Chart_Line3.StrokeDashCap = PenLineCap.Triangle;
                    Chart_Line3.StrokeEndLineCap = PenLineCap.Square;
                    Chart_Line3.StrokeStartLineCap = PenLineCap.Round;
                    Plot_Chart.Children.Add(Chart_Line3);
                    ////////////////////////水平往左最小值对应的虚线//////////////////////////////////////
                    Chart_Line4.X1 = 0;
                    Chart_Line4.X2 = (2.5 * SLA_H.StartValue);
                    Chart_Line4.Y1 = (200 - (SLA_V.StartValue * 2));
                    Chart_Line4.Y2 = (200 - (SLA_V.StartValue * 2));
                    Chart_Line4.Stroke = Brushes.Yellow;
                    Chart_Line4.StrokeThickness = 2;
                    Chart_Line4.StrokeDashArray = new DoubleCollection() { 2, 3 };
                    Chart_Line4.StrokeDashCap = PenLineCap.Triangle;
                    Chart_Line4.StrokeEndLineCap = PenLineCap.Square;
                    Chart_Line4.StrokeStartLineCap = PenLineCap.Round;
                    Plot_Chart.Children.Add(Chart_Line4);
                    ////////////////////////竖直往下的最大值对应的虚线//////////////////////////////////////
                    Chart_Line5.X1 = (2.5 * SLA_H.EndValue);
                    Chart_Line5.X2 = (2.5 * SLA_H.EndValue);
                    Chart_Line5.Y1 = (200 - (SLA_V.EndValue * 2));
                    Chart_Line5.Y2 = 200;
                    Chart_Line5.Stroke = Brushes.Blue;
                    Chart_Line5.StrokeThickness = 2;
                    Chart_Line5.StrokeDashArray = new DoubleCollection() { 2, 3 };
                    Chart_Line5.StrokeDashCap = PenLineCap.Triangle;
                    Chart_Line5.StrokeEndLineCap = PenLineCap.Square;
                    Chart_Line5.StrokeStartLineCap = PenLineCap.Round;
                    Plot_Chart.Children.Add(Chart_Line5);
                    ////////////////////////水平往左最大值对应的虚线//////////////////////////////////////
                    Chart_Line6.X1 = 0;
                    Chart_Line6.X2 = (2.5 * SLA_H.EndValue);
                    Chart_Line6.Y1 = (200 - (SLA_V.EndValue * 2));
                    Chart_Line6.Y2 = (200 - (SLA_V.EndValue * 2));
                    Chart_Line6.Stroke = Brushes.Blue;
                    Chart_Line6.StrokeThickness = 2;
                    Chart_Line6.StrokeDashArray = new DoubleCollection() { 2, 3 };
                    Chart_Line6.StrokeDashCap = PenLineCap.Triangle;
                    Chart_Line6.StrokeEndLineCap = PenLineCap.Square;
                    Chart_Line6.StrokeStartLineCap = PenLineCap.Round;
                    Plot_Chart.Children.Add(Chart_Line6);

                    Max_Percent.Content = SLA_V.EndValue.ToString() + "%";
                    Min_Percent.Content = SLA_V.StartValue.ToString() + "%";
                    double Max_Vel_f = 0.0;
                    double Min_Vel_f = 0.0;
                    Max_Vel_f = SLA_H.EndValue / 10.0;
                    Min_Vel_f = SLA_H.StartValue / 10.0;
                    Max_Vel.Content = Max_Vel_f.ToString("#0.0") + "m/s";
                    Min_Vel.Content = Min_Vel_f.ToString("#0.0") + "m/s";
                    Constants.Intelligent_Pump_MaxValue_Set = SLA_V.EndValue;
                    Constants.Intelligent_Pump_MinValue_Set = SLA_V.StartValue;
                    Constants.Intelligent_Vel_MaxValue_Set = SLA_H.EndValue;
                    Constants.Intelligent_Vel_MinValue_Set = SLA_H.StartValue;
                    Constants.Intelligent_Motor_Value_Set = (Int32)Math.Round(Fog_Slider.Value);
                    Constants.Fixed_Pump_Value_Set = (Int32)Math.Round(Fog_ConstantPumpSlider.Value);
                    Constants.Fixed_Motor_Value_Set = (Int32)Math.Round(Fog_ConstantSlider.Value);

                    if(M_Duanyao_Action.SelectedIndex==0)
                    {
                        Constants.Set_shuiben_duanyao_action = 0;//无动作
                    }
                    else if (M_Duanyao_Action.SelectedIndex == 1)
                    {
                        Constants.Set_shuiben_duanyao_action = 1;//返航
                    }
                    else
                    {
                        Constants.Set_shuiben_duanyao_action = 2;//悬停
                    }
                }
                else
                {
                    if (SLA_V.StartValue < 20)
                    {
                        SLA_V.SetMInValue(20);
                    }
                    if (SLA_V.EndValue < 30)
                    {
                        SLA_V.SetMAxValue(30);
                    }
                }
            }
        }
        int Get_Current2_Value_Flag = 0;
        private void SLA_H_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Get_Current2_Value_Flag = 1;
            Constants.Set_Pump_Flag = 1;
        }

        private void SLA_H_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Get_Current2_Value_Flag = 0;
            Constants.Set_Pump_Flag = 0;
        }

        private void SLA_H_PreviewMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (Get_Current2_Value_Flag == 1)
            {
                ///////////////////////绘制最小速度最小泵量->最大速度最大泵量的线段//////////////////////////
                Plot_Chart.Children.Remove(Chart_Line);
                Plot_Chart.Children.Remove(Chart2_Line);
                Plot_Chart.Children.Remove(Chart_Line3);
                Plot_Chart.Children.Remove(Chart_Line4);
                Plot_Chart.Children.Remove(Chart_Line5);
                Plot_Chart.Children.Remove(Chart_Line6);
                Chart_Line.X1 = (2.5 * SLA_H.StartValue);
                Chart_Line.X2 = (2.5 * SLA_H.EndValue);
                Chart_Line.Y1 = (200 - (SLA_V.StartValue * 2));
                Chart_Line.Y2 = (200 - (SLA_V.EndValue * 2));
                Chart_Line.Stroke = Brushes.Coral;
                Chart_Line.StrokeThickness = 2;
                //         Chart_Line.StrokeDashArray = new DoubleCollection() { 2, 3 };
                Chart_Line.StrokeDashCap = PenLineCap.Triangle;
                Chart_Line.StrokeEndLineCap = PenLineCap.Square;
                Chart_Line.StrokeStartLineCap = PenLineCap.Round;
                Plot_Chart.Children.Add(Chart_Line);
                ///////////////////////绘制大速度最大泵量->终点速度（8m/s）最大泵量
                Chart2_Line.X1 = (2.5 * SLA_H.EndValue);
                Chart2_Line.X2 = 200;
                Chart2_Line.Y1 = (200 - (SLA_V.EndValue * 2));
                Chart2_Line.Y2 = (200 - (SLA_V.EndValue * 2));
                Chart2_Line.Stroke = Brushes.Coral;
                Chart2_Line.StrokeThickness = 2;
                //       Chart2_Line.StrokeDashArray = new DoubleCollection() { 2, 3 };
                Chart2_Line.StrokeDashCap = PenLineCap.Triangle;
                Chart2_Line.StrokeEndLineCap = PenLineCap.Square;
                Chart2_Line.StrokeStartLineCap = PenLineCap.Round;
                Plot_Chart.Children.Add(Chart2_Line);
                //////////////////////竖直往下的最小值对应的虚线///////////////////////////////////////
                Chart_Line3.X1 = (2.5 * SLA_H.StartValue);
                Chart_Line3.X2 = (2.5 * SLA_H.StartValue);
                Chart_Line3.Y1 = (200 - (SLA_V.StartValue * 2));
                Chart_Line3.Y2 = 200;
                Chart_Line3.Stroke = Brushes.Yellow;
                Chart_Line3.StrokeThickness = 2;
                Chart_Line3.StrokeDashArray = new DoubleCollection() { 2, 3 };
                Chart_Line3.StrokeDashCap = PenLineCap.Triangle;
                Chart_Line3.StrokeEndLineCap = PenLineCap.Square;
                Chart_Line3.StrokeStartLineCap = PenLineCap.Round;
                Plot_Chart.Children.Add(Chart_Line3);
                ////////////////////////水平往左最小值对应的虚线//////////////////////////////////////
                Chart_Line4.X1 = 0;
                Chart_Line4.X2 = (2.5 * SLA_H.StartValue);
                Chart_Line4.Y1 = (200 - (SLA_V.StartValue * 2));
                Chart_Line4.Y2 = (200 - (SLA_V.StartValue * 2));
                Chart_Line4.Stroke = Brushes.Yellow;
                Chart_Line4.StrokeThickness = 2;
                Chart_Line4.StrokeDashArray = new DoubleCollection() { 2, 3 };
                Chart_Line4.StrokeDashCap = PenLineCap.Triangle;
                Chart_Line4.StrokeEndLineCap = PenLineCap.Square;
                Chart_Line4.StrokeStartLineCap = PenLineCap.Round;
                Plot_Chart.Children.Add(Chart_Line4);
                ////////////////////////竖直往下的最大值对应的虚线//////////////////////////////////////
                Chart_Line5.X1 = (2.5 * SLA_H.EndValue);
                Chart_Line5.X2 = (2.5 * SLA_H.EndValue);
                Chart_Line5.Y1 = (200 - (SLA_V.EndValue * 2));
                Chart_Line5.Y2 = 200;
                Chart_Line5.Stroke = Brushes.Blue;
                Chart_Line5.StrokeThickness = 2;
                Chart_Line5.StrokeDashArray = new DoubleCollection() { 2, 3 };
                Chart_Line5.StrokeDashCap = PenLineCap.Triangle;
                Chart_Line5.StrokeEndLineCap = PenLineCap.Square;
                Chart_Line5.StrokeStartLineCap = PenLineCap.Round;
                Plot_Chart.Children.Add(Chart_Line5);
                ////////////////////////水平往左最大值对应的虚线//////////////////////////////////////
                Chart_Line6.X1 = 0;
                Chart_Line6.X2 = (2.5 * SLA_H.EndValue);
                Chart_Line6.Y1 = (200 - (SLA_V.EndValue * 2));
                Chart_Line6.Y2 = (200 - (SLA_V.EndValue * 2));
                Chart_Line6.Stroke = Brushes.Blue;
                Chart_Line6.StrokeThickness = 2;
                Chart_Line6.StrokeDashArray = new DoubleCollection() { 2, 3 };
                Chart_Line6.StrokeDashCap = PenLineCap.Triangle;
                Chart_Line6.StrokeEndLineCap = PenLineCap.Square;
                Chart_Line6.StrokeStartLineCap = PenLineCap.Round;
                Plot_Chart.Children.Add(Chart_Line6);

                double Max_Vel_f = 0.0;
                double Min_Vel_f = 0.0;
                Max_Vel_f = SLA_H.EndValue / 10.0;
                Min_Vel_f = SLA_H.StartValue / 10.0;
                Max_Vel.Content = Max_Vel_f.ToString("#0.0") + "m/s";
                Min_Vel.Content = Min_Vel_f.ToString("#0.0") + "m/s";
                Constants.Intelligent_Pump_MaxValue_Set = SLA_V.EndValue;
                Constants.Intelligent_Pump_MinValue_Set = SLA_V.StartValue;
                Constants.Intelligent_Vel_MaxValue_Set = SLA_H.EndValue;
                Constants.Intelligent_Vel_MinValue_Set = SLA_H.StartValue;
                Constants.Intelligent_Motor_Value_Set = (Int32)Math.Round(Fog_Slider.Value);
                Constants.Fixed_Pump_Value_Set = (Int32)Math.Round(Fog_ConstantPumpSlider.Value);
                Constants.Fixed_Motor_Value_Set = (Int32)Math.Round(Fog_ConstantSlider.Value);
                if (M_Duanyao_Action.SelectedIndex == 0)
                {
                    Constants.Set_shuiben_duanyao_action = 0;
                }
                else
                {
                    Constants.Set_shuiben_duanyao_action = 1;
                }
            }
        }

        private void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            Fog_Value.Content = Fog_Slider.Value.ToString("#0") + "%";
            Constants.Intelligent_Pump_MaxValue_Set = SLA_V.EndValue;
            Constants.Intelligent_Pump_MinValue_Set = SLA_V.StartValue;
            Constants.Intelligent_Vel_MaxValue_Set = SLA_H.EndValue;
            Constants.Intelligent_Vel_MinValue_Set = SLA_H.StartValue;
            Constants.Intelligent_Motor_Value_Set = (Int32)Math.Round(Fog_Slider.Value);
            Constants.Fixed_Pump_Value_Set = (Int32)Math.Round(Fog_ConstantPumpSlider.Value);
            Constants.Fixed_Motor_Value_Set = (Int32)Math.Round(Fog_ConstantSlider.Value);
            if (M_Duanyao_Action.SelectedIndex == 0)
            {
                Constants.Set_shuiben_duanyao_action = 0;
            }
            else
            {
                Constants.Set_shuiben_duanyao_action = 1;
            }
        }

        private void Fog_ConstantSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            Fog_ConstantValue.Content = Fog_ConstantSlider.Value.ToString("#0") + "%";
            Constants.Intelligent_Pump_MaxValue_Set = SLA_V.EndValue;
            Constants.Intelligent_Pump_MinValue_Set = SLA_V.StartValue;
            Constants.Intelligent_Vel_MaxValue_Set = SLA_H.EndValue;
            Constants.Intelligent_Vel_MinValue_Set = SLA_H.StartValue;
            Constants.Intelligent_Motor_Value_Set = (Int32)Math.Round(Fog_Slider.Value);
            Constants.Fixed_Pump_Value_Set = (Int32)Math.Round(Fog_ConstantPumpSlider.Value);
            Constants.Fixed_Motor_Value_Set = (Int32)Math.Round(Fog_ConstantSlider.Value);
            if (M_Duanyao_Action.SelectedIndex == 0)
            {
                Constants.Set_shuiben_duanyao_action = 0;
            }
            else
            {
                Constants.Set_shuiben_duanyao_action = 1;
            }
        }

        private void Fog_ConstantPumpSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            int Fixed_Pump_Value_Set = (Int32)Math.Round(Fog_ConstantPumpSlider.Value);
            if (Fixed_Pump_Value_Set >= 20)
            {
                Fog_ConstantPumpValue.Content = Fog_ConstantPumpSlider.Value.ToString("#0") + "%";
                Constants.Intelligent_Pump_MaxValue_Set = SLA_V.EndValue;
                Constants.Intelligent_Pump_MinValue_Set = SLA_V.StartValue;
                Constants.Intelligent_Vel_MaxValue_Set = SLA_H.EndValue;
                Constants.Intelligent_Vel_MinValue_Set = SLA_H.StartValue;
                Constants.Intelligent_Motor_Value_Set = (Int32)Math.Round(Fog_Slider.Value);
                Constants.Fixed_Pump_Value_Set = (Int32)Math.Round(Fog_ConstantPumpSlider.Value);
                Constants.Fixed_Motor_Value_Set = (Int32)Math.Round(Fog_ConstantSlider.Value);
                if (M_Duanyao_Action.SelectedIndex == 0)
                {
                    Constants.Set_shuiben_duanyao_action = 0;
                }
                else
                {
                    Constants.Set_shuiben_duanyao_action = 1;
                }
            }
            else
            {
                Fog_ConstantPumpSlider.Value = 20;
                Fog_ConstantPumpValue.Content = "20" + "%";
            }
        }
        private void Btn_PumPIndex_Get_Click(object sender, RoutedEventArgs e)
        {
            function_Chart_Pump();
            function_Chart_Pump();
        }
        private void Button_PumP_SETClick(object sender, RoutedEventArgs e)
        {
            Constants.Intelligent_Pump_MaxValue_Set = SLA_V.EndValue;
            Constants.Intelligent_Pump_MinValue_Set = SLA_V.StartValue;
            Constants.Intelligent_Vel_MaxValue_Set = SLA_H.EndValue;
            Constants.Intelligent_Vel_MinValue_Set = SLA_H.StartValue;
            Constants.Intelligent_Motor_Value_Set = (Int32)Math.Round(Fog_Slider.Value);
            Constants.Fixed_Pump_Value_Set = (Int32)Math.Round(Fog_ConstantPumpSlider.Value);
            Constants.Fixed_Motor_Value_Set = (Int32)Math.Round(Fog_ConstantSlider.Value);
            if (M_Duanyao_Action.SelectedIndex == 0)
            {
                Constants.Set_shuiben_duanyao_action = 0;
            }
            else
            {
                Constants.Set_shuiben_duanyao_action = 1;
            }
            Constants.Set_PumpDianji_Flag = 1;
            MainWindow.pWin.Function_Wait_ExternShow();
        }

        private void Left_Move_Btn_Click(object sender, RoutedEventArgs e)
        {
            if (Ay_FlightModel_Index > 1)
            {
                Ay_FlightModel_Index = (Ay_FlightModel_Index - 1);
                Function_List_FlightModel(Ay_FlightModel_Index);
                PhotoCanvas.Index = Ay_FlightModel_Index;
            }
            else
            {
                System.Windows.MessageBox.Show("It Has Been The First");
            }
        }

        private void Right_Move_Btn_Click(object sender, RoutedEventArgs e)
        {
            if (Ay_FlightModel_Index < 14)
            {
                Ay_FlightModel_Index = (Ay_FlightModel_Index + 1);
                Function_List_FlightModel(Ay_FlightModel_Index);
                PhotoCanvas.Index = Ay_FlightModel_Index;
            }
            else
            {
                System.Windows.MessageBox.Show("It Has Been The Last");
            }
        }

        private void LeftBattery_Move_Btn_Click(object sender, RoutedEventArgs e)
        {
            if (Ay_BatteryModel_Index > 1)
            {
                Ay_BatteryModel_Index = (Ay_BatteryModel_Index - 1);
                Function_List_BatteryModel(Ay_BatteryModel_Index);
                BatteryPhotoCanvas.Index = Ay_BatteryModel_Index;
            }
            else
            {
                System.Windows.MessageBox.Show("It Has Been The First");
            }
        }

        private void RightBattery_Move_Btn_Click(object sender, RoutedEventArgs e)
        {
            if (Ay_BatteryModel_Index < 4)
            {
                Ay_BatteryModel_Index = (Ay_BatteryModel_Index + 1);
                Function_List_BatteryModel(Ay_BatteryModel_Index);
                BatteryPhotoCanvas.Index = Ay_BatteryModel_Index;
            }
            else
            {
                System.Windows.MessageBox.Show("It Has Been The Last");
            }
        }

        private void LeftYK_Move_Btn_Click(object sender, RoutedEventArgs e)
        {

            if (Ay_TelModel_Index > 1)
            {
                Ay_TelModel_Index = (Ay_TelModel_Index - 1);
                Function_List_TelModel(Ay_TelModel_Index);
                TelPhotoCanvas.Index = Ay_TelModel_Index;
            }
            else
            {
                System.Windows.MessageBox.Show("It Has Been The First");
            }
        }

        private void RightYK_Move_Btn_Click(object sender, RoutedEventArgs e)
        {
            if (Ay_TelModel_Index < 2)
            {
                Ay_TelModel_Index = (Ay_TelModel_Index + 1);
                Function_List_TelModel(Ay_TelModel_Index);
                TelPhotoCanvas.Index = Ay_TelModel_Index;
            }
            else
            {
                System.Windows.MessageBox.Show("It Has Been The Last");
            }
        }

        private void Fog_Slider_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Constants.Set_Pump_Flag = 1;
        }

        private void Fog_Slider_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Constants.Set_Pump_Flag = 0;
        }

        private void Fog_ConstantSlider_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Constants.Set_Pump_Flag = 1;
        }

        private void Fog_ConstantSlider_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Constants.Set_Pump_Flag = 0;
        }

        private void Fog_ConstantPumpSlider_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Constants.Set_Pump_Flag = 1;
        }

        private void Fog_ConstantPumpSlider_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Constants.Set_Pump_Flag = 0;
        }
        public void Function_YKQ_CHANGENEXTSTEP()
        {
            YKQ_Middle_Next.Content = "Next";
        }

        private void AM_YKQ_Reback_Set()//美国手的对应机制
        {
            int am_left_Y = (int)((Constants.Show_RebackOrNotReback >> 2) & 0x0001);//throttle
            int am_left_X = (int)((Constants.Show_RebackOrNotReback >> 3) & 0x0001);//yaw
            int am_right_X = (int)(Constants.Show_RebackOrNotReback & 0x0001);//roll
            int am_right_Y = (int)((Constants.Show_RebackOrNotReback >> 1) & 0x0001);//pitch
            int am_switch_A = (int)((Constants.Show_RebackOrNotReback >> 4) & 0x0001);//switcha
            int am_switch_B = (int)((Constants.Show_RebackOrNotReback >> 5) & 0x0001);//switchb
            int am_switch_C = (int)((Constants.Show_RebackOrNotReback >> 6) & 0x0001);//switchc
            int am_switch_D = (int)((Constants.Show_RebackOrNotReback >> 7) & 0x0001);//switchd
            Constants.AutoWing_Tel_StepsValue = (int)Constants.Show_RebackOrNotReback;

            if (am_left_X == 1)//1
            {
                m_leftcheck_amercia_x.IsChecked = true;
            }
            else
            {
                m_leftcheck_amercia_x.IsChecked = false;
            }
            if (am_left_Y == 1)//2
            {
                m_leftcheck_amercia_y.IsChecked = true;
            }
            else
            {
                m_leftcheck_amercia_y.IsChecked = false;
            }
            if (am_right_X == 1)//3
            {
                m_rightcheck_amercia_x.IsChecked = true;
            }
            else
            {
                m_rightcheck_amercia_x.IsChecked = false;
            }
            if (am_right_Y == 1)//4
            {
                m_rightcheck_amercia_y.IsChecked = true;
            }
            else
            {
                m_rightcheck_amercia_y.IsChecked = false;
            }
            if (am_switch_A == 1)//5
            {
                m_check_sw1.IsChecked = true;
            }
            else
            {
                m_check_sw1.IsChecked = false;
            }
            if (am_switch_B == 1)//6
            {
                m_check_sw2.IsChecked = true;
            }
            else
            {
                m_check_sw2.IsChecked = false;
            }
            if (am_switch_C == 1)//7
            {
                m_check_sw3.IsChecked = true;
            }
            else
            {
                m_check_sw3.IsChecked = false;
            }
            if (am_switch_D == 1)//8
            {
                m_check_sw4.IsChecked = true;
            }
            else
            {
                m_check_sw4.IsChecked = false;
            }
        }
        private void JA_YKQ_Reback_Set()//日本手的对应机制
        {
            int am_right_Y = (int)((Constants.Show_RebackOrNotReback >> 2) & 0x0001);//yaw
            int am_left_Y = (int)((Constants.Show_RebackOrNotReback >> 1) & 0x0001);//pitch
            int am_right_X = (int)(Constants.Show_RebackOrNotReback & 0x0001);//roll
            int am_left_X = (int)((Constants.Show_RebackOrNotReback >> 3) & 0x0001);//throttle
            int am_switch_A = (int)((Constants.Show_RebackOrNotReback >> 4) & 0x0001);//switcha
            int am_switch_B = (int)((Constants.Show_RebackOrNotReback >> 5) & 0x0001);//switchb
            int am_switch_C = (int)((Constants.Show_RebackOrNotReback >> 6) & 0x0001);//switchc
            int am_switch_D = (int)((Constants.Show_RebackOrNotReback >> 7) & 0x0001);//switchd
            Constants.AutoWing_Tel_StepsValue = (int)Constants.Show_RebackOrNotReback;
            if (am_left_X == 1)//1
            {
                m_leftcheck_amercia_x.IsChecked = true;
            }
            else
            {
                m_leftcheck_amercia_x.IsChecked = false;
            }
            if (am_left_Y == 1)//2
            {
                m_leftcheck_amercia_y.IsChecked = true;
            }
            else
            {
                m_leftcheck_amercia_y.IsChecked = false;
            }
            if (am_right_X == 1)//3
            {
                m_rightcheck_amercia_x.IsChecked = true;
            }
            else
            {
                m_rightcheck_amercia_x.IsChecked = false;
            }
            if (am_right_Y == 1)//4
            {
                m_rightcheck_amercia_y.IsChecked = true;
            }
            else
            {
                m_rightcheck_amercia_y.IsChecked = false;
            }
            if (am_switch_A == 1)//5
            {
                m_check_sw1.IsChecked = true;
            }
            else
            {
                m_check_sw1.IsChecked = false;
            }
            if (am_switch_B == 1)//6
            {
                m_check_sw2.IsChecked = true;
            }
            else
            {
                m_check_sw2.IsChecked = false;
            }
            if (am_switch_C == 1)//7
            {
                m_check_sw3.IsChecked = true;
            }
            else
            {
                m_check_sw3.IsChecked = false;
            }
            if (am_switch_D == 1)//8
            {
                m_check_sw4.IsChecked = true;
            }
            else
            {
                m_check_sw4.IsChecked = false;
            }

        }
        private void YKQ_Middle_Next_Click(object sender, RoutedEventArgs e)
        {
            string cancelornext_string = (string)YKQ_Middle_Next.Content;
            if (cancelornext_string == "Cancel")
            {
                ////////////////若这边检测到的名字是退出就发送校准退出的值////////////////
                MainWindow.pWin.Function_Wait_ExternShow();
                Constants.AutoWing_Tel_Modifynext_Flag = 0;//将方向修改的发送部分停止
                Constants.AutoWing_Tel_Steps = 10;//退出校准//恢复到主界面
                Constants.AutoWing_Tel_Control_flag = 0;//遥控器校准开始标志
                Constants.AutoWing_Tel_Finish_flag = 0;//遥控器校准完成进入修正界面
                Constants.AutoWing_Tel_Modifymiddle_Flag = 0;//遥控器中点修正标志
                Constants.AutoWing_Tel_Modifynext_Flag = 0;//遥控器中点修正界面下一步
                Constants.AutoWing_Tel_Makesure_flag = 1;//开启遥控器方向确认发送标志
                //////////////返回到一开始的校准界面
                m_TabControl_Title.SelectedIndex = 9;//校准界面
                ////////////////若这边检测到的名字是退出就发送校准退出的值////////////////
            }
            else
            {
                m_TabControl_Title.SelectedIndex = 10;//方向确认
                Constants.AutoWing_Tel_Steps = 9;//中点完成下一步发送的值
                Constants.AutoWing_Tel_StepsValue = 0;//正反方向考虑的需要通过勾选来改变值
                Constants.AutoWing_Tel_Control_flag = 0;//遥控器校准开始标志
                Constants.AutoWing_Tel_Finish_flag = 0;//遥控器校准完成进入修正界面
                Constants.AutoWing_Tel_Modifymiddle_Flag = 0;//遥控器中点修正标志
                Constants.AutoWing_Tel_Makesure_flag = 0;//遥控器校准方向确定标志
                Constants.AutoWing_Tel_Modifynext_Flag = 1;//中点修正中的下一步标志位
                if (Which_Country_Flag == 0)//如果是美国手
                {
                    /////////////////////显示为美国手的界面///////////////////////////
                    try
                    {
                        America_radio.IsChecked = true;
                        AM_YKQ_Reback_Set();
                        m_leftcheck_amercia_x.Content = "航向反向";
                        m_leftcheck_amercia_y.Content = "油门反向";
                        m_rightcheck_amercia_x.Content = "横滚反向";
                        m_rightcheck_amercia_y.Content = "俯仰反向";
                    }
                    catch (Exception)
                    {

                    }
                }
                if (Which_Country_Flag == 1)//如果是日本手
                {
                    /////////////////////显示为日本手的界面///////////////////////////
                    try
                    {
                        Japan_radio.IsChecked = true;
                        JA_YKQ_Reback_Set();
                        m_leftcheck_amercia_x.Content = "航向反向";
                        m_leftcheck_amercia_y.Content = "俯仰反向";
                        m_rightcheck_amercia_x.Content = "横滚反向";
                        m_rightcheck_amercia_y.Content = "油门反向";
                    }
                    catch (Exception)
                    {

                    }
                }
            }
        }

        private void Ykq_Middle_Modify_Click(object sender, RoutedEventArgs e)
        {
            MainWindow.pWin.Function_Wait_ExternShow();
            Constants.AutoWing_Tel_Steps = 6;//中点修正发的值是6
            Constants.AutoWing_Tel_StepsValue = 0;//正反方向为0这个时候是不考虑的
            Constants.AutoWing_Tel_Control_flag = 0;//遥控器校准开始标志
            Constants.AutoWing_Tel_Finish_flag = 0;//遥控器校准完成进入修正界面
            Constants.AutoWing_Tel_Modifynext_Flag = 0;//遥控器中点修正界面下一步
            Constants.AutoWing_Tel_Makesure_flag = 0;//遥控器校准方向确定标志
            Constants.AutoWing_Tel_Modifymiddle_Flag = 1;//中点修正标志位
        }
        private void Left_X_Checked(object sender, RoutedEventArgs e)//左边摇杆部分X选中
        {
            if (Which_Country_Flag == 0)
            {
                yaw_if_reverse = 1;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
            if (Which_Country_Flag == 1)
            {
                yaw_if_reverse = 1;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
        }
        private void Left_X_UnChecked(object sender, RoutedEventArgs e)//左边摇杆部分X没选中
        {
            if (Which_Country_Flag == 0)
            {
                yaw_if_reverse = 0;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
            if (Which_Country_Flag == 1)
            {
                yaw_if_reverse = 0;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
        }
        private void Left_Y_Checked(object sender, RoutedEventArgs e)//左边摇杆部分Y选中
        {
            if (Which_Country_Flag == 0)
            {
                throttle_if_reverse = 1;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
            if (Which_Country_Flag == 1)
            {
                pitch_if_reverse = 1;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
        }
        private void Left_Y_UnChecked(object sender, RoutedEventArgs e)//左边摇杆部分Y没选中
        {
            if (Which_Country_Flag == 0)
            {
                throttle_if_reverse = 0;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
            if (Which_Country_Flag == 1)
            {
                pitch_if_reverse = 0;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
        }
        private void Right_X_Checked(object sender, RoutedEventArgs e)//右边摇杆部分X选中
        {
            if (Which_Country_Flag == 0)
            {
                roll_if_reverse = 1;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
            if (Which_Country_Flag == 1)
            {
                roll_if_reverse = 1;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
        }
        private void Right_X_UnChecked(object sender, RoutedEventArgs e)//右边摇杆部分X没选中
        {
            if (Which_Country_Flag == 0)
            {
                roll_if_reverse = 0;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
            if (Which_Country_Flag == 1)
            {
                roll_if_reverse = 0;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
        }
        private void Right_Y_Checked(object sender, RoutedEventArgs e)//右边摇杆部分Y选中
        {
            if (Which_Country_Flag == 0)
            {
                pitch_if_reverse = 1;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
            if (Which_Country_Flag == 1)
            {
                throttle_if_reverse = 1;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
        }
        private void Right_Y_UnChecked(object sender, RoutedEventArgs e)//右边摇杆部分Y没选中
        {
            if (Which_Country_Flag == 0)
            {
                pitch_if_reverse = 0;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
            if (Which_Country_Flag == 1)
            {
                throttle_if_reverse = 0;
                Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                       | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
            }
        }
        private void SW1_Checked(object sender, RoutedEventArgs e)//开关1选中
        {
            switcha_reverse = 1;
            Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                   | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
        }
        private void SW1_UnChecked(object sender, RoutedEventArgs e)//开关1未选中
        {
            switcha_reverse = 0;
            Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                   | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
        }
        private void SW2_Checked(object sender, RoutedEventArgs e)//开关2选中
        {
            switchb_reverse = 1;
            Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                   | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
        }
        private void SW2_UnChecked(object sender, RoutedEventArgs e)//开关2未选中
        {
            switchb_reverse = 0;
            Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                   | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
        }
        private void SW3_Checked(object sender, RoutedEventArgs e)//开关3选中
        {
            switchc_reverse = 1;
            Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                   | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
        }
        private void SW3_UnChecked(object sender, RoutedEventArgs e)//开关3未选中
        {
            switchc_reverse = 0;
            Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                   | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
        }
        private void SW4_Checked(object sender, RoutedEventArgs e)//开关4选中
        {
            switchd_reverse = 1;
            Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                   | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
        }
        private void SW4_UnChecked(object sender, RoutedEventArgs e)//开关4未选中
        {
            switchd_reverse = 0;
            Constants.AutoWing_Tel_StepsValue = (((roll_if_reverse) & 0x01) | ((pitch_if_reverse << 1) & 0x02) | ((throttle_if_reverse << 2) & 0x04) | ((yaw_if_reverse << 3) & 0x08)
                                                   | ((switcha_reverse << 4) & 0x10) | ((switchb_reverse << 5) & 0x20) | ((switchc_reverse << 6) & 0x40) | ((switchd_reverse << 7) & 0x80));
        }

        private void RadioButton_Checked_2(object sender, RoutedEventArgs e)
        {
            Which_Country_Flag = 0;//美国手

        }

        private void RadioButton_Checked_3(object sender, RoutedEventArgs e)
        {
            Which_Country_Flag = 1;//日本手
        }

        private void Btn_MagAndABIndex_Set_Click(object sender, RoutedEventArgs e)
        {
            if (Edit_Mag_OffsetAngle.Text == "" || Edit_AB_ApproachLine_Range.Text == "" || Edit_AB_ApproachLine_DeadZone.Text == "" || Edit_AB_ApproachPoint_Range.Text == "" || Edit_AB_HorizoMove_Acc.Text == ""
                || Edit_AB_HorizoMove_Range.Text == "" || Edit_AB_FowardMove_Acc.Text == "" || Edit_AB_VelToPosition_Percent.Text == "" || Edit_AB_BToThreshold_Percent.Text == "" || Edit_AB_AToThreshold_Percen.Text == ""
                || Edit_AB_BackThreshold_Value.Text == "")
            {
                System.Windows.MessageBox.Show("Please Enter Relevant Index");
                return;
            }
            else
            {
                Constants.Mag_Offset_Angle_Set = Convert.ToInt16(Edit_Mag_OffsetAngle.Text);//磁偏角
                Constants.AB_ApproachLine_Range_Set = (float)Convert.ToDouble(Edit_AB_ApproachLine_Range.Text) * 10;//追线距离限幅
                Constants.AB_ApproachLine_DeadZone_Set = (float)Convert.ToDouble(Edit_AB_ApproachLine_DeadZone.Text) * 10;//追线距离死区
                Constants.AB_ApproachPoint_Range_Set = (float)Convert.ToDouble(Edit_AB_ApproachPoint_Range.Text) * 10;//追点距离限幅
                Constants.AB_HorizoMove_Acc_Set = (float)Convert.ToDouble(Edit_AB_HorizoMove_Acc.Text) * 10;//AB点平移时加速度
                Constants.AB_HorizoMove_Range_Set = (float)Convert.ToDouble(Edit_AB_HorizoMove_Range.Text);//AB点平移距离限幅
                Constants.AB_FowardMove_Acc_Set = (float)Convert.ToDouble(Edit_AB_FowardMove_Acc.Text) * 10;//AB点前行时加速度
                Constants.AB_VelToPosition_Percent_Set = (float)Convert.ToDouble(Edit_AB_VelToPosition_Percent.Text) * 100;//速度到位置比例系数
                Constants.AB_BToThreshold_Percent_Set = (float)Convert.ToDouble(Edit_AB_BToThreshold_Percent.Text) * 10;//AB点B点到达Threshold比例
                Constants.AB_AToThreshold_Percen_Set = (float)Convert.ToDouble(Edit_AB_AToThreshold_Percen.Text) * 10; //AB点A点到达Threshold比例
                Constants.AB_BackThreshold_Set = (float)Convert.ToDouble(Edit_AB_BackThreshold_Value.Text) * 10; //AB点返航阈值
                /////////////////////////////////////限制输入范围部分//////////////////////////////////////////////////////////////////////////////////////
                if (Constants.Mag_Offset_Angle_Set < -20 || Constants.Mag_Offset_Angle_Set > 20)
                {
                    System.Windows.MessageBox.Show("Mag Angle Is Between -20 And 20");
                    return;
                }
                if (Constants.AB_ApproachLine_Range_Set < 0 || Constants.AB_ApproachLine_Range_Set > 80)
                {
                    System.Windows.MessageBox.Show("Line Range Is Between 0m And 8m");
                    return;
                }
                if (Constants.AB_ApproachLine_DeadZone_Set < 0 || Constants.AB_ApproachLine_DeadZone_Set > 10)
                {
                    System.Windows.MessageBox.Show("Line Distance Is Between 0m And 1m");
                    return;
                }
                if (Constants.AB_ApproachPoint_Range_Set < 0 || Constants.AB_ApproachPoint_Range_Set > 10)
                {
                    System.Windows.MessageBox.Show("Point Range Is Between 0m And 1m");
                    return;
                }
                if (Constants.AB_HorizoMove_Acc_Set < 0 || Constants.AB_HorizoMove_Acc_Set > 100)
                {
                    System.Windows.MessageBox.Show("ABPoint Acc Is Between 0m/s^2 And 10m/s^2");
                    return;
                }
                if (Constants.AB_HorizoMove_Range_Set < 0 || Constants.AB_HorizoMove_Range_Set > 40)
                {
                    System.Windows.MessageBox.Show("ABPoint Range Is Between 0m And 40m");
                    return;
                }
                if (Constants.AB_FowardMove_Acc_Set < 0 || Constants.AB_FowardMove_Acc_Set > 60)
                {
                    System.Windows.MessageBox.Show("ABPoint Forward Acc Is Between 0m/s^2 And 6m/s^2");
                    return;
                }
                if (Constants.AB_VelToPosition_Percent_Set < 0 || Constants.AB_VelToPosition_Percent_Set > 60)
                {
                    System.Windows.MessageBox.Show("Vel To Position Percent Is Between 0 And 0.6");
                    return;
                }
                if (Constants.AB_BToThreshold_Percent_Set < 2 || Constants.AB_BToThreshold_Percent_Set > 30)
                {
                    System.Windows.MessageBox.Show("B Point To Threshold Percent Is Between 0.2 And 3");
                    return;
                }
                if (Constants.AB_AToThreshold_Percen_Set < 2 || Constants.AB_AToThreshold_Percen_Set > 10)
                {
                    System.Windows.MessageBox.Show("A Point To Threshold Percent Is Between 0.2 And 1");
                    return;
                }
                if (Constants.AB_BackThreshold_Set < 2 || Constants.AB_BackThreshold_Set > 10)
                {
                    System.Windows.MessageBox.Show("Reback Value Percent Is Between 0.2 And 1");
                    return;
                }

                MainWindow.pWin.Function_Wait_ExternShow();
                Constants.Set_Mag_And_AB_Flag = 1;
            }
        }

        private void slider_ziwen_pid_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if(lock_pid_flag==1)
            {
                int value = (Int32)slider_ziwen_pid.Value * 10;
                label_ziwen_pid.Content = value.ToString() + "%";
                ///////////////////////////////根据进度条推算公式/////////////////////////////////////
                ////////////////////////p=50+80*杆量////////////////////////////////////////////////
                ///////////////////////I=125+200*杆量///////////////////////////////////////////////
                ///////////////////////D=2+4*杆量////////////////////////////////////////////////////
                Constants.Set_gyro_kp_rp = 50 + 80 * (value / (float)100.0);
                Constants.Set_gyro_ki_rp = 125 + 200 * (value / (float)100.0);
                Constants.Set_gyro_kd_rp = 2 + 4 * (value / (float)100.0);
                ////////////////////////p=250+200*杆量////////////////////////////////////////////////
                ///////////////////////I=150+200*杆量///////////////////////////////////////////////
                ///////////////////////D=0////////////////////////////////////////////////////
                Constants.Set_gyro_kp_y = 250 + 200 * (value / (float)100.0);
                Constants.Set_gyro_ki_y = 150 + 200 * (value / (float)100.0);
                Constants.Set_gyro_kd_y = 0;
                Constants.Set_prop_kp_rp = (float)0.3;
                Constants.Set_prop_ki_rp = (float)0.6;
                Constants.Set_prop_kp_y = (float)0.6;
                Constants.Set_prop_ki_y = (float)0.3;


                Edit_GyproKp_Rp.Text = Constants.Set_gyro_kp_rp.ToString("#0.00"); ;
                Edit_GyproKi_Rp.Text = Constants.Set_gyro_ki_rp.ToString("#0.00"); ;
                Edit_GyproKd_Rp.Text = Constants.Set_gyro_kd_rp.ToString("#0.00"); ;
                Edit_GyproKp_Y.Text = Constants.Set_gyro_kp_y.ToString("#0.00"); ;
                Edit_GyproKi_Y.Text = Constants.Set_gyro_ki_y.ToString("#0.00"); ;
                Edit_GyproKd_Y.Text = Constants.Set_gyro_kd_y.ToString("#0.00"); ;
                Edit_PropKp_Rp.Text = Constants.Set_prop_kp_rp.ToString("#0.00"); ;
                Edit_PropKi_Rp.Text = Constants.Set_prop_ki_rp.ToString("#0.00"); ;
                Edit_PropKp_Y.Text = Constants.Set_prop_kp_y.ToString("#0.00"); ;
                Edit_PropKi_Y.Text = Constants.Set_prop_ki_y.ToString("#0.00"); ;
            }


        }

        private void slider_ziwen_pid_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            lock_pid_flag = 1;
        }

        private void slider_ziwen_pid_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            lock_pid_flag = 0;
        }
        ////////////////////////////////////////////姿态pid roll pitch yaw杆量//////////////////////////////////////


        private void slider_zitai_pid_ValueChanged1(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            ///////////////////////////////根据进度条roll pitch yaw推算公式/////////////////////////////////////
            ////////////////////////p=20+80*杆量////////////////////////////////////////////////
            ///////////////////////I=0///////////////////////////////////////////////
            ///////////////////////D=0////////////////////////////////////////////////////
            int value = (Int32)slider_zitai_pid1.Value * 10;
            label_zitai_pid1.Content = value.ToString() + "%";
            Constants.Set_zitai_p = 20 + 80 * (value / (float)100.0);
            Constants.Set_zitai_i = 0;
            Constants.Set_zitai_d = 0;
            Constants.Set_hangxiang_p = 10 + 40 * (value / (float)100.0);
            Constants.Set_hangxiang_i = 0;
            Constants.Set_hangxiang_d = 0;
            ///////////////////////////////根据进度条速度推算公式/////////////////////////////////////
            ////////////////////////p=20////////////////////////////////////////////////
            ///////////////////////I=20+60*杆量///////////////////////////////////////////////
            ///////////////////////D=0////////////////////////////////////////////////////
            int sudu_value = (Int32)slider_zitai_pid3.Value * 10;
            Constants.Set_sudu_p = 20;
            Constants.Set_sudu_i = 20 + 60 * (sudu_value / (float)100.0);
            Constants.Set_sudu_d = 0;
            ///////////////////////////////根据进度条高度推算公式/////////////////////////////////////
            ////////////////////////p=10+40*杆量////////////////////////////////////////////////
            ///////////////////////I=10+20*杆量///////////////////////////////////////////////
            ///////////////////////D=30+20*杆量////////////////////////////////////////////////////
            int gaodu_value = (Int32)slider_zitai_pid2.Value * 10;
            Constants.Set_gaodu_p = 10 + 40 * (gaodu_value / (float)100.0);
            Constants.Set_gaodu_i = 10 + 20 * (gaodu_value / (float)100.0);
            Constants.Set_gaodu_d = 30 + 20 * (gaodu_value / (float)100.0);

        }
        private void slider_zitai_pid1_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Constants.AutoWing_ShangCengPIDSet_Flag = 1;//开启PID设置发送标志
        }

        private void slider_zitai_pid1_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Constants.AutoWing_ShangCengPIDSet_Flag = 0;//开启PID设置发送标志
        }
        ////////////////////////////////////////////姿态pid 高度杆量////////////////////////////////////////////////
        private void slider_zitai_pid_ValueChanged2(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            ///////////////////////////////根据进度条roll pitch yaw推算公式/////////////////////////////////////
            ////////////////////////p=20+80*杆量////////////////////////////////////////////////
            ///////////////////////I=0///////////////////////////////////////////////
            ///////////////////////D=0////////////////////////////////////////////////////
            int value = (Int32)slider_zitai_pid1.Value * 10;
            Constants.Set_zitai_p = 20 + 80 * (value / (float)100.0);
            Constants.Set_zitai_i = 0;
            Constants.Set_zitai_d = 0;
            Constants.Set_hangxiang_p = 20 + 60 * (value / (float)100.0);
            Constants.Set_hangxiang_i = 0;
            Constants.Set_hangxiang_d = 0;
            ///////////////////////////////根据进度条速度推算公式/////////////////////////////////////
            ////////////////////////p=20////////////////////////////////////////////////
            ///////////////////////I=20+60*杆量///////////////////////////////////////////////
            ///////////////////////D=0////////////////////////////////////////////////////
            int sudu_value = (Int32)slider_zitai_pid3.Value * 10;
            Constants.Set_sudu_p = 20;
            Constants.Set_sudu_i = 20 + 60 * (sudu_value / (float)100.0);
            Constants.Set_sudu_d = 0;
            ///////////////////////////////根据进度条高度推算公式/////////////////////////////////////
            ////////////////////////p=10+40*杆量////////////////////////////////////////////////
            ///////////////////////I=10+20*杆量///////////////////////////////////////////////
            ///////////////////////D=30+20*杆量////////////////////////////////////////////////////
            int gaodu_value = (Int32)slider_zitai_pid2.Value * 10;
            label_zitai_pid2.Content = gaodu_value.ToString() + "%";
            Constants.Set_gaodu_p = 10 + 40 * (gaodu_value / (float)100.0);
            Constants.Set_gaodu_i = 10 + 20 * (gaodu_value / (float)100.0);
            Constants.Set_gaodu_d = 30 + 20 * (gaodu_value / (float)100.0);
        }
        private void slider_zitai_pid2_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Constants.AutoWing_ShangCengPIDSet_Flag = 1;//开启PID设置发送标志
        }

        private void slider_zitai_pid2_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Constants.AutoWing_ShangCengPIDSet_Flag = 0;//开启PID设置发送标志
        }
        ////////////////////////////////////////////姿态pid速度杆量/////////////////////////////////////////////////
        private void slider_zitai_pid_ValueChanged3(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            ///////////////////////////////根据进度条roll pitch yaw推算公式/////////////////////////////////////
            ////////////////////////p=20+80*杆量////////////////////////////////////////////////
            ///////////////////////I=0///////////////////////////////////////////////
            ///////////////////////D=0////////////////////////////////////////////////////
            int value = (Int32)slider_zitai_pid1.Value * 10;
            Constants.Set_zitai_p = 20 + 80 * (value / (float)100.0);
            Constants.Set_zitai_i = 0;
            Constants.Set_zitai_d = 0;
            Constants.Set_hangxiang_p = 20 + 60 * (value / (float)100.0);
            Constants.Set_hangxiang_i = 0;
            Constants.Set_hangxiang_d = 0;
            ///////////////////////////////根据进度条速度推算公式/////////////////////////////////////
            ////////////////////////p=20////////////////////////////////////////////////
            ///////////////////////I=20+60*杆量///////////////////////////////////////////////
            ///////////////////////D=0////////////////////////////////////////////////////
            int sudu_value = (Int32)slider_zitai_pid3.Value * 10;
            label_zitai_pid3.Content = sudu_value.ToString() + "%";
            Constants.Set_sudu_p = 20;
            Constants.Set_sudu_i = 20 + 60 * (sudu_value / (float)100.0);
            Constants.Set_sudu_d = 0;
            ///////////////////////////////根据进度条高度推算公式/////////////////////////////////////
            ////////////////////////p=10+40*杆量////////////////////////////////////////////////
            ///////////////////////I=10+20*杆量///////////////////////////////////////////////
            ///////////////////////D=30+20*杆量////////////////////////////////////////////////////
            int gaodu_value = (Int32)slider_zitai_pid2.Value * 10;
            Constants.Set_gaodu_p = 10 + 40 * (gaodu_value / (float)100.0);
            Constants.Set_gaodu_i = 10 + 20 * (gaodu_value / (float)100.0);
            Constants.Set_gaodu_d = 30 + 20 * (gaodu_value / (float)100.0);
        }

        private void slider_zitai_pid3_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Constants.AutoWing_ShangCengPIDSet_Flag = 1;//开启PID设置发送标志
        }

        private void slider_zitai_pid3_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Constants.AutoWing_ShangCengPIDSet_Flag = 0;//开启PID设置发送标志
        }

        private void Aerial_Mode_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 17;//航拍的设置主界面
            Constants.Set_UAV_CurrentStyle = 1;//设置的参数赋值为1置为航拍
            Constants.AutoWing_HangPaiAndZhibaoChangge_Flag = 1;//开启航拍模式的发送
            MainWindow.pWin.Function_Wait_ExternShow();
        }

        private void Plant_Protection_Mode_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 17;//植保的设置主界面
            Constants.Set_UAV_CurrentStyle = 0;//设置的参数赋值为0置为植保
            Constants.AutoWing_HangPaiAndZhibaoChangge_Flag = 1;//开启航拍模式的发送
            MainWindow.pWin.Function_Wait_ExternShow();
        }

        private void Basis_Set_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 1;//基础设置切换到基础的选择界面
            Constants.Jump_To5or50Hz_Flag = 0;
        }

        private void Workstyle_Set_Click(object sender, RoutedEventArgs e)
        {
            if (Constants.Show_UAV_CurrentStyle == 1)
            {
                m_TabControl_Title.SelectedIndex = 18;//作业设置切换到航拍参数修改界面
                Function_Hangpai_ShowInfo();
            }
            else
            {
                m_TabControl_Title.SelectedIndex = 19;//作业设置切换到植保参数修改界面
                Function_ZhiBaiZuoYe_ShowInfo();
            }

        }

        private void AdjustThe_Set_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 12;//上层PID设置
            Constants.Jump_To5or50Hz_Flag = 3;//切换到50hz下的上层pid包也就是data7
        }

        private void SecurityStrategy_Set_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 20;//安全策略
            Function_AnQuanCeLue_ShowInfo();
        }

        private void jichuxuanxiangback_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 16;//基础选项返回到植保航拍类型选择
            Constants.Jump_To5or50Hz_Flag = 0;
        }

        private void jitipeizhiback_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 17;//机体配置返回到基础选项
            Constants.Jump_To5or50Hz_Flag = 0;
        }

        private void jitixuanzeback_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 1;//机体选择返回到机体配置
        }

        private void qifeigaoduback_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 1;//飞行设置返回到机体配置
        }

        private void dianjiceshiback_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 1;//电机测试返回到机体配置
        }

        private void M9back_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 17;//航拍作业设置返回到基础选项
        }

        private void pensafuduback_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 17;//植保作业设置返回到基础选项
        }

        private void shuibengback_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 19;//水泵返回到植保作业选项
        }

        private void cipianjiaoback_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 19;//磁偏角返回到植保作业选项
        }

        private void zitaitiaojieback_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 17;//姿态调参返回到基础选项
            Constants.Jump_To5or50Hz_Flag = 0;
        }

        private void ziwentiaojieback_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 12;//上层PID设置
            Constants.Jump_To5or50Hz_Flag = 3;//切换到50hz下的上层pid包也就是data7
        }

        private void anquancelueback_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 17;//安全策略返回到基础选项
            Constants.Jump_To5or50Hz_Flag = 0;
        }

        private void shujuhuifangback_Click(object sender, RoutedEventArgs e)
        {
            m_TabControl_Title.SelectedIndex = 17;//数据回放返回到基础选项
            Constants.Jump_To5or50Hz_Flag = 0;
        }

        private void AnquancelueHuoqu_Btn_Click(object sender, RoutedEventArgs e)
        {
            Function_AnQuanCeLue_ShowInfo();
        }

        private void AnquancelueShezhi_Btn_Click(object sender, RoutedEventArgs e)
        {
            if (Edit_FirstNotice_Volt.Text == "")
            {
                System.Windows.MessageBox.Show("1level Notice Voltage Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_Noticefirstvolt = (float)Convert.ToDouble(Edit_FirstNotice_Volt.Text);//一级电压报警
            }
            if (Constants.Set_UAV_Noticefirstvolt < 3.5 || Constants.Set_UAV_Noticefirstvolt > 3.9)
            {
                System.Windows.MessageBox.Show("1level Notice Voltage Is Between 3.5v And 3.9v");
                return;
            }

            if (Edit_SecondNotice_Volt.Text == "")
            {
                System.Windows.MessageBox.Show("2level Notice Voltage Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_Noticesecondvolt = (float)Convert.ToDouble(Edit_SecondNotice_Volt.Text);//一级电压报警
            }
            if (Constants.Set_UAV_Noticesecondvolt < 3.4 || Constants.Set_UAV_Noticesecondvolt > 3.8)
            {
                System.Windows.MessageBox.Show("2level Notice Voltage Is Between 3.4v And 3.8v");
                return;
            }

            Constants.Set_UAV_Noticesecondaction = M_Lowvolt_Action.SelectedIndex;
            Constants.Set_UAV_Noticelosecontrolaction = M_Losecontrol_Action.SelectedIndex;

            if (Edit_Enclosuree_Distance.Text == "")
            {
                System.Windows.MessageBox.Show("Crawl Distance Range Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_Noticedistance = (float)Convert.ToDouble(Edit_Enclosuree_Distance.Text);//围栏距离
            }
            if (Constants.Set_UAV_Noticedistance < 0 || Constants.Set_UAV_Noticedistance > 3000)
            {
                System.Windows.MessageBox.Show("Crawl Distance Range Is Between 0m And 3000m");
                return;
            }

            if (Edit_Enclosuree_Height.Text == "")
            {
                System.Windows.MessageBox.Show("Crawl Height Range Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_Noticeheight = (float)Convert.ToDouble(Edit_Enclosuree_Height.Text);//围栏高度
            }
            if (Constants.Set_UAV_Noticeheight < 0 || Constants.Set_UAV_Noticeheight > 1000)
            {
                System.Windows.MessageBox.Show("Crawl Height Range Is Between 0m And 1000m");
                return;
            }

            if (M_FangHangHeight.Text=="")
            {
                System.Windows.MessageBox.Show("Reback Height Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_Noticefanhanggaodu = (float)Convert.ToDouble(M_FangHangHeight.Text);//返航高度
            }
            if (Constants.Set_UAV_Noticefanhanggaodu < 0 || Constants.Set_UAV_Noticefanhanggaodu > 25)
            {
                System.Windows.MessageBox.Show("Reback Height Is Between 0m And 25m");
                return;
            }

            Constants.AutoWing_NoticeIndex_Flag = 1;//策略设置开启
            MainWindow.pWin.Function_Wait_ExternShow();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Function_ZhiBaiZuoYe_ShowInfo();
        }

        private void zhibaocanshushezhi_Click(object sender, RoutedEventArgs e)
        {
            if (Edit_Zhibao_Speed.Text == "")
            {
                System.Windows.MessageBox.Show("Plant Speed Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_ZhiBaoSpeed = (float)Convert.ToDouble(Edit_Zhibao_Speed.Text);//植保速度
            }
            if (Constants.Set_UAV_ZhiBaoSpeed < 0 || Constants.Set_UAV_ZhiBaoSpeed > 20)
            {
                System.Windows.MessageBox.Show("Plant Speed Is Between 0m/s And 20m/s");
                return;
            }

            if (Edit_Zhibao_Range.Text == "")
            {
                System.Windows.MessageBox.Show("Plant Range Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_ZhiBaoFuDu = (float)Convert.ToDouble(Edit_Zhibao_Range.Text);//植保幅值
            }
            if (Constants.Set_UAV_ZhiBaoFuDu < 0 || Constants.Set_UAV_ZhiBaoFuDu > 20)
            {
                System.Windows.MessageBox.Show("Plant Range Is Between 0m And 20m");
                return;
            }
            Constants.Set_UAV_ZhiBaoAction = m_TerrainFollowYesOrNo.SelectedIndex;
            Constants.AutoWing_ZhiBaoZuoYe_Flag = 1;//植保作业标志
            MainWindow.pWin.Function_Wait_ExternShow();

        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            Function_Hangpai_ShowInfo();
        }

        private void hangpaicanshushezhi_Click(object sender, RoutedEventArgs e)
        {
            if (Edit_Photo_Speed.Text == "")
            {
                System.Windows.MessageBox.Show("Aerial Speed Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_HangPaiSpeed = (float)Convert.ToDouble(Edit_Photo_Speed.Text);//航拍速度
            }
            if (Constants.Set_UAV_HangPaiSpeed < 1 || Constants.Set_UAV_HangPaiSpeed > 25)
            {
                System.Windows.MessageBox.Show("Aerial Speed Is Between 0m/s And 25m/s");
                return;
            }

            if (Edit_Photo_upSpeed.Text == "")
            {
                System.Windows.MessageBox.Show("Aerial Up Speed Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_HangPaiUpSpeed = (float)Convert.ToDouble(Edit_Photo_upSpeed.Text);//航拍上升速度
            }
            if (Constants.Set_UAV_HangPaiUpSpeed < 0.1 || Constants.Set_UAV_HangPaiUpSpeed > 6)
            {
                System.Windows.MessageBox.Show("Aerial Up Speed Is Between 0.1m/s And 6m/s");
                return;
            }
            if (Edit_Photo_downSpeed.Text == "")
            {
                System.Windows.MessageBox.Show("Aerial Down Speed Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_HangPaiDownSpeed = (float)Convert.ToDouble(Edit_Photo_downSpeed.Text);//航拍下降速度
            }
            if (Constants.Set_UAV_HangPaiDownSpeed < 0.1 || Constants.Set_UAV_HangPaiDownSpeed > 4)
            {
                System.Windows.MessageBox.Show("Aerial Down Speed Is Between 0.1m/s And 4m/s");
                return;
            }



            if (Edit_M9Low_Position.Text == "")
            {
                System.Windows.MessageBox.Show("M9 Low Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_M9Low = (float)Convert.ToDouble(Edit_M9Low_Position.Text);//M9低位
            }
            if (Constants.Set_UAV_M9Low < 0 || Constants.Set_UAV_M9Low > 5000)
            {
                System.Windows.MessageBox.Show("M9 Low Is Between 0 And 5000");
                return;
            }

            if (Edit_M9Middle_Position.Text == "")
            {
                System.Windows.MessageBox.Show("M9 Middle Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_M9Middle = (float)Convert.ToDouble(Edit_M9Middle_Position.Text);//M9中位
            }
            if (Constants.Set_UAV_M9Middle < 0 || Constants.Set_UAV_M9Middle > 5000)
            {
                System.Windows.MessageBox.Show("M9 Middle Is Between 0 And 5000");
                return;
            }

            if (Edit_M9High_Position.Text == "")
            {
                System.Windows.MessageBox.Show("M9 High Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_M9High = (float)Convert.ToDouble(Edit_M9High_Position.Text);//M9高位
            }
            if (Constants.Set_UAV_M9High < 0 || Constants.Set_UAV_M9High > 5000)
            {
                System.Windows.MessageBox.Show("M9 High Is Between 0 And 5000");
                return;
            }

            if (Edit_M10Low_Position.Text == "")
            {
                System.Windows.MessageBox.Show("M10 Low Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_M10Low = (float)Convert.ToDouble(Edit_M10Low_Position.Text);//M10低位
            }
            if (Constants.Set_UAV_M10Low < 0 || Constants.Set_UAV_M10Low > 5000)
            {
                System.Windows.MessageBox.Show("M10 Low Is Between 0 And 5000");
                return;
            }

            if (Edit_M10Middle_Position.Text == "")
            {
                System.Windows.MessageBox.Show("M10 Middle Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_M10Middle = (float)Convert.ToDouble(Edit_M10Middle_Position.Text);//M10中位
            }
            if (Constants.Set_UAV_M10Middle < 0 || Constants.Set_UAV_M10Middle > 5000)
            {
                System.Windows.MessageBox.Show("M10 Middle Is Between 0 And 5000");
                return;
            }

            if (Edit_M10High_Position.Text == "")
            {
                System.Windows.MessageBox.Show("M10 High Can Not Be Null");
                return;
            }
            else
            {
                Constants.Set_UAV_M10High = (float)Convert.ToDouble(Edit_M10High_Position.Text);//M10高位
            }
            if (Constants.Set_UAV_M10High < 0 || Constants.Set_UAV_M10High > 5000)
            {
                System.Windows.MessageBox.Show("M10 High Is Between 0 And 5000");
                return;
            }
            Constants.AutoWing_HangPai_Flag = 1;//航拍标志
            MainWindow.pWin.Function_Wait_ExternShow();
        }

        private void m_TerrainFollowYesOrNo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }


    }
}
