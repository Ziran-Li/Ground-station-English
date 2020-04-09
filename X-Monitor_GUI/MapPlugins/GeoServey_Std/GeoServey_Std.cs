using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using GMapNetInterFace;


namespace GeoServey_Std
{
    public class GeoServey_Std : GMapNetInterFace.IF_Plugin
    {
        private IF_Host _host;
        private int _iProcessFlag;
        private string _strURL;
        //        private string _strFileName;

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
        /// 取得strMapName
        /// </summary>
        public string strMapName
        {
            get
            {
				return "国土地理院地図(標準)";
            }
        }



        /// <summary>
        /// 取得strMapName
        /// </summary>
        public string strLicence
        {
            get
            {
                return string.Format("©{0} 国土地理院", DateTime.Today.Year);
            }
        }


        /// <summary>
        /// 取得ベースURL
        /// </summary>
        string UrlFormat = "http://cyberjapandata.gsi.go.jp/xyz/std/{0}/{1}/{2}.png";

        /// <summary>
        /// 取得URL
        /// </summary>
        public string strURL
        {
            get
            {
                return _strURL;
            }
        }

        /// <summary>
        /// 取得URL
        /// </summary>
        public string strFileName
        {
            get
            {
                return "";
            }
			set
			{
				string _str = value;
			}
        }



        public void Run()
        {
            int z = _host.iMapZoom;

			_iProcessFlag = _host.iProcessFlag_URL;
			_strURL = string.Format(UrlFormat,
									z,
									_host.lMapX,
									_host.lMapY);
            //　ズームにより表示するマップの種類を設定
			//if (z == 18)
			//{
			//	_iProcessFlag = _host.iProcessFlag_URL;
			//	_strURL = string.Format(UrlFormat18,
			//							z,
			//							_host.lMapX,
			//							_host.lMapY);
			//}
			//else if (z >= 15 && z <= 17)
			//{
			//	_iProcessFlag = _host.iProcessFlag_URL;
			//	_strURL = string.Format(UrlFormatORT,
			//							z,
			//							_host.lMapX,
			//							_host.lMapY);
			//}
			//else
			//{
			//	_iProcessFlag = _host.iProcessFlag_URL;
			//	_strURL = string.Format(UrlFormat,
			//							z,
			//							_host.lMapX,
			//							_host.lMapY);
			//}
        }

    }
}
