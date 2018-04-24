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

namespace S8_53_USB {

    public partial class MainForm : Form {

        private bool needForDisconnect = false;

        // Этот порт используется для соединения по USB
        private LibraryS8_53.ComPort port = new LibraryS8_53.ComPort();
        // Этот сокет используется для соединения по LAN
        private LibraryS8_53.SocketTCP socket = new LibraryS8_53.SocketTCP();

        private Dictionary<Button, string> mapButtons = new Dictionary<Button, string>();

        private Queue<string> commands = new Queue<string>();
        // Сюда будем считывать данные из порта
        private Queue<byte> data = new Queue<byte>();
        // Mutex на буфер data
        private Mutex mutexData = new Mutex();
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

            LibraryS8_53.ComPort.port.DataReceived += new SerialDataReceivedEventHandler(DataReceiveHandler);

            buttonUpdatePorts_Click(null, null);

            buttonConnectLAN.Enabled = true;
        }

        private void button_MouseDown(object sender, MouseEventArgs e) {
            commands.Enqueue("KEY:" + StringToSendForButton(sender) + " DOWN");
        }

        private void button_MouseUp(object sender, MouseEventArgs e) {
            commands.Enqueue("KEY:" + StringToSendForButton(sender) + " UP");
        }

        private void governor_RotateLeft(object sender, EventArgs e) {
            commands.Enqueue("GOV:" + ((Governor)sender).ValueToSend + " LEFT");
        }

        private void governor_RotateRight(object sender, EventArgs e) {
            commands.Enqueue("GOV:" + ((Governor)sender).ValueToSend + " RIGHT");
        }

        private string StringToSendForButton(object btn) {
            return mapButtons[(Button)btn];
        }

        private void buttonUpdatePorts_Click(object sender, EventArgs e) {
            string[] ports = port.GetPorts();
            comboBoxPorts.Items.Clear();
            comboBoxPorts.Items.AddRange(ports);
            comboBoxPorts.SelectedIndex = ports.Length - 1;
        }

        private void buttonConnectUSB_Click(object sender, EventArgs e)
        {
            if (port.IsOpen())                                  // Если порт открыт - идёт обмен с прибором. Будем отключать
            {
                buttonConnectUSB.Text = "Подкл";
                comboBoxPorts.Enabled = true;
                buttonUpdatePorts.Enabled = true;
                needForDisconnect = true;                       // сообщаем прибору, что нужно отключиться при первой возможности

                textBoxIP.Enabled = true;
                textBoxPort.Enabled = true;
                buttonConnectLAN.Enabled = true;
            }
            else
            {
                if (port.Connect(comboBoxPorts.SelectedIndex, false)) // иначе делаем попыткую подключиться
                {
                    needForDisconnect = false;

                    textBoxIP.Enabled = false;
                    textBoxPort.Enabled = false;
                    buttonConnectLAN.Enabled = false;

                    comboBoxPorts.Enabled = false;
                    buttonUpdatePorts.Enabled = false;

                    buttonConnectUSB.Text = "Откл";
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
                    needForDisconnect = true;
                    EnableControlsUSB(true);
                }
                else                                                                // А по этой ветке подключаемся
                {
                    if(socket.Connect(textBoxIP.Text, Int32.Parse(textBoxPort.Text)))
                    {
                        needForDisconnect = false;

                        buttonConnectLAN.Text = "Откл";
                        textBoxIP.Enabled = false;
                        textBoxPort.Enabled = false;

                        comboBoxPorts.Enabled = false;
                        buttonUpdatePorts.Enabled = false;
                        buttonConnectUSB.Enabled = false;

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

        private ManualResetEvent waitForSend = new ManualResetEvent(false);

        private void OnEndFrameEvent(object sender, EventArgs e)
        {
            if (port.IsOpen())                                      // Если идёт обмен по USB
            {
                if (needForDisconnect)
                {
                    port.Stop();
                }
                else
                {
                    while (commands.Count != 0)
                    {
                        Thread.Sleep(10);
                        port.SendString(commands.Dequeue());
                        Thread.Sleep(10);
                    }
                    Thread.Sleep(10);
                    port.SendString("DISPLAY:AUTOSEND 2");
                }
            }
            else                                                    // Если обмен идёт по LAN
            {
                if(needForDisconnect)
                {
                    socket.Disconnect();
                }
                else
                {
                    while(commands.Count != 0)
                    {
                        Thread.Sleep(50);
                        socket.SendString(commands.Dequeue());
                        Thread.Sleep(50);
                    }

                    Thread.Sleep(50);
                    socket.SendString("DISPLAY:AUTOSEND 2");
                }
            }
        }

        private void MainForm_Closed(object sender, EventArgs e)
        {
            // Закрывать порт непосредственно по закрытии формы нельзя, чтобы поток не завис.
            needForDisconnect = true;                           // Поэтому устанавливаем признако того, что порт надо закрыть
            while(port.IsOpen() || socket.IsConnected()) { }    // И ждём пока это произойдёт
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

        private void DataReceiveHandler(object sender, SerialDataReceivedEventArgs args)
        {
            string indata = LibraryS8_53.ComPort.port.ReadExisting();

            SerialPort port = LibraryS8_53.ComPort.port;

            mutexData.WaitOne();
            while(port.BytesToRead != 0)
            {
                data.Enqueue((byte)port.ReadByte());
            }
            mutexData.ReleaseMutex();

            RunData();
        }

        // Выполнить имеющиеся данные
        private void RunData()
        {
            mutexData.WaitOne();

            while(data.Count != 0)
            {
                byte command = (byte)int8();

                if((Command)command == Command.SET_COLOR)
                {
                    Display.SetColor((uint)int8());
                }
                else if((Command)command == Command.SET_PALETTE)
                {
                    Display.SetPalette((byte)int8(), (ushort)int16());
                }
                else if ((Command)command == Command.FILL_REGION)
                {
                    Display.FillRegion(int16(), int8(), int16(), int8());
                }
                else if ((Command)command == Command.END_SCENE)
                {
                    Display.EndScene();
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
                    Console.WriteLine("LOAD FONT");
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

            mutexData.ReleaseMutex();
        }

        private int int8()
        {
            return data.Dequeue();
        }

        private int int16()
        {
            return int8() + (int8() << 8);
        }
    }
}
