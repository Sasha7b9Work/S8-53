using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO.Ports;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Drawing.Imaging;
using System.Net;
using System.Net.Sockets;

namespace ControlLibraryS8_53
{
    public partial class Display : UserControl
    {
        [DllImport("gdi32")]
        private extern static int SetDIBitsToDevice(HandleRef hDC, int xDest, int yDest, int dwWidth, int dwHeight, int XSrc, int YSrc, int uStartScan,
            int cScanLines, ref int lpvBits, ref BITMAPINFO lpbmi, uint fuColorUse);

        [StructLayout(LayoutKind.Sequential)]
        private struct BITMAPINFOHEADER
        {
            public int bihSize;
            public int bihWidth;
            public int bihHeight;
            public short bihPlanes;
            public short bihBitCount;
            public int bihCompression;
            public int bihSizeImage;
            public double bihXPelsPerMeter;
            public double bihClrUsed;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct BITMAPINFO
        {
            public BITMAPINFOHEADER biHeader;
            public int biColors;
        }

        BITMAPINFO bmi;
        private static int[] buffer = new int[320 * 240];
        private static Graphics g;

        public class Symbol
        {
            public int width;
            public int[] bytes;
            public Symbol()
            {
                bytes = new int[8];
            }

            public Symbol(int width_, int x0, int x1, int x2, int x3, int x4, int x5, int x6, int x7)
            {
                width = width_;
                bytes = new int[8];
                bytes[0] = x0;
                bytes[1] = x1;
                bytes[2] = x2;
                bytes[3] = x3;
                bytes[4] = x4;
                bytes[5] = x5;
                bytes[6] = x6;
                bytes[7] = x7;
            }

            public Symbol(int width_, int[] bytes_)
            {
                width = width_;
                bytes = bytes_;
            }
        }

        public class MyFont 
        {
            public int height;
            public Symbol[] symbols;
            public MyFont()
            {
                symbols = new Symbol[256];
            }
        }

        public static MyFont []fonts = new MyFont[4];

        private static Color color = Color.Black;

        private static Color[] colors = new Color[16];

        private static PictureBox display;

        private static Stopwatch stopWatch = new Stopwatch();

        public static int currentFont = 0;

        public Display()
        {
            InitializeComponent();

            InitFont5();
            InitFont8();
            InitFontUGO();
            InitFontUGO2();

            bmi = new BITMAPINFO
            {
                biHeader =
                {
                    bihBitCount = 32,
                    bihPlanes = 1,
                    bihSize = 40,
                    bihWidth = 320,
                    bihHeight = 240,
                    bihSizeImage = 320 * 240 * 4
                }
            };

            g = pbDisplay.CreateGraphics();
            display = pbDisplay;

            stopWatch.Start();
        }

        public static void SetPoint(int x, int y)
        {
            if(x >= 0 && x < 320 && y >= 0 && y < 240)
            {
                buffer[x + y * 320] = color.ToArgb();
            }
        }

        public static void EndScene()
        {
            Bitmap bmp = new Bitmap(320, 240, PixelFormat.Format32bppArgb);
            BitmapData bmData = bmp.LockBits(new Rectangle(0, 0, 320, 240), ImageLockMode.ReadWrite, bmp.PixelFormat);
            bmData.Stride = 320 * 4;
            Marshal.Copy(buffer, 0, bmData.Scan0, 320 * 240);
            bmp.UnlockBits(bmData);
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
            g.DrawImage(bmp, new Rectangle(0, 0, 639, 479), 0, 0, 319, 239, GraphicsUnit.Pixel);
        }

        public static void FillRegion(int x, int y, int width, int height)
        {
            for(int col = x; col <= x + width; col++)
            {
                for(int row = y; row <= y + height; row++)
                {
                    SetPoint(col, row);
                }
            }
        }

        public static void DrawHLine(int y, int x0, int x1)
        {
            if(x1 < x0)
            {
                int temp = x0;
                x0 = x1;
                x1 = temp;
            }
            for(int x = x0; x <= x1; x++)
            {
                SetPoint(x, y);
            }
        }

        public static void DrawVLine(int x, int y0, int y1)
        {
            if(y1 < y0)
            {
                int temp = y0;
                y0 = y1;
                y1 = temp;
            }
            for(int y = y0; y <= y1; y++)
            {
                SetPoint(x, y);
            }
        }

        public static void SetColor(uint numColor)
        {
            if(numColor < 16)
            {
                color = colors[numColor];
            }
        }

        private static int GetBit(int value, int numBit)
        {
            return (value >> numBit) & 1;
        }

        public static void DrawText(int x, int y, char[] str)
        {
            for (int numSymbol = 0; numSymbol < str.Length; numSymbol++)
            {
                int symbol = (int)str[numSymbol];
                int height = fonts[currentFont].height;
                int width = fonts[currentFont].symbols[symbol].width;

                for (int row = 0; row < height; row++)
                {
                    for (int col = 0; col < width; col++)
                    {
                        if (GetBit(fonts[currentFont].symbols[symbol].bytes[row], col + (8 - width)) == 1)
                        {
                            SetPoint(x + (width - col), y + row);
                        }
                    }
                }

                x += width + 1;
            }
        }

        public static void SetPalette(byte numColor, UInt16 color)
        {
            colors[numColor] = ColorFromUINT16(color);
        }

        private static Color ColorFromUINT16(UInt16 color)
        {
            float kRed = 255.0f / 31;
            float kGreen = 255.0f / 63;
            float kBlue = 255.0f / 31;
            int r = (color >> 11) & 31;
            int g = (color >> 5) & 63;
            int b = color & 31;
            return Color.FromArgb((int)(r * kRed), (int)(g * kGreen), (int)(b * kBlue));
        }
    }
}
