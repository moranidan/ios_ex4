// game_logic.c

// Description - This module provides the game logic that shows in the instractions and happen in the recive thread.

// Includes --------------------------------------------------------------------

#include "HardCodedData.h"
#include "group34_ex4_client.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#pragma warning(disable:4996) // in order to avoid warning about fopen being unsafe function.


// Function Definitions --------------------------------------------------------

void parse_and_print_leaderboard(char *file_text) {
	char delim[2] = ",";
	char *word = strtok(file_text, delim);
	while (word != NULL) {
		printf(word);
		printf("\t\t");
		word = strtok(NULL, delim);
	}
}

int game_logic_in_recive_thread(char *message_type, char *params[], char *message_between_threads) {
	char *user_input[MAX_USER_LEN_INPUT];
	user_input[0] = '\0';

	// open mutex
	HANDLE message_between_threads_mutex_handle = NULL;
	if (open_and_check_mutex(&message_between_threads_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_MESSAGE_BETWEEN_THREADS_NAME, NULL) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}

	if (strcmp(message_type, "SERVER_DENIED") == 0) {  // need to move
		try_to_connect(SERVER_DENIED_MENU_BEF, SERVER_DENIED_MENU_AFT);
	}
	if (strcmp(message_type, "SERVER_MAIN_MENU") == 0) {

		//lock mutex
		if (lock_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}
		//critical zone
		strcpy_s(message_between_threads, MAX_MESSAGE_TYPE_LENGTH, "SERVER_MAIN_MENU");
		//free mutex
		if (release_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}

		printf("Choose what to do next:\n1. Play against another client\n2. Play against the server\n3. View the leaderbord\n4. Quit\n");
	}
	if (strcmp(message_type, "SERVER_INVITE") == 0 ) {
	}
	if (strcmp(message_type, "SERVER_LEADERBOARD") == 0) {
		parse_and_print_leaderboard(params[0]);

		//lock mutex
		if (lock_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}
		//critical zone
		strcpy_s(message_between_threads, MAX_MESSAGE_TYPE_LENGTH, "SERVER_LEADERBOARD");
		//free mutex
		if (release_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}

		printf("Choose what to do next:\n1. Refresh leaderboard\n2. Return to the main menu\n");
	}
	if (strcmp(message_type, "SERVER_PLAYER_MOVE_REQUEST") == 0) {
		//lock mutex
		if (lock_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}
		//critical zone
		strcpy_s(message_between_threads, MAX_MESSAGE_TYPE_LENGTH, "SERVER_PLAYER_MOVE_REQUEST");
		//free mutex
		if (release_mutex(&message_between_threads_mutex_handle, NULL) != SUCCESS_CODE) {
			goto ERR_WITH_MUTEX;
		}

		printf("Choose a move from the list: Rock, Paper, Scissors, Lizard or Spock:\n");
	}
	if (strcmp(message_type, "SERVER_GAME_RESULTS") == 0) {
		printf("You played:%s\n%s played:%s\n", params[2], params[0], params[1]);
		if (params[3] != NULL) {
			printf("%s won!\n", params[3]);
		}
	}
	if (strcmp(message_type, "SERVER_GAME_OVER_MENU") == 0) {
		printf("Choose what to do next:\n1. Play again\n2. Return to the main menu\n");
	}
	if (strcmp(message_type, "SERVER_OPPONENT_QUIT") == 0) {
		printf("%s has left the game!\n", params[0]);
	}

	close_handle(&message_between_threads_mutex_handle);
	return SUCCESS_CODE;
ERR_WITH_MUTEX:
	close_handle(&message_between_threads_mutex_handle);
	return ERR_CODE_MUTEX;
}


void game_logic_in_send_thread(char *SendStr, char *message_between_threads, int *quit) {
	int send_str_len = MAX_SENDSTR_FOR_CLIENT;
	if (strcmp(message_between_threads, "SERVER_MAIN_MENU") == 0) {
		char *params[] = { NULL, NULL, NULL, NULL };
		strcpy_s(message_between_threads, MAX_MESSAGE_TYPE_LENGTH, MESSAGE_RECIVED_BETWEEN_THREADS);
		if (*SendStr == '1') {
			create_string_to_send(SendStr, "CLIENT_VERSUS", &params, &send_str_len);
			strcpy_s(message_between_threads, MAX_MESSAGE_TYPE_LENGTH, SENT_CLIENT_VERSUS);
		}
		if (*SendStr == '2') {
			create_string_to_send(SendStr, "CLIENT_CPU", &params, &send_str_len);
		}
		if (*SendStr == '3') {
			create_string_to_send(SendStr, "CLIENT_LEADERBOARD", &params, &send_str_len);
		}
		if (*SendStr == '4') {
			create_string_to_send(SendStr, "CLIENT_DISCONNECT", &params, &send_str_len);
			strcpy_s(message_between_threads, MAX_MESSAGE_TYPE_LENGTH, USER_ASKED_TO_QUIT);
			*quit = 1;
		}
	}
	else if (strcmp(message_between_threads, "SERVER_LEADERBOARD") == 0) {
		char *params[] = { NULL, NULL, NULL, NULL };
		strcpy_s(message_between_threads, MAX_MESSAGE_TYPE_LENGTH, MESSAGE_RECIVED_BETWEEN_THREADS);
		if (*SendStr == '1') {
			create_string_to_send(SendStr, "CLIENT_REFRESH", &params, &send_str_len);
			//wait to server leaderboard message
		}
		if (*SendStr == '2') {
			create_string_to_send(SendStr, "CLIENT_MAIN_MENU", &params, &send_str_len);
			//wait to server main menu message
		}
	}
	else if (strcmp(message_between_threads, "SERVER_PLAYER_MOVE_REQUEST") == 0) {
		char *move[MAX_USER_LEN_INPUT];
		move[0] = '\0';
		strcpy_s(move, MAX_SENDSTR_FOR_CLIENT, SendStr);
		char *params[4] = { move, NULL, NULL, NULL };
		strcpy_s(message_between_threads, MAX_MESSAGE_TYPE_LENGTH, MESSAGE_RECIVED_BETWEEN_THREADS);
		create_string_to_send(SendStr, "CLIENT_PLAYER_MOVE", &params, &send_str_len);
	}
}