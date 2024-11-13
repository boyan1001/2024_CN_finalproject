#include "client.h"
#include "UI.h"

int main(){
    system("clear");

    // print title
    title(1);
    clientWelcome();
    system("clear");

    title(1);

    // Create a socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(client_fd == -1){
        printError("Creating a socket");
        exit(1);
    }

    // connect to the server
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    int connection_status = connect(client_fd, (struct sockaddr*)&server_address, sizeof(server_address));
    if(connection_status == -1){
        printError("Connecting to the server");
        exit(1);
    }

    // If connected, start sending and receiving data
    cout << "[\033[1;36mStatus\033[0m] Connected to the server..." << endl;
    cout << "[\033[1;33mNew connection\033[0m][\033[1mServer IP\033[0m] " << SERVER_IP << endl;
    cout << "[\033[1;33mNew connection\033[0m][\033[1mServer Port\033[0m] " << SERVER_PORT << endl;
    // receive data from the server
    char buffer[4096];
    bzero(buffer, 4096);

    while(1){
        string client_message;
        cout << "Type the message you want to send to server: ";
        getline(cin, client_message);
        if(client_message == "exit"){
            cout << "[\033[1;36mStatus\033[0m] Closing the connection" << endl;
            break;
        }
        cout << "[\033[1mClient\033[0m] " << client_message << endl;

        // Send the message to the server
        send(client_fd, client_message.c_str(), client_message.size() + 1, 0);

        int bytes_received = recv(client_fd, buffer, 4096, 0);
        if(bytes_received < 0){
            printError("Receiving data from the server");
            break;
        }
        cout << "[\033[1;32mServer\033[0m] " << string(buffer, 0, bytes_received) << endl;
    }

    return 0;
}