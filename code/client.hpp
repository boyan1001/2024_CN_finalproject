#include <string>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#include <map>
#include <string>
#include <regex>
#include <queue>


#pragma once

using namespace std;

typedef struct{
    string username;
    string password;
}User;

struct ChattingArgs{
    string me;
    string target;
    int client_fd;
    bool* exitFlag; // if the chatting is over
};

string userRegistration(string username);

string UserLogin(string &username);

string getNowUsername(int client_fd);

string getIPfromDomain(string domain);

void returnMessage();

void chatting(int client_fd, string username);

void* chatingRcvThread(void* arg);

void chatRoom(int client_fd, string me, string target);

void resizeQueue();

void printMessage();

void printChatRoom(string username);