using System;
using System.Collections.Generic;
using System.Xml.Linq;              // XDocument オブジェクト
using System.Xml.XPath;
using System.Xml;

using Point;

// GMap.NETライブラリ
using GMap.NET;                         //GMap.NET Core

namespace XmlBase
{
	public class XmlConvert
	{
		public static Message GetMessage(string _xml, string _tag)
		{
			//string _message = "";
			var _message = new XmlBase.Message();

			XDocument xml = XDocument.Parse(_xml);	// textデータをxmlデータに読み込む
			var xmlRoot = xml.Root.Element("message");	// xmlからデータを取得

			if (xmlRoot != null)
			{
				_message.MessageVersion = xmlRoot.Attribute("ver").Value;

				_message.InformationMessage = "";
				foreach (var _e in xmlRoot.XPathSelectElements("information"))
				{
					_message.InformationMessage += "[" + _e.Attribute("i").Value + "]";
					string _s = _e.Value.Replace(" ", "");
					_s = _s.Replace("　", "");
					_s = _s.Replace("	", "");
					_message.InformationMessage += _s + "\n";
				}
				foreach (var _e in xmlRoot.XPathSelectElements("update"))
				{
					if (_e.Attribute("name").Value == _tag)
					{
						_message.UpadataVersion = _e.Attribute("ver").Value;
						string _s = _e.Value.Replace(" ", "");
						_s = _s.Replace("　", "");
						_s = _s.Replace("	", "");
						_message.UpadataMessage = _s;

						break;
					}
				}
			}

			return _message;
		}


		public static PointData GetGpsModuleData(string _mission)
		{
			PointData _point = new PointData();

			///////////////////////////////////////////////////
			//
			XDocument xml = XDocument.Parse(_mission);	// textデータをxmlデータに読み込む
			var xmlRoot = xml.Root.Element("GPS");	// xmlからデータを取得

			// GPS
			_point.Lat = double.Parse(XmlDataGet(xmlRoot, "Lat"));
			_point.Lon = double.Parse(XmlDataGet(xmlRoot, "Lon"));
			_point.Alt = double.Parse(XmlDataGet(xmlRoot, "Alt"));

			return _point;
		}

		// 数字/文字チェック
		private static string ValueCheck(string value)
		{
			string result = string.Empty;

			double d;
			if (double.TryParse(value, out d))
			{
				int i;
				if (int.TryParse(value, out i))
				{
					return value;
				}
				else
				{
					return value;
				}

			}
			else
			{
				return "0";
			}
		}
		// XMLデータ内の数値取得
		private static string XmlDataGet(XElement xmlRoot, string tag, string att = "", string attValue = "")
		{
			try
			{
				//タグがない場合
				if (xmlRoot.XPathSelectElement(tag) == null)
				{
					return "0";
				}

				// 空の場合
				if (String.IsNullOrEmpty(xmlRoot.XPathSelectElement(tag).Value))
				{
					return "0";
				}

				string val = xmlRoot.XPathSelectElement(tag).Value;
				if (att != "")
				{
					foreach (var element in xmlRoot.XPathSelectElements(tag))
					{
						string _attVal = element.Attribute(att).Value;
						if (_attVal == attValue)
						{
							//System.Diagnostics.Debug.WriteLine("XmlDataGet: " + _attVal + "/" + element.Value);
							return ValueCheck(element.Value);
						}
					}
					val = "0";
				}

				return ValueCheck(val);
			}
			catch (Exception ex)
			{
				System.Windows.MessageBox.Show("(802) XmlDataGet Error : " + ex.Message);
				return "0";
			}
		}

	}

	public class Message
	{
		public string MessageVersion { get; set; }
		public string InformationMessage { get; set; }
		public string UpadataVersion { get; set; }
		public string UpadataMessage { get; set; }
	}
}
