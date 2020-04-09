using System.Windows.Controls;
using System.Windows.Media.Media3D;
using HelixToolkit.Wpf;

namespace XMonitor_GUI.Window
{
    /// <summary>
    /// ModelWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class ModelWindow : UserControl
    {
        private int Window_Id;       // オプション画面ID
        private double UAV_Roll;     // 姿勢角(Roll)
        private double UAV_Pitch;    // 姿勢角(Pitch)
        private double UAV_Yaw;      // 姿勢角(Yaw)


        public ModelWindow(int id, double r, double p ,double y)
        {
            InitializeComponent();

            Width = double.NaN;
            Height = double.NaN;

			//3Dモデル軽量化　http://d.hatena.ne.jp/thegoodbadugly/20121211/1355204093

            //var reader = new HelixToolkit.Wpf.StudioReader();
            //Model3DGroup model = reader.Read(System.IO.Path.GetFullPath(@"Resources/Model.3ds"));
            //TestMode_bus.Content = model;

			//var reader = new HelixToolkit.Wpf.ObjReader();
			//Model3DGroup MyModel = reader.Read(System.IO.Path.GetFullPath(@"Resources/untitled.obj"));
			//TestMode_bus.Content = MyModel;

            //StLReader reader = new StLReader();
            //Model3DGroup MyModel = reader.Read(System.IO.Path.GetFullPath(@"Resources/Model.stl"));
            //TestMode_bus.Content = MyModel;
            //model1.Source = HelixToolkit.Wpf.ModelImporter.Load(@"Resources/Model.3ds");
            //model1.Source = System.IO.Path.GetFullPath(@"Resources/Model.3ds");

			//var reader = new OffReader();
			//Model3DGroup MyModel = reader.Read(System.IO.Path.GetFullPath(@"Resources/Model.off"));
			//TestMode_bus.Content = MyModel;

			model1.Source = System.IO.Path.GetFullPath(@"Resources/Model.3ds");

            Window_Id   = id;
            UAV_Roll    = r;
            UAV_Pitch   = p;
            UAV_Yaw     = y;

			RotateModel(r,p,y);
        }

        public void RotateModel(double r, double p, double y)
        {
            Matrix3D m = Matrix3D.Identity;
            m.Rotate(new Quaternion(new Vector3D(1, 0, 0), r ));
            m.Rotate(new Quaternion(new Vector3D(0, 1, 0), p ));
            m.Rotate(new Quaternion(new Vector3D(0, 0, 1), y));

            MatrixTransform3D rotate = new MatrixTransform3D();
            rotate.Matrix = m;
            //RotateTransform3D rotate = new RotateTransform3D();
            //rotate.Rotation = new AxisAngleRotation3D(new Vector3D(0, 0, 1), UAV_Roll);
            TestMode_bus.Transform = rotate;

			RollValue.Content = r.ToString("0.0");
			PitchValue.Content = p.ToString("0.0");
			YawValue.Content = y.ToString("0.0");
        }

        public int ID
        {
            get { return (int)Window_Id; }
            set { Window_Id = value; }
        }
        public double Roll
        {
            get { return (double)UAV_Roll; }
            set { 
                UAV_Roll = value;
                /*
                RotateTransform3D rotate = new RotateTransform3D();
                rotate.Rotation = new AxisAngleRotation3D(new Vector3D(0, 0, 1), UAV_Roll);
                TestMode_bus.Transform = rotate;
                System.Diagnostics.Debug.WriteLine("UAV_Roll:" + UAV_Roll.ToString());
                 * */
            }
        }
        public double Pitch
        {
            get { return (double)UAV_Pitch; }
            set
            {
                UAV_Pitch = value;
                /*
                RotateTransform3D rotate = new RotateTransform3D();
                rotate.Rotation = new AxisAngleRotation3D(new Vector3D(1, 0, 0), UAV_Pitch);
                TestMode_bus.Transform = rotate;
                System.Diagnostics.Debug.WriteLine("UAV_Pitch:" + UAV_Pitch.ToString());
                 */ 
            }
        }
        public double Yaw
        {
            get { return (double)UAV_Yaw; }
            set
            {
                UAV_Yaw = value;
                /*
                RotateTransform3D rotate = new RotateTransform3D();
                rotate.Rotation = new AxisAngleRotation3D(new Vector3D(0, 1, 0), UAV_Yaw);
                TestMode_bus.Transform = rotate;
                System.Diagnostics.Debug.WriteLine("UAV_Yaw:" + UAV_Yaw.ToString());
                 */ 
            }
        }

		private void view1_MouseDoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			var _pc = new PerspectiveCamera();
			_pc.Position = new Point3D(-0.651054740895669, -1.07511878013611E-05, -0.285663618246635);
			_pc.LookDirection = new Vector3D(0.654548496951501, 0, 0.327274248475751);
			_pc.FieldOfView = 45;
			_pc.UpDirection = new Vector3D(0, 0, -1);
			_pc.NearPlaneDistance = 0.125;
			view1.Camera = _pc;
			view1.ModelUpDirection = new Vector3D(0, 0, 1);
			view1.ZoomExtentsWhenLoaded = true;
			view1.CameraRotationMode = CameraRotationMode.Trackball;
			view1.ModelUpDirection = new Vector3D(0, 0, 1);
			view1.CoordinateSystemHeight = 53;
			view1.IsHeadLightEnabled = true;
			view1.ShowCoordinateSystem = true;

		}

    }
}
