#include <iostream>
#include <string>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <regex>

#define DEBUG 1

#pragma once

using namespace std;

typedef struct{
    string username;
    string password;
}User;

// string getLocalAddress();

string getIPAddress();

// void bindAndListen(int server_fd, int server_port, string interface);

// int acceptClient(int server_fd, struct sockaddr_in accept_address, socklen_t accept_addressSize);

void *handleClient(void* new_fd);

string userRegistration(string username);

string UserLogin(string rcv_message, User &login_user, int client_fd);

string UserLogout(User &login_user, int client_fd);

string getInfo(string rcv_message, User &login_user);

void chatting(string snd_message, int client_fd, User login_user);