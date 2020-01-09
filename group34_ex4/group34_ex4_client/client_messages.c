//client_messages.c

// Description - This module contains the messages the cilent can send to the server

// Includes --------------------------------------------------------------------

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include "group34_ex4_client.h"
#include "HardCodedData.h"
#include <stdio.h>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#pragma warning(disable:4996) // in order to avoid warning about fopen being unsafe function.

//Messages Declarations-------------------------------------------------------------------


