using System;
using System.Windows;

namespace TouchControls
{
	static class TouchConstants
	{
		public const double angFix = 2.0;
	}
    public class TouchControl
    {
        public static readonly TouchControl Empty = new TouchControl();


        private int TouchId1;       // 1タッチ目ID
        private int TouchId2;       // 2タッチ目ID
        private int TouchCnt;       // タッチ数
        private Point TouchP1;      // 1タッチ目Ponit
        private Point TouchP2;      // 2タッチ目Ponit
        private Point InitTouchP;   // 初期タッチ目Ponit
        private Point TransTouchP;

        private double TransVect;   //
        private double OldTransVect;

        private double Ang;         // マルチタッチ-回転
        private double Dis;         // マルチタッチ-距離（Zoomに使用）
        private double Trans;       // マルチタッチ-距離（Zoomに使用）

        private double OldAng;     // マルチタッチ-回転
        private double OldDis;     // マルチタッチ-距離（Zoomに使用）
        private double OldTrans;   // マルチタッチ-距離（Zoomに使用）

        private double NewAng;     // マルチタッチ-回転
        private double NewDis;     // マルチタッチ-距離（Zoomに使用）
        private double NewTrans;   // マルチタッチ-距離（Zoomに使用）

        private bool MultiTouchF;
        int TCount= 0;

        public TouchControl(int TouchId1, int TouchId2, int TouchCnt, Point TouchP1, Point TouchP2, Point InitTouchP, double TransVect, double Ang, double Dis, double Trans, bool MultiTouchF)
        {
          this.TouchId1 = TouchId1;
          this.TouchId2 = TouchId2;
          this.TouchCnt = TouchCnt;
          this.TouchP1  = TouchP1;
          this.TouchP2  = TouchP2;
          this.InitTouchP = InitTouchP;
          this.TransVect = TransVect;
          this.Ang      = Ang;
          this.Dis      = Dis;
          this.Trans = Trans;
          this.MultiTouchF = MultiTouchF;

        }
        public TouchControl()
        {
            this.TouchId1 = 0;
            this.TouchId2 = 0;
            this.TouchCnt = 0;
            this.TouchP1 = new Point();
            this.TouchP2 = new Point();
            this.InitTouchP = new Point();
			this.TransTouchP = new Point();

			this.TransVect = 0;

            this.Ang = 0;
            this.Dis = 0;
            this.Trans = 0;

            this.OldAng = 0;
            this.OldDis = 0;
            this.OldTrans = 0;

            this.NewAng = 0;
            this.NewDis = 0;
            this.NewTrans = 0;


            this.MultiTouchF = false;

        }

        #region プロパティ
        /// <summary>
        /// 
        /// </summary>
        public int TouchID1
        {
            get{ return this.TouchId1;  }
            set{ this.TouchId1 = value; }
        }
        public int TouchID2
        {
            get { return this.TouchId2; }
            set { this.TouchId2 = value; }
        }
        public int TouchCount
        {
            get { return this.TouchCnt; }
            set { this.TouchCnt = value; }
        }
        public Point TouchPonit1
        {
            get { return this.TouchP1; }
            set { this.TouchP1 = value; }
        }
        public Point TouchPonit2
        {
            get { return this.TouchP2; }
            set { this.TouchP2 = value; }
        }
        public Point InitTouchPonit
        {
            get { return this.InitTouchP; }
            set { this.InitTouchP = value; }
        }
        public Point TransTouchPonit
        {
            get { return this.TransTouchP; }
            set { this.TransTouchP = value; }
        }
        public double Angle
        {
            get { return this.Ang; }
            set { this.Ang = value; }
        }
        public double Distance
        {
            get { return this.Dis; }
            set { this.Dis = value; }
        }
        public double Translation
        {
            get { return this.Trans; }
            set { this.Trans = value; }
        }
        public double NewAngle
        {
            get { return this.NewAng; }
            set { this.NewAng = value; }
        }
        public double NewDistance
        {
            get { return this.NewDis; }
            set { this.NewDis = value; }
        }
        public double NewTranslation
        {
            get { return this.NewTrans; }
            set { this.NewTrans = value; }
        }
        public double OldAngle
        {
            get { return this.OldAng; }
            set { this.OldAng = value; }
        }
        public double OldDistance
        {
            get { return this.OldDis; }
            set { this.OldDis = value; }
        }
        public double OldTranslation
        {
            get { return this.OldTrans; }
            set { this.OldTrans = value; }
        }
        public bool MultiTouchFlag
        {
            get { return this.MultiTouchF; }
            set { this.MultiTouchF = value; }
        }
        public double TranslationVector
        {
            get { return this.TransVect; }
            set { this.TransVect = value; }
        }
        public double OldTranslationVector
        {
            get { return this.OldTransVect; }
            set { this.OldTransVect = value; }
        }
        #endregion


