using System.Collections.Generic;

namespace Plot
{
    public class PlotData
    {
        public static readonly PlotData Empty = new PlotData();

        private int _id;                    //オプション画面ID
        private List<string> _PlotName;     //タグ名
        private List<double> _PlotData;     //値

        public PlotData()
        {
            _id = 0;
            _PlotName = new List<string>();
            _PlotData = new List<double>();
        }

        #region プロパティ
        public int ID
        {
            get { return this._id; }
            set { this._id = value; }
        }
        public List<string> PlotName
        {
            get { return this._PlotName; }
            set { this._PlotName = value; }
        }
        public List<double> PlotValue
        {
            get { return this._PlotData; }
            set { this._PlotData = value; }
        }
        #endregion

    }
}
