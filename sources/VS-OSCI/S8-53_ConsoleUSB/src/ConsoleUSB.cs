using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace S8_53_ConsoleUSB
{
    //public delegate void FuncOnCommand();

    public class ConsoleUSB
    {
        private bool isRun = true;

        private String prompt = "S8-53 USB > ";
        private String emptyPrompt = "            ";

        private Queue<Command> commands = new Queue<Command>();

        private ComPort port = new ComPort();

        private string[] namesPorts;

        public delegate void FuncOnCommand(string[] args);

        public void Run()
        {
            commands.Enqueue(new Command("ports", 
                "список доступных портов", 
                CommandGetPorts));
            commands.Enqueue(new Command("connect", 
                "подключиться заданному порту", 
                CommandConnect));
            commands.Enqueue(new Command("disconnect", 
                "отключиться от порта", 
                CommandDisconnect));
            commands.Enqueue(new Command("exit", 
                "выход", 
                CommandExit));
            commands.Enqueue(new Command("help", 
                "список доступных команд", 
                CommandWriteHelp));

            CommandGetPorts();

            WriteLine("Жду команду. Для получения помощи наберите \"help\" или \"?\"");

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
            
            for(int i = 0; i < commands.Count(); i++)
            {
                if(commands.ToArray()[i].name == args[0])
                {
                    commands.ToArray()[i].Run(args);
                    return;
                }
            }

            WriteLine("Неправильная команда \"" + args[0] + "\"");
        }

        private void WriteLine(String str)
        {
            Console.WriteLine(prompt + str);
        }

        private String ReadLine()
        {
            Console.Write(prompt);
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

        private void CommandConnect(string[] args)
        {
            if (port.IsOpen())
            {
                WriteLine("Сначала отсоедините действующее устройство (команда disconnect)");
            }
            else
            {
                for (int i = 0; i < namesPorts.Length; i++)
                {
                    if (args[1] == namesPorts[i])
                    {
                        if (port.Open(i))
                        {
                            WriteLine("Устройство подключено к порту " + args[1]);
                        }
                        else
                        {
                            WriteLine("Ошибка!!! Не погу подключить устройство");
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
    }
}