        #region マルチタッチ
        // マルチタッチ-初期化(クリア)
        public void MultiTouchdInit()
        {
            this.TouchId1 = 0;
            this.TouchId2 = 0;
            this.TouchCnt = 0;
            this.TouchP1 = new Point();
            this.TouchP2 = new Point();
            this.InitTouchP = new Point();

            this.TransVect = 0;
            this.OldTransVect = 0;

            this.Ang = 0;
            this.Dis = 0;
            this.Trans = 0;

            this.OldAng = 0;
            this.OldDis = 0;
            this.OldTrans = 0;

            this.NewAng = 0;
            this.NewDis = 0;
            this.NewTrans = 0;

            this.TCount = 0;
        }

        // マルチタッチ-検出
        public bool MultiTouchdDtection()
        {
            if (this.TouchCount >= 1)
            {
                return (true);
            }
            else
            {
                return (false);
            }

        }

        // マルチタッチ-回転操作の検出
        private double MultiTouchdRotation()
        {
            

            // Center
            Point c = new Point();
            c.X = (TouchPonit2.X - TouchPonit1.X) / 2 + TouchPonit1.X;
            c.Y = (TouchPonit2.Y - TouchPonit1.Y) / 2 + TouchPonit1.Y;

            // Point
            Point p = new Point();
            p.X = TouchPonit2.X - c.X;
            p.Y = TouchPonit2.Y - c.Y;
            double angle = (Math.Atan2(p.Y, p.X) * 180 / Math.PI) + 90;

            return angle;

        }
        // マルチタッチ-ズーム操作の検出
        private double MultiTouchdDistance()
        {
            Point p = new Point();

            p.X = TouchPonit2.X - TouchPonit1.X;
            p.Y = TouchPonit2.Y - TouchPonit1.Y;

            double dis = Math.Sqrt(((p.X * p.X) + (p.Y * p.Y)));

            return dis;

        }
        // マルチタッチ-移動操作の検出 
        private double MultiTouchdTranslation()
        {
            Point p = new Point();

            p.X = TouchP1.X - InitTouchP.X;
            p.Y = TouchP1.Y - InitTouchP.Y;

            double trans = Math.Sqrt(((p.X * p.X) + (p.Y * p.Y)));

            return trans;
        }

        // マルチタッチ操作
        public void MultiTouch()
        {
            // ロール/ズーム/スライドの判定値
            double dif = 100000.0;   
            int dis = 100;

            this.MultiTouchF = MultiTouchdDtection();

            // マルチタッチデータの更新
            this.NewDis = MultiTouchdDistance();
            this.NewAng = MultiTouchdRotation();
            this.NewTrans = MultiTouchdTranslation();

            if (this.TCount == 0)
            {
                // マルチタッチデータの保存
                this.OldDis = this.NewDis;
                this.OldAng = this.NewAng;
                this.OldTrans = this.NewTrans;
                this.TCount = 1;
            }

            // マルチタッチ処理
            if (this.MultiTouchF)
            {
                this.Dis = (this.NewDis - this.OldDis);
                this.Ang = (this.NewAng - this.OldAng);
                this.Trans = (this.NewTrans - this.OldTrans) ;

                TransTouchP.X = this.InitTouchP.X - this.TouchP1.X;
                TransTouchP.Y = this.InitTouchP.Y - this.TouchP1.Y;
                this.TransVect = (Math.Atan2(TransTouchP.Y, TransTouchP.X));

                double d = Math.Sqrt(((TransTouchP.X * TransTouchP.X) + (TransTouchP.Y * TransTouchP.Y)));

				//System.Diagnostics.Debug.WriteLine("d:" + d + "  NewDis:" + this.NewDis + "  Ang:" + Ang);
				
                if (d < dif && this.NewDis > dis)// 回転（ロール）
                {
					//Console.Write("ロール d = {0}, Dis = {1}, Ang = {2} \n", d, NewDis, Ang); 
                    this.Dis = 0;
                    this.Trans = 0;
					this.Ang *= TouchConstants.angFix;            //操作性の調整

                    // デッドバンド
                    if ((this.Ang < 0.1) && (this.Ang > -0.1) || (this.Ang > 12) || (this.Ang < -12))
                    {
                        this.Ang = 0;
                    }
                }
                else if (this.NewDis > dis)     //地図拡大縮小（ズーム）
                {
					//Console.Write("ズーム d = {0}, Dis = {1}, Ang = {2} \n", d, NewDis, Ang); 
                    this.Ang = 0;
                    this.Trans = 0;

                    // デッドバンド
                    /*
                    if ((this.Dis < 3.5) && (this.Dis > -3.5))
                    {
                        this.Dis = 0;
                    }
                    */ 
                }
                else if (this.NewDis <= dis)    // 地図移動（スライド）
                {
					//Console.Write("スライド d = {0}, Dis = {1}, Ang = {2} \n", d, NewDis, Ang); 
                    this.Dis = 0;
                    this.Ang = 0;
                    //this.Trans *= 0.75;        //操作性の調整
                }
                else
                {
                    this.Dis = 0;
                    this.Ang = 0;
                    this.Trans = 0;
                }
                
                // タッチ基準点の更新
                if ((this.Trans < this.OldTrans))
                {
                    this.InitTouchP = this.TouchP1;
                    this.OldTransVect = this.TransVect;
                    this.NewTrans *= -1;
                    this.Trans = 0;
                }else if( !((this.TransVect < (this.OldTransVect + 10)) && (this.TransVect > (this.OldTransVect - 10))) ){
                    this.TCount = 0;
                }
            }
            

            // マルチタッチデータの保存
            this.OldDis = this.NewDis;
            this.OldAng = this.NewAng;
            this.OldTrans = this.NewTrans;
        }
		
