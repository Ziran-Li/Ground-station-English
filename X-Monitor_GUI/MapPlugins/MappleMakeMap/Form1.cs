using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

using System.IO;

using MpgCommon;
using MpgMap;

namespace makeMapData
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            ////////////////////////////////////////
            //Encoding sjisEnc = Encoding.GetEncoding("Shift_JIS");
            //StreamWriter writer =
            //  new StreamWriter(@"Test.txt", true, sjisEnc);
            ////////////////////////////////////////

            try
            {
                // 地図移動モード（マウスドラッグで地図を移動なし）に変更する。
                mpgMap1.MapMode = MapMode.Write;

                // false := 地図中心マーカーを表示しません。 
                mpgMap1.CenterMarker = false;

                // false := 地図中心マーカーを表示しません。 
                mpgMap1.UserMarker = false;

                ////////////////////////////////////////
                //writer.WriteLine("---------- Input Data ----------");
                //writer.WriteLine(Program.dInLongW.ToString());
                //writer.WriteLine(Program.dInLatW.ToString());
                //writer.WriteLine(Program.iInZoom.ToString());
                //writer.WriteLine(Program.strInOutputFileName.ToString());
                //writer.WriteLine(Program.dMapLeftLngW.ToString());
                //writer.WriteLine(Program.dMapRightLngW.ToString());
                //writer.WriteLine(Program.dMapUpperLatW.ToString());
                //writer.WriteLine(Program.dMapLowerLatW.ToString());
                ////////////////////////////////////////

                // オブジェクトを作成する。
                MpgTransform.Tky2Jgd obj = MpgTransform.Tky2Jgd.GetInstance("latest");
                // オブジェクトが生成されなかった場合はエラーとする。
                if (obj == null)
                {
                    MessageBox.Show("シリアル番号が不正です。");
                    //自分自身のフォームを閉じる(アプリケーションが終了)
                    this.Close(); 
                    return;
                }

                // 世界測地系中心位置の設定
                int iLongW = (int)(Program.dInLongW * 3600000.0);
                int iLatW = (int)(Program.dInLatW * 3600000.0);

                // 世界測地系 -> 日本測地系変換
                PointLL ptCenter = new PointLL(iLongW, iLatW);
                ptCenter = obj.ToTky(ptCenter);

                // 中心位置の設定
                mpgMap1.MapCenter = ptCenter;

                // 地図の縮尺を設定する。
                int iLeftLngW = (int)(Program.dMapLeftLngW * 3600000.0);
                int iRightLngW = (int)(Program.dMapRightLngW * 3600000.0);
                int iUpperLatW = (int)(Program.dMapUpperLatW * 3600000.0);
                int iLowerLatW = (int)(Program.dMapLowerLatW * 3600000.0);

                // 世界測地系 -> 日本測地系変換
                PointLL ptRU = new PointLL(iRightLngW, iUpperLatW);
                ptRU = obj.ToTky(ptRU);
                PointLL ptRL = new PointLL(iRightLngW, iLowerLatW);
                ptRL = obj.ToTky(ptRL);

                PointLL ptLU = new PointLL(iLeftLngW, iUpperLatW);
                ptLU = obj.ToTky(ptLU);
                PointLL ptLL = new PointLL(iLeftLngW, iLowerLatW);
                ptLL = obj.ToTky(ptLL);

                // 2点間の距離m単位
                double dblMDistanceLeft = mpgMap1.CalcDistance(ptLU, ptLL);
                double dblMDistanceRight = mpgMap1.CalcDistance(ptRU, ptRL);
                double dblMDistanceUpper = mpgMap1.CalcDistance(ptRU, ptLU);
                double dblMDistanceLower = mpgMap1.CalcDistance(ptRL, ptLL);

                PointF dpiDummy = new PointF();
                dpiDummy.X = 1.0F;
                dpiDummy.Y = 1.0F;
                // 画面の解像度 dot/Inch
                PointF dpiReal = mpgMap1.SetDPI(dpiDummy.X, dpiDummy.Y);

                mpgMap1.SetDPI(dpiReal.X, dpiReal.Y);

                ////////////////////////////////////////
                //writer.WriteLine("==============================");
                //writer.WriteLine(dblMDistanceLeft.ToString() + " m(Left)");
                //writer.WriteLine(dblMDistanceRight.ToString() + " m(Right)");
                //writer.WriteLine(dblMDistanceUpper.ToString() + " m(Upper)");
                //writer.WriteLine(dblMDistanceLower.ToString() + " m(Lower)");
                //writer.WriteLine(dpiReal.X.ToString() + " dpi");
                //writer.WriteLine(dpiReal.Y.ToString() + " dpi");
                ////////////////////////////////////////

                // 2点間の距離inch単位
                double dInch = dblMDistanceLeft / 0.0254;
                //            double dInch = dblMDistanceLower / 0.0254;

                ////////////////////////////////////////
                //writer.WriteLine(dInch.ToString() + " inch");
                ////////////////////////////////////////

                // 画面の解像度 画面上での距離inch単位
                double dInchGamen = 256.0 / dpiReal.X;

                ////////////////////////////////////////
                //writer.WriteLine(dInchGamen.ToString() + " inch");
                ////////////////////////////////////////

                // 地図縮尺を設定
                mpgMap1.MapScale = (int)(dInch / dInchGamen);

                mpgMap1.RePaint();

                // 地図をファイル出力する
                int w = mpgMap1.Size.Width; // コントロールの幅
                int h = mpgMap1.Size.Height; // コントロールの高さ

                ////////////////////////////////////////
                //writer.WriteLine("w = " + w.ToString());
                //writer.WriteLine("h = " + h.ToString());
                ////////////////////////////////////////

                // 変換する緯度経度を設定する。
                PointLL[] geos = new PointLL[]
        　　    {
　　　　            ptRU, ptRL, ptLU, ptLL
                };
                // 緯度経度をPixel座標に変換する。
                Point[] pxls = mpgMap1.Convert(geos);
                //writer.WriteLine(pxls[0].X + ", " + pxls[0].Y);
                //writer.WriteLine(pxls[1].X + ", " + pxls[1].Y);
                //writer.WriteLine(pxls[2].X + ", " + pxls[2].Y);
                //writer.WriteLine(pxls[3].X + ", " + pxls[3].Y);
                int iBmpW = pxls[0].X - pxls[2].X;
                int iBmpH = pxls[1].Y - pxls[0].Y;

                //writer.WriteLine("Bmp Size w = " + iBmpW.ToString() + ", h = " + iBmpH.ToString() );

                //            using (Bitmap bmp = new Bitmap(iBmpW, iBmpH))
                using (Bitmap bmp = new Bitmap(w, h))
                {
                    mpgMap1.DrawToBitmap(bmp, new Rectangle(0, 0, w, h));

                    Bitmap dest = new Bitmap(256, 256);
                    Graphics g = Graphics.FromImage(dest);
                    g.InterpolationMode = InterpolationMode.HighQualityBicubic;

                    //                g.DrawImage(bmp, 0, 0, 256, 256);
                    g.DrawImage(bmp, new Rectangle(0, 0, 256, 256), new Rectangle(pxls[2].X, pxls[0].Y, iBmpW, iBmpH), GraphicsUnit.Pixel);
                    dest.Save(Program.strInOutputFileName, System.Drawing.Imaging.ImageFormat.Png);

                    //                bmp.Save(Program.strInOutputFileName + ".png" , System.Drawing.Imaging.ImageFormat.Png);
                }

                // 切り出した画像の中心の緯度経度を求める。
                // Pixel値
                int iCenterX = (int)(pxls[0].X + pxls[2].X) / 2;
                int iCenterY = (int)(pxls[1].Y + pxls[0].Y) / 2;
                //writer.WriteLine("Center " + iCenterX + ", " + iCenterY);
            }
            catch (Exception)
            {
                //writer.WriteLine(except.Message.ToString());
            }

            ////////////////////////////////////////
            //writer.Close();
            ////////////////////////////////////////

            //自分自身のフォームを閉じる(アプリケーションが終了)
            this.Close();
        }

        private void mpgMap1_Load(object sender, EventArgs e)
        {

        }
    }
}
