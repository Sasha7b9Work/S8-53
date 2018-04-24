using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

using ControlLibraryS8_53;
using LibraryS8_53;

namespace S8_53_USB {

    public partial class MainForm : Form {
        // Этот порт используется для соединения по USB
        private static LibraryS8_53.ComPort port = new LibraryS8_53.ComPort();
        // Этот сокет используется для соединения по LAN
        private static LibraryS8_53.SocketTCP socket = new LibraryS8_53.SocketTCP();
        // Этот процесс будет заниматься непосредственно рисованием
        private static Thread runProcess;
        // true, если рисующий поток работает
        private static bool isRunning = false;
        private Dictionary<Button, string> mapButtons = new Dictionary<Button, string>();
        private static Queue<string> commands = new Queue<string>();
        // Сюда будем считывать данные из порта
        private static Queue<byte> data = new Queue<byte>();
        // Mutex на буфер data
        private static Mutex mutexData = new Mutex();
        // Признак того, что нужно отключиться от портов при первой возможности
        private static bool needForDisconnect = false;

        private enum Command : byte
        {
            SET_COLOR = 1,
            FILL_REGION = 2,
            END_SCENE = 3,
            DRAW_HLINE = 4,
            DRAW_VLINE = 5,
            SET_POINT = 6,
            DRAW_SIGNAL_LINES = 7,
            DRAW_TEXT = 8,
            SET_PALETTE = 9,
            SET_FONT = 10,
            DRAW_VLINES_ARRAY = 13,
            DRAW_SIGNAL_POINTS = 14,
            DRAW_MULTI_HPOINT_LINES_2 = 17,
            DRAW_MULTI_VPOINT_LINES = 18,
            LOAD_FONT = 19
        };

        public MainForm() {
            InitializeComponent();

            mapButtons.Add(btnChannel0,    "CHAN1");
            mapButtons.Add(btnService,     "SERVICE");
            mapButtons.Add(btnChannel1,    "CHAN2");
            mapButtons.Add(btnDisplay,     "DISPLAY");
            mapButtons.Add(btnTime,        "TIME");
            mapButtons.Add(btnMemory,      "MEMORY");
            mapButtons.Add(btnTrig,        "TRIG");
            mapButtons.Add(btnStart,       "START");
            mapButtons.Add(btnCursors,     "CURSORS");
            mapButtons.Add(btnMeasures,    "MEASURES");
            mapButtons.Add(btnHelp,        "HELP");
            mapButtons.Add(btnMenu,        "MENU");
            mapButtons.Add(btnF1,          "1");
            mapButtons.Add(btnF2,          "2");
            mapButtons.Add(btnF3,          "3");
            mapButtons.Add(btnF4,          "4");
            mapButtons.Add(btnF5,          "5");

            // Устанавливаем количество байт в приёмном буфере, при котором будет вызываться обработчик приёма
            LibraryS8_53.ComPort.port.ReceivedBytesThreshold = 1;

            buttonUpdatePorts_Click(null, null);

            buttonConnectLAN.Enabled = true;
        }

        private void button_MouseDown(object sender, MouseEventArgs args) {
            if ((Button)sender != buttonConnectUSB)
            {
                try
                {
                    commands.Enqueue("KEY:" + StringToSendForButton(sender) + " DOWN");
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                }
            }
        }

        private void button_MouseUp(object sender, MouseEventArgs args) {
            if ((Button)sender != buttonConnectUSB)
            {
                try
                {
                    commands.Enqueue("KEY:" + StringToSendForButton(sender) + " UP");
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                }
            }
        }

