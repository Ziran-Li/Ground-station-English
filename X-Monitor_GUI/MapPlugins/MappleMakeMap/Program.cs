using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace makeMapData
{
    static class Program
    {
        static public double dInLongW;
        static public double dInLatW;
        static public int iInZoom;
        static public string strInOutputFileName;
        static public double dMapLeftLngW;
        static public double dMapRightLngW;
        static public double dMapUpperLatW;
        static public double dMapLowerLatW;

        /// <summary>
        /// アプリケーションのメイン エントリ ポイントです。
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            //MessageBox.Show("makeMapData開始！");

            if (args.Length != 8)
            {
                // ポップアップ表示する。
                MessageBox.Show("コマンドライン引数はありません。");
            }
            else
            {
                dInLongW = double.Parse(args[0]);
                dInLatW = double.Parse(args[1]);
                iInZoom = int.Parse(args[2]);
                strInOutputFileName = String.Copy(args[3]);

                dMapLeftLngW = double.Parse(args[4]);
                dMapRightLngW = double.Parse(args[5]);
                dMapUpperLatW = double.Parse(args[6]);
                dMapLowerLatW = double.Parse(args[7]);
            }
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }
    }
}
