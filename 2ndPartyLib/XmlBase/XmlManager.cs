using System;
using System.Xml;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Xml.XPath;

using GMap.NET;                         //GMap.NET Core

using MulitWaypoint;


namespace XmlBase
{
	/*　
	* 　XMLファイルの読み込み 参考：http://fernweh.jp/b/csharp-xelement/
	* 
	* （1）ソリューション「XML/Message」にXMLファイルを追加すること
	* （2）追加したXMLファイルはプロパティより、ビルドアクション「埋め込まれたリソース」出力ディレクトリにコピー「常にコピーする」に設定すること
	* （3）Xml/Message内のXMLファイル名のまま追記すること
	* 
	*/
	public enum XmlMessage
    {
        Close,							//关闭消息
        Get_UAVData,					//获取消息无人机数据采集表
        Get_Initial,					//采集的COM端口号和获取信息的核心部分的目录
        Get_COMPort,					//获取消息核心部分的COM端口号
        MainlinkSend_Firm,				//MainlinkSend消息　固件信息获取
        MainlinkSend_Home,				//MainlinkSend消息　Home数据发送到UAV
        MainlinkSend_Land,				//MainlinkSend消息　Land数据发送到UAV
        MainlinkSend_Pause,				//MainlinkSend消息　Pause数据发送到UAV
        MainlinkSend_Resume,			//MainlinkSend消息　Land数据发送到UAV
        MainlinkSend_Waypoint,			//MainlinkSend消息　Resume数据发送到UAV
        MainlinkSend_MultiWaypoint,		//MainlinkSend消息　MultiWaypoint数据发送到UAV
        MainlinkSend_MultiWaypointAck,	//MainlinkSend消息　MultiWaypoint数据发送到UAV
        Set_COMPort,					//Set消息　设置核心单位的COM端口号
        Set_FlightLogEdit,				//Set消息　保存飞行日志
        Set_FlightLogOpenClose,			//Set消息　要开启/关闭飞行日志
        Set_FlightLogPlay,				//Set消息　播放/停止/暂停/模式进/模式的回归飞行日志
        Set_FlightLogSlidebar			//Set消息　飞行日志滑动条
    }

    public class XmlManager
    {
        public static readonly XmlManager Empty = new XmlManager();

        private List<string> Data;				    // XMLdata
        private string dataUAV;                     //
        private int get_Size;				        // 消息的大小
        private int mode;					        // 命令按钮的状态（1：无人机操作模式2：登录播放模式）
        private int _reply;					        // 状态（0：正常1：COM设置2：打开飞行日志3：关闭飞行日志 4：保存）
        private List<string> MessageList;	        // 核心部分的消息模板列表
        private List<string> FlightModeList;        // 
        private List<ModeManager> ControlModeList;  // 
        private XmlData _BindingData;		        // 

        // 初期化
        public XmlManager()
        {
            Data = new List<string>();
            dataUAV = string.Empty;
            get_Size = 0;
            mode = 1;
            _reply = 0;
            _BindingData = new XmlData();
        }

        #region 属性
        public string xmlUAVData
		{
            get { return this.dataUAV; }
            set { this.dataUAV = value; }
        }
        public List<string> strXmlData
        {
            get { return this.Data; }
            set { this.Data = value; }
        }
        public int XMLgetsize
        {
            get { return this.get_Size; }
            set { this.get_Size = value; }
        }
        public int CommandMode
        {
            get { return this.mode; }
            set { this.mode = value; }
        }
        public int ReplyMode
        {
            get { return this._reply; }
            set { this._reply = value; }
        }
        public XmlData BindingData
        {
            get { return this._BindingData; }
            set { this._BindingData = value; }
        }
        #endregion

        #region XML管理

        // 初始化（读取XML文件的消息）
        public void Initialize()
        {
            MessageList = new List<string>();

            // 列出了预定义的枚举
            foreach (XmlMessage list in Enum.GetValues(typeof(XmlMessage)))//加载每个变量的信息
            {
                MessageList.Add(ReadXMLFile(list));//加载XmlMessage信息到MessageList中
            }

            InitModeName();	// 读控制/飞行模式名称列表

            Data = new List<string>();
            get_Size = 0;
            mode = 1;
            _reply = 0;
            _BindingData = new XmlData();

        }

        // 所选文件的读取
        private string ReadXMLFile(XmlMessage file)
        {
            string path = "Xml/Message/" + file.ToString() + ".xml";

            System.IO.StreamReader read = new System.IO.StreamReader(System.IO.Path.GetFullPath(path), System.Text.Encoding.GetEncoding("UTF-8"));
            string data = read.ReadToEnd();
            data += "\0";

            return data;
        }

        // 请选择消息
        public string ReadMassage(XmlMessage file)
        {
            return MessageList[(int)file];
        }

