#include "server.hpp"
#include "src/UI.hpp"

// get local ip address
// string getLocalAddress(){
//     struct ifaddrs *interfaces = nullptr;
//     struct ifaddrs *temp_addr = nullptr;

//     // get current interfaces
//     if(getifaddrs(&interfaces) == -1){
//         printError("Getting network interfaces");
//         return "";
//     }

//     string local_ip = "";
//     temp_addr = interfaces;

//     // extract the local IP address from the interfaces
//     while(temp_addr != nullptr){
//         if(temp_addr->ifa_addr->sa_family == AF_INET){
//             char addressBuffer[INET_ADDRSTRLEN];
//             void* addr_ptr = &((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr;
            
//             // convert the IP to a string and store it in local_ip
//             inet_ntop(AF_INET, addr_ptr, addressBuffer, INET_ADDRSTRLEN);
//             local_ip = addressBuffer;
//             break;
//         }
//         temp_addr = temp_addr->ifa_next;
//     }

//     // free the memory
//     freeifaddrs(interfaces);
    
//     return local_ip;
// }

// get ip addresses from external interface to the server
string getIPAddress(){
    struct ifaddrs *ifaddr, *ifa;\
    string ip_address;

    if (getifaddrs(&ifaddr) == -1) {
        std::cerr << "Failed to get network interface information." << std::endl;
        return ip_address;
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) {
            continue;
        }

        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *addr = reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr);
            ip_address = inet_ntoa(addr->sin_addr);
        }
    }

    freeifaddrs(ifaddr);
    return ip_address;
}

// bind and listen to the server
void bindAndListen(int server_fd, int server_port, string server_ip){
    struct sockaddr_in server_address;

    // bind the socket to an IP / port
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip.c_str());

    if(bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        printError("Binding the socket to an IP / port");
        exit(1);
    }
    cout << "[\033[1;36mStatus\033[0m] Server is binded" << endl;
    cout << "[\033[1;33mInfo\033[0m][\033[1mServer IP\033[0m] " << server_ip << endl;
    cout << "[\033[1;33mInfo\033[0m][\033[1mServer Port\033[0m] " << server_port << endl;

    // listen for connections
    int listening = listen(server_fd, 5);
    if(listening < 0){
        printError("Listening for connections");
        exit(1);
    }
    cout << "[\033[1;36mStatus\033[0m] Server is listening..." << endl;
}

// accept the client connection
int acceptClient(int server_fd, struct sockaddr_in accept_address, socklen_t accept_addressSize){
    int client_fd = accept(server_fd, (struct sockaddr*)&accept_address, &accept_addressSize);
    if(client_fd < 0){
        printError("Accepting the call");
        return -1;
    }
    cout << "[\033[1;33mNew connection\033[0m][\033[1mClient IP\033[0m] " << inet_ntoa(accept_address.sin_addr) << endl;
    cout << "[\033[1;33mNew connection\033[0m][\033[1mClient Port\033[0m] " << ntohs(accept_address.sin_port) << endl;

    return client_fd;
}

