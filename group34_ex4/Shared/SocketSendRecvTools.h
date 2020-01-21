/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/* 
 This file was written for instruction purposes for the 
 course "Introduction to Systems Programming" at Tel-Aviv
 University, School of Electrical Engineering, Winter 2011, 
 by Amnon Drory.
*/
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#ifndef SOCKET_SEND_RECV_TOOLS_H
#define SOCKET_SEND_RECV_TOOLS_H

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

typedef enum { TRNS_FAILED, TRNS_DISCONNECTED, TRNS_SUCCEEDED, TRNS_TIMEOUT } TransferResult_t;

#define MAX_MESSAGE_TYPE_LENGTH  28

/**
 * SendBuffer() uses a socket to send a buffer.
 *
 * Accepts:
 * -------
 * Buffer - the buffer containing the data to be sent.
 * BytesToSend - the number of bytes from the Buffer to send.
 * sd - the socket used for communication.
 *
 * Returns:
 * -------
 * TRNS_SUCCEEDED - if sending succeeded
 * TRNS_FAILED - otherwise
 */
TransferResult_t SendBuffer( const char* Buffer, int BytesToSend, SOCKET sd );

/**
 * SendString() uses a socket to send a string.
 * Str - the string to send. 
 * sd - the socket used for communication.
 */ 
TransferResult_t SendString( const char *Str, SOCKET sd );

/**
 * Accepts:
 * -------
 * ReceiveBuffer() uses a socket to receive a buffer.
 * OutputBuffer - pointer to a buffer into which data will be written
 * OutputBufferSize - size in bytes of Output Buffer
 * BytesReceivedPtr - output parameter. if function returns TRNS_SUCCEEDED, then this 
 *					  will point at an int containing the number of bytes received.
 * sd - the socket used for communication.
 *
 * Returns:
 * -------
 * TRNS_SUCCEEDED - if receiving succeeded
 * TRNS_DISCONNECTED - if the socket was disconnected
 * TRNS_FAILED - otherwise
 */ 
TransferResult_t ReceiveBuffer( char* OutputBuffer, int RemainingBytesToReceive, SOCKET sd );

/**
 * ReceiveString() uses a socket to receive a string, and stores it in dynamic memory.
 * 
 * Accepts:
 * -------
 * OutputStrPtr - a pointer to a char-pointer that is initialized to NULL, as in:
 *
 *		char *Buffer = NULL;
 *		ReceiveString( &Buffer, ___ );
 *
 * a dynamically allocated string will be created, and (*OutputStrPtr) will point to it.
 * 
 * sd - the socket used for communication.
 * 
 * Returns:
 * -------
 * TRNS_SUCCEEDED - if receiving and memory allocation succeeded
 * TRNS_DISCONNECTED - if the socket was disconnected
 * TRNS_FAILED - otherwise
 */ 
TransferResult_t ReceiveString( char** OutputStrPtr, SOCKET sd );


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//declaration on our shared functions

/*
This function create the string that wiil be send between client and server by using the targil rules 
Input:  char *send_str - a string that will contains the string that will be send' there have to be a malloc on it befor calling this func.
		char *message_type - a string cntains the message type you want to send.
		char *params[] - a list [4] of the parameters if needed, need to be inizilaized to NULL.
		int *max_send_len - nedd to send to the fun the max len of message_type + params.
Output: void
*/
void create_string_to_send(char *send_str, char *message_type, char *params[], int *max_send_len);

/*
This function parse the string that we get from the client or the server by using the targil rules
Input:  char *recv_string - a string that contains the string we get from the client or server.
		char *message_type - a string cntains the message type you want to send.
		char *params[] - a list [4] of the parameters if needed, need to be inizilaized to NULL.
Output: void
*/
void parse_recv_string(char *recv_string, char *message_type, char *params[]);

#endif // SOCKET_SEND_RECV_TOOLS_H