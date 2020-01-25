// ListenThread.c

// This module provides the functions and logic of the listening-to-new-connections thread
// Listen thread is the thread that accepts new connections and creates their threads

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// Includes --------------------------------------------------------------------

#include "ListenThread.h"

// Function Definitions --------------------------------------------------------

DWORD WINAPI ListenThread(LPVOID lpParam) {
	LISTEN_THREAD_params_t *p_params;			// pointer for the parameters
	if (NULL == lpParam) {					// check if NULL was received instead of parameter
		printf("Error declaring parameters in ListenThread");
		return ERR_CODE_DEFAULT;
	}
	p_params = (LISTEN_THREAD_params_t *)lpParam;	// cast the parameters as 'LISTEN_THREAD_params_t' struct we declared
	int Ind;
	BOOL is_full = FALSE;

	CONNECTION_THREAD_params_t *p_cthread_params;
	p_cthread_params = (CONNECTION_THREAD_params_t *)calloc(1, sizeof(CONNECTION_THREAD_params_t)); // allocate memory for thread parameters
	if (NULL == p_cthread_params) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return ERR_CODE_DEFAULT;
	}

	while (!*(p_params->Done)) {
		SOCKET AcceptSocket = accept(*(p_params->MainSocket), NULL, NULL);
		if (AcceptSocket == INVALID_SOCKET)
		{
			printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
			return ERR_ACCEPT_SOCKET;
		}


		Ind = FindFirstUnusedThreadSlot(p_params->ThreadHandles);

		if (Ind == NUM_OF_WORKER_THREADS) //no slot is available
		{
			is_full = TRUE;

			p_cthread_params->WorkerSocket = AcceptSocket;
			p_cthread_params->Done = p_params->Done;
			p_cthread_params->is_full = &is_full;
			p_params->SpareHandle = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)ServiceThread,
				p_cthread_params,
				0,
				NULL
			);
		}
		else
		{
			p_params->ThreadInputs[Ind] = AcceptSocket; // shallow copy: don't close 
														// AcceptSocket, instead close 
														// ThreadInputs[Ind] when the
														// time comes.

			p_cthread_params->WorkerSocket = (p_params->ThreadInputs[Ind]);
			p_cthread_params->Done = p_params->Done;
			p_cthread_params->is_full = &is_full;
			p_params->ThreadHandles[Ind] = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)ServiceThread,
				p_cthread_params,
				0,
				NULL
			);
		}
	}
	free(p_cthread_params);
}


static int FindFirstUnusedThreadSlot(HANDLE *ThreadHandles)
{
	int Ind;

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] == NULL)
			break;
		else
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], 0);

			if (Res == WAIT_OBJECT_0) // this thread finished running
			{
				CloseHandle(ThreadHandles[Ind]);
				ThreadHandles[Ind] = NULL;
				break;
			}
		}
	}

	return Ind;
}