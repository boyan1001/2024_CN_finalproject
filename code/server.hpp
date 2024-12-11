#include <iostream>
#include <string>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <map>
#include <string>
#include <vector>

#pragma once

using namespace std;

typedef struct{
    string username;
    string password;
}User;

string getLocalAddress();

vector<string> getIPAddress();

void bindAndListen(int server_fd, int server_port, string interface);