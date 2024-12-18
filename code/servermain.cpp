#include "server.hpp"
#include "src/UI.hpp"

int main(int argc, char *argv[]){
    srand(time(NULL));

    // argument
    if(argc != 2){
        printError("Invalid argument, Usage: ./server.o <server port>");
        exit(1);
    }
    int server_port = atoi(argv[1]);

    system("clear");


    // get server interfaces
    string interfaces = getIPAddress();
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

    string server_ip = interfaces;

    // bind and listen to the server
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip.c_str());

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
    cout << "[\033[1;33mInfo\033[0m][\033[1mServer IP\033[0m] " << server_ip << endl;
    cout << "[\033[1;33mInfo\033[0m][\033[1mServer Port\033[0m] " << server_port << endl;

    struct sockaddr_in accept_address;
    socklen_t accept_addressSize = sizeof(accept_address);

    while(1){
        // accept the call
        int client_fd = accept(server_fd, (struct sockaddr*)&accept_address, &accept_addressSize);
        if(client_fd < 0){
            printError("Accepting the call");
            return -1;
        }
        
        cout << "[\033[1;33mNew connection\033[0m][\033[1mClient IP\033[0m] " << inet_ntoa(accept_address.sin_addr) << endl;
        cout << "[\033[1;33mNew connection\033[0m][\033[1mClient Port\033[0m] " << ntohs(accept_address.sin_port) << endl;

        int* new_client_fd = new int(client_fd);
        pthread_t client_thread;


        if(pthread_create(&client_thread, nullptr, handleClient, (void*)new_client_fd) != 0){
            printError("Creating a thread");
            delete new_client_fd;
            close(client_fd);
        }


        pthread_detach(client_thread);
    }

    close(server_fd);

    return 0;
}