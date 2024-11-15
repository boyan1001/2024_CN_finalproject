#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>

#pragma once

using namespace std;

typedef struct{
    string username;
    string password;
}User;

string userRegistration();