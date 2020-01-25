// HardCodedData.h

// This header consists of the constat numbers and strings in the project

#pragma once

#include "../Shared/SharedHardCodedDataAndFunctions.h"
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

// Constants -------------------------------------------------------------------

#define MAX_USER_NAME_INPUT 21 
#define MAX_MESSAGE_TYPE_LENGTH  30
#define MAX_IP_STRING_LEN 16
#define MAX_USER_LEN_INPUT 10
#define MAX_SENDSTR_FOR_CLIENT 60
static LPCTSTR MUTEX_MESSAGE_BETWEEN_THREADS_NAME = _T("group34_ex4_client_message_between_threads_mutex");  //create mutex for the message_between_threads varibel
#define MESSAGE_RECIVED_BETWEEN_THREADS "message recived"
#define USER_ASKED_TO_QUIT "user asked to quit"
#define SENT_CLIENT_VERSUS "user asked to play agains another client"
#define FAILED_CONNECTION_MENU_BEF "Failed connecting to server on"
#define FAILED_CONNECTION_MENU_AFT ""
#define CONNECTION_BREAK_MENU_BEF "Connection to server on"
#define CONNECTION_BREAK_MENU_AFT " has been lost"
#define SERVER_DENIED_MENU_BEF "Server on"
#define SERVER_DENIED_MENU_AFT " denied the connection request"


