using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
using GMap.NET.WindowsPresentation;
using System.Diagnostics;

using System;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace WpfPrototypeA.CustomMarkers
{
    /// <summary>
    /// UAVWayPoint.xaml の相互作用ロジック
    /// </summary>
    public partial class UAVWayPoint : UserControl
    {
        Popup Popup;
        Label Label;
        GMapMarker Marker;
        MainWindow MainWindow;

        bool EndFlag = true;    //スレッドループ
        bool UavAngSw = false;  //UAVマーカー回転（長押し）
        int ang = 0;

        public UAVWayPoint(MainWindow window, GMapMarker marker, string title)
        {
            InitializeComponent();

            this.MainWindow = window;
            this.Marker = marker;

            Popup = new Popup();
            Label = new Label();

            Popup.Placement = PlacementMode.Mouse;
            {
                Label.Background = Brushes.Blue;
                Label.Foreground = Brushes.White;
                Label.BorderBrush = Brushes.WhiteSmoke;
                Label.BorderThickness = new Thickness(2);
                Label.Padding = new Thickness(5);
                Label.FontSize = 12;
                Label.Content = "Ang/0" + "\nLat/" + this.Marker.Position.Lat.ToString() + "\nLng/" + this.Marker.Position.Lng.ToString();
            }
            Popup.Child = Label;

            var task = Task.Factory.StartNew(() =>
            {
                //int ang = 0;
                int cnt = 0;
                while (EndFlag)
                {
                    // TODO:1/29角度変換が必要　時計回りに360[rad]　X-Monitorは北方向0[rad]右回り+180　左周りに-180

                    if (UavAngSw && cnt > 100)   //cnt（1.5秒長押し）
                    {
                        ang += 1;
                        if (ang > 360) ang = 0;
                    }
                    else if (!UavAngSw)
                    {
                        cnt = 0;
                    }

                    Dispatcher.BeginInvoke(new Action<int>(Worker), ang);
                    System.Threading.Thread.Sleep(10);   //1msec
                    ++cnt;

                }
            });
        }

        public void Worker(int data)
        {
            if (UavAngSw)
            {
                icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/UAVWayPointB.png"))); 
                
                icon.RenderTransform = new RotateTransform { Angle = data };
                Label.Content = "Ang/" + data.ToString() + "\nLat/" + this.Marker.Position.Lat.ToString() + "\nLng/" + this.Marker.Position.Lng.ToString();

            }
            else
            {
                icon.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("Resources/UavWayPointL.png"))); 
            }
        }

        void UAVWayPoint_Loaded(object sender, RoutedEventArgs e)
        {
            // TODO:1/28　何個(8コ)もマーカーを出すとエラーになる
            // CommandBuilder の使用時にエラー メッセージ "'System.NullReferenceException' のハンドルされていない例外" が発生する
           /*
            if (icon.Source.CanFreeze)
            {
                icon.Source.Freeze();
            }
            * */
        }

        void UAVWayPoint_Unloaded(object sender, RoutedEventArgs e)
        {
            this.Unloaded -= new RoutedEventHandler(UAVWayPoint_Unloaded);
            this.Loaded -= new RoutedEventHandler(UAVWayPoint_Loaded);
            this.SizeChanged -= new SizeChangedEventHandler(UAVWayPoint_SizeChanged);
            this.MouseEnter -= new MouseEventHandler(MarkerControl_MouseEnter);
            this.MouseLeave -= new MouseEventHandler(MarkerControl_MouseLeave);
            this.MouseMove -= new MouseEventHandler(UAVWayPoint_MouseMove);
            this.MouseLeftButtonUp -= new MouseButtonEventHandler(UAVWayPoint_MouseLeftButtonUp);
            this.MouseLeftButtonDown -= new MouseButtonEventHandler(UAVWayPoint_MouseLeftButtonDown);

            Marker.Shape = null;
            icon.Source = null;
            icon = null;
            Popup = null;
            Label = null;

            EndFlag = false;
        }

        void UAVWayPoint_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            Marker.Offset = new Point(-e.NewSize.Width / 2, -e.NewSize.Height);
        }

        void UAVWayPoint_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed && IsMouseCaptured)
            {
                Point p = e.GetPosition(MainWindow.MainMap);
                Marker.Position = MainWindow.MainMap.FromLocalToLatLng((int)(p.X), (int)(p.Y));
                Label.Content = "Ang/" + ang.ToString() + "\nLat/" + this.Marker.Position.Lat.ToString() + "\nLng/" + this.Marker.Position.Lng.ToString();
            }
        }

        void UAVWayPoint_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            
            if (!IsMouseCaptured)
            {
                Mouse.Capture(this);
            }

        }

        void UAVWayPoint_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (IsMouseCaptured)
            {
                Mouse.Capture(null);
            }

        }

        void MarkerControl_MouseLeave(object sender, MouseEventArgs e)
        {
            
            Marker.ZIndex -= 10000;
            Popup.IsOpen = false;
        }

        void MarkerControl_MouseEnter(object sender, MouseEventArgs e)
        {
            Marker.ZIndex += 10000;
            Popup.IsOpen = true;
        }

        // TODO:【要確認】Windows8等タッチパネル式の右クリックは「タッチ長押し」
        private void UAVWayPoint_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            UavAngSw = true;
        }

        private void UAVWayPoint_MouseRightButtonUo(object sender, MouseButtonEventArgs e)
        {
            // TODO:1/27 課題としてマーカー外にマウスを移動し、話してもイベントが発生しない。マーカー上でのイベントのみ
            UavAngSw = false;
        }
    }
}