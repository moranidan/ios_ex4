
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ConnectionThread.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Shlwapi.lib")



//Service thread is the thread that opens for each successful client connection and "talks" to the client.
DWORD ServiceThread(LPVOID lpParam)
{
	CONNECTION_THREAD_params_t *p_params;			// pointer for the parameters
	if (NULL == lpParam) {					// check if NULL was received instead of parameter
		printf("Error declaring parameters in ConnectionThread");
		return ERR_CODE_DEFAULT;
	}
	p_params = (CONNECTION_THREAD_params_t *)lpParam;

	SOCKET t_socket = p_params->WorkerSocket;

	BOOL Done = FALSE;
	char *AcceptedStr = NULL;
	char username[MAX_USERNAME_LEN];

	if (approve_client(p_params, username, t_socket) < 0) {
		goto EXIT;
	}

	
	int main_menu_decision;
	BOOL replay;

	Done = Done || *(p_params->Done);
	while (!Done)
	{
		Done = Done || *(p_params->Done);
		replay = TRUE;
		if (main_menu(&main_menu_decision, t_socket) < 0) {
			break;
		}
		if (main_menu_decision == CLIENT_CPU) {
			while (replay == TRUE) {
				if (cpu_game(&replay, username, t_socket) < 0) {
					break;
				}
			}
		}
		else if (main_menu_decision == CLIENT_VERSUS) {
			if (versus_game(&replay, username, t_socket) < 0) {
				break;
			}
		}
		else if (main_menu_decision == CLIENT_LEADERBOARD) {
			printf("leaderboard\n");
			//leaderboard();
		}
		else if (main_menu_decision == CLIENT_DISCONNECT) {
			Done = TRUE;
		}
	}

EXIT:
	closesocket(t_socket);	// TODO check
	(p_params->WorkerSocket) = NULL;
	return 0;
}


int ReceiveAndCheck(char **AcceptedStr, SOCKET socket) {
	TransferResult_t RecvRes;
	RecvRes = ReceiveString(AcceptedStr, socket);
	if (RecvRes == TRNS_FAILED)
	{
		printf("Service socket error while reading, closing thread.\n");
		closesocket(socket);
		return ERR_RECV_SOCKET;
	}
	else if (RecvRes == TRNS_DISCONNECTED)
	{
		printf("Connection closed while reading, closing thread.\n");
		closesocket(socket);
		return ERR_RECV_SOCKET;
	}
	return SUCCESS_CODE;
}


int SendAndCheck(char *to_send, SOCKET socket) {
	TransferResult_t SendRes;
	SendRes = SendString(to_send, socket);

	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(socket);
		return ERR_SEND_SOCKET;
	}
}


int approve_client(CONNECTION_THREAD_params_t *p_params, char *username, SOCKET t_socket) {
	char *AcceptedStr = NULL;
	char *message_type[MAX_MESSAGE_TYPE_LENGTH];
	char *params[NUM_PARAMETERS];
	char *to_send[50];
	int max_size = 50;

	// set timeout
	int timeout = 15000;
	if (setsockopt(t_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
		printf("setsockopt failed\n");
	}
	if (ReceiveAndCheck(&AcceptedStr, t_socket) == ERR_RECV_SOCKET) {
		return ERR_RECV_SOCKET;
	}
	// disable timeout
	timeout = 0;
	if (setsockopt(t_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
		printf("setsockopt failed\n");
	}
	parse_recv_string(AcceptedStr, message_type, &params);
	if (STRINGS_ARE_EQUAL(message_type, "CLIENT_REQUEST")) {
		if (*(p_params->is_full) == TRUE) {
			// 2 client already connected, deny request
			char *send_params[4] = { "Too many clients", NULL, NULL, NULL };
			create_string_to_send(to_send, "SERVER_DENIED", &send_params, &max_size);
			if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
				return ERR_SEND_SOCKET;
			}
			*(p_params->Done) = TRUE;
		}
		else {
			// approve request
			create_string_to_send(to_send, "SERVER_APPROVED", NULL, &max_size);
			if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
				return ERR_SEND_SOCKET;
			}
			strcpy_s(username, MAX_USERNAME_LEN, params[0]);
		}
	}
	else {
		printf("Unknown protocol message\n");
		return ERR_CODE_DEFAULT;
	}
	return SUCCESS_CODE;
}


