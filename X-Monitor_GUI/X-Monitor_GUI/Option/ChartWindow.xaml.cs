using System.Windows.Controls;
using System.Windows;
using System.ComponentModel;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Xml.XPath;
using System;
using Microsoft.Research.DynamicDataDisplay.DataSources;

//WPF Dynamic Data Display
using Microsoft.Research.DynamicDataDisplay;


namespace XMonitor_GUI
{
    /// <summary>
    /// ChartWindow.xaml 相互交互逻辑
    /// </summary>
    public partial class ChartWindow : UserControl
    {
        public bool PlotEditable = false;				// true:有效的情节 fasle:禁用的情节
        private int Window_Id;                          // 选项屏幕ID
        private int Selected_Index;                // 选择组合框数字

        private List<ObservableDataSource<System.Windows.Point>> source1;     //所有的数据
		List<string> _tagHi;
		List<string> _tagLo;

		List<string> _plotDataSelectList;
		XElement _element;

        public ChartWindow(int id)
        {
            InitializeComponent();

            Width = double.NaN;
            Height = double.NaN;

            Window_Id = id;
			Selected_Index = -1;

            #region 读图数据列表
			//
			var xmle= XDocument.Load(@"Xml/Settings.xml");				//    
			_element = xmle.Root.Element("Settings").Element("PlotContents");


            //加载 XML 文件
			XDocument xml = XDocument.Load(@"Xml/Message/Get_UAVData.xml");				//     
            var xmlRoot = xml.Root.Element("message").Element("Data").Element("UAV");   // 从 XML 中检索数据

            xml = XDocument.Load(System.Environment.GetFolderPath(System.Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\X-Monitor\Setting\GUI\Settings.xml");
			_tagHi = new List<string>();
			_tagLo = new List<string>();
			_plotDataSelectList = new List<string>();

			foreach (var element in xml.Root.XPathSelectElements("Settings/Plot/Tag"))
			{
				string _attVal = element.Attribute("a").Value;
				if(_attVal == "H")
				{
					_tagHi.Add(element.Value);
				}
				else
				{
					_tagLo.Add(element.Value);
				}	
			}

            System.Xml.XmlDocument xmlDoc = new System.Xml.XmlDocument();
            xmlDoc.LoadXml(xmlRoot.ToString());											//转换
            System.Xml.XmlElement xmlRoot1 = xmlDoc.DocumentElement;

            string trvChild = string.Empty;
            Make(xmlRoot1, ref trvChild);		//提取标记名称

            //PlotList.Items.Add("CPU使用率");

			PlotList _xmlTag = new PlotList
			{
				XMLTag = "CPU使用率",
				Contents = "PCの使用率"
			};
			TagList.Items.Add(_xmlTag);
            #endregion

            source1 = new List<ObservableDataSource<System.Windows.Point>>();
        }

        // 提取的父节点和子节点
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

                        // 检查排除标记
						foreach (var tag in _tagHi)
						{
							if (tag == trvChild)
							{
								_flagHi = false;
								break;
							}
						}

                        // 添加父节点
						if (_flagHi && (trvParent == null || trvParent == ""))
						{
                            // 检查子节点
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

                        // 如果你有一个父节点
						if (trvParent != null && trvParent != "")
						{
							bool _flagLo = true;
                            // 检查排除标记
							foreach (var tag in _tagLo)
							{
								if (tag == trvChild)
								{
									_flagLo = false;
									break;
								}
							}

                            //添加一个子节点
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
            //父节点的设置
			trvParent = xmlParent.Name;
		}
		//　标记提取
		private void GetXmlPathAIT(string _path, string _a, string _i, string _t)
		{
			PlotList _xmlTag = new PlotList
			{
				XMLTag = _path,
				Contents = "NoData"
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
		}

        // PlotData更新
        public void PlotUpdate(int i, System.Windows.Point p)
        {
			source1[i].AppendAsync(Dispatcher, p);
			double diff = (int)(source1[i].Collection.Last().X - source1[i].Collection.First().X);

            // 绘制时间
			if (diff >= SpanTime.Value)
			{
				source1[i].Collection.RemoveAt(0);
			}
        }

        #region 内部イベント
		//// 2回目以降のPlotData選択
		//private void PlotDataSelectionChanged(object sender, SelectionChangedEventArgs e)
		//{
		//	if (Selected_Index != -1)
		//	{
		//		PlotDataSelection();
		//	}
		//}
		// 初期のPlotData選択
		//private void PlotDataDropDownClosed(object sender, System.EventArgs e)
		//{
		//	System.Diagnostics.Debug.WriteLine(">>>" + PlotDataSelect + "/" + (string)PlotList.SelectedItem);
		//	if (PlotDataSelect != (string)PlotList.SelectedItem)
		//	{
		//		SpanTime.IsEnabled = false;
		//		PlotDataSelection();
		//	}
		//}

		//// PlotData選択
		//private void PlotDataSelection()
		//{
		//	var _t = (PlotList)TagList.SelectedItems[i];
		//	_tag.Add(_t.XMLTag);

		//	if (PlotEditable && ((string)TagList.SelectedItem != null))
		//	{
		//		source1.Add(new ObservableDataSource<System.Windows.Point>());
		//		source1[source1.Count - 1].SetXYMapping(p => p);
		//		plotter.AddLineGraph(source1[source1.Count - 1], 1, (string)PlotList.SelectedItem);
		//		Selected_Index = PlotList.SelectedIndex;
		//		SetValue(PlotDataSelectProperty, (string)PlotList.SelectedItem);
		//	}
		//}

		private EnumerableDataSource<double> CreateCurrencyDataSource(double[] data)
        {
            double[] x_axis = new double[50];
            for(int i=0 ; i < 50; i++ ){
                x_axis[i] = i;
            }
            EnumerableDataSource<double> xSrc = new EnumerableDataSource<double>(x_axis);
            xSrc.SetXMapping(x => x);

            EnumerableDataSource<double> ds = new EnumerableDataSource<double>(data);
            ds.SetYMapping(y => y);
            return ds;
        }
        // PlotDataクリア
        private void PlotClearButton_Click(object sender, RoutedEventArgs e)
        {
            PlotEditable = false;
			Selected_Index = -1;
			SpanTime.IsEnabled = true;
			PlotDataSelect = string.Empty;

			//　クリア
			foreach (var dataSource in source1)
			{
				dataSource.Collection.Clear();
			}
			source1.Clear();
			plotter.Children.RemoveAll(typeof(LineGraph));

			_splitButton.IsEnabled = true;
			SpanTime.IsEnabled = true;
        }
        #endregion

        #region プロパティ
		public bool isEnable
		{
			get { return (bool)this.IsEnabled; }
			set { this.IsEnabled = value; }
		}
        public int ID
        {
            get { return (int)Window_Id; }
            set { Window_Id = value; }
        }
        public int SelectedIndex
        {
            get { return (int)Selected_Index; }
            set { Selected_Index = value; }
        }

        public static readonly DependencyProperty PlotDataSelectProperty =
            DependencyProperty.Register(
            "PlotDataSelect", typeof(string), typeof(ChartWindow),
            new FrameworkPropertyMetadata(
            default(string),
            FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
            OnPlotDataSelectChanged));

        [Category("Window")]
        [Description("PlotDataSelect")]
        [Browsable(true)]
        [Bindable(true)]
        public string PlotDataSelect
        {
            get { return (string)GetValue(PlotDataSelectProperty); }
            set { SetValue(PlotDataSelectProperty, value); }
        }
        #endregion

		public List<string> PlotDataSelectList
		{
			get { return _plotDataSelectList; }
			set { _plotDataSelectList = value; }
		}

        #region プロパティメソッド
        private static void OnPlotDataSelectChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            ChartWindow control = (ChartWindow)d;
            control.TagList.SelectedItem = (string)e.NewValue;

        }

        #endregion

		public void Clear(){
			PlotEditable = false;
			Selected_Index = -1;
			SpanTime.IsEnabled = true;
			PlotDataSelect = string.Empty;

			//　クリア
			foreach (var dataSource in source1)
			{
				dataSource.Collection.Clear();
			}
			source1.Clear();
			_plotDataSelectList.Clear();
			plotter.Children.RemoveAll(typeof(LineGraph));
		}

		private void Button_Click(object sender, System.Windows.RoutedEventArgs e)
		{
			_splitButton.IsOpen = false;
		}

		private void SplitButton_Click(object sender, System.Windows.RoutedEventArgs e)
		{
			//Xceed.Wpf.Toolkit.MessageBox.Show("Thanks for clicking me!", "SplitButton Click");

			////////////////////////////////////////////////////////////////////////////
			// 選択中のタグリスト
			_plotDataSelectList.Clear();

			// 左上に配置
			plotter.Legend.LegendRight = Double.NaN;
			plotter.Legend.LegendTop = 10;
			plotter.Legend.LegendLeft = 10;
			plotter.Legend.LegendBottom = Double.NaN;
			
			for (int i = 0; i < TagList.SelectedItems.Count; i++)
			{
				var _t = (PlotList)TagList.SelectedItems[i];
				source1.Add(new ObservableDataSource<System.Windows.Point>());
				source1[source1.Count - 1].SetXYMapping(p => p);

				plotter.AddLineGraph(source1[source1.Count - 1], 1, _t.Contents);
				Selected_Index = i;
				//SetValue(PlotDataSelectProperty, _t.XMLTag);
				_plotDataSelectList.Add(_t.XMLTag);
			}

			_splitButton.IsEnabled = false;
			SpanTime.IsEnabled = false;
		}

		#region イベント
		// Latチェックボックス　チェック時
		private void LatCheck_Checked(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < TagList.Items.Count; i++)
			{
				var _t = (PlotList)TagList.Items[i];

				if (_t.XMLTag.IndexOf("Nav/Lat") != -1 || _t.XMLTag.IndexOf("Wpt/Lat") != -1)
				{
					TagList.SelectedItems.Add(_t);
				}
			}
		}
		// Latチェックボックス　チェック解除時　
		private void LatCheck_Unchecked(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < TagList.Items.Count; i++)
			{
				var _t = (PlotList)TagList.Items[i];
				if (_t.XMLTag.IndexOf("Nav/Lat") != -1 || _t.XMLTag.IndexOf("Wpt/Lat") != -1)
				{
					TagList.SelectedItems.Remove(_t);
				}
			}
		}

		// Lonチェックボックス　チェック時
		private void LonCheck_Checked(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < TagList.Items.Count; i++)
			{
				var _t = (PlotList)TagList.Items[i];

				if (_t.XMLTag.IndexOf("Nav/Lon") != -1 || _t.XMLTag.IndexOf("Wpt/Lon") != -1)
				{
					TagList.SelectedItems.Add(_t);
				}
			}
		}
		// Lonチェックボックス　チェック解除時　
		private void LonCheck_Unchecked(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < TagList.Items.Count; i++)
			{
				var _t = (PlotList)TagList.Items[i];
				if (_t.XMLTag.IndexOf("Nav/Lon") != -1 || _t.XMLTag.IndexOf("Wpt/Lon") != -1)
				{
					TagList.SelectedItems.Remove(_t);
				}
			}
		}

