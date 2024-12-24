#include "client.hpp"
#include "src/UI.hpp"

static queue<string> chatting_message;

string userRegistration(string username)
{
    User account;
    string message;

    statusMessage(username);
    cout << endl;

    cout << "Please enter your \033[1musername\033[0m: ";
    getline(cin, account.username);
    cout << "Please enter your \033[1mpassword\033[0m: ";
    getline(cin, account.password);
    cout << endl;

    if (account.username.empty() || account.password.empty())
    {
        printError("Username or password cannot be empty");
        return "";
    }

    if (account.username.find(":") != string::npos || account.password.find(":") != string::npos || account.username.find(" ") != string::npos || account.password.find(" ") != string::npos)
    {
        printError("Username or password cannot contain ':' and space");
        return "";
    }

    message = "[User Registration] " + account.username + ":" + account.password;
    return message;
}

string UserLogin(string &username)
{
    User account;
    string message;

    statusMessage(username);
    cout << endl;

    cout << "Please enter your \033[1musername\033[0m: ";
    getline(cin, account.username);
    cout << "Please enter your \033[1mpassword\033[0m: ";
    getline(cin, account.password);
    cout << endl;

    if (account.username.empty() || account.password.empty())
    {
        printError("Username or password cannot be empty");
        return "";
    }

    if (account.username.find(":") != string::npos || account.password.find(":") != string::npos || account.username.find(" ") != string::npos || account.password.find(" ") != string::npos)
    {
        printError("Username or password cannot contain ':' and space");
        return "";
    }

    message = "[User Login] " + account.username + ":" + account.password;

    return message;
}

string getNowUsername(int client_fd)
{
    char buffer[4096];
    string client_message = "[Info] Get Username";
    send(client_fd, client_message.c_str(), client_message.size() + 1, 0);
    int bytes_received = recv(client_fd, buffer, 4096, 0);
    if (bytes_received < 0)
    {
        printError("Receiving data from the server");
        return "";
    }
    if (string(buffer, 0, bytes_received).find("[\033[1;31mError\033[0m]") != string::npos)
    {
        return "";
    }
    else
    {
        string username = string(buffer, 0, bytes_received);
        username = username.substr(username.find(" ") + 1);
        return string(buffer, 0, bytes_received).substr(string(buffer, 0, bytes_received).find(" ") + 1);
    }
}

string getIPfromDomain(string domain)
{
    struct hostent *host;
    struct in_addr **addr_list;
    string ip;

    if ((host = gethostbyname(domain.c_str())) == NULL)
    {
        printError("Getting IP from domain");
        return "";
    }

    addr_list = (struct in_addr **)host->h_addr_list;
    for (int i = 0; addr_list[i] != NULL; i++)
    {
        ip = inet_ntoa(*addr_list[i]);
    }

    return ip;
}

void returnMessage()
{
    cout << "[\033[1;36mStatus\033[0m] Closing the connection" << endl;
    cout << "Exiting the program..." << endl;
    return;
}

void chatting(int client_fd, string username)
{
    system("clear");
    title(1);

    // variables
    string snd_message;
    int bytes_received;
    string rcv_message;
    string target_username = chatMenu(username);
    cout << endl;
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    // check if target online
    snd_message = "[Info][Online] " + target_username;
    send(client_fd, snd_message.c_str(), snd_message.size() + 1, 0);
    bytes_received = recv(client_fd, buffer, 4096, 0);
    rcv_message = string(buffer, 0, bytes_received);

    if (rcv_message.find("[\033[1;31mError\033[0m]") != string::npos)
    {
        cout << target_username << " is \033[1;31moffline\033[0m. Please wait for it online." << endl
             << endl;
        cout << ">>> Press ENTER to continue" << endl;
        cin.get();
        return;
    }

    // wait for connection
    snd_message = "[Chatting][Invite] " + username + ":" + target_username;
    send(client_fd, snd_message.c_str(), snd_message.size() + 1, 0);

    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(client_fd, buffer, 4096, 0);
    rcv_message = string(buffer, 0, bytes_received);

    if (rcv_message.find("[Chatting][Start]") != string::npos)
    {
        cout << endl
             << "Now you can chatting with \033[1m" << target_username << "\033[0m" << endl;
        cout << ">>> Press ENTER to continue" << endl;
        cin.get();
    }
    else
    {
        cout << endl
             << "Waiting \033[1m" << target_username << "\033[0m to accept your invitation..." << endl;

        while (1)
        {
            int bytes_received = recv(client_fd, buffer, 4096, 0);
            string rcv_message = string(buffer, 0, bytes_received);

            if (rcv_message.find("[Chatting][Start]") != string::npos)
            {
                cout << endl
                     << "Now you can chatting with \033[1m" << target_username << "\033[0m" << endl;
                cout << ">>> Press ENTER to continue" << endl;
                cin.get();
                break;
            }
        }
    }

    // Enter the chatroom
    chatRoom(client_fd, username, target_username);

    return;
}

