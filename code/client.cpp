#include "client.h"

int main(){
    // Create a socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(client_fd == -1){
        cout << "[\033[1;31mError\033[0m] Creating a socket" << endl;
        exit(1);
    }

    // connect to the server
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    int connection_status = connect(client_fd, (struct sockaddr*)&server_address, sizeof(server_address));
    if(connection_status == -1){
        cout << "[\033[1;31mError\033[0m] In connecting to the server" << endl;
        exit(1);
    }

    // receive data from the server
    char buffer[4096];
    bzero(buffer, 4096);

    while(1){
        string client_message;
        cout << "Type the message you want to send to server: ";
        getline(cin, client_message);
        cout << "[\033[1mClient\033[0m] " << client_message << endl;

        // Send the message to the server
        send(client_fd, client_message.c_str(), client_message.size() + 1, 0);

        int bytes_received = recv(client_fd, buffer, 4096, 0);
        if(bytes_received < 0){
            cout << "[\033[1;31mError\033[0m] In receiving data" << endl;
            break;
        }
        cout << "[\033[1mServer\033[0m] " << string(buffer, 0, bytes_received) << endl;
    }

    return 0;
}