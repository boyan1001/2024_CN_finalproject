#include "client.hpp"
#include "src/UI.hpp"
#include "src/crypt.hpp"
#include "src/file.hpp"

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

string getNowUsername(int client_fd, unsigned char *key, unsigned char *iv)
{
    // cout << "[\033[1;33mInfo\033[0m][\033[1mAES Key\033[0m] ";
    // for(int i = 0; i < 32; i++){
    //     printf("%02x", key[i]);
    // }
    // cout << endl;
    // cout << "[\033[1;33mInfo\033[0m][\033[1mAES IV\033[0m] ";
    // for(int i = 0; i < 16; i++){
    //     printf("%02x", iv[i]);
    // }
    // cout << endl;

    char buffer[4096] = {0}; // 確保 buffer 被初始化
    string snd_message = "[Info] Get Username";
    vector<unsigned char> snd_message_cipher = encrypt(snd_message, key, iv);
    // cout << "snd_message_cipher size: " << snd_message_cipher.size() << endl;

    // 傳送加密訊息
    send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

    // 接收加密的回覆
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    // cout << "bytes_received: " << bytes_received << endl;
    if (bytes_received <= 0)
    {
        cerr << "Failed to receive data from the client" << endl;
        return "";
    }

    // 解密收到的訊息
    vector<unsigned char> rcv_message_cipher = vector<unsigned char>(buffer, buffer + bytes_received);
    // rcv_message_cipher = rcv_message_cipher.substr(0, rcv_message_cipher.find('\0')); // 去除沒用的字元
    // cout << "rcv_message_cipher: " << rcv_message_cipher << endl;
    // cout << "rcv_message_cipher size: " << rcv_message_cipher.size() << endl;
    string rcv_message = decrypt(rcv_message_cipher, key, iv);
    // cout << "rcv_message: " << rcv_message << endl;
    if (rcv_message.empty())
    {
        cerr << "Failed to decrypt message from client" << endl;
        return "";
    }

    // 判斷是否有錯誤訊息
    if (rcv_message.find("[\033[1;31mError\033[0m]") != string::npos)
    {
        return "";
    }

    // 從解密後的訊息中提取用戶名
    size_t space_pos = rcv_message.find(" ");
    if (space_pos == string::npos)
    {
        cerr << "Invalid message format, no space found." << endl;
        return "";
    }

    string username = rcv_message.substr(space_pos + 1);
    return username;
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
    // cout << "[\033[1;36mStatus\033[0m] Closing the connection" << endl;
    cout << "Exiting the program..." << endl;
    return;
}

void chatting(int client_fd, string username, unsigned char *key, unsigned char *iv)
{
    system("clear");
    title(1);

    // variables
    string snd_message;
    vector<unsigned char> snd_message_cipher;
    int bytes_received;
    string rcv_message;
    vector<unsigned char> rcv_message_cipher;
    string target_username = chatMenu(username);
    cout << endl;
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    // check if target online
    snd_message = "[Info][Online] " + target_username;
    snd_message_cipher = encrypt(snd_message, key, iv);
    send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

    bytes_received = recv(client_fd, buffer, 4096, 0);
    rcv_message_cipher = vector<unsigned char>(buffer, buffer + bytes_received);
    rcv_message = decrypt(rcv_message_cipher, key, iv);

    if (rcv_message.find("[\033[1;31mError\033[0m]") != string::npos)
    {
        cout << target_username << " is \033[1;31moffline\033[0m. Please wait for it online." << endl;
        cout << endl;
        cout << ">>> Press ENTER to continue" << endl;
        cin.get();
        return;
    }

    // wait for connection
    snd_message = "[Chatting][Invite] " + username + ":" + target_username;
    snd_message_cipher = encrypt(snd_message, key, iv);
    send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(client_fd, buffer, 4096, 0);
    rcv_message_cipher = vector<unsigned char>(buffer, buffer + bytes_received);
    rcv_message = decrypt(rcv_message_cipher, key, iv);

    if (rcv_message.find("[Chatting][Start]") != string::npos)
    {
        cout << endl;
        cout << "Now you can chatting with \033[1m" << target_username << "\033[0m" << endl;
        cout << ">>> Press ENTER to continue" << endl;
        cin.get();
    }
    else
    {
        cout << endl;
        cout << "Waiting \033[1m" << target_username << "\033[0m to accept your invitation..." << endl;

        while (1)
        {
            int bytes_received = recv(client_fd, buffer, 4096, 0);
            rcv_message_cipher = vector<unsigned char>(buffer, buffer + bytes_received);
            // rcv_message_cipher = rcv_message_cipher.substr(0, rcv_message_cipher.find('\0')); // remove useless characters
            rcv_message = decrypt(rcv_message_cipher, key, iv);

            if (rcv_message.find("[Chatting][Start]") != string::npos)
            {
                cout << endl;
                cout << "Now you can chatting with \033[1m" << target_username << "\033[0m" << endl;
                cout << ">>> Press ENTER to continue" << endl;
                cin.get();
                break;
            }
        }
    }

    // Enter the chatroom
    chatRoom(client_fd, username, target_username, key, iv);

    return;
}

