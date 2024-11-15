#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>

#define SERVER_IP "127.0.0.1"

#pragma once

using namespace std;

typedef struct{
    string username;
    string password;
}User;