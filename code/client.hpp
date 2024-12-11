#include <string>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include <map>
#include <string>


#pragma once

using namespace std;

typedef struct{
    string username;
    string password;
}User;

string userRegistration(string username);

string UserLogin(string &username);

string getNowUsername(int client_fd);

string getIPfromDomain(string domain);