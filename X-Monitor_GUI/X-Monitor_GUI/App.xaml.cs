using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace XMonitor_GUI
{
    /// <summary>
    /// App.xaml 交互逻辑
    /// </summary>
    public partial class App : Application
    {
        private static System.Threading.Mutex mutex;

        private void Application_Startup(object sender, StartupEventArgs e)
        {
			var name = this.GetType().Assembly.GetName().Name;
			mutex = new System.Threading.Mutex(false, name);

            // 双启动检查
            if (!mutex.WaitOne(0, false))
            {
				// 双启动显示错误并且退出
				if (System.Windows.Forms.MessageBox.Show("不能启动多个应用程序。\n" + "退出当前运行的监视器吗?\n", "监视器结束确认画面", System.Windows.Forms.MessageBoxButtons.YesNo, System.Windows.Forms.MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.No)
				{
					// 互斥锁
					mutex.Close();
					mutex = null;

					//结束
					this.Shutdown();
				}
				else
				{
					System.Diagnostics.Process[] ps = System.Diagnostics.Process.GetProcessesByName("X-Monitor");

					foreach (System.Diagnostics.Process p in ps)
					{
						//发送关闭消息
						p.CloseMainWindow();

						//等待10秒钟以上直到该进程完成
						p.WaitForExit(5000);
						//检查进程是否已经终止
						if (p.HasExited)
						{
							MessageBox.Show("监视器已正常关闭，请重新启动");
							// 互斥字关闭
							mutex.Close();
							mutex = null;

							// 结束 
							this.Shutdown();
						}
						else
						{
							//强行中止进程
							p.Kill();
							MessageBox.Show("在启动监视器过程中中止了，请重新启动");
							// 关闭互斥对象
							mutex.Close();
							mutex = null;

							// 结束
							this.Shutdown();
						}
						
					}
				}

                // 如果双启动，显示错误并且退出
                //MessageBox.Show("双启动警告");

				return;
            }

			var _mainWindow = new MainWindow();
			if (_mainWindow.MainWindowsEnabled())
			{
				_mainWindow.Show();
			}
			else
			{
				// 结束
				this.Shutdown();

				return;
			}
        }

        private void Application_Exit(object sender, ExitEventArgs e)
        {
            if (mutex != null)
            {
				mutex.ReleaseMutex();	// 互斥字解锁
				mutex.Close();		// 互斥字关闭
            }
        }
    }
}
