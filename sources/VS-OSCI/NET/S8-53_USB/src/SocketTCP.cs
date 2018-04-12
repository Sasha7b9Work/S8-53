using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace LibraryS8_53
{
    public class StateObject
    {
        public const int BUFFER_SIZE = 256;
        public byte[] buffer = new byte[BUFFER_SIZE];
        public StringBuilder sb = new StringBuilder();
    }

    public class EventArgsReceiveSocketTCP : EventArgs
    {
        public String data;
        public EventArgsReceiveSocketTCP(String data)
        {
            this.data = data;
        }
    }

    public class SocketTCP
    {
        private Mutex mutex = new Mutex();

        private Socket socket = null;

        private String response = String.Empty;

        public event EventHandler<EventArgs> ReceiveEvent;

        private static ManualResetEvent connectDone = new ManualResetEvent(false);

        ~SocketTCP()
        {
            Disconnect();
        }

        public bool IsConnected()
        {
            return (socket == null) ? false : socket.Connected;
        }

        public bool Connect(String ip, int port)
        {
            String[] bytes = ip.Split('.');

            byte[] addressBytes = new byte[4];

            for (int i = 0; i < 4; i++)
            {
                addressBytes[i] = (byte)Int32.Parse(bytes[i]);
            }

            try
            {
                IPAddress address = new IPAddress(addressBytes);

                IPEndPoint remoteEP = new IPEndPoint(address, port);

                socket = new Socket(remoteEP.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

                socket.ReceiveBufferSize = 100 * 1024;

                connectDone.Reset();

                socket.BeginConnect(remoteEP, new AsyncCallback(ConnectCallback), socket);

                connectDone.WaitOne(1000);

                if(!socket.Connected)
                {
                    return false;
                }

                SendString("REQUEST ?");
                string answer = ReadString();
                if(answer != "S8-53/1")
                {
                    socket.Disconnect(false);
                    socket.Close();
                    return false;
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            return socket != null && socket.Connected;
        }

        private void ConnectCallback(IAsyncResult ar)
        {
            //socket.EndConnect(ar);
            connectDone.Set();
        }

        private void Receive()
        {
            try
            {
                StateObject state = new StateObject();
                socket.BeginReceive(state.buffer, 0, StateObject.BUFFER_SIZE, 0, new AsyncCallback(ReceiveCallback), state);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public void SendString(string data)
        {
            mutex.WaitOne();

            if (socket.Connected)
            {
                byte[] byteData = Encoding.ASCII.GetBytes(":" + data + "\x0d");
                socket.Send(byteData);
            }

            mutex.ReleaseMutex();
        }

        private string ReadString()
        {
            byte[] byteData = new byte[500];

            string line = "";

            try
            {
                int size = socket.Receive(byteData);

                byte[] buffer = new byte[size];

                for (int i = 0; i < size; i++)
                {
                    buffer[i] = byteData[i];
                }

                line = Encoding.ASCII.GetString(buffer);
                return line.Substring(0, line.Length - 2);
            }
            catch (Exception)
            {

            }

            return line;
        }

        private void ReceiveCallback(IAsyncResult ar)
        {
            try
            {
                StateObject state = (StateObject)ar.AsyncState;
                if (socket != null && socket.Connected)
                {
                    int bytesRead = socket.EndReceive(ar);

                    if (bytesRead > 0)
                    {
                        EventHandler<EventArgs> handler = ReceiveEvent;

                        if (handler != null)
                        {
                            state.sb.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesRead));
                            String data = state.sb.ToString();
                            handler(null, new EventArgsReceiveSocketTCP(data.Substring(0, data.Length - 2)));
                        }


                        Receive();
                    }
                    else
                    {
                        if (state.sb.Length > 1)
                        {
                            response = state.sb.ToString();
                            Console.WriteLine(response);
                        }
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public void Disconnect()
        {
            try
            {
                if (socket != null)
                {
                    socket.Disconnect(false);
                    socket.Close();
                    socket = null;
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public bool DeviceExistOnAddress(string ip, int port)
        {
            return false;
        }

        public int BytesToRead()
        {
            return socket.Available;
        }

        public void Read(byte[] buffer, int numBytes)
        {
            socket.Receive(buffer, numBytes, SocketFlags.None);
        }

        public Socket GetSocket()
        {
            return socket;
        }
    }
}
