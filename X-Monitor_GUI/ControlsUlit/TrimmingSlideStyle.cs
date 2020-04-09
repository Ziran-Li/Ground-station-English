using System;
using System.Windows.Data;

namespace ControlsUnit
{
    public class TrimmingSlideStyle : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {

            double _time = (double)value;
            int m = (int)(_time / 60);
            int s = (int)(_time - (m * 60));
            int ms = (int)((_time - (int)(_time)) * 1000);

            // 経過秒→分:秒:ミリ秒
            string text = m.ToString() + ":" + s.ToString() + "." + ms.ToString();

			System.Diagnostics.Debug.WriteLine("Timer: " + (_time - (int)(_time)));
			System.Diagnostics.Debug.WriteLine("Timer: " + _time);
			System.Diagnostics.Debug.WriteLine("TrimmingSlide: " + text + "\n\n");

            return text;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