// handle client
void *handleClient(void* new_fd){
    User login_user;
    int client_fd = *(int*)new_fd;
    while(1){
        char buffer[4096];
        bzero(buffer, 4096);

        // receive a message
        int bytes_received = recv(client_fd, buffer, 4096, 0);
        if(bytes_received <= 0){
            if(bytes_received == 0){
                // client disconnected
                cout << "[\033[1;36mStatus\033[0m] Client disconnected" << endl;
                cout << "[\033[1;36mStatus\033[0m] Server is listening..." << endl;
            } else {
                printError("Receiving data from the client");
            }
            break; // break the loop
        }
        cout << "[\033[1mClient\033[0m] " << string(buffer, 0, bytes_received) << endl;

        if(string(buffer, 0, bytes_received).find("[User Registration]") != string::npos){
            // User Registration
            // get username and pwd
            User account;
            int gap = 20;
            account.username = string(buffer, gap, string(buffer, gap, bytes_received).find(":"));
            account.password = string(buffer, gap + string(buffer, gap, bytes_received).find(":") + 1, bytes_received - gap - string(buffer, gap, bytes_received).find(":") - 1);
            cout << "[\033[1;33mUser Registration\033[0m][\033[1mUsername\033[0m] " << account.username << endl;
            cout << "[\033[1;33mUser Registration\033[0m][\033[1mPassword\033[0m] " << account.password << endl;

            // find if the user already exists
            ifstream file("./data/account.csv");
            string line;
            bool user_exist = false;
            getline(file, line); // skip the first line
            while(getline(file, line)){
                if(line.find(account.username) != string::npos){
                    user_exist = true;
                    break;
                }
            }

            // send back to client a message and regigster the user
            if(user_exist){
                string message = "[\033[1;31mError\033[0m] User already exists";
                cout << "[\033[1mServer\033[0m]" << message << endl;
                send(client_fd, message.c_str(), message.size() + 1, 0);
            }else{
                ofstream file("./data/account.csv", ios::app);
                file << account.username << "," << account.password << endl;

                string message = "[\033[1;32mSuccess\033[0m] User registered successfully";
                cout << "[\033[1mServer\033[0m]" << message << endl;
                send(client_fd, message.c_str(), message.size() + 1, 0);
            }

            file.close();
        }else if(string(buffer, 0, bytes_received).find("[User Login]") != string::npos){
            // User Login
            // get username and pwd
            User account;
            int gap = 13;
            account.username = string(buffer, gap, string(buffer, gap, bytes_received).find(":"));
            account.password = string(buffer, gap + string(buffer, gap, bytes_received).find(":") + 1, bytes_received - gap - string(buffer, gap, bytes_received).find(":") - 1);
            cout << "[\033[1;33mUser Login\033[0m][\033[1mUsername\033[0m] " << account.username << endl;
            cout << "[\033[1;33mUser Login\033[0m][\033[1mPassword\033[0m] " << account.password << endl;

            // find if the user exists
            ifstream file("./data/account.csv");
            string line;
            bool user_exist = false;
            bool login_access = false;
            getline(file, line); // skip the first line
            while(getline(file, line)){
                string uname = line.substr(0, line.find(","));
                string pwd = line.substr(line.find(",") + 1);
                if(uname == account.username){
                    user_exist = true;
                    if(pwd == account.password){
                        login_access = true;
                    }
                    break;
                }
            }

            cout << "[\033[1;33mUser Login\033[0m][\033[1mUser Exist\033[0m] " << user_exist << endl;
            cout << "[\033[1;33mUser Login\033[0m][\033[1mLogin Access\033[0m] " << login_access << endl;

            // check
            if(!user_exist){
                string message = "[\033[1;31mError\033[0m] User does not exist";
                cout << "[\033[1mServer\033[0m] " << message << endl;
                send(client_fd, message.c_str(), message.size() + 1, 0);
            }else if(!login_access){
                string message = "[\033[1;31mError\033[0m] Incorrect password";
                cout << "[\033[1mServer\033[0m] " << message << endl;
                send(client_fd, message.c_str(), message.size() + 1, 0);
            }else{
                // store login user info
                login_user.username = account.username;
                login_user.password = account.password;

                string message = "[\033[1;32mSuccess\033[0m] User logged in successfully";
                cout << "[\033[1mServer\033[0m]" << message << endl;
                send(client_fd, message.c_str(), message.size() + 1, 0);
            }
        }else if(string(buffer, 0, bytes_received).find("[User Logout]") != string::npos){
            // User Logout
            cout << "[\033[1;33mUser Logout\033[0m]" << endl;
            
            // check if the user is logined
            string message;
            if(login_user.username.empty()){
                message = "[\033[1;31mError\033[0m] Not Logined";
            }else{
                login_user = User();
                message = "[\033[1;32mSuccess\033[0m] User logged out successfully";
            }

            cout << "[\033[1mServer\033[0m]" << message << endl;
            send(client_fd, message.c_str(), message.size() + 1, 0);

        }else if(string(buffer, 0, bytes_received).find("[Info]") != string::npos){
            // get info
            if(string(buffer, 0, bytes_received).find("Get Username") != string::npos){
                // get login username
                string message = "";
                if(login_user.username.empty()){
                    message = "[\033[1;31mError\033[0m] Not Logined";
                }else{
                    message = "[\033[1;32mSuccess\033[0m] " + login_user.username;
                }
                cout << "[\033[1mServer\033[0m]" << message << endl;
                send(client_fd, message.c_str(), message.size() + 1, 0);
            }
        }else{
            // send back to client a message
            string message = "Server received the message";
            send(client_fd, message.c_str(), message.size() + 1, 0);
        }
    }
    close(client_fd);
    return nullptr;
}
