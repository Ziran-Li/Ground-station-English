using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;
using System.Windows.Shapes;
using System.ComponentModel;

namespace ControlsUnit
{
	/// <summary>
	/// SpeedView.xaml の相互作用ロジック
	/// </summary>
	public partial class SpeedView : UserControl
	{
		private double _speed;

		public SpeedView()
		{
			InitializeComponent();
			_speed = 0;
		}

		public event PropertyChangedEventHandler PropertyChanged = delegate { };

		protected virtual void OnPropertyChanged(string prop)
		{
			PropertyChanged(this, new PropertyChangedEventArgs(prop));
		}

		private double pctComplete = 0.0;
		public double PctComplete
		{
			get { return pctComplete; }
			set
			{
				if (pctComplete != value)
				{
					pctComplete = value;
					OnPropertyChanged("PctComplete");
				}
			}
		}

		public void SpeedValue(double _x, double _y, double _z)
		{
			double _xx = _x * _x;
			double _yy = _y * _y;
			double _zz = _z * _z;

			double _spd = Math.Sqrt(_xx + _yy+ _zz);
			SpdProBar.Value = _spd;
			SpdVal.Content = _spd.ToString("0.00");
		}
		public double Speed
		{
			get{return (double)_speed ; }
			set 
			{ 
				_speed  = value;
				SpdProBar.Value = _speed ;
				SpdVal.Content = _speed .ToString("0.00");
			}
		}
	}

	static class ConstantsCircularProgress
	{
		public const double startAngle = 0;
		public const double endAngle = 90;
		public const double MAX = 10;			//最大速度
	}

	public class DoubleToPctConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			string result = "";
			double d = (double)value;
			result = string.Format("{0}m/s", d);

			return result;
		}

		public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}

	public class CircularProgress : Shape
	{
		static CircularProgress()
		{
			Brush myGreenBrush = new SolidColorBrush(Color.FromArgb(255, 6, 176, 37));
			myGreenBrush.Freeze();

			StrokeProperty.OverrideMetadata(
				typeof(CircularProgress),
				new FrameworkPropertyMetadata(System.Windows.Media.Brushes.Green));
			FillProperty.OverrideMetadata(
				typeof(CircularProgress),
				new FrameworkPropertyMetadata(Brushes.Transparent));

			StrokeThicknessProperty.OverrideMetadata(
				typeof(CircularProgress),
				new FrameworkPropertyMetadata(20.0));
		}

		// Value (0-100)
		public double Value
		{
			get { return (double)GetValue(ValueProperty); }
			set { SetValue(ValueProperty, value); }
		}

		// DependencyProperty - Value (0 - 100)
		private static FrameworkPropertyMetadata valueMetadata =
				new FrameworkPropertyMetadata(
					0.0,     // Default value
					FrameworkPropertyMetadataOptions.AffectsRender,
					null,    // Property changed callback
					new CoerceValueCallback(CoerceValue));   // Coerce value callback

		public static readonly DependencyProperty ValueProperty =
			DependencyProperty.Register("Value", typeof(double), typeof(CircularProgress), valueMetadata);

		private static object CoerceValue(DependencyObject depObj, object baseVal)
		{
			double val = (double)baseVal;
			val = Math.Min(val, 9.999);
			val = Math.Max(val, 0.0);
			return val;
		}

		protected override Geometry DefiningGeometry
		{
			get
			{
				double startAngle = ConstantsCircularProgress.startAngle;
				double endAngle = ConstantsCircularProgress.startAngle - ((Value / ConstantsCircularProgress.MAX) * ConstantsCircularProgress.endAngle);

				double maxWidth = Math.Max(0.0, RenderSize.Width - StrokeThickness);
				double maxHeight = Math.Max(0.0, RenderSize.Height - StrokeThickness);

				double xStart = maxWidth / 2.0 * Math.Cos(startAngle * Math.PI / 180.0);
				double yStart = maxHeight / 2.0 * Math.Sin(startAngle * Math.PI / 180.0);

				double xEnd = maxWidth / 2.0 * Math.Cos(endAngle * Math.PI / 180.0);
				double yEnd = maxHeight / 2.0 * Math.Sin(endAngle * Math.PI / 180.0);

				StreamGeometry geom = new StreamGeometry();
				using (StreamGeometryContext ctx = geom.Open())
				{
					ctx.BeginFigure(
						new Point((RenderSize.Width / 2.0) + xStart,
								  (RenderSize.Height / 2.0) - yStart),
						true,   // Filled
						false);  // Closed
					ctx.ArcTo(
						new Point((RenderSize.Width / 2.0) + xEnd,
								  (RenderSize.Height / 2.0) - yEnd),
						new Size(maxWidth / 2.0, maxHeight / 2),
						0.0,     // rotationAngle
						(startAngle - endAngle) > 180,   // greater than 180 deg?
						SweepDirection.Clockwise,
						true,    // isStroked
						false);
					//    ctx.LineTo(new Point((RenderSize.Width / 2.0), (RenderSize.Height / 2.0)), true, true);
				}

				return geom;
			}
		}
	}
}
