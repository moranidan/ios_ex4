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
description
*/


// Function Definitions --------------------------------------------------------

int main(int argc, char *argv[]) {
	int return_code = SUCCESS_CODE;
	if (check_arguments(argc, &return_code) != SUCCESS_CODE) {
		return return_code;
	}

	unsigned long server_ip;		// server ip string
	int server_port = 0;        //server port int
	char *username[MAX_USER_NAME_INPUT];
	SOCKET *p_server_socket;

	// copy server ip string to the format used by TCP
	server_ip = inet_addr(argv[1]);

	//copy server port 
	server_port = htons(argv[2]);

	//copy username string
	strcpy_s(username, MAX_USER_NAME_INPUT, argv[3]);
	strupr(username);

	return_code = connecting_to_server(server_ip, server_port, &p_server_socket);


	closesocket(*p_server_socket);
	return return_code;
}