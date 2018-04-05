using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace S8_53_ConsoleLAN
{
    public class Command
    {
        public String name;

        public String comment;

        private ConsoleLAN.FuncOnCommand func;

        public Command(String n, String c, ConsoleLAN.FuncOnCommand f)
        {
            name = n;
            comment = c;
            func = f;
        }

        public void Run(string[] args)
        {
            func(args);
        }
    }
}
