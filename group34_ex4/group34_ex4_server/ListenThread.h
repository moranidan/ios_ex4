#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef LISTEN_THREAD_H
#define LISTEN_THREAD_H

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "HardCodedData.h"
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "ConnectionThread.h"
#include "Server.h"

static int FindFirstUnusedThreadSlot(HANDLE *ThreadHandles);
DWORD WINAPI ListenThread(LPVOID lpParam);

#endif // LISTEN_THREAD_H