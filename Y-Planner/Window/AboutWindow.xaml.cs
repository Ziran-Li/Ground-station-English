using System.Text;
using System.Windows;
using System.Windows.Input;
using System.Diagnostics;       //Process関連
using System.IO;

namespace Y_Planner
{
    /// <summary>
    /// AboutWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class AboutWindow
    {
        public AboutWindow()
        {
            InitializeComponent();

            StreamReader sr = new StreamReader(System.IO.Path.GetFullPath(@"Info\License.txt"), Encoding.GetEncoding("Shift_JIS"));
            AboutText.Text = sr.ReadToEnd();
            sr.Close();
        }
        private void ButAboutEnd(object sender, RoutedEventArgs e)
        {
            Close();
        }
        private void ButAboutEnd2(object sender, RoutedEventArgs e)
        {
            Close();
        }
        private void AboutMiniSuvURL(object sender, MouseButtonEventArgs e)
        {
            Process.Start("http://mini-surveyor.com/");
        }
        private void AboutACSLURL(object sender, MouseButtonEventArgs e)
        {
            Process.Start("http://www.acsl.co.jp/");
        }
		public string YPlannerVersion
		{
			get { return (string)YPlannerVer.Content; }
			set
			{
				string _ver = value;
				YPlannerVer.Content = "1. Y-Planner Ver " + _ver;
			}
		}
		public string APFCVersion
		{
			get { return (string)APFCVer.Content; }
			set
			{
				string _ver = value;
				APFCVer.Content = "2. APFC Ver " + _ver;
			}
		}
    }
}
