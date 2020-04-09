// 
//　 HealthData管理クラス
//
//

namespace Health
{
    public class HealthData
    {
        public static readonly HealthData Empty = new HealthData();

        // Alert
        private int AlertLinkVal;   //
        private double VolMinVal;   //
        private double VolMaxVal;   //
        private double AlertVolVal; //
        private int AlertRCrecVal;  //
		private int AlertGPSVal;	//

		//FirmwareVersion
		public string FirmwareVersionH { get; set; }
		public string FirmwareVersionL { get; set; }
		public string FirmwareBuildDateH { get; set; }
		public string FirmwareBuildDateL { get; set; }
		public string FirmwareEditionH { get; set; }
		public string FirmwareEditionL { get; set; }

        #region Prosessor
        // Prosessor
        private int Progress_Processor_LowLvMcuProcTime;        // Lower Level MCU Proc. Time
        private int Progress_Processor_HighLvMcuProcTime;       // Higher Level MCU Proc. Time
        #endregion
        #region LowerLevelMCU
        // LowerLevelMCU
        private int Progress_LLMCU_RcReceiverCommunication;     // RC Receiver Communication
        private int Progress_LLMCU_RcReceiverReception;         // RC Receiver Reception(UAV-プロポ間の通信状態)
        private int Progress_LLMCU_GyroSensorCommunication;     // Gyro Sensor Communication
        private int Progress_LLMCU_GyroSensorMeasurement;       // Gyro Sensor Measurement
        #endregion
        #region LowerLevelMCU - HigherLevelMCU Communication
        // LowerLevelMCU - HigherLevelMCU Communication
        private int Progress_McuComm_LowToHigh;// Low ---> High
        private int Progress_McuComm_HighToLow;// Low <--- High
        #endregion
        #region HigherLevelMCU
        // HigherLevelMCU
        private int Progress_HLMCU_IMUCommunication;            // IMU Communication
        private int Progress_HLMCU_IMUGyroMeasurement;          // IMU Gyro Measurement
        private int Progress_HLMCU_IMUAccMeasurement;           // IMU Acc Measurement
        private int Progress_HLMCU_IMUMagMeasurement;           // IMU Mag Measurement
        private int Progress_HLMCU_PresSensorCommunication;     // Pres. Sensor Communication
        private int Progress_HLMCU_PresSensorMeasurement;       // Pres. Sensor Measurement
        private int Progress_HLMCU_GpsReceiverCommunication;    // GPS Receiver Communication
        private int Progress_HLMCU_GpsMeasurement;				// GPS Measurement
        #endregion
        #region UAV - Ground Control Station Cummunication
        // UAV - Ground Control Station Cummunication
        private int Progress_MainLink_UavToGcs;//UAV ---> GCS(UAV-モニタ間の通信状態)
        private int Progress_MainLink_GcsToUav;//UAV <--- GCS
        #endregion

        // 初期化
        public HealthData()
        {
            // ================================================================================
            // HealthData管理系初期設定
            this.AlertLinkVal = 66;
            this.AlertRCrecVal = 66;
            this.VolMinVal = 3.4;
            this.VolMaxVal = 4.2;
            this.AlertVolVal = 3.7;
			this.AlertGPSVal = 7;

			this.FirmwareVersionH = "NoData";
			this.FirmwareVersionL = "NoData";

            // Prosessor
            this.Progress_Processor_LowLvMcuProcTime = 0;
            this.Progress_Processor_HighLvMcuProcTime = 0;

            // LowerLevelMCU
            this.Progress_LLMCU_RcReceiverCommunication = 0;
            this.Progress_LLMCU_RcReceiverReception = 0;
            this.Progress_LLMCU_GyroSensorCommunication = 0;
            this.Progress_LLMCU_GyroSensorMeasurement = 0;

            // LowerLevelMCU - HigherLevelMCU Communication
            this.Progress_McuComm_LowToHigh = 0;
            this.Progress_McuComm_HighToLow = 0;

            // HigherLevelMCU
            this.Progress_HLMCU_IMUCommunication = 0;
            this.Progress_HLMCU_IMUGyroMeasurement = 0;
            this.Progress_HLMCU_IMUAccMeasurement = 0;
            this.Progress_HLMCU_IMUMagMeasurement = 0;
            this.Progress_HLMCU_PresSensorCommunication = 0;
            this.Progress_HLMCU_PresSensorMeasurement = 0;
            this.Progress_HLMCU_GpsReceiverCommunication = 0;
            this.Progress_HLMCU_GpsMeasurement = 0;

            // UAV - Ground Control Station Cummunication
            this.Progress_MainLink_UavToGcs = 0;
            this.Progress_MainLink_GcsToUav = 0;
        }

