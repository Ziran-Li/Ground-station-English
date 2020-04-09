using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.ComponentModel;
using System.IO;
namespace XMonitor_GUI
{
    public class SerialPortModel : SerialPort, INotifyPropertyChanged
    {
        //public SerialPort Port { get; set; }

        //DispatcherTimer

        private string[] portnames;
        /// <summary>
        /// 所有端口号
        /// </summary>
        public string[] PortNames
        {
            get { return portnames; }
            set
            {
                portnames = value;
                RaisePropertyChanged("PortNames");
            }
        }

        private bool issend;
        /// <summary>
        /// 是否发送数据
        /// </summary>
        public bool IsSend
        {
            get { return issend; }
            set
            {
                issend = value;
                RaisePropertyChanged("IsSend");
            }
        }


        private int senddatacounter;

        public int SendDataCounter
        {
            get { return senddatacounter; }
            set
            {
                senddatacounter = value;
                RaisePropertyChanged("SendDataCounter");
            }
        }

        private int sendbytescounter;

        public int SendBytesCounter
        {
            get { return sendbytescounter; }
            set
            {
                sendbytescounter = value;
                RaisePropertyChanged("SendBytesCounter");
            }
        }


        private int receivedatacounter;

        public int ReceiveDataCounter
        {
            get { return receivedatacounter; }
            set
            {
                receivedatacounter = value;
                RaisePropertyChanged("ReceiveDataCounter");
            }
        }

        private int receivebytescounter;

        public int ReceiveBytesCounter
        {
            get { return receivebytescounter; }
            set
            {
                receivebytescounter = value;
                RaisePropertyChanged("ReceiveBytesCounter");
            }
        }


        private int errordatacounter;

        public int ErrorDataCounter
        {
            get { return errordatacounter; }
            set
            {
                errordatacounter = value;
                RaisePropertyChanged("ErrorDataCounter");
            }
        }

        private bool ishexdata;

        public bool IsHexData
        {
            get { return ishexdata; }
            set
            {
                ishexdata = value;
                RaisePropertyChanged("IsHexData");
            }
        }

        private bool isshowdate;

        public bool IsShowDate
        {
            get { return isshowdate; }
            set
            {
                isshowdate = value;
                RaisePropertyChanged("IsShowDate");
            }
        }

        private bool isshowsend;

        public bool IsShowSend
        {
            get { return isshowsend; }
            set
            {
                isshowsend = value;
                RaisePropertyChanged("IsShowSend");
            }
        }



        private string BytesToHexString(byte[] bytes)
        {
            string str = bytes[0].ToString("X2");
            for (int i = 1; i < bytes.Length; i++)
            {
                str = string.Format("{0} {1:X2}", str, bytes[i]);
            }
            return str;
        }

        public string SendDataUpdate(string str)
        {

            if (IsOpen == false)
            {
                throw new Exception("请打开串口！");
            }

            if (str == "")
            {
                throw new Exception("请输入十六进制数！");
            }

            try
            {
                if (IsHexData)//判断是否为十六进制的数
                {
                    str = str.Replace(" ", "");
                    if ((str.Length % 2) != 0)
                    {
                        str = str.Insert(str.Length - 1, "0");
                    }
                    byte[] txData = new byte[str.Length / 2];

                    for (int i = 0; i < txData.Length; i++)
                    {
                        txData[i] = Convert.ToByte(str.Substring(i * 2, 2), 16);
                    }

                    return SendDataUpdate(txData);
                }
                else
                {
                    WriteLine(str);

                    SendDataCounter++;
                    SendBytesCounter += str.Length;

                    if (IsShowSend) str = "[发送] " + str;

                    if (IsShowDate) str = String.Format("[{0:HH:mm:ss.fff}]{1}", DateTime.Now, str);

                    return str;

                }
            }
            catch (Exception)
            {
                throw new Exception("请输入标准hex，必须是标准Hex啊.例如：1F8A 或者: 1E 2F ");
            }


        }

