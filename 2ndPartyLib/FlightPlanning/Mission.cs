using System.ComponentModel;
using System;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Controls;
using System.Windows.Data;

namespace FlightPlanning
{
    /// <summary>
    /// Missionパラメータクラス
    /// </summary>
	[Serializable()]
    public class Mission
    {
		// DisplayName
		// DefaultValue				プロパティの既定値を指定します。
		// Browsable				[プロパティ] ウィンドウにプロパティやイベントを表示するかどうかを指定します。
		// Category					プロパティまたはイベントをビジュアル デザイナで表示するカテゴリを指定します。
		// Description				プロパティまたはイベントの説明を指定します。
		// DesignOnly				プロパティを設定できるのがデザイン時だけかどうかを指定します。
		// ReadOnly					この属性が関連付けられているプロパティがデザイン時に読み取り専用か、または読み書き可能かを指定します。
		// ParenthesizePropertyName 関連付けられているプロパティの名前をかっこで囲んで [プロパティ] ウィンドウに表示するかどうかを示します。
		// Editor					Control機能


        [Category("1.Mission")]
        [DisplayName("任务名称")]
        [Description("任务名称")]
		[ReadOnly(true)]
        [Browsable(true)]							// 不显示属性窗口
        public string Name { get; set; }            //任务名称

        [Category("其他")]
        [DisplayName("任务设置")]
        [Description("飞行路径的设定")]
        [Browsable(true)]
		[ReadOnly(true)]
        public bool enabled { get; set; }           //从飞行路径中排除

		[DisplayName("纬度")]
        [Category("2.Origin"),Description("坐标原点纬度 [deg]"),Browsable(true)]
		[Editor(typeof(UpDownLatValueEditor), typeof(UpDownLatValueEditor))]
		public double Latitude { get; set; }        //坐标原点纬度 [deg]

		[DisplayName("经度")]
        [Category("2.Origin"),Description("坐标原点经度 [deg]"),Browsable(true)]
		[Editor(typeof(UpDownLonValueEditor), typeof(UpDownLonValueEditor))] 
        public double Longitude { get; set; }       //坐标原点经度 [deg]

        [Category("2.Origin")]
		[DisplayName("高度")]
        [Description("坐标原点高度 [m]")]
        [Browsable(false)]
		[Editor(typeof(UpDownHeadingValueEditor), typeof(UpDownHeadingValueEditor))]
		[ReadOnly(true)]
		public double Altitude { get; set; }        //坐标原点高度 [m]

        [Category("2.Origin")]
		[DisplayName("旋转")]
        [Description("X轴方位角 [deg]")]
		[Editor(typeof(UpDownZValueEditor), typeof(UpDownZValueEditor))] 
        public double Rotation { get; set; }        //X轴方位角[deg]

        [Category("3.Scale")]
        [Description("X轴缩小率")]
		[Editor(typeof(UpDownXValueEditor), typeof(UpDownXValueEditor))] 
        public double ScaleX { get; set; }          //X轴缩小率

        [Category("3.Scale")]
        [Description("Y轴缩小率")]
		[Editor(typeof(UpDownYValueEditor), typeof(UpDownYValueEditor))]
        public double ScaleY { get; set; }          //Y轴缩小率

        [Category("3.Scale")]
        [Description("Z轴缩小率")]
        [ReadOnly(true)]
		[Browsable(false)]
        public double ScaleZ { get; set; }          //Z轴缩小率

        [Category("其他")]
		[DisplayName("高度设定")]
        [Description("高度设定")]
        [Xceed.Wpf.Toolkit.PropertyGrid.Attributes.ItemsSource(typeof(AltitudeModeList))]
        [ReadOnly(true)]
        public string AltitudeMode { get; set; }    //高度设定

		[Category("其他")]
		[DisplayName("以下任务ID")]
		[Browsable(false)]
		[ReadOnly(true)]
		[Description("以下任务ID")]
		public int AccessID { get; set; }            //目标ID

		[Category("其他")]
		[DisplayName("前一任务ID")]
		[Browsable(false)]
		[ReadOnly(true)]
        [Description("前一任务ID")]
		public int ConnectID { get; set; }            //连接ID

    }

    // 参考：http://wpftoolkit.codeplex.com/wikipage?title=PropertyGrid#custom_itemssource
    public class AltitudeModeList : Xceed.Wpf.Toolkit.PropertyGrid.Attributes.IItemsSource
    {
        public Xceed.Wpf.Toolkit.PropertyGrid.Attributes.ItemCollection GetValues()
        {
            Xceed.Wpf.Toolkit.PropertyGrid.Attributes.ItemCollection _items = new Xceed.Wpf.Toolkit.PropertyGrid.Attributes.ItemCollection();
            _items.Add("relativeToHome");
            _items.Add("relativeToGround");
            _items.Add("absolute");

            return _items;
        }

    }
	
