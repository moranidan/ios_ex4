// SharedHardCodedData.h

// This header consists of the constat numbers and strings that nutals to both server and client

#pragma once

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

// Constants -------------------------------------------------------------------


static const int SUCCESS_CODE = 0;
static const int ERR_CODE_NOT_ENOUGH_ARGUMENTS = -5;
static const int ERR_CODE_ALLOCCING_MEMORY = -6;
static const int ERR_CODE_SOCKET = -1;
static const int ERR_CODE_TIMEOUT = -8;
static const int ERR_CODE_THREAD = -2;
static const int ERR_CODE_CREAT_THEARD = -3;
static const int ERR_CODE_CLOSE_THREAD = -4;
static const int ERR_CODE_MUTEX = -7;
static const int ERR_CODE_CONNECTION = -10;
static const int ERR_CODE_WSACLEANUP = 11;
static const int ERR_GET_EXITCODE = -12;

//function Declaration ----------------------------------------------------------------------------------

/*
This function create the string that wiil be send between client and server by using the targil rules
Input:  char *send_str - a string that will contains the string that will be send' there have to be a malloc on it befor calling this func.
		char *message_type - a string cntains the message type you want to send.
		char *params[] - a list [4] of the parameters if needed, need to be inizilaized to NULL.
		int *max_send_len - nedd to send to the fun the max len of message_type + params.
Output: void
*/
void create_string_to_send(char *send_str, char *message_type, char *params[], int *max_send_len);

/*
This function parse the string that we get from the client or the server by using the targil rules
Input:  char *recv_string - a string that contains the string we get from the client or server.
		char *message_type - a string cntains the message type you want to send.
		char *params[] - a list [4] of the parameters if needed, need to be inizilaized to NULL.
Output: void
*/
void parse_recv_string(char *recv_string, char *message_type, char *params[]);

/*
This function checks if enough arguments were given.
Input:  int argc - number of arguments
		int *return_code - pointer to the return code, to be updated in case of an error
Output: int, success code of given enough arguments, error code else
*/
int check_arguments(int argc, int *return_code);

/*
This function creates mutex by name, and checks if it opened correctly.
Input:  HANDLE *mutex_handle - pointer to handle who will hold the mutex
		LPSECURITY_ATTRIBUTES lpMutexAttributes - access mode for the mutex
		BOOL bInitialOwner - initial state of the mutex, locked or released
		LPCTSTR lpName - mutex unique name
		int *return_code - pointer to the return code, to be updated in case of an error
Output: int, success code if mutex created successfully, error code else
*/
int create_and_check_mutex(
	HANDLE *mutex_handle,
	LPSECURITY_ATTRIBUTES lpMutexAttributes,
	BOOL bInitialOwner,
	LPCTSTR lpName,
	int *return_code);

/*
This function opens mutex by name, and checks if it opened correctly.
Input:  HANDLE *mutex_handle - pointer to handle who will hold the mutex
		LPSECURITY_ATTRIBUTES lpMutexAttributes - access mode for the mutex
		BOOL bInitialOwner - initial state of the mutex, locked or released
		LPCTSTR lpName - mutex unique name
		int *return_code - pointer to the return code, to be updated in case of an error
Output: int, success code if mutex created successfully, error code else
*/
int open_and_check_mutex(
	HANDLE *mutex_handle,
	LPSECURITY_ATTRIBUTES lpMutexAttributes,
	BOOL bInitialOwner,
	LPCTSTR lpName,
	int *return_code
);

/*
This function locks a given mutex.
Input:  HANDLE *mutex_handle - pointer to handle of the mutex to be locked
		int *return_code - pointer to the return code, to be updated in case of an error
Output: int, success code if mutex locked successfully, error code else
*/
int lock_mutex(HANDLE *mutex_handle, int *return_code);

/*
This function releases a given mutex.
Input:  HANDLE *mutex_handle - pointer to handle of the mutex to be released
		int *return_code - pointer to the return code, to be updated in case of an error
Output: int, success code if mutex released successfully, error code else
*/
int release_mutex(HANDLE *mutex_handle, int *return_code);

/*
This function closes given handle, in case of an error prints it.
Input:  HANDLE *handle - pointer to the handle to be closed
Output: void
*/
void close_handle(HANDLE *handle);
