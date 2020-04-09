using System;
using System.Collections.Generic;
//Process関連
using System.Windows;
using System.Xml.Linq;
using System.Xml.XPath;

using Health;

namespace XMonitor_GUI
{
    /// <summary>
    /// DataWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class DataWindow
    {
        HealthData HealthDataSet;   //ヘルスデータ管理
		UAVList _uavData;
		UAVList _waypointData;

        public DataWindow()
        {
            InitializeComponent();
			GetDataList();

			_uavData = new UAVList()
			{
				Name = "UAV[Nav]",
				Lat = "",
				Lon = "",
				Alt = "",
				Yaw = "" 
			};
			_waypointData = new UAVList()
			{
				Name = "目標[Ref]",
				Lat = "",
				Lon = "",
				Alt = "",
				Yaw = ""
			};
			UAVListView.Items.Add(_uavData);
			UAVListView.Items.Add(_waypointData);
        }
		// データリスト
		List<string> _tagHi;
		List<string> _tagLo;
		XElement _element;
		private void GetDataList()
		{
			#region プロットデータリストの読込
			//
			var xmle = XDocument.Load(@"Xml/Settings.xml");				//    
			_element = xmle.Root.Element("Settings").Element("PlotContents");


			//XMLファイルを読み込む
			XDocument xml = XDocument.Load(@"Xml/Message/Get_UAVData.xml");				//     
			var xmlRoot = xml.Root.Element("message").Element("Data").Element("UAV");   // xmlからデータを取得

			xml = XDocument.Load(System.Environment.GetFolderPath(System.Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\X-Monitor\Setting\GUI\Settings.xml");
			_tagHi = new List<string>();
			_tagLo = new List<string>();


			foreach (var element in xml.Root.XPathSelectElements("Settings/Plot/Tag"))
			{
				string _attVal = element.Attribute("a").Value;
				if (_attVal == "H")
				{
					_tagHi.Add(element.Value);
				}
				else
				{
					_tagLo.Add(element.Value);
				}
			}

			System.Xml.XmlDocument xmlDoc = new System.Xml.XmlDocument();
			xmlDoc.LoadXml(xmlRoot.ToString());											//変換
			System.Xml.XmlElement xmlRoot1 = xmlDoc.DocumentElement;

			string trvChild = string.Empty;
			Make(xmlRoot1, ref trvChild);		//タグ名の抽出

			//PlotList.Items.Add("CPU使用率");

			DataList _xmlTag = new DataList
			{
				XMLTag = "CPU使用率",
				Contents = "PCの使用率",
				Value = "NoData"
			};
			TagList.Items.Add(_xmlTag);
			#endregion
		}
		// 親ノードと子ノードの抽出
		private void Make(System.Xml.XmlElement xmlParent, ref string trvParent)
		{
			if (xmlParent.HasChildNodes)
			{
				for (int i = 0; i < xmlParent.ChildNodes.Count; i++)
				{
					if (xmlParent.ChildNodes[i].GetType().ToString().IndexOf("XmlElement") >= 0)
					{
						System.Xml.XmlElement xmlChild = (System.Xml.XmlElement)xmlParent.ChildNodes[i];
						string trvChild;

						trvChild = xmlChild.Name;


						bool _flagHi = true;

						// 除外タグのチェック
						foreach (var tag in _tagHi)
						{
							if (tag == trvChild)
							{
								_flagHi = false;
								break;
							}
						}

						// 親ノード追加
						if (_flagHi && (trvParent == null || trvParent == ""))
						{
							// 子ノードのチェック
							if (xmlChild.HasChildNodes)
							{
								Make(xmlChild, ref trvChild);
							}
							else
							{
								//iPlotList.Items.Add(trvChild);
								string _atti = xmlChild.GetAttribute("i");
								string _atta = xmlChild.GetAttribute("a");
								string _attt = xmlChild.GetAttribute("t");
								GetXmlPathAIT(trvChild, _atta, _atti, _attt);
							}
						}

						// 親ノードがある場合
						if (trvParent != null && trvParent != "")
						{
							bool _flagLo = true;
							// 除外タグのチェック
							foreach (var tag in _tagLo)
							{
								if (tag == trvChild)
								{
									_flagLo = false;
									break;
								}
							}

							// 親子ノード追加
							if (_flagLo)
							{
								if (xmlChild.HasChildNodes)
								{
									trvChild = trvParent + "/" + trvChild;
									Make(xmlChild, ref trvChild);
								}
								else
								{
									string _atti = xmlChild.GetAttribute("i");
									string _atta = xmlChild.GetAttribute("a");
									string _attt = xmlChild.GetAttribute("t");

									GetXmlPathAIT(trvParent + "/" + trvChild, _atta, _atti, _attt);

								}
							}
						}
					}
				}
			}
			//親ノードの設定
			trvParent = xmlParent.Name;
		}
		//　タグリスト抽出
		private DataList GetXmlPathAIT(string _path, string _a, string _i, string _t)
		{
			DataList _xmlTag = new DataList
			{
				XMLTag = _path,
				Contents = "NoData",
				Value = "NoData"
			};
			foreach (var _e in _element.XPathSelectElements(_path))
			{
				if (_e != null)
				{
					var _aa = _e.Attribute("a");
					var _ai = _e.Attribute("i");
                    var _at = _e.Attribute("t");

                    string _ata = ""; if (_aa != null) _ata = _aa.Value;
                    string _ati = ""; if (_ai != null) _ati = _ai.Value;
                    string _att = ""; if (_at != null) _att = _at.Value;

                    if ((_ata == _a) && (_ati == _i) && (_att == _t))
                    {
                        if (_a != "") _xmlTag.XMLTag += "[@" + _aa + "]";
                        if (_t != "") _xmlTag.XMLTag += "[@" + _at + "]";
						if (_i != "") _xmlTag.XMLTag += "[@" + _ai + "]";
						_xmlTag.Contents = _e.Value;
					}
				}
			}
			TagList.Items.Add(_xmlTag);

			return _xmlTag;
		}
        // 「×」（閉じる）ボタン 
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Hide();     //非表示
            //Close();
        }

        /// <summary>
        /// Health Data
        /// </summary>
        //　ヘルスデータプロパティ
        public HealthData HealthDataSetting
        {
            get { return (HealthData)HealthDataSet; }
            set { HealthDataSet = value; HealthData_Update(); }
        }
        //　ヘルスデータ更新
        private void HealthData_Update()
        {
            // Prosessor
            LowLvMcuProcTimeBar.Value   = HealthDataSet.LowLvMcuProcTime;
            HighLvMcuProcTimeBar.Value  = HealthDataSet.HighLvMcuProcTime;
            LowLvMcuProcTimeText.Text   = HealthDataSet.LowLvMcuProcTime.ToString()     + "%";
            HighLvMcuProcTimeText.Text  = HealthDataSet.HighLvMcuProcTime.ToString()    + "%";

            // LowerLevelMCU
            LLMCU_RcReceiverCommunicationBar.Value  = HealthDataSet.LLMCU_RcReceiverCommunication;
            LLMCU_RcReceiverReceptionBar.Value      = HealthDataSet.LLMCU_RcReceiverReception;
            LLMCU_GyroSensorCommunicationBar.Value  = HealthDataSet.LLMCU_GyroSensorCommunication;
            LLMCU_GyroSensorMeasurementBar.Value    = HealthDataSet.LLMCU_GyroSensorMeasurement;
            LLMCU_RcReceiverCommunicationText.Text  = HealthDataSet.LLMCU_RcReceiverCommunication.ToString()    + "/" + LLMCU_RcReceiverCommunicationBar.Maximum.ToString();
            LLMCU_RcReceiverReceptionText.Text      = HealthDataSet.LLMCU_RcReceiverReception.ToString() + "/" + LLMCU_RcReceiverReceptionBar.Maximum.ToString();
            LLMCU_GyroSensorCommunicationText.Text  = HealthDataSet.LLMCU_GyroSensorCommunication.ToString() + "/" + LLMCU_GyroSensorCommunicationBar.Maximum.ToString();
            LLMCU_GyroSensorMeasurementText.Text    = HealthDataSet.LLMCU_GyroSensorMeasurement.ToString() + "/" + LLMCU_GyroSensorMeasurementBar.Maximum.ToString();


            // LowerLevelMCU - HigherLevelMCU Communication
            McuComm_LowToHighBar.Value = HealthDataSet.McuComm_LowToHigh;
            McuComm_HighToLowBar.Value = HealthDataSet.McuComm_HighToLow;
            McuComm_LowToHighText.Text = HealthDataSet.McuComm_LowToHigh.ToString() + "/" + McuComm_LowToHighBar.Maximum.ToString();
            McuComm_HighToLowText.Text = HealthDataSet.McuComm_HighToLow.ToString() + "/" + McuComm_HighToLowBar.Maximum.ToString();

            // HigherLevelMCU
            HLMCU_IMUCommunicationBar.Value         = HealthDataSet.HLMCU_IMUCommunication;
            HLMCU_IMUGyroMeasurementBar.Value       = HealthDataSet.HLMCU_IMUGyroMeasurement;
            HLMCU_IMUAccMeasurementBar.Value        = HealthDataSet.HLMCU_IMUAccMeasurement;
            HLMCU_IMUMagMeasurementBar.Value        = HealthDataSet.HLMCU_IMUMagMeasurement;
            HLMCU_PresSensorCommunicationBar.Value  = HealthDataSet.HLMCU_PresSensorCommunication;
            HLMCU_PresSensorMeasurementBar.Value    = HealthDataSet.HLMCU_PresSensorMeasurement;
            HLMCU_GpsReceiverCommunicationBar.Value = HealthDataSet.HLMCU_GpsReceiverCommunication;
            HLMCU_GpsMeasurementBar.Value			= HealthDataSet.HLMCU_GpsMeasurement;

            HLMCU_IMUCommunicationText.Text = HealthDataSet.HLMCU_IMUCommunication.ToString()                   + "/" + HLMCU_IMUCommunicationBar.Maximum.ToString();
            HLMCU_IMUGyroMeasurementText.Text = HealthDataSet.HLMCU_IMUGyroMeasurement.ToString()               + "/" + HLMCU_IMUGyroMeasurementBar.Maximum.ToString();
            HLMCU_IMUAccMeasurementText.Text = HealthDataSet.HLMCU_IMUAccMeasurement.ToString()                 + "/" + HLMCU_IMUAccMeasurementBar.Maximum.ToString();
            HLMCU_IMUMagMeasurementText.Text = HealthDataSet.HLMCU_IMUMagMeasurement.ToString()                 + "/" + HLMCU_IMUMagMeasurementBar.Maximum.ToString();
            HLMCU_PresSensorCommunicationText.Text = HealthDataSet.HLMCU_PresSensorCommunication.ToString()     + "/" + HLMCU_PresSensorCommunicationBar.Maximum.ToString();
            HLMCU_PresSensorMeasurementText.Text = HealthDataSet.HLMCU_PresSensorMeasurement.ToString()         + "/" + HLMCU_PresSensorMeasurementBar.Maximum.ToString();
            HLMCU_GpsReceiverCommunicationText.Text = HealthDataSet.HLMCU_GpsReceiverCommunication.ToString()   + "/" + HLMCU_GpsReceiverCommunicationBar.Maximum.ToString();
            HLMCU_GpsMeasurementText.Text = HealthDataSet.HLMCU_GpsMeasurement.ToString()						+ "/" + HLMCU_GpsMeasurementBar.Maximum.ToString();

            // UAV - Ground Control Station Cummunication
            MainLink_UavToGcsBar.Value = HealthDataSet.MainLink_UavToGcs;
            MainLink_GcsToUavBar.Value = HealthDataSet.MainLink_GcsToUav;
            MainLink_UavToGcsText.Text = HealthDataSet.MainLink_UavToGcs.ToString() + "/" + MainLink_UavToGcsBar.Maximum.ToString();
            MainLink_GcsToUavText.Text = HealthDataSet.MainLink_GcsToUav.ToString() + "/" + MainLink_GcsToUavBar.Maximum.ToString();

        }

		public static readonly DependencyProperty MessageProperty = DependencyProperty.Register(
			"Message", typeof(string), typeof(DataWindow),
			new FrameworkPropertyMetadata(
			default(string),
			FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
			OnMessageChanged));

		private static void OnMessageChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			try
			{
				DataWindow control = (DataWindow)d;
				string _getMessage = (string)e.NewValue;

				if (_getMessage != "")
				{
					XDocument xml = XDocument.Parse(_getMessage);	// textデータをxmlデータに読み込む
					//dataUAV = string.Empty;						// クリア
					var xmlRoot = xml.Root.Element("message");	// xmlからデータを取得

					for (int i = 0; i < control.TagList.Items.Count; i++)
					{
						var _t = (DataList)control.TagList.Items[i];
						int match = _t.XMLTag.CompareTo("CPU使用率");
						if (match == 0)
						{
						}
						else
						{
							_t.Value = control.PlotDataGet(xmlRoot, "Data/UAV/" + _t.XMLTag);
						}
						control.TagList.Items[i] = _t;

						if(_t.Value != "NoData")
						{
							if (_t.XMLTag == @"Nav/Lat[@t=""R""]")
							{
								control._uavData.Lat = double.Parse(_t.Value).ToString() + "　[deg]";
							}
							if (_t.XMLTag == @"Nav/Lon[@t=""R""]")
							{
								control._uavData.Lon = double.Parse(_t.Value).ToString() + "　[deg]";
							}
							if (_t.XMLTag == @"Nav/Alt[@t=""R""]")
							{
								control._uavData.Alt = double.Parse(_t.Value).ToString() + "　[m]";
							}
							if (_t.XMLTag == @"IMU/Att[@t=""R3""][@i=""2""]")
							{
								control._uavData.Yaw = double.Parse(_t.Value).ToString() + "　[deg]";
							}

							if (_t.XMLTag == @"Wpt/Lat[@t=""R""]")
							{
								control._waypointData.Lat = double.Parse(_t.Value).ToString() + "　[deg]";
							}
							if (_t.XMLTag == @"Wpt/Lon[@t=""R""]")
							{
								control._waypointData.Lon = double.Parse(_t.Value).ToString() + "　[deg]";
							}
							if (_t.XMLTag == @"Wpt/Alt[@t=""R""]")
							{
								control._waypointData.Alt = double.Parse(_t.Value).ToString() + "　[m]";
							}
							if (_t.XMLTag == @"Wpt/Yaw[@t=""R""]")
							{
								control._waypointData.Yaw = double.Parse(_t.Value).ToString() + "　[deg]";
							}
						}
					}
					control.TagList.Items.Refresh();

					control.UAVListView.Items[0] = control._uavData;
					control.UAVListView.Items[1] = control._waypointData;
					control.UAVListView.Items.Refresh();
				}
			}
			catch
			{

			}

		}

		public string Message
		{
			get { return (string)GetValue(MessageProperty); }
			set { SetValue(MessageProperty, value); }
		}

		private string PlotDataGet(XElement xmlRoot, string tag)
		{
			//string newValue = tag;
			////string pattern = @"(\[)(?<Name>.+?)(\])";
			//string att = "";
			////string attValue = System.Text.RegularExpressions.Regex.Match(tag, pattern).Groups["Name"].Value;

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
			string _val = XmlDataGet(xmlRoot, tag);
			return _val;
		}
		// XMLデータ内の数値取得
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
				//return "NoData";
				return value;
			}
		}
		private string XmlDataGet(XElement xmlRoot, string tag, string att = "", string attValue = "")
		{
			try
			{
				//タグがない場合
				if (xmlRoot.XPathSelectElement(tag) == null)
				{
					return "NoData";
				}

				// 空の場合
				if (String.IsNullOrEmpty(xmlRoot.XPathSelectElement(tag).Value))
				{
					return "NoData";
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
					val = "NoData";
				}

				return ValueCheck(val);
			}
			catch (Exception ex)
			{
				System.Windows.Forms.MessageBox.Show("(802) XmlDataGet Error : " + ex.Message);
				return "0";
			}
		}
		public void PlotDataClear()
		{
			try
			{
					for (int i = 0; i < TagList.Items.Count; i++)
					{
						var _t = (DataList)TagList.Items[i];
						int match = _t.XMLTag.CompareTo("CPU使用率");
						if (match == 0)
						{
						}
						else
						{
							_t.Value = "NoData";
						}
						TagList.Items[i] = _t;

						if (_t.XMLTag == "Nav/Lat")
						{
							_uavData.Lat = "NoData";
						}
						if (_t.XMLTag == "Nav/Lon")
						{
							_uavData.Lon = "NoData";
						}
						if (_t.XMLTag == "Nav/Alt")
						{
							_uavData.Alt = "NoData";
						}
						if (_t.XMLTag == "IMU/Att[2]")
						{
							_uavData.Yaw = "NoData";
						}

						if (_t.XMLTag == "Wpt/Lat")
						{
							_waypointData.Lat = "NoData";
						}
						if (_t.XMLTag == "Wpt/Lon")
						{
							_waypointData.Lon = "NoData";
						}
						if (_t.XMLTag == "Wpt/Alt")
						{
							_waypointData.Alt = "NoData";
						}
						if (_t.XMLTag == "Wpt/Yaw")
						{
							_waypointData.Yaw = "NoData";
						}
					}
					TagList.Items.Refresh();

					UAVListView.Items[0] = _uavData;
					UAVListView.Items[1] = _waypointData;
					UAVListView.Items.Refresh();
			
			}
			catch
			{

			}
		}
    }

	public class DataList
	{
		public string XMLTag { get; set; }
		public string Contents { get; set; }
		public string Value { get; set; }
	}
	public class UAVList
	{
		public string Name { get; set; }
		public string Lat { get; set; }
		public string Lon { get; set; }
		public string Alt { get; set; }
		public string Yaw { get; set; }
	}
}
