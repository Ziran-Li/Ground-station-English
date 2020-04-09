using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using GMap.NET.WindowsPresentation;
using System.Windows.Threading;

namespace XMonitor_GUI.CustomMarkers
{
    /// <summary>
    /// Area_Choose.xaml 的交互逻辑
    /// </summary>
    public partial class Area_Choose : UserControl
    {
        int First_Open_Timer = 0;
        private DispatcherTimer _Timer;
        private int _TimeDelay = 500;  //定时1秒
        GMapMarker Marker;
        public Area_Choose()
        {
            InitializeComponent();
        }
        public Area_Choose(MainWindow window, GMapMarker marker, bool mode, int _no, bool _icon)
        {
            InitializeComponent();
            this.Marker = marker;
            this.Marker.ID = _no;
            this.Marker.Mode = 0;
            if (_icon)
            {
                icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/Area_ChooseClic.png")));
                Main_AreaMarker.RenderTransform = new ScaleTransform(0.5, 0.5);	//マーカーサイズ縮小
            }
            else
            {
                icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/Area_blue.png")));
                Main_AreaMarker.RenderTransform = new ScaleTransform(0.5, 0.5);	//マーカーサイズ縮小
            }
            Number.Content = _no.ToString()+"区";

            if(First_Open_Timer==0)
            {
                _Timer = new DispatcherTimer();
                _Timer.Interval = TimeSpan.FromMilliseconds(_TimeDelay);
                _Timer.Tick += _Timer_Tick;    //1秒计时完成触发该事件
                _Timer.Start();    //这个可以放在按钮事件里执行
            }
        }
        private void _Timer_Tick(object sender, object e)
        {
            //时间时间处理

            if (!Marker.IsMouseOver && Marker.Mode == -1)
            {
                icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/Area_blue.png")));
                Marker.Mode = 0;
            }
        }


        void AreaMarker_Loaded(object sender, RoutedEventArgs e)
        {

        }
        void AreaMarker_Unloaded(object sender, RoutedEventArgs e)
        {
            try
            {
                this.Unloaded -= new RoutedEventHandler(AreaMarker_Unloaded);
                this.Loaded -= new RoutedEventHandler(AreaMarker_Loaded);
                this.MouseEnter -= new MouseEventHandler(AreaMarkerControl_MouseEnter);
                this.MouseLeave -= new MouseEventHandler(AreaMarkerControl_MouseLeave);
                this.MouseLeftButtonUp -= new MouseButtonEventHandler(AreaMarker_MouseLeftButtonUp);
                this.MouseLeftButtonDown -= new MouseButtonEventHandler(AreaMarker_MouseLeftButtonDown);
            }
            catch
            {

            }

        }
        void AreaMarkerControl_MouseLeave(object sender, MouseEventArgs e)
        {
            try
            {
                Main_AreaMarker.RenderTransform = new ScaleTransform(0.5, 0.5);	//マーカーサイズ縮小
            }
            catch
            {

            }

        }
        void AreaMarkerControl_MouseEnter(object sender, MouseEventArgs e)
        {
            try
            {
                Main_AreaMarker.RenderTransform = new ScaleTransform(1, 1);	//マーカーサイズ標準
            }
            catch
            {

            }
        }
        void AreaMarker_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            try
            {
                Marker.IsMouseOver = true;
                icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/Area_ChooseClic.png")));
            }
            catch
            {

            }

        }
        void AreaMarker_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Marker.IsMouseOver = false;
            Main_AreaMarker.RenderTransform = new ScaleTransform(0.5, 0.5);
        }

        void AreaMarkerControl_TouchLeave(object sender, TouchEventArgs e)
        {
            try
            {
                Main_AreaMarker.RenderTransform = new ScaleTransform(0.5, 0.5);	//マーカーサイズ縮小
            }
            catch
            {

            }

        }
        void AreaMarkerControl_TouchEnter(object sender, TouchEventArgs e)
        {
            try
            {
                Main_AreaMarker.RenderTransform = new ScaleTransform(1, 1);	//マーカーサイズ標準
            }
            catch
            {

            }
        }
        void AreaMarkerControl_TouchDown(object sender, TouchEventArgs e)
        {
            try
            {
                Marker.IsMouseOver = true;
                icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/Area_ChooseClic.png")));
            }
            catch
            {

            }

        }
        void AreaMarkerControl_TouchUp(object sender, TouchEventArgs e)
        {
            Marker.IsMouseOver = false;
            Main_AreaMarker.RenderTransform = new ScaleTransform(0.5, 0.5);
        }
    }
}
