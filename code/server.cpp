#include "server.hpp"
#include "src/UI.hpp"
#include "src/crypt.hpp"
#include "src/file.hpp"

// gloable variables
static unordered_map<string, int> name_to_fd;
static unordered_set<string> waiting_for_chatting;
static unordered_map<string, string> chatting_pair;

// get ip addresses from external interface to the server
string getIPAddress()
{
    struct ifaddrs *ifaddr, *ifa;
    string ip_address;

    if (getifaddrs(&ifaddr) == -1)
    {
        std::cerr << "Failed to get network interface information." << std::endl;
        return ip_address;
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == nullptr)
        {
            continue;
        }

        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *addr = reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr);
            ip_address = inet_ntoa(addr->sin_addr);
        }
    }

    freeifaddrs(ifaddr);
    return ip_address;
}

// handle client
void *handleClient(void *arg)
{
    User login_user;
    ClientArgs *args = (ClientArgs *)arg;
    int client_fd = args->client_fd;
    unsigned char *key = args->aes_key;
    unsigned char *iv = args->aes_iv;

    char buffer[4096] = {0};

    ssize_t bytes_received;

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

    // send to client AES key and IV
    send(client_fd, key, 32, 0);
    send(client_fd, iv, 16, 0);
    cout << "[\033[1mServer\033[0m] AES key and IV sent to client " << client_fd << endl;

    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0)
    {
        // cout << "bytes_received: " << bytes_received << endl;
        vector<unsigned char> rcv_message_cipher = vector<unsigned char>(buffer, buffer + bytes_received);
        // cout << "rcv_message_cipher size: " << rcv_message_cipher.size() << endl;
        // rcv_message_cipher = rcv_message_cipher.substr(0, rcv_message_cipher.find('\0')); // 去除沒用的字元
        // cout << "rcv_message_cipher: " << rcv_message_cipher << endl;
        // show aes key and iv
        // cout << "[\033[1;33mInfo\033[0m][\033[1mAES Key\033[0m] ";
        // for(int i = 0; i < 32; i++){
        //     printf("%02x", key[i]);
        // }
        // cout << endl;
        // cout << "[\033[1;33mInfo\033[0m][\033[1mAES IV\033[0m] ";
        // for(int i = 0; i < 16; i++){
        //     printf("%02x", iv[i]);
        // }
        // cout << "rcv_message_cipher size: " << rcv_message_cipher.size() << endl;
        string rcv_message = decrypt(rcv_message_cipher, key, iv);
        string snd_message;
        vector<unsigned char> snd_message_cipher;

        if (login_user.username != "")
        {
            name_to_fd[login_user.username] = client_fd;
        }

        cout << "[\033[1mClient\033[0m] " << rcv_message << endl;

        if (rcv_message.find("[Exit]") != string::npos && rcv_message.find("[Chatting]") == string::npos)
        {
            cout << "[\033[1mServer\033[0m] Client exited" << endl;
            break;
        }

        if (rcv_message.find("[User Registration]") != string::npos)
        {
            snd_message = userRegistration(rcv_message, key, iv);
        }
        else if (rcv_message.find("[User Login]") != string::npos)
        {
            snd_message = UserLogin(rcv_message, login_user, client_fd, key, iv);
        }
        else if (rcv_message.find("[User Logout]") != string::npos)
        {
            snd_message = UserLogout(login_user, client_fd, key, iv);
        }
        else if (rcv_message.find("[Chatting]") != string::npos)
        {
            // chatting
            chatting(rcv_message, client_fd, login_user, key, iv);
            continue;
        }
        else if (rcv_message.find("[Info]") != string::npos)
        {
            snd_message = getInfo(rcv_message, login_user, key, iv);
        }
        else
        {
            // send back to client a message
            cout << "[\033[1mServer\033[0m] " << rcv_message << endl;
        }

        cout << "[\033[1mServer\033[0m] " << snd_message << endl;

        snd_message_cipher = encrypt(snd_message, key, iv);
        send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

        bzero(buffer, sizeof(buffer));
    }

    if (login_user.username != "")
    {
        name_to_fd[login_user.username] = 0;
    }

    cout << "Client disconnected, socket: " << client_fd << endl;
    close(client_fd); // 關閉 client socket
    return nullptr;
}

