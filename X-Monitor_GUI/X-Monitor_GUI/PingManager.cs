using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XMonitor_GUI
{
    class PingManager
    {

        private int m_nSend = 0;
        private int m_nReply = 0;
        private int m_nReply_Last = 0;
        private int m_nReply_Last2 = 0;
       public void Send()
        {
            m_nSend++;

            if (m_nSend >= 8)
            {
                m_nReply_Last2 = m_nReply_Last;
                m_nReply_Last = m_nReply;
                m_nReply = 0;
                m_nSend = 0;
            }
        }
       public void Reply()
        {
            m_nReply++;
        }

       public double GetReplyRate()
        {
            return (double)(m_nReply_Last + m_nReply_Last2) / (2 * 8);
        }

       public bool IsConnected()
        {
            return (m_nReply + m_nReply_Last) != 0;
        }
    }

}