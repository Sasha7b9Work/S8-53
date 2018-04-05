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

        private String response = String.Empty;

        public bool IsConnected()
        {
            return (socket == null) ? false : socket.Connected;
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

                if (!socket.Connected)
                {
                    socket.Shutdown(SocketShutdown.Both);
                    socket.Close();
                    socket = null;
                }
                else
                {
                    SendString("REQUEST ?");
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
                Receive();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private void Receive()
        {
            try
            {
                StateObject state = new StateObject();
                state.workSocket = socket;
                socket.BeginReceive(state.buffer, 0, StateObject.BUFFER_SIZE, 0, new AsyncCallback(ReceiveCallback), state);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public void SendString(string data)
        {
            if (socket.Connected)
            {
                byte[] bytes = Encoding.UTF8.GetBytes(data);
                socket.Send(bytes);
                Console.WriteLine("Длина посылки " + bytes.Length + ' ' + bytes[0] + ' ' + bytes[1]);
            }
        }

        private void ReceiveCallback(IAsyncResult ar)
        {
            try
            {
                StateObject state = (StateObject)ar.AsyncState;
                int bytesRead = socket.EndReceive(ar);

                if(bytesRead > 0)
                {
                    state.sb.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesRead));
                    socket.BeginReceive(state.buffer, 0, StateObject.BUFFER_SIZE, 0, new AsyncCallback(ReceiveCallback), state);
                }
                else
                {
                    if(state.sb.Length > 1)
                    {
                        response = state.sb.ToString();
                    }
                }
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public void Disconnect()
        {
            socket.Close();
        }

        public bool DeviceExistOnAddress(string ip, int port)
        {
            return false;
        }
    }
}
