// group34_ex4_client.h

// This header consists the structs and function declerations for all the functions used in the project, and their documentation.

#pragma once

#include "HardCodedData.h"
#include <windows.h>
#include <math.h>
#include <stdio.h>

//function Declaration ----------------------------------------------------------------------------------
int connecting_to_server(unsigned long *server_ip, int server_port, void *p_server_socket);

