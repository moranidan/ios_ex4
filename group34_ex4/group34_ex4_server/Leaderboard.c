// Leaderboard.c

// This module provides the functions of the leaderboard feature.

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// Includes --------------------------------------------------------------------

#include "Leaderboard.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Shlwapi.lib")

// Function Definitions --------------------------------------------------------

void initialize_leaderboard() {
	HANDLE leaderboard_mutex = NULL;
	HANDLE write_mutex = NULL;
	HANDLE leaderboard_semaphore = NULL;

	if (PathFileExists((LPCTSTR) LEADERBOARD_PATH) == FALSE) {
		FILE *f = fopen(LEADERBOARD_PATH, "w");
		fprintf(f, "Name,Won,Lost,W/L Ratio\n");
		fclose(f);
	}
	create_and_check_mutex(&leaderboard_mutex, NULL, FALSE, LEADERBOARD_MUTEX, NULL);
	create_and_check_mutex(&write_mutex, NULL, FALSE, WRITE_MUTEX, NULL);
	CreateSemaphore(NULL, 2, 2, LEADERBOARD_SEMAPHORE);
}


int leaderboard(SOCKET t_socket) {
	BOOL refresh = TRUE;
	while (refresh == TRUE) {
		char *file_content = NULL;
		file_content = read_all_file();
		char *to_send = NULL;
		to_send = malloc(strlen(file_content) + 1 + MAX_MESSAGE_TYPE_LENGTH);
		int max_size = strlen(file_content) + 1 + MAX_MESSAGE_TYPE_LENGTH;
		char *send_params[4] = { file_content, NULL, NULL, NULL };
		create_string_to_send(to_send, "SERVER_LEADERBOARD", &send_params, &max_size);
		if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
			goto EXIT;
		}
		if (check_if_refresh(&refresh, t_socket) < 0) {
			goto EXIT;
		}
	}
EXIT:
	return SUCCESS_CODE;
	
}


int check_if_refresh(BOOL *refresh, SOCKET t_socket) {
	char *AcceptedStr = NULL;
	char *to_send[MAX_MESSAGE_TYPE_LENGTH];
	int max_size = MAX_MESSAGE_TYPE_LENGTH;
	char *message_type[MAX_MESSAGE_TYPE_LENGTH];
	char *params[NUM_PARAMETERS];

	create_string_to_send(to_send, "SERVER_LEADERBOARD_MENU", NULL, &max_size);
	if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}

	if (ReceiveAndCheck(&AcceptedStr, t_socket) == ERR_RECV_SOCKET) {
		return ERR_RECV_SOCKET;
	}
	parse_recv_string(AcceptedStr, message_type, &params);
	if (STRINGS_ARE_EQUAL(AcceptedStr, "CLIENT_REFRESH")) {
		*refresh = TRUE;
	}
	else if (STRINGS_ARE_EQUAL(AcceptedStr, "CLIENT_MAIN_MENU")) {
		*refresh = FALSE;
	}
	else {
		printf("Unknown protocol message\n");
		return ERR_CODE_DEFAULT;
	}
	return SUCCESS_CODE;
}


char* read_all_file() {
	HANDLE leaderboard_mutex;
	HANDLE leaderboard_semaphore;
	open_and_check_mutex(&leaderboard_mutex, SYNCHRONIZE, FALSE, LEADERBOARD_MUTEX, NULL);
	leaderboard_semaphore = OpenSemaphore(SYNCHRONIZE, FALSE, LEADERBOARD_SEMAPHORE);
	
	// lock mutex
	lock_mutex(&leaderboard_mutex, NULL);
	// wait on semaphore
	WaitForSingleObject(leaderboard_semaphore, INFINITE);
	// release mutex
	release_mutex(&leaderboard_mutex, NULL);

	FILE *f = fopen(LEADERBOARD_PATH, "r");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *string = NULL;
	string = (char*) malloc((fsize + 1) * sizeof(char));
	if (string == NULL) {
		printf("read leaderboard malloc error!!");
	}
	fgets(string, fsize, f);
	fclose(f);

	string[fsize] = 0;

	ReleaseSemaphore(leaderboard_semaphore, 1, NULL);
	return string;
}

void update_line(char *username, int win) {
	HANDLE leaderboard_mutex;
	HANDLE write_mutex;
	open_and_check_mutex(&leaderboard_mutex, SYNCHRONIZE, FALSE, LEADERBOARD_MUTEX, NULL);
	open_and_check_mutex(&write_mutex, SYNCHRONIZE, FALSE, WRITE_MUTEX, NULL);
	char to_write[1000];

	lock_mutex(&write_mutex, NULL);

	FILE *f = fopen(LEADERBOARD_PATH, "r+");
	char *buffer = NULL;
	buffer = (char*)malloc(200 * sizeof(char));
	if (buffer == NULL) {
		printf("updateline malloc error!!");
	}
	char *params[3];
	while (fgets(buffer, MAX_USERNAME_LEN, f) != NULL) {
		char *name = strtok(buffer, ",");
		if (STRINGS_ARE_EQUAL(name, username)) {
			char *param_in_str;
			param_in_str = strtok(NULL, "\n");
			params[0] = strtok(param_in_str, ";");
			for (int i = 1; i < 3; i++) {
				params[i] = strtok(NULL, ";");
			}
			int wins = atoi(params[0]);
			int loses = atoi(params[1]);
			if (win == 1) {
				wins++;
			}
			else {
				loses++;
			}
			float ratio = 0;
			if (loses != 0 && wins != 0) {
				ratio = wins / loses;
			}
			strncat(to_write, name, 200);
			strncat(to_write, ",", 1);
			strncat(to_write, params[0], 200);
			strncat(to_write, ",", 1);
			strncat(to_write, params[1], 200);
			strncat(to_write, ",", 1);
			strncat(to_write, params[2], 200);
			strncat(to_write, "\n", 1);

		}
		else {
			strncat(to_write, buffer, 200);
			strncat(to_write, "\n", 1);
		}
	}
	fclose(f);
	FILE *f2 = fopen(LEADERBOARD_PATH, "w");
	fprintf(f2, to_write);
	fclose(f2);

	release_mutex(&write_mutex, NULL);

	free(buffer);
}