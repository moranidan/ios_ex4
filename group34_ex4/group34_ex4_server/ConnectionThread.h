// ConnectionThread.h

// This header consists the function declerations for all the functions used in ListenThread.c, and their documentaion

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef CONNECTION_THREAD_H
#define CONNECTION_THREAD_H

// Includes --------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "HardCodedData.h"
#include <time.h>
#include <stdlib.h>
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include <WS2tcpip.h>
#include <windows.h>
#include "Shlwapi.h"

// Function Declarations -------------------------------------------------------

/*
This function is the main function of ServiceThread - aka ConnectionThread
Service thread is the thread that opens for each successful client connection and "talks" to the client.
Input:  LPVOID lpParam - pointer to parameters for the new thread, only one is permitted so we send struct
Output: DWORD, success or error code
*/
DWORD ServiceThread(LPVOID lpParam);

/*
This function receives string from the client, and checks if the receive succeded.
Input:  char **AcceptedStr - string to receive (pointer to pointer)
		SOCKET socket - connection socket
Output: int, return code
*/
int ReceiveAndCheck(char **AcceptedStr, SOCKET socket);

/*
This function sends string to the client, and checks if the send succeded.
Input:  char *to_send - string to send
		SOCKET socket - connection socket
Output: int, return code
*/
int SendAndCheck(char *to_send, SOCKET socket);

/*
This function approves new connection.
Input:  CONNECTION_THREAD_params_t *p_params - pointer to the Connection Thread parameters struct
		char *username - user name, to be filled
		SOCKET t_socket - connection socket
Output: int, return code
*/
int approve_client(CONNECTION_THREAD_params_t *p_params, char *username, SOCKET t_socket);

/*
This function sends the main menu and receives a choich.
Input:  int *main_menu_decision - client decision, to be filles based on what received
		SOCKET t_socket - connection socket
Output: int, return code
*/
int main_menu(int *main_menu_decision, SOCKET t_socket);

/*
This function implements a one-player game against the server.
Input:  BOOL *replay - pointer to the replay variable, if the client wants to play again
		char *username - user name, to be filled
		SOCKET t_socket - connection socket
Output: int, return code (via check_if_replay)
*/
int cpu_game(BOOL *replay, char *username, SOCKET t_socket);

/*
This function asks the client for a move, and receives the answer.
Input:  int *player_choice - clients choice, to be updated
		SOCKET t_socket - connection socket
Output: int, return code
*/
int ask_and_receive_move(int *player_choice, SOCKET t_socket);

/*
This function finds the winner of the game, and sends the results to the client.
Input:  int *player1_choice - first player choice
		int *player2_choice - second player choice
		char *player1_username - username of player1
		char *player2_username - username of player2
		int you - which player are you, 1 or 2
		SOCKET t_socket - connection socket
Output: int, return code
*/
int find_and_send_winner(int *player1_choice, int *player2_choice, char *player1_username, char *player2_username, int you, SOCKET t_socket);

/*
This function checks if player 1 is the winner.
Input:  int player_choice - player1 choice
		int cpu_choice - player2 choice
Output: BOOL, true if player1 wins, false ele
*/
BOOL player_win(int player_choice, int cpu_choice);

/*
This function converts a move from text to the equivalent number. 
Input:  char *move_name - move name
Output: int, equivalent to move
*/
int name_to_number(char *move_name);

/*
This function converts a move from number to the equivalent text.
Input:  int num - players choice in numeric representaion
		char *move_name - move name
Output: void
*/
void number_to_name(int num, char *move_name);

/*
This function checks if the client wants to play again.
Input:  BOOL *replay - pointer to the replay variable, if the client wants to play again
		SOCKET t_socket - connection socket
Output: int, return code
*/
int check_if_replay(BOOL *replay, SOCKET t_socket);

/*
This function implements multiplayer game.
Input:  BOOL *replay - pointer to the replay variable, if the client wants to play again
		char *username - user name, to be filled
		SOCKET t_socket - connection socket
Output: int, return code
*/
int versus_game(BOOL *replay, char *username, SOCKET t_socket);

/*
This function manages player1 in multiplayer games.
Input:  FILE *fd - pointer to gamesession.txt file
		HANDLE file_mutex_handle - handle of the file protection mutex
		HANDLE player1_event - handle of player1 event
		HANDLE player2_event - handle of player2 event
		char *username - user name, to be filled
		SOCKET t_socket - connection socket
Output: int, return code
*/
int player1_game(FILE *fd, HANDLE file_mutex_handle, HANDLE player1_event, HANDLE player2_event, char *username, SOCKET t_socket);

/*
This function manages player2 in multiplayer games.
Input:  FILE *fd - pointer to gamesession.txt file
		HANDLE file_mutex_handle - handle of the file protection mutex
		HANDLE player1_event - handle of player1 event
		HANDLE player2_event - handle of player2 event
		char *username - user name, to be filled
		SOCKET t_socket - connection socket
Output: int, return code
*/
int player2_game(FILE *fd, HANDLE file_mutex_handle, HANDLE player1_event, HANDLE player2_event, char *username, SOCKET t_socket);

/*
This function checks if gamesession.txt exists. if not, open it.
Input:  FILE *fd - pointer to gamesession.txt file
Output: BOOL, true if the file exists, false otherwise
*/
BOOL file_exists(FILE *fd);

/*
This function deletes gamesession.txt.
Input:  No input
Output: int, return code
*/
int delete_file();

/*
This function writes a line to gamesession.txt file.
Input:  FILE *fd - pointer to gamesession.txt file
		char *line - line to be written
Output: int, return code
*/
int write_line(FILE *fd, char *line);

/*
This function gets the last line of gamesession.txt file.
Input:  FILE *fp - pointer to gamesession.txt file
Output: char*, pointer to line read
*/
char* GetFileLastRow(FILE *fp);

/*
This function creates an event, with non-signales initial state.
Input:  HANDLE *event_handle - pointer to the event handle
		char *event_name - events name
Output: int, return code
*/
int create_and_check_event(HANDLE *event_handle, char *event_name);

/*
This function signal an event, and check if it was succesful.
Input:  HANDLE *event_handle - pointer to the event handle
Output: int, return code
*/
int set_and_check_event(HANDLE *event_handle);







#endif // CONNECTION_THREAD_H