namespace GMap.NET.MapProviders
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using GMap.NET.Projections;
    public abstract class AMapProviderBase : GMapProvider
    {
       private GMapProvider[] overlays;

        public AMapProviderBase()
        {
            this.MaxZoom = null;
            base.RefererUrl = "http://www.amap.com/";
            base.Copyright = string.Format("©{0} 高德软件 Image© DigitalGlobe & chinasiwei | AIRBUS & EastDawn", DateTime.Today.Year);
        }

        public override GMapProvider[] Overlays
        {
            get
            {
                if (this.overlays == null)
                {
                    this.overlays = new GMapProvider[] { this };
                }
                return this.overlays;
            }
        }

        public override PureProjection Projection
        {
            get
            {
                return MercatorProjectionGCJ.Instance;
            }
        }
    }

    public class AMapProvider : AMapProviderBase
    {
        private readonly Guid id = new Guid("EF3DD303-3F74-4938-BF40-232D0595EE88");
        public static readonly AMapProvider Instance = new AMapProvider();
        //private readonly string name = Resources.Strings.AMap;
        private readonly string name = "AMap";
        private static readonly string UrlFormat = "http://webrd03.is.autonavi.com/appmaptile?x={0}&y={1}&z={2}&lang=zh_cn&size=1&scale=1&style=7";

        public override PureImage GetTileImage(GPoint pos, int zoom)
        {
            string url = this.MakeTileImageUrl(pos, zoom, GMapProvider.LanguageStr);
            return base.GetTileImageUsingHttp(url);
        }

        private string MakeTileImageUrl(GPoint pos, int zoom, string language)
        {
            string str = string.Format(UrlFormat, pos.X, pos.Y, zoom);
            Console.WriteLine("url:" + str);
            return str;
        }

        public override Guid Id
        {
            get
            {
                return this.id;
            }
        }

        public override string Name
        {
            get
            {
                return this.name;
            }
        }
    }

}