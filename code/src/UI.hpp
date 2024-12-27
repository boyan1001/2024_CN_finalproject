#include <iostream>
#include <cstdlib>

#include <string>
#include <queue>

#pragma once

using namespace std;

void title(int mode);

void printError(string error);

void serverWelcome();

void clientWelcome();

string clientMainMenu(string username, int mode);

void statusMessage(string username);

string chatMenu(string username);

string outlineChatMenu(string username);

void resizeQueue(queue<string> &chatting_message);

void printMessage(queue<string> &chatting_message);

void printChatRoom(string username, queue<string> &chatting_message);