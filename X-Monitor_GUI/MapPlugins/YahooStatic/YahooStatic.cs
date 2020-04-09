using GMapNetInterFace;

using System;
using System.Diagnostics;

namespace YahooStatic
{
    public class YahooStatic : GMapNetInterFace.IF_Plugin
    {
        private IF_Host _host;
        private int _iProcessFlag;
        private string _strURL;

        string UrlFormat = "http://map.olp.yahooapis.jp/OpenLocalPlatform/V1/static?appid={0}&mode=hybrid&lat={2}&lon={1}&z={3}&width=512&height=512&pointer=off&scalebar=off";

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
                return "YahooStaticMap";
            }
        }



        /// <summary>
        /// 取得strMapName
        /// </summary>
        public string strLicence
        {
            get
            {
                return string.Format("©{0} Yahoo! Inc.", DateTime.Today.Year);
            }
        }



        public string strURL
        {
            get
            {
                return _strURL;
            }
        }



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

            _iProcessFlag = _host.iProcessFlag_URL;

            int z = _host.iMapZoom + 2;

            if (z < 21)
            {
                _strURL = string.Format(UrlFormat,
                    "dj0zaiZpPXBJNW5KeUo2MzYyTSZzPWNvbnN1bWVyc2VjcmV0Jng9ZmI-",
                    _host.dMapCenterLng,
                    _host.dMapCenterLat,
                    z);
            }
            else
            {
                _strURL = "";
            }

        }
    }
}
