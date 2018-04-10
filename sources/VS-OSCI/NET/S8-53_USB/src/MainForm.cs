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

            Display.EndFrameEvent += OnEndFrameEvent;

            btnUpdatePorts_Click(null, null);
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

        private void btnUpdatePorts_Click(object sender, EventArgs e) {
            string[] ports = port.GetPorts();
            cbPorts.Items.Clear();
            cbPorts.Items.AddRange(ports);
            cbPorts.SelectedIndex = ports.Length - 1;
        }

        private void btnConnectUSB_Click(object sender, EventArgs e)
        {
            if (port.IsOpen())                                  // Если порт открыт - идёт обмен с прибором
            {
                needForDisconnect = true;                       // сообщаем прибору, что нужно отключиться при первой возможности
                btnConnectUSB.Text = "Подкл";
            }
            else
            {
                if (port.Connect(cbPorts.SelectedIndex, false)) // иначе делаем попыткую подключиться
                {
                    btnConnectUSB.Text = "Откл";
                    port.SendString("DISPLAY:AUTOSEND 1");
                    display.StartDrawing(port.GetSerialPort());
                    needForDisconnect = false;
                    tabControl1.Enabled = false;
                }
            }
        }

        private void buttonConnectLAN_Click(object sender, EventArgs e)
        {

        }

        private void cbPorts_SelectedIndexChanged(object sender, EventArgs e)
        {
            btnConnectUSB.Enabled = port.DeviceConnectToPort(cbPorts.SelectedIndex);
        }

        private void OnEndFrameEvent(object sender, EventArgs e)
        {
            if (needForDisconnect)
            {
                port.Stop();
            }
            else
            {
                while (commands.Count != 0)
                {
                    port.SendString(commands.Dequeue());
                }
                port.SendString("DISPLAY:AUTOSEND 2");
                display.StartDrawing(port.GetSerialPort());
            }
        }

        private void MainForm_Closed(object sender, EventArgs e)
        {
            // Закрывать порт непосредственно по закрытии формы нельзя, чтобы поток не завис.
            needForDisconnect = true;           // Поэтому устанавливаем признако того, что порт надо закрыть
            while(port.IsOpen()) { }            // И ждём пока это произойдёт
        }
    }
}
