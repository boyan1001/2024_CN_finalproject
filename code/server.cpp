#include "server.hpp"
#include "src/UI.hpp"

int main(int argc, char *argv[]){

    // argument
    if(argc != 2){
        printError("Invalid argument, Usage: ./server <server port>");
        exit(1);
    }
    int server_port = atoi(argv[1]);
    system("clear");

    // print title
    title(0);
    serverWelcome();
    system("clear");

    title(0);


    // create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){
        printError("Creating a socket");
        exit(1);
    }

    // check if address already in use
    int on = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
        printError("Setting socket options");
        exit(1);
    }
    

    // bind the socket to an IP / port
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    if(bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        printError("Binding the socket to an IP / port");
        exit(1);
    }

    // listen for connections
    int listening = listen(server_fd, 5);
    if(listening < 0){
        printError("Listening for connections");
        exit(1);
    }
    cout << "[\033[1;36mStatus\033[0m] Server is listening..." << endl;

    struct sockaddr_in accept_address;
    socklen_t accept_addressSize = sizeof(accept_address);

    while(1){
        // accept the call
        int new_fd = accept(server_fd, (struct sockaddr*)&accept_address, &accept_addressSize);
        if(new_fd < 0){
            printError("Accepting the call");
            continue;
        }
        cout << "[\033[1;33mNew connection\033[0m][\033[1mClient IP\033[0m] " << inet_ntoa(accept_address.sin_addr) << endl;
        cout << "[\033[1;33mNew connection\033[0m][\033[1mClient Port\033[0m] " << ntohs(accept_address.sin_port) << endl;

        while(1){
            char buffer[4096];
            bzero(buffer, 4096);

            // receive a message
            int bytes_received = recv(new_fd, buffer, 4096, 0);
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

            // User Registration
            if(string(buffer, 0, bytes_received).find("[User Registration]") != string::npos){
                // get username and pwd
                User account;
                account.username = string(buffer, 20, string(buffer, 20, bytes_received).find(":"));
                account.password = string(buffer, 20 + string(buffer, 20, bytes_received).find(":") + 1, bytes_received - 20 - string(buffer, 20, bytes_received).find(":") - 1);
                
                // find if the user already exists
                ifstream file("./data/account.csv");
                string line;
                bool userExists = false;
                getline(file, line); // skip the first line
                while(getline(file, line)){
                    if(line.find(account.username) != string::npos){
                        userExists = true;
                        break;
                    }
                }

                // send back to client a message and regigster the user
                if(userExists){
                    string message = "[\033[1;31mError\033[0m] User already exists";
                    cout << "[\033[1;32mServer\033[0m]" << message << endl;
                    send(new_fd, message.c_str(), message.size() + 1, 0);
                }else{
                    ofstream file("./data/account.csv", ios::app);
                    file << account.username << "," << account.password << endl;

                    string message = "User registered successfully";
                    cout << "[\033[1;32mServer\033[0m] " << message << endl;
                    send(new_fd, message.c_str(), message.size() + 1, 0);
                }

                file.close();
            }else{
                // send back to client a message
                string message = "Server received the message";
                send(new_fd, message.c_str(), message.size() + 1, 0);
            }
        }
        close(new_fd);
    }
    close(server_fd);

    return 0;
}