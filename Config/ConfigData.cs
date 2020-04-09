using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Config
{
	public static class Constant
	{
		#region SetMessage
		// Core部DBクリア
		public const string SetMessageClear =
@"
<?xml version=""1.0"" encoding=""UTF-8""?>
<root>
  <message type=""set"" a=""Clear"">
  </message>
</root>" + "\0";

		public const string SetMessageCoreHidden =
@"
<?xml version=""1.0"" encoding=""UTF-8""?>
<root>
  <message type=""set"" a=""Setting"">
    <Setting>
      <Core t=""S"">Hidden</Core>
    </Setting>
  </message>
</root>" + "\0";
		public const string SetMessageCoreVisible =
@"
<?xml version=""1.0"" encoding=""UTF-8""?>
<root>
  <message type=""set"" a=""Setting"">
    <Setting>
      <Core t=""S"">Visible</Core>
    </Setting>
  </message>
</root>" + "\0";
		#endregion
	}
    public class ConfigData
    {
		 public static readonly ConfigData Empty = new ConfigData();

        private List<AddressData> _addressList;

        public ConfigData()
        {
			_addressList = new List<AddressData>();
        }

        #region プロパティ
		public List<AddressData> AddressList
        {
			get { return this._addressList; }
			set { this._addressList = value; }
        }
        #endregion

    }

	public class AddressData
	{
		public string Name { get; set; }
		public double Lat { get; set; }
		public double Lng { get; set; }
	}
}
