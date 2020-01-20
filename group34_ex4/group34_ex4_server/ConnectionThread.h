#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef CONNECTION_THREAD_H
#define CONNECTION_THREAD_H

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "HardCodedData.h"
#include <time.h>
#include <stdlib.h>
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"




DWORD ServiceThread(LPVOID lpParam);
int ReceiveAndCheck(char **AcceptedStr, SOCKET *socket);
int SendAndCheck(char *to_send, SOCKET *socket);
int approve_client(CONNECTION_THREAD_params_t *p_params);
int main_manu(CONNECTION_THREAD_params_t *p_params, int *main_manu_decision);
int cpu_game(CONNECTION_THREAD_params_t *p_params, BOOL *replay);
BOOL player_win(int player_choice, int cpu_choice);
int name_to_number(char *move_name);
void number_to_name(int num, char *move_name);
int check_if_replay(CONNECTION_THREAD_params_t *p_params, BOOL *replay);


#endif // CONNECTION_THREAD_H