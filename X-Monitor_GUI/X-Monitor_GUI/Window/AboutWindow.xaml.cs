using System.Text;
using System.Windows;
using System.Windows.Input;
using System.Diagnostics;       //Process関連
using System.IO;

namespace XMonitor_GUI
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

		public string XMonitorVersion
		{
			get { return (string)XMonitorVer.Content; }
			set { 
				string _ver = value;
				XMonitorVer.Content = "1. X-Monitor Ver " + _ver;
			}
		}
		public string APFCVersion_L
		{
			get { return (string)APFCVer_L.Content; }
			set
			{
				string _ver = value;
				APFCVer_L.Content = "2. APFC(L) Ver " + _ver;
			}
		}
		public string APFCVersion_H
		{
			get { return (string)APFCVer_H.Content; }
			set
			{
				string _ver = value;
				APFCVer_H.Content = "3. APFC(H) Ver " + _ver;
			}
		}
		private void Updata_Click(object sender, RoutedEventArgs e)
		{
			MessageWindow Mwindow = new MessageWindow();
			Mwindow.flag = true;
            if (Mwindow.Initialize())
            {
                Mwindow.ShowDialog();
            } 
		}
    }
}
