#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "HardCodedData.h"
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"

#ifndef CONNECTION_THREAD_H
#define CONNECTION_THREAD_H

DWORD ServiceThread(SOCKET *t_socket);


#endif // CONNECTION_THREAD_H