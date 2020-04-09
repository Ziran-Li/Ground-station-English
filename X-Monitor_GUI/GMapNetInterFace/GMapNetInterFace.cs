using System;

namespace GMapNetInterFace
{
    public interface IF_Plugin
    {
        /// <summary>
        /// メンバ
        /// </summary>
        // Map名称
        String strMapName { get; }

        // ライセンス名称
        String strLicence { get; }

        /// 処理フラグ
        int iProcessFlag { get; }

        // MapへのアクセスURL
        String strURL { get; }

        // Mapのファイル名
		String strFileName { get; set; }

        /// プラグインのホスト
        IF_Host Host { get; set; }

        /// <summary>
        /// プラグインを実行する
        /// </summary>
        void Run();
    }


    public interface IF_Host
    {

        /// <summary>
        /// メンバ
        /// </summary>

        // Mapのズーム値
        int iMapZoom { get; }

        // MapのXYZ形式でのX値
        long lMapX { get; }
        // MapのXYZ形式でのY値
        long lMapY { get; }

        // MapのXYZ形式でのタイルの中心経度
        double dMapCenterLng { get; }
        // MapのXYZ形式でのタイルの中心緯度
        double dMapCenterLat { get; }


        // MapのXYZ形式でのタイルの左経度
        double dMapLeftLng { get; }
        // MapのXYZ形式でのタイルの右経度
        double dMapRightLng { get; }
        // MapのXYZ形式でのタイルの上緯度
        double dMapUpperLat { get; }
        // MapのXYZ形式でのタイルの下緯度
        double dMapLowerLat { get; }

        /// 処理フラグ定数
        int iProcessFlag_URL { get; }
        int iProcessFlag_Image { get; }
    }
}