string userRegistration(string rcv_message, unsigned char *key, unsigned char *iv)
{
    // rcv_message = "[User Registration] username:password"
    // User Registration
    // get username and pwd
    User account;
    regex pattern(R"(\[User Registration\]\s+(.+):(\w+))");
    smatch match;
    if (regex_search(rcv_message, match, pattern))
    {
        account.username = match[1];
        account.password = match[2];
    }
    else
    {
        return "[\033[1;31mError\033[0m] Invalid message format";
    }

    cout << "[\033[1;33mUser Registration\033[0m][\033[1mUsername\033[0m] " << account.username << endl;
    cout << "[\033[1;33mUser Registration\033[0m][\033[1mPassword\033[0m] " << account.password << endl;

    // find if the user already exists
    ifstream file("./data/server/account.csv");
    string line;
    bool user_exist = false;
    getline(file, line); // skip the first line
    while (getline(file, line))
    {
        if (line.find(account.username) != string::npos)
        {
            user_exist = true;
            break;
        }
    }

    // send back to client a message and regigster the user
    string message;
    if (user_exist)
    {
        message = "[\033[1;31mError\033[0m] User already exists";
    }
    else
    {
        ofstream file("./data/server/account.csv", ios::app);
        file << account.username << "," << account.password << endl;

        message = "[\033[1;32mSuccess\033[0m] User registered successfully";
    }

    file.close();
    return message;
}

string UserLogin(string rcv_message, User &login_user, int client_fd, unsigned char *key, unsigned char *iv)
{
    // User Login
    // get username and pwd
    User account;
    string message;
    regex pattern(R"(\[User Login\]\s+(.+):(\w+))");
    smatch match;

    if (regex_search(rcv_message, match, pattern))
    {
        account.username = match[1];
        account.password = match[2];
    }
    else
    {
        return "[\033[1;31mError\033[0m] Invalid message format";
    }

    if (account.username.empty() || account.password.empty())
    {
        message = "[\033[1;31mError\033[0m] Username or password cannot be empty";
        cout << "[\033[1mServer\033[0m] " << message << endl;
        return message;
    }

    if (name_to_fd[account.username] != 0)
    {
        message = "[\033[1;31mError\033[0m] A user can only login one device";
        cout << "[\033[1mServer\033[0m] " << message << endl;
        return message;
    }

    cout << "[\033[1;33mUser Login\033[0m][\033[1mUsername\033[0m] " << account.username << endl;
    cout << "[\033[1;33mUser Login\033[0m][\033[1mPassword\033[0m] " << account.password << endl;

    // find if the user exists
    ifstream file("./data/server/account.csv");
    string line;
    bool user_exist = false;
    bool login_access = false;
    getline(file, line); // skip the first line
    while (getline(file, line))
    {
        string uname = line.substr(0, line.find(","));
        string pwd = line.substr(line.find(",") + 1);
        if (uname == account.username)
        {
            user_exist = true;
            if (pwd == account.password)
            {
                login_access = true;
            }
            break;
        }
    }

    cout << "[\033[1;33mUser Login\033[0m][\033[1mUser Exist\033[0m] " << user_exist << endl;
    cout << "[\033[1;33mUser Login\033[0m][\033[1mLogin Access\033[0m] " << login_access << endl;

    // check
    if (!user_exist)
    {
        message = "[\033[1;31mError\033[0m] User does not exist";
        cout << "[\033[1mServer\033[0m] " << message << endl;
    }
    else if (!login_access)
    {
        message = "[\033[1;31mError\033[0m] Incorrect password";
        cout << "[\033[1mServer\033[0m] " << message << endl;
    }
    else
    {
        // remove original name_to_fd
        if (!name_to_fd[login_user.username])
            name_to_fd[login_user.username] = 0;

        // store login user info
        login_user.username = account.username;
        login_user.password = account.password;

        // update name_to_fd
        name_to_fd[login_user.username] = client_fd;

        message = "[\033[1;32mSuccess\033[0m] User logged in successfully";

        cout << "[\033[1mServer\033[0m]" << message << endl;
    }
    return message;
}

string UserLogout(User &login_user, int client_fd, unsigned char *key, unsigned char *iv)
{
    // User Logout
    // check if the user is logined
    string message;
    if (login_user.username.empty())
    {
        message = "[\033[1;31mError\033[0m] Not Logined";
        cout << "[\033[1mServer\033[0m]" << message << endl;
    }
    else
    {
        // remove original name_to_fd
        if (name_to_fd[login_user.username] != 0)
            name_to_fd[login_user.username] = 0;

        login_user.username = "";
        login_user.password = "";
        message = "[\033[1;32mSuccess\033[0m] User logged out successfully";
        cout << "[\033[1mServer\033[0m]" << message << endl;
    }
    return message;
}

string getInfo(string rcv_message, User &login_user, unsigned char *key, unsigned char *iv)
{
    // shoe aes key and iv
    // cout << "[\033[1;33mInfo\033[0m][\033[1mAES Key\033[0m] ";
    // for (int i = 0; i < 32; i++)
    // {
    //     printf("%02x", key[i]);
    // }
    // cout << endl;
    // cout << "[\033[1;33mInfo\033[0m][\033[1mAES IV\033[0m] ";
    // for (int i = 0; i < 16; i++)
    // {
    //     printf("%02x", iv[i]);
    // }
    // cout << endl;
    // get info
    string message;
    if (rcv_message.find("[Info] Get Username") != string::npos)
    {
        if (login_user.username.empty())
        {
            message = "[\033[1;31mError\033[0m] Not Logined";
        }
        else
        {
            message = "[\033[1;32mSuccess\033[0m] " + login_user.username;
        }
    }
    else if (rcv_message.find("[Info][Online]") != string::npos)
    {
        rcv_message = rcv_message.substr(rcv_message.find("] ") + 2);
        string target_username = rcv_message;
        cout << target_username << endl;

        if (name_to_fd[target_username] == 0)
        {
            message = "[\033[1;31mError\033[0m] User is offline";
        }
        else
        {
            message = "[\033[1;32mSuccess\033[0m] User is online";
        }
    }
    return message;
}