        public string SendDataUpdate(byte[] data)
        {
            if (!IsOpen)
            {
                throw new Exception("端口未打开！");
            }

            Write(data, 0, data.Length);

            IsSend = true;

            SendDataCounter++;
            SendBytesCounter += data.Length;

            string str = BytesToHexString(data);

            if (IsShowSend) str = "[发送] " + str;

            if (IsShowDate) str = String.Format("[{0:HH:mm:ss.fff}]{1}", DateTime.Now, str);

            return str;
        }

        public string ReceiveDataUpdate(byte[] data)
        {
            ReceiveDataCounter++;
            ReceiveBytesCounter += data.Length;

            string str = null;

            if (IsHexData)
            {
                str = BytesToHexString(data);
            }
            else
            {
                str = Encoding.ASCII.GetString(data);
            }

            if (IsShowSend) str = "[接收] " + str;

            if (IsShowDate) str = String.Format("[{0:HH:mm:ss.fff}]{1}", DateTime.Now, str);

            return str;
        }

        public string ReceiveDataUpdate(string str)
        {
            ReceiveDataCounter++;
            ReceiveBytesCounter += str.Length;

            if (IsShowSend) str = "[接收] " + str;

            if (IsShowDate) str = String.Format("[{0:HH:mm:ss.fff}]{1}", DateTime.Now, str);

            return str;
        }

        public string ErrorTextUpdate(byte[] data)
        {
            ErrorDataCounter++;

            string str = BytesToHexString(data);

            if (IsShowSend) str = "[接收] " + str;

            if (IsShowDate) str = String.Format("[{0:HH:mm:ss.fff}]{1}", DateTime.Now, str);

            return str;
        }

        public SerialPortModel()
        {
            PortNames = SerialPort.GetPortNames();

            IsHexData = true;

            IsSend = false;

            IsShowDate = false;

            IsShowSend = false;
        }

        public SerialPortModel(bool ishex = true, bool isshowdate = false, bool isshowsend = false)
        {
            PortNames = SerialPort.GetPortNames();

            IsHexData = ishex;

            IsSend = false;

            IsShowDate = isshowdate;

            IsShowSend = isshowsend;
        }

        public void ClearCounter()
        {
            SendBytesCounter = 0;
            SendDataCounter = 0;
            ReceiveBytesCounter = 0;
            ReceiveDataCounter = 0;
        }


