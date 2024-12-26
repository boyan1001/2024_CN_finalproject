#include <string>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cmath>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <map>
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
    unsigned char aes_key[32];
    unsigned char aes_iv[16];
    bool* exitFlag; // if the chatting is over
};

string userRegistration(string username);

string UserLogin(string &username);

string getNowUsername(int client_fd, unsigned char *key, unsigned char *iv);

string getIPfromDomain(string domain);

void returnMessage();

void chatting(int client_fd, string username, unsigned char *key, unsigned char *iv);

void* chatingRcvThread(void* arg);

void chatRoom(int client_fd, string me, string target, unsigned char *key, unsigned char *iv);

void resizeQueue();

void printMessage();

void printChatRoom(string username);

bool isIP(string ip);

bool sendFile(int client_fd, string file_path, string sender, string receiver, unsigned char *key, unsigned char *iv);

bool recvFile(int client_fd, string file_path, int file_size, string sender, string receiver, unsigned char *key, unsigned char *iv);