void *chatingRcvThread(void *arg)
{
    ChattingArgs *args = (ChattingArgs *)arg;
    string me = args->me;
    string target = args->target;
    int client_fd = args->client_fd;
    bool *exitFlag = args->exitFlag;

    string rcv_message;
    char buffer[4096];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        int bytes_received = recv(client_fd, buffer, 4096, 0);
        rcv_message = string(buffer, 0, bytes_received);

        if (rcv_message.find("[Chatting][Exit]") != string::npos)
        {
            printChatRoom(me);
            cout << endl
                 << "\033[1m" << target << "\033[0m has left the chatroom" << endl
                 << endl;
            cout << ">>> Press ENTER to continue" << endl;

            // exit the chatroom
            *exitFlag = true;
            break;
        }
        else
        {
            // [Chatting][Message] sender:receiver:message
            regex pattern(R"(\[Chatting\]\[Message\]\s+(.+):(.+):(.+))");
            smatch match;
            if (regex_search(rcv_message, match, pattern))
            {
                string sender = match[1];
                string message = match[3];
                chatting_message.push("\033[1m" + sender + "\033[0m: " + message);

                // print the message
                printChatRoom(me);
                cout << endl
                     << "You: " << flush;
            }
        }
    }

    pthread_exit(nullptr);
}

void chatRoom(int client_fd, string me, string target)
{
    bool exit = false;

    ChattingArgs *args = new ChattingArgs();
    args->me = me;
    args->target = target;
    args->client_fd = client_fd;
    args->exitFlag = &exit;

    // open rcv thread
    pthread_t rcv_thread;
    if (pthread_create(&rcv_thread, NULL, chatingRcvThread, (void *)args) < 0)
    {
        printError("Creating a thread");
        return;
    }

    while (1)
    {
        if (exit)
        {
            break;
        }
        string snd_message;
        char buffer[4096];

        // print the chatroom
        if (!exit)
        {
            printChatRoom(me);
            cout << endl
                 << "You: " << flush;
        }
        getline(cin, snd_message);

        // send message
        if (snd_message == "<exit>")
        {
            snd_message = "[Chatting][Exit] " + me + ":" + target;
            send(client_fd, snd_message.c_str(), snd_message.size() + 1, 0);

            // exit the chatroom
            exit = true;
            break;
        }
        else if (snd_message.empty())
        {
            continue;
        }
        else
        {
            // send message
            chatting_message.push("\033[1;36mYou\033[0;36m: " + snd_message + "\033[0m");
            snd_message = "[Chatting][Message] " + me + ":" + target + ":" + snd_message;
            send(client_fd, snd_message.c_str(), snd_message.size() + 1, 0);
        }
    }

    pthread_join(rcv_thread, NULL);

    chatting_message = queue<string>();
    delete args;
    return;
}

void resizeQueue()
{
    while (chatting_message.size() > 40)
    {
        chatting_message.pop();
    }
    return;
}

void printMessage()
{
    queue temp = chatting_message;
    while (!temp.empty())
    {
        cout << temp.front() << endl;
        temp.pop();
    }
    return;
}

void printChatRoom(string username)
{
    system("clear");
    title(1);
    statusMessage(username);
    cout << endl;

    resizeQueue();
    printMessage();
    return;
}