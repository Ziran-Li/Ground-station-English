using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

namespace Y_Planner.Option
{
	/// <summary>
	/// AltitudeProperty.xaml の相互作用ロジック
	/// </summary>
	public partial class AltitudeProperty : UserControl
	{
		List<double> _altitudeList;
		int counter;
		bool f;
		int _id;

		public AltitudeProperty()
		{
			InitializeComponent();

			_altitudeList = new List<double>();
			Clear();

			this._Min.ValueChanged += new System.Windows.RoutedPropertyChangedEventHandler<object>(this.Min_ValueChanged);

			this._Max.ValueChanged += new System.Windows.RoutedPropertyChangedEventHandler<object>(this.Max_ValueChanged);
		}

		public void AltitudeAdd(double val = 0)
		{
			int w = 10;

			Slider _slider = new Slider();
			_slider.Name = "s" + counter.ToString();
			_slider.Maximum = (double)_Max.Value;
			_slider.Minimum = (double)_Min.Value;
			_slider.Value = val;
			_slider.Height = 200;
			_slider.ValueChanged += new System.Windows.RoutedPropertyChangedEventHandler<double>(this.Uav_ValueChanged);	//イベント追加
			_slider.SetValue(Canvas.TopProperty, 20.0);
			_slider.SetValue(Canvas.LeftProperty, 40.0 * counter + w);
			_slider.Orientation = Orientation.Vertical;
			canvas1.Children.Add(_slider);
			//
			Label _label = new Label();
			_label.Content = (counter + 1).ToString();
			_label.SetValue(Canvas.TopProperty, 0.0);
			_label.SetValue(Canvas.LeftProperty, 40.0 * counter + w);
			canvas1.Children.Add(_label);
			//
			Label _label2 = new Label();
			_label2.Content = _slider.Value.ToString();
			_label2.SetValue(Canvas.TopProperty, 220.0);
			_label2.SetValue(Canvas.LeftProperty, 40.0 * counter + w);
			Binding bind = new Binding();
			bind.ElementName = "s" + counter.ToString();//"UAV";
			bind.Path = new PropertyPath(Slider.ValueProperty);
			_label2.SetBinding(Label.ContentProperty, bind);
			canvas1.Children.Add(_label2);


			canvas1.Width += 40;

			counter++;
		}
		public void AltitudeUpdate()
		{
			int k = canvas1.Children.Count / 3;
			for (int i = 0; i < k; i++)
			{
				Slider _slider = (Slider)canvas1.Children[i * 3];
				var _val= this._altitudeList[i];
				_slider.Value = (double)_val;
				canvas1.Children[i * 3] = _slider;

				Label _label = (Label)canvas1.Children[i * 3 + 2];
				_label.Content = _slider.Value.ToString();
				canvas1.Children[i * 3 + 2] = _label;
			}
		}

		private void Uav_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			Slider _slider = sender as Slider;
			_slider.Value = Math.Round(_slider.Value, 0, MidpointRounding.AwayFromZero);
			string _str = _slider.Name.Replace("s", "");
			int i = int.Parse(_str);

			if (i <= this._altitudeList.Count)
			{
				this._altitudeList[i] = _slider.Value;
			}
			_id = i + 1;
			Label _label = (Label)canvas1.Children[i * 3 + 2];
			_label.Content = _slider.Value.ToString();
			canvas1.Children[i * 3 + 2] = _label;

			SetValue(ValueProperty, _slider.Value);
			//Value = _slider.Value;
		}

