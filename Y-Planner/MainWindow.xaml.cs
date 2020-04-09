using System;
using System.Collections.Generic;
using System.Diagnostics;       //Process关联
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Xml;

// GMap.NET库
using GMap.NET;                         //GMap.NET Core
using GMap.NET.MapProviders;            //GMap.NET 地图提供商
using GMap.NET.WindowsPresentation;     //GMap.NET C# WPF用
using GMapNetInterFace;

// log4net
using log4net;
using log4net.Appender;
using log4net.Repository.Hierarchy;

using Y_Planner.CustomMarkers;      //在地图上显示标记

// CommonLib库
using SettingData;
using TouchControls;
using FlightPlanning;
using XmlBase;
using Pipe;

namespace Y_Planner
{
	/// <summary>
    /// 常量定义的类
	/// </summary>
	static class Constants
	{
		//public const double GPS_ELLIPSOID_A = 6378137.0;
		//public const double GPS_ELLIPSOID_F = 0.00335281066475;
		//public const double GPS_ELLIPSOID_B = (GPS_ELLIPSOID_A * (1.0 - GPS_ELLIPSOID_F));
        public const int _mapPosition = 1;			// 地图中心和无人机标记的距离差1公里
        public const int _maxMultiWaypoint = 1024;	//　最多的路点数
	}
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
	{
		#region 宣言
        bool EndFlag = true;                            // 线程循环标志

        GMapMarker UAVMarker;                           // 当前无人机标记
        SetData SettingData;                            // 配置管理类
        TouchControl MultiTouchControl;                 // 触摸操作类
		XmlManager DataConstellation;                   // XML数据管理
		Socket Client;                                  // socket通信
        PipeClient pClient;								// 命名管道

		int BaseSelect;									// 选择WaypointID
        List<int> WaypointSelect = new List<int>();     // 选择WaypointID
		Project ProjectCopy;							// 复制ID

		List<List<Project>> ProjectRecord;				// 记录所有操作的管理类
		List<List<Project>> ProjectSubRecord;			// 
        List<Project> ProjectList;                      // Mission/Waypoint/Path管理类
        int DrawMode;									// 0标准 1线 2矩形 3圈 4往返1 5往返2
		System.Windows.Point p1, p2;					//　
		int PointCount;									//
		FlightPlanning.Waypoint WaypointAllChange;
		
		// 地图
        Dictionary<String, String> dicMapNameLicence = new Dictionary<String, String>();

        // 保存目标文件
        string FilePath = string.Empty;

        // 时间
        System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
		// 排他処理
		object obj = new object();
		// 登陆
		private static readonly ILog _logger = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

		bool WaitViewFlag = false;
		#endregion

		#region MainWindow
		// 主界面
        public MainWindow()
		{
#if DEBUG
			/*_logger.Debug("[TEST]開発中のデバッグ／トレースに使用する");
			_logger.Info("[TEST]情報（操作履歴等）");
			_logger.Warn("[TEST]注意／警告（障害の一歩手前）");
			_logger.Error("[TEST]システムが停止するまではいかない障害が発生");
			_logger.Fatal("[TEST]システムが停止する致命的な障害が発生");*/


            _logger.Debug（“[测试]我曾经在开发调试/跟踪”）;
             _logger.Info（“[测试]信息（操作历史等）”）;
             _logger.Warn（“[测试]注意/警告故障（边缘）”）;
             _logger.Error（“[测试]发生系统故障，不走，直到你停止”）;
             _logger.Fatal（“[测试]灾难性故障发生系统停止”）;
#endif
            try
			{
				string appPath = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
				Debug.WriteLine("appPath: " + appPath);

				_logger.Info("[MainWindow]" + appPath);

                //改变当前目录
				System.Environment.CurrentDirectory = appPath;

				// MainWindow初始化
				InitializeComponent();

				// 初始设置 
				Init();

				// 线程循环初始化
				WorkThread();
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(101) Initialization Error: " + ex.Message);
				_logger.Error("(101) Initialization Error: " + ex.Message);
				Close();
			}
        }
        // 关闭主界面
		private void MainWindowCosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            
            try
            {
                // 退出确认对话框
                if (System.Windows.Forms.MessageBox.Show("你确定要退出Y-Planner？", "退出Y-Planner主界面", System.Windows.Forms.MessageBoxButtons.YesNo, System.Windows.Forms.MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.No)
                {
                    e.Cancel = true;
                    EndFlag = true;
                }
                else
                {
					_logger.Info("MainWindowCosing");
					if (ProjectList.Count > 0)
					{
						if (ProjectList[BaseSelect].WaypointData.Count > 0)
						{
                            if (System.Windows.Forms.MessageBox.Show("确定保存飞行计划？", "飞行计划保存确认画面", System.Windows.Forms.MessageBoxButtons.YesNo, System.Windows.Forms.MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.Yes)
							{
								SaveDocument();

							}
						}
					}

                    //停止秒表计时
                    sw.Stop();

					// 清除
					MarkerAllClear();

                    // 退出线程循环
                    EndFlag = false;


                    Properties.Settings.Default.Lat = MainMap.Position.Lat;
                    Properties.Settings.Default.Lng = MainMap.Position.Lng;
					Properties.Settings.Default.Provider = MapProviders.SelectedItem.ToString();
					// 文件保存 
					Properties.Settings.Default.Save();
					
                }
            }
            catch (Exception ex)
            {
				System.Windows.MessageBox.Show("(001) Close Error: " + ex.Message);
				_logger.Error("(001) Close Error: " + ex.Message);
            }
            
        }
       #endregion

        #region 初始化设置
		// 初始化
        private void Init()
        {
            SettingData = new SetData();        // 配置管理类

            #region 平板电脑设置
            // ================================================================================
            //PC检测和设置
            SettingData.DeviceMode = true;              // true:Mouse   false:Touch
            foreach (TabletDevice tabletDevice in Tablet.TabletDevices)
            {
                //触摸功能检测
                if (tabletDevice.Type == TabletDeviceType.Touch)
                {
                    SettingData.DeviceMode = false;     // true:Mouse   false:Touch
                }
            }
            MainMap.TouchEnabled = false;               // 禁用触摸操作当鼠标操作（停止地图移动功能的触摸笔）
            ChangeTouchMouse(SettingData.DeviceMode, true);   // 触摸鼠标操作切换

            // ================================================================================
            // 多点触控操作 - 初始化设置
            System.Windows.Point initp = new System.Windows.Point();
            MultiTouchControl = new TouchControl(0, 0, 0, initp, initp, initp, 0, 0, 0, 0, false);

            #endregion

            #region 初始化设置
            // ================================================================================
            // 背景设置
            Resources["WindowBackgroundBrush"] = new SolidColorBrush(Color.FromRgb(31, 26, 80));

			// ================================================================================
			// 初始化设置
			SettingData.Set = true;
            SettingData.TrajectoryMode = 1;             //1地图的中心坐标 2机身中心坐标 3航点中心坐标
			SettingData.TrajectoryFlag = false;
			SettingData.LatLngPoint = MainMap.Position;
            #endregion

            #region GMap.NET初始化设置
            // ================================================================================
            // GMap.NET初始化设置
            MainMap.Manager.Mode = AccessMode.ServerAndCache;
            GMapProvider.WebProxy = System.Net.WebRequest.GetSystemWebProxy();
            GMapProvider.WebProxy.Credentials = System.Net.CredentialCache.DefaultCredentials;
            MainMap.MapProvider = GMapProviders.OviSatelliteMap;                                                      // 地図プロバイダー
            Copyright.Content = GMapProviders.OpenStreetMap.Copyright;                                              // 地図プロバイダーCopyright
			if (Properties.Settings.Default.Lat != 0 && Properties.Settings.Default.Lng != 0)
			{
				MainMap.Position = new PointLatLng(Properties.Settings.Default.Lat, Properties.Settings.Default.Lng);               // 地図表示　経度緯度
			}
			else
			{
                MainMap.Position = new PointLatLng(32.0103691272, 118.7828039246);	//千叶大
			}
            SelectMapProviders();

		//	MapProviders.SelectedItem = Properties.Settings.Default.Provider;
            #endregion

            #region 相关参数
            // ================================================================================
            // 相关参数

            // 项目初始化
            ProjectList = new List<Project>();

            //项目运行记录初始化
			ProjectRecord = new List<List<Project>>();
			ProjectSubRecord = new List<List<Project>>();
			//ProjectRecord.Add(ProjectList);

            //MainMap.Markers.CollectionChanged += new NotifyCollectionChangedEventHandler(Marker_CollectionChanged);

            // 经度纬度表示
            LatText.Text = MainMap.Position.Lat.ToString();
            LngText.Text = MainMap.Position.Lng.ToString();

            //　通信初始化
			string ad = Properties.Settings.Default.CoreIPaddress;
			int port = Properties.Settings.Default.CorePort;

			Debug.WriteLine("IPAddress:" + ad + "   Port:" + port);

			IPEndPoint remoteEP = new IPEndPoint(IPAddress.Parse(ad), port);
			Client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

			// ================================================================================
			// 
			DataConstellation = new XmlManager();	// 初始化
			DataConstellation.Initialize();			// 初始化设置

			VersionName.Text = Properties.Settings.Default.YPlannerVersion;

			// WaypointListView初始化
			WaypointAllChange = new FlightPlanning.Waypoint()
			{
				ID = 0,
				Name = "ALL",
				Latitude = 0,
				Longitude = 0,
				x = 0,
				y = 0,
				Height = 0,
				Heading = 0,
				ThreasholdH = 2,
				ThreasholdV = 2,
				Wait = 0,
				Function = 0,
				Speed = 2
			};
			WaypointAllChange_DataGrid.Items.Add(WaypointAllChange);
			this.WaypointListView.Items.Clear();
            #endregion

            #region 绘图相关
            DrawMode = PointCount = 0;

			this.Radio_StandardDraw.Checked += new System.Windows.RoutedEventHandler(this.AutoWaypointDraw_Checked);
			this.Radio_LineDraw.Checked += new System.Windows.RoutedEventHandler(this.AutoWaypointDraw_Checked);
			this.Radio_RectangleDraw.Checked += new System.Windows.RoutedEventHandler(this.AutoWaypointDraw_Checked);
			this.Radio_CircleDraw.Checked += new System.Windows.RoutedEventHandler(this.AutoWaypointDraw_Checked);
			this.Radio_ReturnDraw1.Checked += new System.Windows.RoutedEventHandler(this.AutoWaypointDraw_Checked);
			this.Radio_ReturnDraw2.Checked += new System.Windows.RoutedEventHandler(this.AutoWaypointDraw_Checked);

			#endregion

			// 生成的客户端类
			pClient = new PipeClient();

            // 登陆方法和事件过程
			pClient.ReceiveCallEvent += new PipeClient.ReceiveCallDelegate(PipeClient_ReceiveCall);

			MissionDoc.IsVisible = false;

			// Waypoint标记
			this.MainMap.MouseDoubleClick += new System.Windows.Input.MouseButtonEventHandler(this.Uav_WayPoint);
			this.MainMap.MouseRightButtonDown += new System.Windows.Input.MouseButtonEventHandler(this.MarkerControl_MouseRightButtonDown);
			this.MainMap.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(MainMap_MouseLeftButtonDown);

			// 地图操作
			this.MainMap.MouseDown += new System.Windows.Input.MouseButtonEventHandler(this.MainMap_MouseDown);
			this.MainMap.MouseMove += new System.Windows.Input.MouseEventHandler(this.MainMap_MouseMove);
			this.MainMap.MouseUp += new System.Windows.Input.MouseButtonEventHandler(this.MainMap_MouseUp);
			this.MainMap.TouchDown += new EventHandler<TouchEventArgs>(this.MainMap_TouchDown);
			this.MainMap.TouchUp += new EventHandler<TouchEventArgs>(this.MainMap_TouchUp);
			this.MainMap.TouchMove += new EventHandler<TouchEventArgs>(this.MainMap_TouchMove);

			this.Main_Window.TouchUp += new EventHandler<TouchEventArgs>(this.Main_TouchUp);
        }
        // 地图提供者列表获取 即为可选择的地图列表
		private void SelectMapProviders()
        {
            // get map types
			//var dicMapNameLicence = new Dictionary<String, String>();

            MapProviders.Items.Add(GMapProviders.OpenStreetMap.ToString());

			// GMapProvers备用项
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
							MapProviders.Items.Add(GMapProviders.GoogleHybridMap.ToString());
						}
					}
#endif
				}
			}
            // 候选另外的的GMapPlugin
			// DLL加载
			GMapProvPluginInfo pInfo = new GMapProvPluginInfo();
			String[] strArray = pInfo.FindPluginFileNames();
			foreach (var item in strArray)
			{
				GMapProvPluginInfo pluginInfo = new GMapProvPluginInfo(item);
                // DLL的实例化
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

                MapProviders.Items.Add(ifPlugin.strMapName);
			}
            MapProviders.SelectedItem = MainMap.MapProvider.ToString();
            Copyright.Content = MainMap.MapProvider.Copyright;
        }
        #endregion

        #region 线
#if DEBUG
		Stopwatch psw = new Stopwatch();	//処理速度計測用１
		Stopwatch psw2 = new Stopwatch();	//処理速度計測用２
