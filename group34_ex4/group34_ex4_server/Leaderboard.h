// Leaderboard.h

// This header consists the function declerations for all the functions used in Leaderboard.c, and their documentaion

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef LEADERBOARD_H
#define LEADERBOARD_H

// Includes --------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "HardCodedData.h"
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "Shlwapi.h"


// Function Declarations -------------------------------------------------------

char* read_all_file();

#endif // LEADERBOARD_H