void *chatingRcvThread(void *arg)
{
    ChattingArgs *args = (ChattingArgs *)arg;
    string me = args->me;
    string target = args->target;
    int client_fd = args->client_fd;
    unsigned char *key = args->aes_key;
    unsigned char *iv = args->aes_iv;
    bool *exitFlag = args->exitFlag;

    string rcv_message;
    vector<unsigned char> rcv_message_cipher;
    char buffer[4096];
    

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        int bytes_received = recv(client_fd, buffer, 4096, 0);
        rcv_message_cipher = vector<unsigned char>(buffer, buffer + bytes_received);
        // rcv_message_cipher = rcv_message_cipher.substr(0, rcv_message_cipher.find('\0')); // remove useless characters
        rcv_message = decrypt(rcv_message_cipher, key, iv);

        cout << "rcv_message: " << rcv_message << endl;

        if (rcv_message.find("[Chatting][Exit]") != string::npos)
        {
            printChatRoom(me);
            cout << endl;
            cout << "\033[1;33m" << target << "\033[0;33m has left the chatroom\033[0m" << endl;
            cout << endl;
            cout << ">>> Press ENTER to continue" << endl;

            // exit the chatroom
            *exitFlag = true;
            break;
        }
        else if(rcv_message.find("[Chatting][File]") != string::npos){
            cout << "rcv_message: " << rcv_message << endl;
            string file_name;
            string file_path;
            string sender;
            string receiver;
            int file_size;
            string content;
            // [Chatting][File] sender:receiver:file_name:start
            regex pattern(R"(\[Chatting\]\[File\]\s+(.+):(.+):(.+):(.+):(.+))");
            smatch match;
            if(regex_search(rcv_message, match, pattern)){
                sender = match[1];
                receiver = match[2];
                file_name = match[3];
                file_path = "./data/client/" + me + "/file/" + file_name;
                file_size = stoi(match[4]);
                content = match[5];
            }else{
                printError("Invalid file message");
                continue;
            }

            // print message
            printChatRoom(me);
            cout << endl;
            cout << "\033[1;33m" + target + "\033[0;33m is sendding a file to you, please don't send message or leave...\033[0m" << endl;
        
            // receive file
            if(recvFile(client_fd, file_path, file_size, sender, receiver, key, iv)){
                chatting_message.push("\033[1m" + sender + "\033[0m: send a file (\033[1;36m" + file_name + "\033[0m)");
            }else{
                chatting_message.push("\033[1;31mError\033[0;31m: cannot receive a file (\033[1;31m" + file_name + "\033[0;31m)\033[0m");
            }

            printChatRoom(me);
            cout << endl;
            cout << "You: " << flush;
        }
        else if (rcv_message.find("[Chatting][Message]") != string::npos)
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
                cout << endl;
                cout << "You: " << flush;
            }
        }
    }

    pthread_exit(nullptr);
}

void chatRoom(int client_fd, string me, string target, unsigned char *key, unsigned char *iv)
{
    bool exit = false;

    ChattingArgs *args = new ChattingArgs();
    args->me = me;
    args->target = target;
    args->client_fd = client_fd;
    args->exitFlag = &exit;
    memcpy(args->aes_key, key, 32);
    memcpy(args->aes_iv, iv, 16);

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
        vector<unsigned char> snd_message_cipher;
        string rcv_message;
        vector<unsigned char> rcv_message_cipher;
        char buffer[4096];

        // print the chatroom
        if (!exit)
        {
            printChatRoom(me);
            cout << endl;
            cout << "You: " << flush;
        }
        getline(cin, snd_message);

        // send message
        if (snd_message == "<exit>")
        {
            snd_message = "[Chatting][Exit] " + me + ":" + target;
            snd_message_cipher = encrypt(snd_message, key, iv);
            send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

            // exit the chatroom
            exit = true;
            break;
        }
        else if (snd_message.empty())
        {
            continue;
        }
        else if(snd_message.find("<file>") != string::npos){
            // send file
            regex pattern(R"(<file>\s+(.+))");
            smatch match;
            string file_path;
            string file_name;
            if(regex_search(snd_message, match, pattern)){
                file_name = match[1];
            }else{
                printError("Invalid file path");
                continue;
            }
            file_path = "./data/client/" + me + "/file/" + file_name;

            printChatRoom(me);
            cout << endl;
            cout << "\033[1;33m"+ file_name + "\033[0;33m is sendding, please don't send message or leave...\033[0m" << endl;

            if(sendFile(client_fd, file_path, me, target, key, iv)){
                chatting_message.push("\033[1;36mYou\033[0;36m: send a file (\033[1;36m" + file_name + "\033[0;36m)\033[0m");
            }else{
                chatting_message.push("\033[1;31mError\033[0;31m: cannot send a file (\033[1;31m" + file_name + "\033[0;31m)\033[0m");
            }

        }
        else
        {
            // send message
            chatting_message.push("\033[1;36mYou\033[0;36m: " + snd_message + "\033[0m");
            snd_message = "[Chatting][Message] " + me + ":" + target + ":" + snd_message;
            snd_message_cipher = encrypt(snd_message, key, iv);
            send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);
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

bool isIP(string ip)
{
    regex pattern(R"(\d+\.\d+\.\d+\.\d+)");
    return regex_match(ip, pattern);
}
