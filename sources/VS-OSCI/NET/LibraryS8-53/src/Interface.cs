using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LibraryS8_53
{
    public abstract class Interface
    {
        //public abstract event EventHandler<EventArgs> ReceiveEvent;

        public abstract void Stop();

        public abstract void SendByte(byte data);

        public abstract void SendString(string str);
    }
}
