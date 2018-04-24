#pragma once
#include "defines.h"


struct tcp_pcb;

static const int    DEFAULT_PORT = 7,
                    POLICY_PORT = 843;

class SocketTCP
{
public:
static bool Init(void (*funcConnect)(void), void (*funcReciever)(const char *buffer, uint length));
static bool Send(const char *buffer, uint length);
static void SendFormatString(char *format, ...);
};
