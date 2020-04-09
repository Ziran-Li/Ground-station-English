namespace makeMapData
{
    partial class Form1
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.mpgMap1 = new MpgMap.MpgMap();
            this.SuspendLayout();
            // 
            // mpgMap1
            // 
            this.mpgMap1.CenterMarker = true;
            this.mpgMap1.Location = new System.Drawing.Point(22, 12);
            this.mpgMap1.MapAngle = 0F;
            this.mpgMap1.MapCenter = new System.Drawing.Point(503173398, 128439361);
            this.mpgMap1.MapMode = MpgMap.MapMode.Move;
            this.mpgMap1.MapPath = "latest";
            this.mpgMap1.MapScale = 25000;
            this.mpgMap1.Name = "mpgMap1";
            this.mpgMap1.Size = new System.Drawing.Size(300, 300);
            this.mpgMap1.StyleName = "標準";
            this.mpgMap1.StylePath = "latest";
            this.mpgMap1.TabIndex = 0;
            this.mpgMap1.UserMarker = false;
            this.mpgMap1.UserMarkerPosition = new System.Drawing.Point(0, 0);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(340, 318);
            this.Controls.Add(this.mpgMap1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private MpgMap.MpgMap mpgMap1;
    }
}

