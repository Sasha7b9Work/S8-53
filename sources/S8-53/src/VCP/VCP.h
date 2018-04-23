#pragma once


class VCP
{
public:
    static void Init();
    static void SendDataSynch(const uint8 *data, int size);
    static void SendStringSynch(char *data);                   ///< Передаётся строка без завершающего нуля.
    static void SendFormatStringAsynch(char *format, ...);     ///< Эта строка передаётся с завершающими символами \r\n.
    static void SendFormatStringSynch(char *format, ...);      ///< Эта строка передаётся с завершающими символами \r\n.
    static void SendByte(uint8 data);
    static void Flush();
};
