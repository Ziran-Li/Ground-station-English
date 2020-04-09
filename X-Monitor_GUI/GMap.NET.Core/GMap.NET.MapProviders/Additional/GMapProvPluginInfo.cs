namespace GMapNetInterFace
{

    using System;
    using System.IO;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Text;


    /// <summary>
    /// プラグインに関する情報
    /// </summary>
    public class GMapProvPluginInfo : IF_Host
    {
        private string _strDllFileName;
        private string _strDllClassName;
        private Guid _guidDll;


        private int _iMapZoom;
        private long _lMapX;
        private long _lMapY;
        private double _dMapCenterLng;
        private double _dMapCenterLat;
        private double _dMapLeftLng;
        private double _dMapRightLng;
        private double _dMapUpperLat;
        private double _dMapLowerLat;


        /// <summary>
        /// 
        /// </summary>
        public Guid guidDll
        {
            get
            {
                return _guidDll;
            }
        }



        /// <summary>
        /// 
        /// </summary>
        public int iMapZoom
        {
            get
            {
                return _iMapZoom;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public long lMapX
        {
            get
            {
                return _lMapX;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public long lMapY
        {
            get
            {
                return _lMapY;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public double dMapCenterLng
        {
            get
            {
                return _dMapCenterLng;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public double dMapCenterLat
        {
            get
            {
                return _dMapCenterLat;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        public double dMapLeftLng
        {
            get
            {
                return _dMapLeftLng;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public double dMapRightLng
        {
            get
            {
                return _dMapRightLng;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public double dMapUpperLat
        {
            get
            {
                return _dMapUpperLat;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public double dMapLowerLat
        {
            get
            {
                return _dMapLowerLat;
            }
        }



        /// <summary>
        /// 
        /// </summary>
        public int iProcessFlag_URL
        {
            get
            {
                return 1;
            }
        }


        /// <summary>
        /// 
        /// </summary>
        public int iProcessFlag_Image
        {
            get
            {
                return 2;
            }
        }



        public GMapProvPluginInfo()
        {
            #region Comment
            Debug.WriteLine(
                 string.Concat(
                     "GMapProvPluginInfo : GMapProvPluginInfo()"
                )
            );
            #endregion
        }



        /// <summary>
        /// PluginInfoクラスのコンストラクタ
        /// Pluginを実行させる場合は、このコントラクタを使用する。
        /// </summary>
        public GMapProvPluginInfo(string strDllFileName)
        {
            #region Comment
            Debug.WriteLine(
                 string.Concat(
                     "GMapProvPluginInfo : GMapProvPluginInfo(",
                     strDllFileName,
                     ")"
                )
            );
            #endregion


            //アセンブリとして読み込む
            System.Reflection.Assembly asm = System.Reflection.Assembly.LoadFrom(strDllFileName);

            foreach (Type tt in asm.GetTypes())
            {
                this._strDllFileName = strDllFileName;
                this._strDllClassName = tt.FullName;
                this._guidDll = getGUIDfromFile(this._strDllFileName);

                #region Comment
                Debug.WriteLine(
                    string.Concat(
                        "GMapProvPluginInfo - GMapProvPluginInfo : ",
                        _strDllFileName,
                        " / ",
                        _strDllClassName,
                        " / ",
                        _guidDll.ToString(),
                        " / ",
                        Convert.ToString(asm.GetTypes().Length),
                        " / ",
                        Convert.ToString(tt.IsClass),
                        " / ",
                        Convert.ToString(tt.IsPublic),
                        " / ",
                        Convert.ToString(!tt.IsAbstract),
                        " / ",
                        Convert.ToString(tt.GetInterface(typeof(GMapNetInterFace.IF_Host).FullName)),
                        " / ",
                        Convert.ToString(tt.GetInterface(typeof(GMapNetInterFace.IF_Plugin).FullName))
                    )
                );
                #endregion
            }
        }


        /// <summary>
        /// 有効なプラグインを探す
        /// </summary>
        /// <returns>有効なプラグインのPluginInfo配列</returns>
        public String[] FindPluginFileNames()
        {
            #region Comment
            Debug.WriteLine(
                string.Concat(
                    "GMapProvPluginInfo - FindPluginFileNames : "
                )
            );
            #endregion


            System.Collections.ArrayList plugins =
                new System.Collections.ArrayList();
            //IPlugin型の名前
            string ipluginName = typeof(GMapNetInterFace.IF_Plugin).FullName;

            //プラグインフォルダ
            string folder = System.IO.Path.GetDirectoryName(
                System.Reflection.Assembly
                .GetExecutingAssembly().Location);
            folder += "\\..\\mapPlugin";
            if (!System.IO.Directory.Exists(folder))
                throw new ApplicationException(
                    "プラグインフォルダ\"" + folder +
                    "\"が見つかりませんでした。");


            //.dllファイルを探す
            string[] dlls =
                System.IO.Directory.GetFiles(folder, "*.dll");


            foreach (string dll in dlls)
            {
                try
                {
                    //アセンブリとして読み込む
                    System.Reflection.Assembly asm =
                        System.Reflection.Assembly.LoadFrom(dll);
                    foreach (Type t in asm.GetTypes())
                    {
                        //-----------------------------------------------------------------------------
                        // クラス以外の型、抽象クラス、非公開クラス、外部利用不可なクラスは除く
                        //-----------------------------------------------------------------------------
                        //アセンブリ内のすべての型について、
                        //プラグインとして有効か調べる 
                        if (t.IsClass
                            && t.IsPublic
                            && !t.IsAbstract
                            && t.IsVisible
                            && t.GetInterface(ipluginName) != null)
                        {
                            //PluginInfoをコレクションに追加する
                            plugins.Add(dll);
                        }
                    }
                }
                catch
                {
                }
            }

            //コレクションを配列にして返す
            return (String[])plugins.ToArray(typeof(String));
        }




        /// <summary>
        /// 
        /// </summary>
        public GMapNetInterFace.IF_Plugin getDllInstance()
        {
            try
            {
                #region Comment
                Debug.WriteLine(
                    string.Concat(
                        "PluginInfo - getDllInstance : ",
                        _strDllFileName,
                        " / ",
                        _strDllClassName
                    )
                );
                #endregion


                //アセンブリを読み込む
                System.Reflection.Assembly asm =
                    System.Reflection.Assembly.LoadFrom(_strDllFileName);

                //クラス名からインスタンスを作成する
                IF_Plugin plugin =
                     (IF_Plugin)asm.CreateInstance(_strDllClassName);

                //IPluginHostの設定
                plugin.Host = this;

                return plugin;
            }
            catch (Exception e)
            {
                #region Comment
                Debug.WriteLine(
                    string.Concat(
                        "GMapNetInterFace.IF_Plugin getDllInstance() exception : ",
                        e.Message
                    )
                );
                #endregion

                return null;
            }
        }


        public void setParam(
            int iMapZoom,
            long lMapX,
            long lMapY,
            double dMapCenterLng,
            double dMapCenterLat,
            double dMapLeftLng,
            double dMapRightLng,
            double dMapUpperLat,
            double dMapLowerLat
            )
        {
            _iMapZoom = iMapZoom;
            _lMapX = lMapX;
            _lMapY = lMapY;

            _dMapCenterLng = dMapCenterLng;
            _dMapCenterLat = dMapCenterLat;
            _dMapLeftLng = dMapLeftLng;
            _dMapRightLng = dMapRightLng;
            _dMapUpperLat = dMapUpperLat;
            _dMapLowerLat = dMapLowerLat;
        }


        /// <summary>
        /// プラウグインファイルから疑似GUIDを生成
        /// </summary>
        /// <param name="strFilePathName"></param>
        /// <returns></returns>
        public Guid getGUIDfromFile(string strFilePathName)
        {
            #region Comment
            Debug.WriteLine(
                string.Concat(
                    "GMapProvPluginInfo - getGUIDfromFile : "
                )
            );
            #endregion

            // ファイルパスの設定
            FileInfo fiFileInfo = new FileInfo(strFilePathName);

            // ファイル名設定
            string strFileName = fiFileInfo.Name;
            // ファイル更新時刻設定
            string strDateTime = fiFileInfo.LastWriteTimeUtc.ToString();


            //文字列をbyte型配列に変換する
            byte[] data = System.Text.Encoding.UTF8.GetBytes(fiFileInfo.Name + strDateTime);
            //MD5CryptoServiceProviderオブジェクトを作成
            System.Security.Cryptography.MD5CryptoServiceProvider md5 =
                new System.Security.Cryptography.MD5CryptoServiceProvider();
            //ハッシュ値を計算する
            byte[] bs = md5.ComputeHash(data);
            //リソースを解放する
            md5.Clear();

            // ファイルGUID設定
            Guid _guidPluginFile = new Guid(bs);

            return _guidPluginFile;
        }
    }
}
