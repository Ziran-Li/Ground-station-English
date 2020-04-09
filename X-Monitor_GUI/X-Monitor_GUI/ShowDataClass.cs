using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XMonitor_GUI
{
    public class ShowDataClass
    {
        public float Roll=0;
        public float Pitch=0;
        public float Yaw = 0;
        public float P = 0;
        public float Q = 0;
        public float R = 0;
        public float XAcc = 0;
        public float YAcc = 0;
        public float ZAcc = 0;
        public float XMag = 0;
        public float YMag = 0;
        public float ZMag = 0;
        public long LRoll = 0;
        public long LPitch = 0;
        public long LYaw = 0;
        public long LPower = 0;
        public UInt32 BSw1 = 0;
        public UInt32 BSw2 = 0;
        public UInt32 BSw3 = 0;
        public UInt32 BSw4 = 0;
        public int AutoWingDataPacket = 0;
        public int AutoWingMarkerNums = 0;
        public int AutoWingWhichMarker = 0;
        public float Motor_F = 0;
        public float Motor_B = 0;
        public float Motor_L = 0;
        public float Motor_R = 0;
        public float Motor_X = 0;
        public float Motor_Y = 0;
        public float AutoWingTransverseVelocity = 0;
        public float AutoWingLongitudinalVelocity = 0;
        public float AutoWingUpAndDownVelocity = 0;
        public double AutoWingLatitude = 0.0;
        public double AutoWingLongitude = 0.0;
        public int AutoWingStarNum = 0;
        public float AutoWingAltitude = 0;
        public int AutoWingFlightMode = 0;
        public float AutoWingVoltValue = 0;
        public int AutoWingFlag = 0;
        public int AutoWingSensorState = 0;
        public int AutoWingCommunitationState = 0;
        public int AutoWingTakePictureFlag = 0;
        public int New_Data1_Undefined1 = 0;
        public int New_Data1_Undefined2 = 0;
        public int New_Data1_Undefined3 = 0;
        public int New_Data1_Undefined4 = 0;
        public int New_Data1_Undefined5 = 0;

        public int Prop_Failure_Code = 0;
        public float Zacc_Noise = 0;
        public float New_GPS_Dop = 0;
        public float New_Bara_Alt = 0;
        public float New_Frame_Type =0;
        public float New_Motor_Starting_Percent=0;
        public Int32 New_Param_Headfile_Num=0;
        public float New_Vesion_AP=0;
        public float New_Vesion_IMU=0;
        public float New_Ref_Roll=0;
        public float New_Kalman_Roll=0;
        public float New_U_Roll=0;
        public float  New_Ref_Pitch =0;
        public float  New_Kalman_Pitch=0;
        public float  New_U_Pitch=0;
        public float  New_Ref_Yaw=0;
        public float  New_Feedback_Yaw=0;
        public float   New_Kalman_Yaw=0;
        public float   New_U_Yaw=0;
        public float   New_Ref_Alt=0;
        public float   New_Kalman_Zacc=0;
        public float   New_Kalman_Vd=0;
        public float   New_Kalman_Alt=0;
        public float   New_U_Alt=0;
        public float   New_Ref_Vn=0;
        public float   New_Pos_N_Feedback=0;
        public float   New_Kalman_Vn=0;
        public float   New_Kalman_PosN=0;
        public float   New_U_PosN, New_Ref_Ve=0;
        public float    New_Pos_E_Feedback=0;
        public float   New_Kalman_Ve=0;
        public float   New_Kalman_PosE=0;
        public float    New_U_PosE=0;
        public float    New_Data2_Undefined1=0;
        public float    New_Data2_Undefined2=0;
        public float    New_Data2_Undefined3=0;
        public float   New_Data2_Undefined4=0;
        public float New_Data2_Undefined5 = 0;
        public float New_Total_Flow = 0;//新增流量值
        public float New_Current_Flow_Rate = 0;//新增当前流速
        public float New_AB_Point_Alloffset = 0;//ab点飞行过程中磁偏角及拐弯参数设置

        public int AutoWing_Tasete_Hour=0;
        public int AutoWing_Tasete_Second=0;
        public float Set_Pensa_Sudu=0;
        public float Set_Pensa_Juli=0;
        public float Set_Pensa_Banjing=0;
        public float Set_Pensa_Gaodu=0;
        public int XbeeRecv = 0;
        public float GPS_Heading = 0;

        public float Modify_Rollcontrol_Roll = 0;//Roll
        public float Modify_Rollcontrol_Rollrate = 0;//Rollrate
        public float Modify_Rollcontrol_Proproll = 0;//LRoll
        public float Modify_Yawcontrol_Yaw =  0;//Yaw
        public float Modify_Yawcontrol_Yawrate =  0;//Yawrate
        public float Modify_Yawcontrol_Propyaw =  0;//LYaw
        public float Modify_Altcontrol_Zacc =  0;//Zacc
        public float Modify_Altcontrol_Throttle =  0;//Lpower
        public float Modify_Altcontrol_Alt = 0;//GPS_Alt3
        public float New_Bara_Alt2=0;//Bara_Alt2
        public float New_GPS_Vd =  0;//GPS_Vd
        public float New_Roll_Ref =  0;//Roll_Ref
        public float New_Vel_Y =  0;//Vel_Y
        public float New_Pos_Y = 0;//Pos_Y
        public float New_Data4_Undefined1 = 0;//Undefined1
        public float New_Data4_Undefined2 =  0;//Undefined2
        public float New_Data4_Undefined3 =  0;//Undefined3
        public float New_Data4_Undefined4 = 0;//Undefined4
        public float Modify_Star_Num = 0;//Star_Num
        public float Modify_Volt = 0;//Voltag
        public float Gyro_RP_KP = 0;//Gyro_KP_RP
        public float Gyro_RP_KI = 0;//Gyro_KI_RP
        public float Gyro_RP_KD = 0;//Gyro_KD_RP
        public float Gyro_Y_KP = 0;//Gyro_KP_Y
        public float Gyro_Y_KI = 0;//Gyro_KI_Y
        public float Gyro_Y_KD = 0;//Gyro_KD_Y
        public float Prop_RP_KP =0;//Prop_KP_RP
        public float Prop_RP_KI =0;//Prop_KI_RP
        public float Prop_Y_KP = 0;//Prop_KP_Y
        public float Prop_Y_KI = 0;//Prop_KI_Y

        public long New_YK_roll = 0;//Prop_Roll
        public long New_YK_pitch = 0;//Prop_Pitch
        public long New_YK_throttle = 0;//Prop_Thr
        public long New_YK_yaw = 0;//Prop_Yaw
        public long New_YK_switch1 = 0;//Switch_A
        public long New_YK_switch2 = 0;//Switch_B
        public long New_YK_switch3 = 0;//Switch_C
        public long New_YK_switch4 = 0;//Switch_D


        public float Change_New_YK_roll = 0;
        public float Change_New_YK_pitch = 0;
        public float Change_New_YK_throttle = 0;
        public float Change_New_YK_yaw = 0;
        public float Change_New_YK_switch1 = 0;
        public float Change_New_YK_switch2 = 0;
        public float Change_New_YK_switch3 = 0;
        public float Change_New_YK_switch4 = 0;


        public long New_YK_Minroll = 0;//Prop_Roll_Min
        public long New_YK_Minpitch = 0;//Prop_Pitch_Min
        public long New_YK_Minthrottle = 0;//Prop_Thr_Min
        public long New_YK_MinYaw = 0;//Prop_Yaw_Min
        public long New_YK_Minswitch1 = 0;//Switch_A_Min
        public long New_YK_Minswitch2 = 0;//Switch_B_Min
        public long New_YK_Minswitch3 = 0;//Switch_C_Min
        public long New_YK_Minswitch4 = 0;//Switch_D_Min
        public long New_YK_Maxroll = 0;//Prop_Roll_Max
        public long New_YK_Maxpitch = 0;//Prop_Pitch_Max
        public long New_YK_Maxthrottle = 0;//Prop_Thr_Max
        public long New_YK_MaxYaw = 0;//Prop_Yaw_Max
        public long New_YK_Maxswitch1 = 0;//Switch_A_Max
        public long New_YK_Maxswitch2 = 0;//Switch_B_Max
        public long New_YK_Maxswitch3 = 0;//Switch_C_Max
        public long New_YK_Maxswitch4 = 0;//Switch_D_Max
        public long SuccessOrFail = 0;//Pop_Calibration_Step
        public long RebackOrNotReback = 0;
        public float New_Data5_Undefined1 = 0;//Undefined1
        public float New_Data5_Undefined2 = 0;//Undefined2
        public float New_Data5_Undefined3 = 0;//Undefined3


        public float ZiTai_P = 0;//姿态p
        public float ZiTai_I = 0;//姿态i
        public float ZiTai_D = 0;//姿态d
        public float HangXiang_P = 0;//航向p
        public float HangXiang_I = 0;//航向i
        public float HangXiang_D = 0;//航向d
        public float SuDu_P = 0;//速度p
        public float SuDu_I = 0;//速度i
        public float SuDu_D = 0;//速度d
        public float GaoDu_P = 0;//高度p
        public float GaoDu_I = 0;//高度i
        public float GaoDu_D = 0;//高度d

        public float Anquancelue_Code = 0;//安全策略编号
        public float Anquancelue_Value = 0;//安全策略编码对应的值
        public float Hangpaicanshu_Code = 0;//航拍对应的编号
        public float Hangpaicanshu_Value = 0;//航拍对应的值
        public float Feixingshezhi_Code = 0;//飞行设置对应的编号
        public float Feixingshezhi_Value = 0;//飞行设置对应的值
        public float Shuibenaction_Value = 0;//水泵动作
        public float Hangpaizhibao_Value = -1;//当前航拍植保值


        public float PlantProtectOperation = 0;
        public float Arialphotoparam = 0;
        public float basicflightparam = 0;
        public double AutoWingDestinationLongitude = 0.0;
        public double AutoWingDestinationLatitude = 0.0;
        public float ref_position = 0;
        public float velocityreference_vn = 0;
        public float velocityreference_ve = 0;
        public float attitudereference_roll = 0;
        public float attitudereference_pitch =0;
        public float stabillizeref_roll = 0;
        public float stabillizeref_pitch = 0;
        public float attitude_yaw = 0;
        public float feedback_yaw =0;
        public float stabillizeref_yaw =0;
        public float motor_roll = 0;
        public float  motor_pitch = 0;
        public float motor_yaw = 0;
        public float ref_alt = 0;
        public float altvel = 0;
        public float  feedback_vd = 0;
        public float  stabillizeref_throttle =0;
        public float  motor_m7 =0;
        public float  motor_m8 =0;
        public float  motor_m9 =0;
        public float motor_m10 = 0;
    }
}