        // 设置/MainlinkSend 消息生成 通信上之后

        public string SetMassage(XmlMessage file, List<string> tag, List<string> value, bool base64 = true)
        {
            string text = MessageList[(int)file];
            text = text.Replace("\0", "");
            XDocument xml = XDocument.Parse(text);                                   // 文本数据加载到 xml 数据
            int i = 0;
            foreach (var list in tag)//tag为鼠标点击下去返回的标志位
            {
                if (xml.Root.Element("message").XPathSelectElement(list) != null)//根据路径选择元素
                {
                    string data;
                    var attr = xml.Root.Element("message").XPathSelectElement(list).Attribute("t");//向元素添加属性
                    if (base64 == false)
                    {
                        data = value[i];
                    }
                    else
                    {
                        if (attr != null)
                        {
                            if (attr.Value == "S")
                            {
                                data = value[i];
                            }
                            else if (attr.Value == "B")
                            {
                                data = value[i];	//T:真　F：偽
                            }
                            else
                            {
                                // Base64変換 int型:4byte 
                                byte[] byteArray = BitConverter.GetBytes(int.Parse(value[i]));
                                data = System.Convert.ToBase64String(byteArray, 0, byteArray.Length);
                                //data = IsDecimal(value[i]);
                            }
                        }
                        else
                        {
                            // Base64変換 double型:8byte 
                            byte[] byteArray = BitConverter.GetBytes(double.Parse(value[i]));
                            data = System.Convert.ToBase64String(byteArray, 0, byteArray.Length);
                            //data = IsDecimal(value[i]);
                        }
                    }
                    xml.Root.Element("message").XPathSelectElement(list).Value = data;
                    //System.Diagnostics.Debug.WriteLine("XmlData: " + list + "/" + data);
                }
                i++;
            }
            text = xml.ToString();
            return text + "\0";
        }

        // Base64変換(int型:4byte　double型:8byte)
        private string IsDecimal(string value)
        {
            string result = string.Empty;

            double d;
            if (double.TryParse(value, out d))
            {
                int i;
                if (int.TryParse(value, out i))
                {
                    // Base64変換 int型:4byte 
                    byte[] byteArray = BitConverter.GetBytes(int.Parse(value));
                    result = System.Convert.ToBase64String(byteArray, 0, byteArray.Length);
                }
                else
                {
                    // Base64変換 double型:8byte 
                    byte[] byteArray = BitConverter.GetBytes(double.Parse(value));
                    result = System.Convert.ToBase64String(byteArray, 0, byteArray.Length);
                }

            }
            else
            {
                result = string.Empty;	//存储空的内容元素
            }

            return result;
        }

        // 阅读的消息中获得的数据
        public string GetMassage(string tag, bool delFlag = true)
        {
            string value = string.Empty;

            for (int i = 0; i < Data.Count; i++)
            {
                if (!string.IsNullOrEmpty(Data[i]))
                {
                    // 阅读文本数据的XML数据
                    XDocument xml = XDocument.Parse(Data[i]);
                    //System.Diagnostics.Debug.WriteLine("Read: " + Data + "\n\n");
                    if (xml.Root.Element("message").XPathSelectElement(tag) != null)
                    {
                        var attr = xml.Root.Element("message").XPathSelectElement(tag).Attribute("t");
                        if (attr == null)
                        {
                            // Base64 double型
                            byte[] byteArray = Convert.FromBase64String(xml.Root.Element("message").XPathSelectElement(tag).Value);
                            value = BitConverter.ToDouble(byteArray, 0).ToString();
                            if (delFlag)
                            {
                                Data.RemoveAt(i);
                            }
                            return value;
                        }
                        else
                        {
                            // 文字型
                            value = xml.Root.Element("message").XPathSelectElement(tag).Value;
                            if (delFlag || value == "NG")
                            {
                                Data.RemoveAt(i);
                            }
                            return value;
                        }

                        //System.Diagnostics.Debug.WriteLine("GetMassage: " + value);
                    }
                }
            }

            return "";
        }

        public string CloseMassage(XmlMessage file)
        {
            string text = MessageList[(int)file];
            text = text.Replace("\0", "");
            string close = MessageList[(int)XmlMessage.Close];
            close = close.Replace("\0", "");

            XDocument xmltext = XDocument.Parse(text);
            XDocument xmlclose = XDocument.Parse(close);

            if (xmltext.Root.Element("message") != null && xmlclose.Root.Element("message") != null)
            {
                xmlclose.Root.Element("message").Attribute("name").Value = xmltext.Root.Element("message").Attribute("name").Value;
                close = xmlclose.ToString();
            }
            else
            {
                close = string.Empty; ;
            }

            return close + "\0";
        }