int main_menu(int *main_menu_decision, SOCKET t_socket) {
	char *AcceptedStr = NULL;
	char *to_send[MAX_MESSAGE_TYPE_LENGTH];
	int max_size = MAX_MESSAGE_TYPE_LENGTH;
	char *message_type[MAX_MESSAGE_TYPE_LENGTH];
	char *params[NUM_PARAMETERS];

	// send main menu
	create_string_to_send(to_send, "SERVER_MAIN_MENU", NULL, &max_size);
	if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}
	// receive clients choice
	if (ReceiveAndCheck(&AcceptedStr, t_socket) == ERR_RECV_SOCKET) {
		return ERR_RECV_SOCKET;
	}
	parse_recv_string(AcceptedStr, message_type, &params);
	if (STRINGS_ARE_EQUAL(AcceptedStr, "CLIENT_CPU")) {
		*main_menu_decision = CLIENT_CPU;
	}
	else if (STRINGS_ARE_EQUAL(AcceptedStr, "CLIENT_VERSUS")) {
		*main_menu_decision = CLIENT_VERSUS;
	}
	else if (STRINGS_ARE_EQUAL(AcceptedStr, "CLIENT_LEADERBOARD")) {
		*main_menu_decision = CLIENT_LEADERBOARD;
	}
	else if (STRINGS_ARE_EQUAL(AcceptedStr, "CLIENT_DISCONNECT")) {
		*main_menu_decision = CLIENT_DISCONNECT;
	}
	else {
		printf("Unknown protocol message\n");
		return ERR_CODE_DEFAULT;
	}
	return SUCCESS_CODE;
}


int cpu_game(BOOL *replay, char *username, SOCKET t_socket) {
	int player_choice = -1;
	int cpu_choice;

	// choose random move
	srand(time(NULL));   // Initialization
	cpu_choice = rand() % 5;


	int ret_code = ask_and_receive_move(&player_choice, t_socket);
	if (ret_code < 0) {
		return ret_code;
	}

	// find the winner and send results
	char player2_username[MAX_USERNAME_LEN] = "SERVER";
	find_and_send_winner(&player_choice, &cpu_choice, username, player2_username, 1, t_socket);

	return check_if_replay(replay, t_socket);
}


int ask_and_receive_move(int *player_choice, SOCKET t_socket) {
	char *AcceptedStr = NULL;
	char *to_send[MAX_MESSAGE_TYPE_LENGTH];
	int max_size = MAX_MESSAGE_TYPE_LENGTH;
	char *message_type[MAX_MESSAGE_TYPE_LENGTH];
	char *params[NUM_PARAMETERS];

	// ask for players move
	create_string_to_send(to_send, "SERVER_PLAYER_MOVE_REQUEST", NULL, &max_size);
	if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}

	// receive players move
	if (ReceiveAndCheck(&AcceptedStr, t_socket) == ERR_RECV_SOCKET) {
		return ERR_RECV_SOCKET;
	}
	parse_recv_string(AcceptedStr, message_type, params);
	if (STRINGS_ARE_EQUAL(AcceptedStr, "CLIENT_PLAYER_MOVE")) {
		*player_choice = name_to_number(params[0]);
	}
	return SUCCESS_CODE;
}


int find_and_send_winner(int *player1_choice, int *player2_choice, char *player1_username, char *player2_username, int you, SOCKET t_socket) {
	char *to_send[MAX_RESULTS_LENGTH];
	int max_size = MAX_RESULTS_LENGTH;
	char player1_move[MAX_MOVE_LEN];
	number_to_name(*player1_choice, player1_move);
	char player2_move[MAX_MOVE_LEN];
	number_to_name(*player2_choice, player2_move);

	char *send_params[4] = { NULL, NULL, NULL, NULL };
	if (you == 1) {
		send_params[0] = player2_username;
		send_params[1] = player2_move;
		send_params[2] = player1_move;
	}
	else if (you == 2) {
		send_params[0] = player1_username;
		send_params[1] = player1_move;
		send_params[2] = player2_move;
	}

	if (*player1_choice == *player2_choice) {
		create_string_to_send(to_send, "SERVER_GAME_RESULTS", &send_params, &max_size);
	}
	else {
		if (player_win(*player1_choice, *player2_choice) == TRUE) {
			send_params[3] = player1_username;
			create_string_to_send(to_send, "SERVER_GAME_RESULTS", &send_params, &max_size);
		}
		else {
			send_params[3] = player2_username;
			create_string_to_send(to_send, "SERVER_GAME_RESULTS", &send_params, &max_size);
		}
	}
	if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}
	return SUCCESS_CODE;
}


