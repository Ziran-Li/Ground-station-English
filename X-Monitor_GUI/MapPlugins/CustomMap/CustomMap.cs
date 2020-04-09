using System;
using GMapNetInterFace;

namespace CustomMap
{
	public class CustomMap : GMapNetInterFace.IF_Plugin
	{
		private IF_Host _host;
		private int _iProcessFlag;

		private string _strFileName;
		private string _filePath;

		/// <summary>
		/// ホスト側の設定
		/// </summary>
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
		/// 取得URL
		/// </summary>
		public string strURL
		{
			get
			{
				return "a";
			}
		}

		/// <summary>
		/// 取得strMapName
		/// </summary>
		public string strMapName
		{
			get
			{
				return "CustomMap";
			}
		}

		/// <summary>
		/// 取得strMapName
		/// </summary>
		public string strLicence
		{
			get
			{
				return "© 自律制御システム研究所";
			}
		}



		/// <summary>
		/// 取得URL
		/// </summary>
		public string strFileName
		{
			get
			{
				return _strFileName;
			}
			set
			{
				_filePath = value;
			}
		}



		public void Run()
		{
			try
			{
				string _path = String.Format(@"{0}\{1}\{2}\{3}.png", _filePath, _host.iMapZoom, _host.lMapX, _host.lMapY);

				if (System.IO.File.Exists(System.IO.Path.GetFullPath(_path)) == true)
				{
					_strFileName = _path;
				}
				else
				{
					_strFileName = "";
				}
				_iProcessFlag = _host.iProcessFlag_Image;
			}
			catch
			{
				_iProcessFlag = _host.iProcessFlag_Image;
				_strFileName = "";
			}
		}
	}
}
