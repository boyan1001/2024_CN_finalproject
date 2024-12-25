#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cmath>

#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/rand.h>
#include <openssl/core_names.h>
#include <openssl/core.h>


#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <regex>

#pragma once

using namespace std;

void generateAESKeyAndIV(unsigned char *key, unsigned char *iv);

vector<unsigned char> encrypt(const string &plain_text, const unsigned char *key, const unsigned char *iv);

string decrypt(const vector<unsigned char> &origin_cipher_text, const unsigned char *key, const unsigned char *iv);