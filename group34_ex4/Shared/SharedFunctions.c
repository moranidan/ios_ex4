// SharedFunctions.c

// Description - This module provides the shared function that both server and client need to use.

// Includes --------------------------------------------------------------------


#include "SocketSendRecvTools.h"
#include "../Shared/SharedHardCodedDataAndFunctions.h"
#include <stdio.h>
#include <string.h>

#pragma warning(disable:4996) // in order to avoid warning about fopen being unsafe function.

// protocol functions  --------------------------------------------------------------------

void create_string_to_send(char *send_str, char *message_type, char *params[], int *max_send_len) {
	strcpy_s(send_str, *max_send_len, message_type);
	if (params == NULL) {
		sprintf_s(send_str + strlen(send_str), *max_send_len, "\n");
		return;
	}
	else if (params[1] == NULL) {
		sprintf_s(send_str + strlen(send_str), *max_send_len, ":%s\n", params[0]);
		return;
	}
	else if (params[2] == NULL) {
		sprintf_s(send_str + strlen(send_str), *max_send_len, ":%s;%s\n", params[0], params[1]);
		return;
	}
	else if (params[3] == NULL) {
		sprintf_s(send_str + strlen(send_str), *max_send_len, ":%s;%s;%s\n", params[0], params[1], params[2]);
		return;
	}
	sprintf_s(send_str + strlen(send_str), *max_send_len, ":%s;%s;%s;%s\n", params[0], params[1], params[2], params[3]);
}


void parse_recv_string(char *recv_string, char *message_type, char *params[]) {
	if (strchr(recv_string, ':') == NULL) {
		char *inner_msg_type = strtok(recv_string, "\n");
		strcpy_s(message_type, 30, inner_msg_type);
		return;
	}
	char *inner_msg_type = strtok(recv_string, ":");
	strcpy_s(message_type, 30, inner_msg_type);
	char *param_in_str;
	param_in_str = strtok(NULL, "\n");
	params[0] = strtok(param_in_str, ";");
	for (int i = 1; i < 4; i++) {
		params[i] = strtok(NULL, ";");
	}
	return;
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
