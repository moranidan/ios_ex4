// Server.h

// This header consists the function declerations for all the functions used in server.c, and their documentaion

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef SERVER_H
#define SERVER_H

// Includes --------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "HardCodedData.h"
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "ConnectionThread.h"
#include "ListenThread.h"


// Function Declarations -------------------------------------------------------


/*
This function is the main server program.
Input:  char *argv[] - pointer to command line arguments
Output: void
*/
void MainServer(char *argv[]);

/*
This function closes all the open connection threads, and the listen thread.
Called when closing program.
Input:  void
Output: void
*/
static void CleanupWorkerThreads();

/*
This function creats a thread.
Input:  LPTHREAD_START_ROUTINE p_start_routine - function to be called in the new thread created
		LPVOID p_thread_parameters - pointer to parameters for the new thread, only one is permitted so we send struct
		LPDWORD p_thread_id - pointer for thread id to be written
Output: void, creates thread
*/
static HANDLE CreateThreadSimple(
	LPTHREAD_START_ROUTINE p_start_routine,
	LPVOID p_thread_parameters,
	LPDWORD p_thread_id
);

#endif // SERVER_H