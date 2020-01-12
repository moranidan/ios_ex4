#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "HardCodedData.h"
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "ConnectionThread.c"
#include "ListenThread.h"

typedef struct
{
	SOCKET *MainSocket;
	HANDLE *ThreadHandles;
	SOCKET *ThreadInputs;
} LISTEN_THREAD_params_t;

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

void MainServer(char *argv[]);

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#endif // SERVER_H