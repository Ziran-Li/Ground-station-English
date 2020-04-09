using System;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

using GMap.NET;
using GMap.NET.WindowsPresentation;

namespace XMonitor_GUI.CustomMarkers
{
	/// <summary>
	/// CustomMap.xaml の相互作用ロジック
	/// </summary>
	public partial class CustomMap : UserControl
	{
		GMapMarker Marker;

		public CustomMap(GMapMarker marker, string path)
		{
			InitializeComponent();

			// CustomMapのアイコンを変更
			icon.Source = new BitmapImage(new Uri(path));		

			//BitmapImage m_bitmap = null;    // 読み込んだ画像用

			//// 既に読み込まれていたら解放する
			//if (m_bitmap != null)
			//{
			//	m_bitmap = null;
			//}
			//// BitmapImageにファイルから画像を読み込む
			//m_bitmap = new BitmapImage();
			//m_bitmap.BeginInit();
			//m_bitmap.UriSource = new Uri(path);
			//m_bitmap.EndInit();
			//// Imageコントロールに表示
			//icon.Source = m_bitmap;

			this.Marker = marker;
			Marker.ZIndex = 0;
		}
	}
}
