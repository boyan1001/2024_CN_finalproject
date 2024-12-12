#include <iostream>
#include <string>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <map>
#include <string>
#include <vector>

#pragma once

using namespace std;

typedef struct{
    string username;
    string password;
}User;

// string getLocalAddress();

string getIPAddress();

void bindAndListen(int server_fd, int server_port, string interface);

int acceptClient(int server_fd, struct sockaddr_in accept_address, socklen_t accept_addressSize);

void *handleClient(void* client_fd);