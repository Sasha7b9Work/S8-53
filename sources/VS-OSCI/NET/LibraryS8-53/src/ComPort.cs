using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.IO.Ports;

namespace LibraryS8_53
{
    public class EventArgsReceiveComPort : EventArgs
    {
        public string data;

        public EventArgsReceiveComPort(string data)
        {
            this.data = data;
        }
    }

    public class ComPort : Interface
    {
        private static SerialPort port;
        private static string[] ports;
        private static Mutex mutex = new Mutex();
        // Если true, то будет включён обработчик события SerialPort
        private bool dataReceivedHandlerEnable = false;
        public event EventHandler<EventArgs> ReceiveEvent;

        public ComPort()
        {
            port = new SerialPort();
            port.ReadTimeout = 100;
            port.BaudRate = 125000;

            port.DataReceived += new SerialDataReceivedEventHandler(DataReceiveHandler);
        }

        public override void Stop()
        {
            port.Close();
        }

        public string[] GetPorts()
        {
            ports = SerialPort.GetPortNames();
            return ports;
        }

        public bool DeviceConnectToPort(int numPort)
        {
            port.PortName = ports[numPort];
            string answer;
            try
            {
                port.Open();
                if (port.IsOpen)
                {
                    SendString("REQUEST ?");
                    answer = ReadLine();
                    port.Close();
                    bool retValue = (answer == "S8-53" || answer == "S8-53/1");
                    return retValue;
                }
            }
            catch (SystemException)
            {
                port.Close();
            }
            return false;
        }

        public override void SendByte(byte data)
        {
            if (port.IsOpen)
            {
                byte[] sendingData = new byte[1];
                sendingData[0] = data;
                port.Write(sendingData, 0, 1);
            }
        }

        public override void SendString(string str)
        {
            mutex.WaitOne();

            if (port.IsOpen)
            {
                while (port.BytesToWrite != 0) { };
                port.Write(":" + str + "\x0d");
            }

            mutex.ReleaseMutex();
        }

        public void SendBytes(byte[] buffer)
        {
            port.Write(buffer, 0, buffer.Length);
        }

        static public string ReadLine()
        {
            string line;
            try
            {
                line = port.ReadLine();
                return line.Substring(0, line.Length - 2);
            }
            catch (SystemException)
            {

            }
            return null;
        }

        public void OnlyOpen(int numPort)
        {
            port.PortName = ports[numPort];
            port.Open();
        }

        public void OnlyClose()
        {
            port.Close();
        }

        public bool Connect(int numPort, bool handlerEnable)
        {
            dataReceivedHandlerEnable = handlerEnable;

            try
            {
                port.PortName = ports[numPort];
                port.Open();
                if (port.IsOpen)
                {
                    SendString("REQUEST ?");
                    string answer = ReadLine();
                    if (answer == "S8-53")
                    {
                    }
                    else if (answer == "S8-53/1")
                    {
                    }
                }
            }
            catch (SystemException)
            {
                port.Close();
            }
            return port.IsOpen;
        }

        private static byte[] data = new byte[16 * 1024];

        public SerialPort GetSerialPort()
        {
            return port;
        }

        public bool IsOpen()
        {
            return port.IsOpen;
        }

        private void DataReceiveHandler(object sender, SerialDataReceivedEventArgs args)
        {
            if(IsOpen() && dataReceivedHandlerEnable)
            {
                SerialPort sp = (SerialPort)sender;
                if(sp != null)
                {
                    string indata = sp.ReadExisting();

                    EventHandler<EventArgs> handler = ReceiveEvent;

                    if(handler != null)
                    {
                        handler(null, new EventArgsReceiveComPort(indata));
                    }
                }
            }
        }
    }
}
