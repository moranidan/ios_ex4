// HardCodedData.h

// This header consists of the constat numbers and strings in the project

#pragma once

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

// Constants -------------------------------------------------------------------

#define MAX_USER_NAME_INPUT 21 
#define MAX_MESSAGE_TYPE_LENGTH  30
#define MAX_IP_STRING_LEN 16
#define MAX_USER_LEN_INPUT 10
#define MAX_SENDSTR_FOR_CLIENT 50
static LPCTSTR MUTEX_MESSAGE_BETWEEN_THREADS_NAME = _T("group34_ex4_client_message_between_threads_mutex");  //create mutex for the message_between_threads varibel
//typedef enum { CLIENT_REQUEST, CLIENT_MAIN_MENU, CLIENT_CPU, CLIENT_VERSUS, CLIENT_LEADERBOARD, CLIENT_PLAYER_MOVE,CLIENT_REPLAY, CLIENT_REFRESH, CLIENT_DISCONNECT };
//typedef enum { SERVER_MAIN_MENU, SERVER_APPROVED, SERVER_DENIED, SERVER_INVITE, SERVER_PLAYER_MOVE_REQUEST, SERVER_GAME_RESULTS, SERVER_GAME_OVER_MENU, SERVER_OPPONENT_QUIT, SERVER_NO_OPPONENTS, SERVER_LEADERBOARD, SERVER_LEADERBORAD_MENU };
#define MESSAGE_RECIVED_BETWEEN_THREADS "message recived"
#define USER_ASKED_TO_QUIT "user asked to quit"
#define SENT_CLIENT_VERSUS "user asked to play agains another client"
#define FAILED_CONNECTION_MENU_BEF "Failed connecting to server on"
#define FAILED_CONNECTION_MENU_AFT ""
#define CONNECTION_BREAK_MENU_BEF "Connection to server on"
#define CONNECTION_BREAK_MENU_AFT " has been lost."
#define SERVER_DENIED_MENU_BEF "Server on"
#define SERVER_DENIED_MENU_AFT " denied the connection request."
/*
#define MAX_LINE_LENGTH 52    //max lenght of line in names and room file
#define LEN_FILE_NAME_RESIDENTS_NAMES 11   // the len of "/names.txt"
#define MAX_ROOM_NUM 5
#define MAX_RESIDENT_NUM 15
#define THREAD_TIMEOUT_IN_MILLISECONDS 5000
#define BRUTAL_TERMINATION_CODE 0x55
#define BOOKLOG_FILE_ADDRESS_LEN 14  //the len of "/roomlog.txt"
#define FILE_NAME_RESIDENTS_NAMES "\\names.txt"
#define FILE_NAME_ROOMS_NAMES "\\rooms.txt"
#define FILE_NAME_BOOK_LOG "\\roomLog.txt"
static LPCTSTR MUTEX_DAYS_NAME = _T("moed_bet_inn_days_mutex");  //create mutex for the days varibel
static LPCTSTR MUTEX_ROOMLOG_FILE_NAME = _T("moed_bet_inn_roomlog_file_mutex"); // create mutex for the pointer to roomlog file
static LPCTSTR MUTEX_EXIT_RESIDENTS = _T("moed_bet_inn_exit_residents_mutex");  //create mutex for the exit_residents varible
*/
static const int SUCCESS_CODE = 0;
static const int ERR_CODE_NOT_ENOUGH_ARGUMENTS = -5;
static const int ERR_CODE_ALLOCCING_MEMORY = -6;
static const int ERR_CODE_SOCKET = -7;
static const int ERR_CODE_TIMEOUT = -8;
static const int ERR_CODE_THREAD = -2;
static const int ERR_CODE_CREAT_THEARD = -3;
static const int ERR_CODE_CLOSE_THREAD = -4;
static const int ERR_CODE_MUTEX = -1;
static const int ERR_CODE_CONNECTION = -10;


