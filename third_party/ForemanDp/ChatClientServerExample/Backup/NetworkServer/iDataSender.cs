using System;
using System.Collections.Generic;
using System.Text;

namespace NetworkServer
{
    public interface iDataSender
    {
        byte[] DataToSend { set; }
    }
}
