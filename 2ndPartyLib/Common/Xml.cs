using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Common
{
	class Xml
	{
		//// XMLからすべてのパスを抽出
		//// 親ノードと子ノードの抽出
		//private void Make(System.Xml.XmlElement xmlParent, ref string trvParent)
		//{
		//	if (xmlParent.HasChildNodes)
		//	{
		//		for (int i = 0; i < xmlParent.ChildNodes.Count; i++)
		//		{
		//			if (xmlParent.ChildNodes[i].GetType().ToString().IndexOf("XmlElement") >= 0)
		//			{
		//				System.Xml.XmlElement xmlChild = (System.Xml.XmlElement)xmlParent.ChildNodes[i];
		//				string trvChild;

		//				trvChild = xmlChild.Name;


		//				bool _flagHi = true;

		//				// 除外タグのチェック
		//				foreach (var tag in _tagHi)
		//				{
		//					if (tag == trvChild)
		//					{
		//						_flagHi = false;
		//						break;
		//					}
		//				}

		//				// 親ノード追加
		//				if (_flagHi && (trvParent == null || trvParent == ""))
		//				{
		//					// 子ノードのチェック
		//					if (xmlChild.HasChildNodes)
		//					{
		//						Make(xmlChild, ref trvChild);
		//					}
		//					else
		//					{
		//						//iPlotList.Items.Add(trvChild);
		//						string _atti = xmlChild.GetAttribute("i");
		//						string _atta = xmlChild.GetAttribute("a");
		//						string _attt = xmlChild.GetAttribute("t");
		//						GetXmlPathAIT(trvChild, _atta, _atti, _attt);
		//					}
		//				}

		//				// 親ノードがある場合
		//				if (trvParent != null && trvParent != "")
		//				{
		//					bool _flagLo = true;
		//					// 除外タグのチェック
		//					foreach (var tag in _tagLo)
		//					{
		//						if (tag == trvChild)
		//						{
		//							_flagLo = false;
		//							break;
		//						}
		//					}

		//					// 親子ノード追加
		//					if (_flagLo)
		//					{
		//						if (xmlChild.HasChildNodes)
		//						{
		//							trvChild = trvParent + "/" + trvChild;
		//							Make(xmlChild, ref trvChild);
		//						}
		//						else
		//						{
		//							string _atti = xmlChild.GetAttribute("i");
		//							string _atta = xmlChild.GetAttribute("a");
		//							string _attt = xmlChild.GetAttribute("t");

		//							GetXmlPathAIT(trvParent + "/" + trvChild, _atta, _atti, _attt);

		//						}
		//					}
		//				}
		//			}
		//		}
		//	}
		//	//親ノードの設定
		//	trvParent = xmlParent.Name;
		//}
		////　タグリスト抽出
		//private DataList GetXmlPathAIT(string _path, string _a, string _i, string _t)
		//{
		//	DataList _xmlTag = new DataList
		//	{
		//		XMLTag = _path,
		//		Contents = "NoData",
		//		Value = "NoData"
		//	};
		//	foreach (var _e in _element.XPathSelectElements(_path))
		//	{
		//		if (_e != null)
		//		{
		//			var _aa = _e.Attribute("a");
		//			var _ai = _e.Attribute("i");
		//			var _at = _e.Attribute("t");

		//			string _ata = ""; if (_aa != null) _ata = _aa.Value;
		//			string _ati = ""; if (_ai != null) _ati = _ai.Value;
		//			string _att = ""; if (_at != null) _att = _at.Value;

		//			if ((_ata == _a) && (_ati == _i) && (_att == _t))
		//			{
		//				if (_a != "") _xmlTag.XMLTag += "[@" + _aa + "]";
		//				if (_t != "") _xmlTag.XMLTag += "[@" + _at + "]";
		//				if (_i != "") _xmlTag.XMLTag += "[@" + _ai + "]";
		//				_xmlTag.Contents = _e.Value;
		//			}
		//		}
		//	}
		//	//TagList.Items.Add(_xmlTag);

		//	return _xmlTag;
		//}

		// XMLパス読込判定

		// 1行ずつXML読込->解析
	}
	public class DataList
	{
		public string XMLTag { get; set; }
		public string Contents { get; set; }
		public string Value { get; set; }
	}
	public class UAVList
	{
		public string Name { get; set; }
		public string Lat { get; set; }
		public string Lon { get; set; }
		public string Alt { get; set; }
		public string Yaw { get; set; }
	}
}