        #region Alert
		public int AlertGPSValue
		{
			get { return this.AlertGPSVal; }
			set { this.AlertGPSVal = value; }
		}
        public int AlertLinkValue
        {
            get { return this.AlertLinkVal; }
            set { this.AlertLinkVal = value; }
        }
        public double VolMinValue
        {
            get { return this.VolMinVal; }
            set { this.VolMinVal = value; }
        }
        public double VolMaxValue
        {
            get { return this.VolMaxVal; }
            set { this.VolMaxVal = value; }
        }
        public double AlertVolValue
        {
            get { return this.AlertVolVal; }
            set { this.AlertVolVal = value; }
        }
        public int AlertRCrecValue
        {
            get { return this.AlertRCrecVal; }
            set { this.AlertRCrecVal = value; }
        }
        #endregion

        #region Prosessor
        public int LowLvMcuProcTime
        {
            get { return this.Progress_Processor_LowLvMcuProcTime; }
            set { this.Progress_Processor_LowLvMcuProcTime = value; }
        }
        public int HighLvMcuProcTime
        {
            get { return this.Progress_Processor_HighLvMcuProcTime; }
            set { this.Progress_Processor_HighLvMcuProcTime = value; }
        }
        #endregion

        #region LowerLevelMCU
        public int LLMCU_RcReceiverCommunication
        {
            get { return this.Progress_LLMCU_RcReceiverCommunication; }
            set { this.Progress_LLMCU_RcReceiverCommunication = value; }
        }
        public int LLMCU_RcReceiverReception
        {
            get { return this.Progress_LLMCU_RcReceiverReception; }
            set { this.Progress_LLMCU_RcReceiverReception = value; }
        }
        public int LLMCU_GyroSensorCommunication
        {
            get { return this.Progress_LLMCU_GyroSensorCommunication; }
            set { this.Progress_LLMCU_GyroSensorCommunication = value; }
        }
        public int LLMCU_GyroSensorMeasurement
        {
            get { return this.Progress_LLMCU_GyroSensorMeasurement; }
            set { this.Progress_LLMCU_GyroSensorMeasurement = value; }
        }
        #endregion

        #region LowerLevelMCU - HigherLevelMCU Communication
        public int McuComm_LowToHigh
        {
            get { return this.Progress_McuComm_LowToHigh; }
            set { this.Progress_McuComm_LowToHigh = value; }
        }
        public int McuComm_HighToLow
        {
            get { return this.Progress_McuComm_HighToLow; }
            set { this.Progress_McuComm_HighToLow = value; }
        }
        #endregion

        #region HigherLevelMCU
        public int HLMCU_IMUCommunication
        {
            get { return this.Progress_HLMCU_IMUCommunication; }
            set { this.Progress_HLMCU_IMUCommunication = value; }
        }
        public int HLMCU_IMUGyroMeasurement
        {
            get { return this.Progress_HLMCU_IMUGyroMeasurement; }
            set { this.Progress_HLMCU_IMUGyroMeasurement = value; }
        }
        public int HLMCU_IMUAccMeasurement
        {
            get { return this.Progress_HLMCU_IMUAccMeasurement; }
            set { this.Progress_HLMCU_IMUAccMeasurement = value; }
        }
        public int HLMCU_IMUMagMeasurement
        {
            get { return this.Progress_HLMCU_IMUMagMeasurement; }
            set { this.Progress_HLMCU_IMUMagMeasurement = value; }
        }
        public int HLMCU_PresSensorCommunication
        {
            get { return this.Progress_HLMCU_PresSensorCommunication; }
            set { this.Progress_HLMCU_PresSensorCommunication = value; }
        }
        public int HLMCU_PresSensorMeasurement
        {
            get { return this.Progress_HLMCU_PresSensorMeasurement; }
            set { this.Progress_HLMCU_PresSensorMeasurement = value; }
        }
        public int HLMCU_GpsReceiverCommunication
        {
            get { return this.Progress_HLMCU_GpsReceiverCommunication; }
            set { this.Progress_HLMCU_GpsReceiverCommunication = value; }
        }
        public int HLMCU_GpsMeasurement
        {
            get { return this.Progress_HLMCU_GpsMeasurement; }
            set { this.Progress_HLMCU_GpsMeasurement = value; }
        }

        #endregion

        #region UAV - Ground Control Station Cummunication
        public int MainLink_UavToGcs
        {
            get { return this.Progress_MainLink_UavToGcs; }
            set { this.Progress_MainLink_UavToGcs = value; }
        }
        public int MainLink_GcsToUav
        {
            get { return this.Progress_MainLink_GcsToUav; }
            set { this.Progress_MainLink_GcsToUav = value; }
        }
        #endregion
    }
}