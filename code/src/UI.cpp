#include "UI.hpp"

// mode: 0 for server, 1 for client
void title(int mode)
{
    cout << "--------------------------------------------------" << endl;
    cout << "Computer Network Socket Programming Project (Phase 2)" << endl;
    cout << "Student ID: 41247020S" << endl;

    if (mode == 0)
    {
        cout << "Mode: \033[1mServer\033[0m" << endl;
    }
    else
    {
        cout << "Mode: \033[1mClient\033[0m" << endl;
    }

    cout << "--------------------------------------------------" << endl;
    return;
}

void printError(string error)
{
    cout << "[\033[1;31mError\033[0m] " << error << endl;
    return;
}

void serverWelcome()
{
    cout << "Welcome to \033[1mServer\033[0m mode" << endl
         << endl;
    cout << "Press ENTER to start the server" << endl;
    cin.get();
    return;
}

void clientWelcome()
{
    cout << "Welcome to \033[1mClient\033[0m mode" << endl
         << endl;
    cout << "Press ENTER to start the client" << endl;
    cin.get();
    return;
}

string clientMainMenu(string username, int mode)
{
    string choice;

    statusMessage(username);
    cout << endl;

    cout << "Press the key to choose the operation you want to do:" << endl;
    cout << "   0: User Registration" << endl;

    if (mode == 0)
    {
        cout << "   1: User Login" << endl;
    }
    else
    {
        cout << "   2: User Logout" << endl;
        cout << "   3: Chatting" << endl;
    }
    cout << "   R: Exit" << endl;
    cout << endl;
    cout << "Your choice: ";

    getline(cin, choice);

    cout << endl;
    return choice;
}

void statusMessage(string username)
{
    if (username.empty())
    {
        cout << "[\033[1mUser Account Status\033[0m] \033[1;31mNot Logined\033[0m" << endl;
        return;
    }
    cout << "[\033[1mUser Account Status\033[0m] \033[1;32mLogined\033[0m" << endl;
    cout << "[\033[1mUsername\033[0m] " << username << endl;
    return;
}

string chatMenu(string username)
{
    statusMessage(username);
    cout << endl;
    cout << "Enter the user name you want to chat: " << endl;
    cout << ">>> ";
    getline(cin, username);
    return username;
}

string outlineChatMenu(string username)
{
    statusMessage(username);
    cout << endl;
    cout << "Do you want to wait for the user to be ready or leave?" << endl;
    cout << "   1. Wait" << endl;
    cout << "   2. Leave" << endl;
    cout << ">>> Your choice: ";
    string choice;
    getline(cin, choice);
    return choice;
}

void resizeQueue(queue<string> &chatting_message)
{
    while (chatting_message.size() > 40)
    {
        chatting_message.pop();
    }
    return;
}

void printMessage(queue<string> &chatting_message)
{
    queue temp = chatting_message;
    while (!temp.empty())
    {
        cout << temp.front() << endl;
        temp.pop();
    }
    return;
}

void printChatRoom(string username, queue<string> &chatting_message)
{
    system("clear");
    title(1);
    statusMessage(username);
    cout << endl;

    resizeQueue(chatting_message);
    printMessage(chatting_message);
    return;
}