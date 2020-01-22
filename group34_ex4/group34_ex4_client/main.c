// main.c

/*
Authors –
	Moran Idan - 315239079
	Ofer Bear - 207943366
Project - TestGrade
Description - This project create the client program for the game
*/

// Includes --------------------------------------------------------------------

#include "HardCodedData.h"
#include "group34_ex4_client.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#pragma warning(disable:4996) // in order to avoid warning about fopen being unsafe function.

/*
In this entire project, we check for errors in each function that might fail.
If there's a failure, we update the return code and use the 'goto' function to the end of main.c.
*/


// Function Definitions --------------------------------------------------------

int main(int argc, char *argv[]) {
	int return_code = SUCCESS_CODE;
	if (check_arguments(argc, &return_code) != SUCCESS_CODE) {
		return return_code;
	}

	char *server_ip[MAX_IP_STRING_LEN];		// server ip string
	server_ip[0] = '\0';
	int server_port = 0;        //server port int
	char *username[MAX_USER_NAME_INPUT];
	SOCKET *p_server_socket;

	//copy server ip string
	strcpy_s(server_ip, MAX_IP_STRING_LEN, argv[1]);

	//copy server port 
	server_port = atoi(argv[2]);

	//copy username string
	strcpy_s(username, MAX_USER_NAME_INPUT, argv[3]);
	strupr(username);

	// create exit_residents mutex
	HANDLE message_between_threads_mutex_handle = NULL;
	if (create_and_check_mutex(&message_between_threads_mutex_handle, NULL, FALSE, MUTEX_MESSAGE_BETWEEN_THREADS_NAME, &return_code) != SUCCESS_CODE) {
		goto SKIP;
	}

	return_code = connecting_to_server(server_ip, server_port, &p_server_socket,username);

	// close mutex handle and socket
	close_handle(&message_between_threads_mutex_handle);
SKIP:
	return return_code;
}