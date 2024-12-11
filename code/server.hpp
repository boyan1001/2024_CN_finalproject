#include <iostream>
#include <string>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <map>
#include <string>

#pragma once

using namespace std;

typedef struct{
    string username;
    string password;
}User;

string getLocalAddress();