#endif
        private void WorkThread()
		{
			// 开始
			sw.Start();

            #region 通信线程
            var task = Task.Factory.StartNew(() =>
			{

				try
				{
                    long nextFrame = sw.ElapsedMilliseconds;		//经历的秒数（毫秒）
					const long period = 20;	//周期(msec)

					while (EndFlag)
					{
						long tickCount = sw.ElapsedMilliseconds;	// 获取现在的时刻

						// 将会忽略的处理
						if (tickCount < nextFrame)
						{
							if (nextFrame - tickCount > 1)
							{

								System.Threading.Thread.Sleep((int)(nextFrame - tickCount));	// Wait
							}
							//Debug.WriteLine("Skip：" + (nextFrame - tickCount));
							DoEvents();		// 需要处理的Windows消息
							continue;
						}
						Dispatcher.BeginInvoke((Action)(() =>
						{
							// 接收到的数据更新
							lock (obj)
							{
								if (Client.Connected)
								{
									ReceiveDataUpdate();
									string str = DataConstellation.XmlSelect();
								}
							}

						}));

						nextFrame += period;
					}

                    // 退出核心部分（关闭消息），如果（Client.Connected）
					if (Client.Connected)
					{
						string text = DataConstellation.CloseMassage(XmlMessage.Get_UAVData);
						Send(Client, text);
						sendDone.WaitOne();
                        // 退出核心部分（关闭消息），如果（Client.Connected）

						// Release the socket.
						Client.Shutdown(SocketShutdown.Both);
						Client.Close();
					}
				}
				catch (Exception ex)
				{
					System.Windows.MessageBox.Show("(301) Massage Receive Error: " + ex.Message);
					_logger.Error("(301) Massage Receive Error: " + ex.Message);
				}

			});

			#endregion

            #region 绘图更新线程
            var view = Task.Factory.StartNew(() =>
			{
				try
				{
					long nextFrame = sw.ElapsedMilliseconds;		//経過秒(msec)
					const long period = 100;	//周期(msec)

					while (EndFlag)
					{
						long tickCount = sw.ElapsedMilliseconds;	// 当前时间的获取

                        // 将会忽略的处理
						if (tickCount < nextFrame)
						{
							if (nextFrame - tickCount > 1)
							{

								System.Threading.Thread.Sleep((int)(nextFrame - tickCount));	// Wait
							}
                            DoEvents();		// 要处理的Windows消息
							continue;
						}
						if ((double)sw.ElapsedMilliseconds >= nextFrame + period)
						{
							nextFrame += period;
							continue;
						}
						// 异步处理
						Dispatcher.BeginInvoke((Action)(() =>
                        {
                            #region 绘图处理（数据更新）
                            if (Client.Connected)
							{
								bool ResultUpdate = false;
								// 检测接收到的数据是否正常
								if (DataConstellation.XmlDataCheck())
								{
									ResultUpdate = MainDataUpdate();

									if (ResultUpdate)
									{
										MapUpdate(ResultUpdate);
									}
								}
							}
							SubDataUpdate();
							#endregion
						}));
						nextFrame += period;
					}
				}
				catch (Exception ex)
				{
					System.Windows.MessageBox.Show("(303) Update Error: " + ex.Message);
					_logger.Error("(303) Update Error: " + ex.Message);
				}
			});

			#endregion

            #region Sub更新线程
            Task.Factory.StartNew(() =>
			{
				try
				{
                    long nextFrame = sw.ElapsedMilliseconds;		//经历的秒数（毫秒）
					const long period = 100;	//周期(msec)

					while (EndFlag)
					{
						long tickCount = sw.ElapsedMilliseconds;	// 当前时间获取

						// 将会忽略的流程
						if (tickCount < nextFrame)
						{
							if (nextFrame - tickCount > 1)
							{

								System.Threading.Thread.Sleep((int)(nextFrame - tickCount));	// Wait
							}
							DoEvents();		// 要处理的Windows消息
							continue;
						}
						if ((double)sw.ElapsedMilliseconds >= nextFrame + period)
						{
							nextFrame += period;
							continue;
						}
						//异步处理
						Dispatcher.BeginInvoke((Action)(() =>
						{
							SubDataUpdate();
						}));
						nextFrame += period;
					}
				}
				catch (Exception ex)
				{
					System.Windows.MessageBox.Show("(303-1) Update Error: " + ex.Message);
					_logger.Error("(303-1) Update Error: " + ex.Message);
				}
			});

			#endregion
		}

		//
		public void DoEvents()
		{
			DispatcherFrame frame = new DispatcherFrame();
			Dispatcher.CurrentDispatcher.BeginInvoke(DispatcherPriority.Background,
				new DispatcherOperationCallback(ExitFrames), frame);
			Dispatcher.PushFrame(frame);
		}
		public object ExitFrames(object f)
		{
			((DispatcherFrame)f).Continue = false;

			return null;
		}
		#endregion

        #region 数据更新
        // 从核心部分接收到的数据的更新
		private bool MainDataUpdate()
		{
			try
			{
				// XML语言转换  c:检测输的的確認結果
				int c = DataConstellation.XmlConvert();

				if (c == -1)
				{
					return false;
				}

				// TEST用
				//DataConstellation.BindingData.Health.MainLink_UavToGcs = 15;

				// ================================================================================
				//　現在位置
				#region 现在位置
				UAVMarker.Position = new PointLatLng(DataConstellation.BindingData.UAVpoint.Lat, DataConstellation.BindingData.UAVpoint.Lon);
				UAVMarker.Altitude = AltitudePropertyValue.UAVAltitude = DataConstellation.BindingData.UAVpoint.Alt;
				UAVMarker.Azimuth = DataConstellation.BindingData.UAVpoint.Yaw;

				UAVLatText.Text = DataConstellation.BindingData.UAVpoint.Lat.ToString();
				UAVLngText.Text = DataConstellation.BindingData.UAVpoint.Lon.ToString();
				UAVAltText.Text = DataConstellation.BindingData.UAVpoint.Alt.ToString();
				UAVYawText.Text = DataConstellation.BindingData.UAVpoint.Yaw.ToString();

				AltitudePropertyValue.HomeAltitude = DataConstellation.BindingData.Homepoint.Alt;
				#endregion

				return true;
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(501) MainData Update Error: " + ex.Message);
				_logger.Error("(501) MainData Update Error: " + ex.Message);
				return false;
			}
		}
		private void MapUpdate(bool ResultUpdate)
		{
			try
			{
				if (SettingData.Set)
				{
					switch (SettingData.TrajectoryMode)
					{
						case 1:
							var _dis = Math.Round(
                                    MainMap.MapProvider.Projection.GetDistance(
                                    MainMap.Position,     // 1Pont
                                    UAVMarker.Position    // 2Point
                                    ), 4, MidpointRounding.AwayFromZero);
							if (_dis > Constants._mapPosition)
							{
								MainMap.Position = UAVMarker.Position;
							}
                            
							break;
						case 2:
							MainMap.Position = UAVMarker.Position;
							break;
						case 3:
							//if(WaypointCount > 0){
							//	MainMap.Position = WaypointMarker.Position;
							//}
							//MainMap.Position = NWaypointMarker.Position;
							break;
					}
				}
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(504) MapPostion Error: " + ex.Message);
				_logger.Error("(504) MapPostion Error: " + ex.Message);
			}
		}
		private void SubDataUpdate()
		{
			if (WaitViewFlag)
			{
				if (WaitWindow.Visibility != Visibility.Visible)
				{
					WaitWindow.Visibility = Visibility.Visible;
				}
                // 命令来请求到服务器（命令）
				string _message = "test";
				string _path = "";
				_path = System.Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
				_path += @"\FlightPlanning_" + DateTime.Now.Year.ToString("0000")
											+ DateTime.Now.Month.ToString("00")
											+ DateTime.Now.Day.ToString("00")
											+ "_" + DateTime.Now.Hour.ToString("00")
											+ DateTime.Now.Minute.ToString("00")
											+ ".wpp";   // Default file name

				_message = FlightPlanning.FlightPlanXML.WppWrite(_path, ProjectList);

                // 运行该服务器连接处理
				WaitViewFlag = false;
				pClient.Connect(_message);
			}
			else
			{
				if (WaitWindow.Visibility != Visibility.Hidden)
				{
					WaitWindow.Visibility = Visibility.Hidden;
				}
			}
		}
		#endregion

        #region 功能
        // “×”按钮：主画面的关闭
        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
        // 主界面最大化
        private void MaxWindowButton_Click(object sender, RoutedEventArgs e)
        {
            this.WindowState = WindowState.Maximized;
        }
        // 主界面最小化
        private void MinWindowButton_Click(object sender, RoutedEventArgs e)
        {
            this.WindowState = WindowState.Minimized;
        }
        //返回到主屏幕的原始尺寸
        private void DefaultWindowButton_Click(object sender, RoutedEventArgs e)
        {
            this.WindowState = WindowState.Normal;
        }
        #endregion

        #region 菜单
        // 新建
        private void NewFile_Click(object sender, RoutedEventArgs e)
        {
            FilePath = "";
            MarkerAllClear();
			PointCount = 0;
        }
        // 打开显示
        private void FileOpen_Click(object sender, RoutedEventArgs e)
        {
			MarkerAllClear();
            MultiWaypointFile();
        }
        // 关闭显示
        private void FileClose_Click(object sender, RoutedEventArgs e)
        {
            MarkerAllClear();
			PointCount = 0;
        }
        // 另存为显示
        private void Save_Click(object sender, RoutedEventArgs e)
        {
            SaveDocument();
        }
        // 保存画面的显示
        private void OverWrite_Click(object sender, RoutedEventArgs e)
        {
            SaveDocument(FilePath);
        }
        //  显示手动
        private void Manual_Click(object sender, RoutedEventArgs e)
        {
			//运行启动其他程序
			Process.Start(System.IO.Path.GetFullPath(@"Info\Y-Planner取扱説明書.chm"));
        }
        // 「About界面显示
        private void About_Click(object sender, RoutedEventArgs e)
        {
            AboutWindow AWindow = new AboutWindow();
            AWindow.Background = this.Background;
			AWindow.YPlannerVersion = Properties.Settings.Default.YPlannerVersion;
			AWindow.APFCVersion = Properties.Settings.Default.APFCVersion;
            AWindow.Show();
        }
        // 关闭 
        private void MenuItem_Click_End(object sender, RoutedEventArgs e)
        {
            Close();
        }
        #endregion

        #region 构建网格模式（不能编辑）
        /// <summary>
        /// 构建网格图案
        /// </summary>
        private const int GRID_SIZE = 50;   //グリッドサイズ
        private ScaleTransform scaleTransform = new ScaleTransform();
        private void canvas_Loaded(object sender, RoutedEventArgs e)
        {
			if (MainMap.MapProvider.ToString() == "国土地理院地图(航空)")
			{
				BuildView(Brushes.White);
			}
			else
			{
				BuildView(new SolidColorBrush(Color.FromRgb(31, 26, 80)));
			}
        }
        private void canvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
			if (MainMap.MapProvider.ToString() == "国土地理院地图(航空)")
			{
				BuildView(Brushes.White);
			}
			else
			{
				BuildView(new SolidColorBrush(Color.FromRgb(31, 26, 80)));
			}
        }
		private void BuildView(System.Windows.Media.Brush _brushe)
        {
            canvas.Children.Clear();
            scaleTransform.ScaleX = scaleTransform.ScaleY = 1;  //グッドスケール

            // 縦線
			//for (int i = 0; i < canvas.ActualWidth; i += GRID_SIZE)
			for (int i = (int)(canvas.ActualWidth - GRID_SIZE); i > 0; i -= GRID_SIZE)
            {

                System.Windows.Shapes.Path path = new System.Windows.Shapes.Path()
                {
					Data = new LineGeometry(new System.Windows.Point(i, 0), new System.Windows.Point(i, canvas.ActualHeight)),
					Stroke = _brushe,
                    StrokeThickness = 0.25
                };

                path.Data.Transform = scaleTransform;

                canvas.Children.Add(path);
            }

            // 横線
            for (int i = 0; i < canvas.ActualHeight; i += GRID_SIZE)
            {
                System.Windows.Shapes.Path path = new System.Windows.Shapes.Path()
                {
					Data = new LineGeometry(new System.Windows.Point(0, i), new System.Windows.Point(canvas.ActualWidth, i)),
					Stroke = _brushe,
                    StrokeThickness = 0.25
                };

                path.Data.Transform = scaleTransform;

                canvas.Children.Add(path);
            }
        }
        #endregion

        #region 主屏操作相关
        // 地图旋转按钮
        private void RotateAzimuthalValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            MainMap.RenderTransform = new RotateTransform { Angle = MapRotate.Angle };
        }
        // 地图旋转向北（0度）按钮
        private void MapRotate_MouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            MapRotate.Angle = 0;
            MainMap.RenderTransform = new RotateTransform { Angle = MapRotate.Angle };
        }
        // 相应的地图缩放事件网格线距离显示
        private void MapZoomChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            try
            {
                GPoint p = MainMap.FromLatLngToLocal(MainMap.Position);
                PointLatLng Point = MainMap.FromLocalToLatLng((int)p.X + GRID_SIZE, (int)p.Y);
                double dis = Math.Round(MainMap.MapProvider.Projection.GetDistance(MainMap.Position, Point), 4, MidpointRounding.AwayFromZero);
				if (dis < 1)
				{
					dis *= 1000;// km -> m
					DisName.Content = dis.ToString() + " m";
				}
				else
				{
					DisName.Content = dis.ToString() + "km";
				}
                System.Diagnostics.Debug.WriteLine("Dis: " + dis.ToString());
            }
            catch
            {
				_logger.Error("MapZoomChanged");
            }
        }
        // 窗口屏幕尺寸的变化事件（画面切换）
        private void Window_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            /*
            double r = e.NewSize.Width / e.NewSize.Height;
            //PlayerSlide.Width = GridLine.Width;
            SlideSizeChanged();

            //　长宽比
            if (r < 1)
            {
                // 垂直显示
                GridLine.SetValue(Grid.RowProperty, 1);
                GridLine.SetValue(Grid.ColumnProperty, 1);
                GridLine.SetValue(Grid.RowSpanProperty, 1);
                GridLine.SetValue(Grid.ColumnSpanProperty, 3);

                RightsideArea.SetValue(Grid.RowProperty, 3);
                RightsideArea.SetValue(Grid.ColumnProperty, 1);
                RightsideArea.SetValue(Grid.RowSpanProperty, 1);
                RightsideArea.SetValue(Grid.ColumnSpanProperty, 3);

                Container.SetValue(Grid.RowProperty, 3);
                Container.SetValue(Grid.ColumnProperty, 1);
                Container.SetValue(Grid.RowSpanProperty, 1);
                Container.SetValue(Grid.ColumnSpanProperty, 3);

                AlertMessageName.SetValue(Grid.RowProperty, 1);
                AlertMessageName.SetValue(Grid.ColumnProperty, 1);
                AlertMessageName.SetValue(Grid.RowSpanProperty, 1);
                AlertMessageName.SetValue(Grid.ColumnSpanProperty, 3);

                OptionGridSplitter.SetValue(Grid.RowProperty, 2);
                OptionGridSplitter.SetValue(Grid.ColumnProperty, 1);
                OptionGridSplitter.SetValue(Grid.RowSpanProperty, 1);
                OptionGridSplitter.SetValue(Grid.ColumnSpanProperty, 3);
                OptionGridSplitter.ResizeDirection = GridResizeDirection.Rows;

                PlayerSlide.SetValue(Grid.RowProperty, 4);
                PlayerSlide.SetValue(Grid.ColumnProperty, 1);
                PlayerSlide.SetValue(Grid.RowSpanProperty, 1);
                PlayerSlide.SetValue(Grid.ColumnSpanProperty, 3);
                PlayerSlide.VerticalAlignment = VerticalAlignment.Top;

                BaseColumn.Width = new GridLength(250);
                BaseRow.Height = new GridLength(250);
                BaseHeight.Height = new GridLength(160);

                WindowSwitch = true;

                if (_WaypointWindowID != 0)
                {
                    Container.Children[_WaypointWindowID - 1].Width = 420;
                    Container.Children[_WaypointWindowID - 1].Height = 250;
                }
            }
            else
            {
                // 横表示
                GridLine.SetValue(Grid.RowProperty, 1);
                GridLine.SetValue(Grid.ColumnProperty, 1);
                GridLine.SetValue(Grid.RowSpanProperty, 3);
                GridLine.SetValue(Grid.ColumnSpanProperty, 1);

                RightsideArea.SetValue(Grid.RowProperty, 0);
                RightsideArea.SetValue(Grid.ColumnProperty, 3);
                RightsideArea.SetValue(Grid.RowSpanProperty, 5);
                RightsideArea.SetValue(Grid.ColumnSpanProperty, 1);

                Container.SetValue(Grid.RowProperty, 1);
                Container.SetValue(Grid.ColumnProperty, 3);
                Container.SetValue(Grid.RowSpanProperty, 3);
                Container.SetValue(Grid.ColumnSpanProperty, 1);

                AlertMessageName.SetValue(Grid.RowProperty, 1);
                AlertMessageName.SetValue(Grid.ColumnProperty, 1);
                AlertMessageName.SetValue(Grid.RowSpanProperty, 1);
                AlertMessageName.SetValue(Grid.ColumnSpanProperty, 1);

                OptionGridSplitter.SetValue(Grid.RowProperty, 1);
                OptionGridSplitter.SetValue(Grid.ColumnProperty, 2);
                OptionGridSplitter.SetValue(Grid.RowSpanProperty, 3);
                OptionGridSplitter.SetValue(Grid.ColumnSpanProperty, 1);
                OptionGridSplitter.ResizeDirection = GridResizeDirection.Columns;

                PlayerSlide.SetValue(Grid.RowProperty, 3);
                PlayerSlide.SetValue(Grid.ColumnProperty, 1);
                PlayerSlide.SetValue(Grid.RowSpanProperty, 1);
                PlayerSlide.SetValue(Grid.ColumnSpanProperty, 1);
                PlayerSlide.VerticalAlignment = VerticalAlignment.Bottom;

                BaseColumn.Width = new GridLength(250);
                BaseRow.Height = new GridLength(250);
                BaseHeight.Height = new GridLength(100);

                WindowSwitch = false;

                if (_WaypointWindowID != 0)
                {
                    Container.Children[_WaypointWindowID - 1].Width = 250;
                    Container.Children[_WaypointWindowID - 1].Height = 420;
                }

            }

            // 地图更新（当你改变窗口大小，以避免触摸飞在地图上移动的现象）
            if (!SettingData.DeviceMode)
            {
                MainMap.Zoom += 1;
                System.Threading.Thread.Sleep(5);
                MainMap.Zoom -= 1;
            }
             */
        }
		// メイン画面のキーボードイベント（押した時）
		private void Window_PreviewKeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            #region 功能操作相关
            if (e.Key == Key.LeftCtrl || e.Key == Key.RightCtrl)
			{
				foreach (var _no in WaypointSelect)
				{
					ProjectList[BaseSelect].WaypointMarker[_no - 1].Mode = 1;
				}
			}
			//string _text = KeyEventArgsToString("StackPanel_KeyDown", e);
			//Debug.WriteLine("key" + _text);

			////////////////////////////////////////////////////////////////
            // 按Ctrl+ C标记副本
			if (Keyboard.Modifiers == ModifierKeys.Control && e.Key == Key.C)
			{
				if (ProjectList.Count != 0)
				{
					ProjectCopy = new Project();
					ProjectCopy.MissionData = DeepCopyHelper.DeepCopy<Mission>(ProjectList[BaseSelect].MissionData);
					ProjectCopy.MissionData.Latitude = MainMap.Position.Lat;
					ProjectCopy.MissionData.Longitude = MainMap.Position.Lng;
					ProjectCopy.OldBasePoint = DeepCopyHelper.DeepCopy<PointLatLng>(ProjectList[BaseSelect].OldBasePoint);
					ProjectCopy.PathData = DeepCopyHelper.DeepCopy<List<FlightPlanning.Path>>(ProjectList[BaseSelect].PathData);
					ProjectCopy.WaypointData = DeepCopyHelper.DeepCopy<List<FlightPlanning.Waypoint>>(ProjectList[BaseSelect].WaypointData);

				}

				Debug.WriteLine("Ctrl + C");
			}
			////////////////////////////////////////////////////////////////
            // Ctrl + V Marker标记粘贴
			if (Keyboard.Modifiers == ModifierKeys.Control && e.Key == Key.V)
			{
				if (ProjectCopy.WaypointData.Count != 0)
				{
					ProjectList.Add(ProjectCopy);
					BaseSelect = ProjectList.Count - 1;
					ProjectList[BaseSelect].MissionData.Name = "Mission" + ProjectList.Count.ToString();
					BaseMarkerAdd(ProjectCopy.MissionData, BaseSelect);
					MainMap.Markers.Add(ProjectList[BaseSelect].WaypointPath);
					int j = 1;
					foreach (var _waypoint in ProjectCopy.WaypointData)
					{
						WaypointMarkerAdd(_waypoint, j);
						WaypointPathAdd(ProjectList[BaseSelect].WaypointMarker[j - 1]);	// Path追加
						j++;
					}
					ProjectList[BaseSelect].OldBasePoint = BaseMove(ProjectList[BaseSelect].BaseMarker, ProjectList[BaseSelect].OldBasePoint);
					BasePointValueChanged(ProjectList[BaseSelect].BaseMarker);
					ProjectCopy = new Project();
				}

				Debug.WriteLine("Ctrl + V");
			}
			if (Keyboard.Modifiers == ModifierKeys.Control && e.Key == Key.A)
			{
                // 导标全部选择

				Debug.WriteLine("Ctrl + A");
			}
			if (Keyboard.Modifiers == ModifierKeys.Control && e.Key == Key.Z)
			{
                // 导标撤消
				WaypointMarkerUndo();
				Debug.WriteLine("Ctrl + Z");
			}
			if (Keyboard.Modifiers == ModifierKeys.Control && e.Key == Key.Y)
			{
                //导标重做
				WaypointMarkerRedo();
				Debug.WriteLine("Ctrl + Y");
			}
			#endregion
            #region 更改窗口颜色
            //////////////////////////////////////////////////////////////////
			// ウィンドウ色変更
			//if (e.Key == Key.D0)
			//{
			//	// 背景設定
			//	Resources["WindowBackgroundBrush"] = new SolidColorBrush(Color.FromRgb(0, 0, 0));
			//	ZoomName.ZoomBackground = new SolidColorBrush(Color.FromRgb(0, 0, 0));
			//	Resources["CanvasBackgroundBrush"] = ZoomName.ZoomBackground;
			//}
			//else if (e.Key == Key.D1)
			//{
			//	Resources["WindowBackgroundBrush"] = new SolidColorBrush(Color.FromRgb(31, 26, 80));
			//	ZoomName.ZoomBackground = new SolidColorBrush(Color.FromRgb(31, 26, 80));
			//	Resources["CanvasBackgroundBrush"] = ZoomName.ZoomBackground;
			//}
			//else if (e.Key == Key.D2)
			//{
			//	Resources["WindowBackgroundBrush"] = new SolidColorBrush(Color.FromRgb(86, 79, 184));
			//	ZoomName.ZoomBackground = new SolidColorBrush(Color.FromRgb(86, 79, 184));
			//	Resources["CanvasBackgroundBrush"] = ZoomName.ZoomBackground;
			//}
			//else if (e.Key == Key.D3)
			//{
			//	Resources["WindowBackgroundBrush"] = new SolidColorBrush(Color.FromRgb(236, 94, 0));
			//	ZoomName.ZoomBackground = new SolidColorBrush(Color.FromRgb(236, 94, 0));
			//	Resources["CanvasBackgroundBrush"] = ZoomName.ZoomBackground;
			//}
			//else if (e.Key == Key.D4)
			//{
			//	Resources["WindowBackgroundBrush"] = new SolidColorBrush(Color.FromRgb(150, 63, 170));
			//	ZoomName.ZoomBackground = new SolidColorBrush(Color.FromRgb(150, 63, 170));
			//	Resources["CanvasBackgroundBrush"] = ZoomName.ZoomBackground;
			//}
			//else if (e.Key == Key.D5)
			//{
			//	Resources["WindowBackgroundBrush"] = new SolidColorBrush(Color.FromRgb(209, 66, 51));
			//	ZoomName.ZoomBackground = new SolidColorBrush(Color.FromRgb(176, 61, 79));
			//	Resources["CanvasBackgroundBrush"] = ZoomName.ZoomBackground;
			//}
			//else if (e.Key == Key.D6)
			//{
			//	Resources["WindowBackgroundBrush"] = new SolidColorBrush(Color.FromRgb(199, 204, 208));
			//	ZoomName.ZoomBackground = new SolidColorBrush(Color.FromRgb(199, 204, 208));
			//	Resources["CanvasBackgroundBrush"] = ZoomName.ZoomBackground;
			//}
			//else if (e.Key == Key.D7)
			//{
			//	Resources["WindowBackgroundBrush"] = new SolidColorBrush(Color.FromRgb(60, 210, 123));
			//	ZoomName.ZoomBackground = new SolidColorBrush(Color.FromRgb(60, 210, 123));
			//	Resources["CanvasBackgroundBrush"] = ZoomName.ZoomBackground;
			//}
			//else if (e.Key == Key.D8)
			//{
			//	Resources["WindowBackgroundBrush"] = new SolidColorBrush(Color.FromRgb(240, 240, 240));
			//	ZoomName.ZoomBackground = new SolidColorBrush(Color.FromRgb(240, 240, 240));
			//	Resources["CanvasBackgroundBrush"] = ZoomName.ZoomBackground;
			//}
			//else if (e.Key == Key.D9)
			//{
			//	Resources["WindowBackgroundBrush"] = new SolidColorBrush(Color.FromRgb(172, 197, 224));
			//	ZoomName.ZoomBackground = new SolidColorBrush(Color.FromRgb(172, 197, 224));
			//	Resources["CanvasBackgroundBrush"] = ZoomName.ZoomBackground;
			//}
			//else
			//{
			//	// デフォルト
			//	Resources["WindowBackgroundBrush"] = new SolidColorBrush(Color.FromRgb(31, 26, 80));
			//	ZoomName.ZoomBackground = new SolidColorBrush(Color.FromRgb(31, 26, 80));
			//	Resources["CanvasBackgroundBrush"] = ZoomName.ZoomBackground;
			//}
			#endregion
		}
        // 键盘事件的主画面（释放）
		private void Window_PreviewKeyUp(object sender, System.Windows.Input.KeyEventArgs e)
		{
			if (ProjectList.Count != 0)
			{
				if (e.Key == Key.LeftCtrl || e.Key == Key.RightCtrl)
				{
                    // 水平面的转换
					PointLatLng OriginMeter = FlightPlanXML.LatLngPerMeter(ProjectList[BaseSelect].BaseMarker.Position);
					double _ang = ProjectList[BaseSelect].MissionData.Rotation * Math.PI / 180;
					double c = Math.Cos(_ang);
					double s = Math.Sin(_ang);
					double sx = ProjectList[BaseSelect].MissionData.ScaleX;
					double sy = ProjectList[BaseSelect].MissionData.ScaleY;
					double T11 = c * sx;
					double T12 = -s * sy;
					double T21 = s * sx;
					double T22 = c * sy;
					double det = 1 / (T11 * T22 - T12 * T21);

					T11 *= det;
					T12 *= det;
					T21 *= det;
					T22 *= det;

					for (int i = 0; i < ProjectList[BaseSelect].WaypointMarker.Count; i++)
					{
						ProjectList[BaseSelect].WaypointMarker[i].Mode = -1;
						WaypointValueUpdata(ProjectList[BaseSelect].WaypointMarker[i], OriginMeter, T11, T12, T21, T22);
					}
					WaypointSelect.Clear();
					WaypointValueChanged(BaseSelect);
				}

			}
		}
        // 鼠标触控操作切换按钮
        private void TouchIcon_Click(object sender, RoutedEventArgs e)
        {
			foreach (TabletDevice tabletDevice in Tablet.TabletDevices)
			{
				if (tabletDevice.Type == TabletDeviceType.Touch)
				{
					if (SettingData.DeviceMode)
					{
						SettingData.DeviceMode = false;
					}
					else
					{
						SettingData.DeviceMode = true;
					}
					ChangeTouchMouse(SettingData.DeviceMode, false);
				}
			}
        }
        //　鼠标触摸操作事件开关（鼠标事件发生预防在触摸操作的时间）
        private void ChangeTouchMouse(bool f, bool Init)
        {
            //Altitude.DeviceMode = f;
            if (f)
            {
                // 对于鼠标操作
                TouchIcon.LargeImageSource = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/mouse2.png")));

				//// Waypoint标记
				//this.MainMap.MouseDoubleClick += new System.Windows.Input.MouseButtonEventHandler(this.Uav_WayPoint);
				//this.MainMap.MouseRightButtonDown += new System.Windows.Input.MouseButtonEventHandler(this.MarkerControl_MouseRightButtonDown);
				//this.MainMap.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(MainMap_MouseLeftButtonDown);

				//// 地图操作
				//this.MainMap.MouseDown += new System.Windows.Input.MouseButtonEventHandler(this.MainMap_MouseDown);
				//this.MainMap.MouseMove += new System.Windows.Input.MouseEventHandler(this.MainMap_MouseMove);
				//this.MainMap.MouseUp += new System.Windows.Input.MouseButtonEventHandler(this.MainMap_MouseUp);
				//this.MainMap.TouchDown += new EventHandler<TouchEventArgs>(this.MainMap_TouchDown);
				//this.MainMap.TouchUp += new EventHandler<TouchEventArgs>(this.MainMap_TouchUp);
				//this.MainMap.TouchMove += new EventHandler<TouchEventArgs>(this.MainMap_TouchMove);
                

				//this.Main_Window.TouchUp += new EventHandler<TouchEventArgs>(this.Main_TouchUp);

                MainMap.CanDragMap = true; // 在地图上的功能开始用鼠标右键单击
				MissionProperty.FontSize = 12;
				WaypointProperty.FontSize = 12;
            }
            else
            {
                // 触摸操作
                TouchIcon.LargeImageSource = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/command2.png")));

				//// Waypoint标记
				//this.MainMap.MouseDoubleClick += new System.Windows.Input.MouseButtonEventHandler(this.Uav_WayPoint);
				//this.MainMap.MouseRightButtonDown += new System.Windows.Input.MouseButtonEventHandler(this.MarkerControl_MouseRightButtonDown);
				//this.MainMap.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(MainMap_MouseLeftButtonDown);

				//// 地图操作
				//this.MainMap.MouseDown += new System.Windows.Input.MouseButtonEventHandler(this.MainMap_MouseDown);
				//this.MainMap.MouseMove += new System.Windows.Input.MouseEventHandler(this.MainMap_MouseMove);
				//this.MainMap.MouseUp += new System.Windows.Input.MouseButtonEventHandler(this.MainMap_MouseUp);
				//this.MainMap.TouchDown += new EventHandler<TouchEventArgs>(this.MainMap_TouchDown);
				//this.MainMap.TouchUp += new EventHandler<TouchEventArgs>(this.MainMap_TouchUp);
				//this.MainMap.TouchMove += new EventHandler<TouchEventArgs>(this.MainMap_TouchMove);

				//this.Main_Window.TouchUp += new EventHandler<TouchEventArgs>(this.Main_TouchUp);

				MissionProperty.FontSize = 18;
				WaypointProperty.FontSize = 18;
            }
        }
        #endregion

        #region 色带（顶部菜单区）
        // 功能区显示/隐藏
		private void RibbonButton_Click(object sender, RoutedEventArgs e)
		{
			if (MainRibbon.IsMinimized)
			{
				MainRibbon.IsMinimized = false;
				RibbonRow.Height = new GridLength(140);
			}
			else
			{
				MainRibbon.IsMinimized = true;
				RibbonRow.Height = new GridLength(50);
			}

		}

        #region 地图设置
        //CustomMap参考按钮
		private void CustomMapButton_Click(object sender, RoutedEventArgs e)
		{
			try
			{
				var Dlg = new System.Windows.Forms.FolderBrowserDialog();

                Dlg.Description = "选择";

				if (Dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
				{
					CustomMapText.Text = Dlg.SelectedPath;

					GMap.NET.MapProviders.DllMapProvider.Instance.SetTileImage(CustomMapText.Text);

				}
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show(" Error: " + ex.Message);
				_logger.Error(" Error: " + ex.Message);
			}

		}
        // 地图的选择
        Dictionary<String, String> dictGmapPlugin = new Dictionary<string, string>();
        private void MapProviders_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // 如果你选择了地图，改变地图选择国家/ MapProviderDLL设置
            if (MapProviders.SelectedItem != null)
            {
				MainMap.MapProvider = GMapProviders.OpenStreetMap;

                string map = (string)MapProviders.SelectedItem;
                //从地图名称找到MapProvider，进行设置。

                // 我们将继续以枚举元素
                GMapProvider vvvvv = null;
                foreach (var item in GMapProviders.List)
                {
                    Debug.WriteLine(item.Name);
                    if (map.CompareTo(item.Name) == 0)
                    {
                        MainMap.MapProvider = item;
                        return;
                    }
                    if (item.GetType().ToString().CompareTo("GMap.NET.MapProviders.DllMapProvider") == 0)
                    {
                        vvvvv = item;
                    }
                }

                GMapProviders.List.Remove(vvvvv);

                GMap.NET.MapProviders.DllMapProvider.Instance.setPluging(dictGmapPlugin[map]);
                GMapProvider _map = GMap.NET.MapProviders.DllMapProvider.Instance;

				MainMap.MapProvider = GMapProviders.OpenStreetMap;
				//MainMap.MapProvider = null;//GMap.NET.MapProviders.EmptyProvider.Instance;      // 空MapProvider for Clear
				Thread.Sleep(1 * 1000); //空MapProvider
                MainMap.MapProvider = _map;
                Copyright.Content = dicMapNameLicence[map];

				if (MainMap.MapProvider.ToString() == "国土地理院地图(航空)")
				{
					DisName.Foreground = Brushes.White;
					Copyright.Foreground = Brushes.White;
					GridLine.BorderBrush = Brushes.White;
					Dis1.Stroke = Brushes.White;
					BuildView(Brushes.White); 
				}
				else
				{
					DisName.Foreground = new SolidColorBrush(Color.FromRgb(31, 26, 80));
					Copyright.Foreground = new SolidColorBrush(Color.FromRgb(31, 26, 80));
					Dis1.Stroke = new SolidColorBrush(Color.FromRgb(31, 26, 80));
					GridLine.BorderBrush = new SolidColorBrush(Color.FromRgb(31, 26, 80));
					BuildView(new SolidColorBrush(Color.FromRgb(31, 26, 80)));
				}
				
            }
        }
		private void LatLngSet_Click(object sender, RoutedEventArgs e)
		{
			try
			{
				point.Lat = double.Parse(LatText.Text);
				point.Lng = double.Parse(LngText.Text);
				MainMap.Position = point;
			}
			catch
			{
				_logger.Error("TextBox_TextChanged");
			}
		}
        //地理编码搜索
		private void AddressSearch_Click(object sender, RoutedEventArgs e)
		{
			try
			{
                // TODO:用于地理编码的要仅向谷歌的限制，考虑自由使用的注意
				//SearchAddressValue("http://maps.google.com/maps/api/geocode/xml?address=" + AddressName.Text + "&sensor=false");

				string _text = AddressText.Text;
				AddressText.Text = "Wait...";
				Thread.Sleep(10);
				Geocoding("http://www.geocoding.jp/api/?v=1.1&q=" + _text);
				AddressText.Text = "[" + _text + "] Lat/ " + point.Lat.ToString() + "    Lng/ " + point.Lng.ToString();
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(1001) Geocoding Error: " + ex.Message);
				_logger.Error(" Error: " + ex.Message);
			}
		}
        // 地理编码搜索的体现
		private void AddressSet_Click(object sender, RoutedEventArgs e)
		{
			MainMap.Position = point;
			LatText.Text = point.Lat.ToString();
			LngText.Text = point.Lng.ToString();
		}
		private PointLatLng point = new PointLatLng();
		private void Geocoding(string SearchAddress)
		{
			// http://www.geocoding.jp/api/
            // 由于需要3-5秒，以XML输出，请避免更多的接入每隔5秒。
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
        // 海拔高度采集
		private void ElevationSet_Click(object sender, RoutedEventArgs e)
		{
			try
			{
				string _text = ElevationText.Text;
				ElevationText.Text = "Wait...";
				Thread.Sleep(10);
				SettingData.Elevation = GetElevationValue("http://cyberjapandata2.gsi.go.jp/general/dem/scripts/getelevation.php?lon=" + MainMap.Position.Lng.ToString() + "&lat=" + MainMap.Position.Lat.ToString() + "&outtype=JSON");

				ElevationText.Text = SettingData.Elevation.ToString() + "m";
				//Altitude.ElevationValue = SettingData.Elevation;
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(1001) Geocoding Error: " + ex.Message);
				_logger.Error("(1001) Geocoding Error: " + ex.Message);
			}

		}
		private double GetElevationValue(string ElevationProvider)
		{
			// 国土地理院
			try
			{
                //由URI标识的数据和资源传送和接收的共同类
				WebClient downloadClient = new WebClient();

				// URL
				Uri uriUrl = new Uri("http://cyberjapandata2.gsi.go.jp/general/dem/scripts/getelevation.php?lon=" + MainMap.Position.Lng.ToString() + "&lat=" + MainMap.Position.Lat.ToString() + "&outtype=JSON");
                // 地理涂层
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
				return (0);
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(1002) Elevation Error: " + ex.Message);
				_logger.Error("(1002) Elevation Error: " + ex.Message);
				return (0);
			}

		}
        #endregion

        #region 航点
        //撤消（撤消）按钮
        private void UndoButton_Click(object sender, RoutedEventArgs e)
        {
            WaypointMarkerUndo();
        }
        //重做（重做）按钮
        private void RedoButton_Click(object sender, RoutedEventArgs e)
        {
            WaypointMarkerRedo();
        }
        #endregion

		// MissionView　表示
		private void MissionView_Click(object sender, RoutedEventArgs e)
		{
			if(!MissionDoc.IsVisible){
				MissionDoc.IsVisible = true;
			}

			MissionDoc.Show();
			
		}
		// WaypointView	表示
		private void WaypointView_Click(object sender, RoutedEventArgs e)
		{
			if (!WaypointDoc.IsVisible)
			{
				WaypointDoc.IsVisible = true;
			}
			WaypointDoc.Show();
		}
        // 高度列表视图的显示
		private void AltitudeListView_Click(object sender, RoutedEventArgs e)
		{
			if (!AltitudeDock.IsVisible)
			{
				AltitudeDock.IsVisible = true;
			}
			AltitudeDock.Show();
		}
        // X-Monitor发送飞行计划
		private void Transmit_Click(object sender, RoutedEventArgs e)
		{
			if (ProjectList.Count > 0)
			{
				if (ProjectList[BaseSelect].WaypointData.Count > 0)
				{
					WaitViewFlag = true;
					WaitWindow.Visibility = Visibility.Visible;
				}
			}

		}
        #endregion

        #region 对话框
        // 飞行计划文件打开对话框
        private void MultiWaypointFile()
        {
            try
            {
                // Instantiate the dialog box
                Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();

                // Configure open file dialog box
                dlg.InitialDirectory = System.IO.Path.GetFullPath(@"Xml/MissionPlanner/");
                //dlg.FileName = "MultiWaypoint";			// Default file name
				dlg.DefaultExt = ".wpp";				// Default file extension
				dlg.Filter = "MultiWaypoint Project|*.wpp";//"Y-Planner Project|*.ypp|MultiWaypoint Project|*.wpp";		// Filter files by extension

                // Open the dialog box modally
                Nullable<bool> result = dlg.ShowDialog();

                // Process open file dialog box results
                if (result == true)
                {
					_logger.Info("MultiWaypointFile:" + dlg.FileName);
					
					MarkerAllClear();
					PointCount = 0;

					FilePath = dlg.FileName;
					if (dlg.FileName.IndexOf(".ypp") != -1)
					{
						ProjectList = FlightPlanning.FlightPlanXML.YppRead(dlg.FileName);
					}
					if (dlg.FileName.IndexOf(".wpp") != -1)
					{
						// wpp读取
						ProjectList = FlightPlanning.FlightPlanXML.WppRead(dlg.FileName);
					}

					if (ProjectList.Count > 0)
					{
						MainMap.Position = new PointLatLng(ProjectList[0].MissionData.Latitude, ProjectList[0].MissionData.Longitude);
						MarkerAllSet();
					}
					ProjectRecord.Add(ProjectDeepCopy(ProjectList));			// ProjectRecord追加
					//ProjectRecord.Clear();
					UndoButton.IsEnabled = false;
					RedoButton.IsEnabled = false;
                }
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("(401) MultiWaypoint FileOpen Error: " + ex.Message);
				_logger.Error("(401) MultiWaypoint FileOpen Error: " + ex.Message);
            }


        }
        // 保存对话框
        private void SaveDocument(string _filePath = "")
        {
            try
            {
                // Configure save file dialog
                Microsoft.Win32.SaveFileDialog dlg = new Microsoft.Win32.SaveFileDialog();
				dlg.FileName = "FlightPlanning_" + DateTime.Now.Year.ToString("0000")
										+ DateTime.Now.Month.ToString("00")
										+ DateTime.Now.Day.ToString("00")
										+ "_" + DateTime.Now.Hour.ToString("00")
										+ DateTime.Now.Minute.ToString("00");   // Default file name

				dlg.DefaultExt = ".wpp";					                            // Default file extension
				dlg.Filter = "MultiWaypoint Project|*.wpp";//"Y-Planner Project|*.ypp|MultiWaypoint Project|*.wpp";	                                    // Filter files by extension
				_logger.Info("SaveDocument:" + _filePath);
                // Show save file dialog
                if (_filePath != "")
                {
                    // 退出确认对话框
                    if (System.Windows.Forms.MessageBox.Show("确定需要保存么？", "保存", System.Windows.Forms.MessageBoxButtons.YesNo, System.Windows.Forms.MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.Yes)
                    {
						if (_filePath.IndexOf(".ypp") != -1)
						{
							FlightPlanning.FlightPlanXML.YppWrite(_filePath, ProjectList);
						}
						if (_filePath.IndexOf(".wpp") != -1)
						{
                            // 覆盖保存
							FlightPlanning.FlightPlanXML.WppWrite(_filePath, ProjectList);
						}
                    }
                }
                else
                {
                    Nullable<bool> result = dlg.ShowDialog();
                    // Process save file dialog results
                    if (result == true)
                    {
                        // 另存为
                        FilePath = dlg.FileName;
						if (FilePath.IndexOf(".ypp") != -1)
						{
							FlightPlanning.FlightPlanXML.YppWrite(dlg.FileName, ProjectList);
						}
						if (FilePath.IndexOf(".wpp") != -1)
						{
							FlightPlanning.FlightPlanXML.WppWrite(dlg.FileName, ProjectList);
						}
                    }
                }
                
            }
            catch (Exception ex)
            {
				System.Windows.MessageBox.Show("(403) FlightPlanning Save Error : " + ex.Message);
				_logger.Error("(403) FlightPlanning Save Error : " + ex.Message);
            }

        }
        #endregion

        #region Base/Waypoint/Path操作

        //////////////////////////////////////////////////////////////////////////////////////
        // 操作
        #region 航点鼠标/触摸事件
        // 航点的设置流程（双击左键）
        private void Uav_WayPoint(object sender, MouseButtonEventArgs e)
        {
			if (ProjectList.Count != 0)
			{
				if (ProjectList[BaseSelect].WaypointData.Count >= Constants._maxMultiWaypoint)
				{
					System.Windows.MessageBox.Show("(000) 航点数设置已达到上限。");
					return;
				}
				if (DrawMode == 0)
				{
					p1 = new System.Windows.Point();
					p1 = e.GetPosition(MainMap);
					WaypointDraw(DrawMode, e.LeftButton, p1, p2);
				}
				else
				{
					if (PointCount == 0)
					{
						p1 = new System.Windows.Point();
						p1 = e.GetPosition(MainMap);
						//WaypointDraw(DrawMode, e.LeftButton, p1, p2);
						PointCount = 1;
					}
					else if (PointCount == 1)
					{

						if (ProjectList[BaseSelect].BaseMarker.IsMouseOver == true)
						{
							return;
						}
						for (int i = 0; i < ProjectList[BaseSelect].WaypointMarker.Count; i++)
						{
                            // 现在是当前鼠标（触摸）操作
							if (ProjectList[BaseSelect].WaypointMarker[i].IsMouseOver == true)
							{
								return;
							}
						}
						p2 = new System.Windows.Point();
						p2 = e.GetPosition(MainMap);
						WaypointDraw(DrawMode, e.LeftButton, p1, p2);
						p1 = p2;
						if (DrawMode == 1)
						{
							PointCount = 1;
						}
						else
						{
							PointCount = 0;
						}
						//PointCount++;
					}
					else
					{
						PointCount = 0;
					}

				}
			}
        }
        // 航点设置处理工艺（右键点击和触摸操作的时候 - 长按）
        private void MarkerControl_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
			if (ProjectList.Count != 0)
			{
				if (ProjectList[BaseSelect].WaypointData.Count >= Constants._maxMultiWaypoint)
				{
					System.Windows.MessageBox.Show("(000) 航点数设置已达到上限。");
					return;
				}
				if (DrawMode == 0)
				{
					//p1 = new System.Windows.Point();
					//p1 = e.GetPosition(MainMap);
					//WaypointDraw(DrawMode, e.LeftButton, p1, p2);

					if (Keyboard.Modifiers != ModifierKeys.Control)
					{
						WaypointSelect.Clear();
					}
					if ((e.RightButton == MouseButtonState.Pressed) /*&& (SettingData.DeviceMode == false)*/)
					{
						if (ProjectList.Count > 0)
						{
                            // 仅在触摸操作
							if (WaypointSelect.Count > 0)
							{
								if (!ProjectList[BaseSelect].WaypointMarker[WaypointSelect[0] - 1].IsMouseOver)
								{
									WaypointAndPathAdd(e.GetPosition(MainMap), BaseSelect);		// Waypoint追加
									ProjectRecord.Add(ProjectDeepCopy(ProjectList));			// ProjectRecord追加
								}
							}
							else
							{
								WaypointAndPathAdd(e.GetPosition(MainMap), BaseSelect);		// Waypoint追加
								ProjectRecord.Add(ProjectDeepCopy(ProjectList));			// ProjectRecord追加
							}
						}
					}

					if (ProjectList[BaseSelect].WaypointMarker.Count > 0)
					{
						for (int i = 0; i < ProjectList[BaseSelect].WaypointMarker.Count; i++)
						{
							if (WaypointSelect.Count > 0)
							{
								if (WaypointSelect[0] != ProjectList[BaseSelect].WaypointMarker[i].ID)
								{
									ProjectList[BaseSelect].WaypointMarker[i].Mode = -1;
								}
							}
						}
					}
				}
				else
				{
					if (PointCount == 0)
					{
						p1 = new System.Windows.Point();
						p1 = e.GetPosition(MainMap);
						//WaypointDraw(DrawMode, e.LeftButton, p1, p2);
						PointCount = 1;
					}
					else if (PointCount == 1)
					{

						if (ProjectList[BaseSelect].BaseMarker.IsMouseOver == true)
						{
							return;
						}
						for (int i = 0; i < ProjectList[BaseSelect].WaypointMarker.Count; i++)
						{
                            // 現在是当前鼠标（触摸）操作
							if (ProjectList[BaseSelect].WaypointMarker[i].IsMouseOver == true)
							{
								return;
							}
						}
						p2 = new System.Windows.Point();
						p2 = e.GetPosition(MainMap);
						WaypointDraw(DrawMode, e.LeftButton, p1, p2);
						p1 = p2;
						if (DrawMode == 1)
						{
							PointCount = 1;
						}
						else
						{
							PointCount = 0;
						}
						//PointCount++;
					}
					else
					{
						PointCount = 0;
					}

				}

			}
        }
        
		// TODO
        // 所有航点删除按钮
		private void AllClear_Click(object sender, RoutedEventArgs e)
		{
			WaypointAllClear();
			//MarkerAllClear();
			//ProjectRecord.Clear();
			//ProjectSubRecord.Clear();
		}
        // 所有航点的选择
		private void AllSelect_Click(object sender, RoutedEventArgs e)
		{
			if (Keyboard.Modifiers != ModifierKeys.Control)
			{
				WaypointSelect.Clear();
				foreach (var _Waypoint in ProjectList[BaseSelect].WaypointData)
				{
					WaypointSelect.Add(_Waypoint.ID);
				}
			}
		}
		#endregion

        #region 要通过（撤消）恢复为航点源|重做（重做）事件
        private void WaypointMarkerUndo()
        {
			try
			{
				if (ProjectRecord.Count > 1)
				{
                    ProjectSubRecord.Add(ProjectDeepCopy(ProjectList));						// 重做（重做）添加
					MarkerAllClear();
					ProjectRecord.RemoveAt(ProjectRecord.Count - 1);
                    ProjectList = ProjectDeepCopy(ProjectRecord[ProjectRecord.Count - 1]);	// 撤消（撤消）(Undo)
					WaypointListView.Items.Clear();
					MarkerAllSet();											//　Project再設定
				}
				if (ProjectRecord.Count > 0)
				{
                    // 重做（重做）按钮切换到使用
					RedoButton.IsEnabled = true;
				}

				if (ProjectRecord.Count == 1)
				{
                    // 切换到撤消（撤消）按钮被禁用
					UndoButton.IsEnabled = false;
				}
				missionSelect.IsEnabled = true;
				MissionProperty.IsEnabled = true;

			}
			catch
			{

			}
        }
        private void WaypointMarkerRedo()
        {
			try
			{
				if (ProjectSubRecord.Count > 0)
				{
					ProjectRecord.Add(ProjectDeepCopy(ProjectSubRecord[ProjectSubRecord.Count - 1]));
					MarkerAllClear();
					ProjectList = ProjectDeepCopy(ProjectSubRecord[ProjectSubRecord.Count - 1]);	// 重做(Undo)
					ProjectSubRecord.RemoveAt(ProjectSubRecord.Count - 1);
					WaypointListView.Items.Clear();
					MarkerAllSet();																	//　Project再設定

                    // 可切换到撤消（撤消）按钮
					UndoButton.IsEnabled = true;
				}

				if (ProjectSubRecord.Count == 0)
				{
                    // 不可切换到重做按钮
					RedoButton.IsEnabled = false;
				}
				missionSelect.IsEnabled = true;
				MissionProperty.IsEnabled = true;
			}
			catch
			{

			}
        }
        #endregion

        //////////////////////////////////////////////////////////////////////////////////////
        // 基準点
        #region 参考点标记
        // 添加参考点（添加任务）
        private void BasePointAdd_Click(object sender, RoutedEventArgs e)
        {
			try
			{
				MissionAndBaseMarkerAdd();					// Base追加
				ProjectRecord.Add(ProjectDeepCopy(ProjectList));				// ProjectRecord追加
			}
			catch
			{

			}
        }
        // 删除基准点（使命删除）
        private void BasePointDel_Click(object sender, RoutedEventArgs e)
        {
			try
			{
				if (ProjectList.Count > 0)
				{
					MissionDel(BaseSelect);
					PointCount = 0;
					//MapToMarkerClear(ProjectList[BaseSelect].BaseMarker);
					//BaseMarker.RemoveAt(BaseMarker.Count - 1);
				}
			}
			catch
			{

			}

        }

		// MissionView
		private void LatValue_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			try
			{
                // BaseMarker移动
				ProjectList[BaseSelect].BaseMarker.Position = new PointLatLng((double)e.NewValue, ProjectList[BaseSelect].BaseMarker.Position.Lng);
				ProjectList[BaseSelect].OldBasePoint = BaseMove(ProjectList[BaseSelect].BaseMarker, ProjectList[BaseSelect].OldBasePoint);
				BasePointValueChanged(ProjectList[BaseSelect].BaseMarker);

				//BaseMarker[0].Position = new PointLatLng((double)e.NewValue, BaseMarker[0].Position.Lng);
				//_oldBasePoint = BaseMove(BaseMarker[0], _oldBasePoint);
				//BasePointValueChanged(BaseMarker[0]);
				MissionProperty.SelectedObject = ProjectList[BaseSelect].MissionData;
			}
			catch
			{

			}

		}
		// MissionView 
		private void LonValue_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			try
			{
                // BaseMarker移动
				ProjectList[BaseSelect].BaseMarker.Position = new PointLatLng(ProjectList[BaseSelect].BaseMarker.Position.Lat, (double)e.NewValue);
				ProjectList[BaseSelect].OldBasePoint = BaseMove(ProjectList[BaseSelect].BaseMarker, ProjectList[BaseSelect].OldBasePoint);
				BasePointValueChanged(ProjectList[BaseSelect].BaseMarker);

				//BaseMarker[0].Position = new PointLatLng(BaseMarker[0].Position.Lat, (double)e.NewValue);
				//_oldBasePoint = BaseMove(BaseMarker[0], _oldBasePoint);
				//BasePointValueChanged(BaseMarker[0]);
				MissionProperty.SelectedObject = ProjectList[BaseSelect].MissionData;
			}
			catch
			{

			}
		}
		// MissionView  missionSelect
		private void missionSelect_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			try
			{
				if (missionSelect.SelectedIndex != -1)
				{
					BaseSelect = missionSelect.SelectedIndex;
					// MissionView
					BasePointViewUpdata(BaseSelect);
					WaypointValueChanged(BaseSelect);
				}
			}
			catch
			{

			}
		}
		// MissionView  missionConnec
		private void missionConnect_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			try
			{
				if (missionConnect.SelectedIndex != missionSelect.SelectedIndex && missionConnect.SelectedIndex != -1)
				{
					if (ProjectList[missionSelect.SelectedIndex].WaypointData.Count > 0)
					{
						var _waypoint1 = ProjectList[missionSelect.SelectedIndex].WaypointData.Last();
						var _waypoint2 = ProjectList[missionConnect.SelectedIndex].WaypointData.First();
						ProjectList[missionSelect.SelectedIndex].ProjectPath.Points[0] = new PointLatLng(_waypoint1.Latitude, _waypoint1.Longitude);
						ProjectList[missionSelect.SelectedIndex].ProjectPath.Points[1] = new PointLatLng(_waypoint2.Latitude, _waypoint2.Longitude);
						ProjectList[missionSelect.SelectedIndex].ProjectPath.RegenerateShape(MainMap);

						ProjectList[missionSelect.SelectedIndex].MissionData.AccessID = missionConnect.SelectedIndex + 1;
						ProjectList[missionConnect.SelectedIndex].MissionData.ConnectID = missionSelect.SelectedIndex + 1;

						BasePointViewUpdata(missionSelect.SelectedIndex);
					}
				}
			}
			catch
			{

			}

		}

		private void RotValue_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			try
			{
				ProjectList[BaseSelect].MissionData.Rotation = (double)e.NewValue;
				ProjectList[BaseSelect].BaseMarker.Azimuth = (double)e.NewValue;
				ProjectList[BaseSelect].OldBasePoint = BaseMove(ProjectList[BaseSelect].BaseMarker, ProjectList[BaseSelect].OldBasePoint);
				BasePointValueChanged(ProjectList[BaseSelect].BaseMarker);
				MissionProperty.SelectedObject = ProjectList[BaseSelect].MissionData;
			}
			catch
			{

			}
		}

		private void XValue_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			try
			{
				ProjectList[BaseSelect].MissionData.ScaleX = (double)e.NewValue;
				ProjectList[BaseSelect].OldBasePoint = BaseMove(ProjectList[BaseSelect].BaseMarker, ProjectList[BaseSelect].OldBasePoint);
				BasePointValueChanged(ProjectList[BaseSelect].BaseMarker);
				MissionProperty.SelectedObject = ProjectList[BaseSelect].MissionData;
			}
			catch
			{

			}
		}

		private void YValue_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			try
			{
				ProjectList[BaseSelect].MissionData.ScaleY = (double)e.NewValue;
				ProjectList[BaseSelect].OldBasePoint = BaseMove(ProjectList[BaseSelect].BaseMarker, ProjectList[BaseSelect].OldBasePoint);
				BasePointValueChanged(ProjectList[BaseSelect].BaseMarker);
				MissionProperty.SelectedObject = ProjectList[BaseSelect].MissionData;
			}
			catch
			{

			}
		}
        #endregion

		#endregion

        #region Project（任务/航点/路径管理类）
        //////////////////////////////////////////////////////////////////////////////////////
		// Project管理
		#region Project管理
        //ProjectList设置所有标记
		private void MarkerAllSet()
		{
			try
			{
				int k = 0;
				foreach (var _project in ProjectList)
				{
					BaseSelect = k;
					if (_project.MissionData.enabled)
					{
						BaseMarkerAdd(_project.MissionData, BaseSelect);
						MainMap.Markers.Add(ProjectList[BaseSelect].WaypointPath);

						//BasePointValueChanged(_project.BaseMarker);
						int j = 1;
						foreach (var _waypoint in _project.WaypointData)
						{
							_project.WaypointData[j - 1].ID = j;
							WaypointMarkerAdd(_waypoint, j);
							WaypointPathAdd(ProjectList[BaseSelect].WaypointMarker[j - 1]);	// Path追加
							j++;
						}
					}
					AltitudePropertyValue.AltitudeUpdate();
					BasePointValueChanged(ProjectList[BaseSelect].BaseMarker);
					BasePointViewUpdata(BaseSelect);

					k++;
				}

				if (ProjectList.Count > 0)
				{
                    // 使得切换到撤消（撤消）按钮可用
					UndoButton.IsEnabled = true;
				}
			}
			catch
			{

			}
		}
        // 删除所有标记
		private void MarkerAllClear()
		{
			try
			{
				WaypointListView.Items.Clear();
				missionSelect.Items.Clear();
				missionConnect.Items.Clear();
				ProjectList.Clear();
				MainMap.Markers.Clear();
				AltitudePropertyValue.Clear();
				PointCount = 0;
                // 显示连接时无人机标志
				if (Client.Connected)
				{
                    // 当前的经度和纬度/方位UAVmarker
					UAVMarker = new GMapMarker(new PointLatLng());
					{
						UAVMarker.Shape = new Position(UAVMarker);
						UAVMarker.Offset = new System.Windows.Point(-15, -25);
						UAVMarker.ZIndex = int.MaxValue;
						MainMap.Markers.Add(UAVMarker);
					}
				}
				WaypointProperty.IsEnabled = false;
				MissionProperty.IsEnabled = false;
				missionSelect.IsEnabled = false;
			}
			catch
			{

			}
		}
        // 航点全部删除
		private void WaypointAllClear()
		{
			try
			{
				WaypointListView.Items.Clear();
				PointCount = 0;
				//missionSelect.Items.Clear();
				//missionConnect.Items.Clear();
				//ProjectList.Clear();
				//MainMap.Markers.Clear();


                // 重新显示连接时间无人机标志
				//if (Client.Connected)
				//{
				//	// 現在経度緯度/方位角　UAVmarker
				//	UAVMarker = new GMapMarker(new PointLatLng());
				//	{
				//		UAVMarker.Shape = new Position(UAVMarker);
				//		UAVMarker.Offset = new System.Windows.Point(-15, -25);
				//		UAVMarker.ZIndex = int.MaxValue;
				//		MainMap.Markers.Add(UAVMarker);
				//	}
				//}
				for (int i = 0; i < ProjectList[BaseSelect].WaypointMarker.Count; i++)
				{
					var clear = MainMap.Markers.Where(p => p != null && p == ProjectList[BaseSelect].WaypointMarker[i]);
					if (clear != null)
					{
						for (int j = 0; j < clear.Count(); j++)
						{
							MainMap.Markers.Remove(clear.ElementAt(j));
							j--;
						}
					}
				}
				ProjectList[BaseSelect].WaypointData.Clear();
				ProjectList[BaseSelect].WaypointMarker.Clear();
				ProjectList[BaseSelect].WaypointPath.Points.Clear();
				ProjectList[BaseSelect].WaypointPath.Clear();
				AltitudePropertyValue.Clear();

				WaypointProperty.IsEnabled = false;
				WaypointProperty.SelectedObject = null;
			}
			catch
			{

			}

		}
		#endregion

		//////////////////////////////////////////////////////////////////////////////////////
		// Mission管理
		#region 任务管理
		// 新的Mission/BaseMarker追加
		private void MissionAndBaseMarkerAdd()
		{
			ProjectList.Add(new Project());										//添加项目
			BaseSelect = ProjectList.Count - 1;									//项目选择更换

			ProjectList[BaseSelect].WaypointData = new List<FlightPlanning.Waypoint>();
			ProjectList[BaseSelect].WaypointMarker = new List<GMapMarker>();
			ProjectList[BaseSelect].PathData = new List<FlightPlanning.Path>();

			MainMap.Markers.Add(ProjectList[BaseSelect].WaypointPath);

			ProjectList[BaseSelect].MissionData = MissionAdd(MainMap.Position);	// 新規Mission追加
			BaseMarkerAdd(ProjectList[BaseSelect].MissionData, BaseSelect);					// 新規BaseMarker追加

			// MissionView
			BasePointViewUpdata(BaseSelect);

			missionSelect.IsEnabled = true;
			MissionProperty.IsEnabled = true;
		}
		// 新規Mission追加
		private Mission MissionAdd(PointLatLng _point)
		{
			// Mission管理
			var _mission = new Mission()
			{
				Name = "Mission" + (BaseSelect + 1).ToString(),
				enabled = true,
				Latitude = _point.Lat,
				Longitude = _point.Lng,
				Altitude = 0,   //
				Rotation = 0,   //
				ScaleX = 1.0,   //
				ScaleY = 1.0,   //
				ScaleZ = 1.0,   //
				AltitudeMode = "relativeToHome"
			};

			return _mission; 
		}
		// 新規BaseMarker追加
		private void BaseMarkerAdd(Mission _mission, int _index)
		{
			// 选择清除
			if (ProjectList.Count > 0)
			{
				for(int i = 0;i < (ProjectList.Count - 1) ; i++)
				{
					if (ProjectList[i].BaseMarker != null)
					{
						ProjectList[i].BaseMarker.Mode = -1;
					}
				}
			}
            // 嵌入的地图上的参考点
			GMapMarker _baseMarker = new GMapMarker(new PointLatLng(_mission.Latitude,_mission.Longitude));
			{
				_baseMarker.Altitude = _mission.Altitude;
				_baseMarker.Azimuth = _mission.Rotation;
				_baseMarker.Speed = Properties.Settings.Default.Waypoint_Speed;
				_baseMarker.Shape = new BasePoint(this, _baseMarker, SettingData.DeviceMode, ProjectList.Count, true);
				_baseMarker.Offset = new System.Windows.Point(-15, -15);
				_baseMarker.ZIndex = int.MaxValue;
				_baseMarker.Mode = 1;
			}
			ProjectList[_index].BaseMarker = _baseMarker;

            // 嵌入BaseMarker在地图上
			MainMap.Markers.Add(ProjectList[_index].BaseMarker);

			// 
			missionSelect.Items.Add(_mission.Name);
			missionConnect.Items.Add(_mission.Name);

			ProjectList[_index].ProjectPath.Points.Add(new PointLatLng());
			ProjectList[_index].ProjectPath.Points.Add(new PointLatLng());
			ProjectList[_index].ProjectPath.LineBrush = Brushes.Red;
			MainMap.Markers.Add(ProjectList[_index].ProjectPath);

			missionSelect.IsEnabled = true;
			MissionProperty.IsEnabled = true;
		}
		
		// Mission清除
		private void MissionDel(int _index)
		{
			if (ProjectList.Count > 0 && ProjectList.Count > _index)
			{
				MapToMarkerClear(ProjectList[_index].BaseMarker);
				for (int i = 0; i < ProjectList[_index].WaypointMarker.Count; i++)
				{
					MapToMarkerClear(ProjectList[_index].WaypointMarker[i]);
				}
				ProjectList[_index].WaypointPath.Points.Clear();
				MapToMarkerClear(ProjectList[_index].WaypointPath);
				ProjectList.RemoveAt(_index);
				
				// MissionView更新
				BaseSelect = 0;
				missionSelect.Items.Clear();
				for (int i = 0; i < ProjectList.Count; i++)
				{
					ProjectList[i].MissionData.Name = "Mission" + (i + 1).ToString();
					missionSelect.Items.Add("Mission" + (i + 1).ToString());
				}
				BasePointViewUpdata(BaseSelect);
				WaypointValueChanged(BaseSelect);
			}
			MarkerAllClear();
		}

        // 基准点的数据更新
		private void BasePointValueChanged(GMapMarker _marker)
		{
			var _mission = new Mission()
			{
				Name = ProjectList[BaseSelect].MissionData.Name,
				enabled = true,
				Latitude = _marker.Position.Lat,
				Longitude = _marker.Position.Lng,
				Altitude = _marker.Altitude,   //
				Rotation = _marker.Azimuth,   //
				ScaleX = ProjectList[BaseSelect].MissionData.ScaleX,   //
				ScaleY = ProjectList[BaseSelect].MissionData.ScaleY,   //
				ScaleZ = ProjectList[BaseSelect].MissionData.ScaleZ,   //
				AltitudeMode = ProjectList[BaseSelect].MissionData.AltitudeMode,
				AccessID = ProjectList[BaseSelect].MissionData.AccessID,
				ConnectID = ProjectList[BaseSelect].MissionData.ConnectID
			};

            ProjectList[BaseSelect].MissionData = _mission;                      //管理类
			if (!MarkerMove) BasePointViewUpdata(BaseSelect);
		}

		// Misson属性値変化
		private void MissionProperty_PropertyValueChanged(object sender, Xceed.Wpf.Toolkit.PropertyGrid.PropertyValueChangedEventArgs e)
		{
			try
			{
				var _mission = (Mission)MissionProperty.SelectedObject;
				{
					ProjectList[BaseSelect].BaseMarker.Position = new PointLatLng(_mission.Latitude, _mission.Longitude);
					ProjectList[BaseSelect].BaseMarker.Altitude = _mission.Altitude;
					ProjectList[BaseSelect].BaseMarker.Azimuth = _mission.Rotation;
					ProjectList[BaseSelect].MissionData = DeepCopyHelper.DeepCopy<Mission>(_mission);
					ProjectList[BaseSelect].OldBasePoint = BaseMove(ProjectList[BaseSelect].BaseMarker, ProjectList[BaseSelect].OldBasePoint);
					//BasePointValueChanged(ProjectList[BaseSelect].BaseMarker);
					BasePointViewUpdata(BaseSelect);
				}
			}
			catch (Exception ex)
			{
				_logger.Error("Error: " + ex.Message);
			}
		}
		//
		private void BasePointViewUpdata(int _index)
		{
			if (ProjectList.Count > _index)
			{
				missionSelect.SelectedIndex =_index;
				missionName.Text = ProjectList[_index].MissionData.Name;
				missonEnabled.IsChecked = ProjectList[_index].MissionData.enabled;
				LatValue.Value = ProjectList[_index].MissionData.Latitude;
				LonValue.Value = ProjectList[_index].MissionData.Longitude;
				AltValue.Value = ProjectList[_index].MissionData.Altitude;
				RotValue.Value = ProjectList[_index].MissionData.Rotation;

				XValue.Value = ProjectList[_index].MissionData.ScaleX;
				YValue.Value = ProjectList[_index].MissionData.ScaleY;
				ZValue.Value = ProjectList[_index].MissionData.ScaleZ;

				AltitudeMode.SelectedIndex = 0;

				missionConnect.SelectedIndex = ProjectList[_index].MissionData.AccessID - 1;
			}
		}

        // Waypoint属性値変化
		private void WaypointProperty_PropertyValueChanged(object sender, Xceed.Wpf.Toolkit.PropertyGrid.PropertyValueChangedEventArgs e)
		{
			try
			{
				Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem item = e.OriginalSource as Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem;

				if (item != null)
				{
					var _waypoint = (FlightPlanning.Waypoint)WaypointProperty.SelectedObject;
					{
						int _id = _waypoint.ID - 1;
						ProjectList[BaseSelect].WaypointData[_id] = _waypoint;
						WaypointProperty.SelectedObject = null;

						switch (item.DisplayName)
						{
							case "纬度":
								ProjectList[BaseSelect].WaypointMarker[_id].Position = new PointLatLng(ProjectList[BaseSelect].WaypointData[_id].Latitude, ProjectList[BaseSelect].WaypointData[_id].Longitude);
								var _p = FlightPlanXML.MeterPerLatLng(new PointLatLng(ProjectList[BaseSelect].MissionData.Latitude, ProjectList[BaseSelect].MissionData.Longitude), new PointLatLng(_waypoint.Latitude, _waypoint.Longitude));
								ProjectList[BaseSelect].WaypointData[_id].x = _p.X;
								ProjectList[BaseSelect].WaypointData[_id].y = _p.Y;
								break;
							case "经度":
								ProjectList[BaseSelect].WaypointMarker[_id].Position = new PointLatLng(ProjectList[BaseSelect].WaypointData[_id].Latitude, ProjectList[BaseSelect].WaypointData[_id].Longitude);
								var _pp = FlightPlanXML.MeterPerLatLng(new PointLatLng(ProjectList[BaseSelect].MissionData.Latitude, ProjectList[BaseSelect].MissionData.Longitude), new PointLatLng(_waypoint.Latitude, _waypoint.Longitude));
								ProjectList[BaseSelect].WaypointData[_id].x = _pp.X;
								ProjectList[BaseSelect].WaypointData[_id].y = _pp.Y;
								break;
							case "X":
								ProjectList[BaseSelect].WaypointData[_id] = FlightPlanXML.InverseTranslation(ProjectList[BaseSelect].MissionData, _waypoint);
								ProjectList[BaseSelect].WaypointMarker[_id].Position = new PointLatLng(ProjectList[BaseSelect].WaypointData[_id].Latitude, ProjectList[BaseSelect].WaypointData[_id].Longitude);
								break;
							case "Y":
								ProjectList[BaseSelect].WaypointData[_id] = FlightPlanXML.InverseTranslation(ProjectList[BaseSelect].MissionData, _waypoint);
								ProjectList[BaseSelect].WaypointMarker[_id].Position = new PointLatLng(ProjectList[BaseSelect].WaypointData[_id].Latitude, ProjectList[BaseSelect].WaypointData[_id].Longitude);
								break;
							case "高度":
								ProjectList[BaseSelect].WaypointMarker[_id].Altitude = ProjectList[BaseSelect].WaypointData[_id].Height;
								if (AltitudePropertyValue.AltitudeList.Count == ProjectList[BaseSelect].WaypointData.Count)
								{
									List<double> _list = new List<double>();
									_list = AltitudePropertyValue.AltitudeList;
									_list[_id] = ProjectList[BaseSelect].WaypointData[_id].Height;
									AltitudePropertyValue.AltitudeList = _list;
								}
								break;
							case "方位":
								ProjectList[BaseSelect].WaypointMarker[_id].Azimuth = ProjectList[BaseSelect].WaypointData[_id].Heading + ProjectList[BaseSelect].MissionData.Rotation;
								break;
							case "最大速度":
								ProjectList[BaseSelect].WaypointMarker[_id].Speed = ProjectList[BaseSelect].WaypointData[_id].Speed;
								ProjectList[BaseSelect].PathData[_id].Speed = ProjectList[BaseSelect].WaypointData[_id].Speed;
								break;
						}
						ProjectList[BaseSelect].WaypointPath.Points[_id] = ProjectList[BaseSelect].WaypointMarker[_id].Position;        //　Waypoint位置をPath位置に反映
						ProjectList[BaseSelect].WaypointPath.RegenerateShape(MainMap);													//　地図上にPath反映
						WaypointValueChanged(BaseSelect);
						WaypointProperty.SelectedObject = ProjectList[BaseSelect].WaypointData[_id];
					}

				}
			

			}
			catch (Exception ex)
			{
				_logger.Error("Error: " + ex.Message);
			}
		}
		#endregion

		//////////////////////////////////////////////////////////////////////////////////////
		// Waypoint管理
		#region 航点管理
		// 新的WaypointMarker追加
		private void WaypointMarkerAdd(FlightPlanning.Waypoint _waypoint, int _id)
		{
			GMapMarker _marker = new GMapMarker(new PointLatLng(_waypoint.Latitude, _waypoint.Longitude));
			{
				_marker.Altitude = _waypoint.Height;
				_marker.Azimuth = _waypoint.Heading + ProjectList[BaseSelect].MissionData.Rotation;
				_marker.Speed = Properties.Settings.Default.Waypoint_Speed;
				_marker.Shape = new CustomMarkers.Waypoint(this, _marker, SettingData.DeviceMode, _id, false);
				_marker.Offset = new System.Windows.Point(-25, -40);
				_marker.ZIndex = int.MaxValue;
			}
			ProjectList[BaseSelect].WaypointMarker.Add(_marker);
			MainMap.Markers.Add(ProjectList[BaseSelect].WaypointMarker[_id - 1]);  //
			//WaypointPathAdd(ProjectList[BaseSelect].WaypointMarker[ProjectList[BaseSelect].WaypointMarker.Count - 1]);      //
			// this.PathListView.Items.Add();
			// WaypointListViewAdd(WaypointMarker[WaypointMarker.Count - 1], Waypoint);

			//List<double> _a = new List<double>();
			//_a = AltitudePropertyValue.AltitudeList;
			//_a.Add(_waypoint.Height);
			//AltitudePropertyValue.AltitudeList = _a;
			AltitudePropertyValue.AltitudeList.Add(_waypoint.Height);
			AltitudePropertyValue.AltitudeAdd(_waypoint.Height);

			WaypointListView.Items.Add(_waypoint);     //WaypointListView的追加
			WaypointProperty.SelectedObject = _waypoint;    //属性显示

			WaypointProperty.IsEnabled = true;
		}
		// 新規Waypoint管理追加
		private void WaypointAdd(PointLatLng _point, int _id)
		{
			try
			{
				var _waypoint = new FlightPlanning.Waypoint();

				System.Windows.Point _p = FlightPlanXML.MeterPerLatLng(ProjectList[BaseSelect].BaseMarker.Position, _point);
				int _count = ProjectList[BaseSelect].WaypointData.Count;

				if (_count > 0)
				{
					FlightPlanning.Waypoint _waypointList = new FlightPlanning.Waypoint()
					{
						ID = _id,
						Name = "",
						Latitude = _point.Lat,
						Longitude = _point.Lng,
						x = Math.Round(_p.X, 3, MidpointRounding.AwayFromZero),
						y = Math.Round(_p.Y, 3, MidpointRounding.AwayFromZero),
						Height = Math.Round(ProjectList[BaseSelect].WaypointData[_count - 1].Height, 3, MidpointRounding.AwayFromZero),
						Heading = Math.Round(ProjectList[BaseSelect].WaypointData[_count - 1].Heading, 0, MidpointRounding.AwayFromZero),
						ThreasholdH = ProjectList[BaseSelect].WaypointData[_count - 1].ThreasholdH,
						ThreasholdV = ProjectList[BaseSelect].WaypointData[_count - 1].ThreasholdV,
						Wait = ProjectList[BaseSelect].WaypointData[_count - 1].Wait,
						Function = 0,
						Speed = ProjectList[BaseSelect].WaypointData[_count - 1].Speed
					};
					ProjectList[BaseSelect].WaypointData.Add(_waypointList);
				}
				else
				{
					FlightPlanning.Waypoint _waypointList = new FlightPlanning.Waypoint()
					{
						ID = _id,
						Name = "",
						Latitude = _point.Lat,
						Longitude = _point.Lng,
						x = Math.Round(_p.X, 3, MidpointRounding.AwayFromZero),
						y = Math.Round(_p.Y, 3, MidpointRounding.AwayFromZero),
						Height = 0,//Math.Round(Altitude.WaypointAlt, 3, MidpointRounding.AwayFromZero),
						Heading = 0,
						ThreasholdH = Properties.Settings.Default.Waypoint_Threashold,
						ThreasholdV = Properties.Settings.Default.Waypoint_Threashold,
						Wait = Properties.Settings.Default.Waypoint_Wait,
						Function = 0,
						Speed = Properties.Settings.Default.Waypoint_Speed
					};

					ProjectList[BaseSelect].WaypointData.Add(_waypointList);
				}
			}
			catch
			{

			}
			

		}
		#endregion

        #region 航点的ListView/路径路径更新
        // Waypoint&Path的清除
		private void WaypointAndPathDeleted(int _no)
		{
            //MapToMarkerClear(WaypointMarker[_no]);           // 从地图上删除最新的航点
			//WaypointManager.Add(WaypointMarker[_no]);   // Undo/Redo管理的追加
            //WaypointMarker.RemoveAt(_no);               // Waypoint从标记管理删除
			//this.WaypointListView.Items.RemoveAt(_no);  // WaypointListViewから削除
			//this.PathListView.Items.RemoveAt(_no);      // PathListViewから削除
		}

		// Waypoint&Pathのコピー
		private void WaypointAndPathCope(System.Windows.Point point)
		{
			//PointLatLng Waypoint = new PointLatLng();                           //Waypoint位置
			//Waypoint = MainMap.FromLocalToLatLng((int)point.X, (int)point.Y);   //地図上の位置

			//GMapMarker _marker = new GMapMarker(Waypoint);
			//{
			//	_marker.Altitude = 0;
			//	_marker.Azimuth = 0;
			//	_marker.Speed = Properties.Settings.Default.Waypoint_Speed;
			//	_marker.Shape = new CustomMarkers.Waypoint(this, _marker, SettingData.DeviceMode, ProjectList[BaseSelect].WaypointMarker.Count + 1, true);
			//	_marker.Offset = new System.Windows.Point(-25, -40);
			//	_marker.ZIndex = int.MaxValue;

			//	WaypointMarker.Add(_marker);                                    //
			//	MainMap.Markers.Add(WaypointMarker[WaypointMarker.Count - 1]);  //
			//	WaypointPathAdd(WaypointMarker[WaypointMarker.Count - 1]);      //
			//	PathListViewAdd();                                              //
			//	WaypointListViewAdd(WaypointMarker[WaypointMarker.Count - 1], Waypoint);
			//}

			//if (WaypointMarker.Count > 0)
			//{
			//	// 元に戻す(Undo)ボタン使用可に切替
			//	UndoButton.IsEnabled = true;
			//}
		}
		//// WaypointListView更新
		//private void WaypointListViewUdata()
		//{
		//	//CollectionViewSource _view = new CollectionViewSource();
		//	//ObservableCollection<FlightPlanning.Waypoint> _waypointList = new ObservableCollection<FlightPlanning.Waypoint>();

		//	//for (int i = 0; i < WaypointMarker.Count; i++)
		//	//{
		//	//	_waypointList.Add(new FlightPlanning.Waypoint()
		//	//	{
		//	//		ID = i,
		//	//		Name = "",
		//	//		x = 0,
		//	//		y = 0,
		//	//		Height = WaypointMarker[i].Altitude,
		//	//		Heading = WaypointMarker[i].Azimuth,
		//	//		ThreasholdH = 0,
		//	//		ThreasholdV = 0,
		//	//		Wait = 0,
		//	//		Function = 0
		//	//	});
		//	//}

		//	//_view.Source = _waypointList;
		//	//this.WaypointListView.DataContext = _view;

		//	//// 自動調整
		//	//foreach (GridViewColumn column in (this.WaypointListView.View as GridView).Columns)
		//	//{
		//	//	column.Width = 0;
		//	//	column.Width = double.NaN;
		//	//}
		//}
		//// WaypointListView追加
		//private void WaypointListViewAdd(GMapMarker _marker, PointLatLng _point)
		//{
		//	//// TODO 変換処理
		//	//System.Windows.Point _p = FlightPlanXML.MeterPerLatLng(MainMap.Position, _marker.Position);

		//	//FlightPlanning.Waypoint _waypointList = new FlightPlanning.Waypoint()
		//	//{
		//	//	ID = WaypointMarker.Count,
		//	//	Name = "",
		//	//	Latitude = _marker.Position.Lat,
		//	//	Longitude = _marker.Position.Lng,
		//	//	x = _p.X,
		//	//	y = _p.Y,
		//	//	Height = _marker.Altitude,
		//	//	Heading = _marker.Azimuth,
		//	//	ThreasholdH = 0,
		//	//	ThreasholdV = 0,
		//	//	Wait = 0,
		//	//	Function = 0
		//	//};
		//	//this.WaypointListView.Items.Add(_waypointList);     //WaypointListViewに追加
		//	////TODO
		//	//if (BaseMarker.Count == 1)
		//	//{
		//	//	ProjectList[0].WaypointData.Add(_waypointList);
		//	//	WaypointProperty.SelectedObject = ProjectList[0].WaypointData[ProjectList[0].WaypointData.Count - 1];    //プロパティ表示
		//	//}
		//}
		#endregion

		// ListView&Property的数据更新
		private void WaypointValueUpdata(GMapMarker _marker,PointLatLng _originMeter, double T11, double T12, double T21, double T22)
		{
			var _waypointList = new FlightPlanning.Waypoint();
			//_waypointList = (FlightPlanning.Waypoint)this.WaypointListView.Items[_marker.ID - 1];
			_waypointList = ProjectList[BaseSelect].WaypointData[_marker.ID - 1];

			if (_waypointList.ID == _marker.ID)
			{
				//System.Windows.Point _p = FlightPlanXML.MeterPerLatLng(ProjectList[BaseSelect].BaseMarker.Position, _marker.Position);
				_waypointList.Latitude = _marker.Position.Lat;
				_waypointList.Longitude = _marker.Position.Lng;
				if (!BaseMarkerMove)
				{
					double xx = (_marker.Position.Lat - ProjectList[BaseSelect].BaseMarker.Position.Lat) * _originMeter.Lat;
					double yy = (_marker.Position.Lng - ProjectList[BaseSelect].BaseMarker.Position.Lng) * _originMeter.Lng;
					double _x = T22 * xx - T12 * yy;
					double _y = -T21 * xx + T11 * yy;
					_waypointList.x = Math.Round(_x, 3, MidpointRounding.AwayFromZero);
					_waypointList.y = Math.Round(_y, 3, MidpointRounding.AwayFromZero);
					_waypointList.Height = Math.Round(_marker.Altitude, 3, MidpointRounding.AwayFromZero);
					double _diff = (_waypointList.Heading + ProjectList[BaseSelect].MissionData.Rotation) - _marker.Azimuth;
					if ((_diff > 1) || (_diff < -1))
					{
						double _ang = _marker.Azimuth - ProjectList[BaseSelect].MissionData.Rotation;
						_waypointList.Heading = Math.Round(_ang, 0, MidpointRounding.AwayFromZero);
					}
				}
				WaypointListView.Items[_marker.ID - 1] = _waypointList;    // リストビュー表示
				ProjectList[BaseSelect].WaypointData[_marker.ID - 1] = _waypointList;
			}
		}

		private void WaypointValueChanged(int _index)
		{
			double _dis = 0;
			double _time = 0;
			if (ProjectList.Count > _index)
			{
				WaypointListView.Items.Clear();
				int i = 0;
				foreach (var _waypoint in ProjectList[_index].WaypointData)
				{
					if (i != 0 && i < ProjectList[_index].WaypointData.Count)
					{
						#region 推算飞行距离和时间
						// 距離算出
						var _sdis = Math.Round(
							MainMap.MapProvider.Projection.GetDistance(
							new PointLatLng(ProjectList[_index].WaypointMarker[i - 1].Position.Lat, ProjectList[_index].WaypointMarker[i - 1].Position.Lng),			// 1Point
							new PointLatLng(ProjectList[_index].WaypointMarker[i].Position.Lat, ProjectList[_index].WaypointMarker[i].Position.Lng)        // 2Point
							), 4, MidpointRounding.AwayFromZero);
						_sdis *= 1000;    // km -> m

						double _h = Math.Abs(ProjectList[_index].WaypointMarker[i].Altitude - ProjectList[_index].WaypointMarker[i - 1].Altitude);
						 _sdis = Math.Sqrt((_sdis * _sdis) + (_h * _h));
						_dis += _sdis;

						double _t = 0;
						for (int k = 1; k <= _sdis; k++)
						{
							double _s = k*0.2;
							if (ProjectList[_index].WaypointMarker[i].Speed > _s)
							{
								_t += _s;
							}
							else
							{
								_t += ProjectList[_index].WaypointMarker[i].Speed;
							}
						}
						double _stime = _t;
						//var _stime = _sdis / (ProjectList[_index].WaypointData[i].Speed);	// [m/s]
						_time += _stime + ProjectList[_index].WaypointData[i].Wait;

						#endregion
					}
					WaypointListView.Items.Add(_waypoint);
					i++;
				}

                // 飞行计划估计距离和预计到达时间
				TotalText.Text = _dis.ToString();		//总距离[m]

				//int millisec = Environment.TickCount; // 毫秒単位

				int _sec = (int)Math.Truncate(_time);
				//int _millisec = (int)(_time % 1000.0);
				//var _ts = new TimeSpan(0, 0, 0, _sec, _millisec);
				var _ts = new TimeSpan(0, 0, 0, _sec);
				EstimationText.Text = _ts.ToString();	//预计任务时间
			}
		}
		//////////////////////////////////////////////////////////////////////////////////////
		// Path管理
		#region 路径管理
		// 新的Path追加
		void WaypointPathAdd(GMapMarker _marker)
		{
			ProjectList[BaseSelect].WaypointPath.Points.Add(_marker.Position);
			ProjectList[BaseSelect].WaypointPath.RegenerateShape(MainMap);                      //　地图上的Path显示

			int Count = ProjectList[BaseSelect].WaypointPath.Points.Count;
			FlightPlanning.Path _PathList = new FlightPlanning.Path()
			{
				Waypoint1 = Count,
				Waypoint2 = Count + 1,
				Speed = ProjectList[BaseSelect].WaypointData[Count- 1].Speed,
				Shape = "Linear"
			};
			//this.PathListView.Items.Add(_PathList);
			ProjectList[BaseSelect].PathData.Add(_PathList);
		}
		// 根目录更新
		private void PathListViewUdata()
		{
			//CollectionViewSource _view = new CollectionViewSource();
			//ObservableCollection<FlightPlanning.Path> _PathList = new ObservableCollection<FlightPlanning.Path>();

			//for (int i = 0; i < UAVRoute.Points.Count; i++)
			//{
			//	_PathList.Add(new FlightPlanning.Path()
			//	{
			//		Waypoint1 = i + 1,
			//		Waypoint2 = i + 2,
			//		Speed = 1,
			//		Shape = "Linear"
			//	});
			//}

			//_view.Source = _PathList;
			//this.PathListView.DataContext = _view;

			//// 自動調整
			//foreach (GridViewColumn column in (this.WaypointListView.View as GridView).Columns)
			//{
			//	column.Width = 0;
			//	column.Width = double.NaN;
			//}
		}
		// ルート追加
		private void PathListViewAdd()
		{
			//FlightPlanning.Path _PathList = new FlightPlanning.Path()
			//{
			//	Waypoint1 = UAVRoute.Points.Count,
			//	Waypoint2 = UAVRoute.Points.Count + 1,
			//	Speed = 1,
			//	Shape = "Linear"
			//};
			//this.PathListView.Items.Add(_PathList);

			////TODO
			//if (BaseMarker.Count == 1)
			//{
			//	ProjectList[0].PathData.Add(_PathList);
			//}
		}

		#endregion

		#region 路径追加

		#endregion

		//////////////////////////////////////////////////////////////////////////////////////
		// マーカ
        // 删除指定的标记
		private void MapToMarkerClear(GMapMarker _marker)
		{
			// 削除标记
			var clear = MainMap.Markers.Where(p => p == _marker);

			if (clear != null)
			{
				// 削除
				for (int i = 0; i < clear.Count(); i++)
				{
					MainMap.Markers.Remove(clear.ElementAt(i));
					i--;
				}
			}
		}

		// Waypoint&Path的追加
		private void WaypointAndPathAdd(System.Windows.Point _point, int _baseSelect)
		{
			try
			{
				PointLatLng Waypoint = new PointLatLng();							//Waypoint位置
				Waypoint = MainMap.FromLocalToLatLng((int)_point.X, (int)_point.Y); //地图上的位置

				int _id = ProjectList[BaseSelect].WaypointData.Count + 1;
				WaypointAdd(Waypoint, _id);											// Wapoint管理情報追加
				WaypointMarkerAdd(ProjectList[BaseSelect].WaypointData[_id - 1], _id);	// WaypointMarker追加
				WaypointPathAdd(ProjectList[BaseSelect].WaypointMarker[_id - 1]);	// Path追加

				if (ProjectList.Count > 0)
				{
                    // 切换到撤消（撤消）按钮可用
					UndoButton.IsEnabled = true;
				}

				//// 水平面的変換
				//PointLatLng OriginMeter = FlightPlanXML.LatLngPerMeter(ProjectList[BaseSelect].BaseMarker.Position);
				//double _ang = ProjectList[BaseSelect].MissionData.Rotation * Math.PI / 180;
				//double c = Math.Cos(_ang);
				//double s = Math.Sin(_ang);
				//double sx = ProjectList[BaseSelect].MissionData.ScaleX;
				//double sy = ProjectList[BaseSelect].MissionData.ScaleY;
				//double T11 = c * sx;
				//double T12 = -s * sy;
				//double T21 = s * sx;
				//double T22 = c * sy;
				//double det = 1 / (T11 * T22 - T12 * T21);

				//T11 *= det;
				//T12 *= det;
				//T21 *= det;
				//T22 *= det;

				//// 属性更新
				//WaypointValueUpdata(ProjectList[BaseSelect].WaypointMarker[_id - 1], OriginMeter, T11, T12, T21, T22);
			}
			catch
			{

			}

		}

        // 参考点的移动（航点/路径特派团整个运动）
		private PointLatLng BaseMove(GMapMarker _marker, PointLatLng _old)
		{
			PointLatLng _new = new PointLatLng();
			_new = _marker.Position;

			PointLatLng OriginMeter = FlightPlanXML.LatLngPerMeter(_marker.Position);

			// 水平面的変換
			double _ang = ProjectList[BaseSelect].MissionData.Rotation * Math.PI / 180;
			double c = Math.Cos(_ang);
			double s = Math.Sin(_ang);
			double sx = ProjectList[BaseSelect].MissionData.ScaleX; 
			double sy = ProjectList[BaseSelect].MissionData.ScaleY; 
			double T11 = c * sx;
			double T12 = -s * sy;
			double T21 = s * sx;
			double T22 = c * sy;

			// Waypoint全移動
			int i = 0;
			foreach (var _waypoint in ProjectList[BaseSelect].WaypointData)
			{
				double xx = T11 * (_waypoint.x) + T12 * (_waypoint.y);
				double yy = T21 * (_waypoint.x) + T22 * (_waypoint.y);

				_waypoint.Latitude = ProjectList[BaseSelect].MissionData.Latitude + xx / OriginMeter.Lat;
				_waypoint.Longitude = ProjectList[BaseSelect].MissionData.Longitude + yy / OriginMeter.Lng;
				//_waypoint.Height = _waypoint.Height * ProjectList[BaseSelect].MissionData.ScaleZ;

				ProjectList[BaseSelect].WaypointMarker[i].Position = new PointLatLng(_waypoint.Latitude, _waypoint.Longitude);
				ProjectList[BaseSelect].WaypointMarker[i].Azimuth =  ProjectList[BaseSelect].WaypointData[i].Heading + ProjectList[BaseSelect].MissionData.Rotation;
				ProjectList[BaseSelect].WaypointPath.Points[i] = ProjectList[BaseSelect].WaypointMarker[i].Position;
				
				//WaypointValueUpdata(ProjectList[BaseSelect].WaypointMarker[i]);
				i++;
			}

			//　地图上Path反映
			ProjectList[BaseSelect].WaypointPath.RegenerateShape(MainMap);

			return _new;
		}
		#endregion

        #region 进程间通信
        /// <summary>
		/// TCP/IP　非同期　通信
		/// </summary>

		// ManualResetEvent instances signal completion.
		private ManualResetEvent connectDone = new ManualResetEvent(false);
		private ManualResetEvent sendDone = new ManualResetEvent(false);
		private ManualResetEvent receiveDone = new ManualResetEvent(false);

		// The response from the remote device.
		private List<string> response = new List<string>();

		// 连接处理
		private void ConnectCallback(IAsyncResult ar)
		{
			try
			{
				// Retrieve the socket from the state object.
				Socket _client = (Socket)ar.AsyncState;

				// Complete the connection.
				_client.EndConnect(ar);

				Debug.WriteLine("Socket connected to " + _client.RemoteEndPoint.ToString());

				// Signal that the connection has been made.
				connectDone.Set();
			}
			catch (Exception e)
			{
				_logger.Error("ConnectCallback: " + e.Message);
			}
		}
        //异步数据的接收状态对象
		private class AsyncStateObject
		{
			public System.Net.Sockets.Socket Socket;
			public byte[] ReceiveBuffer;
			public System.IO.MemoryStream ReceivedData;

			public AsyncStateObject(System.Net.Sockets.Socket soc)
			{
				this.Socket = soc;
				this.ReceiveBuffer = new byte[1024];
				this.ReceivedData = new System.IO.MemoryStream();
			}
		}
        //接收开始
		private void Receive(Socket _client)
		{
			try
			{
				AsyncStateObject state = new AsyncStateObject(_client);

                //启动异步接收
				Debug.WriteLine("Receive-----------------------Open");
				_client.BeginReceive(state.ReceiveBuffer,
					0,
					state.ReceiveBuffer.Length,
					System.Net.Sockets.SocketFlags.None,
					new System.AsyncCallback(ReceiveCallback),
					state);

			}
			catch (Exception e)
			{
				_logger.Error("Receive: " + e.Message);
			}
		}
        // 接收回调
		private void ReceiveCallback(IAsyncResult ar)
		{
			try
			{
				AsyncStateObject state = (AsyncStateObject)ar.AsyncState;
				//Debug.WriteLine("ReceiveCallback");
                //读取长度的获取
				int len = 0;

				try
				{
					len = state.Socket.EndReceive(ar);
				}
				catch (System.ObjectDisposedException e)
				{
					//閉じた時
					//Debug.WriteLine("ReceiveCallback-----------------------Close");
					_logger.Error("ReceiveCallback-close: " + e.Message);
					return;
				}

				//连接状态
				if (len <= 0)
				{
					Debug.WriteLine("ReceiveCallback-----------------------Disconnect");
					
					Dispatcher.BeginInvoke((Action)(() =>
					{
						UAVConect.LargeImageSource = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/disconnect.png")));
						UAVConect.Label = "Disconnect";

						var clear = MainMap.Markers.Where(p => p != null && p == UAVMarker);
						if (clear != null)
						{
							for (int i = 0; i < clear.Count(); i++)
							{
								MainMap.Markers.Remove(clear.ElementAt(i));
								i--;
							}
						}
					}));
				
					state.Socket.Close();
					return;
				}

                state.ReceivedData.Write(state.ReceiveBuffer, 0, len);		//存储所接收的数据
				if (state.Socket.Available == 0)
				{
                    //接收到最后
                    //接收的数据转换为字符串
					string str = System.Text.Encoding.UTF8.GetString(state.ReceivedData.ToArray());
                    //response = str;		//接收数据
					if (str.IndexOf("\0") > 0)
					{
						System.Globalization.StringInfo si = new System.Globalization.StringInfo(str);
						int le = str.IndexOf("\0");

						response.Add(si.SubstringByTextElements(0, le));

					}
					//for (int i = 0; i < response.Count; i++)
					//{
					//	Debug.WriteLine("[" + i + "] response--->>>   " + response[i].Length + "   <<<---");
					//}
					// クリア
					state.ReceivedData.Close();
					state.ReceivedData = new System.IO.MemoryStream();
				}

				//开始接收
				state.Socket.BeginReceive(state.ReceiveBuffer,
					0,
					state.ReceiveBuffer.Length,
					System.Net.Sockets.SocketFlags.None,
					new System.AsyncCallback(ReceiveCallback),
					state);
			}
			catch (Exception e)
			{
				_logger.Error("ReceiveCallback: " + e.Message);
			}
		}
		// 发送处理
		private void Send(Socket _client, String data)
		{
			// Convert the string data to byte data using ASCII encoding.
			byte[] byteData = Encoding.UTF8.GetBytes(data);

			// Begin sending the data to the remote device.
			_client.BeginSend(byteData, 0, byteData.Length, 0, new AsyncCallback(SendCallback), _client);
		}
		private void SendCallback(IAsyncResult ar)
		{
			try
			{
				// Retrieve the socket from the state object.
				Socket client = (Socket)ar.AsyncState;

				// Complete sending the data to the remote device.
				int bytesSent = client.EndSend(ar);

				// Signal that all bytes have been sent.
				sendDone.Set();
			}
			catch (Exception e)
			{
				_logger.Error("SendCallback: " + e.Message);
			}
		}
        // 所接收的数据的从芯部完成判断
		private bool ReceiveDataUpdate()
		{
			if (response.Count > 0)
			{
				//DataConstellation.strXmlData = new List<string>();
				for (int i = 0; i < response.Count; i++)
				{
					if (!String.IsNullOrEmpty(response[i]))
					{

						if (response[i].Length > 0)
						{
							DataConstellation.strXmlData.Add(response[i]);		//　		
							Debug.WriteLine("[" + i + "]ReceiveDataUpdate:" + response[i].Length + "\r\n");
						}
					}
				}
                response.Clear();	//　清除接收数据

				return true;
			}
			else
			{
				return false;
			}
		}
		// UAV连接
		private void UAVConect_Click(object sender, RoutedEventArgs e)
		{
			if (Client.Connected)
			{
				UAVConect.LargeImageSource = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/disconnect.png")));
				UAVConect.Label = "连接失败";

                // 退出核心部分（关闭消息）
				string text = DataConstellation.CloseMassage(XmlMessage.Get_UAVData);
				Send(Client, text);
				sendDone.WaitOne();
                // 退出核心部分（执行核心部分结束蝙蝠）

                // 释放套接字。
				Client.Shutdown(SocketShutdown.Both);
				Client.Close();

				var clear = MainMap.Markers.Where(p => p != null && p == UAVMarker);
				if (clear != null)
				{
					for (int i = 0; i < clear.Count(); i++)
					{
						MainMap.Markers.Remove(clear.ElementAt(i));
						i--;
					}
				}
			}
			else
			{
				if (ConnectLoop())
				{
                    // 连接成功
					UAVConect.LargeImageSource = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/connect.png")));
					UAVConect.Label = "连接成功";
				
					#region コア部初期COMポート番号の取得
					// コア部初期COMポート番号の取得
					string text = DataConstellation.ReadMassage(XmlMessage.Get_Initial);
					//Debug.WriteLine("COM Port： " + text + "\n");
					Send(Client, text);
					sendDone.WaitOne(); //送信待機

					Receive(Client);

					while (!ReceiveDataUpdate()) ;

					SettingData.SerialPortSetting = "COM" + DataConstellation.GetMassage("Config/COM/Port", false);		//受信データを削除しない
					DataConstellation.BindingData.FilePath = DataConstellation.GetMassage("Config/FlightLog/FilePath");

					Debug.WriteLine("COM Port：" + SettingData.SerialPortSetting);
					Debug.WriteLine("FilePath：" + DataConstellation.BindingData.FilePath);

					text = DataConstellation.CloseMassage(XmlMessage.Get_Initial);
					Send(Client, text);
					sendDone.WaitOne();

					#endregion

                    // 初始得到的消息
					string data = DataConstellation.ReadMassage(XmlMessage.Get_UAVData);
					DataConstellation.XMLgetsize = data.ToList().Where(c => c.Equals('\n')).Count() + 1;
					Send(Client, data);
					sendDone.WaitOne();

					DataConstellation.CommandMode = 1;				//初始値

					// 現在经度纬度/方位角　UAVmarker
					UAVMarker = new GMapMarker(new PointLatLng());
					{
						UAVMarker.Shape = new Position(UAVMarker);
						UAVMarker.Offset = new System.Windows.Point(-15, -25);
						UAVMarker.ZIndex = int.MaxValue;
						MainMap.Markers.Add(UAVMarker);
					}
				}
				else
				{
					// 连接失败时
					UAVConect.LargeImageSource = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/disconnect.png")));
					UAVConect.Label = "Disconnect";
				}
			}
		}
		// 连接确认
		private bool ConnectLoop()
		{
			try
			{
				//　通信初始值设置
				string ad = Properties.Settings.Default.CoreIPaddress;
				int port = Properties.Settings.Default.CorePort;

				Debug.WriteLine("IPAddress:" + ad + "   Port:" + port);

				IPEndPoint remoteEP = new IPEndPoint(IPAddress.Parse(ad), port);
				Client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

				// Connect to the remote endpoint.
				Client.BeginConnect(remoteEP, new AsyncCallback(ConnectCallback), Client);
				if (!connectDone.WaitOne(1000))
				{
					Client.Close();
					//EndFlag = false;
					return false;
				}

				Debug.WriteLine("ConnectLoop" + Client.Connected);
				return true;
			}
			catch (Exception ex)
			{
				//System.Windows.MessageBox.Show("(301) Massage Receive Error-2: " + ex.Message);
				_logger.Error("(301) Massage Receive Error-2: " + ex.Message);
				//EndFlag = false;
				return false;
			}

		}

		//------------
        // 在启动时接收信号电报事件过程的时间
        // 第１引数: 事件过程的调用对象
        // 第２引数: 命令请求到服务器（命令）
        // 第３引数: 从服务器发送回复
		public void PipeClient_ReceiveCall(object sender, DateTime Event_Time, string Reply_Message)
		{

			//// 結果表示
            //label1.Text = Event_Time.ToString() + ": 从服务器接收回复。\n\r";


            //// 字符阅读标签上显示
			//label1.Text += "受信内容: *** " + Reply_Message + " ***";


            //// 启用按钮
			//button1.IsEnabled = true;

		}
		#endregion

        #region 地图显示屏幕事件
        //当鼠标移动屏幕显示地图
		private bool MarkerMove = false;
		private bool BaseMarkerMove = false;
        // 鼠标操作
		private void MainMapMove(System.Windows.Point _p)
		{
			if (ProjectList.Count > 0)
			{
				#region
				if (ProjectList[BaseSelect].WaypointMarker.Count > 0)
				{
					PointLatLng _point = MainMap.FromLocalToLatLng((int)_p.X, (int)_p.Y);
					var _maker = ProjectList[BaseSelect].WaypointMarker.Where(p => p.Position == _point);
					var i = ProjectList[BaseSelect].WaypointMarker.Select(p => p.Position == _point);
				}
				//foreach (var _project in ProjectList)
				//{
				//	if (_project.BaseMarker.IsMouseOver)
				//	{
				//		BaseSelect = _project.BaseMarker.ID - 1;
				//	}
				//}
				// Base
				if (ProjectList[BaseSelect].BaseMarker.IsMouseOver == true)
				{
					MarkerMove = true;//移動
					// BaseMarker移動
					ProjectList[BaseSelect].OldBasePoint = BaseMove(ProjectList[BaseSelect].BaseMarker, ProjectList[BaseSelect].OldBasePoint);
					BaseMarkerMove = true;
					BasePointValueChanged(ProjectList[BaseSelect].BaseMarker);

                    // 选择的连接任务
					if (ProjectList.Count > 1)
					{
						if (BaseSelect != -1)
						{
							int _connectID = ProjectList[BaseSelect].MissionData.ConnectID;
							int _accessID = ProjectList[BaseSelect].MissionData.AccessID;

							if (_connectID != 0)
							{
								_connectID = ProjectList[BaseSelect].MissionData.ConnectID - 1;
								var _waypoint1 = ProjectList[_connectID].WaypointData.Last();
								var _waypoint2 = ProjectList[BaseSelect].WaypointData.First();
								ProjectList[_connectID].ProjectPath.Points[0] = new PointLatLng(_waypoint1.Latitude, _waypoint1.Longitude);
								ProjectList[_connectID].ProjectPath.Points[1] = new PointLatLng(_waypoint2.Latitude, _waypoint2.Longitude);
								ProjectList[_connectID].ProjectPath.RegenerateShape(MainMap);
							}
							if (_accessID != 0)
							{
								var _waypoint1 = ProjectList[BaseSelect].WaypointData.Last();
								var _waypoint2 = ProjectList[missionConnect.SelectedIndex].WaypointData.First();
								ProjectList[missionSelect.SelectedIndex].ProjectPath.Points[0] = new PointLatLng(_waypoint1.Latitude, _waypoint1.Longitude);
								ProjectList[missionSelect.SelectedIndex].ProjectPath.Points[1] = new PointLatLng(_waypoint2.Latitude, _waypoint2.Longitude);
								ProjectList[missionSelect.SelectedIndex].ProjectPath.RegenerateShape(MainMap);
							}
						}
					}
				}
				#endregion

				#region
				// WaypointSelect.Clear();
				for (int i = 0; i < ProjectList[BaseSelect].WaypointMarker.Count; i++)
				{
                    // 現在是当前鼠标（触摸）操作
					if (ProjectList[BaseSelect].WaypointMarker[i].IsMouseOver == true)
					{
                        // Waypoint与位置变化相关的路径变化
						if (ProjectList[BaseSelect].WaypointMarker.Count > 0)
						{
							MarkerMove = true;//移動
							ProjectList[BaseSelect].WaypointPath.Points[i] = ProjectList[BaseSelect].WaypointMarker[i].Position;        //　Waypoint位置和Path位置的反映
							ProjectList[BaseSelect].WaypointPath.RegenerateShape(MainMap);												//　地图上显示Path反映
						}

						if (Keyboard.Modifiers == ModifierKeys.Control)
						{
                            // TODO：在航点更改选择性高度（政策呢？要高度变化的同时按下或Ctrl只有在高度已经改变）
							ProjectList[BaseSelect].WaypointMarker[i].Altitude = 0;// Altitude.WaypointAlt;
						}


						if (ProjectList.Count > 1)
						{
							if (BaseSelect != -1)
							{
								int _connectID = ProjectList[BaseSelect].MissionData.ConnectID;
								int _accessID = ProjectList[BaseSelect].MissionData.AccessID;

								if (_connectID != 0)
								{
									_connectID = ProjectList[BaseSelect].MissionData.ConnectID - 1;
									if (i == 0)
									{
										var _waypoint1 = ProjectList[_connectID].WaypointData.Last();
										var _waypoint2 = ProjectList[BaseSelect].WaypointData.First();
										ProjectList[_connectID].ProjectPath.Points[0] = new PointLatLng(_waypoint1.Latitude, _waypoint1.Longitude);
										ProjectList[_connectID].ProjectPath.Points[1] = ProjectList[BaseSelect].WaypointMarker[i].Position;
										ProjectList[_connectID].ProjectPath.RegenerateShape(MainMap);
									}
								}
								if (_accessID != 0)
								{
									if (i == (ProjectList[BaseSelect].WaypointMarker.Count - 1))
									{
										var _waypoint1 = ProjectList[BaseSelect].WaypointData.Last();
										var _waypoint2 = ProjectList[missionConnect.SelectedIndex].WaypointData.First();
										ProjectList[BaseSelect].ProjectPath.Points[0] = ProjectList[BaseSelect].WaypointMarker[i].Position;
										ProjectList[BaseSelect].ProjectPath.Points[1] = new PointLatLng(_waypoint2.Latitude, _waypoint2.Longitude);
										ProjectList[BaseSelect].ProjectPath.RegenerateShape(MainMap);
									}

								}
							}
						}

					}
				}
				#endregion				
			}
		}
        // 触摸操作
		private void MainMapMoveT(System.Windows.Point _p)
		{
			if (ProjectList.Count > 0)
			{

				#region
				////////////////////////////////////////////////////////////0
				// Waypoint操作
				if (WaypointSelect.Count > 0)
				{
					int i = WaypointSelect[0] - 1;


					// 現在是触摸操作
					if (ProjectList[BaseSelect].WaypointMarker[i].IsMouseOver == true)
					{
						MainMap.Mode = -1;
                        // 与航点的位置变化相关的路径变化
						if (ProjectList[BaseSelect].WaypointMarker.Count > 0)
						{
							MarkerMove = true;//移動
							ProjectList[BaseSelect].WaypointPath.Points[i] = ProjectList[BaseSelect].WaypointMarker[i].Position;        //　Waypoint位置をPath位置に反映
							ProjectList[BaseSelect].WaypointPath.RegenerateShape(MainMap);												//　地図上にPath反映
						}

						if (ProjectList.Count > 1)
						{
							if (BaseSelect != -1)
							{
								int _connectID = ProjectList[BaseSelect].MissionData.ConnectID;
								int _accessID = ProjectList[BaseSelect].MissionData.AccessID;

								if (_connectID != 0)
								{
									_connectID = ProjectList[BaseSelect].MissionData.ConnectID - 1;
									if (i == 0)
									{
										var _waypoint1 = ProjectList[_connectID].WaypointData.Last();
										var _waypoint2 = ProjectList[BaseSelect].WaypointData.First();
										ProjectList[_connectID].ProjectPath.Points[0] = new PointLatLng(_waypoint1.Latitude, _waypoint1.Longitude);
										ProjectList[_connectID].ProjectPath.Points[1] = ProjectList[BaseSelect].WaypointMarker[i].Position;
										ProjectList[_connectID].ProjectPath.RegenerateShape(MainMap);
									}
								}
								if (_accessID != 0)
								{
									if (i == (ProjectList[BaseSelect].WaypointMarker.Count - 1))
									{
										var _waypoint1 = ProjectList[BaseSelect].WaypointData.Last();
										var _waypoint2 = ProjectList[missionConnect.SelectedIndex].WaypointData.First();
										ProjectList[BaseSelect].ProjectPath.Points[0] = ProjectList[BaseSelect].WaypointMarker[i].Position;
										ProjectList[BaseSelect].ProjectPath.Points[1] = new PointLatLng(_waypoint2.Latitude, _waypoint2.Longitude);
										ProjectList[BaseSelect].ProjectPath.RegenerateShape(MainMap);
									}
								}
							}
						}

					}
				}
				//// WaypointSelect.Clear();
				//for (int i = 0; i < ProjectList[BaseSelect].WaypointMarker.Count; i++)
				//{

				//}
				#endregion
				#region 基准点
				////////////////////////////////////////////////////////////0
				// Base操作
				if (ProjectList[BaseSelect].BaseMarker.IsMouseOver == true)
				{
					MarkerMove = true;//移動
					MainMap.Mode = -1;
					// BaseMarker移動
					ProjectList[BaseSelect].OldBasePoint = BaseMove(ProjectList[BaseSelect].BaseMarker, ProjectList[BaseSelect].OldBasePoint);
					BaseMarkerMove = true;
					BasePointValueChanged(ProjectList[BaseSelect].BaseMarker);

                    //选定的连接任务
					if (ProjectList.Count > 1)
					{
						if (BaseSelect != -1)
						{
							int _connectID = ProjectList[BaseSelect].MissionData.ConnectID;
							int _accessID = ProjectList[BaseSelect].MissionData.AccessID;

							if (_connectID != 0)
							{
								_connectID = ProjectList[BaseSelect].MissionData.ConnectID - 1;
								var _waypoint1 = ProjectList[_connectID].WaypointData.Last();
								var _waypoint2 = ProjectList[BaseSelect].WaypointData.First();
								ProjectList[_connectID].ProjectPath.Points[0] = new PointLatLng(_waypoint1.Latitude, _waypoint1.Longitude);
								ProjectList[_connectID].ProjectPath.Points[1] = new PointLatLng(_waypoint2.Latitude, _waypoint2.Longitude);
								ProjectList[_connectID].ProjectPath.RegenerateShape(MainMap);
							}
							if (_accessID != 0)
							{
								var _waypoint1 = ProjectList[BaseSelect].WaypointData.Last();
								var _waypoint2 = ProjectList[missionConnect.SelectedIndex].WaypointData.First();
								ProjectList[missionSelect.SelectedIndex].ProjectPath.Points[0] = new PointLatLng(_waypoint1.Latitude, _waypoint1.Longitude);
								ProjectList[missionSelect.SelectedIndex].ProjectPath.Points[1] = new PointLatLng(_waypoint2.Latitude, _waypoint2.Longitude);
								ProjectList[missionSelect.SelectedIndex].ProjectPath.RegenerateShape(MainMap);
							}
						}
					}
				}
				#endregion

			}
		}
		private void MainMap_MouseMove(object sender, System.Windows.Input.MouseEventArgs e)
		{
			try
			{
				if ((e.LeftButton == MouseButtonState.Pressed)/* && (SettingData.DeviceMode == true)*/)
				{
					MainMapMove(e.GetPosition(MainMap));
					Debug.WriteLine("MainMap_MouseMove" + e.GetPosition(MainMap).X.ToString() + "/" + e.GetPosition(MainMap).Y.ToString());
				}
			}
			catch
			{

			}

		}
		private void MainMap_TouchMove(object sender, TouchEventArgs e)
		{
			try
			{
				#region
				if (MultiTouchControl.TouchCount == 0)
				{
					//if ((SettingData.DeviceMode == false))
					{
						///////////////////////////////////////////////////////////////////////////////////
						// Marker操作
						if (ProjectList.Count > 0)
						{
							TouchPoint _p = e.TouchDevice.GetTouchPoint(MainMap);
							System.Windows.Point _pp = new System.Windows.Point(_p.Position.X, _p.Position.Y);
							MainMapMoveT(_pp);

							Debug.WriteLine("MainMap_TouchMove" + _pp.X.ToString() + "/" + _pp.Y.ToString());

							//MainMapMove(_pp);

							//if (ProjectList[BaseSelect].BaseMarker.IsMouseOver)
							//{
							//	MainMap.Mode = -1;
							//	return;
							//}
							//if (ProjectList[BaseSelect].WaypointMarker.Count > 0 && WaypointSelect.Count > 0)
							//{
							//	if (ProjectList[BaseSelect].WaypointMarker[WaypointSelect[0] - 1].IsMouseOver)
							//	{
							//		MainMap.Mode = -1;
							//		return;
							//	}
							//}
						}
					}
				}
				else
				{
					///////////////////////////////////////////////////////////////////////////////////
					// 地图操作
					TouchPoint AreaP = e.GetTouchPoint(GridLine);
					TouchPoint p = e.GetTouchPoint(this);

					if (MultiTouchControl.TouchID1 == e.TouchDevice.Id)
					{
						MultiTouchControl.TouchPonit1 = p.Position;
					}

					if (MultiTouchControl.TouchID2 == e.TouchDevice.Id)
					{
						MultiTouchControl.TouchPonit2 = p.Position;

						if (MultiTouchControl.MultiTouchdDtection())
						{
							MultiTouchControl.MultiTouchT();

                            // 多触摸模式选择
							if (MultiTouchControl.Angle != 0)
							{
								//////////////////////////////////////////////////////////////////////////
                                // 旋转
								MapRotate.Angle += MultiTouchControl.Angle;
								MainMap.Mode = 1;
							}
							else if (MultiTouchControl.Distance != 0)
							{
								//////////////////////////////////////////////////////////////////////////
								// 允许扩大和缩小
                                ZoomName.Value += MultiTouchControl.Distance / (ZoomName.Value + 100);     // <---地图缩放比例调整

                                //距离网格
								GPoint pe = MainMap.FromLatLngToLocal(MainMap.Position);
								PointLatLng Point = MainMap.FromLocalToLatLng((int)pe.X + GRID_SIZE, (int)pe.Y);
								double dis = Math.Round(MainMap.MapProvider.Projection.GetDistance(MainMap.Position, Point), 4, MidpointRounding.AwayFromZero);
								dis *= 1000;// km -> m
								DisName.Content = dis.ToString() + " m";
								MainMap.Mode = 2;
							}
							MultiTouchControl.MultiTouchFlag = false;
						}
					}
				}
				#endregion
			}
			catch
			{

			}
		}

        // 地图显示屏幕，当鼠标向上
		private void MainMapUp()
		{
            // 功能区 - 地图集 - 经纬度显示更新
			LatText.Text = MainMap.Position.Lat.ToString();
			LngText.Text = MainMap.Position.Lng.ToString();

			//if (Keyboard.Modifiers != ModifierKeys.Control)
			//{
			//	WaypointSelect.Clear();
			//}

			if (ProjectList.Count < 1)
			{
				return;
			}

			// 更新
			if (ProjectList.Count > 0)
			{
                // Mission属性显示更新
				MissionProperty.SelectedObject = ProjectList[BaseSelect].MissionData;

				// MissionVew表示更新  
				BasePointViewUpdata(BaseSelect);

				if (ProjectList[BaseSelect].WaypointMarker.Count > 0)
				{
					// 水平面的变换
					PointLatLng OriginMeter = FlightPlanXML.LatLngPerMeter(ProjectList[BaseSelect].BaseMarker.Position);
					double _ang = ProjectList[BaseSelect].MissionData.Rotation * Math.PI / 180;
					double c = Math.Cos(_ang);
					double s = Math.Sin(_ang);
					double sx = ProjectList[BaseSelect].MissionData.ScaleX;
					double sy = ProjectList[BaseSelect].MissionData.ScaleY;
					double T11 = c * sx;
					double T12 = -s * sy;
					double T21 = s * sx;
					double T22 = c * sy;
					double det = 1 / (T11 * T22 - T12 * T21);

					T11 *= det;
					T12 *= det;
					T21 *= det;
					T22 *= det;

					// 选择多个Marka
					for (int i = 0; i < ProjectList[BaseSelect].WaypointMarker.Count; i++)
					{
						if (Keyboard.Modifiers != ModifierKeys.Control)
						{
							if (WaypointSelect.Count > 0)
							{
								if (WaypointSelect[0] != ProjectList[BaseSelect].WaypointMarker[i].ID)
								{
									ProjectList[BaseSelect].WaypointMarker[i].Mode = -1;
								}
							}
							else
							{
								ProjectList[BaseSelect].WaypointMarker[i].Mode = -1;
							}

						}

                        // 属性更新
						WaypointValueUpdata(ProjectList[BaseSelect].WaypointMarker[i], OriginMeter, T11, T12, T21, T22);
					}
                    // Waypoint属性更新
					if (WaypointSelect.Count > 0)
					{
						WaypointProperty.SelectedObject = null;
						if (WaypointSelect.Count > 0)
						{
							WaypointProperty.SelectedObject = ProjectList[BaseSelect].WaypointData[WaypointSelect[0] - 1];    //プロパティ表示
						}
						else
						{
							WaypointProperty.SelectedObject = ProjectList[BaseSelect].WaypointData[0];    //プロパティ表示
						}
						// 
						WaypointValueChanged(BaseSelect);
					}

					// 選択中Waypoint
					if (WaypointSelect.Count > 0)
					{
						WaypointListView.SelectedIndex = WaypointSelect[0] - 1;
					}
				}

			}
			MarkerMove = false;//移動
			BaseMarkerMove = false;
		}
		private void MainMap_MouseUp(object sender, MouseButtonEventArgs e)
		{
			try
			{
				MainMapUp();
			}
			catch
			{

			}
		}
		private void MainMap_TouchUp(object sender, TouchEventArgs e)
		{
			try
			{
				MultiTouchControl.TouchCount -= 1;
				if (MultiTouchControl.TouchCount < 1)
				{
                    MultiTouchControl.MultiTouchdInit();   //多点触控初始化
				}
				else if (MultiTouchControl.TouchCount < 2)
				{
                    MultiTouchControl.MultiTouchdInit();   //多点触控初始化
				}
				if (MultiTouchControl.TouchCount == 0)
				{
					MainMapUp();
				}

				MainMap.Mode = 0;
			}
			catch
			{

			}

		}

        // 在地图上显示画面按下鼠标
		private GPoint MainMapDown()
		{
			GPoint _p = new GPoint();
			try
			{

				if (Keyboard.Modifiers != ModifierKeys.Control)
				{
					WaypointSelect.Clear();
				}

				// TODO
                // 搜索较早选择
                // 然后调整未被选中的
				if (ProjectList.Count > 0)
				{
					int k = 0;
					foreach (var _project in ProjectList)
					{
						if (_project.BaseMarker.IsMouseOver)					//BaseMarker-MouseOver
						{
							BaseSelect = k;										//切替
							ProjectList[k].BaseMarker.Mode = 1;

							// MissionView
							BasePointViewUpdata(BaseSelect);

							WaypointValueChanged(BaseSelect);

							ProjectRecord.Add(ProjectDeepCopy(ProjectList));	// ProjectRecord追加
							_p = MainMap.FromLatLngToLocal(_project.BaseMarker.Position);
							break;
						}

						// Waypoint
						foreach (GMapMarker _marker in _project.WaypointMarker)
						{
							if (_marker.IsMouseOver)							//Waypoint-MouseOver
							{
                                WaypointSelect.Add(_marker.ID);					//选择航点
								BaseSelect = k;									//切替
								ProjectList[k].BaseMarker.Mode = 1;

								// MissionView
								BasePointViewUpdata(BaseSelect);

								ProjectRecord.Add(ProjectDeepCopy(ProjectList));// ProjectRecord追加

								_p = MainMap.FromLatLngToLocal(_project.BaseMarker.Position);
								break;
							}
						}
						k++;
					}

					k = 0;
					foreach (var _project in ProjectList)
					{
						if (BaseSelect != k)
						{
							// Base
							ProjectList[k].BaseMarker.Mode = -1;
						}

						// Waypoint
						for (int i = 0; i < _project.WaypointMarker.Count; i++)
						{
							if (WaypointSelect.Count > 0 && WaypointSelect.Count <= 1)
							{
								if (WaypointSelect[0] != _project.WaypointMarker[i].ID)
								{
									ProjectList[k].WaypointMarker[i].Mode = -1;
								}
								else
								{
                                    // 高度栏显示切换
									if (AltitudePropertyValue.AltitudeList.Count == _project.WaypointMarker.Count)
									{
										AltitudePropertyValue.AltitudeList[i] = _project.WaypointMarker[i].Altitude;
									}
								}
							}
						}
						k++;
					}
				}

			}
			catch
			{

			}

			return _p;
		}
		private void MainMap_MouseDown(object sender, MouseButtonEventArgs e)
		{
			try
			{
				MainMapDown();
			}
			catch
			{

			}
		}
		private void MainMap_TouchDown(object sender, TouchEventArgs e)
		{
			try
			{
				TouchPoint p = e.GetTouchPoint(this);

				double dis = 9999;

				if (ProjectList.Count > 0)
				{
					TouchPoint p1 = e.GetTouchPoint(MainMap);
					System.Windows.Point p2 = new System.Windows.Point();
					GPoint p3 = MainMapDown();

					p2.X = (p1.Position.X - p3.X);
					p2.Y = (p1.Position.Y - p3.Y);
					dis = Math.Sqrt((p2.X * p2.X) + (p2.Y * p2.Y));

				}

				oldp = p.Position;	//初始化
				if (dis > 1000)
				{
					if (MultiTouchControl.TouchCount == 0)
					{
						MultiTouchControl.InitTouchPonit = p.Position;
						MultiTouchControl.TouchID1 = e.TouchDevice.Id;
						MultiTouchControl.TouchPonit1 = p.Position;

					}
					else if (MultiTouchControl.TouchCount == 1)
					{
						MultiTouchControl.TouchID2 = e.TouchDevice.Id;
						MultiTouchControl.TouchPonit2 = p.Position;
					}
					MultiTouchControl.TouchCount++;
				}
				else
				{
					MainMap.Mode = -1;
				}
			}
			catch
			{

			}
			
		}

        // 目标高度栏事件的ValueChanged
		private void Altitude_TargetValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			try
			{
				if (ProjectList.Count > 0)
				{
					if (ProjectList[BaseSelect].WaypointMarker.Count > 0)
					{
						// 水平面的変換
						PointLatLng OriginMeter = FlightPlanXML.LatLngPerMeter(ProjectList[BaseSelect].BaseMarker.Position);
						double _ang = ProjectList[BaseSelect].MissionData.Rotation * Math.PI / 180;
						double c = Math.Cos(_ang);
						double s = Math.Sin(_ang);
						double sx = ProjectList[BaseSelect].MissionData.ScaleX;
						double sy = ProjectList[BaseSelect].MissionData.ScaleY;
						double T11 = c * sx;
						double T12 = -s * sy;
						double T21 = s * sx;
						double T22 = c * sy;
						double det = 1 / (T11 * T22 - T12 * T21);

						T11 /= det;
						T12 /= det;
						T21 /= det;
						T22 /= det;

						for (int i = 0; i < ProjectList[BaseSelect].WaypointMarker.Count; i++)
						{
							if (WaypointSelect.Count > 0)
							{
								foreach (var _id in WaypointSelect)
								{
									if (_id == ProjectList[BaseSelect].WaypointMarker[i].ID)
									{
                                        // 高度栏显示切换
										if (AltitudePropertyValue.AltitudeList.Count == ProjectList[BaseSelect].WaypointMarker.Count)
										{
											ProjectList[BaseSelect].WaypointMarker[i].Altitude = AltitudePropertyValue.AltitudeList[i];
										}
										//ProjectList[BaseSelect].WaypointMarker[i].Altitude = Altitude.WaypointAlt;
									}
								}
								WaypointValueUpdata(ProjectList[BaseSelect].WaypointMarker[i], OriginMeter, T11, T12, T21, T22);
							}
						}
						if (WaypointSelect.Count > 0)
						{
							WaypointProperty.SelectedObject = null;
                            WaypointProperty.SelectedObject = ProjectList[BaseSelect].WaypointData[WaypointSelect[0] - 1];    //属性显示
						}
						WaypointValueChanged(BaseSelect);
					}
				}
			}
			catch
			{

			}
			
		}

		private System.Windows.Point oldp = new System.Windows.Point();

		private void Main_TouchUp(object sender, TouchEventArgs e)
		{
			try
			{
				MultiTouchControl.TouchCount -= 1;
				if (MultiTouchControl.TouchCount < 1)
				{
                    MultiTouchControl.MultiTouchdInit();   //多点触控初始化
				}
				else if (MultiTouchControl.TouchCount < 2)
				{
                    MultiTouchControl.MultiTouchdInit();   //多点触控初始化
				}
				if (MultiTouchControl.TouchCount == 0)
				{
					MainMapUp();
				}

				MainMap.Mode = 0;
			}
			catch
			{

			}

		}

        // UAVMarka更改为中心
		private void MarkerIcon_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
            //设置地图的参考坐标
			if (Client.Connected)
			{
				MainMap.Position = UAVMarker.Position;
			}
		}
		#endregion

        #region 航点绘图功能（自选）
        void MainMap_MouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			//if (ProjectList.Count != 0)
			//{
			//	if (ProjectList[BaseSelect].WaypointData.Count >= Constants._maxMultiWaypoint)
			//	{
			//		System.Windows.MessageBox.Show("(000) ウェイポイントの設置限度数を超えています。");
			//		return;
			//	}

			//	if (DrawMode != 0)
			//	{
			//		if (PointCount == 0)
			//		{
			//			p1 = new System.Windows.Point();
			//			p1 = e.GetPosition(MainMap);
			//			//WaypointDraw(DrawMode, e.LeftButton, p1, p2);
			//			PointCount = 1;
			//		}
			//		else if (PointCount >= 1)
			//		{

			//			if (ProjectList[BaseSelect].BaseMarker.IsMouseOver == true)
			//			{
			//				return;
			//			}
			//			for (int i = 0; i < ProjectList[BaseSelect].WaypointMarker.Count; i++)
			//			{
			//				// 現在マウス（タッチ）操作しているもの
			//				if (ProjectList[BaseSelect].WaypointMarker[i].IsMouseOver == true)
			//				{
			//					return;
			//				}
			//			}
			//			p2 = new System.Windows.Point();
			//			p2 = e.GetPosition(MainMap);
			//			WaypointDraw(DrawMode, e.LeftButton, p1, p2);
			//			p1 = p2;
			//			PointCount++;
			//		}
			//		else
			//		{
			//			PointCount = 0;
			//		}

			//	}
			//}
		}
		private void AutoWaypointDraw_Checked(object sender, RoutedEventArgs e)
		{
			var radioButton = (RadioButton)sender;
			//this.textBlockSelected.Text = radioButton.Content.ToString();
			string _str = radioButton.Content.ToString();

			List<string> _list = new List<string>();
			_list.Add((string)Radio_StandardDraw.Content);
			_list.Add((string)Radio_LineDraw.Content);
			_list.Add((string)Radio_RectangleDraw.Content);
			_list.Add((string)Radio_CircleDraw.Content);
			_list.Add((string)Radio_ReturnDraw1.Content);
			_list.Add((string)Radio_ReturnDraw2.Content);

			for (int i = 0; i < _list.Count; i++)
			{
				if (_str.IndexOf(_list[i]) != -1)
				{
					DrawMode = i;

					if (DrawMode == 3)
					{
						NumberOfPartitionsValue1.Value = 45;
					}
					else
					{
						NumberOfPartitionsValue1.Value = 2;
					}
					PointCount = 0;
					return;
				}
			}
		}
        // 绘图模式选择
		private void WaypointDraw(int _mode, MouseButtonState _bState, System.Windows.Point _p1, System.Windows.Point _p2)
		{
			try
			{
				switch (_mode)
				{
					case 0:
						// 标准
						if (PointCount == 0) StandardDraw(_p1, _bState);
						break;

					case 1:
						// 线
						if (PointCount >= 1) LineDraw(_p1, _p2, (int)NumberOfPartitionsValue1.Value, true);
						break;

					case 2:
						// 四角
						if (PointCount >= 1) RectangleDraw(_p1, _p2, (int)NumberOfPartitionsValue1.Value, (int)NumberOfPartitionsValue2.Value);
						break;

					case 3:
						// 圆
						if (PointCount >= 1) CircleDraw(_p1, _p2, (int)NumberOfPartitionsValue1.Value);
						break;

					case 4:
						// 往复1
						if (PointCount >= 1) ReturnDraw1(_p1, _p2, (int)NumberOfPartitionsValue1.Value, (int)NumberOfPartitionsValue2.Value);
						break;

					case 5:
						// 往復2
						if (PointCount >= 1) ReturnDraw2(_p1, _p2, (int)NumberOfPartitionsValue1.Value, (int)NumberOfPartitionsValue2.Value);
						break;

					default:
						break;
				}
				AltitudePropertyValue.AltitudeUpdate();
				ProjectRecord.Add(ProjectDeepCopy(ProjectList));
				WaypointValueChanged(BaseSelect);
			}
			catch
			{

			}
			
		}
		// 标准描画
		private void StandardDraw(System.Windows.Point _p, MouseButtonState _bState)
		{
			if (Keyboard.Modifiers != ModifierKeys.Control)
			{
				WaypointSelect.Clear();
			}
			if ((_bState == MouseButtonState.Pressed) /*&& (SettingData.DeviceMode == true)*/)
			{
				if (ProjectList.Count > 0)
				{
					WaypointAndPathAdd(_p, BaseSelect);		// Waypoint追加
				}
			}

			if (ProjectList[BaseSelect].WaypointMarker.Count > 0)
			{
				for (int i = 0; i < ProjectList[BaseSelect].WaypointMarker.Count; i++)
				{
					if (WaypointSelect.Count > 0)
					{
						if (WaypointSelect[0] != ProjectList[BaseSelect].WaypointMarker[i].ID)
						{
							ProjectList[BaseSelect].WaypointMarker[i].Mode = -1;
						}
					}
				}
			}
		}
		// 线描画
		private List<System.Windows.Point> LineDraw(System.Windows.Point _p1, System.Windows.Point _p2, int _c, bool _f)
		{
			List<System.Windows.Point> _pointList = new List<System.Windows.Point>();
			_pointList.Add(_p1);

			double _x = (_p1.X - _p2.X);
			double _y = (_p1.Y - _p2.Y);
			_x /= _c;
			_y /= _c;

			for (int i = 1; i < _c; i++)
			{
				System.Windows.Point _p = new System.Windows.Point();

				_p.X = _p1.X - (_x * i);
				_p.Y = _p1.Y - (_y * i);

				_pointList.Add(_p);
			}
			_pointList.Add(_p2);

			if (_f)
			{
				int k = 0;
				if (PointCount > 1)
				{
					k = 1;
				}
				for (int i = k; i < _pointList.Count; i++)
				{
					WaypointAndPathAdd(_pointList[i], BaseSelect);		// Waypoint追加
				}
			}


			return _pointList;
		}
		// 圆描画
		private void CircleDraw(System.Windows.Point _p1, System.Windows.Point _p2, int _ang)
		{
			List<System.Windows.Point> _pointList = new List<System.Windows.Point>();
			int _r = (int)Math.Sqrt((_p2.X - _p1.X) * (_p2.X - _p1.X) + (_p2.Y - _p1.Y) * (_p2.Y - _p1.Y));

			int _oldCount = ProjectList[BaseSelect].WaypointMarker.Count();

			for (int i = 0; i <= 360; i += _ang)
			{
				double angle = Math.PI * (i - 90) / 180.0;
				double _x = _r * Math.Cos(angle);
				double _y = _r * Math.Sin(angle);

				_pointList.Add(new System.Windows.Point(_p1.X + _x, _p1.Y + _y));
			}
			for (int i = 0; i < _pointList.Count; i++)
			{
				WaypointAndPathAdd(_pointList[i], BaseSelect);				// Waypoint追加

				//
				int k = _oldCount + i;
				ProjectList[BaseSelect].WaypointData[k].Heading =(i * _ang) -180;
				ProjectList[BaseSelect].WaypointMarker[k].Azimuth = (i * _ang) - 180;
			}
		}
		// 四角描画
		private void RectangleDraw(System.Windows.Point _p1, System.Windows.Point _p2, int _c1, int _c2)
		{
			List<System.Windows.Point> _pointList = new List<System.Windows.Point>();

			System.Windows.Point _pp1 = new System.Windows.Point(_p1.X, _p1.Y);
			System.Windows.Point _pp2 = new System.Windows.Point(_p2.X, _p1.Y);
			System.Windows.Point _pp3 = new System.Windows.Point(_p2.X, _p2.Y);
			System.Windows.Point _pp4 = new System.Windows.Point(_p1.X, _p2.Y);

			//_pointList.Add(_pp1);
			//_pointList.Add(_pp2);
			//_pointList.Add(_pp3);
			//_pointList.Add(_pp4);

			List<System.Windows.Point> _pointList1 = new List<System.Windows.Point>();
			List<System.Windows.Point> _pointList2 = new List<System.Windows.Point>();
			List<System.Windows.Point> _pointList3 = new List<System.Windows.Point>();
			List<System.Windows.Point> _pointList4 = new List<System.Windows.Point>();

			_pointList1 = LineDraw(_pp1, _pp2, _c1, false);
			_pointList2 = LineDraw(_pp2, _pp3, _c2, false);
			_pointList3 = LineDraw(_pp3, _pp4, _c1, false);
			_pointList4 = LineDraw(_pp4, _pp1, _c2, false);

			for (int i = 0; i < _pointList1.Count; i++)
			{
				_pointList.Add(_pointList1[i]);
			}
			for (int i = 1; i < _pointList2.Count; i++)
			{
				_pointList.Add(_pointList2[i]);
			}
			for (int i = 1; i < _pointList3.Count; i++)
			{
				_pointList.Add(_pointList3[i]);
			}
			for (int i = 1; i < _pointList4.Count; i++)
			{
				_pointList.Add(_pointList4[i]);
			}

			for (int i = 0; i < _pointList.Count; i++)
			{
				WaypointAndPathAdd(_pointList[i], BaseSelect);		// Waypoint追加
			}
		}
		// 往復1描画
		private void ReturnDraw1(System.Windows.Point _p1, System.Windows.Point _p2, int _c1, int _c2)
		{
			// 1----2
			//      |
			// 4----3
			// |
			// 5----6
			//      |
			// 8----7

			List<System.Windows.Point> _pointList = new List<System.Windows.Point>();

			System.Windows.Point _pp1 = new System.Windows.Point(_p1.X, _p1.Y);
			System.Windows.Point _pp2 = new System.Windows.Point(_p2.X, _p1.Y);
			System.Windows.Point _pp3 = new System.Windows.Point(_p2.X, _p2.Y);
			System.Windows.Point _pp4 = new System.Windows.Point(_p1.X, _p2.Y);


			List<System.Windows.Point> _pointList1 = new List<System.Windows.Point>();
			List<System.Windows.Point> _pointList2 = new List<System.Windows.Point>();
			List<System.Windows.Point> _pointList3 = new List<System.Windows.Point>();
			List<System.Windows.Point> _pointList4 = new List<System.Windows.Point>();

			_pointList1 = LineDraw(_pp1, _pp4, _c2, false);
			_pointList2 = LineDraw(_pp2, _pp3, _c2, false);

			_pointList = LineDraw(_pp1, _pp2, _c1, false);
			for (int i = 1; i < _pointList2.Count; i++)
			{
				List<System.Windows.Point> _p = new List<System.Windows.Point>();
				if (i % 2 == 0)
				{
					_p = LineDraw(_pointList1[i], _pointList2[i], _c1, false);
				}
				else
				{
					_p = LineDraw(_pointList2[i], _pointList1[i], _c1, false);
				}


				for (int j = 0; j < _p.Count; j++)
				{
					_pointList.Add(_p[j]);
				}

			}
			for (int i = 0; i < _pointList.Count; i++)
			{
				WaypointAndPathAdd(_pointList[i], BaseSelect);		// Waypoint追加
			}
		}
		// 往復2描画
		private void ReturnDraw2(System.Windows.Point _p1, System.Windows.Point _p2, int _c1, int _c2)
		{
			// 1 4-5
			// | | |     
			// 2-3 6         

			List<System.Windows.Point> _pointList = new List<System.Windows.Point>();

			System.Windows.Point _pp1 = new System.Windows.Point(_p1.X, _p1.Y);
			System.Windows.Point _pp2 = new System.Windows.Point(_p2.X, _p1.Y);
			System.Windows.Point _pp3 = new System.Windows.Point(_p2.X, _p2.Y);
			System.Windows.Point _pp4 = new System.Windows.Point(_p1.X, _p2.Y);


			List<System.Windows.Point> _pointList1 = new List<System.Windows.Point>();
			List<System.Windows.Point> _pointList2 = new List<System.Windows.Point>();
			List<System.Windows.Point> _pointList3 = new List<System.Windows.Point>();
			List<System.Windows.Point> _pointList4 = new List<System.Windows.Point>();

			_pointList1 = LineDraw(_pp1, _pp2, _c1, false);
			_pointList2 = LineDraw(_pp4, _pp3, _c1, false);

			_pointList = LineDraw(_pp1, _pp4, _c2, false);
			for (int i = 1; i < _pointList2.Count; i++)
			{
				List<System.Windows.Point> _p = new List<System.Windows.Point>();
				if (i % 2 == 0)
				{
					_p = LineDraw(_pointList1[i], _pointList2[i], _c2, false);
				}
				else
				{
					_p = LineDraw(_pointList2[i], _pointList1[i], _c2, false);
				}


				for (int j = 0; j < _p.Count; j++)
				{
					_pointList.Add(_p[j]);
				}

			}

			for (int i = 0; i < _pointList.Count; i++)
			{
				WaypointAndPathAdd(_pointList[i], BaseSelect);		// Waypoint追加
			}
		}

		private void StandardDraw_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			Radio_StandardDraw.IsChecked = true;
		}
		private void LineDraw_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			Radio_LineDraw.IsChecked = true;
		}
		private void RectangleDraw_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			Radio_RectangleDraw.IsChecked = true;
		}
		private void CircleDraw_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			Radio_CircleDraw.IsChecked = true;
		}
		private void ReturnDraw1_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			Radio_ReturnDraw1.IsChecked = true;
		}
		private void ReturnDraw2_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			Radio_ReturnDraw2.IsChecked = true;
		}
		#endregion

		#region WaypontList

		private void AllWaypoint_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			try
			{
				if (ProjectList.Count > 0)
				{
					if (ProjectList[BaseSelect].WaypointData.Count > 0)
					{
						Xceed.Wpf.Toolkit.DoubleUpDown _command = sender as Xceed.Wpf.Toolkit.DoubleUpDown;
						if (_command != null)
						{
							string str = (string)_command.Name;


							var _waypointList = new FlightPlanning.Waypoint();
							_waypointList = (FlightPlanning.Waypoint)this.WaypointAllChange_DataGrid.Items[0];

							for (int i = 0; i < ProjectList[BaseSelect].WaypointData.Count; i++)
							{
								switch (str)
								{
									case "AllWaypointX":
										WaypointAllChange.x = (double)e.NewValue;
										var _waypoint = WaypointAllChange;
										_waypoint.y = ProjectList[BaseSelect].WaypointData[i].y;
										_waypoint = FlightPlanXML.InverseTranslation(ProjectList[BaseSelect].MissionData, _waypoint);
										ProjectList[BaseSelect].WaypointData[i].x = _waypoint.x;
										ProjectList[BaseSelect].WaypointData[i].Latitude = _waypoint.Latitude;
										ProjectList[BaseSelect].WaypointData[i].Longitude = _waypoint.Longitude;

										ProjectList[BaseSelect].WaypointMarker[i].Position = new PointLatLng(ProjectList[BaseSelect].WaypointData[i].Latitude, ProjectList[BaseSelect].WaypointData[i].Longitude);
										break;

									case "AllWaypointY":
										WaypointAllChange.y = (double)e.NewValue;
										var _waypoint1 = WaypointAllChange;
										_waypoint1.x = ProjectList[BaseSelect].WaypointData[i].x;
										_waypoint1 = FlightPlanXML.InverseTranslation(ProjectList[BaseSelect].MissionData, _waypoint1);
										ProjectList[BaseSelect].WaypointData[i].y = _waypoint1.y;
										ProjectList[BaseSelect].WaypointData[i].Latitude = _waypoint1.Latitude;
										ProjectList[BaseSelect].WaypointData[i].Longitude = _waypoint1.Longitude;
										ProjectList[BaseSelect].WaypointMarker[i].Position = new PointLatLng(ProjectList[BaseSelect].WaypointData[i].Latitude, ProjectList[BaseSelect].WaypointData[i].Longitude);
										break;

									case "AllWaypointHeight":
										ProjectList[BaseSelect].WaypointMarker[i].Altitude = ProjectList[BaseSelect].WaypointData[i].Height = WaypointAllChange.Height = (double)e.NewValue;
										if (AltitudePropertyValue.AltitudeList.Count == ProjectList[BaseSelect].WaypointData.Count)
										{
											List<double> _list = new List<double>();
											_list = AltitudePropertyValue.AltitudeList;
											_list[i] = ProjectList[BaseSelect].WaypointData[i].Height;
											AltitudePropertyValue.AltitudeList = _list;
										}
										break;

									case "AllWaypointHeading":
                                        if ((double)e.NewValue > 180)
                                        {
                                            WaypointAllChange.Heading = 180;
                                        }
                                        else if ((double)e.NewValue < -180)
                                        {
                                            WaypointAllChange.Heading = -180;
                                        }
                                        else
                                        {
                                            WaypointAllChange.Heading = (double)e.NewValue;
                                        }
										ProjectList[BaseSelect].WaypointMarker[i].Azimuth = ProjectList[BaseSelect].WaypointData[i].Heading = WaypointAllChange.Heading;
										break;

									case "AllWaypointThreasholdH":
										ProjectList[BaseSelect].WaypointData[i].ThreasholdH = WaypointAllChange.ThreasholdH = (double)e.NewValue;
										break;
									case "AllWaypointThreasholdV":
										ProjectList[BaseSelect].WaypointData[i].ThreasholdV = WaypointAllChange.ThreasholdV = (double)e.NewValue;
										break;
									case "AllWaypointWait":
										ProjectList[BaseSelect].WaypointData[i].Wait = WaypointAllChange.Wait = (double)e.NewValue;
										break;
									case "AllWaypointFunction":
										ProjectList[BaseSelect].WaypointData[i].Function = WaypointAllChange.Function = (double)e.NewValue;
										break;
									case "AllWaypointSpeed":
                                        if ((double)e.NewValue > 10)
                                        {
                                            WaypointAllChange.Speed = 10;
                                        }
                                        else if ((double)e.NewValue < 0.1)
                                        {
                                            WaypointAllChange.Speed = 0.1;
                                        }
                                        else
                                        {
                                            WaypointAllChange.Speed = (double)e.NewValue;
                                        }
                                        ProjectList[BaseSelect].WaypointMarker[i].Speed = ProjectList[BaseSelect].WaypointData[i].Speed = WaypointAllChange.Speed;
										ProjectList[BaseSelect].PathData[i].Speed = ProjectList[BaseSelect].WaypointData[i].Speed;
										break;
									default:
										break;
								}
								ProjectList[BaseSelect].WaypointPath.Points[i] = ProjectList[BaseSelect].WaypointMarker[i].Position;        //　Waypoint位置をPath位置に反映
								ProjectList[BaseSelect].WaypointPath.RegenerateShape(MainMap);													//　地図上にPath反映

							}
							WaypointValueChanged(BaseSelect);
							WaypointProperty.SelectedObject = null;
							WaypointProperty.SelectedObject = ProjectList[BaseSelect].WaypointData[0];
						}
					}
				}
			}
			catch (Exception ex)
			{
				_logger.Error("Error: " + ex.Message);
			}
		}

		private void AltitudePropertyValue_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			try
			{
				var _id = AltitudePropertyValue.ID;


				ProjectList[BaseSelect].WaypointMarker[_id - 1].Altitude = ProjectList[BaseSelect].WaypointData[_id - 1].Height = AltitudePropertyValue.AltitudeList[_id - 1];

				WaypointValueChanged(BaseSelect);
				WaypointProperty.SelectedObject = null;
				WaypointProperty.SelectedObject = ProjectList[BaseSelect].WaypointData[_id - 1];
			}
			catch
			{

			}
		}

		private void AltitudePropertyValue_AllValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			try
			{
				var _id = AltitudePropertyValue.ID;

				for (int i = 0; i < ProjectList[BaseSelect].WaypointMarker.Count; i++)
				{
					ProjectList[BaseSelect].WaypointMarker[i].Altitude = ProjectList[BaseSelect].WaypointData[i].Height = AltitudePropertyValue.AllValue;
				}

				WaypointValueChanged(BaseSelect);
				WaypointProperty.SelectedObject = null;
				WaypointProperty.SelectedObject = ProjectList[BaseSelect].WaypointData[_id - 1];
			}
			catch
			{

			}

		}
        // [WaypointView]删除按钮
		private void SingleClear_Click(object sender, RoutedEventArgs e)
		{
			try
			{
				if (ProjectList.Count > 0)
				{
					if (ProjectList[BaseSelect].WaypointData.Count > 0)
					{
						for (int i = 0; i < WaypointSelect.Count; i++)
						{
                            // 指定航点的删除和对准调整
							ProjectSubRecord.Add(ProjectDeepCopy(ProjectList));						// 追加
							ProjectList[BaseSelect].WaypointData.RemoveAt(WaypointSelect[i] - 1);
							ProjectList[BaseSelect].WaypointMarker.RemoveAt(WaypointSelect[i] - 1);
							var _project = ProjectDeepCopy(ProjectList);
							MarkerAllClear();
							//ProjectRecord.RemoveAt(ProjectRecord.Count - 1);

							ProjectList = ProjectDeepCopy(_project);
							WaypointListView.Items.Clear();
							MarkerAllSet();											//　Project再設定
						}
						ProjectRecord.Add(ProjectDeepCopy(ProjectList));			// ProjectRecord追加
					}
				}
			}
			catch
			{

			}

		}
		// [WaypointView]添加按钮
		private void WaypointAdd_Click(object sender, RoutedEventArgs e)
		{
			try
			{
				if (ProjectList.Count > 0)
				{
					if (ProjectList[BaseSelect].WaypointMarker.Count > 0)
					{
						GPoint p = MainMap.FromLatLngToLocal(ProjectList[BaseSelect].WaypointMarker.Last().Position);
						WaypointAndPathAdd(new System.Windows.Point(p.X, p.Y), BaseSelect);		// Waypoint追加
						ProjectRecord.Add(ProjectDeepCopy(ProjectList));			// ProjectRecord追加
					}
				}
			}
			catch
			{

			}

		}
        // [WaypointView]升序
		private void WaypointUp_Click(object sender, RoutedEventArgs e)
		{
			try
			{
				if (WaypointListView != null)
				{
					if (WaypointListView.Items.Count > 0)
					{
						var _w = (FlightPlanning.Waypoint)WaypointListView.SelectedItem;
						if (_w != null)
						{
							int _id = _w.ID;
							if (1 < _id)
							{
								ProjectSubRecord.Add(ProjectDeepCopy(ProjectList));						// 追加
								var _wp = ProjectList[BaseSelect].WaypointMarker[_id - 1];

								ProjectList[BaseSelect].WaypointData.Insert(_id - 2, _w);
								ProjectList[BaseSelect].WaypointMarker.Insert(_id - 2, _wp);

								ProjectList[BaseSelect].WaypointData.RemoveAt(_id);
								ProjectList[BaseSelect].WaypointMarker.RemoveAt(_id);
								var _project = ProjectDeepCopy(ProjectList);
								MarkerAllClear();
								//ProjectRecord.RemoveAt(ProjectRecord.Count - 1);

								ProjectList = ProjectDeepCopy(_project);
								for (int i = 0; i < ProjectList[BaseSelect].WaypointData.Count; i++)
								{
									ProjectList[BaseSelect].WaypointData[i].ID = i + 1;
								}
								WaypointListView.Items.Clear();
								MarkerAllSet();											//　Project再設定}

							}
						}

						ProjectRecord.Add(ProjectDeepCopy(ProjectList));			// ProjectRecord追加
					}
				}
			}
			catch
			{

			}

		}
        // [WaypointView]降序
		private void WaypointDown_Click(object sender, RoutedEventArgs e)
		{
			try
			{
				if (WaypointListView != null)
				{
					if (WaypointListView.Items.Count > 0)
					{
						var _w = (FlightPlanning.Waypoint)WaypointListView.SelectedItem;
						if (_w != null)
						{
							int _id = _w.ID;
							if (WaypointListView.Items.Count > _id)
							{
								ProjectSubRecord.Add(ProjectDeepCopy(ProjectList));						// 追加
								var _wp = ProjectList[BaseSelect].WaypointMarker[_id - 1];

								ProjectList[BaseSelect].WaypointData.Insert(_id + 1, _w);
								ProjectList[BaseSelect].WaypointMarker.Insert(_id + 1, _wp);

								ProjectList[BaseSelect].WaypointData.RemoveAt(_id - 1);
								ProjectList[BaseSelect].WaypointMarker.RemoveAt(_id - 1);
								var _project = ProjectDeepCopy(ProjectList);
								MarkerAllClear();
								//ProjectRecord.RemoveAt(ProjectRecord.Count - 1);

								ProjectList = ProjectDeepCopy(_project);
								for (int i = 0; i < ProjectList[BaseSelect].WaypointData.Count; i++)
								{
									ProjectList[BaseSelect].WaypointData[i].ID = i + 1;
								}
								WaypointListView.Items.Clear();
								MarkerAllSet();											//　Project再設定}
							}
						}

						ProjectRecord.Add(ProjectDeepCopy(ProjectList));			// ProjectRecord追加
					}
				}
			}
			catch
			{

			}
		
		}

		#endregion

		// DeepCopy
		private List<Project> ProjectDeepCopy(List<Project> _projectList)
		{

			var _projectAdd = new List<Project>();

			try
			{
				foreach (var _project in _projectList)
				{
					var _p = new Project();
					_p.MissionData = DeepCopyHelper.DeepCopy<Mission>(_project.MissionData);
					_p.OldBasePoint = DeepCopyHelper.DeepCopy<PointLatLng>(_project.OldBasePoint);
					_p.PathData = DeepCopyHelper.DeepCopy<List<FlightPlanning.Path>>(_project.PathData);
					_p.WaypointData = DeepCopyHelper.DeepCopy<List<FlightPlanning.Waypoint>>(_project.WaypointData);
					//_p.BaseMarker = DeepCopyHelper.DeepCopy<GMapMarker>(_project.BaseMarker);
					//_p.WaypointMarker = DeepCopyHelper.DeepCopy < List<GMapMarker>>(_project.WaypointMarker);
					//_p.WaypointPath = DeepCopyHelper.DeepCopy<GMapRoute>(_project.WaypointPath);

					//_p.PathData

					//_projectAdd.Add(DeepCopyHelper.DeepCopy<Project>(_project));
					_projectAdd.Add(_p);

				}
			}
			catch
			{

			}
			return _projectAdd;
			//_projectRecord,.Add(DeepCopyHelper.DeepCopy<List<Project>>(_projectAdd));
		}

		private void WaypointListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			try
			{
				if (Keyboard.Modifiers == ModifierKeys.Control)
				{
					return;
				}
				if (ProjectList.Count > 0)
				{
					if (ProjectList[BaseSelect].WaypointData.Count > 0)
					{
						var _w = (FlightPlanning.Waypoint)WaypointListView.SelectedItem;
						if (_w != null)
						{
							foreach (var i in WaypointSelect)
							{
								ProjectList[BaseSelect].WaypointMarker[i - 1].Mode = -1;
							}
							int _id = _w.ID;
							WaypointSelect.Clear();
							WaypointSelect.Add(_id);					//选择的WaypointID
							ProjectList[BaseSelect].BaseMarker.Mode = 1;
							ProjectList[BaseSelect].WaypointMarker[_id - 1].Mode = 11;

                            // 属性更新
							WaypointProperty.SelectedObject = null;
                            WaypointProperty.SelectedObject = ProjectList[BaseSelect].WaypointData[_id - 1];    //属性显示
						}
					}
				}
			}
			catch
			{

			}
			
		}

		// Google Earth调用
		private void kmlWrite_Click(object sender, RoutedEventArgs e)
		{
			try
			{
				if (ProjectList.Count > 0)
				{
					if (ProjectList[BaseSelect].WaypointData.Count > 0)
					{
						// Instantiate the dialog box
						Microsoft.Win32.SaveFileDialog dlg = new Microsoft.Win32.SaveFileDialog();
						dlg.FileName = "FlightPlanning_" + DateTime.Now.Year.ToString("0000")
												+ DateTime.Now.Month.ToString("00")
												+ DateTime.Now.Day.ToString("00")
												+ "_" + DateTime.Now.Hour.ToString("00")
												+ DateTime.Now.Minute.ToString("00");   // Default file name

						// Configure open file dialog box
						//dlg.FileName = "MultiWaypoint";			// Default file name
						dlg.DefaultExt = ".kml";				// Default file extension
						dlg.Filter = "Keyhole Markup Language|*.kml";//"Y-Planner Project|*.ypp|MultiWaypoint Project|*.wpp";		// Filter files by extension

						// Open the dialog box modally
						Nullable<bool> result = dlg.ShowDialog();

						// Process open file dialog box results
						if (result == true)
						{
							FilePath = dlg.FileName;

							FlightPlanning.FlightPlanXML.KmlWrite(FilePath, ProjectList);
						}
					}
				}

			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(401) MultiWaypoint FileOpen Error: " + ex.Message);
				_logger.Error("(401) MultiWaypoint FileOpen Error: " + ex.Message);
			}
		}

		private void LayoutDocument_IsSelectedChanged(object sender, EventArgs e)
		{
			try
			{
				if (MainMap.Markers.Count > 0)
				{
					if (ProjectList.Count > 0)
					{
						//if (MainMap.Markers.Count < ProjectList[0].WaypointMarker.Count)
						{
                            //ProjectSubRecord.Add(ProjectDeepCopy(ProjectList));						// 重做（重做）添加
							MarkerAllClear();
							//ProjectRecord.RemoveAt(ProjectRecord.Count - 1);
                            ProjectList = ProjectDeepCopy(ProjectRecord[ProjectRecord.Count - 1]);	// 撤消（撤消）
							WaypointListView.Items.Clear();
							MarkerAllSet();											//　Project再設定
				

						}
					}
				}
			}
			catch
			{

			}
		}

	}
	// DeepCopy
	public static class DeepCopyHelper
	{
		public static T DeepCopy<T>(T target)
		{

			T result;
			BinaryFormatter b = new BinaryFormatter();

			MemoryStream mem = new MemoryStream();

			try
			{
				b.Serialize(mem, target);
				mem.Position = 0;
				result = (T)b.Deserialize(mem);
			}
			finally
			{
				mem.Close();
			}

			return result;

		}
	}

}
