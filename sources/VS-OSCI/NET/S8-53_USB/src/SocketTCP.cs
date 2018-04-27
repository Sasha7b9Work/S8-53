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
        public const int BUFFER_SIZE = 1;
        public byte[] buffer = new byte[BUFFER_SIZE];
    }

    public class EventArgsReceiveSocketTCP : EventArgs
    {
        public byte[] data;
        public EventArgsReceiveSocketTCP(byte[] data)
        {
            this.data = data;
        }
    }

    public class SocketTCP
    {
        private Socket socket = null;

        private String response = String.Empty;

        static public event EventHandler<EventArgs> ReceiveEvent;

        private static ManualResetEvent connectDone = new ManualResetEvent(false);

        public SocketTCP()
        {
        }

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

                socket.SendTimeout = 1;

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
            if (socket.Connected)
            {
                byte[] byteData = Encoding.ASCII.GetBytes(":" + data + "\x0d");
                socket.Send(byteData);
            }
        }

        public string ReadString()
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
                            byte[] data = new byte[bytesRead];
                            for(int i = 0; i < bytesRead; i++)
                            {
                                data[i] = state.buffer[i];
                            }
                            handler(null, new EventArgsReceiveSocketTCP(data));
                        }


                        Receive();
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
                    //socket.Close();
                    socket = null;
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public void Read(byte[] buffer, int numBytes)
        {
            int recvBytes = socket.Receive(buffer, numBytes, SocketFlags.None);

            if(recvBytes != numBytes)
            {
                Console.WriteLine("Error");
            }
        }

        public byte[] ReadBytes(long timeWaitMS)
        {
            byte[] bytes = new byte[0];
            try
            {
                long timeLast = CurrentTime();
                int numPrevBytes = 0;
                while (CurrentTime() - timeLast < timeWaitMS)
                {
                    if (socket.Available != numPrevBytes)
                    {
                        timeLast = CurrentTime();
                        numPrevBytes = socket.Available;
                    }
                }

                int numBytes = socket.Available;
                if (numBytes != 0)
                {
                    bytes = new byte[numBytes];
                    socket.Receive(bytes, numBytes, SocketFlags.None);
                }
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
            return bytes;
        }

        private static long CurrentTime()
        {
            return DateTime.Now.Ticks / 10000;
        }

        public void Clear()
        {
            if (socket.Connected)
            {
                while(socket.Available != 0)
                {
                    byte[] bytes = new byte[1];
                    socket.Receive(bytes, 1, SocketFlags.None);
                }
            }
        }
    }
}
