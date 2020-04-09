namespace GMap.NET.MapProviders
{
    using GMapNetInterFace;

    using GMap.NET.Internals;
    using GMap.NET.MapProviders;
    using GMap.NET.Projections;
    using GMap.NET.Properties;
    using GMap.NET.CacheProviders;

    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Globalization;
    using System.Xml;
    using System.Security.Cryptography;

    using System.Drawing;
    using System.Drawing.Imaging;
    using System.Drawing.Design;
    using System.IO;
    using System.Reflection;


    /// <summary>
    /// DllMapProviderBase
    /// </summary>
    public abstract class DllMapProviderBase : GMapProvider
    {
        #region GMapProvider Members

        /// <summary>
        /// Id
        /// </summary>
        public override Guid Id
        {
            get
            {
                throw new NotImplementedException();
            }
        }


        /// <summary>
        /// Name
        /// </summary>
        public override string Name
        {
            get
            {
                throw new NotImplementedException();
            }
        }


        /// <summary>
        /// Projection
        /// </summary>
        public override PureProjection Projection
        {
            get
            {
                return MercatorProjection.Instance;
            }
        }

        GMapProvider[] overlays;
        /// <summary>
        /// Overlays
        /// </summary>
        public override GMapProvider[] Overlays
        {
            get
            {
                if (overlays == null)
                {
                    overlays = new GMapProvider[] { this };
                }
                return overlays;
            }
        }

        /// <summary>
        /// GetTileImage
        /// </summary>
        public override PureImage GetTileImage(GPoint pos, int zoom)
        {
            throw new NotImplementedException();
        }
        #endregion
    }



    /// <summary>
    /// DllMapProvider
    /// </summary>
    public class DllMapProvider : DllMapProviderBase
    {
        // DLLのロケーション
        private static string _strDllFileName = "Dummy.dll";

        /// <summary>
        /// GUID
        /// <summary>
        private static Guid id = new Guid("00403A36-725F-4BC4-934F-BFC1C164D100");

        private static string name = "DummyMap";

        /// <summary>
        /// Instance
        /// </summary>
        public static DllMapProvider Instance;

        private static GMapProvPluginInfo pluginInfo;
        private static GMapNetInterFace.IF_Plugin ifPlugin;

        private static Object thisLock = new Object();


        DllMapProvider()
        {
            #region Comment
            Debug.WriteLine("DllMapProvider started !!");
            #endregion
        }



        static DllMapProvider()
        {
            #region Comment
            Debug.WriteLine("DllMapProvider Instance started !!");
            #endregion

            Instance = new DllMapProvider();
        }


        #region GMapProvider Members

        public override Guid Id
        {
            get
            {
                return id;
            }
        }

        /// <summary>
        /// name
        /// <summary>
        public override string Name
        {
            get
            {
                return name;
            }
        }

        /// <summary>
        /// GetTileImage
        /// <summary>
        public override PureImage GetTileImage(GPoint pos, int zoom)
        {
            int iPF;
            string strFileName;
            string url;

            #region X/Y/Z to Center Lat/LNG

            // ななめ隣のタイル値
            GPoint gpPos2 = new GPoint(pos.X + 1, pos.Y + 1);

            GPoint gpPosPixel = Projection.FromTileXYToPixel(pos);
            GPoint gpPos2Pixel = Projection.FromTileXYToPixel(gpPos2);

            GPoint gpCenterPixel
                = new GPoint(
                    (gpPosPixel.X + gpPos2Pixel.X) / 2,
                    (gpPosPixel.Y + gpPos2Pixel.Y) / 2
                );


            PointLatLng pCenterLatLng = Projection.FromPixelToLatLng(gpCenterPixel, zoom);

            PointLatLng pLatLng1 = Projection.FromPixelToLatLng(gpPosPixel, zoom);
            PointLatLng pLatLng2 = Projection.FromPixelToLatLng(gpPos2Pixel, zoom);

            #endregion

            lock (thisLock)
            {
                Stopwatch sw = new Stopwatch();
                sw.Start();

                // GMap.NETプラグインの実行前パラメータ設定
                pluginInfo.setParam(
                    zoom,
                    pos.X,
                    pos.Y,
                    pCenterLatLng.Lng,
                    pCenterLatLng.Lat,
                    pLatLng1.Lng,
                    pLatLng2.Lng,
                    pLatLng1.Lat,
                    pLatLng2.Lat
                );

                // GMap.NETプラグインの実行
                ifPlugin.Run();

                sw.Stop();

                iPF = ifPlugin.iProcessFlag;
                strFileName = ifPlugin.strFileName;
                url = ifPlugin.strURL;

                // ミリ秒単位で出力
                long millisec = sw.ElapsedMilliseconds;

                #region Comment
                Debug.WriteLine(
                     string.Concat(
                         "Plugin Process Time : ",
                         Convert.ToString(millisec),
                         " msec "
                    )
                );
                #endregion
            }

            PureImage pImage;
            if (iPF == pluginInfo.iProcessFlag_Image)
            //                    strFileName.Length != 0)
            {
				var resultImage = Image.FromFile(strFileName);
				var stream = new MemoryStream();
				resultImage.Save(stream, ImageFormat.Png);
				stream.Position = 0L;

				//                    var pImage = GMapProvider.TileImageProxy.FromArray(stream.ToArray());
				pImage = GMapProvider.TileImageProxy.FromArray(stream.ToArray());

                //                    return pImage;
            }
            else
            {
                //                    return GetTileImageUsingHttp(url);
                pImage = GetTileImageUsingHttp(url);
            }
            return pImage;
        }
        #endregion

		/// <summary>
		/// SetImageFile
		/// </summary>
		public void SetTileImage(String filePath)
		{
			ifPlugin.strFileName = filePath;
		}
		

        /// <summary>
        /// プラグインの種類の変更/設定
        /// </summary>
        /// <param name="strFileName"></param>
        public void setPluging(String strFileName)
        {
            #region Comment
            Debug.WriteLine(
                 string.Concat(
                     "DllMapProvider - setPluging : [",
                     strFileName,
                     "]"
                )
            );
            #endregion

            _strDllFileName = strFileName;

            #region Comment
            foreach (Guid key in GMapProviders.Hash.Keys)
            {
                Debug.WriteLine(
                    string.Concat(
                        "DllMapProvider - setPluging - Hash : ",
                        Convert.ToString(key),
                        "/",
                        Convert.ToString(GMapProviders.Hash[key])
                    )
                );
            }
            foreach (int key in GMapProviders.DbHash.Keys)
            {
                Debug.WriteLine(
                    string.Concat(
                        "DllMapProvider - setPluging - DbHash : ",
                        Convert.ToString(key),
                        "/",
                        Convert.ToString(GMapProviders.DbHash[key])
                    )
                );
            }
            #endregion

            GMapProviders.Hash.Remove(this.Id);
            GMapProviders.DbHash.Remove(this.DbId);

            #region Comment
            Debug.WriteLine(
                string.Concat(
                    "DllMapProvider - setPluging - Hash/DbHash Removed : "
                )
            );
            foreach (Guid key in GMapProviders.Hash.Keys)
            {
                Debug.WriteLine(
                    string.Concat(
                        "DllMapProvider - setPluging - Hash : ",
                        Convert.ToString(key),
                        "/",
                        Convert.ToString(GMapProviders.Hash[key])
                    )
                );
            }
            foreach (int key in GMapProviders.DbHash.Keys)
            {
                Debug.WriteLine(
                    string.Concat(
                        "DllMapProvider - setPluging - DbHash : ",
                        Convert.ToString(key),
                        "/",
                        Convert.ToString(GMapProviders.DbHash[key])
                    )
                );
            }
            #endregion

            //            GMapProvPluginInfo pluginInfo = new GMapProvPluginInfo();
            // ここでCreateInstance
            // 使用するプラグインファイル名からPluginInfoを設定
            pluginInfo = new GMapProvPluginInfo(_strDllFileName);
            // DLLのインスタンス化
            ifPlugin = pluginInfo.getDllInstance();

            // DLL MapProviderのGUIDの設定
            id = pluginInfo.guidDll;
            // DLL MapProviderのNAMEの設定
            name = ifPlugin.strMapName;
            //            id = pluginInfo.getGUIDfromFile(strFileName);

            using (var HashProvider = new SHA1CryptoServiceProvider())
            {
                DbId = Math.Abs(BitConverter.ToInt32(HashProvider.ComputeHash(Id.ToByteArray()), 0));

                #region Comment
                Debug.WriteLine(
                     string.Concat(
                        "Name : [",
                        Name,
                         "] DbId : [",
                         Convert.ToString(DbId),
                        "]"
                    )
                );
                #endregion
            }

            GMapProviders.Hash.Add(this.Id, this);
            GMapProviders.DbHash.Add(this.DbId, this);

            #region Comment
            Debug.WriteLine(
                string.Concat(
                    "DllMapProvider - setPluging - Hash Added : "
                )
            );
            foreach (Guid key in GMapProviders.Hash.Keys)
            {
                Debug.WriteLine(
                    string.Concat(
                        "DllMapProvider - setPluging - Hash : ",
                        Convert.ToString(key),
                        "/",
                        Convert.ToString(GMapProviders.Hash[key])
                    )
                );
            }
            foreach (int key in GMapProviders.DbHash.Keys)
            {
                Debug.WriteLine(
                    string.Concat(
                        "DllMapProvider - setPluging - DbHash : ",
                        Convert.ToString(key),
                        "/",
                        Convert.ToString(GMapProviders.DbHash[key])
                    )
                );
            }
            #endregion

            //MemoryCache MC = new MemoryCache();
            //MC.Clear();

            //GMap.NET.Internals.Core.instances;
            //GMaps.Instance.OnTileCacheProgress;
        }
    }
}
