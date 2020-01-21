#pragma once

#define NUM_OF_WORKER_THREADS 2

#define TRNS_TIMEOUT  18596

#define MAX_LOOPS 3

#define SEND_STR_SIZE 35

#define SUCCESS_CODE 0

#define ERR_CODE_DEFAULT -1

#define ERR_ACCEPT_SOCKET -2

#define ERR_RECV_SOCKET -3

#define ERR_SEND_SOCKET -4

#define THREAD_TIMEOUT 1000

#define NUM_PARAMETERS 4

#define MAX_MOVE_LEN 9

typedef enum { CLIENT_CPU, CLIENT_VERSUS, CLIENT_LEADERBOARD, CLIENT_DISCONNECT } client_decision;

typedef enum { ROCK, SPOCK, PAPER, LIZARD, SCISSORS } possible_moves;

typedef struct
{
	SOCKET *MainSocket;
	HANDLE *ThreadHandles;
	SOCKET *ThreadInputs;
	HANDLE *SpareHandle;
	BOOL *Done;
} LISTEN_THREAD_params_t;

typedef struct
{
	SOCKET *WorkerSocket;
	BOOL *Done;
	BOOL *is_full;
	char *username;
} CONNECTION_THREAD_params_t;
