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


//gloubal varibels in order to use in both send and recive thread
SOCKET server_socket;
SOCKADDR_IN clientService;
char global_server_ip[MAX_IP_STRING_LEN] = "\0";
int global_server_port;
char message_between_threads[MAX_MESSAGE_TYPE_LENGTH] = "\0";

// Function Definitions --------------------------------------------------------

int try_to_connect(char *menu_type_bef, char *menu_type_aft) {
	char user_input[MAX_USER_LEN_INPUT];
	user_input[0] = '1';

	// open mutex
	HANDLE message_between_threads_mutex_handle = NULL;
	if (open_and_check_mutex(&message_between_threads_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_MESSAGE_BETWEEN_THREADS_NAME, NULL) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}

	while (*user_input == '1') {
		char err = connect(server_socket, (SOCKADDR*)&clientService, sizeof(clientService));
		if (err == SOCKET_ERROR) {               //connect to server
			printf("%s %s:%d%s.\nChoose what to do next:\n1. Try to reconnect\n2. Exit\n",menu_type_bef, global_server_ip, global_server_port, menu_type_aft);
			gets_s(user_input, sizeof(user_input)); //Reading a string from the keyboard
			Sleep(15);

		}
		else {
			goto CONNECTION_SUCCESS;
		}
	}
	if (*user_input == '2') {
		//lock mutex
		if (lock_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}
		//critical zone
		strcpy_s(message_between_threads, MAX_MESSAGE_TYPE_LENGTH, USER_ASKED_TO_QUIT);
		//free mutex
		if (release_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}
		close_handle(&message_between_threads_mutex_handle);
		return ERR_CODE_CONNECTION;
	}
ERR_WITH_MUTEX:
	close_handle(&message_between_threads_mutex_handle);
	return ERR_CODE_MUTEX;
CONNECTION_SUCCESS:
	close_handle(&message_between_threads_mutex_handle);
	return SUCCESS_CODE;
}

//Reading data coming from the server
static DWORD RecvDataThread(void)
{
	TransferResult_t RecvRes;
	int quit = 0;
	// open mutex
	HANDLE message_between_threads_mutex_handle = NULL;
	if (open_and_check_mutex(&message_between_threads_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_MESSAGE_BETWEEN_THREADS_NAME, NULL) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}

	while (1)
	{
		char message_type[MAX_MESSAGE_TYPE_LENGTH];
		message_type[0] = '\0';
		char *params[] = { "","","","" };
		char *AcceptedStr = NULL;
		int time_val = 15000;

		//lock mutex
		if (lock_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}
		//critical zone
		if (message_between_threads == USER_ASKED_TO_QUIT) {
			quit = 1;
		}
		else if (message_between_threads == SENT_CLIENT_VERSUS) {
			time_val = 30000;
		}
		//free mutex
		if (release_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}

		if (quit == 1) {
			break;
		}

		setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, &time_val, sizeof(time_val));

		RecvRes = ReceiveString(&AcceptedStr, server_socket);

		if (RecvRes == TRNS_FAILED) {
			printf("Socket error while trying to write data to socket\n");
			return ERR_CODE_SOCKET;
		}
		else if (RecvRes == TRNS_DISCONNECTED) {
			if (try_to_connect(CONNECTION_BREAK_MENU_BEF, CONNECTION_BREAK_MENU_AFT) == SUCCESS_CODE) {
				continue;
			}
			else {
				return ERR_CODE_CONNECTION;
			}
		}
		else if (RecvRes == "TRNS_TIMEOUT") {
			if (try_to_connect(CONNECTION_BREAK_MENU_BEF, CONNECTION_BREAK_MENU_AFT) == SUCCESS_CODE) {
				continue;
			}
			else {
				return ERR_CODE_TIMEOUT;
			}
		}
		else {
			parse_recv_string(*AcceptedStr, message_type, *params);
			printf("%s\n", AcceptedStr);
			game_logic_in_recive_thread(message_type, *params, &message_between_threads);
		}

		free(AcceptedStr);
	}

	close_handle(&message_between_threads_mutex_handle);
	return SUCCESS_CODE;
