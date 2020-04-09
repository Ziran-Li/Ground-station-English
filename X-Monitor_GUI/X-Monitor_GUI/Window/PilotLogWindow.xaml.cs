using System.Windows;
using System.Xml.Linq;
using System.Xml.XPath;

// log4net
using log4net;

namespace XMonitor_GUI
{
	public partial class PilotLogWindow
	{
		// ログ
		private static readonly ILog _logger = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

		public PilotLogWindow()
		{
			InitializeComponent();
			LoadCheckList();
			_logger.Info("---OperationLog---");
		}
		private void LoadCheckList()
		{
			var xml = XDocument.Load(@"Xml/Settings.xml");				//    
			foreach (var element in xml.Root.XPathSelectElements("Settings/PilotLog/Must"))
			{
				var _v = new CheckList()
				{
					No = element.Attribute("i").Value,
					Name = element.Value
				};
				MustCheck.Items.Add(_v);
			}
			foreach (var element in xml.Root.XPathSelectElements("Settings/PilotLog/Disc"))
			{
				var _v = new CheckList()
				{
					No = element.Attribute("i").Value,
					Name = element.Value
				};
				DiscCheck.Items.Add(_v);
			}
		}
		private void OK_Click(object sender, RoutedEventArgs e)
		{
			this.DialogResult = true;
		}

		private void Close_Click(object sender, RoutedEventArgs e)
		{
			this.DialogResult = false;
			Close();
		}
		
		// TODO:隠しキー入力（入力スキップ機能）
	}

	public class CheckList
	{
		public string No { get; set; }
		public string Name { get; set; }
	}
}