		private void UAV_ValueChanged_1(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			AllSlider.Value = Math.Round(AllSlider.Value, 0, MidpointRounding.AwayFromZero);
			if (f)
			{
				int k = canvas1.Children.Count / 3;
				for (int i = 0; i < k; i++)
				{
					Slider _slider = (Slider)canvas1.Children[i * 3];
					_slider.Value = AllSlider.Value;
					canvas1.Children[i * 3 ] = _slider;
					this._altitudeList[i] = AllSlider.Value;
				}
				//AllValue = AllSlider.Value;
				SetValue(AllValueProperty, AllSlider.Value);
			}
		}
		private void CheckBox_Checked(object sender, RoutedEventArgs e)
		{
			var _check = sender as CheckBox;
			f =(bool)_check.IsChecked;
			if (f)
			{
				int k = canvas1.Children.Count / 3;
				for (int i = 0; i < k; i++)
				{
					Slider _slider = (Slider)canvas1.Children[i * 3];
					_slider.Value = AllSlider.Value;
					canvas1.Children[i * 3] = _slider;
				}
			}
		}
		private void Max_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			if (canvas1.Children.Count > 0)
			{
				int k = canvas1.Children.Count / 3;
				for (int i = 0; i < k; i++)
				{
					Slider _slider = (Slider)canvas1.Children[i * 3];
					_slider.Maximum = (double)_Max.Value;
					canvas1.Children[i * 3] = _slider;
				}

			}
		}
		private void Min_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			if (canvas1.Children.Count > 0)
			{
				int k = canvas1.Children.Count / 3;
				for (int i = 0; i < k; i++)
				{
					Slider _slider = (Slider)canvas1.Children[i * 3];
					_slider.Minimum = (double)_Min.Value;
					canvas1.Children[i * 3] = _slider;
				}
			}

		}

		public void Clear()
		{
			canvas1.Children.Clear();
			_altitudeList.Clear();
			counter = 0;
			_id = 0;
			f = false;
			canvas1.Width = 20;
		}

		#region イベント
		/// <summary>
		/// 値変更時に呼ばれるイベントです。
		/// </summary>
		public static readonly RoutedEvent ValueChangedEvent =
			EventManager.RegisterRoutedEvent(
				"ValueChanged", RoutingStrategy.Bubble,
				typeof(RoutedPropertyChangedEventHandler<double>),
				typeof(AltitudeProperty));

		/// <summary>
		/// 値変更時に呼ばれるイベントです。
		/// </summary>
		public event RoutedPropertyChangedEventHandler<double> ValueChanged
		{
			add { AddHandler(ValueChangedEvent, value); }
			remove { RemoveHandler(ValueChangedEvent, value); }
		}


		public static readonly RoutedEvent AllValueChangedEvent =
			EventManager.RegisterRoutedEvent(
				"AllValueChanged", RoutingStrategy.Bubble,
				typeof(RoutedPropertyChangedEventHandler<double>),
				typeof(AltitudeProperty));

		/// <summary>
		/// 値変更時に呼ばれるイベントです。
		/// </summary>
		public event RoutedPropertyChangedEventHandler<double> AllValueChanged
		{
			add { AddHandler(AllValueChangedEvent, value); }
			remove { RemoveHandler(AllValueChangedEvent, value); }
		}
		#endregion

		#region 内部イベント

		#endregion

		public void AltitudeListUpdate()
		{
			if (this._altitudeList.Count != counter)
			{
				AltitudeAdd();
				AltitudeUpdate();
			}
			else
			{
				AltitudeUpdate();
			}
		}

		#region プロパティ
		public List<double> AltitudeList
		{
			get { return (List<double>)this._altitudeList; }
			set 
			{ 
				this._altitudeList = value;

				if (this._altitudeList.Count != counter)
				{
					AltitudeAdd();
					AltitudeUpdate();
				}
				else
				{
					AltitudeUpdate();
				}
			
			}
		}
		public double HomeAltitude
		{
			get { return (double)Home.Value; }
			set { Home.Value = Math.Round(value, 3, MidpointRounding.AwayFromZero); }
		}
		public double UAVAltitude
		{
			get { return (double)UAV.Value; }
			set { UAV.Value = Math.Round(value, 3, MidpointRounding.AwayFromZero);}
		}
		public int ID
		{
			get { return (int)_id; }
			set { _id = value; }
		}

		#endregion

		#region プロパティメソッド
		public static readonly DependencyProperty ValueProperty =
	DependencyProperty.Register(
		"Value", typeof(double), typeof(AltitudeProperty),
		new FrameworkPropertyMetadata(
					default(double),
					FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
					OnValueChanged));

		public double Value
		{
			get { return (double)GetValue(ValueProperty); }
			set { SetValue(ValueProperty, value); }
		}
		
		private static void OnValueChanged(DependencyObject d,
								   DependencyPropertyChangedEventArgs e)
		{
			AltitudeProperty control = (AltitudeProperty)d;
			control._altitudeList[control._id - 1] = (double)e.NewValue;

			RoutedPropertyChangedEventArgs<double> args = new RoutedPropertyChangedEventArgs<double>(
				(double)e.OldValue, (double)e.NewValue, ValueChangedEvent);
			control.OnValueChanged(args);
		}
		protected virtual void OnValueChanged(RoutedPropertyChangedEventArgs<double> args)
		{
			RaiseEvent(args);
		}

		public static readonly DependencyProperty AllValueProperty =
	DependencyProperty.Register(
		"AllValue", typeof(double), typeof(AltitudeProperty),
		new FrameworkPropertyMetadata(
					default(double),
					FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
					OnAllValueChanged));

		public double AllValue
		{
			get { return (double)GetValue(AllValueProperty); }
			set { SetValue(AllValueProperty, value); }
		}

		private static void OnAllValueChanged(DependencyObject d,
								   DependencyPropertyChangedEventArgs e)
		{
			AltitudeProperty control = (AltitudeProperty)d;
			control.AllValue = (double)e.NewValue;

			RoutedPropertyChangedEventArgs<double> args = new RoutedPropertyChangedEventArgs<double>(
				(double)e.OldValue, (double)e.NewValue, AllValueChangedEvent);
			control.OnAllValueChanged(args);
		}
		protected virtual void OnAllValueChanged(RoutedPropertyChangedEventArgs<double> args)
		{
			RaiseEvent(args);
		}

		#endregion
	}
}