		// Altチェックボックス　チェック時
		private void AltCheck_Checked(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < TagList.Items.Count; i++)
			{
				var _t = (PlotList)TagList.Items[i];

				if (_t.XMLTag.IndexOf("Nav/Alt") != -1 || _t.XMLTag.IndexOf("Wpt/Alt") != -1)
				{
					TagList.SelectedItems.Add(_t);
				}
			}
		}
		// Altチェックボックス　チェック解除時　
		private void AltCheck_Unchecked(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < TagList.Items.Count; i++)
			{
				var _t = (PlotList)TagList.Items[i];
				if (_t.XMLTag.IndexOf("Nav/Alt") != -1 || _t.XMLTag.IndexOf("Wpt/Alt") != -1)
				{
					TagList.SelectedItems.Remove(_t);
				}
			}
		}

		// Headingチェックボックス　チェック時
		private void HeadingCheck_Checked(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < TagList.Items.Count; i++)
			{
				var _t = (PlotList)TagList.Items[i];

				if (_t.XMLTag.IndexOf(@"IMU/Att[@t=""R3""][@i=""2""]") != -1 || _t.XMLTag.IndexOf(@"Wpt/Yaw[@t=""R""]") != -1)
				{
					TagList.SelectedItems.Add(_t);
				}
			}
		}
		// Headingチェックボックス　チェック解除時　
		private void HeadingCheck_Unchecked(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < TagList.Items.Count; i++)
			{
				var _t = (PlotList)TagList.Items[i];
				if (_t.XMLTag.IndexOf(@"IMU/Att[@t=""R3""][@i=""2""]") != -1 || _t.XMLTag.IndexOf(@"Wpt/Yaw[@t=""R""]") != -1)
				{
					TagList.SelectedItems.Remove(_t);
				}
			}
		}

		// Propoチェックボックス　チェック時
		private void PropoCheck_Checked(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < TagList.Items.Count; i++)
			{
				var _t = (PlotList)TagList.Items[i];

				if ((_t.XMLTag.IndexOf(@"Propo/An[@t=""USI16""][@i=""0""]") != -1 || _t.XMLTag.IndexOf(@"Propo/An[@t=""USI16""][@i=""1""]") != -1) ||
					(_t.XMLTag.IndexOf(@"Propo/An[@t=""USI16""][@i=""2""]") != -1 || _t.XMLTag.IndexOf(@"Propo/An[@t=""USI16""][@i=""3""]") != -1))
				{
					TagList.SelectedItems.Add(_t);
				}
			}
		}
		// Propoチェックボックス　チェック解除時　
		private void PropoCheck_Unchecked(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < TagList.Items.Count; i++)
			{
				var _t = (PlotList)TagList.Items[i];
				if ((_t.XMLTag.IndexOf(@"Propo/An[@t=""USI16""][@i=""0""]") != -1 || _t.XMLTag.IndexOf(@"Propo/An[@t=""USI16""][@i=""1""]") != -1) ||
					(_t.XMLTag.IndexOf(@"Propo/An[@t=""USI16""][@i=""2""]") != -1 || _t.XMLTag.IndexOf(@"Propo/An[@t=""USI16""][@i=""3""]") != -1))
				{
					TagList.SelectedItems.Remove(_t);
				}
			}
		}
		// Inputチェックボックス　チェック時
		private void InputCheck_Checked(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < TagList.Items.Count; i++)
			{
				var _t = (PlotList)TagList.Items[i];

				if (_t.XMLTag.IndexOf(@"Ctrl/Input[@a=""H""][@t=""R4""][@i=""0""]") != -1 || _t.XMLTag.IndexOf(@"Ctrl/Input[@a=""H""][@t=""R4""][@i=""1""]") != -1
					|| _t.XMLTag.IndexOf(@"Ctrl/Input[@a=""H""][@t=""R4""][@i=""2""]") != -1 || _t.XMLTag.IndexOf(@"Ctrl/Input[@a=""H""][@t=""R4""][@i=""3""]") != -1)
				{
					TagList.SelectedItems.Add(_t);
				}
			}
		}
		// Inputチェックボックス　チェック解除時　
		private void InputCheck_Unchecked(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < TagList.Items.Count; i++)
			{
				var _t = (PlotList)TagList.Items[i];
				if (_t.XMLTag.IndexOf(@"Ctrl/Input[@a=""H""][@t=""R4""][@i=""0""]") != -1 || _t.XMLTag.IndexOf(@"Ctrl/Input[@a=""H""][@t=""R4""][@i=""1""]") != -1
					|| _t.XMLTag.IndexOf(@"Ctrl/Input[@a=""H""][@t=""R4""][@i=""2""]") != -1 || _t.XMLTag.IndexOf(@"Ctrl/Input[@a=""H""][@t=""R4""][@i=""3""]") != -1)
				{
					TagList.SelectedItems.Remove(_t);
				}
			}
		}
		#endregion
    }

	public class PlotList
	{
		public string XMLTag { get; set; }
		public string Contents { get; set; }
	}
}
