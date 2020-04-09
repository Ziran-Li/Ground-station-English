// 既存ライブラリ
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Collections.ObjectModel;
using System.Xml;
using System.IO.Ports;  //串行端口
using System.Diagnostics;
using System.Net;
using System.Xml.Linq;
using System.Xml.XPath;

// GMap.NETライブラリ
using GMapNetInterFace;
using GMap.NET;
using GMap.NET.MapProviders;

// log4net
using log4net;
using log4net.Appender;
using log4net.Repository.Hierarchy;

// 自作ライブラリ
using Health;
using SettingData;
using Config;
using System.IO;
namespace XMonitor_GUI
{
    /// <summary>
    /// SettingWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class SettingWindow
    {
        /// <summary>
        /// GMap Plugin Dictionary
        /// </summary>
        Dictionary<String, String> dictGmapPlugin = new Dictionary<string, string>();

        // ログ
        private static readonly ILog _logger = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        HealthData HealthDataSet;
        SetData SettingData = new SetData();
        private PointLatLng point;
        private string path = string.Empty;
        private int _mode;

        #region Main　設定画面
        public SettingWindow()
        {
            InitializeComponent();
            _logger.Info("SettingWindow Open-Init");
            //comboBoxMapType.ItemsSource = GMapProviders.List;

            // すべてのシリアル・ポート名を取得する
            string[] ports = SerialPort.GetPortNames();

            // シリアルポートを毎回取得して表示するために表示の度にリストをクリアする
            //			PortComboBox.Items.Clear();
            foreach (string port in ports)
            {
                // 取得したシリアル・ポート名を出力する
                //				PortComboBox.Items.Add(port);
            }
            SelectMapProviders();

            if (_addressRecord == null)
            {
                _addressRecord = new List<AddressData>();

            }
            MapChange = false;


            if (File.Exists("C:\\AutoWing\\Maintain_Timers.txt"))
            {
                StreamReader sr = File.OpenText("C:\\AutoWing\\Maintain_Timers.txt");
                int k = 0;//当前读取的是第几行
                String[] str = new String[2];
                while (sr.Peek() != -1)
                {
                    str[k] = sr.ReadLine();
                    k++;
                }
                M_Text_Set.Text = str[0];//上次设置的保养时间
                sr.Close();
            }


            if (File.Exists("C:\\AutoWing\\HangpaiZhibaoStyle_Style.txt"))
            {
                StreamReader sr = File.OpenText("C:\\AutoWing\\HangpaiZhibaoStyle_Style.txt");
                int k = 0;//当前读取的是第几行
                String[] str = new String[1];
                while (sr.Peek() != -1)
                {
                    str[k] = sr.ReadLine();
                    k++;
                }
                int choose_style = Convert.ToInt16(str[0]);//当前选的是哪种模式
                if (choose_style == 0)
                {
                    zhibao_style.IsChecked = true;
                }
                else
                {
                    hangpai_style.IsChecked = true;
                }
                sr.Close();
            }
            else
            {
                zhibao_style.IsChecked = true;
            }

        }
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {

            _logger.Info("Window_Loaded-S");
            if (_addressRecord == null)
            {
                _addressRecord = new List<AddressData>();

            }

            foreach (var _add in _addressRecord)
            {
                AddressListView.Items.Add(_add);
            }
            _addressFlag = false;

            //Core設定ファイル
            // XMLファイルからxmlデータに読み込む
            if (SettingData.Mode == 1)
            {
                //				FlightLogFilePath.IsEnabled = false;
            }
            else
            {
                //				FlightLogFilePath.IsEnabled = true;
            }

            //XDocument xml = XDocument.Load(System.Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\X-Monitor\Setting\Core\Setting.xml");
            //string flightLogPath = xml.Root.XPathSelectElement("FlightLog/FilePath").Value;

            //FlightLogFilePath.Text = SettingData.FlightLogFilePath;
            //DivideSizeInByte.Text = SettingData.DivideSizeInByte.ToString();
            //StoragePeriodInDay.Text = SettingData.StoragePeriodInDay.ToString();
            //RecStopTimeFromDataStop.Text = SettingData.RecStopTimeFromDataStop.ToString();

            SettingData.SendCommandFlag = false;
            _mode = 0;

            //			_logger.Info("FlightLog/FilePath:" + FlightLogFilePath.Text);
            _logger.Info("Window_Loaded-E");
        }
        // 1.9.1「×」ボタン
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // Dialog box canceled
            this.DialogResult = false;
            //Close();
        }
        // 「OK」ボタン
        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            if (M_Text_Set.Text != "")
            {
                Constants.User_Set_Maintaintimer = Convert.ToDouble(M_Text_Set.Text);
            }
            if (M_Text_Set.Text == "")//如果倒计时为空
            {
                System.Windows.MessageBox.Show("The Timer Can Not Be Null");
                this.DialogResult = true;
                return;
            }
            StreamWriter sw = new StreamWriter("C:\\AutoWing\\Maintain_Timers.txt");
            sw.WriteLine(Constants.User_Set_Maintaintimer.ToString());
            sw.WriteLine(Constants.User_Set_MaintainStartToEndTime.ToString());//存设置的时间加上当前飞控时间
            sw.Flush();
            sw.Close();

            if (zhibao_style.IsChecked == true)
            {
                Constants.Style_Change_HangPaiAndZhiBao_Flag = 0;//植保模式
            }
            else
            {
                Constants.Style_Change_HangPaiAndZhiBao_Flag = 1;//航拍模式
            }
            StreamWriter sw2 = new StreamWriter("C:\\AutoWing\\HangpaiZhibaoStyle_Style.txt");
            sw2.WriteLine(Constants.Style_Change_HangPaiAndZhiBao_Flag.ToString());
            sw2.Flush();
            sw2.Close();

