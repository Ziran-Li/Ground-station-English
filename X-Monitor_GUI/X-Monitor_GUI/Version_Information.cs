using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XMonitor_GUI
{
    class Version_Information
    {
/*=========================================IMU_Version==============================*/
        public byte[] IMU_HardWare_Version = new byte[20];//IMU的硬件版本空间
        public byte[] IMU_SoftWare_Version = new byte[20];//IMU的软件版本空间
        public byte[] IMU_Equipment_ID = new byte[20];//IMU的ID号空间
/*=========================================AP_Version================================*/
        public byte[] AP_HardWare_Version = new byte[20];//AP的硬件版本空间
        public byte[] AP_SoftWare_Version = new byte[20];//AP的软件版本空间
        public byte[] AP_Equipment_ID = new byte[20];//AP的ID号空间
/*=========================================LED_Version================================*/
        public byte[] LED_HardWare_Version = new byte[20];//LED的硬件版本空间
        public byte[] LED_SoftWare_Version = new byte[20];//LED的软件版本空间
        public byte[] LED_Equipment_ID = new byte[20];//LED的ID号空间
/*=========================================MAG_Version================================*/
        public byte[] MAG_HardWare_Version = new byte[20];//MAG的硬件版本空间
        public byte[] MAG_SoftWare_Version = new byte[20];//MAG的软件版本空间
        public byte[] MAG_Equipment_ID = new byte[20];//MAG的ID号空间
/*=========================================GPS_Version================================*/
        public byte[] GPS_HardWare_Version = new byte[20];//GPS的硬件版本空间
        public byte[] GPS_SoftWare_Version = new byte[20];//GPS的软件版本空间
        public byte[] GPS_Equipment_ID = new byte[20];//GPS的ID号空间
/*=========================================HUB_Version================================*/
        public byte[] HUB_HardWare_Version = new byte[20];//GPS的硬件版本空间
        public byte[] HUB_SoftWare_Version = new byte[20];//GPS的软件版本空间
        public byte[] HUB_Equipment_ID = new byte[20];//GPS的ID号空间
/*=========================================FDR_Version================================*/
        public byte[] FDR_HardWare_Version = new byte[20];//FDR的硬件版本空间
        public byte[] FDR_SoftWare_Version = new byte[20];//FDR的软件版本空间
        public byte[] FDR_Equipment_ID = new byte[20];//FDR的ID号空间
        /*=========================================DTU_Version================================*/
        public byte[] DTU_HardWare_Version = new byte[20];//RTK的硬件版本空间
        public byte[] DTU_SoftWare_Version = new byte[20];//RTK的软件版本空间
        public byte[] DTU_Equipment_ID = new byte[20];//RTK的ID号空间
/*=========================================RTK_Version================================*/
        public byte[] RTK_HardWare_Version = new byte[20];//RTK的硬件版本空间
        public byte[] RTK_SoftWare_Version = new byte[20];//RTK的软件版本空间
        public byte[] RTK_Equipment_ID = new byte[20];//RTK的ID号空间
    }
}
