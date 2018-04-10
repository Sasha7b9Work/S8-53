using System;
using System.Collections.Generic;
using System.Linq;
using System.IO.Ports;
using System.Text;
using System.Threading.Tasks;


namespace S8_53_ConsoleLAN
{
    public class ConsoleLAN
    {
        private Logger logger = new Logger("log.txt");

        private string ipAddress = "192.168.1.200";

        private int port = 7;

        // Указатель на функцию - обработчик консольной команды
        public delegate void FuncOnCommand(string[] args);

        //public delegate void FuncOnReceive(string data);

        private bool isRun = true;

        private String promptRecv = "S8-53 LAN > ";     // Так выводится принятое сообщение
        private String promptSend = "S8-53 LAN < ";     // Так выводится приглашение к набору
        private String emptyPrompt = "            ";

        private Queue<Command> commands = new Queue<Command>();

        private LibraryS8_53.SocketTCP socket = new LibraryS8_53.SocketTCP();

        public void Run()
        {
            socket.ReceiveEvent += DataReceivedHandler;

            commands.Enqueue(new Command("connect",    "подключиться заданному порту", CommandConnect));
            commands.Enqueue(new Command("disconnect", "отключиться от порта",         CommandDisconnect));
            commands.Enqueue(new Command("exit",       "выход",                        CommandExit));
            commands.Enqueue(new Command("help",       "список доступных команд",      CommandWriteHelp));
            commands.Enqueue(new Command("?",          "список доступных команд",      CommandWriteHelp));

            //WriteLine("Жду команду. Для получения помощи наберите \"help\" или \"?\"");

            //CommandConnect();

            while (isRun)
            {
                String command = ReadLine();

                ParseCommand(command);
            }
        }

        private void ParseCommand(String line)
        {
            string[] args = line.Split(' ');

            if (socket.IsConnected())                               // Если устройство подключено
            {
                if(args[0] == "disconnect")                         // То сначала проверяем на команду отключения
                {
                    CommandDisconnect(args);
                }
                else                                                // А потом посылаем сообщение устройству
                {
                    socket.SendString(line);
                }
            }
            else                                                    // Если устройство ещё не подключено
            {
                for (int i = 0; i < commands.Count(); i++)
                {
                    if (commands.ToArray()[i].name == args[0])
                    {
                        commands.ToArray()[i].Run(args);
                        return;
                    }
                }

                WriteLine("Неправильная команда \"" + args[0] + "\"");
            }
        }

        private void WriteLine(String str)
        {
            Console.WriteLine(promptRecv + str);
        }

        private void WriteError(String str)
        {
            Console.Write(promptRecv + "ОШИБКА!!! " + str + '\n');
        }

        private String ReadLine()
        {
            Console.Write(promptSend);
            return Console.ReadLine();
        }

        private void CommandWriteHelp(string[] args)
        {
            // Находим число символов в самой длинной команде

            int maxLength = 0;

            foreach(Command command in commands)
            {
                string name = command.name;

                if(name.Length > maxLength)
                {
                    maxLength = name.Length;
                }
            }

            WriteLine("Список команд");

            foreach (Command command in commands)
            {
                Console.Write(emptyPrompt + command.name);

                for(int i = 0; i < maxLength - command.name.Length; i++)
                {
                    Console.Write(' ');
                }

                Console.WriteLine(" - " + command.comment);
            }

            Console.WriteLine("Для получения подробной информации по команде наберите \"<команда> ?\"");
        }

        private void CommandExit(string[] args)
        {
            CommandDisconnect();
            isRun = false;
        }

        private void CommandConnect(string[] args = null)
        {
            if (socket.IsConnected())
            {
                WriteLine("Сначала отсоедините действующее устройство (команда disconnect)");
            }
            else
            {
                if(socket.Connect(ipAddress, port))
                {
                    WriteLine("Устройство подключено к " + ipAddress + ":" + port.ToString());
                }
                else
                {
                    WriteError("Ввведите адрес удалённой точки");
                }
            }
        }

        private void CommandDisconnect(string[] arg = null)
        {
            socket.Disconnect();
            WriteLine("Устройство отключено");
        }

        public void DataReceivedHandler(object sender, EventArgs args)
        {
            Console.Write("\r");
            WriteLine(((LibraryS8_53.EventArgsReceiveSocketTCP)args).data);
            
            Console.Write(promptSend);
        }
    }
}