		public void MultiTouchT()
		{
			// ロール/ズーム/スライドの判定値
			//double dif = 2.5;
			//int dis = 100;

			this.MultiTouchF = MultiTouchdDtection();

			// マルチタッチデータの更新
			this.NewDis = MultiTouchdDistance();
			this.NewAng = MultiTouchdRotation();
			this.NewTrans = MultiTouchdTranslation();

			if (this.TCount == 0)
			{
				// マルチタッチデータの保存
				this.OldDis = this.NewDis;
				this.OldAng = this.NewAng;
				this.OldTrans = this.NewTrans;
				this.TCount = 1;
			}

			// マルチタッチ処理
			if (this.MultiTouchF)
			{
				this.Dis = (this.NewDis - this.OldDis);
				this.Ang = (this.NewAng - this.OldAng);
				this.Trans = (this.NewTrans - this.OldTrans);

				TransTouchP.X = this.InitTouchP.X - this.TouchP1.X;
				TransTouchP.Y = this.InitTouchP.Y - this.TouchP1.Y;
				this.TransVect = (Math.Atan2(TransTouchP.Y, TransTouchP.X));

				double d = Math.Sqrt(((TransTouchP.X * TransTouchP.X) + (TransTouchP.Y * TransTouchP.Y)));

				//System.Diagnostics.Debug.WriteLine("d:" + d + "  NewDis:" + this.NewDis + "  Ang:" + Ang);
				//Console.Write("ロール d = {0}, Dis = {1}, Ang = {2} \n", d.ToString("#.######"), NewDis.ToString("#.######"), Ang.ToString("#.######")); 
				//Console.Write("ang = {0}\n", Ang);
				//Console.Write("ang = {0}\n", d);

				if(Math.Abs(Ang) > 0.1)
				{
					this.Dis = 0;
					this.Trans = 0;
					//this.Ang *= TouchConstants.angFix;            //操作性の調整
				}else if(d > 0.1)
				{
					this.Ang = 0;
				}
				else
				{
					this.Dis = 0;
					this.Ang = 0;
					this.Trans = 0;
				}

				// タッチ基準点の更新
				if ((this.Trans < this.OldTrans))
				{
					this.InitTouchP = this.TouchP1;
					this.OldTransVect = this.TransVect;
					this.NewTrans *= -1;
					this.Trans = 0;
				}
				else if (!((this.TransVect < (this.OldTransVect + 10)) && (this.TransVect > (this.OldTransVect - 10))))
				{
					this.TCount = 0;
				}
			}

			// マルチタッチデータの保存
			this.OldDis = this.NewDis;
			this.OldAng = this.NewAng;
			this.OldTrans = this.NewTrans;
		}
        
        #endregion
    }
}