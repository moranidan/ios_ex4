// HardCodedData.h

// This header consists of the constat numbers and strings in the project

#pragma once

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

// Constants -------------------------------------------------------------------

#define MAX_USER_NAME_INPUT 21 
#define MAX_MESSAGE_TYPE_LENGTH  28
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
static const int ERR_CODE_SEMAPHORE = -8;
static const int ERR_CODE_THREAD = -2;
static const int ERR_CODE_CREAT_THEARD = -3;
static const int ERR_CODE_CLOSE_THREAD = -4;
static const int ERR_CODE_MUTEX = -1;
