//group34_ex4_client.c

// Description - This module provides functions and tools for main.c

#pragma once

// Includes --------------------------------------------------------------------

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include "group34_ex4_client.h"
#include "HardCodedData.h"
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include <stdio.h>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#pragma warning(disable:4996) // in order to avoid warning about fopen being unsafe function.


// Function Definitions --------------------------------------------------------

SOCKET server_socket;

//Reading data coming from the server
static DWORD RecvDataThread(void)
{
	TransferResult_t RecvRes;

	while (1)
	{
		char *AcceptedStr = NULL;
		RecvRes = ReceiveString(&AcceptedStr, server_socket);

		if (RecvRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("Server closed connection. Bye!\n");
			return 0x555;
		}
		else
		{
			printf("%s\n", AcceptedStr);
		}

		free(AcceptedStr);
	}

	return 0;
}

//Sending data to the server
static DWORD SendDataThread(void)
{
	char SendStr[256];
	TransferResult_t SendRes;

	while (1)
	{
		gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard

		if (STRINGS_ARE_EQUAL(SendStr, "quit"))
			return 0x555; //"quit" signals an exit from the client side

		SendRes = SendString(SendStr, server_socket);

		if (SendRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
	}
}

int connecting_to_server(unsigned long *server_ip, int server_port, void *p_server_socket) {

	SOCKADDR_IN clientService;
	HANDLE hThread[2];
	WSADATA wsaData;

	//Call WSAStartup and check for errors.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);               //open socket
	// Check for errors to ensure that the socket is a valid socket.
	if (server_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return ERR_CODE_SOCKET;
	}
	//Create a sockaddr_in object clientService and set values.
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = server_ip; //Setting the IP address to connect to
	clientService.sin_port = htons(server_port); //Setting the port to connect to.

	// Call the connect function, passing the created socket and the sockaddr_in structure as parameters. 
	// Check for general errors.
	if (connect(server_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {               //connect to server
		printf("Failed to connect.\n");
		WSACleanup();
		return ERR_CODE_SOCKET;
	}

	hThread[0] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)SendDataThread,
		NULL,
		0,
		NULL
	);
	hThread[1] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)RecvDataThread,
		NULL,
		0,
		NULL
	);

	WaitForMultipleObjects(2, hThread, FALSE, INFINITE);
	p_server_socket = &server_socket;
	WSACleanup();
}


// utility functions --------------------------------------------------------------

int check_arguments(int argc, int *return_code) {
	if (argc < 4) {    //check if there are enough arguments
		printf("not enough arguments in group34_ex4_client.exe ");
		*return_code = ERR_CODE_NOT_ENOUGH_ARGUMENTS;
	}
	return *return_code;
}

int create_and_check_mutex(
	HANDLE *mutex_handle,
	LPSECURITY_ATTRIBUTES lpMutexAttributes,
	BOOL bInitialOwner,
	LPCTSTR lpName,
	int *return_code
) {
	*mutex_handle = CreateMutex(lpMutexAttributes, bInitialOwner, lpName);
	if (*mutex_handle == NULL) {
		printf("Error when creating mutex: %d\n", GetLastError());
		*return_code = ERR_CODE_MUTEX;
	}
	return *return_code;
}

int open_and_check_mutex(
	HANDLE *mutex_handle,
	LPSECURITY_ATTRIBUTES lpMutexAttributes,
	BOOL bInitialOwner,
	LPCTSTR lpName,
	int *return_code
) {
	*mutex_handle = OpenMutex(lpMutexAttributes, bInitialOwner, lpName);
	if (*mutex_handle == NULL) {
		printf("Error when opening mutex: %d\n", GetLastError());
		*return_code = ERR_CODE_MUTEX;
	}
	return *return_code;
}

int lock_mutex(HANDLE *mutex_handle, int *return_code) {
	DWORD wait_code;
	wait_code = WaitForSingleObject(*mutex_handle, INFINITE);
	if (wait_code != WAIT_OBJECT_0) {
		printf("Error when locking mutex\n");
		*return_code = ERR_CODE_MUTEX;
		return ERR_CODE_MUTEX;
	}
	return SUCCESS_CODE;
}

int release_mutex(HANDLE *mutex_handle, int *return_code) {
	BOOL ret_val;
	ret_val = ReleaseMutex(*mutex_handle);
	if (ret_val == FALSE) {
		printf("Error when releasing mutex: %d\n", GetLastError());
		*return_code = ERR_CODE_MUTEX;
		return ERR_CODE_MUTEX;
	}
	return SUCCESS_CODE;
}

void close_handle(HANDLE *handle) {
	BOOL ret_val;
	ret_val = CloseHandle(*handle);
	if (FALSE == ret_val) {
		printf("Error when closing handle: %d\n", GetLastError());
	}
}