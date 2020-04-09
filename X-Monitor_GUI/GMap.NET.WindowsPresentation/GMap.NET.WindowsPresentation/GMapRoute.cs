
namespace GMap.NET.WindowsPresentation
{
	using System;
    using System.Collections.Generic;
    using System.Windows.Shapes;


    public interface IShapable
    {
        void RegenerateShape(GMapControl map);
    }
	 [Serializable]
    public class GMapRoute : GMapMarker, IShapable
    {
        public readonly List<PointLatLng> Points = new List<PointLatLng>();
        

        public GMapRoute(IEnumerable<PointLatLng> points)
        {
            Points.AddRange(points);
            if (Points.Count > 0)
            {
                Position = Points[0];
                RegenerateShape(null);
            }
        }

        /*public readonly List<PointLatLng_two> Points_two = new List<PointLatLng_two>();
        public GMapRoute(IEnumerable<PointLatLng_two> points_two)
        {
            Points_two.AddRange(points_two);
            if (Points_two.Count > 0)   //////////////////李自然
            {
                Position = Points[0];
                RegenerateShape(null);
            }
        }*/


        /// <summary>
        /// regenerates shape of route
        /// </summary>
        public virtual void RegenerateShape(GMapControl map)
        {
            if (map != null)
            {
                Map = map;

                if (Points.Count > 1)
                {
                    Position = Points[0];

                    var localPath = new List<System.Windows.Point>();
                    var offset = Map.FromLatLngToLocal(Points[0]);
                    foreach (var i in Points)
                    {
                        var p = Map.FromLatLngToLocal(new PointLatLng(i.Lat, i.Lng));
                        localPath.Add(new System.Windows.Point(p.X - offset.X, p.Y - offset.Y));
                    }

					var shape = map.CreateRoutePath(localPath, true, LineBrush);

                    if (this.Shape != null && this.Shape is Path)
                    {
                        (this.Shape as Path).Data = shape.Data;
                    }
                    else
                    {
                        this.Shape = shape;
                    }
                }
                else
                {
                    this.Shape = null;
                }

                //追加機能
                if (map.IsLoaded)
                {
                    //map.ForceUpdateOverlays();
                    map.InvalidateVisual(true);
                }
            }
        }

		public System.Windows.Media.Brush LineBrush = System.Windows.Media.Brushes.Navy;
    }
}
