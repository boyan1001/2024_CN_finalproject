#include <iostream>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <queue>
#include <mutex>

#include <string>
#include <vector>

// SDL2
#include <SDL2/SDL.h>

// Socket (POSIX)
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// pthread
#include <pthread.h>

#include <atomic>
#include <queue>
#include <string>

#pragma once

using namespace std;

void audioPlaybackCallback(void *userdata, Uint8 *stream, int len);

void audioCaptureCallback(void *userdata, Uint8 *stream, int len);

void *audioRecvThread(void *arg);

bool receiverAudio(int sender_fd, string sender, queue<string> &chatting_message);

bool senderAudio(int receiver_fd, string target);

bool handleAudio(int sender_fd, int receiver_fd);