        public event PropertyChangedEventHandler PropertyChanged;
        public void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        /*********************************************所有发送指令相关的***************************************/
        /////////////////////////////////////发送5hz数据//////////////////////////////////////////////////////////
        public void Send5HZData(int Value)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[8];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x07;
            TxData[3] = 0x00;
            TxData[4] = (byte)(Value);
            for (int i = 2; i <= 4; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[5] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[6] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 7);
            }
            catch (Exception)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }
        /////////////////////////////////////发送50hz上层pid数据//////////////////////////////////////////////////////////
        public void Send50HZPIDData(int Value)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[8];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x07;
            TxData[3] = 0x61;
            TxData[4] = (byte)(Value);
            for (int i = 2; i <= 4; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[5] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[6] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 7);
            }
            catch (Exception)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }
        }
        /////////////////////////////////////发送50hz数据//////////////////////////////////////////////////////////
        public void Send50HZData(int Value)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[8];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x07;
            TxData[3] = 0x21;
            TxData[4] = (byte)(Value);
            for (int i = 2; i <= 4; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[5] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[6] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 7);
            }
            catch (Exception)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }
        }
        /// <summary>
        /// /设置作业参数
        /// </summary>
        /// <param name="Set_work_speed"></param>
        /// <param name="Set_work_radius"></param>
        /// <param name="Set_pensa_radius"></param>
        /// <param name="Set_work_height"></param>
        public void SendSheZhiCanShu(float Set_UAV_Speed, float Set_UAV_Height, float Set_UAV_Upspeed, float Set_UAV_Downspeed, float Set_UAV_Attituderange, float Set_UAV_Maxhangxiangjiaosudu)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[13];
            ushort SBcc = 0;
            short sTemp;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x0C;
            TxData[3] = 0x0E;
            sTemp = (short)((Set_UAV_Speed + 0.05) * 10.0);
            TxData[4] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_UAV_Upspeed + 0.05) * 10.0);
            TxData[5] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_UAV_Downspeed + 0.05) * 10.0);
            TxData[6] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_UAV_Height + 0.05) * 10.0);
            TxData[7] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_UAV_Attituderange));
            TxData[8] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_UAV_Maxhangxiangjiaosudu));
            TxData[9] = (byte)(sTemp & 0x00FF);
            for (int i = 2; i <= 9; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[10] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[11] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 12);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }



        /// <summary>
        /// /植保参数
        /// </summary>
        /// <param name="Set_work_speed"></param>
        /// <param name="Set_work_radius"></param>
        /// <param name="Set_pensa_radius"></param>
        /// <param name="Set_work_height"></param>
        public void SendZhiBaoCanShu(float Set_UAV_ZhiBaoSpeed, float Set_UAV_ZhiBaoFuDu, float Set_UAV_ZhiBaoAction)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[10];
            ushort SBcc = 0;
            short sTemp;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x09;
            TxData[3] = 0x08;
            sTemp = (short)((Set_UAV_ZhiBaoSpeed + 0.005) * 10.0);
            TxData[4] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_UAV_ZhiBaoFuDu + 0.005) * 10.0);
            TxData[5] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_ZhiBaoAction);
            TxData[6] = (byte)(sTemp & 0x00FF);
            for (int i = 2; i <= 6; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[7] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[8] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 9);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }

        /// <summary>
        /// /航拍参数
        /// </summary>
        /// <param name="Set_work_speed"></param>
        /// <param name="Set_work_radius"></param>
        /// <param name="Set_pensa_radius"></param>
        /// <param name="Set_work_height"></param>
        public void SendHangPaiCanShu(float Set_UAV_HangPaiSpeed, float Set_UAV_M9Low, float Set_UAV_M9Middle, float Set_UAV_M9High, float Set_UAV_M10Low, float Set_UAV_M10Middle,float Set_UAV_M10High,float upspeed,float downspeed)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[16];
            ushort SBcc = 0;
            short sTemp;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x0F;
            TxData[3] = 0x09;
            sTemp = (short)((Set_UAV_HangPaiSpeed + 0.05) * 10.0);
            TxData[4] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((upspeed + 0.05) * 10.0);
            TxData[5] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((downspeed + 0.05) * 10.0);
            TxData[6] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_M9Low * 100);
            TxData[7] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_M9Middle * 100);
            TxData[8] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_M9High * 100);
            TxData[9] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_M10Low * 100);
            TxData[10] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_M10Middle * 100);
            TxData[11] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_M10High * 10);
            TxData[12] = (byte)(sTemp & 0x00FF);
            for (int i = 2; i <= 12; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[13] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[14] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 15);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }

        /// <summary>
        /// /设置植保航拍切换
        /// </summary>
        /// <param name="Set_work_speed"></param>
        /// <param name="Set_work_radius"></param>
        /// <param name="Set_pensa_radius"></param>
        /// <param name="Set_work_height"></param>
        public void SendZhibaoHangpaiQiehuan(int Set_UAV_CurrentStyle)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[8];
            ushort SBcc = 0;
            short sTemp;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x07;
            TxData[3] = 0x12;
            sTemp = (short)(Set_UAV_CurrentStyle);
            //    TxData[4] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[4] = (byte)(sTemp & 0x00FF);
            for (int i = 2; i <= 4; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[5] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[6] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 7);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }


        /// <summary>
        /// /策略设置
        /// </summary>
        /// <param name="Set_work_speed"></param>
        /// <param name="Set_work_radius"></param>
        /// <param name="Set_pensa_radius"></param>
        /// <param name="Set_work_height"></param>
        public void SendAnQuanCeLue(float Set_UAV_Noticefirstvolt, float Set_UAV_Noticesecondvolt, float Set_UAV_Noticesecondaction, float Set_UAV_Noticelosecontrolaction, float Set_UAV_Noticedistance, float Set_UAV_Noticeheight, float Set_UAV_Noticefanhanggaodu)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[18];
            ushort SBcc = 0;
            short sTemp;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x11;
            TxData[3] = 0x0D;
            sTemp = (short)((Set_UAV_Noticefirstvolt + 0.005) * 100.0);
            TxData[4] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[5] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_UAV_Noticesecondvolt + 0.005) * 100.0);
            TxData[6] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[7] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_Noticesecondaction);
            TxData[8] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_Noticelosecontrolaction);
            TxData[9] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_UAV_Noticedistance));
            TxData[10] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[11] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_UAV_Noticeheight));
            TxData[12] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[13] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_UAV_Noticefanhanggaodu + 0.05) * 10.0);
            TxData[14] = (byte)(sTemp & 0x00FF);
            for (int i = 2; i <= 14; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[15] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[16] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 17);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }



        /// <summary>
        /// /植保作业
        /// </summary>
        /// <param name="Set_work_speed"></param>
        /// <param name="Set_work_radius"></param>
        /// <param name="Set_pensa_radius"></param>
        /// <param name="Set_work_height"></param>
        public void SendZhiBaoZuoYeCanShu(float Set_UAV_ZhiBaoSpeed, float Set_UAV_ZhiBaoFuDu)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[11];
            ushort SBcc = 0;
            short sTemp;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x0A;
            TxData[3] = 0x08;
            sTemp = (short)((Set_UAV_ZhiBaoSpeed + 0.005) * 100.0);
            TxData[4] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[5] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_UAV_ZhiBaoFuDu + 0.005) * 100.0);
            TxData[6] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[7] = (byte)(sTemp & 0x00FF);
            for (int i = 2; i <= 7; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[8] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[9] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 10);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }


        /// <summary>
        /// /航拍作业
        /// </summary>
        /// <param name="Set_work_speed"></param>
        /// <param name="Set_work_radius"></param>
        /// <param name="Set_pensa_radius"></param>
        /// <param name="Set_work_height"></param>
        public void SendHangPaiZuoYeCanShu(float Set_UAV_HangPaiSpeed, float Set_UAV_M9Low, float Set_UAV_M9Middle, float Set_UAV_M9High, float Set_UAV_M10Low, float Set_UAV_M10Middle, float Set_UAV_M10High)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[21];
            ushort SBcc = 0;
            short sTemp;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x14;
            TxData[3] = 0x08;
            sTemp = (short)((Set_UAV_HangPaiSpeed + 0.005) * 100.0);
            TxData[4] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[5] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_M9Low);
            TxData[6] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[7] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_M9Middle);
            TxData[8] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[9] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_M9High);
            TxData[10] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[11] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_M10Low);
            TxData[12] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[13] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_M10Middle);
            TxData[14] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[15] = (byte)(sTemp & 0x00FF);
            sTemp = (short)(Set_UAV_M10High);
            TxData[16] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[17] = (byte)(sTemp & 0x00FF);
            for (int i = 2; i <= 17; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[18] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[19] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 20);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }



        ///////////////////////////////////////////////////////////机型设置/////////////////////////////////////////////////
        public void SendQudeSet(int DataSelect_QUADE, int tape_speedvalue)
        {

            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[11];
            ushort SBcc = 0;
            short sTemp;
            sTemp = (short)DataSelect_QUADE;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x0A;
            TxData[3] = 0x0C;
            TxData[4] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[5] = (byte)(sTemp & 0x00FF);
            TxData[6] = (byte)(tape_speedvalue & 0x000000FF);
            TxData[7] = 0x00;
            for (int i = 2; i <= 7; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[8] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[9] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 10);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }
        //////////////////////////////////////////遥控器校准开始指令///////////////////////////////////////////////////
        public void SendAutoykqjz_Start(int Tel_Steps, int Tel_StepsValue)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[9];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x08;
            TxData[3] = 0x4A;
            TxData[4] = Convert.ToByte((Tel_Steps & 0x000000FF));
            TxData[5] = Convert.ToByte((Tel_StepsValue & 0x000000FF));
            for (int i = 2; i <= 5; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[6] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[7] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 8);
            }
            catch (Exception)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }
        //////////////////////////////////////////遥控器方向确认指令///////////////////////////////////////////////////
        public void SendYaoKongQiJZ_Sure(int ykqfxqr_Flag)
        {
            if (IsOpen == false)
            {
                return;
            }
            byte[] TxData = new byte[8];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x07;
            TxData[3] = 0x51;
            TxData[4] = Convert.ToByte((ykqfxqr_Flag & 0x000000FF));
            for (int i = 2; i <= 4; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[5] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[6] = (byte)(SBcc & 0x00FF);
            Write(TxData, 0, 7);
        }
        //////////////////////////////////////////遥控器校准结束指令///////////////////////////////////////////////////
        public void SendAutoykqjz_End(int ykqjzend_Flag)
        {
            if (IsOpen == false)
            {
                return;
            }
            byte[] TxData = new byte[8];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x07;
            TxData[3] = 0x4B;
            TxData[4] = Convert.ToByte((ykqjzend_Flag & 0x000000FF));
            for (int i = 2; i <= 4; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[5] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[6] = (byte)(SBcc & 0x00FF);
            Write(TxData, 0, 7);
        }
        /// <summary>
        /// ///////上层pid设置
        /// </summary>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        public void SendSetShangcengPID(float Set_zitai_p, float Set_zitai_i, float Set_zitai_d, float Set_hangxiang_p, float Set_hangxiang_i, float Set_hangxiang_d, float Set_sudu_p, float Set_sudu_i, float Set_sudu_d, float Set_gaodu_p, float Set_gaodu_i, float Set_gaodu_d)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[31];
            ushort SBcc = 0;
            short sTemp;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x1E;
            TxData[3] = 0x75;
            sTemp = (short)((Set_zitai_p));
            TxData[4] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[5] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_zitai_i));
            TxData[6] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[7] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_zitai_d));
            TxData[8] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[9] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_hangxiang_p));
            TxData[10] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[11] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_hangxiang_i));
            TxData[12] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[13] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_hangxiang_d));
            TxData[14] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[15] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_gaodu_p));
            TxData[16] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[17] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_gaodu_i));
            TxData[18] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[19] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_gaodu_d));
            TxData[20] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[21] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_sudu_p));
            TxData[22] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[23] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_sudu_i));
            TxData[24] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[25] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((Set_sudu_d));//Set_gaodu_i
            TxData[26] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[27] = (byte)(sTemp & 0x00FF);
            for (int i = 2; i <= 27; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[28] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[29] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 30);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }
        /////////////////////////////////////////////////////////////PID设置////////////////////////////////////////////
        public void SendSetPID(float gyro_kp_rp, float gyro_ki_rp, float gyro_kd_rp, float gyro_kp_y, float gyro_ki_y, float gyro_kd_y, float prop_kp_rp, float prop_ki_rp, float prop_kp_y, float prop_ki_y)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[27];
            ushort SBcc = 0;
            short sTemp;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x1A;
            TxData[3] = 0x2F;
            sTemp = (short)((gyro_kp_rp + 0.005) * 100.0);
            TxData[4] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[5] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((gyro_ki_rp + 0.005) * 100.0);
            TxData[6] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[7] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((gyro_kd_rp + 0.005) * 100.0);
            TxData[8] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[9] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((gyro_kp_y + 0.005) * 100.0);
            TxData[10] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[11] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((gyro_ki_y + 0.005) * 100.0);
            TxData[12] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[13] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((gyro_kd_y + 0.005) * 100.0);
            TxData[14] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[15] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((prop_kp_rp + 0.005) * 100.0);
            TxData[16] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[17] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((prop_ki_rp + 0.005) * 100.0);
            TxData[18] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[19] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((prop_kp_y + 0.005) * 100.0);
            TxData[20] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[21] = (byte)(sTemp & 0x00FF);
            sTemp = (short)((prop_ki_y + 0.005) * 100.0);
            TxData[22] = (byte)((sTemp & 0xFF00) >> 8);
            TxData[23] = (byte)(sTemp & 0x00FF);
            for (int i = 2; i <= 23; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[24] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[25] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 26);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }
        //自主起飞
        public void SendAutoTakeOff(int TakeOff_Flag)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[8];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x07;
            TxData[3] = 0x02;
            TxData[4] = Convert.ToByte(TakeOff_Flag);
            for (int i = 2; i <= 4; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[5] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[6] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 7);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }
        /// <summary>
        //////////////////// 电机发送测试函数///////////////////////////////////////
        /// </summary>
        /// <param name="FlightLanding_Flag"></param>
        public void SendMotorTest(int Motor1_Test_Index, int Motor2_Test_Index)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[9];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x08;
            TxData[3] = 0x13;
            TxData[4] = (byte)(Motor1_Test_Index);
            TxData[5] = (byte)(Motor2_Test_Index);
            for (int i = 2; i <= 5; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[6] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[7] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 8);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }

        /// <summary>
        /// 水泵发送函数
        /// </summary>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        /// <param name="?"></param>
        public void SendPumpTest(int Intelligent_Pump_MaxValue_Set, int Intelligent_Pump_MinValue_Set,
                    int Intelligent_Vel_MaxValue_Set, int Intelligent_Vel_MinValue_Set, int Intelligent_Motor_Value_Set, int Fixed_Pump_Value_Set, int Fixed_Motor_Value_Set, int Set_shuiben_duanyao_action)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[15];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x0E;
            TxData[3] = 0x18;
            TxData[4] = (byte)(Fixed_Motor_Value_Set);//
            TxData[5] = (byte)(Fixed_Pump_Value_Set);//
            TxData[6] = (byte)(Intelligent_Motor_Value_Set);//
            TxData[7] = (byte)(Intelligent_Vel_MinValue_Set);//
            TxData[8] = (byte)(Intelligent_Pump_MinValue_Set);//
            TxData[9] = (byte)(Intelligent_Vel_MaxValue_Set);//
            TxData[10] = (byte)(Intelligent_Pump_MaxValue_Set);//
            TxData[11] = (byte)(Set_shuiben_duanyao_action);//
            for (int i = 2; i <= 11; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[12] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[13] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 14);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }

        ////////////////////////修改拐弯参数//////////////////////////////////////
        public void SendFlightTurn_Set(float Offset_Angle_Set, float ApproachLine_Range_Set, float ApproachLine_DeadZone_Set, float ApproachPoint_Range_Set
            , float HorizoMove_Acc_Set, float HorizoMove_Range_Set, float FowardMove_Acc_Set, float VelToPosition_Percent_Set, float BToThreshold_Percent_Set, float AToThreshold_Percen_Set, float AB_BackThreshold_Set)
        {

            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[19];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x12;
            TxData[3] = 0x11;
            TxData[4] = (byte)Offset_Angle_Set;
            TxData[5] = (byte)ApproachLine_Range_Set;
            TxData[6] = (byte)ApproachLine_DeadZone_Set;
            TxData[7] = (byte)ApproachPoint_Range_Set;
            TxData[8] = (byte)HorizoMove_Acc_Set;
            TxData[9] = (byte)HorizoMove_Range_Set;
            TxData[10] = (byte)FowardMove_Acc_Set;
            TxData[11] = (byte)VelToPosition_Percent_Set;
            TxData[12] = (byte)BToThreshold_Percent_Set;
            TxData[13] = (byte)AToThreshold_Percen_Set;
            TxData[14] = (byte)AB_BackThreshold_Set;
            TxData[15] = 0;
            for (int i = 2; i <= 15; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[16] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[17] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 18);
            }
            catch (Exception)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }



        //////////////////////////ap编号设置////////////////////////////////////////////
        public void SendAPID_Set(string AP_ID)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }


            int string_length = AP_ID.Length;
            byte[] String_TxData = new byte[14];
            for (int i = 0; i < 14; i++)
            {
                String_TxData[i] = 0;
            }
            StringReader apid_stringreader = new StringReader(AP_ID);
            for (int j = 0; j < string_length; j++)
            {
                String_TxData[j] = (byte)apid_stringreader.Read();
            }
            byte[] TxData = new byte[20];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x14;
            TxData[3] = 0x0B;
            TxData[4] = String_TxData[0];
            TxData[5] = String_TxData[1];
            TxData[6] = String_TxData[2];
            TxData[7] = String_TxData[3];
            TxData[8] = String_TxData[4];
            TxData[9] = String_TxData[5];
            TxData[10] = String_TxData[6];
            TxData[11] = String_TxData[7];
            TxData[12] = String_TxData[8];
            TxData[13] = String_TxData[9];
            TxData[14] = String_TxData[10];
            TxData[15] = String_TxData[11];
            TxData[16] = String_TxData[12];
            TxData[17] = String_TxData[13];
            for (int i = 2; i <= 17; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[18] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[19] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 20);
            }
            catch (Exception)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }
        /// <summary>
        /// 自主着陆
        /// </summary>
        /// <param name="FlightLanding_Flag"></param>
        public void SendAutoLanding(int FlightLanding_Flag)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[8];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x07;
            TxData[3] = 0x03;
            TxData[4] = (byte)(FlightLanding_Flag);
            for (int i = 2; i <= 4; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[5] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[6] = (byte)(SBcc & 0x00FF);

            try
            {
                Write(TxData, 0, 7);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }
        /// <summary>
        /// /发送路点的相关指令
        /// </summary>
        /// <param name="Plan_Flag"></param>
        /// <param name="TakePhoto_Flag"></param>
        /// <param name="Lat_Value"></param>
        /// <param name="Lon_Value"></param>
        /// <param name="Height_Value"></param>
        /// <param name="Yaw_Value"></param>
        /// <param name="Road_Num"></param>
        /// <param name="Which_Road"></param>
        /// <param name="Vel_Flag"></param>
        /// <param name="Vel_Value"></param>
        /// <param name="WhichData_Bag"></param>
        /// <param name="SetCameraAngle_Flag"></param>
        public void SendRefData(int UsedWayPoint_SetIndex, int UsedWayPoint_KeepIndex1, double Lat_Value
            , double Lon_Value, double Height_Value, double UsedWayPoint_HoverTimer, int Road_Num, int Which_Road
            , double UsedWayPoint_Yaw, double UsedWayPoint_HengxiangSudu, int WhichData_Bag, double UsedWayPoint_ZongshangSudu, double UsedWayPoint_ZongxiaSudu
            , double UsedWayPoint_KeepIndex2)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            short STemp;
            ushort SBcc = 0;
            byte[] TxData = new byte[37];
            byte[] LatData = new byte[8];
            byte[] LonData = new byte[8];
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x24;
            TxData[3] = 0x06;
            /////////////////////////路点设置////////////
            TxData[4] = (byte)UsedWayPoint_SetIndex;
            /////////////////////////保留位//////////////
            TxData[5] = (byte)UsedWayPoint_KeepIndex1;
            /////////////////////////任务组////////////
            TxData[6] = (byte)WhichData_Bag;
            ////////////////////////航点总数///////////
            TxData[7] = (byte)Road_Num;
            ////////////////////////当前航点/////////////////
            TxData[8] = (byte)Which_Road;
            ////////////////////////纬度////////////////////
            LatData = BitConverter.GetBytes(Lat_Value);
            LatData.CopyTo(TxData, 9);
            //////////////////////经度//////////////////////
            LonData = BitConverter.GetBytes(Lon_Value);
            LonData.CopyTo(TxData, 17);
            /*****************************高度****************/
            STemp = (short)(Height_Value);
            TxData[25] = (byte)((STemp & 0xFF00) >> 8);
            TxData[26] = (byte)(STemp & 0x00FF);
            /*****************************偏航****************/
            if (UsedWayPoint_Yaw >= 0 && UsedWayPoint_Yaw<=180)
            {
                UsedWayPoint_Yaw = -UsedWayPoint_Yaw;
            }
            if(UsedWayPoint_Yaw>=180 && UsedWayPoint_Yaw<=360)
            {
                UsedWayPoint_Yaw = (360 - UsedWayPoint_Yaw);
            }
            double used_deg = (UsedWayPoint_Yaw / 180.0 * 3.1415926);
            STemp = (short)((used_deg) * 100.0);
            TxData[27] = (byte)((STemp & 0xFF00) >> 8);
            TxData[28] = (byte)(STemp & 0x00FF);
            /***************************横向速度***********/
            STemp = (short)(UsedWayPoint_HengxiangSudu * 10.0);
            TxData[29] = (byte)(STemp & 0x00FF);
            /*************************纵上速度********************/
            STemp = (short)(UsedWayPoint_ZongshangSudu * 10.0);
            TxData[30] = (byte)(STemp & 0x00FF);
            /*************************纵下速度********************/
            STemp = (short)(UsedWayPoint_ZongxiaSudu * 10.0);
            TxData[31] = (byte)(STemp & 0x00FF);
            /*************************悬停时间********************/
            STemp = (short)(UsedWayPoint_HoverTimer);
            TxData[32] = (byte)(STemp & 0x00FF);
            /***********************保留位******************/
            TxData[33] = (byte)UsedWayPoint_KeepIndex2;
            for (int i = 2; i <= 33; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[34] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[35] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 36);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }
        /// <summary>
        /// //一键返航
        /// </summary>
        public void SendOneKeyToReturn_Line(int FlightReturn_Flag)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[8];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x07;
            TxData[3] = 0x04;
            TxData[4] = (byte)(FlightReturn_Flag);
            for (int i = 2; i <= 4; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[5] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[6] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 7);
            }
            catch (Exception)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }
        public void SendOneKeyToReturn_Path(int FlightReturn_Flag)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[8];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x07;
            TxData[3] = 0x05;
            TxData[4] = (byte)(FlightReturn_Flag);
            for (int i = 2; i <= 4; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[5] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[6] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 7);
            }
            catch (Exception)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }
        }
        /// <summary>
        /// /发送修改某一个路点的指令
        /// </summary>
        /// <param name="ChoooseNum"></param>
        /// <param name="Lat_Value"></param>
        /// <param name="Lon_Value"></param>
        public void SendModifyRefData(int ChoooseID, double Lat_Value, double Lon_Value)
        {
            if (IsOpen == false)
            {
                return;
            }
            ushort SBcc = 0;
            byte[] TxData = new byte[24];
            byte[] LatData = new byte[8];
            byte[] LonData = new byte[8];
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x17;
            TxData[3] = 0x12;
            TxData[4] = (byte)ChoooseID;
            LatData = BitConverter.GetBytes(Lat_Value);
            LatData.CopyTo(TxData, 5);
            LonData = BitConverter.GetBytes(Lon_Value);
            LonData.CopyTo(TxData, 13);
            for (int i = 2; i <= 20; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[21] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[22] = (byte)(SBcc & 0x00FF);
            Write(TxData, 0, 23);
        }

        public void SendLedAdjust(int ledstate)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[8];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x07;
            TxData[3] = 0x07;
            TxData[4] = (byte)(ledstate);
            for (int i = 2; i <= 4; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[5] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[6] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 7);
            }
            catch (Exception)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }
        /*******************************************发送固件升级指令******************************************/
        public void SendGuJianShengJi(int DataSelect_GuJIAN)
        {
            if (IsOpen == false)
            {
                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
                return;
            }
            byte[] TxData = new byte[10];
            ushort SBcc = 0;
            TxData[0] = 0x3A;
            TxData[1] = 0xA3;
            TxData[2] = 0x09;
            TxData[3] = 0x70;
            TxData[4] = (byte)(DataSelect_GuJIAN & 0x000000FF);
            TxData[5] = 0x00;
            TxData[6] = 0x00;
            for (int i = 2; i <= 6; i++)
            {
                SBcc += (ushort)((ushort)TxData[i] & 0x00FF);
            }
            TxData[7] = (byte)((SBcc >> 8) & 0x00FF);
            TxData[8] = (byte)(SBcc & 0x00FF);
            try
            {
                Write(TxData, 0, 9);
            }
            catch (Exception)
            {

                MainWindow.pWin.open_com_flag = false;
                MainWindow.pWin.btnOpenPort.IsChecked = false;
            }

        }


        /*********************************************所有发送指令相关的***************************************/

    }
}
