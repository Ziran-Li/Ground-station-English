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
    class XmlManager
    {
        public static readonly XmlManager Empty = new XmlManager();

        private List<string> Data;				//XMLdata
		private string dataUAV;
        private int get_Size;				//getメッセージサイズ
        private int mode;					//Commandボタンの状態（ 1:UAV操作モード 2:ログ再生モード）
		private int _reply;					//replyの状態 （0:通常 1:COM設定 2:飛行ログを開く 3:飛行ログを閉じる 4:切り取り保存）
		private List<string> MessageList;	//コア部とのメッセージテンプレートリスト
		private List<string> FlightModeList;
        private List<ModeManager> ControlModeList;
        private XmlData _BindingData;		//

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

        #region プロパティ
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

		// 初期化（XMLファイルのメッセージを読込）
		public void Initialize()
		{
			MessageList = new List<string>();

			// 定義済みのenumを列挙する
			foreach (XmlMessage list in Enum.GetValues(typeof(XmlMessage)))
			{
				MessageList.Add(ReadXMLFile(list));
			}

			InitModeName();	// 制御/飛行モード名一覧の読込み

			Data = new List<string>();
			get_Size = 0;
			mode = 1;
			_reply = 0;
			_BindingData = new XmlData();

		}

		// ファイル選択したファイルの読込
		private string ReadXMLFile(XmlMessage file)
		{
			string path = "Xml/Message/" + file.ToString() + ".xml";

			System.IO.StreamReader read = new System.IO.StreamReader(System.IO.Path.GetFullPath(path), System.Text.Encoding.GetEncoding("UTF-8"));
			string data = read.ReadToEnd();
			data += "\0";

			return data;
		}
		
		// メッセージ選択
		public string ReadMassage(XmlMessage file)
		{
			return MessageList[(int)file];
		}

		// Set/MainlinkSendメッセージの生成
		public string SetMassage(XmlMessage file, List<string> tag, List<string> value, bool base64=true)
		{
			string text = MessageList[(int)file];
			text = text.Replace("\0", "");
			XDocument xml = XDocument.Parse(text);                                   // textデータをxmlデータに読み込む

			int i = 0;
			foreach (var list in tag)
			{
				if (xml.Root.Element("message").XPathSelectElement(list) != null)
				{
					string data;
					var attr = xml.Root.Element("message").XPathSelectElement(list).Attribute("t");
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
				result = string.Empty;	//要素に空の内容を格納
			}

			return result;
		}

		// 取得したメッセージ内のデータの読込み
		public string GetMassage(string tag, bool delFlag=true)
		{
			string value = string.Empty;

			for (int i = 0; i < Data.Count; i++)
			{
				if (!string.IsNullOrEmpty(Data[i]))
				{
					// textデータをxmlデータに読み込む
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
			
			XDocument xmltext  = XDocument.Parse(text);
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

		// XML形式のデータが受信できたか確認処理
		public bool XmlDataCheck()
		{
			if (!string.IsNullOrEmpty(dataUAV))
			{
				string xmltext = dataUAV;

				// 受信データを正常確認する
				int Index = xmltext.IndexOf("xml version");
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
		// 取得したメッセージ内のデータの読込み<---UAVデータ取得リストに変更がある場合、ココに追加すること	
		public int XmlConvert()
		{
			try
			{
				if (dataUAV == "")
				{
					return (-1);
				}

				int cnt = 0;
				int row = dataUAV.ToList().Where(c => c.Equals('\n')).Count();
				//System.Diagnostics.Debug.WriteLine("XmlConvert-------------------Start " + dataUAV.Length + "[" + get_Size + ">" + row + "]");
				if ((row == get_Size))
				{
					XDocument xml = XDocument.Parse(dataUAV);	// textデータをxmlデータに読み込む
					//dataUAV = string.Empty;						// クリア
					var xmlRoot = xml.Root.Element("message");	// xmlからデータを取得

					// IMU
					_BindingData.UAVpoint.Roll = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/IMU/Att", "i", "0"));
					_BindingData.UAVpoint.Pitch = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/IMU/Att", "i", "1"));
					_BindingData.UAVpoint.Yaw = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/IMU/Att", "i", "2"));
					// Ctrl
					
					// Nav
					_BindingData.UAVpoint.Lon = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Nav/Lon"));
					_BindingData.UAVpoint.Lat = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Nav/Lat"));
					_BindingData.UAVpoint.Alt = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Nav/Alt"));
					// Ref
					// Wpt
					_BindingData.NWaypoint.Lon = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Wpt/Lon"));
					_BindingData.NWaypoint.Lat = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Wpt/Lat"));
					_BindingData.NWaypoint.Alt = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Wpt/Alt"));
					_BindingData.NWaypoint.Yaw = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Wpt/Yaw"));
					_BindingData.NWaypoint.Speed = double.Parse(XmlDataGet(xmlRoot, "Data/UAV/Wpt/Spd"));
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

					if (mode == 2)
					{
						_BindingData.CurrentTime = _BindingData.CurrentTime = double.Parse(XmlDataGet(xmlRoot, "FlightLog/Play/CurrentTime"));
					}

					#region Plot Data
					// プロットデータリスト
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

					//dataUAV = string.Empty;
					//System.Diagnostics.Debug.WriteLine("XmlConvert-------------------End " + cnt);
					return (cnt);
				}
				else
				{
					System.Diagnostics.Debug.WriteLine("XmlConvert-------------------End *******");
					return (-1);
				}
			}
			catch (System.Xml.XmlException ex)
			{
				//System.Diagnostics.Debug.WriteLine("XmlConvert: " + ex.Message);
				System.Windows.Forms.MessageBox.Show("(801) Xml Convert Error : " + ex.Message);
				return (-1);
			}
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

		// XMLデータ内の数値取得
		private string XmlDataGet(XElement xmlRoot, string tag, string att = "", string attValue = "")
		{
			try
			{
				//タグがない場合
				if (xmlRoot.XPathSelectElement(tag) == null)
				{
					return "0";
				}

				// 空の場合
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
			
			string newValue = tag;
			string pattern = @"(\[)(?<Name>.+?)(\])";
			string att = ""; 
			string attValue = System.Text.RegularExpressions.Regex.Match(tag, pattern).Groups["Name"].Value;

			if (attValue != "")
			{
				newValue = newValue.Replace(("[" + attValue + "]"), "");
				int i;
				if (int.TryParse(attValue, out i))
				{
					System.Diagnostics.Debug.WriteLine("plotDataGet: " + newValue + attValue);
					att = "i";
				}
				else
				{
					att = "a";
				}
			}

			return XmlDataGet(xmlRoot, newValue, att, attValue);
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		// モード
        //	FlightMode に対応する文字列を返す
        private string GetFlightModeStr(int mode)
        {
			if (FlightModeList.Count <= mode)
			{
				return ("未定義");
			}

			return FlightModeList[mode];
        }
        //	ControlMode に対応する文字列を返す
        private ModeManager GetControlModeStr(int mode)
        {
            ModeManager _controlMode = new ModeManager();

			if (ControlModeList.Count <= mode)
			{
                _controlMode.ModeNo = -1;
                _controlMode.ModeName = "未定義";
                _controlMode.Reset = false;
                return _controlMode;
			}

            _controlMode = ControlModeList.Find(
            delegate(ModeManager _modeManager) { return _modeManager.ModeNo == mode; });

            return _controlMode;
        }
        // XML/Setting.xml読み込み
        private void InitModeName()
		{
			System.Diagnostics.Debug.WriteLine("");
            ControlModeList = new List<ModeManager>();		// 初期化
			FlightModeList = new List<string>();		// 初期化

			// XMLファイルからxmlデータに読み込む
            XDocument xml = XDocument.Load(System.Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\X-Monitor\Setting\GUI\Settings.xml");

            // Ctrlモード読み込み
			var elements = xml.Root.XPathSelectElements("Settings/UAV/Sys/Mode/Ctrl");
            foreach (var element in elements)
            {
                ModeManager _controlMode = new ModeManager();

                // i属性の読み込み
                if (element.Attribute("i") != null)
                {
                    int i;
                    string _attVal = element.Attribute("i").Value;
                    if (int.TryParse(_attVal, out i))
                    {
                        _controlMode.ModeNo = int.Parse(_attVal);   // モードNo
                        _controlMode.ModeName = element.Value;      // モード名

                        System.Diagnostics.Debug.WriteLine("ControlModeList :(" + _attVal + ")" + element.Value);
                    }
                    else
                    {
                        _controlMode.ModeNo = -1;               // モードNo
                        _controlMode.ModeName = "未定義";  // モード名
                    }
                }
                else 
                {
                    _controlMode.ModeNo = -1;               // モードNo
                    _controlMode.ModeName = "未定義";  // モード名
                }

                // reset属性の読み込み
                if (element.Attribute("reset") != null)
                {
                    string _attVal = element.Attribute("reset").Value;
                    if (_attVal == "Y")
                    {
                        _controlMode.Reset = true;  // 目標値リセット(true)
                        System.Diagnostics.Debug.WriteLine("reset-:" + _attVal);
                    }
                    else
                    {
                        _controlMode.Reset = false;  // 目標値リセット(true)
                        System.Diagnostics.Debug.WriteLine("reset-:" + _attVal);
                    }
                }
                else
                {
                    _controlMode.Reset = false;  // 目標値リセット(false)
                    System.Diagnostics.Debug.WriteLine("reset-:" + "なし");
                }

                ControlModeList.Add(_controlMode);
            }

            // Autoモード読み込み
			elements = xml.Root.XPathSelectElements("Settings/UAV/Sys/Mode/Auto");
			for (int i = 0; i < elements.Count(); i++)
			{
				foreach (var element in elements)
				{
                    // i属性の読み込み
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
        // MulitiWaypoint(.wpp)読込
        public void MulitiWaypointRead(string xmltext)
        {
            XmlDocument xml = new XmlDocument();

            // textデータをxmlデータに読み込む
            xml.LoadXml(xmltext);

            // xmlからデータを取得
            XmlElement xmlRoot = xml.DocumentElement;

            XmlNodeList xmlNode = xmlRoot.GetElementsByTagName("LatOrigin");
            double OriLat = double.Parse(xmlNode[0].InnerXml);
            xmlNode = xmlRoot.GetElementsByTagName("LonOrigin");
            double OriLng = double.Parse(xmlNode[0].InnerXml);

            System.Xml.XmlNodeList nodelist = xml.GetElementsByTagName("waypoint");

            //
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
				_mulitWaypointData.mid.Add(i+1);

                xmlNode = nodelist[i].SelectNodes("x");
                double lat = _mulitWaypointData.Origin.Lat + double.Parse(xmlNode[0].InnerXml) / OriginMeter.Lat;
                xmlNode = nodelist[i].SelectNodes("y");
                double lng = _mulitWaypointData.Origin.Lng + double.Parse(xmlNode[0].InnerXml) / OriginMeter.Lng;
                PointLatLng Waypoint = new PointLatLng(lat, lng);
                _mulitWaypointData.multi.Add(Waypoint);

                xmlNode = nodelist[i].SelectNodes("height");
                double alt = double.Parse(xmlNode[0].InnerXml);
                _mulitWaypointData.malt.Add(alt);

                xmlNode = nodelist[i].SelectNodes("psi");
                double psi = double.Parse(xmlNode[0].InnerXml);
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

            // MulitWaypointデータの更新
            _BindingData.MulitWaypoint = _mulitWaypointData;

        }
        // 飛行計画データのXYをLatLng変換 ()
        PointLatLng MeterPerLatLng(PointLatLng orilat)
        {
            double GPS_ELLIPSOID_A = 6378137.0;
            double GPS_ELLIPSOID_F = 0.00335281066475;
            double GPS_ELLIPSOID_B = (GPS_ELLIPSOID_A * (1.0 - GPS_ELLIPSOID_F));

            double dPseudoLatitudeRad, dDistanceFromCE;
            double dSin, dCos;

            // 変換定数の計算
            PointLatLng r = new PointLatLng();

            //
            //Lat
            //
            // 擬似緯度の計算
            if (orilat.Lat == 90.0 || orilat.Lat == -90.0)
            {
                // tan の値が特異となる場合
                dPseudoLatitudeRad = orilat.Lat * Math.PI / 180.0; // この場合は擬似緯度と緯度が等しい。
            }
            else
            {
                // tan の値が特異とならない場合
                dPseudoLatitudeRad = Math.Atan((1.0 - GPS_ELLIPSOID_F) * (1.0 - GPS_ELLIPSOID_F) * Math.Tan(orilat.Lat * Math.PI / 180.0));
            }

            // 三角関数の計算
            dSin = Math.Sin(dPseudoLatitudeRad);
            dCos = Math.Cos(dPseudoLatitudeRad);

            // 地球中心と現在地点との距離の計算
            dDistanceFromCE = GPS_ELLIPSOID_A * GPS_ELLIPSOID_B / Math.Sqrt(GPS_ELLIPSOID_A * GPS_ELLIPSOID_A * dSin * dSin + GPS_ELLIPSOID_B * GPS_ELLIPSOID_B * dCos * dCos);

            r.Lat = (Math.PI / 180.0) * dDistanceFromCE;

            //
            //Lng
            //
            r.Lng = (Math.PI / 180.0) * dDistanceFromCE;


            return r;
        }
		// 飛行計画データのメッセージ生成
		public string MulitiWaypointSetMassage(long _id, long _funcId = 0)
		{
			string text = string.Empty;

			if (this.BindingData.MulitWaypoint.multi.Count > 0)
			{
				List<string> tag = new List<string>();
				List<string> value = new List<string>();	

				if (_id == 0)
				{
					// 転送開始時
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
					// 転送完了時
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
					// MulitiWaypointデータ（各Waypointデータ）
					int i = (int)(_id - 1);

					tag.Add("Data/UAV/MltWpt/WptId");
					value.Add(this.BindingData.MulitWaypoint.mid[i].ToString());
					tag.Add("Data/UAV/MltWpt/Lat");
					value.Add(this.BindingData.MulitWaypoint.multi[i].Lat.ToString());
					tag.Add("Data/UAV/MltWpt/Lon");
					value.Add(this.BindingData.MulitWaypoint.multi[i].Lng.ToString());
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
    class ModeManager
    {
        public int ModeNo { get; set; }         // モードNo.
        public string ModeName { get; set; }    // モード名
        public bool Reset { get; set; }         // 目標値(Waypoint)リセット
    }
}
