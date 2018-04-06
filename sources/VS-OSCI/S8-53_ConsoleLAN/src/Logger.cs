using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace S8_53_ConsoleLAN
{
    public class Logger
    {
        private const int SIZE = 1024;
        private string path = null;

        [DllImport("kernel32.dll", EntryPoint = "GetPrivateProfileString")]
        private static extern int GetPrivateString(string section, string key, string def, StringBuilder buffer, int size, string path);

        [DllImport("kernel32.dll", EntryPoint = "WritePrivateProfileString")]
        private static extern int WritePrivateString(string section, string key, string str, string path);

        public Logger(String nameLog)
        {
            path = nameLog;
        }

        ~Logger()
        {
                
        }

        private string GetPrivateString(string aSection, string aKey)
        {
            StringBuilder buffer = new StringBuilder(SIZE);

            GetPrivateString(aSection, aKey, null, buffer, SIZE, path);

            return buffer.ToString();
        }

        private void WritePrivateString(string aSection, string aKey, string aValue)
        {
            WritePrivateString(aSection, aKey, aValue, path);
        }

        public String NextCommand()
        {
            return "";
        }

        public String PrevCommand()
        {
            return "";
        }
    }
}
