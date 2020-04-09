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
namespace XMonitor_GUI.CustomMarkers
{
    /// <summary>
    /// Obstacle_Marker.xaml 的交互逻辑
    /// </summary>
    public partial class Obstacle_Marker : UserControl
    {
        public Obstacle_Marker()
        {
            InitializeComponent();
        }
        public Obstacle_Marker(MainWindow window, GMapMarker marker, bool mode, int _no, bool _icon)
        {
            InitializeComponent();
            Main.RenderTransform = new ScaleTransform(0.8, 0.8);
        }
    }
}