ERR_WITH_MUTEX:
	close_handle(&message_between_threads_mutex_handle);
	return ERR_CODE_MUTEX;
}

//Sending data to the server
static DWORD SendDataThread(void)
{
	char SendStr[MAX_SENDSTR_FOR_CLIENT];
	int quit = 0;
	TransferResult_t SendRes;
	// open mutex
	HANDLE message_between_threads_mutex_handle = NULL;
	if (open_and_check_mutex(&message_between_threads_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_MESSAGE_BETWEEN_THREADS_NAME, NULL) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}

	while (1)
	{
		gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard
		strupr(SendStr);

		//lock mutex
		if (lock_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}
		//critical zone
		if (message_between_threads == USER_ASKED_TO_QUIT) {
			quit = 1;
		}
		//free mutex
		if (release_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}

		if (quit == 1) {
			break;
		}
		
		//lock mutex
		if (lock_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}
		//critical zone
		game_logic_in_send_thread(SendStr, &message_between_threads, &quit);
		//free mutex
		if (release_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}

		SendRes = SendString(SendStr, server_socket);

		if (quit == 1) {
			break;
		}

		if (SendRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			close_handle(&message_between_threads_mutex_handle);
			return ERR_CODE_SOCKET;
		}
	}
	//close everything -send and recive threads 
	close_handle(&message_between_threads_mutex_handle);
	return SUCCESS_CODE;
ERR_WITH_MUTEX:
	close_handle(&message_between_threads_mutex_handle);
	return ERR_CODE_MUTEX;
}

int connecting_to_server(char *server_ip, int server_port, void *p_server_socket, char *username) {

	SOCKADDR_IN clientService;
	HANDLE hThread[2];
	WSADATA wsaData;
	char user_input[MAX_USER_LEN_INPUT];
	user_input[0] = '1';

	strcpy_s(global_server_ip, MAX_IP_STRING_LEN, server_ip);
	global_server_port = server_port;

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
	clientService.sin_addr.s_addr = inet_addr(server_ip); //Setting the IP address to connect to by the format used by TCP
	clientService.sin_port = htons(server_port); //Setting the port to connect to.

	// Call the connect function, passing the created socket and the sockaddr_in structure as parameters. 
	// Check for general errors.
	if (try_to_connect(FAILED_CONNECTION_MENU_BEF, FAILED_CONNECTION_MENU_AFT) == ERR_CODE_CONNECTION) {
		WSACleanup();
		return ERR_CODE_SOCKET;
	}
	char *sendstr[MAX_MESSAGE_TYPE_LENGTH + MAX_USER_NAME_INPUT];
	sendstr[0] = '\0';
	sprintf_s(sendstr, MAX_MESSAGE_TYPE_LENGTH + MAX_USER_NAME_INPUT, "CLIENT_REQUEST:%s\n", username);
	SendString(sendstr, server_socket);
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
		if (return_code != NULL) {
			*return_code = ERR_CODE_NOT_ENOUGH_ARGUMENTS;
		}
	}
	return ERR_CODE_NOT_ENOUGH_ARGUMENTS;
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
		if (return_code != NULL) {
			*return_code = ERR_CODE_MUTEX;
		}
	}
	return ERR_CODE_MUTEX;
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
		if (return_code != NULL) {
			*return_code = ERR_CODE_MUTEX;
		}
	}
	return ERR_CODE_MUTEX;
}

int lock_mutex(HANDLE *mutex_handle, int *return_code) {
	DWORD wait_code;
	wait_code = WaitForSingleObject(*mutex_handle, INFINITE);
	if (wait_code != WAIT_OBJECT_0) {
		printf("Error when locking mutex\n");
		if (return_code != NULL) {
			*return_code = ERR_CODE_MUTEX;
		}
		return ERR_CODE_MUTEX;
	}
	return SUCCESS_CODE;
}

int release_mutex(HANDLE *mutex_handle, int *return_code) {
	BOOL ret_val;
	ret_val = ReleaseMutex(*mutex_handle);
	if (ret_val == FALSE) {
		printf("Error when releasing mutex: %d\n", GetLastError());
		if (return_code != NULL) {
			*return_code = ERR_CODE_MUTEX;
		}
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