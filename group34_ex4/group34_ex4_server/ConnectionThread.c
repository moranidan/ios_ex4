
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ConnectionThread.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Shlwapi.lib")


// is merged???

//Service thread is the thread that opens for each successful client connection and "talks" to the client.
DWORD ServiceThread(LPVOID lpParam)
{
	CONNECTION_THREAD_params_t *p_params;			// pointer for the parameters
	if (NULL == lpParam) {					// check if NULL was received instead of parameter
		printf("Error declaring parameters in ConnectionThread");
		return ERR_CODE_DEFAULT;
	}
	p_params = (CONNECTION_THREAD_params_t *)lpParam;

	SOCKET *t_socket = p_params->WorkerSocket;

	BOOL Done = FALSE;
	char *AcceptedStr = NULL;


	if (approve_client(p_params) < 0) {
		goto EXIT;
	}
	
	int main_menu_decision;
	BOOL replay;

	Done = Done || *(p_params->Done);
	while (!Done)
	{
		Done = Done || *(p_params->Done);
		replay = TRUE;
		if (main_menu(p_params, &main_menu_decision) < 0) {
			break;
		}
		if (main_menu_decision == CLIENT_CPU) {
			while (replay == TRUE) {
				if (cpu_game(p_params, &replay) < 0) {
					break;
				}
			}
		}
		else if (main_menu_decision == CLIENT_VERSUS) {
			printf("Versus game\n");
			while (replay == TRUE) {
				if (versus_game(p_params, &replay) < 0) {
					break;
				}
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
	printf("Conversation ended.\n");
	closesocket(*t_socket);	// TODO check
	*(p_params->WorkerSocket) = NULL;
	return 0;
}


int ReceiveAndCheck(char **AcceptedStr, SOCKET *socket) {
	TransferResult_t RecvRes;
	//if (setsockopt(*socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
	//	printf("setsockopt failed\n");
	RecvRes = ReceiveString(AcceptedStr, *socket);
	printf("recvncheck: %s\n", *AcceptedStr);
	if (RecvRes == TRNS_FAILED)
	{
		printf("Service socket error while reading, closing thread.\n");
		closesocket(*socket);
		return ERR_RECV_SOCKET;
	}
	else if (RecvRes == TRNS_DISCONNECTED)
	{
		printf("Connection closed while reading, closing thread.\n");
		closesocket(*socket);
		return ERR_RECV_SOCKET;
	}
	else
	{
		printf("Got string : %s\n", *AcceptedStr);
	}
}


int SendAndCheck(char *to_send, SOCKET *socket) {
	TransferResult_t SendRes;
	SendRes = SendString(to_send, *socket);

	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(*socket);
		return ERR_SEND_SOCKET;
	}
}


int approve_client(CONNECTION_THREAD_params_t *p_params) {
	char *AcceptedStr = NULL;
	char *message_type[MAX_MESSAGE_TYPE_LENGTH];
	char *params[NUM_PARAMETERS];
	char *to_send[50];
	int max_size = 50;
	//set timeout
	printf("receving approve\n");
	int timeout = 15000;

	if (ReceiveAndCheck(&AcceptedStr, p_params->WorkerSocket) == ERR_RECV_SOCKET) {
		return ERR_RECV_SOCKET;
	}
	printf("approve: %s\n", AcceptedStr);
	parse_recv_string(AcceptedStr, message_type, &params);
	printf("message type: %s, params[0]: %s\n", message_type, params[0]);
	if (STRINGS_ARE_EQUAL(message_type, "CLIENT_REQUEST")) {
		if (*(p_params->is_full) == TRUE) {
			// 2 client already connected, deny request
			char *send_params[4] = { "Too many clients", NULL, NULL, NULL };
			create_string_to_send(to_send, "SERVER_DENIED", &send_params, &max_size);
			if (SendAndCheck(to_send, p_params->WorkerSocket) == ERR_SEND_SOCKET) {
				return ERR_SEND_SOCKET;
			}
			*(p_params->Done) = TRUE;
		}
		else {
			// approve request
			create_string_to_send(to_send, "SERVER_APPROVED", NULL, &max_size);
			if (SendAndCheck(to_send, p_params->WorkerSocket) == ERR_SEND_SOCKET) {
				return ERR_SEND_SOCKET;
			}
			p_params->username = params[0];
		}
	}
	else {
		printf("Unknown protocol message\n");
		return ERR_CODE_DEFAULT;
	}
	return SUCCESS_CODE;
}


int main_menu(CONNECTION_THREAD_params_t *p_params, int *main_menu_decision) {
	char *AcceptedStr = NULL;
	char *to_send[MAX_MESSAGE_TYPE_LENGTH];
	int max_size = MAX_MESSAGE_TYPE_LENGTH;
	char *message_type[MAX_MESSAGE_TYPE_LENGTH];
	char *params[NUM_PARAMETERS];

	// send main menu
	create_string_to_send(to_send, "SERVER_MAIN_MENU", NULL, &max_size);
	if (SendAndCheck(to_send, p_params->WorkerSocket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}
	// receive clients choice
	if (ReceiveAndCheck(&AcceptedStr, p_params->WorkerSocket) == ERR_RECV_SOCKET) {
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
	else if (STRINGS_ARE_EQUAL(AcceptedStr, "CLIEN_DISCONNECT")) {
		*main_menu_decision = CLIENT_DISCONNECT;
	}
	else {
		printf("Unknown protocol message\n");
		return ERR_CODE_DEFAULT;
	}
	return SUCCESS_CODE;
}


int cpu_game(CONNECTION_THREAD_params_t *p_params, BOOL *replay) {
	int player_choice = -1;
	int cpu_choice;

	// choose random move
	srand(time(NULL));   // Initialization
	cpu_choice = rand() % 5;


	int ret_code = ask_and_receive_move(&player_choice, p_params);
	if (ret_code < 0) {
		return ret_code;
	}

	// find the winner and send results //TODO make function
	char player2_username[MAX_USERNAME_LEN] = "SERVER";
	find_and_send_winner(p_params, &player_choice, &cpu_choice, p_params->username, player2_username);

	return check_if_replay(p_params, replay);
}


int ask_and_receive_move(int *player_choice, CONNECTION_THREAD_params_t *p_params) {
	char *AcceptedStr = NULL;
	char *to_send[MAX_MESSAGE_TYPE_LENGTH];
	int max_size = MAX_MESSAGE_TYPE_LENGTH;
	char *message_type[MAX_MESSAGE_TYPE_LENGTH];
	char *params[NUM_PARAMETERS];

	// ask for players move
	create_string_to_send(to_send, "SERVER_PLAYER_MOVE_REQUEST", NULL, &max_size);
	if (SendAndCheck(to_send, p_params->WorkerSocket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}

	// receive players move
	if (ReceiveAndCheck(&AcceptedStr, p_params->WorkerSocket) == ERR_RECV_SOCKET) {
		return ERR_RECV_SOCKET;
	}
	parse_recv_string(AcceptedStr, message_type, params);
	if (STRINGS_ARE_EQUAL(AcceptedStr, "CLIENT_PLAYER_MOVE")) {
		*player_choice = name_to_number(params[0]);
	}
	return SUCCESS_CODE;
}


int find_and_send_winner(CONNECTION_THREAD_params_t *p_params, int *player1_choice, int *player2_choice, char *player1_username, char *player2_username) {
	char *to_send[MAX_RESULTS_LENGTH];
	int max_size = MAX_RESULTS_LENGTH;
	char player1_move[MAX_MOVE_LEN];
	number_to_name(*player1_choice, player1_move);
	char player2_move[MAX_MOVE_LEN];
	number_to_name(*player2_choice, player2_move);
	if (*player1_choice == *player2_choice) {
		char *send_params[4] = { player2_username, player2_move, player1_move, NULL };
		create_string_to_send(to_send, "SERVER_GAME_RESULTS", &send_params, &max_size);
	}
	else {
		if (player_win(*player1_choice, *player2_choice) == TRUE) {
			char *send_params[4] = { player2_username, player2_move, player1_move, player1_username };
			create_string_to_send(to_send, "SERVER_GAME_RESULTS", &send_params, &max_size);
		}
		else {
			char *send_params[4] = { player2_username, player2_move, player1_move, "SERVER" };
			create_string_to_send(to_send, "SERVER_GAME_RESULTS", &send_params, &max_size);
		}
	}
	if (SendAndCheck(to_send, p_params->WorkerSocket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}
	return SUCCESS_CODE;
}



BOOL player_win(int player_choice, int cpu_choice) {
	int winner = (cpu_choice - player_choice) % 5;
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


int check_if_replay(CONNECTION_THREAD_params_t *p_params, BOOL *replay) {
	char *AcceptedStr = NULL;
	char *to_send[MAX_MESSAGE_TYPE_LENGTH];
	int max_size = MAX_MESSAGE_TYPE_LENGTH;
	char *message_type[MAX_MESSAGE_TYPE_LENGTH];
	char *params[NUM_PARAMETERS];


	create_string_to_send(to_send, "SERVER_GAME_OVER_MENU", NULL, &max_size);
	if (SendAndCheck(to_send, p_params->WorkerSocket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}

	if (ReceiveAndCheck(&AcceptedStr, p_params->WorkerSocket) == ERR_RECV_SOCKET) {
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


int versus_game(CONNECTION_THREAD_params_t *p_params, BOOL *replay) {
	printf("versus");
	int return_code = SUCCESS_CODE;
	char *AcceptedStr = NULL;
	char *to_send[MAX_MESSAGE_TYPE_LENGTH];
	int max_size = MAX_MESSAGE_TYPE_LENGTH + 21;
	char *message_type[MAX_MESSAGE_TYPE_LENGTH];
	char *params[NUM_PARAMETERS];
	char *username[MAX_USERNAME_LEN];
	BOOL created = FALSE;
	DWORD wait_code;
	FILE *fd = NULL;
	HANDLE player1_username;
	HANDLE player2_username;
	HANDLE file_mutex_handle;


	if (create_and_check_event(&player1_username, PLAYER1_EVENT_NAME) < 0) {
		return ERR_CODE_EVENT;
	}
	if (create_and_check_event(&player2_username, PLAYER2_EVENT_NAME) < 0) {
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
		// player1
		// release file mutex
		if (release_mutex(&file_mutex_handle, &return_code) != SUCCESS_CODE) {
			return ERR_CODE_MUTEX;
		}
		wait_code = WaitForSingleObject(player2_username, INFINITE);	// wait for threads to run TODO change timeout
		if (WAIT_TIMEOUT == wait_code) {
			// timeout reached, no other player 
			create_string_to_send(to_send, "SERVER_NO_OPPONENTS", NULL, &max_size);
			if (SendAndCheck(to_send, p_params->WorkerSocket) == ERR_SEND_SOCKET) {
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
			return_code = ERR_CODE_EVENT;
		}
		// lock file mutex
		if (lock_mutex(&file_mutex_handle, &return_code) != SUCCESS_CODE) {
			return ERR_CODE_MUTEX;
		}
		read_line(&fd, username);
		write_line(&fd, *(p_params->username));
		// release mutex
		if (release_mutex(&file_mutex_handle, &return_code) != SUCCESS_CODE) {
			return ERR_CODE_MUTEX;
		}
		set_and_check_event(player1_username);
		char *send_params[4] = { username, NULL, NULL, NULL };
		create_string_to_send(to_send, "SERVER_INVITE", &send_params, &max_size);
		if (SendAndCheck(to_send, p_params->WorkerSocket) == ERR_SEND_SOCKET) {
			return ERR_SEND_SOCKET;
		}
		//player1_game(&fd, &file_mutex_handle);

		fclose(fd);
		if (delete_file(&fd) < 0) {
			return ERR_CODE_FILE;
		}



	}
	else {
		// player2

		write_line(fd, (p_params->username));
		// release file mutex
		if (release_mutex(&file_mutex_handle, &return_code) != SUCCESS_CODE) {
			return ERR_CODE_MUTEX;
		}
		set_and_check_event(&player2_username);
		wait_code = WaitForSingleObject(&player1_username, INFINITE);	// wait for threads to run
		if (WAIT_OBJECT_0 != wait_code)			// check for errors
		{
			printf("Error when waiting\n");
			// return_code = ERR_CODE_EVENT;
		}
		read_line(&fd, username);
		char *send_params[4] = { username, NULL, NULL, NULL };
		create_string_to_send(to_send, "SERVER_INVITE", &send_params, &max_size);
		if (SendAndCheck(to_send, p_params->WorkerSocket) == ERR_SEND_SOCKET) {
			return ERR_SEND_SOCKET;
		}
		//player2_game();
		fclose(fd);
	}
	return check_if_replay(p_params, replay);

}


BOOL file_exists(FILE *fd) {	// TODO
	if (PathFileExists((LPCTSTR) "GameSession.txt") == TRUE) {
		fd = fopen(GAME_SESSION_PATH, "r+");
		if (fd == NULL) {
			printf("Error when opening game session file\n");
		}
		//fputs("ofertest", fd);
		//fprintf(fd, "%s\n", "morantest"); //delete
		//int error_code = fprintf(*fd, "%s\n", "ofer");//p_params->username);
		//printf(error_code);
		fclose(fd);
		return TRUE;
	}
	else {
		fd = fopen(GAME_SESSION_PATH, "w+");
		//if (fopen_s(fd, GAME_SESSION_PATH, "a+") != 0) {	// open new for read and write
		if (fd == NULL) {
			printf("Error when opening game session file\n");
		}
		//fputs("ofertest2", *fd);
		//fprintf(fd, "%s\n", "morantest2"); //delete
		//int error_code = fprintf(*fd, "%s\n", "ofer");//p_params->username);
		//printf(error_code);
		fclose(fd);
		return FALSE;
	}
}


int delete_file(FILE *fd) {
	if (remove(GAME_SESSION_PATH) == 0) {
		printf("Deleted successfully");
		return SUCCESS_CODE;
	}
	else {
		printf("Unable to delete the file");
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
	if (fprintf(fd, "%s\n", *line) < 0) {
		return ERR_CODE_FILE;
	}
	fclose(fd);
	return SUCCESS_CODE;
}

/*
int single_multiplayer_game() {

}*/

char* GetFileLastRow(FILE *fp) {
	char *buffer = NULL;
	buffer = (char*)malloc(200 * sizeof(char));
	if (buffer == NULL) {
		printf("GetFileLastRow malloc error!!");
	}
	while (fgets(buffer, sizeof(buffer), fp) != NULL) {
		;
	}
	char *return_pointer = strtok(buffer, "\n");
	free(buffer);
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
