using System;
using System.Windows;
using System.Windows.Controls;
using System.Diagnostics;       //Process関連
using System.Runtime.InteropServices; //Win32 APIやDLL関数を呼び出す DllImport
using System.Collections.Generic;

namespace XMonitor_GUI
{
    /// <summary>
    /// ApplicationWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class ApplicationWindow : UserControl
    {
        private int Window_Id;       // オプション画面ID
		private List<string> _appList;

        public int ID
        {
            get { return (int)Window_Id; }
            set { Window_Id = value; }
        }

        public ApplicationWindow(int id)
        {
            InitializeComponent();

            Width = double.NaN;
            Height = double.NaN;

            Window_Id = id;
			_appList = new List<string>();

            //string[] files = System.IO.Directory.GetFiles(System.IO.Path.GetFullPath(@"App/"), "*.*", System.IO.SearchOption.AllDirectories);
            
            foreach (string file in System.IO.Directory.GetFiles( System.Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\X-Monitor\App", "*.*", System.IO.SearchOption.AllDirectories))
            {
                string[] AppName = file.Split('\\');
                if (-1 == AppList.Items.IndexOf(AppName[8]))
                {
                    AppList.Items.Add(AppName[8]);
                }
            }
            AppList.Items.RemoveAt(0);
        }
        // 「参照」ボタンクリック時：表示するアプリケーションを選択
        private void OpenApplication_Click(object sender, RoutedEventArgs e)
        {
            // アプリケーションファイル開くダイアログ
            OpenApplicationFile();
        }
        // アプリケーションファイル開くダイアログ
        void OpenApplicationFile()
        {
            try
            {
                // Instantiate the dialog box
                Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();

                // Configure open file dialog box
                dlg.InitialDirectory = System.Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\X-Monitor\App";
                dlg.FileName = "Application";                  // Default file name
                dlg.DefaultExt = ".exe";                    // Default file extension
				dlg.Filter = "Application|*.exe|すべてのファイル (*.*)|*.*"; // Filter files by extension

                // Open the dialog box modally
                Nullable<bool> result = dlg.ShowDialog();

                // Process open file dialog box results
                if (result == true)
                {
                    ApplicationOpen(dlg.FileName);
                }
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("(701) OptionWindow. Open Application Error: " + ex.Message);

            }
        }
        // アプリケーションリストから選択
        private void ApplicationSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string AppName = (string)AppList.SelectedItem;
            ApplicationOpen(System.Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\X-Monitor\App\" + AppName);
        }

        // WinWPI関連
        const int SW_MAXIMIZE = 3;              //最大化
        const int GWL_STYLE = -16;              //ウィンドウスタイル
        const int WS_CAPTION = 0x00C00000;      //タイトルバー     
        [DllImport("USER32.DLL", CharSet = CharSet.Auto)]
        private static extern System.IntPtr SetParent( System.IntPtr hWndChild,System.IntPtr hWndNewParent);
        [DllImport("User32.Dll")]
        static extern int ShowWindow(IntPtr hWnd, int nCmdShow);
        [DllImport("User32.Dll")]
        private static extern int GetWindowLong(IntPtr hWnd, int nIndex);
        [DllImport("User32.Dll")]
        private static extern int SetWindowLong(IntPtr hWnd, int nIndex, int dwLong);


        // アプリケーション起動
        private void ApplicationOpen(string file)
        {
			try
			{
				// プロセス起動時の詳細情報を設定
				ProcessStartInfo init = new ProcessStartInfo();
				init.FileName = file;  // 実行ファイルパス

				// プロセス起動
				var process = Process.Start(init);
				process.WaitForInputIdle();         // プロセスがアイドル状態になるまで待機

				_appList.Add(process.ProcessName);

				IntPtr ptr = IntPtr.Zero;
				while ((ptr = process.MainWindowHandle) == IntPtr.Zero) ;

				// 外部アプリ画面に反映
				SetParent(process.MainWindowHandle, _panel.Handle);

				//最大化（起動したS/Wウィンドウにフィットさせる）
				ShowWindow(process.MainWindowHandle, SW_MAXIMIZE);

				// タイトルバーを非表示
				var windowStyle = GetWindowLong(process.MainWindowHandle, GWL_STYLE);
				windowStyle &= ~WS_CAPTION;
				windowStyle = SetWindowLong(process.MainWindowHandle, GWL_STYLE, windowStyle);
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(701-1) OptionWindow. Open Application Error: " + ex.Message);

			}
        }

		private void UserControl_Unloaded(object sender, RoutedEventArgs e)
		{
			try
			{
				//Process process = new Process();
				//// 外部アプリ画面に反映
				//SetParent(process.MainWindowHandle, _panel.Handle);
				//process.Close();
				//process.Dispose();

				foreach (var _app in _appList)
				{
					//notepadのプロセスを取得
					System.Diagnostics.Process[] ps = System.Diagnostics.Process.GetProcessesByName(_app);

					//foreach (System.Diagnostics.Process p in ps)
					//{
					//	//クローズメッセージを送信する
					//	p.CloseMainWindow();

					//	//プロセスが終了するまで最大2秒待機する
					//	p.WaitForExit(10000);
					//}
					foreach (System.Diagnostics.Process p in ps)
					{
						//プロセスを強制的に終了させる
						p.Kill();
					}
				}
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(701-2) OptionWindow. Open Application Error: " + ex.Message);

			}
		}
    }
}