BOOL player_win(int player_choice, int cpu_choice) {
	int winner = ((cpu_choice - player_choice)+5) % 5;
	if (winner < 3) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}


int name_to_number(char *move_name) {
	if (STRINGS_ARE_EQUAL(move_name, "ROCK")) {
		return ROCK;
	}
	else if (STRINGS_ARE_EQUAL(move_name, "SPOCK")) {
		return SPOCK;
	}
	else if (STRINGS_ARE_EQUAL(move_name, "PAPER")) {
		return PAPER;
	}
	else if (STRINGS_ARE_EQUAL(move_name, "LIZARD")) {
		return LIZARD;
	}
	else if (STRINGS_ARE_EQUAL(move_name, "SCISSORS")) {
		return SCISSORS;
	}
	else {
		printf("not A valid move\n");
		return ERR_CODE_DEFAULT;
	}
}


void number_to_name(int num, char *move_name) {
	if (num == 0) {
		strcpy_s(move_name, MAX_MOVE_LEN, "ROCK");
	}
	else if (num == 1) {
		strcpy_s(move_name, MAX_MOVE_LEN, "SPOCK");
	}
	else if (num == 2) {
		strcpy_s(move_name, MAX_MOVE_LEN, "PAPER");
	}
	else if (num == 3) {
		strcpy_s(move_name, MAX_MOVE_LEN, "LIZARD");
	}
	else if (num == 4) {
		strcpy_s(move_name, MAX_MOVE_LEN, "SCISSORS");
	}
	else {
		strcpy_s(move_name, MAX_MOVE_LEN, "ERROR");
	}
}


int check_if_replay(BOOL *replay, SOCKET t_socket) {
	char *AcceptedStr = NULL;
	char *to_send[MAX_MESSAGE_TYPE_LENGTH];
	int max_size = MAX_MESSAGE_TYPE_LENGTH;
	char *message_type[MAX_MESSAGE_TYPE_LENGTH];
	char *params[NUM_PARAMETERS];

	create_string_to_send(to_send, "SERVER_GAME_OVER_MENU", NULL, &max_size);
	if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}

	if (ReceiveAndCheck(&AcceptedStr, t_socket) == ERR_RECV_SOCKET) {
		return ERR_RECV_SOCKET;
	}
	parse_recv_string(AcceptedStr, message_type, &params);
	if (STRINGS_ARE_EQUAL(AcceptedStr, "CLIENT_REPLAY")) {
		*replay = TRUE;
	}
	else if (STRINGS_ARE_EQUAL(AcceptedStr, "CLIENT_MAIN_MENU")) {
		*replay = FALSE;
	}
	else {
		printf("Unknown protocol message\n");
		return ERR_CODE_DEFAULT;
	}
	return SUCCESS_CODE;
}


int versus_game(BOOL *replay, char *username, SOCKET t_socket) {
	int return_code = SUCCESS_CODE;
	FILE *fd = NULL;
	HANDLE player1_event;
	HANDLE player2_event;
	HANDLE file_mutex_handle;

	if (create_and_check_event(&player1_event, PLAYER1_EVENT_NAME) < 0) {
		return ERR_CODE_EVENT;
	}
	if (create_and_check_event(&player2_event, PLAYER2_EVENT_NAME) < 0) {
		return ERR_CODE_EVENT;
	}
	if (create_and_check_mutex(&file_mutex_handle, NULL, FALSE, FILE_MUTEX_NAME, NULL) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}
	// lock file mutex
	if (lock_mutex(&file_mutex_handle, &return_code) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}
	if (file_exists(fd) == FALSE) {
		// player1 - file must be created
		return_code = player1_game(fd, file_mutex_handle, player1_event, player2_event, username, t_socket);
		if (delete_file() < 0) {
			return ERR_CODE_FILE;
		}
	}
	else {
		// player2 - file already there
		return_code = player2_game(fd, file_mutex_handle, player1_event, player2_event, username, t_socket);
	}
	return return_code;
}