	public class UpDownLatValueEditor : Xceed.Wpf.Toolkit.PropertyGrid.Editors.ITypeEditor
	{
		Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem m_PropertyItem;
		Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorDoubleUpDown _control;
		bool f;

		public FrameworkElement ResolveEditor(Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem propertyItem)
		{
			m_PropertyItem = propertyItem;
			_control = new Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorDoubleUpDown();
			//_control.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
			//_control.Margin = new Thickness(0, 0, 0, 0);
			_control.Increment = 0.00001;
			_control.Maximum = 90;
			_control.Minimum = -90;
			_control.Value = (double)propertyItem.Value;
			_control.LostFocus += OnLostFocus;
			_control.KeyUp += OnKeyUp;
			_control.ValueChanged += OnValueChanged;
			_control.MouseUp += OnMouseUp;
			_control.TouchUp += OnTouchUp;
			_control.PreviewMouseUp += OnMouseUp;
			_control.PreviewTouchUp += OnTouchUp;
			f = false;
			return _control;
		}
		void OnMouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			f = true;
			if ((double)m_PropertyItem.Value != _control.Value) m_PropertyItem.Value = _control.Value;
		}
		void OnTouchUp(object sender, System.Windows.Input.TouchEventArgs e)
		{
			f = true;
			if ((double)m_PropertyItem.Value != _control.Value) m_PropertyItem.Value = _control.Value;
		}
		void OnValueChanged(object sender, RoutedEventArgs e)
		{
			//if(!f)
			//m_PropertyItem.Value = _control.Value;
		}
		void OnLostFocus(object sender, System.EventArgs e)
		{
			m_PropertyItem.Value = _control.Value;
			f = false;
		}
		void OnKeyUp(object sender, System.Windows.Input.KeyEventArgs e)
		{
			if (!((e.Key >= System.Windows.Input.Key.D0 && e.Key <= System.Windows.Input.Key.D9) || (e.Key >= System.Windows.Input.Key.NumPad0 && e.Key <= System.Windows.Input.Key.NumPad9) || (e.Key == System.Windows.Input.Key.Back) || (e.Key == System.Windows.Input.Key.Delete) || (e.Key == System.Windows.Input.Key.Decimal) || (e.Key == System.Windows.Input.Key.Subtract) || (e.Key == System.Windows.Input.Key.OemMinus)))
			{
				m_PropertyItem.Value = _control.Value;
			}
		}
	}

	public class UpDownLonValueEditor : Xceed.Wpf.Toolkit.PropertyGrid.Editors.ITypeEditor
	{
		Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem m_PropertyItem;
		Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorDoubleUpDown _control;
		bool f;

		public FrameworkElement ResolveEditor(Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem propertyItem)
		{
			m_PropertyItem = propertyItem;
			_control = new Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorDoubleUpDown();
			//_control.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
			//_control.Margin = new Thickness(0, 0, 0, 0);
			_control.Increment = 0.00001;
			_control.Maximum = 180;
			_control.Minimum = -180;
			_control.Value = (double)propertyItem.Value;
			_control.LostFocus += OnLostFocus;
			_control.KeyUp += OnKeyUp;
			_control.ValueChanged += OnValueChanged;
			_control.MouseUp += OnMouseUp;
			_control.TouchUp += OnTouchUp;
			_control.PreviewMouseUp += OnMouseUp;
			_control.PreviewTouchUp += OnTouchUp;
			f = false;
			return _control;
		}
		void OnMouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			f = true;
			if ((double)m_PropertyItem.Value != _control.Value) m_PropertyItem.Value = _control.Value;
		}
		void OnTouchUp(object sender, System.Windows.Input.TouchEventArgs e)
		{
			f = true;
			if ((double)m_PropertyItem.Value != _control.Value) m_PropertyItem.Value = _control.Value;
		}
		void OnValueChanged(object sender, RoutedEventArgs e)
		{
			//if(!f)
			//m_PropertyItem.Value = _control.Value;
		}
		void OnLostFocus(object sender, System.EventArgs e)
		{
			m_PropertyItem.Value = _control.Value;
			f = false;
		}
		void OnKeyUp(object sender, System.Windows.Input.KeyEventArgs e)
		{
			if (!((e.Key >= System.Windows.Input.Key.D0 && e.Key <= System.Windows.Input.Key.D9) || (e.Key >= System.Windows.Input.Key.NumPad0 && e.Key <= System.Windows.Input.Key.NumPad9) || (e.Key == System.Windows.Input.Key.Back) || (e.Key == System.Windows.Input.Key.Delete) || (e.Key == System.Windows.Input.Key.Decimal) || (e.Key == System.Windows.Input.Key.Subtract) || (e.Key == System.Windows.Input.Key.OemMinus)))
			{
				m_PropertyItem.Value = _control.Value;
			}
		}
	}

}
