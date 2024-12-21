#include "server.hpp"
#include "src/UI.hpp"

static unordered_map<string, int> name_to_fd;

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
void *handleClient(void *new_fd)
{
    User login_user;
    int client_fd = *(int *)new_fd;

    char buffer[4096];

    int bytes_received;

    while (bytes_received = recv(client_fd, buffer, 4096, 0) > 0)
    {
        string rcv_message = string(buffer, 0, 4096);
        string snd_message;

        if (login_user.username != "")
        {
            name_to_fd[login_user.username] = client_fd;
        }

        # if DEBUG == 1
        cout << "[\033[1mClient\033[0m] " << rcv_message << endl;
        # endif

        if (rcv_message.find("[Exit]") != string::npos)
        {
            cout << "[\033[1mServer\033[0m] Client exited" << endl;
            break;
        }

        if (rcv_message.find("[User Registration]") != string::npos)
        {
            snd_message = userRegistration(rcv_message);
        }
        else if (rcv_message.find("[User Login]") != string::npos)
        {
            snd_message = UserLogin(rcv_message, login_user, client_fd);
        }
        else if (rcv_message.find("[User Logout]") != string::npos)
        {
            snd_message = UserLogout(login_user, client_fd);
        }
        else if (rcv_message.find("[Info]") != string::npos)
        {
            snd_message = getInfo(rcv_message, login_user);
        }
        else
        {
            // send back to client a message
            # if DEBUG == 1
            cout << "[\033[1mServer\033[0m] " << rcv_message << endl;
            # endif
        }

        # if DEBUG == 1
        cout << "[\033[1mServer\033[0m] " << snd_message << endl;
        # endif
        send(client_fd, snd_message.c_str(), snd_message.size() + 1, 0);

        bzero(buffer, 4096);
    }
    
    if(login_user.username != "")
    {
        name_to_fd[login_user.username] = 0;
    }
    
    cout << "Client disconnected, socket: " << client_fd << endl;
    close(client_fd); // 關閉 client socket
    return nullptr;
}

string userRegistration(string rcv_message)
{
    // rcv_message = "[User Registration] username:password"
    // User Registration
    // get username and pwd
    User account;
    regex pattern(R"(\[User Registration\]\s+(\w+):(\w+))");
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
    ifstream file("./data/account.csv");
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
        ofstream file("./data/account.csv", ios::app);
        file << account.username << "," << account.password << endl;

        message = "[\033[1;32mSuccess\033[0m] User registered successfully";
    }

    file.close();
    return message;
}

string UserLogin(string rcv_message, User &login_user, int client_fd)
{
    // User Login
    // get username and pwd
    User account;
    string message;
    regex pattern(R"(\[User Login\]\s+(\w+):(\w+))");
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

    if(account.username.empty() || account.password.empty())
    {
        message = "[\033[1;31mError\033[0m] Username or password cannot be empty";
        cout << "[\033[1mServer\033[0m] " << message << endl;
        return message;
    }

    if(name_to_fd[account.username] != 0)
    {
        message = "[\033[1;31mError\033[0m] A user can only login one device";
        cout << "[\033[1mServer\033[0m] " << message << endl;
        return message;
    }

    cout << "[\033[1;33mUser Login\033[0m][\033[1mUsername\033[0m] " << account.username << endl;
    cout << "[\033[1;33mUser Login\033[0m][\033[1mPassword\033[0m] " << account.password << endl;

    // find if the user exists
    ifstream file("./data/account.csv");
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

string UserLogout(User &login_user, int client_fd)
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
        if (!name_to_fd[login_user.username])
            name_to_fd[login_user.username] = 0;

        login_user.username = "";
        login_user.password = "";
        message = "[\033[1;32mSuccess\033[0m] User logged out successfully";
        cout << "[\033[1mServer\033[0m]" << message << endl;
    }
    return message;
}

string getInfo(string rcv_message, User &login_user)
{
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
    return message;
}