#include <iostream>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

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

struct ClientArgs{
    int client_fd;
    unsigned char aes_key[32];
    unsigned char aes_iv[16];
};

// gloable variables
static unordered_map<string, int> name_to_fd;
static unordered_set<string> waiting_for_chatting;

// string getLocalAddress();

string getIPAddress();

// void bindAndListen(int server_fd, int server_port, string interface);

// int acceptClient(int server_fd, struct sockaddr_in accept_address, socklen_t accept_addressSize);

void *handleClient(void* arg);

string userRegistration(string username, unsigned char *key, unsigned char *iv);

string UserLogin(string rcv_message, User &login_user, int client_fd, unsigned char *key, unsigned char *iv);

string UserLogout(User &login_user, int client_fd, unsigned char *key, unsigned char *iv);

string getInfo(string rcv_message, User &login_user, unsigned char *key, unsigned char *iv);

void chatting(string snd_message, int client_fd, User login_user, unsigned char *key, unsigned char *iv);