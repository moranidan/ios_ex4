#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef CONNECTION_THREAD_H
#define CONNECTION_THREAD_H

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "HardCodedData.h"
#include <time.h>
#include <stdlib.h>
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"




DWORD ServiceThread(LPVOID lpParam);


#endif // CONNECTION_THREAD_H