int player1_game(FILE *fd, HANDLE file_mutex_handle, HANDLE player1_event, HANDLE player2_event, char *username, SOCKET t_socket) {
	DWORD wait_code;
	char *to_send[MAX_MESSAGE_TYPE_LENGTH];
	int max_size = MAX_MESSAGE_TYPE_LENGTH + MAX_USERNAME_LEN;
	char *username_read = NULL;
	char *choice_str = NULL;
	BOOL player1_replay = TRUE;
	BOOL player2_replay = TRUE;

	// release file mutex
	if (release_mutex(&file_mutex_handle, NULL) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}
	wait_code = WaitForSingleObject(player2_event, INFINITE);	// wait for threads to run TODO change timeout
	if (WAIT_TIMEOUT == wait_code) {
		// timeout reached, no other player 
		create_string_to_send(to_send, "SERVER_NO_OPPONENTS", NULL, &max_size);
		if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
			return ERR_SEND_SOCKET;
		}
		if (delete_file(fd) < 0) {
			return ERR_CODE_FILE;
		}
		return SUCCESS_CODE;
	}
	else if (WAIT_OBJECT_0 != wait_code)			// check for errors
	{
		printf("Error when waiting\n");
		return ERR_CODE_EVENT;
	}
	// lock file mutex
	if (lock_mutex(&file_mutex_handle, NULL) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}
	username_read = GetFileLastRow(fd);
	write_line(fd, username);
	// release mutex
	if (release_mutex(&file_mutex_handle, NULL) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}
	set_and_check_event(&player1_event);
	char *send_params[4] = { username_read, NULL, NULL, NULL };
	create_string_to_send(to_send, "SERVER_INVITE", &send_params, &max_size);
	if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}
	int player1_choice = -1;
	int player2_choice = -1;
	while (player1_replay == TRUE && player2_replay == TRUE) {
		// game
		int ret_code = ask_and_receive_move(&player1_choice, t_socket);
		if (ret_code < 0) {
			return ret_code;
		}
		// lock file mutex
		if (lock_mutex(&file_mutex_handle, NULL) != SUCCESS_CODE) {
			return ERR_CODE_MUTEX;
		}
		char move_name[MAX_MOVE_LEN];
		number_to_name(player1_choice, move_name);
		write_line(fd, move_name);
		// release mutex
		set_and_check_event(&player1_event);
		if (release_mutex(&file_mutex_handle, NULL) != SUCCESS_CODE) {
			return ERR_CODE_MUTEX;
		}
		wait_code = WaitForSingleObject(player2_event, INFINITE);	// wait for threads to run TODO change timeout
		choice_str = GetFileLastRow(fd);
		player2_choice = name_to_number(choice_str);
		// find the winner and send results
		find_and_send_winner(&player1_choice, &player2_choice, username, username_read, 1, t_socket);
		// check if replay
		int err_code = check_if_replay(&player1_replay, t_socket);
		if (err_code < 0) {
			return err_code;
		}
		if (player1_replay == FALSE) {
			break;
		}
		set_and_check_event(&player1_event);
		wait_code = WaitForSingleObject(player2_event, RESPONSE_TIMEOUT);	// wait for threads to run TODO change timeout
		if (WAIT_TIMEOUT == wait_code) {
			// timeout reached, no other player 
			player2_replay = FALSE;
		}
	}
	if (player1_replay == TRUE) {
		char *send_params[4] = { username_read, NULL, NULL, NULL };
		create_string_to_send(to_send, "SERVER_OPPONENT_QUIT", &send_params, &max_size);//TODO
		if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
			return ERR_SEND_SOCKET;
		}
	}
	free(username_read);
	free(choice_str);
	return SUCCESS_CODE;
}