        private void governor_RotateLeft(object sender, EventArgs args) {
            try
            {
                commands.Enqueue("GOV:" + ((Governor)sender).ValueToSend + " LEFT");
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private void governor_RotateRight(object sender, EventArgs args) {
            try
            {
                commands.Enqueue("GOV:" + ((Governor)sender).ValueToSend + " RIGHT");
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private string StringToSendForButton(object btn) {
            string retValue = "";
            
            try
            {
                retValue = mapButtons[(Button)btn];
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            return retValue;
        }

        private void buttonUpdatePorts_Click(object sender, EventArgs args) {
            try
            {
                string[] ports = port.GetPorts();
                comboBoxPorts.Items.Clear();
                comboBoxPorts.Items.AddRange(ports);
                comboBoxPorts.SelectedIndex = ports.Length - 1;
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private void buttonConnectUSB_Click(object sender, EventArgs e)
        {
            if (port.IsOpen())                                  // Если порт открыт - идёт обмен с прибором. Будем отключать
            {
                needForDisconnect = true;
                buttonConnectUSB.Text = "Подкл";
                comboBoxPorts.Enabled = true;
                buttonUpdatePorts.Enabled = true;

                textBoxIP.Enabled = true;
                textBoxPort.Enabled = true;
                buttonConnectLAN.Enabled = true;
            }
            else
            {
                if (port.Connect(comboBoxPorts.SelectedIndex, false)) // иначе делаем попыткую подключиться
                {
                    needForDisconnect = false;
                    LibraryS8_53.ComPort.port.DataReceived += new SerialDataReceivedEventHandler(DataReceiveHandlerUSB);

                    textBoxIP.Enabled = false;
                    textBoxPort.Enabled = false;
                    buttonConnectLAN.Enabled = false;

                    comboBoxPorts.Enabled = false;
                    buttonUpdatePorts.Enabled = false;

                    buttonConnectUSB.Text = "Откл";
                    StartDrawing();
                    port.SendString("DISPLAY:AUTOSEND 1");
                }
            }
        }

        private void buttonConnectLAN_Click(object sender, EventArgs e)
        {
            try
            {
                if(socket.IsConnected())                                            // Проверяем, установлено ли уже соединение, и если да
                {
                    buttonConnectLAN.Text = "Подкл";
                    textBoxIP.Enabled = true;
                    textBoxPort.Enabled = true;
                    buttonUpdatePorts.Enabled = true;
                    EnableControlsUSB(true);
                }
                else                                                                // А по этой ветке подключаемся
                {
                    if(socket.Connect(textBoxIP.Text, Int32.Parse(textBoxPort.Text)))
                    {
                        SocketTCP.ReceiveEvent += DataReceiveHandlerLAN;

                        buttonConnectLAN.Text = "Откл";
                        textBoxIP.Enabled = false;
                        textBoxPort.Enabled = false;

                        comboBoxPorts.Enabled = false;
                        buttonUpdatePorts.Enabled = false;
                        buttonConnectUSB.Enabled = false;

                        StartDrawing();
                        socket.SendString("DISPLAY:AUTOSEND 3");
                    }
                }
            }
            catch(Exception)
            {

            }
        }

        private void cbPorts_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxPorts.Enabled)
            {
                buttonConnectUSB.Enabled = port.DeviceConnectToPort(comboBoxPorts.SelectedIndex);
            }
        }

        private void MainForm_Closed(object sender, EventArgs e)
        {
            needForDisconnect = true;

            while (port.IsOpen() || socket.IsConnected())
            {
            };
        }

        // Активировать/деактивировать элементы управления, отвечающие за связь по USB
       private void EnableControlsUSB(bool enable)
        {
            if (enable)
            {
                cbPorts_SelectedIndexChanged(null, null);
            }
            comboBoxPorts.Enabled = enable;
            buttonUpdatePorts.Enabled = enable;
        }

        private void DataReceiveHandlerLAN(object sender, EventArgs args)
        {
            //Console.WriteLine("ReceiveLAN enter");
            try
            {
                EventArgsReceiveSocketTCP a = (EventArgsReceiveSocketTCP)args;

                mutexData.WaitOne();
                for(int i = 0; i < a.data.Length; i++)
                {
                    data.Enqueue(a.data[i]);
                }
                mutexData.ReleaseMutex();
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            //Console.WriteLine("ReceiveLAN leave");
        }

        private void DataReceiveHandlerUSB(object sender, SerialDataReceivedEventArgs args)
        {
            try
            {
                SerialPort port = LibraryS8_53.ComPort.port;

                if (port.IsOpen && port.BytesToRead != 0)
                {
                    mutexData.WaitOne();
                    while (port.BytesToRead != 0)
                    {
                        data.Enqueue((byte)port.ReadByte());
                    }
                    mutexData.ReleaseMutex();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        // Выполнить имеющиеся данные
        private static void RunData()
        {
            Console.WriteLine("RunData enter");
            try
            {
                while (isRunning)
                {
                    byte command = (byte)int8();

                    if ((Command)command == Command.SET_COLOR)
                    {
                        //Console.WriteLine("SET_COLOR ENTER");
                        Display.SetColor((uint)int8());
                        //Console.WriteLine("SET_COLOR_LEAVE");
                    }
                    else if ((Command)command == Command.SET_PALETTE)
                    {
                        Display.SetPalette((byte)int8(), (ushort)int16());
                    }
                    else if ((Command)command == Command.FILL_REGION)
                    {
                        Display.FillRegion(int16(), int8(), int16(), int8());
                    }
                    else if ((Command)command == Command.END_SCENE)
                    {
                        // Выводим нарисованную картинку
                        Display.EndScene();

                        if (needForDisconnect)
                        {
                            port.Stop();
                            socket.Disconnect();
                            isRunning = false;
                        }
                        else
                        {
                            if (port.IsOpen())
                            {
                                // Посылаем имеющиеся команды
                                while (commands.Count != 0)
                                {
                                    port.SendString(commands.Dequeue());
                                }

                                // И делаем запрос на следующий фрейм
                                port.SendString("DISPLAY:AUTOSEND 2");
                            }
                            else
                            {
                                while(commands.Count != 0)
                                {
                                    socket.SendString(commands.Dequeue());
                                }
                                socket.SendString("DISPLAY:AUTOSEND 2");
                            }
                        }
                    }
                    else if ((Command)command == Command.DRAW_HLINE)
                    {
                        Display.DrawHLine(int8(), int16(), int16());
                    }
                    else if ((Command)command == Command.DRAW_VLINE)
                    {
                        Display.DrawVLine(int16(), int8(), int8());
                    }
                    else if ((Command)command == Command.SET_POINT)
                    {
                        Display.SetPoint(int16(), int8());
                    }
                    else if ((Command)command == Command.DRAW_SIGNAL_POINTS)
                    {
                        int x0 = int16();

                        for (int i = 0; i < 281; i++)
                        {
                            Display.SetPoint(x0 + i, int8());
                        }
                    }
                    else if ((Command)command == Command.DRAW_SIGNAL_LINES)
                    {
                        int x0 = int16();

                        int prevX = int8();

                        for (int i = 0; i < 280; i++)
                        {
                            int nextX = int8();
                            Display.DrawVLine(x0 + i, prevX, nextX);
                            prevX = nextX;
                        }
                    }
                    else if ((Command)command == Command.DRAW_MULTI_HPOINT_LINES_2)
                    {
                        int numLines = int8();
                        int x0 = int16();
                        int numPoints = int8();
                        int dX = int8();
                        for (int i = 0; i < numLines; i++)
                        {
                            int y = int8();

                            for (int point = 0; point < numPoints; point++)
                            {
                                Display.SetPoint(x0 + dX * point, y);
                            }
                        }
                    }
                    else if ((Command)command == Command.DRAW_MULTI_VPOINT_LINES)
                    {
                        int numLines = int8();
                        int y0 = int8();
                        int numPoints = int8();
                        int dY = int8();
                        int8();
                        for (int i = 0; i < numLines; i++)
                        {
                            int x = int16();

                            for (int point = 0; point < numPoints; point++)
                            {
                                Display.SetPoint(x, y0 + dY * point);
                            }
                        }
                    }
                    else if ((Command)command == Command.DRAW_VLINES_ARRAY)
                    {
                        int x0 = int16();
                        int numLines = int8();
                        for (int i = 0; i < numLines; i++)
                        {
                            Display.DrawVLine(x0 + i, int8(), int8());
                        }
                    }
                    else if ((Command)command == Command.LOAD_FONT)
                    {
                        int typeFont = int8();
                        if (typeFont < 4)
                        {
                            Display.fonts[typeFont] = new Display.MyFont();
                            Display.fonts[typeFont].height = int8();
                            int8();
                            int8();
                            int8();
                            for (int i = 0; i < 256; i++)
                            {
                                Display.fonts[typeFont].symbols[i] = new Display.Symbol();
                                Display.fonts[typeFont].symbols[i].width = int8();
                                for (int j = 0; j < 8; j++)
                                {
                                    Display.fonts[typeFont].symbols[i].bytes[j] = int8();
                                }
                            }
                        }
                    }
                    else if ((Command)command == Command.SET_FONT)
                    {
                        Display.currentFont = int8();
                    }
                    else if ((Command)command == Command.DRAW_TEXT)
                    {
                        int x0 = int16();
                        int y0 = int8();
                        int numSymbols = int8();
                        char[] str = new char[numSymbols];
                        for (int i = 0; i < numSymbols; i++)
                        {
                            str[i] = (char)int8();
                        }
                        Display.DrawText(x0, y0, str);
                    }
                    else
                    {
                        Console.WriteLine("Неизвестная команда " + command);
                    }
                }
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
            Console.WriteLine("RunData leave");
        }

        private static int int8()
        {
            int retValue = 0;
            try
            {
                while (data.Count == 0 && isRunning) { };

                mutexData.WaitOne();
                retValue = data.Dequeue();
                mutexData.ReleaseMutex();
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            return retValue;
        }

        private static int int16()
        {
            while (data.Count < 2) { };

            return int8() + (int8() << 8);
        }

        private static void StartDrawing()
        {
            data.Clear();
            isRunning = true;
            runProcess = new Thread(RunData);
            runProcess.Start();
        }
    }
}
