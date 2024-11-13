#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#define CLIENT_PORT 9002
#define SERVER_PORT 5000
#define SERVER_IP "0.0.0.0"

#pragma once

using namespace std;