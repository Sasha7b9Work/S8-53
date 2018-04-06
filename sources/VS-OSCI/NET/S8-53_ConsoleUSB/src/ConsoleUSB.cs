using System;
using System.Collections.Generic;
using System.Linq;
using System.IO.Ports;
using System.Text;
using System.Threading.Tasks;

namespace S8_53_ConsoleUSB
{
    public class ConsoleUSB
    {
        // Указатель на функцию - обработчик консольной команды
        public delegate void FuncOnCommand(string[] args);

        public delegate void FuncOnReceive(string data);

        private bool isRun = true;

        private String promptRecv = "S8-53 USB > ";     // Так выводится принятое сообщение
        private String promptSend = "S8-53 USB < ";     // Так выводится приглашение к набору
        private String emptyPrompt = "            ";

        private Queue<Command> commands = new Queue<Command>();

        private LibraryS8_53.ComPort port = new LibraryS8_53.ComPort();

        private string[] namesPorts;

        public void Run()
        {
            port.ReceiveEvent += DataReceivedHandler;

            commands.Enqueue(new Command("ports",      "список доступных портов",      CommandGetPorts));
            commands.Enqueue(new Command("connect",    "подключиться заданному порту", CommandConnect));
            commands.Enqueue(new Command("disconnect", "отключиться от порта",         CommandDisconnect));
            commands.Enqueue(new Command("exit",       "выход",                        CommandExit));
            commands.Enqueue(new Command("help",       "список доступных команд",      CommandWriteHelp));
            commands.Enqueue(new Command("?",          "список доступных команд",      CommandWriteHelp));

            CommandGetPorts();

            WriteLine("Жду команду. Для получения помощи наберите \"help\" или \"?\"");

            CommandConnect();

            while (isRun)
            {
                String command = ReadLine();

                ParseCommand(command);

                Console.WriteLine();
            }
        }

        private void ParseCommand(String line)
        {
            string[] args = line.Split(' ');

            if (port.IsOpen())                                      // Если устройство подключено
            {
                if(args[0] == "disconnect")                             // То сначала проверяем на команду отключения
                {
                    CommandDisconnect(args);
                }
                else                                                    // А потом посылаем сообщение устройству
                {
                    port.SendString(line);
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

        // Получает список доступных портов и выводит его на экран
        private void CommandGetPorts(string[] args = null)
        {
            if (port.IsOpen())
            {
                WriteLine("Для получения списка портов сначала отсоедините активное устройство (команда disconnect)");
            }
            else
            {
                namesPorts = port.GetPorts();

                WriteLine("Список доступных портов");

                for (int i = 0; i < namesPorts.Length; ++i)
                {
                    if (port.DeviceConnectToPort(i))
                    {
                        Console.WriteLine(emptyPrompt + "* " + namesPorts[i] + "   готов к подключению");
                    }
                    else
                    {
                        Console.WriteLine(emptyPrompt + "  " + namesPorts[i]);
                    }
                }
            }
        }

        private void CommandConnect(string[] args = null)
        {
            if (port.IsOpen())
            {
                WriteLine("Сначала отсоедините действующее устройство (команда disconnect)");
            }
            else
            {
                if (args != null && args.Length > 1)                            // Если есть аргументы, будем подключаться к указанному порту
                {
                    for (int i = 0; i < namesPorts.Length; i++)
                    {
                        if (args[1] == namesPorts[i])
                        {
                            if (port.Connect(i, true))
                            {
                                WriteLine("Устройство подключено к " + args[1]);
                                return;
                            }
                        }
                    }

                    WriteError("Не погу подключить устройство на порту " + args[1]);
                    return;
                }
                else
                {
                    for(int i = 0; i < namesPorts.Length; i++)
                    {
                        if(port.DeviceConnectToPort(i))
                        {
                            if (port.Connect(i, true))
                            {
                                WriteLine("Устройство поключено к " + namesPorts[i] + ". Для отключения наберите \"disconnect\"");
                            }
                            return;
                        }
                    }
                }
            }
        }

        private void CommandDisconnect(string[] arg = null)
        {
            port.Stop();
            WriteLine("Устройство отключено");
        }

        public void DataReceivedHandler(object sender, EventArgs args)
        {
            Console.Write("\r");
            WriteLine(((LibraryS8_53.EventArgsReceiveComPort)args).data);
            Console.Write(promptSend);
        }
    }
}
