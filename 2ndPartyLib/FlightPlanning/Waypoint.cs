using System.ComponentModel;
using System;
using System.Windows;

namespace FlightPlanning
{
    /// <summary>
    /// Waypointパラメータクラス
    /// </summary>
	[Serializable()]
	public class Waypoint
    {
		[ReadOnly(true)]
		[Browsable(false)]
		//[Editor(typeof(ComboBoxNameEditor), typeof(ComboBoxNameEditor))] 
		public string Sub { get; set; }                 //sub

        [Category("1.Waypoint")]
        [DisplayName("航点标识符")]
        [Description("航点标识符")]
		[ReadOnly(true)]
		//[Editor(typeof(ComboBoxNameEditor), typeof(ComboBoxNameEditor))] 
        public int ID { get; set; }                 //航点标识符

        [Category("1.Waypoint")]
        [DisplayName("路点名称")]
        [Description("路点名称")]
		[Browsable(false)]
        public string Name { get; set; }            //路点名称

        [Category("1.Waypoint")]
		[DisplayName("纬度")]
		[Description("纬度 [deg]")]
		[Editor(typeof(UpDownLatValueEditor), typeof(UpDownLatValueEditor))]
        public double Latitude { get; set; }        //纬度 [deg]

        [Category("1.Waypoint")]
		[DisplayName("经度")]
		[Description("经度 [deg]")]
		[Editor(typeof(UpDownLonValueEditor), typeof(UpDownLonValueEditor))] 
        public double Longitude { get; set; }       //经度 [deg]

		[Category("2.Mission Coordinate")]
		[DisplayName("X")]
        [Description("任务坐标 x [m]")]
		[Editor(typeof(UpDownXValueEditor), typeof(UpDownXValueEditor))] 
        public double x { get; set; }               //任务坐标x [m]

		[Category("2.Mission Coordinate")]
		[DisplayName("Y")]
        [Description("任务坐标 y [m]")]
		[Editor(typeof(UpDownYValueEditor), typeof(UpDownYValueEditor))] 
        public double y { get; set; }               //任务坐标y [m]

		[Category("2.Mission Coordinate")]
		[DisplayName("高度")]
		[Description("坐标高度 [m]")]
		[Editor(typeof(UpDownZValueEditor), typeof(UpDownZValueEditor))] 
        public double Height { get; set; }          //坐标高度 [m]

        [Category("3.Heading")]
		[DisplayName("方位")]
		[Description("方位 [deg]")]
		[Editor(typeof(UpDownHeadingValueEditor), typeof(UpDownHeadingValueEditor))] 
        public double Heading { get; set; }         //方位 [deg]

		[Category("7.Threashold"), DefaultValue(3.0)]
		[DisplayName("区域半径")]
		[Description("航点的圆形半径 [m]")]
		[Browsable(false)]
		[Editor(typeof(UpDownThreasholdHValueEditor), typeof(UpDownThreasholdHValueEditor))] 
        public double ThreasholdH { get; set; }     //确定航点的圆形半径 [m]

		[Category("8.Threashold"), DefaultValue(0)]
		[DisplayName("区域高度")]
		[Description("是否到达航点的一半 [m]")]
		[Browsable(false)]
		[Editor(typeof(UpDownThreasholdVValueEditor), typeof(UpDownThreasholdVValueEditor))] 
		[ReadOnly(true)]
        public double ThreasholdV { get; set; }     //TODO:[TBD]航点的一半来确定高度[m]

		[Category("4.Wait"), DefaultValue(0.5)]
		[DisplayName("等待时间")]
        [Description("等待时间 [sec]")]
		[Editor(typeof(UpDownWaitValueEditor), typeof(UpDownWaitValueEditor))]
        public double Wait { get; set; }            //等待时间 [sec]

        [Category("6.Function")]
		[DisplayName("功能")]
		[Description("函数ID")]
		[ReadOnly(true)]
		[Browsable(false)]
        public double Function { get; set; }        //函数ID

		[Category("5.Speed")]
		[DisplayName("最大速度")]
		[Description("最大速度 [m/s]")]
		[Editor(typeof(UpDownSpeedValueEditor), typeof(UpDownSpeedValueEditor))] 
		public double Speed { get; set; }           //路径上的最大速度 [m/s]

