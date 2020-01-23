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
char user_name[MAX_USER_NAME_INPUT] = "\0";

// Function Definitions --------------------------------------------------------

int try_to_connect(char *menu_type_bef, char *menu_type_aft) {
	char user_input[MAX_USER_LEN_INPUT];
	user_input[0] = '1';

	while (*user_input == '1') {
		char err = connect(server_socket, (SOCKADDR*)&clientService, sizeof(clientService));
		if (err == SOCKET_ERROR) {               //connect to server
			printf("%s %s:%d%s.\nChoose what to do next:\n1. Try to reconnect\n2. Exit\n",menu_type_bef, global_server_ip, global_server_port, menu_type_aft);
			gets_s(user_input, sizeof(user_input)); //Reading a string from the keyboard
		}
		else {
			goto CONNECTION_SUCCESS;
		}
	}
	if (*user_input == '2') {
		return ERR_CODE_CONNECTION;
	}
CONNECTION_SUCCESS:
	return SUCCESS_CODE;
}

int try_to_connect_again(char *menu_type_bef, char *menu_type_aft) {
	
	TransferResult_t SendRes;
	int return_code = SUCCESS_CODE;
	// open mutex
	HANDLE message_between_threads_mutex_handle = NULL;
	if (open_and_check_mutex(&message_between_threads_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_MESSAGE_BETWEEN_THREADS_NAME, NULL) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}

	//lock mutex
	if (lock_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
		return_code = ERR_CODE_MUTEX;
		goto CLOSE_AND_BREAK1;
	}
	//critical zone
	strcpy_s(message_between_threads, MAX_MESSAGE_TYPE_LENGTH, "TRY_TO_CONNECT");
	//free mutex
	if (release_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
		return_code = ERR_CODE_MUTEX;
		goto CLOSE_AND_BREAK1;
	}

	printf("%s %s:%d%s.\nChoose what to do next:\n1. Try to reconnect\n2. Exit\n", menu_type_bef, global_server_ip, global_server_port, menu_type_aft);

	Sleep(4000);

	//lock mutex
	if (lock_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
		return_code = ERR_CODE_MUTEX;
		goto CLOSE_AND_BREAK1;
	}
	//critical zone
	if (strcmp(message_between_threads, "1") == 0) {
		char err = connect(server_socket, (SOCKADDR*)&clientService, sizeof(clientService));
		if (err != SOCKET_ERROR) {               //connect to server
			char sendstr[MAX_MESSAGE_TYPE_LENGTH + MAX_USER_NAME_INPUT] = "";
			sprintf_s(sendstr, MAX_MESSAGE_TYPE_LENGTH + MAX_USER_NAME_INPUT, "CLIENT_REQUEST:%s\n", user_name);
			SendRes = SendString(sendstr, server_socket);
			if (SendRes == TRNS_FAILED)
			{
				printf("Socket error while trying to write data to socket\n");
				return_code = ERR_CODE_SOCKET;
				goto CLOSE_AND_BREAK;
			}
		}
		else {
			return_code = ERR_CODE_CONNECTION;
		}
	}
	else if (strcmp(message_between_threads, "2") == 0) {
		strcpy_s(message_between_threads, MAX_MESSAGE_TYPE_LENGTH, USER_ASKED_TO_QUIT);
	}
CLOSE_AND_BREAK:
	//free mutex
	if (release_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
		return_code = ERR_CODE_MUTEX;
		goto CLOSE_AND_BREAK1;
	}

CLOSE_AND_BREAK1:
	close_handle(&message_between_threads_mutex_handle);
	return return_code;
}