int player2_game(FILE *fd, HANDLE file_mutex_handle, HANDLE player1_event, HANDLE player2_event, char *username, SOCKET t_socket) {
	DWORD wait_code;
	char *to_send[MAX_MESSAGE_TYPE_LENGTH];
	int max_size = MAX_MESSAGE_TYPE_LENGTH + MAX_USERNAME_LEN;
	char *username_read = NULL;
	char *choice_str = NULL;
	BOOL player1_replay = TRUE;
	BOOL player2_replay = TRUE;

	write_line(fd, username);
	// release file mutex
	if (release_mutex(&file_mutex_handle, NULL) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}
	set_and_check_event(&player2_event);
	wait_code = WaitForSingleObject(player1_event, INFINITE);	// wait for threads to run
	if (WAIT_OBJECT_0 != wait_code)			// check for errors
	{
		printf("Error when waiting\n");
		return ERR_CODE_EVENT;
	}
	username_read = GetFileLastRow(fd);
	char *send_params[4] = { username_read, NULL, NULL, NULL };
	create_string_to_send(to_send, "SERVER_INVITE", &send_params, &max_size);
	if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}

	while (player1_replay == TRUE && player2_replay == TRUE) {
		// game
		int player1_choice = -1;
		int player2_choice = -1;
		int ret_code = ask_and_receive_move(&player2_choice, t_socket);
		if (ret_code < 0) {
			return ret_code;
		}
		wait_code = WaitForSingleObject(player1_event, INFINITE);	// wait for threads to run TODO change timeout
		choice_str = GetFileLastRow(fd);
		player1_choice = name_to_number(choice_str);
		// lock file mutex
		if (lock_mutex(&file_mutex_handle, NULL) != SUCCESS_CODE) {
			return ERR_CODE_MUTEX;
		}
		char move_name[MAX_MOVE_LEN];
		number_to_name(player2_choice, move_name);
		write_line(fd, move_name);
		// release mutex
		if (release_mutex(&file_mutex_handle, NULL) != SUCCESS_CODE) {
			return ERR_CODE_MUTEX;
		}
		set_and_check_event(&player2_event);

		// find the winner and send results
		find_and_send_winner(&player1_choice, &player2_choice, username_read, username, 2, t_socket);

		int err_code = check_if_replay(&player2_replay, t_socket);
		if (err_code < 0) {
			return err_code;
		}
		if (player2_replay == FALSE) {
			break;
		}
		set_and_check_event(&player2_event);
		wait_code = WaitForSingleObject(player1_event, RESPONSE_TIMEOUT);	
		if (WAIT_TIMEOUT == wait_code) {
			// timeout reached, no other player 
			player1_replay = FALSE;
		}
	}
	if (player2_replay == TRUE) {
		char *send_params[4] = { username_read, NULL, NULL, NULL };
		create_string_to_send(to_send, "SERVER_OPPONENT_QUIT", &send_params, &max_size);
		if (SendAndCheck(to_send, t_socket) == ERR_SEND_SOCKET) {
			return ERR_SEND_SOCKET;
		}
	}
	free(username_read);
	free(choice_str);
	return SUCCESS_CODE;
}


BOOL file_exists(FILE *fd) {
	if (PathFileExists((LPCTSTR) "GameSession.txt") == TRUE) {
		return TRUE;
	}
	else {
		fd = fopen(GAME_SESSION_PATH, "w+");
		if (fd == NULL) {
			printf("Error when opening game session file\n");
		}
		fclose(fd);
		return FALSE;
	}
}


int delete_file() {
	if (remove(GAME_SESSION_PATH) == 0) {
		return SUCCESS_CODE;
	}
	else {
		return ERR_CODE_FILE;
	}
}


int read_line(FILE *fd, char *line) {
	fd = fopen(GAME_SESSION_PATH, "a+");
	if (fd == NULL) {
		printf("Error when opening game session file\n");
	}
	if (fgets(line, sizeof(line), fd) == NULL) {
		return ERR_CODE_FILE;
	}
	fclose(fd);
	return SUCCESS_CODE;
}


int write_line(FILE *fd, char *line) {
	fd = fopen(GAME_SESSION_PATH, "a+");
	if (fd == NULL) {
		printf("Error when opening game session file\n");
	}
	if (fprintf(fd, "%s\n", line) < 0) {
		return ERR_CODE_FILE;
	}
	fclose(fd);
	return SUCCESS_CODE;
}


char* GetFileLastRow(FILE *fp) {
	fp = fopen(GAME_SESSION_PATH, "r");
	if (fp == NULL) {
		printf("Error when opening game session file\n");
	}
	char *buffer = NULL;
	buffer = (char*)malloc(200 * sizeof(char));
	if (buffer == NULL) {
		printf("GetFileLastRow malloc error!!");
	}
	while (fgets(buffer, sizeof(buffer), fp) != NULL) {
		;
	}
	char *return_pointer = strtok(buffer, "\n");
	fclose(fp);
	return return_pointer;
}


int create_and_check_event(HANDLE *event_handle, char *event_name)
{
	/* Get handle to event by name. If the event doesn't exist, create it */
	*event_handle = CreateEvent(
		NULL,			/* default security attributes */
		FALSE,			/* manual-reset event */
		FALSE,			/* initial state is non-signaled */
		event_name);	/* name */
	if (*event_handle == NULL) {
		printf("Error when creating event: %d\n", GetLastError());
		return ERR_CODE_EVENT;
	}

	return SUCCESS_CODE;
}


int set_and_check_event(HANDLE *event_handle) {
	if (SetEvent(*event_handle) == 0) {
		printf("Error when setting event: %d\n", GetLastError());
		return ERR_CODE_EVENT;
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
