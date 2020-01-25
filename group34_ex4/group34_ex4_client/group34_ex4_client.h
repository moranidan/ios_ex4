// group34_ex4_client.h

// This header consists the structs and function declerations for all the functions used in the project, and their documentation.

#pragma once

#include "HardCodedData.h"
#include <windows.h>
#include <math.h>
#include <stdio.h>

//function Declaration ----------------------------------------------------------------------------------

/*
This function tries to connect to the server as long as the user asked her too.
Input:  char *menu_type_bef - the string(before the ip and port number) that explained why the connection failed
		char *menu_type_aft - the string(before the ip and port number) that explained why the connection failed
Output: int, success code if the connection sucess, error code else
*/
int try_to_connect(char *menu_type_bef, char *menu_type_aft);

/*
This function tries to connect to the server as long as the user asked her too after the send and recv threads opened
Input:  char *menu_type_bef - the string(before the ip and port number) that explained why the connection failed
		char *menu_type_aft - the string(before the ip and port number) that explained why the connection failed
Output: int, success code if the connection sucess, error code else
*/
int try_to_connect_again(char *menu_type_bef, char *menu_type_aft);

/*
This function connect to the server and open the recive and send threads.
Input:  char *server_ip - the string contains the ip addres
		int server_port - the number of the port
		void *p_server_socket - a pointer to the server socket in the main module
		char *username - a string contains the username that the user entered
Output: int, success code if the connection sucess, error code else
*/
int connecting_to_server(char *server_ip, int server_port, void *p_server_socket, char *username);

/*
This function print the leaderboard according to the instractions.
Input:  char *file_text - a string contains all the data inside the leaderboard file in csv format
Output: void
*/
void parse_and_print_leaderboard(char *file_text);

/*
This function does the game logic in the recive thread.
Input:  char *message_type - a string contains the type of message we get from the server
		char *params[] - the list of params we get from the server
		char *message_between_threads - a string that use for connection between the threads (its protected by a mutex)
Output: int, success code if mutex locked successfully, error code else
*/
int game_logic_in_recive_thread(char *message_type, char *params[], char *message_between_threads);

/*
This function does the game logic in the recive thread.
Input:  char *SendStr - a string contains the string we want to send to the server
		char *message_between_threads - a string that use for connection between the threads (its protected by a mutex)
		int *quit - a varibel that if equal 1 said that the user want to quit.
Output: void
*/
void game_logic_in_send_thread(char *SendStr, char *message_between_threads, int *quit);

