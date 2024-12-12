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
    bindAndListen(server_fd, server_port, server_ip);

    struct sockaddr_in accept_address;
    socklen_t accept_addressSize = sizeof(accept_address);

    while(1){
        // accept the call
        int client_fd = acceptClient(server_fd, accept_address, accept_addressSize);

        int* new_client_fd = new int(client_fd);
        pthread_t client_thread;

        if(pthread_create(&client_thread, NULL, handleClient, (void*)&new_client_fd) != 0){
            printError("Creating a thread");
            delete new_client_fd;
            close(client_fd);
        }

        pthread_detach(client_thread);
    }

    close(server_fd);

    return 0;
}