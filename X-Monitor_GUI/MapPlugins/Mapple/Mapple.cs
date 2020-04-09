using System;
using GMapNetInterFace;
using System.Xml.Linq;
using System.Xml.XPath;

namespace Mapple
{
    public class Mapple : GMapNetInterFace.IF_Plugin
    {
        private IF_Host _host;
        private int _iProcessFlag;

        private string _strFileName;

        /// <summary>
        /// ホスト側の設定
        /// </summary>
        public IF_Host Host
        {
            get
            {
                return this._host;
            }
            set
            {
                this._host = value;
            }
        }

        /// <summary>
        /// 取得URL
        /// </summary>
        public int iProcessFlag
        {
            get
            {
                return _iProcessFlag;
            }
        }

        /// <summary>
        /// 取得URL
        /// </summary>
        public string strURL
        {
            get
            {
                return "a";
            }
        }

        /// <summary>
        /// 取得strMapName
        /// </summary>
        public string strMapName
        {
            get
            {
                return "Mapple";
            }
        }

        /// <summary>
        /// 取得strMapName
        /// </summary>
        public string strLicence
        {
            get
            {
                return "© 昭文社";
            }
        }



        /// <summary>
        /// 取得URL
        /// </summary>
        public string strFileName
        {
            get
            {
                return _strFileName;
            }
			set
			{
				_strFileName = value;
			}
        }



        public void Run()
        {
            // MapのXYZ形式でのタイルの左経度
            double dMapLeftLng = _host.dMapLeftLng;
            // MapのXYZ形式でのタイルの右経度
            double dMapRightLng = _host.dMapRightLng;
            // MapのXYZ形式でのタイルの上緯度
            double dMapUpperLat = _host.dMapUpperLat;
            // MapのXYZ形式でのタイルの下緯度
            double dMapLowerLat = _host.dMapLowerLat;

            //プラグインフォルダ
            string folder = System.IO.Path.GetDirectoryName(
                System.Reflection.Assembly
                .GetExecutingAssembly().Location);
            //folder += "\\assistApp";
            if (!System.IO.Directory.Exists(folder))
                throw new ApplicationException(
                    "プラグインフォルダ\"" + folder +
                    "\"が見つかりませんでした。");

            //string strFileName = string.Format(folder + "\\temp\\gmap_{0}_{1}_{2}_{3}.png",
// 
            string settingFilePath = System.Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + @"\ACSL\X-Monitor\Setting\GUI\Settings.xml";

            // XMLファイルからxmlデータに読み込む
            XDocument xml = XDocument.Load(settingFilePath);

            // TempFile読み込み
            var elements = xml.Root.XPathSelectElements("Settings/FilePath/TempFile");
            string temp = string.Empty;
            foreach (var element in elements)
            {
                temp = element.Value;
            }
            
            string strFileName = string.Format(temp + "gmap_{0}_{1}_{2}_{3}.png",
                _host.dMapCenterLat.ToString("0.000000"),
                _host.dMapCenterLng.ToString("0.000000"),
                _host.iMapZoom.ToString(),
                System.IO.Path.GetRandomFileName()
            );

            //ファイルを開く
            System.Diagnostics.Process p = new System.Diagnostics.Process();
            //                System.Diagnostics.Process.Start(strCmd);
            p.StartInfo.FileName = folder + "\\assistApp\\MappleMakeMap.exe";
            p.StartInfo.Arguments = _host.dMapCenterLng.ToString("0.000000") + " "
                + _host.dMapCenterLat.ToString("0.000000") + " "
                + _host.iMapZoom.ToString() + " "
                + strFileName + " "
                + _host.dMapLeftLng.ToString("0.000000") + " "
                + _host.dMapRightLng.ToString("0.000000") + " "
                + _host.dMapUpperLat.ToString("0.000000") + " "
                + _host.dMapLowerLat.ToString("0.000000");

            p.Start();

            //終了するまで最大10秒間だけ待機する
            p.WaitForExit(10000);
            //終了したか確認する
            if (p.HasExited)
            {
                // 終了しました。
                //Console.WriteLine("終了しました。");
            }
            else
            {
                // 終了していません。
                //Console.WriteLine("終了していません。");
            }

            _iProcessFlag = _host.iProcessFlag_Image;
            _strFileName = strFileName;
        }
    }
}