        // 确认过程可接收的数据的XML格式
        public bool XmlDataCheck()
        {
            if (!string.IsNullOrEmpty(dataUAV))
            {
                string xmltext = dataUAV;

                // 通常检查接收到的数据
                int Index = xmltext.IndexOf("xml version");//定位xml version出现的位置
                if (Index != 2)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }

            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        //如果读消息中获取的UAV数据采集列表有变化，添加在这里。意思是在这里添加每个数据到相应的变量	
		public int XmlConvert(int _getMode = 0)
		{
			try
			{
				if (dataUAV == "")
				{
					return (-1);
				}

				int cnt = 0;
				//int row = dataUAV.ToList().Where(c => c.Equals('\n')).Count();
				////System.Diagnostics.Debug.WriteLine("XmlConvert-------------------Start " + dataUAV.Length + "[" + get_Size + ">" + row + "]");
				//if ((row == get_Size))
				//{
                XDocument xml = XDocument.Parse(dataUAV);	//阅读文本数据的XML数据
				//dataUAV = string.Empty;						// クリア
                var xmlRoot = xml.Root.Element("message");	//从XML 获得数据

				// IMU
				_BindingData.UAVpoint.Roll = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/IMU/Att", "i", "0"));
				_BindingData.UAVpoint.Pitch = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/IMU/Att", "i", "1"));
				_BindingData.UAVpoint.Yaw = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/IMU/Att", "i", "2"));
				// Ctrl

				// Nav
				_BindingData.UAVpoint.Lon = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Nav/Lon"));
				_BindingData.UAVpoint.Lat = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Nav/Lat"));
				_BindingData.UAVpoint.Alt = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Nav/Alt"));
				double _x = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Nav/Vel", "i", "0"));
				double _y = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Nav/Vel", "i", "1"));
				double _z = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Nav/Vel", "i", "2"));
				double _xx = _x * _x;
				double _yy = _y * _y;
				double _zz = _z * _z;

				double _spd = Math.Sqrt(_xx + _yy + _zz);
				_BindingData.UAVpoint.Speed = _spd;
				// Ref
				// Wpt
				_BindingData.NWaypoint.Lon = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Wpt/Lon"));
				_BindingData.NWaypoint.Lat = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Wpt/Lat"));
				_BindingData.NWaypoint.Alt = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Wpt/Alt"));
				_BindingData.NWaypoint.Yaw = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Wpt/Yaw"));
				_BindingData.NWaypoint.Speed = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Wpt/Spd"));
				_BindingData.NWaypoint.No = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Wpt/No"));
				// Home
				_BindingData.Homepoint.Lon = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Home/Lon"));
				_BindingData.Homepoint.Lat = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Home/Lat"));
				_BindingData.Homepoint.Alt = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Home/Alt"));
				_BindingData.Homepoint.Yaw = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Home/Yaw"));
				// Sys
				_BindingData.nControlMode = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Sys/Mode/Ctrl"));
				ModeManager _controlMode = new ModeManager();
				_controlMode = GetControlModeStr(_BindingData.nControlMode);
				_BindingData.nControlModeName = _controlMode.ModeName;
				_BindingData.nReset = _controlMode.Reset;
				_BindingData.nFlightMode = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Sys/Mode/Auto"));
				_BindingData.nFlightModeName = GetFlightModeStr(_BindingData.nFlightMode);
				_BindingData.nDriveMode = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Sys/Mode/Drv"));

				_BindingData.Health.LowLvMcuProcTime = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Sys/CPU", "a", "L"));
				_BindingData.Health.HighLvMcuProcTime = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Sys/CPU", "a", "H"));
				// Voltage
				_BindingData.BatteryVoltage = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Voltage"));

				// Health
				////  LowerLevelMCU
				_BindingData.Health.LLMCU_RcReceiverCommunication = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/ReceiverComm"));
				_BindingData.Health.LLMCU_RcReceiverReception = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/ReceiverRecep"));
				_BindingData.Health.LLMCU_GyroSensorCommunication = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/GyroComm"));
				_BindingData.Health.LLMCU_GyroSensorMeasurement = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/GyroMeas"));

				////  LowerLevelMCU - HigherLevelMCU Communication
				_BindingData.Health.McuComm_LowToHigh = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/McuComm", "a", "H"));
				_BindingData.Health.McuComm_HighToLow = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/McuComm", "a", "L"));

				////  HigherLevelMCU
				_BindingData.Health.HLMCU_IMUCommunication = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/ImuComm"));
				_BindingData.Health.HLMCU_IMUGyroMeasurement = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/ImuGyroMeas"));
				_BindingData.Health.HLMCU_IMUAccMeasurement = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/ImuAccMeas"));
				_BindingData.Health.HLMCU_IMUMagMeasurement = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/ImuMagMeas"));
				_BindingData.Health.HLMCU_PresSensorCommunication = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/PsComm"));
				_BindingData.Health.HLMCU_PresSensorMeasurement = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/PsMeas"));
				_BindingData.Health.HLMCU_GpsReceiverCommunication = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/GpsComm"));
				_BindingData.Health.HLMCU_GpsMeasurement = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/GpsMeas"));

				////  UAV - Ground Control Station Cummunication
				_BindingData.Health.MainLink_UavToGcs = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/Datalink", "a", "D"));
				_BindingData.Health.MainLink_GcsToUav = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Health/Datalink", "a", "U"));

				// RSSI

				// Propo

				// Time
				// GPS
				_BindingData.nSat = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/GPS/Sats"));

				switch (_getMode)
				{
					case 0:
						break;
					case 1:
						_BindingData.SubPoint.Lat = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/GPS/Lat"));
						_BindingData.SubPoint.Lon = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/GPS/Lon"));
						_BindingData.SubPoint.Alt = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/GPS/Alt"));
						_BindingData.SubPoint.Yaw = _BindingData.UAVpoint.Yaw;
						_BindingData.SubPoint.Pitch = _BindingData.UAVpoint.Pitch;
						_BindingData.SubPoint.Roll = _BindingData.UAVpoint.Roll;
						_x = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/GPS/Vel", "i", "0"));
						_y = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/GPS/Vel", "i", "1"));
						_z = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/GPS/Vel", "i", "2"));
						_xx = _x * _x;
						_yy = _y * _y;
						_zz = _z * _z;

						_spd = Math.Sqrt(_xx + _yy + _zz);
						_BindingData.SubPoint.Speed = _spd;
						break;
				}

				// ERROR
				for (int i = 0; i < 2; i++)
				{
					_BindingData.ErrorID[i] = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Error/ID", "i", i.ToString()));
					_BindingData.ErrorLevel[i] = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Error/Level", "i", i.ToString()));
					_BindingData.ErrorTicks[i] = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Error/Ticks", "i", i.ToString()));
					_BindingData.ErrorCategory[i] = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Error/Category", "i", i.ToString()));
					_BindingData.ErrorParam[i] = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Error/Param", "i", i.ToString()));
				}


				if (mode == 2)
				{
					_BindingData.CurrentTime = _BindingData.CurrentTime = double.Parse(XmlDataGet(xmlRoot, "FlightLog/Play/CurrentTime"));
				}

				#region Plot Data
                // 绘图数据列表
				if (_BindingData.PlotWindow.Count > 0)
				{
					for (int i = 0; i < _BindingData.PlotWindow.Count; i++)
					{
						for (int j = 0; j < _BindingData.PlotWindow[i].PlotName.Count; j++)
						{
							int match = _BindingData.PlotWindow[i].PlotName[j].CompareTo("CPU使用率");

							if (match == 0)
							{
							}
							else
							{
								BindingData.PlotWindow[i].PlotValue[j] = double.Parse(PlotDataGet(xmlRoot, "Data/UAV/" + _BindingData.PlotWindow[i].PlotName[j]));
							}
						}
					}
				}
				#endregion

                // 指令到达确认对话框
				_BindingData.CommandReplay = int.Parse(XmlDataGet(xmlRoot, "Data/UAV/Command/Response"));

                //固件版本
				GetFirmwareVersion(xmlRoot);


                // 清除
				dataUAV = string.Empty;
				//System.Diagnostics.Debug.WriteLine("XmlConvert-------------------End " + cnt);
				return (cnt);
				//}
				//else
				//{
				//	System.Diagnostics.Debug.WriteLine("XmlConvert-------------------End *******");
				//	return (-1);
				//}
			}
            //返回一个异常信息
			catch (System.Xml.XmlException ex)
			{
				//System.Diagnostics.Debug.WriteLine("XmlConvert: " + ex.Message);
				System.Windows.Forms.MessageBox.Show("(801) Xml Convert Error : " + ex.Message);
				return (-1);
			}
		}
        // 阅读的消息中获得的数据
		public bool GetFirmwareVersion(XElement xmlRoot)
		{
			string value = string.Empty;

			//if (BindingData.Health.FirmwareVersionH != "NoData" && BindingData.Health.FirmwareVersionL != "NoData")
			//{
			//	return true;
			//}

		   	var element = xmlRoot.XPathSelectElement("Data/UAV/Firm");

			if (element != null)
			{
				foreach(var _e in element.XPathSelectElements("Ver"))
				{
					if (_e.Value != "")
					{
						var attr = _e.Attribute("a");
						if (attr.Value == "H")
						{
							_BindingData.Health.FirmwareVersionH = _e.Value;
						}
						else if (attr.Value == "L")
						{
							_BindingData.Health.FirmwareVersionL = _e.Value;
						}
					}
				}
				foreach (var _e in element.XPathSelectElements("BuildDate"))
				{
					if (_e.Value != "")
					{
						var attr = _e.Attribute("a");
						if (attr.Value == "H")
						{
							_BindingData.Health.FirmwareBuildDateH = _e.Value;
						}
						else if (attr.Value == "L")
						{
							_BindingData.Health.FirmwareBuildDateL = _e.Value;
						}				
					}

				}
				foreach (var _e in element.XPathSelectElements("Edition"))
				{
					if (_e.Value != "")
					{
						var attr = _e.Attribute("a");
						if (attr.Value == "H")
						{
							_BindingData.Health.FirmwareEditionH = _e.Value;
						}
						else if (attr.Value == "L")
						{
							_BindingData.Health.FirmwareEditionL = _e.Value;
						}
					}

				}
			}
		

			if (BindingData.Health.FirmwareVersionH == "NoData" && BindingData.Health.FirmwareVersionL == "NoData")
			{
				return false;
			}

			BindingData.Health.FirmwareVersionH = RenameFirmware(BindingData.Health.FirmwareVersionH);
			BindingData.Health.FirmwareVersionL = RenameFirmware(BindingData.Health.FirmwareVersionL);
			return true;
		}
		private string RenameFirmware(string _firmware)
		{
			string[] stArrayData = _firmware.Split('.');
			string _renameFirmware = "";

			if (stArrayData.Count() == 3)
			{
				_renameFirmware = int.Parse(ValueCheck(stArrayData[0])) + "." + int.Parse(ValueCheck(stArrayData[1])) + "." + int.Parse(ValueCheck(stArrayData[2]));
			}

			return _renameFirmware;
		}

        private string ValueCheck(string value)
        {
            string result = string.Empty;

            double d;
            if (double.TryParse(value, out d))
            {
                int i;
                if (int.TryParse(value, out i))
                {
                    return value;
                }
                else
                {
                    return value;
                }

            }
            else
            {
                return "0";
            }
        }

        public string XmlSelect()
        {
            //System.Diagnostics.Debug.WriteLine("XmlSelect 削除開始：" + Data.Count);
            for (int i = 0; i < Data.Count; i++)
            {
                if (!string.IsNullOrEmpty(Data[i]))
                {
                    int size = Data[i].ToList().Where(c => c.Equals('\n')).Count();

                    if (size == get_Size)
                    {
                        dataUAV = Data[i];
                        Data.RemoveAt(i);
                        i--;
                    }
                }
            }

            return dataUAV;
            //System.Diagnostics.Debug.WriteLine("XmlSelect 削除終了：" + Data.Count);
        }

		// 
		public void XMLClear()
		{
			//_BindingData = new XmlData();
            //_BindingData.Health = new Health.HealthData();		//清除警告设置就基于固件的已经明确的数据
			_BindingData.Health.FirmwareVersionH = "NoData";
			_BindingData.Health.FirmwareVersionL = "NoData";
			Data.Clear();
			dataUAV = ReadXMLFile(XmlMessage.Get_UAVData);
			dataUAV = dataUAV.Replace("\0","");
		}

        // 数值获取的XML数据
        private string XmlDataGet(XElement xmlRoot, string tag, string att = "", string attValue = "")
        {
            try
            {
                //没有标签  
                if (xmlRoot.XPathSelectElement(tag) == null)
                {
                    return "0";
                }

                // 空
                if (String.IsNullOrEmpty(xmlRoot.XPathSelectElement(tag).Value))
                {
                    return "0";
                }

                string val = xmlRoot.XPathSelectElement(tag).Value;
                if (att != "")
                {
                    foreach (var element in xmlRoot.XPathSelectElements(tag))
                    {
                        string _attVal = element.Attribute(att).Value;
                        if (_attVal == attValue)
                        {
                            //System.Diagnostics.Debug.WriteLine("XmlDataGet: " + _attVal + "/" + element.Value);
                            return ValueCheck(element.Value);
                        }
                    }
                    val = "0";
                }

                return ValueCheck(val);
            }
            catch (Exception ex)
            {
                System.Windows.Forms.MessageBox.Show("(802) XmlDataGet Error : " + ex.Message);
                return "0";
            }
        }

        private string PlotDataGet(XElement xmlRoot, string tag)
        {

			//string newValue = tag;
			//string pattern = @"(\[)(?<Name>.+?)(\])";
			//string att = "";
			//string attValue = System.Text.RegularExpressions.Regex.Match(tag, pattern).Groups["Name"].Value;

			//if (attValue != "")
			//{
			//	newValue = newValue.Replace(("[" + attValue + "]"), "");
			//	int i;
			//	if (int.TryParse(attValue, out i))
			//	{
			//		System.Diagnostics.Debug.WriteLine("plotDataGet: " + newValue + attValue);
			//		att = "i";
			//	}
			//	else
			//	{
			//		att = "a";
			//	}
			//}

            return XmlDataGet(xmlRoot, tag);
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        // 状态
        //	FlightMode 返回对应字符串 飞行模式
        private string GetFlightModeStr(int mode)
        {
            if (FlightModeList.Count <= mode)
            {
                return ("未定义");
            }

            return FlightModeList[mode];
        }
        //	ControlMode 返回对应字符串 控制方式
        private ModeManager GetControlModeStr(int mode)
        {
            ModeManager _controlMode = new ModeManager();

            if (ControlModeList.Count <= mode)
            {
                _controlMode.ModeNo = -1;
                _controlMode.ModeName = "未定义";
                _controlMode.Reset = false;
                return _controlMode;
            }

            _controlMode = ControlModeList.Find(
            delegate(ModeManager _modeManager) { return _modeManager.ModeNo == mode; });

            return _controlMode;
        }
        // XML/Setting.xml浏览
        private void InitModeName()
        {
            System.Diagnostics.Debug.WriteLine("");
            ControlModeList = new List<ModeManager>();		// 初始化
            FlightModeList = new List<string>();		// 初始化

            // 读取XML文件的XML数据
            XDocument xml = XDocument.Load(System.Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\X-Monitor\Setting\GUI\Settings.xml");

            // 按Ctrl方式阅读
            var elements = xml.Root.XPathSelectElements("Settings/UAV/Sys/Mode/Ctrl");
            foreach (var element in elements)
            {
                ModeManager _controlMode = new ModeManager();

                // 阅读属性
                if (element.Attribute("i") != null)
                {
                    int i;
                    string _attVal = element.Attribute("i").Value;
                    if (int.TryParse(_attVal, out i))
                    {
                        _controlMode.ModeNo = int.Parse(_attVal);   // 模式号
                        _controlMode.ModeName = element.Value;      // 模式名

                        System.Diagnostics.Debug.WriteLine("ControlModeList :(" + _attVal + ")" + element.Value);
                    }
                    else
                    {
                        _controlMode.ModeNo = -1;               // 模式号
                        _controlMode.ModeName = "未定义";  // 模式名
                    }
                }
                else
                {
                    _controlMode.ModeNo = -1;               // 控制名
                    _controlMode.ModeName = "未定义";  // 控制名
                }

                // 重置属性加载
                if (element.Attribute("reset") != null)
                {
                    string _attVal = element.Attribute("reset").Value;
                    if (_attVal == "Y")
                    {
                        _controlMode.Reset = true;  // 目标值复位 (true)
                        System.Diagnostics.Debug.WriteLine("reset-:" + _attVal);
                    }
                    else
                    {
                        _controlMode.Reset = false;  // 目标值复位 (true)
                        System.Diagnostics.Debug.WriteLine("reset-:" + _attVal);
                    }
                }
                else
                {
                    _controlMode.Reset = false;  // 目标值复位 (true)
                    System.Diagnostics.Debug.WriteLine("reset-:" + "没有");
                }

                ControlModeList.Add(_controlMode);
            }

            // 自动模式下阅读
            elements = xml.Root.XPathSelectElements("Settings/UAV/Sys/Mode/Auto");
            for (int i = 0; i < elements.Count(); i++)
            {
                foreach (var element in elements)
                {
                    // i属性的阅读
                    string _attVal = element.Attribute("i").Value;
                    if (_attVal == i.ToString())
                    {
                        System.Diagnostics.Debug.WriteLine("FlightModeList :(" + i.ToString() + ")" + element.Value);
                        FlightModeList.Add(element.Value);
                    }
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        // MulitiWaypoint用
        // MulitiWaypoint(.wpp)获取
        public void MulitiWaypointRead(string xmltext)
        {
            XmlDocument xml = new XmlDocument();

            // 文本数据的XML数据读取
            xml.LoadXml(xmltext);

            // 从XML数据获得
            XmlElement xmlRoot = xml.DocumentElement;

            _BindingData.FlightPlan = xmlRoot.OuterXml.Replace("\n", "");		//root保存以下信息
			_BindingData.FlightPlan = _BindingData.FlightPlan.Replace(" ", "");

            // 参考点 - 纬度（项目来源） [deg]
            XmlNodeList xmlNode = xmlRoot.GetElementsByTagName("LatOrigin");
            double OriLat = double.Parse(xmlNode[0].InnerXml);

            // 参考点 - 经度（项目来源） [deg]
            xmlNode = xmlRoot.GetElementsByTagName("LonOrigin");
            double OriLng = double.Parse(xmlNode[0].InnerXml);

            /////////////////////////////////////////////////////////////////////
            // TODO:使命仅有一个对应关系

            //从基准点到任务起源南北方向的移动量[m]
            xmlNode = xmlRoot.GetElementsByTagName("NS_Transition");
            double NS_Transition = double.Parse(xmlNode[0].InnerXml);
            // 从基准点到任务起源东西方向的移动量 [m]
            xmlNode = xmlRoot.GetElementsByTagName("EW_Transition");
            double EW_Transition = double.Parse(xmlNode[0].InnerXml);
            // 任务原点的高度[m]
            xmlNode = xmlRoot.GetElementsByTagName("V_Transition");
            double V_Transition = double.Parse(xmlNode[0].InnerXml);
            // 相对于该任务坐标系的x轴的缩放因子
            xmlNode = xmlRoot.GetElementsByTagName("ScaleX");
            double ScaleX = double.Parse(xmlNode[0].InnerXml);
            // 相对于该任务坐标系的y轴的缩放因子
            xmlNode = xmlRoot.GetElementsByTagName("ScaleY");
            double ScaleY = double.Parse(xmlNode[0].InnerXml);
            // 相对于该任务坐标系的z轴的缩放因子
            xmlNode = xmlRoot.GetElementsByTagName("ScaleZ");
            double ScaleZ = double.Parse(xmlNode[0].InnerXml);
            // 任务坐标系的x轴的方位
            xmlNode = xmlRoot.GetElementsByTagName("Rotation");
            double Rotation = double.Parse(xmlNode[0].InnerXml);

            // 水平面的变换
			double c  = Math.Cos(Rotation);
			double s  = Math.Sin(Rotation);
			double sx = ScaleX;
			double sy = ScaleY;
			double T11 =  c * sx;
			double T12 = -s * sy;
			double T21 =  s * sx;
			double T22 =  c * sy;
            //
            /////////////////////////////////////////////////////////////////////

            System.Xml.XmlNodeList nodelist = xml.GetElementsByTagName("waypoint");

            // MulitWaypoint
            MulitWaypointData _mulitWaypointData = new MulitWaypointData();

            _mulitWaypointData.Origin = new PointLatLng(OriLat, OriLng);
            PointLatLng OriginMeter = MeterPerLatLng(_mulitWaypointData.Origin);

            _mulitWaypointData.mCount = nodelist.Count;

            for (int i = 0; i < _mulitWaypointData.mCount; i++)
            {
                //xmlNode = nodelist[i].SelectNodes("ID");
                //int id = int.Parse(xmlNode[0].InnerXml);
                //_mulitWaypointData.mid.Add(id);

                // TODO 仮ID
                _mulitWaypointData.mid.Add(i + 1);

                xmlNode = nodelist[i].SelectNodes("x");
                double x = double.Parse(xmlNode[0].InnerXml);
                xmlNode = nodelist[i].SelectNodes("y");
                double y = double.Parse(xmlNode[0].InnerXml);

                double xx = T11 * (x - NS_Transition) + T12 * (y - EW_Transition);
                double yy = T21 * (x - NS_Transition) + T22 * (y - EW_Transition);


                double lat = _mulitWaypointData.Origin.Lat + xx / OriginMeter.Lat;
                double lng = _mulitWaypointData.Origin.Lng + yy / OriginMeter.Lng;

                PointLatLng Waypoint = new PointLatLng(lat, lng);
                _mulitWaypointData.multi.Add(Waypoint);

                // 高度
                xmlNode = nodelist[i].SelectNodes("height");
                double alt = V_Transition + ScaleZ * double.Parse(xmlNode[0].InnerXml);
                _mulitWaypointData.malt.Add(alt);

                // 方位
                xmlNode = nodelist[i].SelectNodes("psi");
				double psi = double.Parse(xmlNode[0].InnerXml);//Rotation + double.Parse(xmlNode[0].InnerXml);
				psi += Rotation;
                psi *= 180 / Math.PI;	//  rad->deg 変換
                _mulitWaypointData.mpsi.Add(psi);

                xmlNode = nodelist[i].SelectNodes("speed");
                double speed = double.Parse(xmlNode[0].InnerXml);
                _mulitWaypointData.mspeed.Add(speed);

                xmlNode = nodelist[i].SelectNodes("accuracy");
                double accuracy = double.Parse(xmlNode[0].InnerXml);
                _mulitWaypointData.maccuracy.Add(accuracy);

                xmlNode = nodelist[i].SelectNodes("wait");
                double wait = double.Parse(xmlNode[0].InnerXml);
                _mulitWaypointData.mwait.Add(wait);

                xmlNode = nodelist[i].SelectNodes("func_id");
                int func_id = int.Parse(xmlNode[0].InnerXml);
                _mulitWaypointData.mfunc_id.Add(func_id);

            }

            // MulitWaypoint数据更新
            _BindingData.MulitWaypoint = _mulitWaypointData;

        }
        // 飞行计划数据的XY轴的经纬度变换
        PointLatLng MeterPerLatLng(PointLatLng orilat)
        {
            double GPS_ELLIPSOID_A = 6378137.0;
            double GPS_ELLIPSOID_F = 0.00335281066475;
            double GPS_ELLIPSOID_B = (GPS_ELLIPSOID_A * (1.0 - GPS_ELLIPSOID_F));

            double dPseudoLatitudeRad, dDistanceFromCE;
            double dSin, dCos;

            // 变换系数的计算
            PointLatLng r = new PointLatLng();

            //
            //Lat
            //
            // 伪纬度计算
            if (orilat.Lat == 90.0 || orilat.Lat == -90.0)
            {
                // 如果tan的值为单数
                dPseudoLatitudeRad = orilat.Lat * Math.PI / 180.0; // 在这种情况下的伪纬度和纬度是相等的。
            }
            else
            {
                //如果tan的值不是一个单一的
                dPseudoLatitudeRad = Math.Atan((1.0 - GPS_ELLIPSOID_F) * (1.0 - GPS_ELLIPSOID_F) * Math.Tan(orilat.Lat * Math.PI / 180.0));
            }

            // 三角函数计算
            dSin = Math.Sin(dPseudoLatitudeRad);
            dCos = Math.Cos(dPseudoLatitudeRad);

            // 地球中心与位置点之间的距离的计算
            dDistanceFromCE = GPS_ELLIPSOID_A * GPS_ELLIPSOID_B / Math.Sqrt(GPS_ELLIPSOID_A * GPS_ELLIPSOID_A * dSin * dSin + GPS_ELLIPSOID_B * GPS_ELLIPSOID_B * dCos * dCos);

            r.Lat = (Math.PI / 180.0) * dDistanceFromCE;

            //
            //Lng
            //
            r.Lng = (Math.PI / 180.0) * dDistanceFromCE * dCos;


            return r;
        }
        // 消息生成的飞行计划数据
        public string MulitiWaypointSetMassage(long _id, long _funcId = 0, double _lat = 0, double _lon = 0)
        {
            string text = string.Empty;

            if (this.BindingData.MulitWaypoint.multi.Count > 0)
            {
                List<string> tag = new List<string>();
                List<string> value = new List<string>();

                if (_id == 0)
                {
                    // 发送开始
                    tag.Add("Data/UAV/MltWpt/WptId");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Lat");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Lon");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Alt");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Yaw");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Spd");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Tol");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Wait");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/FuncId");
                    value.Add(_funcId.ToString());
                }
                else if (_id == 4294967295)
                {
                    // 发送完成
                    tag.Add("Data/UAV/MltWpt/WptId");
                    value.Add("4294967295");
                    tag.Add("Data/UAV/MltWpt/Lat");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Lon");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Alt");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Yaw");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Spd");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Tol");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/Wait");
                    value.Add("0");
                    tag.Add("Data/UAV/MltWpt/FuncId");
                    value.Add(_funcId.ToString());
                }
                else
                {
                    if (this.BindingData.MulitWaypoint.multi.Count < _id)
                    {
                        return text;
                    }
                    // MulitiWaypoint数据（各Waypoint数据）
                    int i = (int)(_id - 1);

                    tag.Add("Data/UAV/MltWpt/WptId");
                    value.Add(this.BindingData.MulitWaypoint.mid[i].ToString());
                    tag.Add("Data/UAV/MltWpt/Lat");
                    value.Add((this.BindingData.MulitWaypoint.multi[i].Lat + _lat).ToString());
                    tag.Add("Data/UAV/MltWpt/Lon");
					value.Add((this.BindingData.MulitWaypoint.multi[i].Lng + _lon).ToString());
                    tag.Add("Data/UAV/MltWpt/Alt");
                    value.Add(this.BindingData.MulitWaypoint.malt[i].ToString());
                    tag.Add("Data/UAV/MltWpt/Yaw");
                    value.Add(this.BindingData.MulitWaypoint.mpsi[i].ToString());
                    tag.Add("Data/UAV/MltWpt/Spd");
                    value.Add(this.BindingData.MulitWaypoint.mspeed[i].ToString());
                    tag.Add("Data/UAV/MltWpt/Tol");
                    value.Add(this.BindingData.MulitWaypoint.maccuracy[i].ToString());
                    tag.Add("Data/UAV/MltWpt/Wait");
                    value.Add(this.BindingData.MulitWaypoint.mwait[i].ToString());
                    tag.Add("Data/UAV/MltWpt/FuncId");
                    value.Add(this.BindingData.MulitWaypoint.mfunc_id[i].ToString());
                }

                text = this.SetMassage(XmlMessage.MainlinkSend_MultiWaypoint, tag, value);
            }

            return text;
        }

        #endregion

    }

    // 
    public class ModeManager
    {
        public int ModeNo { get; set; }         // 模式.
        public string ModeName { get; set; }    // 模式名
        public bool Reset { get; set; }         // 目標値(Waypoint)重置
    }
}