		//[Category("0.Test")]
		//[DisplayName("Test")]
		//[Description("Test")]
		//[Xceed.Wpf.Toolkit.PropertyGrid.Attributes.ItemsSource(typeof(TestList))]
		//public string test { get; set; }           //路径上的最大速度 [m/s]

    }
	public class TestList : Xceed.Wpf.Toolkit.PropertyGrid.Attributes.IItemsSource
	{
		//System.Collections.Generic.List<string> _list;
		//Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem m_PropertyItem;
		//Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorComboBox _control;

		//public FrameworkElement ResolveEditor(Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem propertyItem)
		//{
		//	m_PropertyItem = propertyItem;
		//	_control = new Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorComboBox();
		//	_control.SelectionChanged += OnSelectionChanged;
		//	return _control;
		//}
		//void OnSelectionChanged(object sender, EventArgs e)
		//{
		//	//m_PropertyItem.Value = _control.SelectionBoxItem;
		//}

		public Xceed.Wpf.Toolkit.PropertyGrid.Attributes.ItemCollection GetValues()
		{
			Xceed.Wpf.Toolkit.PropertyGrid.Attributes.ItemCollection _items = new Xceed.Wpf.Toolkit.PropertyGrid.Attributes.ItemCollection();
			

			//foreach(var _item in _list)
			//{
			//	_items.Add(_item);
			//}

			return _items;
		}

	}

	public class ComboBoxNameEditor : Xceed.Wpf.Toolkit.PropertyGrid.Editors.ITypeEditor
	{
		Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem m_PropertyItem;
		Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorComboBox _control;

		public FrameworkElement ResolveEditor(Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem propertyItem)
		{
			m_PropertyItem = propertyItem;
			_control = new Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorComboBox();
			_control.SelectionChanged += OnSelectionChanged;
			return _control;
		}
		void OnSelectionChanged(object sender, EventArgs e)
		{
			m_PropertyItem.Value = _control.SelectionBoxItem;
		}
	}

	public class UpDownXValueEditor : Xceed.Wpf.Toolkit.PropertyGrid.Editors.ITypeEditor
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
			_control.Increment = 1;
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

	public class UpDownYValueEditor : Xceed.Wpf.Toolkit.PropertyGrid.Editors.ITypeEditor
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
			_control.Increment = 1;
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

	public class UpDownZValueEditor : Xceed.Wpf.Toolkit.PropertyGrid.Editors.ITypeEditor
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
			_control.Increment = 1;
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

	public class UpDownHeadingValueEditor : Xceed.Wpf.Toolkit.PropertyGrid.Editors.ITypeEditor
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
			_control.Increment = 1;
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

	public class UpDownSpeedValueEditor : Xceed.Wpf.Toolkit.PropertyGrid.Editors.ITypeEditor
	{
		Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem m_PropertyItem;
		Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorDoubleUpDown _control;
		bool f;

		public FrameworkElement ResolveEditor(Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem propertyItem)
		{
			m_PropertyItem = propertyItem;
			_control = new Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorDoubleUpDown();
			_control.Increment = 0.5;
			_control.Maximum = 10;
			_control.Minimum = 1;
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

	public class UpDownThreasholdHValueEditor : Xceed.Wpf.Toolkit.PropertyGrid.Editors.ITypeEditor
	{
		Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem m_PropertyItem;
		Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorDoubleUpDown _control;
		bool f;

		public FrameworkElement ResolveEditor(Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem propertyItem)
		{
			m_PropertyItem = propertyItem;
			_control = new Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorDoubleUpDown();
			_control.Increment = 1;
			_control.Maximum = 10;
			_control.Minimum = 0.01;
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

	public class UpDownThreasholdVValueEditor : Xceed.Wpf.Toolkit.PropertyGrid.Editors.ITypeEditor
	{
		Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem m_PropertyItem;
		Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorDoubleUpDown _control;
		bool f;

		public FrameworkElement ResolveEditor(Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem propertyItem)
		{
			m_PropertyItem = propertyItem;
			_control = new Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorDoubleUpDown();
			_control.Increment = 1;
			_control.Maximum = 10;
			_control.Minimum = 0.01;
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

	public class UpDownWaitValueEditor : Xceed.Wpf.Toolkit.PropertyGrid.Editors.ITypeEditor
	{
		Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem m_PropertyItem;
		Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorDoubleUpDown _control;
		bool f;

		public FrameworkElement ResolveEditor(Xceed.Wpf.Toolkit.PropertyGrid.PropertyItem propertyItem)
		{
			m_PropertyItem = propertyItem;
			_control = new Xceed.Wpf.Toolkit.PropertyGrid.Editors.PropertyGridEditorDoubleUpDown();
			_control.Increment = 1;
			_control.Minimum = 0;
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