void chatting(string rcv_message, int client_fd, User login_user, unsigned char *key, unsigned char *iv)
{
    // invite a user to chat
    if (rcv_message.find("[Chatting][Invite]") != string::npos)
    {
        // [Chatting][Invite] username:target_username
        regex pattern(R"(\[Chatting\]\[Invite\]\s+(.+):(.+))");
        smatch match;
        string inviter;
        string accepter;

        if (regex_search(rcv_message, match, pattern))
        {
            inviter = match[1];
            accepter = match[2];
        }
        else
        {
            return;
        }

        waiting_for_chatting.insert(inviter);

        if (waiting_for_chatting.find(accepter) != waiting_for_chatting.end())
        {
            if (chatting_pair.find(inviter) != chatting_pair.end() || chatting_pair.find(accepter) != chatting_pair.end())
            {
                string snd_message = "[Chatting][Busy] " + inviter + ":" + accepter;
                vector<unsigned char> snd_message_cipher = encrypt(snd_message, key, iv);
                send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);
                return;
            }

            // send to accepter
            int accepter_fd = name_to_fd[accepter];
            string snd_message = "[Chatting][Start] " + accepter + ":" + inviter;
            vector<unsigned char> snd_message_cipher = encrypt(snd_message, key, iv);
            send(accepter_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

            // send to inviter
            snd_message = "[Chatting][Start] " + inviter + ":" + accepter;
            snd_message_cipher = encrypt(snd_message, key, iv);
            send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

            // add chatting pair
            chatting_pair[inviter] = accepter;
            chatting_pair[accepter] = inviter;
        }
        else
        {
            // send to inviter
            string snd_message = "[Chatting][Wait] " + inviter + ":" + accepter;
            vector<unsigned char> snd_message_cipher = encrypt(snd_message, key, iv);
            send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);
        }
    }
    else if (rcv_message.find("[Chatting][Exit]") != string::npos)
    {
        // [Chatting][Exit] me:target
        string me;
        string target;
        regex pattern(R"(\[Chatting\]\[Exit\]\s+(.+):(.+))");
        smatch match;
        if (regex_search(rcv_message, match, pattern))
        {
            me = match[1];
            target = match[2];
        }
        int target_fd = name_to_fd[target];

        // unset
        waiting_for_chatting.erase(target);
        waiting_for_chatting.erase(me);
        chatting_pair.erase(me);
        chatting_pair.erase(target);

        // request target to exit
        string snd_message = "[Chatting][Exit]";

        vector<unsigned char> snd_message_cipher = encrypt(snd_message, key, iv);
        send(target_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

        // send back to me
        snd_message = "[Chatting][Exit]";
        snd_message_cipher = encrypt(snd_message, key, iv);
        send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);
    }
    else if (rcv_message.find("[Chatting][File]") != string::npos)
    {
        // data
        string sender;
        string receiver;
        string file_name;
        int file_size;
        string content;

        // [Chatting][File] sender:receiver:file_name:file_size:content
        regex pattern(R"(\[Chatting\]\[File\]\s+(.+):(.+):(.+):(.+):(.+))");
        smatch match;
        if (regex_search(rcv_message, match, pattern))
        {
            sender = match[1];
            receiver = match[2];
            file_name = match[3];
            file_size = stoi(match[4]);
            content = match[5];
        }
        else
        {
            return;
        }

        // check sender
        if (sender != login_user.username)
        {
            return;
        }

        // get receiver fd
        int receiver_fd = name_to_fd[receiver];

        // send to receiver
        string snd_message = rcv_message;
        vector<unsigned char> snd_message_cipher = encrypt(snd_message, key, iv);
        send(receiver_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

        // file
        if (content == "start")
        {
            handleFile(client_fd, receiver_fd, file_size, key, iv);
        }
        return;
    }
    else if (rcv_message.find("[Chatting][Message]") != string::npos)
    {
        // [Chatting][Message] sender:receiver:message
        regex pattern(R"(\[Chatting\]\[Message\]\s+(.+):(.+):(.+))");
        smatch match;
        if (regex_search(rcv_message, match, pattern))
        {
            string sender = match[1];
            string receiver = match[2];
            string message = match[3];

            int receiver_fd = name_to_fd[receiver];
            string snd_message = rcv_message;
            vector<unsigned char> snd_message_cipher = encrypt(snd_message, key, iv);
            send(receiver_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);
        }
    }
    return;
}