            this.DialogResult = true;
            // Map選択されている場合、Map選択状態／MapProviderDLL設定変更
            //if (MapProviders.SelectedItem != null)
            //{
            //    MapProviderList map = (MapProviderList)MapProviders.SelectedItem;
            //    // 地図名からMapProviderを探し、設定する。

            //    // 要素を列挙していく
            //    GMapProvider vvvvv = null;
            //    foreach (var item in GMapProviders.List)
            //    {
            //        Debug.WriteLine(item.Name);
            //        if (map.ProviderName.CompareTo(item.Name) == 0)
            //        {
            //            //                   gmap.MapProvider = item;
            //            SettingData.MapProvider = item;
            //            return;
            //        }
            //        if (item.GetType().ToString().CompareTo("GMap.NET.MapProviders.DllMapProvider") == 0)
            //        {
            //            vvvvv = item;
            //        }
            //    }

            //    GMapProviders.List.Remove(vvvvv);

            //    GMap.NET.MapProviders.DllMapProvider.Instance.setPluging(dictGmapPlugin[map.ProviderName]);
            //    SettingData.MapProvider = GMap.NET.MapProviders.DllMapProvider.Instance;
            //    SettingData.ProviderCopyright = map.License;
            //}

            if (_mode == 1)
            {
                //XDocument xml = XDocument.Load(System.Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\X-Monitor\Setting\Core\Setting.xml");
                //xml.Root.XPathSelectElement("FlightLog/FilePath").Value = FlightLogFilePath.Text;
                //xml.Root.XPathSelectElement("FlightLog/DivideSizeInByte").Value = DivideSizeInByte.Text;
                //xml.Root.XPathSelectElement("FlightLog/StoragePeriodInDay").Value = StoragePeriodInDay.Text;
                //xml.Root.XPathSelectElement("FlightLog/RecStopTimeFromDataStop").Value = RecStopTimeFromDataStop.Text;
                //xml.Save(System.Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\X-Monitor\Setting\Core\Setting.xml");	//Core部の設定ファイル保存

                //SettingData.FlightLogFilePath = FlightLogFilePath.Text;
                //SettingData.DivideSizeInByte = int.Parse(DivideSizeInByte.Text);
                //SettingData.StoragePeriodInDay = int.Parse(StoragePeriodInDay.Text);
                //SettingData.RecStopTimeFromDataStop = int.Parse(RecStopTimeFromDataStop.Text);
                SettingData.SendCommandFlag = true;
                //_logger.Info(FlightLogFilePath.Text);
            }
        }

        #endregion

        #region 画面設定タブ
        // 地図中心設定
        private void FixCoord_Checked(object sender, RoutedEventArgs e)
        {
            if (((RadioButton)sender).IsChecked == true)
            {
                SettingData.TrajectoryMode = 1;
            }
        }
        private void FollowCoord_Checked(object sender, RoutedEventArgs e)
        {
            if (((RadioButton)sender).IsChecked == true)
            {
                SettingData.TrajectoryMode = 2;
            }
        }
        private void WaypointCoord_Checked(object sender, RoutedEventArgs e)
        {
            if (((RadioButton)sender).IsChecked == true)
            {
                SettingData.TrajectoryMode = 3;
            }
        }
        private void HomeCoord_Checked(object sender, RoutedEventArgs e)
        {
            if (((RadioButton)sender).IsChecked == true)
            {
                SettingData.TrajectoryMode = 4;
            }
        }
        private void GPSCoord_Checked(object sender, RoutedEventArgs e)
        {
            if (((RadioButton)sender).IsChecked == true)
            {
                SettingData.TrajectoryMode = 5;
            }
        }
        private void TrajectoryMode_Checked(int mode)
        {
            switch (mode)
            {
                case 1:
                    //FixCoord.IsChecked = true;
                    //FollowCoord.IsChecked = false;
                    //WaypointCoord.IsChecked = false;
                    break;
                case 2:
                    //FollowCoord.IsChecked = true;
                    break;

                case 3:
                    //WaypointCoord.IsChecked = true;
                    break;

                case 4:
                    //HomeCoord.IsChecked = true;
                    break;

                case 5:
                    //GPSCoord.IsChecked = true;
                    break;

            }
        }


        //　UAV軌跡表示/非表示チェックボックス
        private void TrajectorySetCheck_Click(object sender, RoutedEventArgs e)
        {
            //if ((bool)TrajectorySetCheck.IsChecked)
            //{
            //    SettingData.TrajectoryFlag = true;
            //}
            //else
            //{
            //    SettingData.TrajectoryFlag = false;
            //}
        }
        private void SliderTrajectoryLength_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            //int val = (int)(SliderTrajectoryLength.Value * 10);
            //SliderTrajectoryLength.Value = (double)val / 10;
            //LabelTrajectoryLength.Content = SliderTrajectoryLength.Value.ToString() + "min";

            //SettingData.TrajectoryLength = SliderTrajectoryLength.Value;
        }
        #endregion

        #region 機能設定タブ
        #region バッテリ/通信監視設定入力
        // 通信
        private void LinkHealth_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                //HealthDataSet.AlertLinkValue = int.Parse(LinkHealth.Text);
            }
            catch
            {
                _logger.Error("LinkHealth_TextChanged");
            }
        }
        private void AlertRCrecHealth_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                //HealthDataSet.AlertRCrecValue = int.Parse(AlertRCrecHealth.Text);
            }
            catch
            {
                _logger.Error("AlertRCrecHealth_TextChanged");
            }
        }
        private void AlertLinkView_Checked(object sender, RoutedEventArgs e)
        {
            SettingData.AlertLinkViewFlag = true;
        }
        private void AlertLinkView_Unchecked(object sender, RoutedEventArgs e)
        {
            SettingData.AlertLinkViewFlag = false;
        }
        private void AlertLinkSound_Checked(object sender, RoutedEventArgs e)
        {
            SettingData.AlertLinkSoundFlag = true;
        }
        private void AlertLinkSound_Unchecked(object sender, RoutedEventArgs e)
        {
            SettingData.AlertLinkSoundFlag = false;
        }

        // バッテリ
        private void VoltageMin_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                //HealthDataSet.VolMinValue = double.Parse(VoltageMin.Text);
            }
            catch
            {
            }
        }
        private void VoltageMax_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                //	HealthDataSet.VolMaxValue = double.Parse(VoltageMax.Text);
            }
            catch
            {
                _logger.Error("VoltageMax_TextChanged");
            }
        }
        private void AlertVoltage_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                //	HealthDataSet.AlertVolValue = double.Parse(AlertVoltage.Text);
            }
            catch
            {
                _logger.Error("AlertVoltage_TextChanged");
            }
        }
        private void AlertVoltageView_Checked(object sender, RoutedEventArgs e)
        {
            SettingData.AlertVoltageViewFlag = true;
        }
        private void AlertVoltageView_Unchecked(object sender, RoutedEventArgs e)
        {
            SettingData.AlertVoltageViewFlag = false;
        }
        //private void AlertVoltageSound_Checked(object sender, RoutedEventArgs e)
        //{
        //	SettingData.AlertVoltageSoundFlag = true;
        //}
        //private void AlertVoltageSound_Unchecked(object sender, RoutedEventArgs e)
        //{

        //}
        private void AlertVoltageSound_Checked(object sender, RoutedEventArgs e)
        {
            SettingData.AlertVoltageSoundFlag = true;
        }

        private void AlertVoltageSound_Unchecked(object sender, RoutedEventArgs e)
        {
            SettingData.AlertVoltageSoundFlag = false;
        }

        // GPS
        private void GPSValue_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                //	HealthDataSet.AlertGPSValue = int.Parse(AlertGPS.Text);
            }
            catch
            {
                _logger.Error("GPSValue_TextChanged");
            }
        }
        private void AlertGPSView_Checked(object sender, RoutedEventArgs e)
        {
            SettingData.AlertGPSViewFlag = true;
        }
        private void AlertGPSView_Unchecked(object sender, RoutedEventArgs e)
        {
            SettingData.AlertGPSViewFlag = false;
        }
        private void AlertGPSSound_Checked(object sender, RoutedEventArgs e)
        {
            SettingData.AlertGPSSoundFlag = true;
        }
        private void AlertGPSSound_Unchecked(object sender, RoutedEventArgs e)
        {
            SettingData.AlertGPSSoundFlag = false;
        }
        #endregion
        #region ポート番号の設定
        private void PortComboBox_DropDown(object sender, EventArgs e)
        {
            // すべてのシリアル・ポート名を取得する
            string[] ports = SerialPort.GetPortNames();

            // シリアルポートを毎回取得して表示するために表示の度にリストをクリアする
            //		PortComboBox.Items.Clear();

            foreach (string port in ports)
            {
                // 取得したシリアル・ポート名を出力する
                //			PortComboBox.Items.Add(port);
            }
        }
        private void PortComboBox_DropDownClosed(object sender, EventArgs e)
        {
            //if (PortComboBox.Text != "")
            //{
            //    try
            //    {
            //        SettingData.SerialPortSetting = PortComboBox.Text;
            //        string _comName = PortComboBox.Text.Replace("COM", "");
            //        int _comNo = int.Parse(_comName);

            //        string _path = System.Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\X-Monitor\FlightLog\COM" + _comNo .ToString("00")+ @"\";

            //        // 確認ダイアログ
            //        if (System.Windows.Forms.MessageBox.Show("飛行ログの保存先を自動変更しますか?\n" + _path, "飛行ログ保存先変更確認画面", System.Windows.Forms.MessageBoxButtons.YesNo, System.Windows.Forms.MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.Yes)
            //        {
            //            //自動的に飛行ログフォルダパスを変更
            //            SettingData.FlightLogFilePath = _path;
            //            FlightLogFilePath.Text = _path;
            //            if (!System.IO.Directory.Exists(_path))
            //            {
            //                //string appDataPath = Environment.GetFolderPath(System.Environment.SpecialFolder.LocalApplicationData);
            //                //string path = System.IO.Path.Combine(appDataPath, PortComboBox.Text);
            //                //System.IO.Directory.CreateDirectory(path);
            //                System.IO.Directory.CreateDirectory(_path);
            //                System.Windows.MessageBox.Show(" OKボタンをクリックし、設定を保存します。\n X-Monitorを再起動して下さい。");

            //            }
            //        }

            //        SettingData.SendCommandFlag = true;
            //    }
            //    catch
            //    {

            //    }
            //}
        }
        /*
        private void ModeComboBox_DropDownClosed(object sender, EventArgs e)
        {
            //TODO:　TBOモード選択
            SettingData.ModeSetting = ModeComboBox.Text;
            ModeLabel.Content = SettingData.ModeSetting;
        }
        */
        #endregion
        #endregion

        #region 地図設定タブ
        // MapProvider指定
        private void SelectMapProviders()
        {
            // get map types
            var dicMapNameLicence = new Dictionary<String, String>();

            // GMapProversからの候補追加
            foreach (var item in GMapProviders.List)
            {
                #region Comment
                Debug.WriteLine(
                     string.Concat(
                         "MainWindow - Map Type : [",
                         item.Name,
                         "]"
                    )
                );
                #endregion

                if (item.GetType().FullName.ToString().CompareTo("GMap.NET.MapProviders.DllMapProvider") != 0)
                {
#if DEBUG
					dicMapNameLicence[item.Name] = item.Copyright;
#else
                    if ((GMapProviders.OpenStreetMap.ToString() == item.ToString()))
                    {
                        dicMapNameLicence[item.Name] = item.Copyright;
                    }
                    if (Properties.Settings.Default.Function2 == "MapProvider")
                    {
                        if ((GMapProviders.GoogleHybridMap.ToString() == item.ToString()))
                        {
                            dicMapNameLicence[item.Name] = item.Copyright;
                        }
                    }
#endif
                }
            }
            // GMapPluginからの候補追加
            // DLLを読み込む
            GMapProvPluginInfo pInfo = new GMapProvPluginInfo();
            String[] strArray = pInfo.FindPluginFileNames();
            foreach (var item in strArray)
            {
                GMapProvPluginInfo pluginInfo = new GMapProvPluginInfo(item);
                // DLLのインスタンス化
                GMapNetInterFace.IF_Plugin ifPlugin = pluginInfo.getDllInstance();

                Debug.WriteLine(
                     string.Concat(
                         "MainWindow - Map Type 2: [",
                         ifPlugin.strMapName,
                         "]"
                    )
                );
                dicMapNameLicence[ifPlugin.strMapName] = ifPlugin.strLicence;

                dictGmapPlugin.Add(ifPlugin.strMapName, item);
            }


            ObservableCollection<MapProviderList> list = new ObservableCollection<MapProviderList>();
            foreach (var MapNameLicence in dicMapNameLicence)
            {
                list.Add(new MapProviderList { ProviderName = MapNameLicence.Key, License = MapNameLicence.Value });
                System.Diagnostics.Debug.WriteLine("GMapProviders: " + MapNameLicence.Key + " >>> " + MapNameLicence.Value);
            }

            //MapProviders.ItemsSource = list;

        }
        // 地図プロバイダー選択
        void mySelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            //if (SettingData.MapProvider.ToString() != MapProviders.SelectedItem.ToString())
            {
                MapChange = true;
            }
        }

        //　経度入力イベント
        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                point.Lat = double.Parse(LatText.Text);
                SettingData.LatLngPoint = point;
            }
            catch
            {
                _logger.Error("TextBox_TextChanged");
            }

        }
        private void TextBox_TextChanged_1(object sender, TextChangedEventArgs e)
        {
            try
            {
                point.Lng = double.Parse(LngText.Text);
                SettingData.LatLngPoint = point;
            }
            catch
            {
                _logger.Error("TextBox_TextChanged_1");
            }

        }

        // Geocoding
        private void Button_Search(object sender, RoutedEventArgs e)
        {
            //if (GMapProviders.GoogleMap.ToString() == SettingData.MapProvider.ToString() || GMapProviders.GoogleHybridMap.ToString() == SettingData.MapProvider.ToString())
            //{
            try
            {
                // TODO:ジオコードの使用はGoogleのみに限られるため、使用に注意　フリーを検討
                //SearchAddressValue("http://maps.google.com/maps/api/geocode/xml?address=" + AddressName.Text + "&sensor=false");

                //string _text = AddressName.Text;
                //AddressName.Text = "Wait...";
                //Geocoding("http://www.geocoding.jp/api/?v=1.1&q=" + _text);
                //AddressName.Text = "[" + _text + "] Lat/ " + point.Lat.ToString() + "    Lng/ " + point.Lng.ToString();
            }
            catch (Exception ex)
            {
                //	AddressName.Text = "-Error-";
                System.Windows.MessageBox.Show("(1001) Geocoding Error:" + ex.Message);
                _logger.Error("(1001) Geocoding Error:" + ex.Message);
            }
            //}
            //else
            //{
            //	AddressName.Text = "Not Google";
            //}
        }
        // setボタン
        private void Button_Set(object sender, RoutedEventArgs e)
        {
            SettingData.LatLngPoint = point;
        }
        private void SearchAddressValue(string SearchAddress)
        {

            XmlTextReader reader = new XmlTextReader(SearchAddress);
            XmlDocument doc = new XmlDocument();
            doc.Load(reader);
            XmlNodeList lat = doc.SelectNodes("GeocodeResponse/result/geometry/location/lat");
            foreach (XmlNode xn in lat)
            {
                point.Lat = double.Parse(xn.InnerText);
            }
            XmlNodeList lng = doc.SelectNodes("GeocodeResponse/result/geometry/location/lng");
            foreach (XmlNode xn in lng)
            {
                point.Lng = double.Parse(xn.InnerText);
            }
        }
        private void Geocoding(string SearchAddress)
        {
            // http://www.geocoding.jp/api/
            // XML出力までに３～５秒かかりますので、５秒間に１回以上のアクセスは避けてください。
            XmlTextReader reader = new XmlTextReader(SearchAddress);
            XmlDocument doc = new XmlDocument();
            doc.Load(reader);
            XmlNodeList lat = doc.SelectNodes("result/coordinate/lat");
            foreach (XmlNode xn in lat)
            {
                point.Lat = double.Parse(xn.InnerText);
            }
            XmlNodeList lng = doc.SelectNodes("result/coordinate/lng");
            foreach (XmlNode xn in lng)
            {
                point.Lng = double.Parse(xn.InnerText);
            }
        }

        // タッチキーボード表示イベント
        private void Lat_GotKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
        {
            /*
            if (!SettingData.DeviceMode)
            {
                System.Diagnostics.Process.Start(@"C:\Program Files\Common Files\microsoft shared\ink\TabTip.exe");  
            }
           */
        }
        private void Lng_GotKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
        {
            /*
            if (!SettingData.DeviceMode)
            {
                System.Diagnostics.Process.Start(@"C:\Program Files\Common Files\microsoft shared\ink\TabTip.exe");
            } 
             * */
        }
        private void Add_GotKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
        {
            /*
            if (!SettingData.DeviceMode)
            {
                System.Diagnostics.Process.Start(@"C:\Program Files\Common Files\microsoft shared\ink\TabTip.exe");
            } 
             */
        }
        private void LatText_TouchDown(object sender, TouchEventArgs e)
        {
            if (!SettingData.DeviceMode)
            {
                System.Diagnostics.Process.Start(@"C:\Program Files\Common Files\microsoft shared\ink\TabTip.exe");
            }
        }

        private void LngText_TouchDown(object sender, TouchEventArgs e)
        {
            if (!SettingData.DeviceMode)
            {
                System.Diagnostics.Process.Start(@"C:\Program Files\Common Files\microsoft shared\ink\TabTip.exe");
            }
        }

        private void AddressName_TouchDown(object sender, TouchEventArgs e)
        {
            if (!SettingData.DeviceMode)
            {
                System.Diagnostics.Process.Start(@"C:\Program Files\Common Files\microsoft shared\ink\TabTip.exe");
            }
        }

        /// <summary>
        /// 高度(標高)取得
        /// 【引数】
        /// ElevationProvider　：HTTPリクエスト
        /// </summary>
        /// <remarks>
        /// Google仕様のXML取得となっている。
        /// </remarks>
        private double ElevationValue(string ElevationProvider)
        {
            try
            {
                if (SettingData.Online)
                {
                    // プロバイダー
                    // Google:http://maps.googleapis.com/maps/api/elevation/xml?locations=緯度,経度&sensor=false
                    // 国土地理院："http://cyberjapandata2.gsi.go.jp/general/dem/scripts/getelevation.php?lon=" + lon + "&lat=" + lat +"&outtype=JSON"
                    // OpenStreetMap 
                    // TODO:ライセンス確認
                    XmlTextReader reader = new XmlTextReader(ElevationProvider);
                    XmlDocument doc = new XmlDocument();
                    doc.Load(reader);
                    //((XmlDataProvider)Resources["Data"]).Document = doc;
                    XmlNodeList elevation = doc.SelectNodes("ElevationResponse/result/elevation");      //Googleの高度(標高)取得　XML仕様
                    foreach (XmlNode xn in elevation)
                    {
                        return double.Parse(xn.InnerText);
                    }
                    return (0);
                }
                else
                {
                    return (0);
                }

            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Elevation: " + ex.Message);
                _logger.Error("Elevation: " + ex.Message);
                return (0);
            }

        }
        private double GetElevationValue(string ElevationProvider)
        {
            // 国土地理院
            try
            {
                if (SettingData.Online)
                {
                    // URI で識別されるリソースとのデータの送受信用の共通クラス 
                    WebClient downloadClient = new WebClient();

                    // URL
                    Uri uriUrl = new Uri("http://cyberjapandata2.gsi.go.jp/general/dem/scripts/getelevation.php?lon=" + point.Lng.ToString() + "&lat=" + point.Lat.ToString() + "&outtype=JSON");
                    // ジオコーティング
                    string result = downloadClient.DownloadString(uriUrl);
                    // 取得結果
                    string[] results = result.Split(',');

                    if (results[0] != "" && results[0] != null)
                    {
                        string[] value = results[0].Split(':');
                        double d;
                        if (double.TryParse(value[1], out d))
                        {
                            double _val = double.Parse(value[1]);
                            return (_val);
                        }
                        return (0);
                    }

                    //// プロバイダー
                    //// Google:http://maps.googleapis.com/maps/api/elevation/xml?locations=緯度,経度&sensor=false
                    //// 国土地理院："http://cyberjapandata2.gsi.go.jp/general/dem/scripts/getelevation.php?lon=" + lon + "&lat=" + lat +"&outtype=JSON"
                    //// OpenStreetMap 
                    //// TODO:ライセンス確認
                    //XmlTextReader reader = new XmlTextReader(ElevationProvider);
                    //XmlDocument doc = new XmlDocument();
                    //doc.Load(reader);
                    ////((XmlDataProvider)Resources["Data"]).Document = doc;
                    //XmlNodeList elevation = doc.SelectNodes("ElevationResponse/result/elevation");      //Googleの高度(標高)取得　XML仕様
                    //foreach (XmlNode xn in elevation)
                    //{
                    //	return double.Parse(xn.InnerText);
                    //}
                    return (0);
                }
                else
                {
                    return (0);
                }

            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("(1002) Elevation Error: " + ex.Message);
                //ElevationName.Text = "-Error-";
                _logger.Error("(1002) Elevation Error: " + ex.Message);
                return (0);
            }

        }

        // 標高取得
        private void Elevation_Click(object sender, RoutedEventArgs e)
        {
            //if (GMapProviders.GoogleMap.ToString() == SettingData.MapProvider.ToString() || GMapProviders.GoogleHybridMap.ToString() == SettingData.MapProvider.ToString())
            //{
            //SettingData.Elevation = ElevationValue("http://maps.googleapis.com/maps/api/elevation/xml?locations=" + point.Lat.ToString() + "," + point.Lng.ToString() + "&sensor=false");

            //	string _text = ElevationName.Text;
            SettingData.Elevation = GetElevationValue("http://cyberjapandata2.gsi.go.jp/general/dem/scripts/getelevation.php?lon=" + point.Lng.ToString() + "&lat=" + point.Lat.ToString() + "&outtype=JSON");

            //	ElevationName.Text = SettingData.Elevation.ToString() + "m";
            //}
            //else
            //{
            //	ElevationName.Text = "Not Google";
            //}

        }

        // CustomMap参照ボタン
        private void CustomMapButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var Dlg = new System.Windows.Forms.FolderBrowserDialog();

                Dlg.Description = "選択";

                if (Dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    //CustomMapText.Text = Dlg.SelectedPath;

                    //	GMap.NET.MapProviders.DllMapProvider.Instance.SetTileImage(CustomMapText.Text);

                }
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("(402) FlightLog FileOpen Error: " + ex.Message);
                _logger.Error("(402) FlightLog FileOpen Error: " + ex.Message);
            }

        }
        #endregion

        #region オプション画面設定タブ
        // オプション画面表示/非表示
        private void ViwerSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                XmlElement mySelectedElement = (XmlElement)OptionViwer.SelectedItems[OptionViwer.SelectedItems.Count - 1];
                if (mySelectedElement.GetAttribute("No") != null)
                {
                    SettingData.WindowNo.Add(int.Parse((string)mySelectedElement.GetAttribute("No")));
                    System.Diagnostics.Debug.WriteLine(SettingData.WindowNo.Count + ") Window No" + SettingData.WindowNo[SettingData.WindowNo.Count - 1]);
                }
            }
            catch (Exception)
            {
                
            }

        }


        #endregion

        #region プロパティ
        /// <summary>
        /// ヘルスデータ
        /// </summary>
        public HealthData HealthDataSetting
        {
            get { return (HealthData)HealthDataSet; }
            set
            {
                HealthDataSet = value;
                //VoltageMin.Text = HealthDataSet.VolMinValue.ToString();				//電源MIN
                //VoltageMax.Text = HealthDataSet.VolMaxValue.ToString();				//電源MAX
                //AlertVoltage.Text = HealthDataSet.AlertVolValue.ToString();			//電源監視
                //AlertRCrecHealth.Text = HealthDataSet.AlertRCrecValue.ToString();	//RC監視
                //LinkHealth.Text = HealthDataSet.AlertLinkValue.ToString();			//Link監視
                //AlertGPS.Text = HealthDataSet.AlertGPSValue.ToString();				//GPS監視
            }
        }
        /// <summary>
        /// 設定データ
        /// </summary>
        public SetData SetDataSetting
        {
            get { return (SetData)SettingData; }
            set
            {
                SettingData = value;										//Setting更新
                SettingData.WindowNo.Clear();								//オプション画面番号クリア

                // 機能タブ
                ////		PortComboBox.Text = SettingData.SerialPortSetting;			//
                //		PortLabel.Content = SettingData.SerialPortSetting;
                //		IPaddress.Text = SettingData.CoreIPaddress;
                //		PortNo.Text = SettingData.CorePort.ToString();
                //ModeComboBox.Text = SettingData.ModeSetting;	//TODO:　TBOモード選択
                //ModeLabel.Content = SettingData.ModeSetting;	//TODO:　TBOモード選択
                point = SettingData.LatLngPoint;
                //AlertVoltageView.IsChecked = SettingData.AlertVoltageViewFlag;		//電源監視　警告表示
                //AlertVoltageSound.IsChecked = SettingData.AlertVoltageSoundFlag;	//電源監視　警告音
                //AlertLinkView.IsChecked = SettingData.AlertLinkViewFlag;			//通信監視　警告表示
                //AlertLinkSound.IsChecked = SettingData.AlertLinkSoundFlag;			//通信監視　警告音
                //AlertGPSView.IsChecked = SettingData.AlertGPSViewFlag;				//GPS　警告表示
                //AlertGPSSound.IsChecked = SettingData.AlertGPSSoundFlag;			//GPS　警告音

                //// 画面タブ
                //TrajectorySetCheck.IsChecked = SettingData.TrajectoryFlag;
                //TrajectoryMode_Checked(SettingData.TrajectoryMode);
                //SliderTrajectoryLength.Value = SettingData.TrajectoryLength;

                // 地図タブ
                LatText.Text = point.Lat.ToString();
                LngText.Text = point.Lng.ToString();

                //if (GMapProviders.GoogleMap.ToString() == SettingData.MapProvider.ToString() || GMapProviders.GoogleHybridMap.ToString() == SettingData.MapProvider.ToString())
                //{
                //	AddressName.IsEnabled = true;
                //	AddressName.Text = "千葉大";
                //	ElevationName.Text = SettingData.Elevation.ToString() + "m";
                //}
                //else
                //{
                //	AddressName.IsEnabled = false;
                //	AddressName.Text = "Not Google";
                //	ElevationName.Text = "Not Google";
                //}
                //AddressName.Text = "千葉大";
            }
        }

        public string FilePath
        {
            get { return (string)path; }
            set { path = value; }
        }

        public bool MapChange = false;
        #endregion

        /////////////////////////////////////////////////////////////////
        // 入力制限
        // Key 列挙体 http://msdn.microsoft.com/ja-jp/library/system.windows.input.key%28v=vs.110%29.aspx
        // 整数入力
        private void textBoxNum_KeyDown(object sender, KeyEventArgs e)
        {
            //0～9と、バックスペース以外の時は、イベントをキャンセルする

            var textBox = sender as TextBox;
            if (textBox == null) return;

            if ((Key.D0 <= e.Key && e.Key <= Key.D9) ||
                (Key.NumPad0 <= e.Key && e.Key <= Key.NumPad9) ||
                (Key.Delete == e.Key) || (Key.Back == e.Key) || (Key.Tab == e.Key))
            {
                e.Handled = false;
            }
            else
            {
                e.Handled = true;
            }
        }
        // 浮動小数入力
        private void textBoxNumDot_KeyDown(object sender, KeyEventArgs e)
        {
            //0～9と、バックスペース以外の時は、イベントをキャンセルする

            var textBox = sender as TextBox;
            if (textBox == null) return;

            if ((Key.Decimal == e.Key) || (Key.OemPeriod == e.Key))
            {
                if (textBox.Text.IndexOf(".") >= 0 || textBox.Text.Length == 0)
                {
                    e.Handled = true;
                }
            }
            else if ((Key.D0 <= e.Key && e.Key <= Key.D9) ||
                (Key.NumPad0 <= e.Key && e.Key <= Key.NumPad9) ||
                (Key.Delete == e.Key) || (Key.Back == e.Key) || (Key.Tab == e.Key))
            {
                e.Handled = false;
            }
            else
            {
                e.Handled = true;
            }
        }
        // 初期化
        private void Button_Click_Init(object sender, RoutedEventArgs e)
        {
            // 
            Properties.Settings.Default.Reset();

            // ウィンドウの値を Settings に格納
            //Properties.Settings.Default.MainWindow_Left = XMonitor_GUI.Properties.Settings.Default.MainWindow_Left;
            //Properties.Settings.Default.MainWindow_Top = XMonitor_GUI.Properties.Settings.Default.MainWindow_Top;
            //Properties.Settings.Default.MainWindow_Width = XMonitor_GUI.Properties.Settings.Default.MainWindow_Width;
            //Properties.Settings.Default.MainWindow_Height = XMonitor_GUI.Properties.Settings.Default.MainWindow_Height;
            //Properties.Settings.Default.Lat = XMonitor_GUI.Properties.Settings.Default.Lat;
            //Properties.Settings.Default.Lng = XMonitor_GUI.Properties.Settings.Default.Lng;

            HealthDataSet.AlertLinkValue = Properties.Settings.Default.AlertLink;
            HealthDataSet.AlertRCrecValue = Properties.Settings.Default.AlertRCrec;
            HealthDataSet.AlertVolValue = Properties.Settings.Default.AlertVoltage;
            HealthDataSet.VolMinValue = Properties.Settings.Default.VoltageMin;
            HealthDataSet.VolMaxValue = Properties.Settings.Default.VoltageMax;
            SettingData.AlertLinkSoundFlag = Properties.Settings.Default.AlertLinkSound;
            SettingData.AlertVoltageSoundFlag = Properties.Settings.Default.AlertVoltageSound;
            SettingData.AlertLinkViewFlag = Properties.Settings.Default.AlertLinkSound;
            SettingData.AlertVoltageViewFlag = Properties.Settings.Default.AlertVoltageSound;

            //VoltageMin.Text = HealthDataSet.VolMinValue.ToString();				//電源MIN
            //VoltageMax.Text = HealthDataSet.VolMaxValue.ToString();				//電源MAX
            //AlertVoltage.Text = HealthDataSet.AlertVolValue.ToString();			//電源監視
            //AlertRCrecHealth.Text = HealthDataSet.AlertRCrecValue.ToString();	//RC監視
            //LinkHealth.Text = HealthDataSet.AlertLinkValue.ToString();			//Link監視
            //AlertGPS.Text = HealthDataSet.AlertGPSValue.ToString();				//GPS監視

            //AlertLinkSound.IsChecked = (bool)SettingData.AlertLinkSoundFlag;
            //AlertVoltageSound.IsChecked = (bool)SettingData.AlertVoltageSoundFlag;
            //AlertLinkView.IsChecked = (bool)SettingData.AlertLinkViewFlag;
            //AlertVoltageView.IsChecked = (bool)SettingData.AlertVoltageViewFlag;
            //AlertGPSView.IsChecked = (bool)SettingData.AlertGPSViewFlag;
            //AlertGPSSound.IsChecked = (bool)SettingData.AlertGPSSoundFlag;
        }


        // アドレス情報の登録
        private void AddressRec_Click(object sender, RoutedEventArgs e)
        {
            AddressData _add = new AddressData()
            {
                Name = AddressText.Text,
                Lat = double.Parse(LatText.Text),
                Lng = double.Parse(LngText.Text)
            };
            AddressListView.Items.Add(_add);
            _addressRecord.Add(_add);
        }

        private List<AddressData> _addressRecord;
        /// <summary>
        /// アドレス情報
        /// </summary>
        public List<AddressData> AddressRecord
        {
            get { return _addressRecord; }
            set { _addressRecord = value; }
        }
        private bool _addressFlag;
        /// <summary>
        /// アドレス選択フラグ
        /// </summary>
        public bool AddressFlag
        {
            get { return _addressFlag; }
            set { _addressFlag = value; }
        }
        // アドレス情報の選択イベント
        private void AddressListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (_addressMode != -1)
            {
                AddressData _add = (AddressData)AddressListView.SelectedItem;
                PointLatLng _point = new PointLatLng(_add.Lat, _add.Lng);
                point = _point;
                _addressFlag = true;
                _addressMode = 0;
                SettingData.LatLngPoint = point;
            }
        }
        //登録情報を削除
        int _addressMode = 0;
        private void AddressDel_Click(object sender, RoutedEventArgs e)
        {
            int k = AddressListView.SelectedIndex;
            //AddressListView.SelectedIndex = 0;
            _addressMode = -1;
            AddressListView.Items.RemoveAt(k);
            _addressRecord.RemoveAt(k);
        }

        // Core部設定ファイル
        private void DivideSizeInByte_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                //	var val = double.Parse(DivideSizeInByte.Text);
                _mode = 1;
            }
            catch
            {
                _logger.Error("DivideSizeInByte_TextChanged");
            }
        }

        private void StoragePeriodInDay_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                //	var val = double.Parse(StoragePeriodInDay.Text);
                _mode = 1;
            }
            catch
            {
                _logger.Error("StoragePeriodInDay_TextChanged");
            }
        }

        private void RecStopTimeFromDataStop_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                //	var val = double.Parse(RecStopTimeFromDataStop.Text);
                _mode = 1;
            }
            catch
            {
                _logger.Error("RecStopTimeFromDataStop_TextChanged");
            }
        }

        // 参照ボタン
        private void Compare_Click(object sender, RoutedEventArgs e)
        {
            //FolderBrowserDialogクラスのインスタンスを作成
            System.Windows.Forms.FolderBrowserDialog fbd = new System.Windows.Forms.FolderBrowserDialog();


            fbd.Description = "フォルダを指定してください。";		//上部に表示する説明テキストを指定する
            //ルートフォルダを指定する
            //デフォルトでDesktop
            fbd.RootFolder = Environment.SpecialFolder.Desktop;
            //最初に選択するフォルダを指定する
            //RootFolder以下にあるフォルダである必要がある
            //	fbd.SelectedPath = FlightLogFilePath.Text;
            //ユーザーが新しいフォルダを作成できるようにする
            //デフォルトでTrue
            fbd.ShowNewFolderButton = true;

            //ダイアログを表示する
            if (fbd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                //選択されたフォルダを表示する
                //	FlightLogFilePath.Text = fbd.SelectedPath + @"\";
                _mode = 1;
            }
        }

        //　飛行ログの削除
        private void FlightLogDel_Click(object sender, RoutedEventArgs e)
        {
            // 確認ダイアログ
            if (System.Windows.Forms.MessageBox.Show("飛行ログを全て削除することに同意しますか?", "飛行ログ削除同意確認画面", System.Windows.Forms.MessageBoxButtons.YesNo, System.Windows.Forms.MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.Yes)
            {
                //foreach (string stFilePath in System.IO.Directory.GetFiles(FlightLogFilePath.Text))
                //{
                //    try
                //    {
                //        System.IO.File.Delete(stFilePath);
                //    }
                //    catch
                //    {
                //        _logger.Error("FlightLogDel　Error:" + stFilePath);
                //    }
                //}
            }
        }
        // 操作ログの削除
        private void ControlLogDel_Click(object sender, RoutedEventArgs e)
        {
            // 確認ダイアログ
            if (System.Windows.Forms.MessageBox.Show("操作ログを全て削除することに同意しますか?", "操作ログ削除同意確認画面", System.Windows.Forms.MessageBoxButtons.YesNo, System.Windows.Forms.MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.Yes)
            {
                string _path = System.Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\Logs\";

                foreach (string stFilePath in System.IO.Directory.GetFiles(_path))
                {
                    try
                    {
                        System.IO.File.Delete(stFilePath);
                    }
                    catch
                    {
                        _logger.Error("ControlLogDel　Error:" + stFilePath);
                    }
                }
            }

        }

        private void Maintain_Success_Btn_Click(object sender, RoutedEventArgs e)
        {
            //判断当前通讯是否正常不正常的话直接返回
            if (Constants.Connect_YesOrNot_Flag > 20)
            {
                //通讯正常的话直接用当前接收到的时间加上设置的时间并且保存在文件
                if (M_Text_Set.Text != "")
                {
                    Constants.User_Set_Maintaintimer = Convert.ToDouble(M_Text_Set.Text);
                }
                if (M_Text_Set.Text == "")//如果倒计时为空
                {
                    System.Windows.MessageBox.Show("时间设置栏不能为空");
                    return;
                }
                Constants.User_Set_MaintainStartToEndTime = Constants.User_Current_Timer + Constants.User_Set_Maintaintimer;
                StreamWriter sw = new StreamWriter("C:\\AutoWing\\Maintain_Timers.txt");
                sw.WriteLine(Constants.User_Set_Maintaintimer.ToString());//存设置的时间
                sw.WriteLine(Constants.User_Set_MaintainStartToEndTime.ToString());//存设置的时间加上当前飞控时间
                sw.Flush();
                sw.Close();

            }
            else
            {
                System.Windows.MessageBox.Show("请保持通信连接正常");
                return;
            }


        }

    }

    public class MapProviderList
    {
        public string ProviderName { get; set; }
        public string License { get; set; }
    }

}
