using GMapNetInterFace;

using System;
using System.Diagnostics;

namespace BingStaticMap
{
    public class BingStaticMap: GMapNetInterFace.IF_Plugin
    {
        private IF_Host _host;
        private int _iProcessFlag;
        private string _strURL;

        string UrlFormat = "http://dev.virtualearth.net/REST/V1/Imagery/Map/AerialWithLabels/{2},{1}/{3}?mapSize=512,512&key={0}";

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
                return "BingStaticMap";
            }
        }



        /// <summary>
        /// 取得strMapName
        /// </summary>
        public string strLicence
        {
            get
            {
                return "(C)Bing";
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
				string str = value;
			}
        }


        public void Run()
        {

            _iProcessFlag = _host.iProcessFlag_URL;

            int z = _host.iMapZoom + 2;

            if (z < 19)
            {
                _strURL = string.Format(UrlFormat,
                    "AkgGrE4piQ3k894Hx9Ji_18K9bDMajhPoNWKkZuE-BR8PLC2bvQofg0Blc2eCPTc",
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
