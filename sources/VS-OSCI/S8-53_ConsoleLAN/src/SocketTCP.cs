using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace S8_53_ConsoleLAN
{
    public class StateObject
    {
        public Socket workSocket = null;
        public const int BUFFER_SIZE = 256;
        public byte[] buffer = new byte[BUFFER_SIZE];
        public StringBuilder sb = new StringBuilder();
    }

    class SocketTCP
    {
        private Socket socket = null;

        public bool IsConnected()
        {
            return (socket == null) ? false : socket.Connected;
        }

        public void SendString(string data)
        {

        }

        public bool Connect(String ip, int port)
        {
            String[] bytes = ip.Split('.');

            byte[] addressBytes = new byte[4];

            for(int i = 0; i < 4; i++)
            {
                addressBytes[i] = (byte)Int32.Parse(bytes[i]);
            }

            try
            {
                IPAddress address = new IPAddress(addressBytes);

                IPEndPoint remoteEP = new IPEndPoint(address, port);

                socket = new Socket(remoteEP.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

                socket.BeginConnect(remoteEP, new AsyncCallback(ConnectCallback), socket);

                Stopwatch time = new Stopwatch();

                time.Start();

                while (time.Elapsed.TotalMilliseconds < 1000.0 && !socket.Connected) { };

                if (socket.Connected)
                {
                
                }
                else
                {
                    socket.Shutdown(SocketShutdown.Both);
                    socket.Close();
                    socket = null;
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            return socket != null;
        }

        private void ConnectCallback(IAsyncResult ar)
        {
            try
            {
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public bool Disconnect()
        {
            return false;
            //socket.Shutdown();
        }

        public bool DeviceExistOnAddress(string ip, int port)
        {
            return false;
        }

        public string AddressRemote()
        {
            return socket.RemoteEndPoint.Serialize().ToString();
        }
    }
}
