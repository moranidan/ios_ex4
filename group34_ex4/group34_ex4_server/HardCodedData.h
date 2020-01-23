#pragma once

#include <tchar.h>

#define NUM_OF_WORKER_THREADS 2

#define MAX_LOOPS 3

#define SEND_STR_SIZE 35

#define SUCCESS_CODE 0

#define ERR_CODE_DEFAULT -1

#define ERR_ACCEPT_SOCKET -2

#define ERR_RECV_SOCKET -3

#define ERR_SEND_SOCKET -4

#define ERR_CODE_FILE -5

#define ERR_CODE_EVENT -6

#define ERR_CODE_MUTEX -7

#define THREAD_TIMEOUT 1000

#define RESPONSE_TIMEOUT 15000

#define NUM_PARAMETERS 4

#define MAX_MOVE_LEN 9

#define MAX_USERNAME_LEN 21

#define MAX_RESULTS_LENGTH 100

#define GAME_SESSION_PATH "GameSession.txt"

static LPCTSTR PLAYER1_EVENT_NAME = _T("Player1event");

static LPCTSTR PLAYER2_EVENT_NAME = _T("Player2event");

static LPCTSTR FILE_MUTEX_NAME = _T("filemutex_ConnectionThread");

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
