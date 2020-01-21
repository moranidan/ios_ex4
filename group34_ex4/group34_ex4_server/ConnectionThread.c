
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ConnectionThread.h"
#pragma comment(lib, "Ws2_32.lib")

// is merged???

//Service thread is the thread that opens for each successful client connection and "talks" to the client.
DWORD ServiceThread(LPVOID lpParam)
{
	CONNECTION_THREAD_params_t *p_params;			// pointer for the parameters
	if (NULL == lpParam) {					// check if NULL was received instead of parameter
		printf("Error declaring parameters in ConnectionThread");
		return ERR_CODE_DEFAULT;
	}
	p_params = (LISTEN_THREAD_params_t *)lpParam;

	SOCKET *t_socket = p_params->WorkerSocket;
	char SendStr[SEND_STR_SIZE];

	BOOL Done = FALSE;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	char *AcceptedStr = NULL;


	//strcpy(SendStr, "Welcome to this server!");
	//RecvRes = ReceiveString(&AcceptedStr, *t_socket);
	if (approve_client(p_params) < 0) {
		goto EXIT;
	}

	/*
	strcpy(SendStr, "Welcome to this server!");
	SendRes = SendString(SendStr, *t_socket);

	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(*t_socket);
		return 1;
	}*/
	
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
			//versus_game();
		}
		else if (main_menu_decision == CLIENT_LEADERBOARD) {
			printf("leaderboard\n");
			//leaderboard();
		}
		else if (main_menu_decision == CLIENT_DISCONNECT) {
			Done = TRUE;
		}
		
		
		/*RecvRes = ReceiveString(&AcceptedStr, *t_socket);

		if (RecvRes == TRNS_FAILED)
		{
			printf("Service socket error while reading, closing thread.\n");
			closesocket(*t_socket);
			return 1;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("Connection closed while reading, closing thread.\n");
			closesocket(*t_socket);
			return 1;
		}
		else
		{
			printf("Got string : %s\n", AcceptedStr);
		}


		if (STRINGS_ARE_EQUAL(AcceptedStr, "hello"))
		{
			strcpy(SendStr, "what's up?");
		}
		else if (STRINGS_ARE_EQUAL(AcceptedStr, "how are you?"))
		{
			strcpy(SendStr, "great");
		}
		else if (STRINGS_ARE_EQUAL(AcceptedStr, "bye"))
		{
			strcpy(SendStr, "see ya!");
			Done = TRUE;
		}
		else
		{
			strcpy(SendStr, "I don't understand");
		}

		SendRes = SendString(SendStr, *t_socket);

		if (SendRes == TRNS_FAILED)
		{
			printf("Service socket error while writing, closing thread.\n");
			closesocket(*t_socket);
			return 1;
		}

		free(AcceptedStr);
		

		*/
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
	char *AcceptedStr = NULL;
	char *to_send[140];
	int max_size = 140;
	char *message_type[MAX_MESSAGE_TYPE_LENGTH];
	char *params[NUM_PARAMETERS];
	int player_choice;
	int cpu_choice;

	// choose random move
	srand(time(NULL));   // Initialization
	cpu_choice = rand() % 5;

	// ask for players move
	create_string_to_send(to_send, "SERVER_PLAYER_MOVE_REQUEST", NULL, &max_size);
	if (SendAndCheck(to_send, p_params->WorkerSocket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}

	// receive players move
	if (ReceiveAndCheck(&AcceptedStr, p_params->WorkerSocket) == ERR_RECV_SOCKET) {
		return ERR_RECV_SOCKET;
	}
	parse_recv_string(AcceptedStr, message_type, &params);
	if (STRINGS_ARE_EQUAL(AcceptedStr, "CLIENT_PLAYER_MOVE")) {
		player_choice = name_to_number(params[0]);
	}
	// find the winner and send results
	char *cpu_move[MAX_MOVE_LEN];
	number_to_name(cpu_choice, cpu_move);
	if (player_choice == cpu_choice) {
		char *send_params[4] = { "SERVER", cpu_move, params[0], NULL };
		create_string_to_send(to_send, "SERVER_GAME_RESULTS", &send_params, &max_size);
	}
	if (player_win(player_choice, cpu_choice) == TRUE) {
		char *send_params[4] = { "SERVER", cpu_move, params[0], p_params->username };
		create_string_to_send(to_send, "SERVER_GAME_RESULTS", &send_params, &max_size);
	}
	else {
		char *send_params[4] = { "SERVER", cpu_move, params[0], "SERVER" };
		create_string_to_send(to_send, "SERVER_GAME_RESULTS", &send_params, &max_size);

	}
	if (SendAndCheck(to_send, p_params->WorkerSocket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}
	return check_if_replay(p_params, replay);
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


int versus_game(CONNECTION_THREAD_params_t *p_params) {
	printf("versus");
	/*
	char *AcceptedStr = NULL;
	char *to_send[MAX_MESSAGE_TYPE_LENGTH];
	int max_size = MAX_MESSAGE_TYPE_LENGTH;
	char *message_type[MAX_MESSAGE_TYPE_LENGTH];
	char *params[NUM_PARAMETERS];
	BOOL created = FALSE;
	FILE *fd = NULL;

	// open/create mutex
	open_mutex();
	open_file(&created, fd);

	create_string_to_send(to_send, "SERVER_GAME_OVER_MENU", NULL, &max_size);
	if (SendAndCheck(to_send, p_params->WorkerSocket) == ERR_SEND_SOCKET) {
		return ERR_SEND_SOCKET;
	}
	*/
}