//Reading data coming from the server
static DWORD RecvDataThread(void)
{
	TransferResult_t RecvRes;
	// open mutex
	HANDLE message_between_threads_mutex_handle = NULL;
	if (open_and_check_mutex(&message_between_threads_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_MESSAGE_BETWEEN_THREADS_NAME, NULL) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}
	char message_type[MAX_MESSAGE_TYPE_LENGTH] = "";
	while (1)
	{
		strcpy_s(message_type, MAX_MESSAGE_TYPE_LENGTH, "");
		char *params[] = { "","","","" };
		char *AcceptedStr = NULL;
		RecvRes = ReceiveString(&AcceptedStr, server_socket);
		if (RecvRes == TRNS_FAILED) {
			printf("Socket error while trying to write data to socket\n");
			close_handle(&message_between_threads_mutex_handle);
			free(AcceptedStr);
			return ERR_CODE_SOCKET;
		}
		else if (RecvRes == TRNS_DISCONNECTED) {
			int err = SUCCESS_CODE;
			err = try_to_connect_again(SERVER_DENIED_MENU_BEF, SERVER_DENIED_MENU_AFT);
			while (err == ERR_CODE_CONNECTION) {
				err = try_to_connect_again(SERVER_DENIED_MENU_BEF, SERVER_DENIED_MENU_AFT);
			}
			if (err == SUCCESS_CODE) {
				free(AcceptedStr);
				continue;
			}
			else {
				close_handle(&message_between_threads_mutex_handle);
				free(AcceptedStr);
				return ERR_CODE_CONNECTION;
			}
		}
		else if (RecvRes == TRNS_TIMEOUT) {
			int err = SUCCESS_CODE;
			err = try_to_connect_again(CONNECTION_BREAK_MENU_BEF, CONNECTION_BREAK_MENU_AFT);
			while (err == ERR_CODE_CONNECTION) {
				err = try_to_connect_again(SERVER_DENIED_MENU_BEF, SERVER_DENIED_MENU_AFT);
			}
			if (err == SUCCESS_CODE) {
				free(AcceptedStr);
				continue;
			}
			else {
				close_handle(&message_between_threads_mutex_handle);
				free(AcceptedStr);
				return ERR_CODE_TIMEOUT;
			}
		}
		else {
			parse_recv_string(AcceptedStr, message_type, &params);
			printf("%s\n", AcceptedStr);
			game_logic_in_recive_thread(message_type, &params, &message_between_threads);
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
	char *SendStr[MAX_SENDSTR_FOR_CLIENT];
	SendStr[0] = '\0';
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
		if (strcmp(message_between_threads, USER_ASKED_TO_QUIT) == 0) {
			quit = 1;
		}
		//free mutex
		if (release_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
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
		if (SendRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			close_handle(&message_between_threads_mutex_handle);
			return ERR_CODE_SOCKET;
		}

		if (quit == 1) {
			break;
		}
	}
	//close everything - send and recive threads 
	close_handle(&message_between_threads_mutex_handle);
	return SUCCESS_CODE;
ERR_WITH_MUTEX:
	close_handle(&message_between_threads_mutex_handle);
	return ERR_CODE_MUTEX;
}

int connecting_to_server(char *server_ip, int server_port, void *p_server_socket, char *username) {

	strcpy_s(user_name, MAX_USER_NAME_INPUT, username);
	HANDLE hThread[2] = { NULL, NULL };
	WSADATA wsaData;
	TransferResult_t SendRes;
	char user_input[MAX_USER_LEN_INPUT];
	user_input[0] = '1';
	int return_code = SUCCESS_CODE;

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
		if (WSACleanup() == SOCKET_ERROR) {
			printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());
			return ERR_CODE_WSACLEANUP;
		}
		return ERR_CODE_SOCKET;
	}
	//Create a sockaddr_in object clientService and set values.
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(server_ip); //Setting the IP address to connect to by the format used by TCP
	clientService.sin_port = htons(server_port); //Setting the port to connect to.

	// Call the connect function, passing the created socket and the sockaddr_in structure as parameters. 
	// Check for general errors.
	if (try_to_connect(FAILED_CONNECTION_MENU_BEF, FAILED_CONNECTION_MENU_AFT) == ERR_CODE_CONNECTION) {
		if (WSACleanup() == SOCKET_ERROR) {
			printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());
			return ERR_CODE_WSACLEANUP;
		}
		return ERR_CODE_SOCKET;
	}
	char sendstr[MAX_MESSAGE_TYPE_LENGTH + MAX_USER_NAME_INPUT] = "";
	sprintf_s(sendstr, MAX_MESSAGE_TYPE_LENGTH + MAX_USER_NAME_INPUT, "CLIENT_REQUEST:%s\n", username);
	SendRes = SendString(sendstr, server_socket);
	if (SendRes == TRNS_FAILED)
	{
		printf("Socket error while trying to write data to socket\n");
		return ERR_CODE_SOCKET;
	}
	// open mutex
	HANDLE message_between_threads_mutex_handle = NULL;
	if (open_and_check_mutex(&message_between_threads_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_MESSAGE_BETWEEN_THREADS_NAME, NULL) != SUCCESS_CODE) {
		return_code = ERR_CODE_MUTEX;
		goto ERR_WITH_MUTEX1;
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
	int quit = 0;
	BOOL ret_val;
	DWORD exit_code;
	LPDWORD lpExitCodeSend;
	LPDWORD lpExitCodeRecv;

	while (quit == 0) {
		//lock mutex
		if (lock_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			return_code = ERR_CODE_MUTEX;
			goto ERR_WITH_MUTEX2;
		}
		//critical zone
		if (strcmp(message_between_threads, USER_ASKED_TO_QUIT) == 0) {
			quit = 1;
		}
		//free mutex
		if (release_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			return_code = ERR_CODE_MUTEX;
			goto ERR_WITH_MUTEX2;
		}
	}
	ret_val = TerminateThread(hThread[1], &exit_code);
	if (FALSE == ret_val)
	{
		printf("Error when terminating thread: %d\n", GetLastError());
	}
	if (GetExitCodeThread(hThread[0], &lpExitCodeSend) == 0) {
		return_code = ERR_GET_EXITCODE;
	}
	if (lpExitCodeSend == STILL_ACTIVE) {
		ret_val = TerminateThread(hThread[0], &exit_code);
		if (FALSE == ret_val)
		{
			printf("Error when terminating thread: %d\n", GetLastError());
		}
	}
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	if (GetExitCodeThread(hThread[0], &lpExitCodeSend) == 0) {
		return_code = ERR_GET_EXITCODE;
	}
	if (GetExitCodeThread(hThread[1], &lpExitCodeRecv) == 0) {
		return_code = ERR_GET_EXITCODE;
	}
	if (lpExitCodeSend == SUCCESS_CODE) {
		return_code = lpExitCodeRecv;
	}
	else {
		return_code = lpExitCodeSend;
	}
ERR_WITH_MUTEX2:
	close_handle(&hThread[0]);
	close_handle(&hThread[1]);
ERR_WITH_MUTEX1:
	if (closesocket(server_socket) == SOCKET_ERROR)
	{
		printf("recv() failed, error %d\n", WSAGetLastError());
		return_code = ERR_CODE_SOCKET;
	}
	if (WSACleanup() == SOCKET_ERROR) {
		printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());
		return_code = ERR_CODE_WSACLEANUP;
	}
	return return_code;
}


// utility functions --------------------------------------------------------------

int check_arguments(int argc, int *return_code) {
	if (argc < 4) {    //check if there are enough arguments
		printf("not enough arguments in group34_ex4_client.exe ");
		if (return_code != NULL) {
			*return_code = ERR_CODE_NOT_ENOUGH_ARGUMENTS;
		}
		return ERR_CODE_NOT_ENOUGH_ARGUMENTS;
	}
	return SUCCESS_CODE;
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
		return ERR_CODE_MUTEX;
	}
	return SUCCESS_CODE;
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
		return ERR_CODE_MUTEX;
	}
	return SUCCESS_CODE;
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