#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cmath>
#include <unistd.h>

#include <fstream>
#include <filesystem>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <map>
#include <regex>
#include <queue>
#include <vector>
#include <string>

#pragma once

using namespace std;

bool sendFile(int client_fd, string file_path, string sender, string receiver, unsigned char *key, unsigned char *iv);

bool recvFile(int client_fd, string file_path, int file_size, string sender, string receiver, unsigned char *key, unsigned char *iv);

void handleFile(int client_fd, int receiver_fd, int file_size, unsigned char *key, unsigned char *iv);

bool sendAll(int client_fd, const unsigned char *data, size_t size);

bool recvAll(int client_fd, unsigned char *data, size_t size);