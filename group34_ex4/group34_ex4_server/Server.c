
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include "Server.h"


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];
HANDLE SpareHandle = NULL;		// handle for connection when we exceed the max. closed shortly after.

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//static int FindFirstUnusedThreadSlot();
static void CleanupWorkerThreads();
static HANDLE CreateThreadSimple(
	LPTHREAD_START_ROUTINE p_start_routine,
	LPVOID p_thread_parameters,
	LPDWORD p_thread_id
);

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

void MainServer(char *argv[])
{
	delete_file();	// if gemwsession.txt exists, delete it
	int Ind;
	int Loop;
	SOCKET MainSocket = INVALID_SOCKET;
	unsigned long Address;
	SOCKADDR_IN service;
	int bindRes;
	int ListenRes;
	BOOL Done = FALSE;

	HANDLE ListenThreadHandle = NULL;
	DWORD ListenThreadID;
	LISTEN_THREAD_params_t *p_lthread_params;
	p_lthread_params = (LISTEN_THREAD_params_t *)calloc(1, sizeof(LISTEN_THREAD_params_t)); // allocate memory for thread parameters
	if (NULL == p_lthread_params) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		// return_code = ERR_CODE_DEFAULT;
		goto server_cleanup_2;
	}

	// Initialize Winsock.
	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (StartupRes != NO_ERROR)
	{
		printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());
		// Tell the user that we could not find a usable WinSock DLL.                                  
		return;
	}

	/* The WinSock DLL is acceptable. Proceed. */

	// Create a socket.    
	MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (MainSocket == INVALID_SOCKET)
	{
		printf("Error at socket( ): %ld\n", WSAGetLastError());
		goto server_cleanup_1;
	}

	// Bind the socket.
   // Create a sockaddr_in object and set its values.
   // Declare variables

	Address = inet_addr(SERVER_ADDRESS_STR);
	if (Address == INADDR_NONE)
	{
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n",
			SERVER_ADDRESS_STR);
		goto server_cleanup_2;
	}

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = Address;
	service.sin_port = htons(atoi(argv[1])); //The htons function converts a u_short from host to TCP/IP network byte order 
									   //( which is big-endian ).

	// Call the bind function, passing the created socket and the sockaddr_in structure as parameters. 
	// Check for general errors.
	bindRes = bind(MainSocket, (SOCKADDR*)&service, sizeof(service));
	if (bindRes == SOCKET_ERROR)
	{
		printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Listen on the Socket.
	ListenRes = listen(MainSocket, SOMAXCONN);
	if (ListenRes == SOCKET_ERROR)
	{
		printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
		ThreadHandles[Ind] = NULL;

	p_lthread_params->ThreadHandles = ThreadHandles;
	p_lthread_params->ThreadInputs = ThreadInputs;
	p_lthread_params->SpareHandle = &SpareHandle;
	p_lthread_params->MainSocket = &MainSocket;
	p_lthread_params->Done = &Done;

	ListenThreadHandle = CreateThreadSimple(ListenThread, p_lthread_params, &ListenThreadID);	// create thread
	if (NULL == ListenThreadHandle) {	// check for errors when opening ListenThread
		printf("Error when creating thread: %d\n", GetLastError());
		goto server_cleanup_2;
	}
	// wait for "exit" to be typed
	while (1)
	{
		char keyboard[6];
		gets_s(keyboard, sizeof(keyboard)); //Reading a string from the keyboard

		if (STRINGS_ARE_EQUAL(keyboard, "exit")) {
			Done = TRUE;
			break;
		}

	}
	// close ListenThread
	DWORD wait_code;
	BOOL ret_val;
	DWORD exit_code;

	wait_code = WaitForSingleObject(ListenThreadHandle, THREAD_TIMEOUT);	// wait for thread to finish running
	if (WAIT_OBJECT_0 != wait_code)			// check for errors
	{
		ret_val = TerminateThread(ListenThreadHandle, &exit_code);
		if (FALSE == ret_val)
		{
			printf("Error when terminating thread: %d\n", GetLastError());
		}
	}
	ret_val = CloseHandle(ListenThreadHandle);
	if (FALSE == ret_val)
	{
		printf("Error when closing thread: %d\n", GetLastError());
	}

server_cleanup_3:
	CleanupWorkerThreads();

server_cleanup_2:
	free(p_lthread_params);
	if (closesocket(MainSocket) == SOCKET_ERROR)
		printf("Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());

server_cleanup_1:
	delete_file();
	if (WSACleanup() == SOCKET_ERROR)
		printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());
}


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

static void CleanupWorkerThreads()
{
	int Ind;
	BOOL ret_val;
	int exit_code;

	if (SpareHandle != NULL) {
		ret_val = CloseHandle(SpareHandle);
		if (FALSE == ret_val)
		{
			printf("Error when closing thread: %d\n", GetLastError());
		}
	}
	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] != NULL)
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], THREAD_TIMEOUT);

			if (Res == WAIT_OBJECT_0) {
				if (ThreadInputs[Ind] != NULL) {
					closesocket(ThreadInputs[Ind]) != FALSE;
				}
				ret_val = CloseHandle(ThreadHandles[Ind]);
				if (FALSE == ret_val)
				{
					printf("Error when closing thread: %d\n", GetLastError());
				}
				ThreadHandles[Ind] = NULL;
				continue;
			}
			else if (Res == WAIT_TIMEOUT) {
				
				ret_val = TerminateThread(ThreadHandles[Ind], &exit_code);
				if (FALSE == ret_val)
				{
					printf("Error when terminating thread: %d\n", GetLastError());
				}
			
				ret_val = CloseHandle(ThreadHandles[Ind]);
				if (FALSE == ret_val)
				{
				printf("Error when closing thread: %d\n", GetLastError());
				}
				if (closesocket(ThreadInputs[Ind]) != FALSE) {	
					printf("Error when closing socket\n");
				}
				ThreadHandles[Ind] = NULL;
				continue;
			}
		else
			{
				printf("Waiting for thread failed. Ending program\n");
				return;
			}
		}
	}
}

static HANDLE CreateThreadSimple(
	LPTHREAD_START_ROUTINE p_start_routine,
	LPVOID p_thread_parameters,
	LPDWORD p_thread_id
) {
	HANDLE thread_handle;

	if (NULL == p_start_routine)
	{
		printf("Error when creating a thread\n");
		printf("Received null pointer, start routine\n");
		return NULL;		// return NULL, error will be caught in "create_threads"
	}

	if (NULL == p_thread_id)
	{
		printf("Error when creating a thread\n");
		printf("Received null pointer, thread id\n");
		return NULL;		// return NULL, error will be caught in "create_threads"
	}

	thread_handle = CreateThread(
		NULL,                /*  default security attributes */
		0,                   /*  use default stack size */
		p_start_routine,     /*  thread function */
		p_thread_parameters, /*  argument to thread function */
		0,                   /*  use default creation flags */
		p_thread_id);        /*  returns the thread identifier */

	if (NULL == thread_handle)
	{
		printf("Couldn't create thread\n");
	}

	return thread_handle;
}