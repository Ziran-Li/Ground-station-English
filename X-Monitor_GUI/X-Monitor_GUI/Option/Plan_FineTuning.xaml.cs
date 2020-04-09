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

namespace XMonitor_GUI
{
    /// <summary>
    /// Plan_FineTuning.xaml 的交互逻辑
    /// </summary>
    public partial class Plan_FineTuning : UserControl
    {
        int value = 0;
        public Plan_FineTuning(int id)
        {
            InitializeComponent();
        }
        private void Lat_North_Btn_Click(object sender, RoutedEventArgs e)
        {
            bool True_Or_False = MainWindow.pWin.Function_Move_FourthDirection(0);
            if(True_Or_False==false)
            {
                System.Windows.MessageBox.Show("Please Plan The Task First");
            }
        }
        private void Lat_South_Btn_Click(object sender, RoutedEventArgs e)
        {
            bool True_Or_False = MainWindow.pWin.Function_Move_FourthDirection(1);
            if (True_Or_False == false)
            {
                System.Windows.MessageBox.Show("Please Plan The Task First");
            }
        }
        private void Lat_West_Btn_Click(object sender, RoutedEventArgs e)
        {
            bool True_Or_False = MainWindow.pWin.Function_Move_FourthDirection(2);
            if (True_Or_False == false)
            {
                System.Windows.MessageBox.Show("Please Plan The Task First");
            }
        }
        private void Lat_East_Btn_Click(object sender, RoutedEventArgs e)
        {
            bool True_Or_False = MainWindow.pWin.Function_Move_FourthDirection(3);
            if (True_Or_False == false)
            {
                System.Windows.MessageBox.Show("Please Plan The Task First");
            }
        }
        public void Function_ChooseStartLatAndLng_Show()
        {
            Edit_Start_Lat.Content = Constants.Start_ChooseLat.ToString();
            Edit_Start_Lng.Content = Constants.Start_ChooseLng.ToString();
        }

        private void Start_Btn_ChooseSure_Click(object sender, RoutedEventArgs e)
        {
            bool True_Or_False=MainWindow.pWin.Function_NewMake_Plan();
            if (True_Or_False == false)
            {
                System.Windows.MessageBox.Show("Please Make Sure That having The Point Of Taking Off");
            }
        }

        private void Add_Obstacle_Point_Click(object sender, RoutedEventArgs e)
        {
            if(Constants.Obstacle_StartAdd_Flag == 1)
            {
                if (Edit_Obstacle_Lng.Text == "" || Edit_Obstacle_Lng.Text == "0.0"
                    || Edit_Obstacle_Lat.Text == "" || Edit_Obstacle_Lat.Text == "0.0"
                    || Edit_Obstacle_Radius.Text == "" || Edit_Obstacle_Radius.Text == "0.0")
                {
                    System.Windows.MessageBox.Show("Please Enter The Obstacle Point And Radius");
                    return;
                }
                else
                {
                    Constants.Used_Obstacle_Lat = Convert.ToDouble(Edit_Obstacle_Lat.Text);//障碍点纬度
                    Constants.Used_Obstacle_Lng = Convert.ToDouble(Edit_Obstacle_Lng.Text);//障碍点经度
                    Constants.Used_Obstacle_Radius = Convert.ToDouble(Edit_Obstacle_Radius.Text);//障碍点高度
                    MainWindow.pWin.Function_Option_ObstacleAdd();
                }
                Constants.Obstacle_IfInclude_Flag = true;
            }
            else
            {
                System.Windows.MessageBox.Show("Please Start Up The Obstacle Function");
            }

        }

        private void Success_Obstacle_Point_Click(object sender, RoutedEventArgs e)
        {
            if(Constants.Obstacle_StartAdd_Flag == 0)
            {
                Constants.Obstacle_StartAdd_Flag = 1;
                Success_Obstacle_Point.Content = "Obstacle Function Close";
                System.Windows.MessageBox.Show("Start Up The Obstacle Success");
            }
            else if (Constants.Obstacle_StartAdd_Flag == 1)
            {
                Constants.Obstacle_StartAdd_Flag = 0;
                Success_Obstacle_Point.Content = "Obstacle Function Open";
                System.Windows.MessageBox.Show("Close The Obstacle Success");
            }
        }

        private void Delete_Obstacle_Point_Click(object sender, RoutedEventArgs e)
        {
            Constants.Used_Obstacle_Lat = 0.0;//障碍点纬度
            Constants.Used_Obstacle_Lng = 0.0;//障碍点经度
            MainWindow.pWin.Function_Option_ObstacleDelete();
        }
        public void Function_ChooseObstacleLatAndLng_Show()
        {
            Edit_Obstacle_Lng.Text = Constants.Used_Obstacle_Lng.ToString();
            Edit_Obstacle_Lat.Text = Constants.Used_Obstacle_Lat.ToString();
        }
        public double Function_Get_ObstacleRadius()
        {
            if (Edit_Obstacle_Radius.Text == "" || Edit_Obstacle_Radius.Text == "0.0")
            {
                System.Windows.MessageBox.Show("Please Enter The Obstacle Radius");
                return 0;
            }
            Constants.Used_Obstacle_Radius = Convert.ToDouble(Edit_Obstacle_Radius.Text);//障碍点高度
            return Constants.Used_Obstacle_Radius;
        }
        private void Grid_Unloaded(object sender, RoutedEventArgs e)
        {
            Constants.Obstacle_StartAdd_Flag = 0;
        }
        private void Reback_Before_Click(object sender, RoutedEventArgs e)
        {
            if (Constants.WayPointMove_Flag==1)
            {
                Constants.WayPointMove_Flag = 0;//复位状态下可以重新保存记忆点
                MainWindow.pWin.Function_Reback_Before();
            }
        }
       private void RetractionSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
           value = (Int32)RetractionSlider.Value;
           Constants.Retracted_Dis = value;
           Static_Retracted_Show.Content = value.ToString() + "M";
           bool True_Or_False = MainWindow.pWin.Function_Retraction_Area();
           if(True_Or_False==false)
           {
               System.Windows.MessageBox.Show("Please Make The Plan First");
           }
        }
    }
}
