// ListenThread.h

// This header consists the function declerations for all the functions used in ListenThread.c, and their documentaion

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef LISTEN_THREAD_H
#define LISTEN_THREAD_H

// Includes --------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "HardCodedData.h"
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "ConnectionThread.h"

// Function Declarations -------------------------------------------------------

/*
This function is the main thread function, which accepts new connections.
Input:  LPVOID lpParam - pointer to parameters for the new thread, only one is permitted so we send struct
Output: DWORD, success or error code
*/
DWORD WINAPI ListenThread(LPVOID lpParam);

/*
This function finds the first available slot index, for storing new thread handle.
Input:  char *argv[] - pointer to command line arguments
Output: HANDLE *ThreadHandles - pointer to Thread Handles array
*/
static int FindFirstUnusedThreadSlot(HANDLE *ThreadHandles);


#endif // LISTEN_THREAD_H