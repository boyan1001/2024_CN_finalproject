#include "client.hpp"
#include "src/UI.hpp"

int main(int argc, char *argv[]){

    // argument
    if(argc != 3){
        printError("Invalid argument, Usage: ./client <server IP> <server port>");
        exit(1);
    }

    string server_ip = argv[1];
    int server_port = atoi(argv[2]);

    system("clear");

    // print title
    title(1);
    clientWelcome();

    // Create a socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(client_fd == -1){
        printError("Creating a socket");
        exit(1);
    }

    // connect to the server
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip.c_str());

    int connection_status = connect(client_fd, (struct sockaddr*)&server_address, sizeof(server_address));
    if(connection_status == -1){
        printError("Connecting to the server");
        exit(1);
    }

    // If connected, start sending and receiving data
    system("clear");
    title(1);
    cout << "Connected to the server..." << endl << endl;
    cout << "[\033[1;32mSuccess\033[0m] Connection established" << endl;
    cout << "[\033[1mServer IP\033[0m] " << server_ip << endl;
    cout << "[\033[1mServer Port\033[0m] " << server_port << endl;
    cout << endl << ">>> Press ENTER to continue" << endl;
    cin.get();

    // receive data from the server
    char buffer[4096];
    bzero(buffer, 4096);
    
    string username = getNowUsername(client_fd);

    while(1){
        system("clear");
        title(1);
        string choice;
        if(username.empty()){
            choice = clientMainMenu(username, 0);
        }else{
            choice = clientMainMenu(username, 1);
        }
        string client_message;

        if(choice == "R"){
            cout << "[\033[1;36mStatus\033[0m] Closing the connection" << endl;
            cout << "Exiting the program..." << endl;
            close(client_fd);
            return 0;
        }else if(choice == "0"){
            // User Registration
            while(1){
                system("clear");
                title(1);
                // enter data
                client_message = userRegistration(username);
                if(client_message.empty()){
                    cout << ">>> Press ENTER to continue" << endl;
                    cin.get();
                    continue;
                }

                // send to server
                send(client_fd, client_message.c_str(), client_message.size() + 1, 0);

                // receive from server
                int bytes_received = recv(client_fd, buffer, 4096, 0);
                if(bytes_received < 0){
                    printError("Receiving data from the server");
                    break;
                }
                cout << "[\033[1mServer\033[0m] " << string(buffer, 0, bytes_received) << endl;
                cout << endl;
                cout << ">>> Press ENTER to continue" << endl;
                cin.get();
                break;
            }
        }else if(choice == "1"){
            // User Login
            while(1){
                system("clear");
                title(1);
                // enter data
                client_message = UserLogin(username);
                if(client_message.empty()){
                    cout << ">>> Press ENTER to continue" << endl;
                    cin.get();
                    continue;
                }

                // send to server
                send(client_fd, client_message.c_str(), client_message.size() + 1, 0);

                // receive from server
                int bytes_received = recv(client_fd, buffer, 4096, 0);
                if(bytes_received < 0){
                    printError("Receiving data from the server");
                    continue;
                }

                // check if the user is logined
                username = getNowUsername(client_fd);

                cout << "[\033[1mServer\033[0m] " << string(buffer, 0, bytes_received) << endl;
                cout << endl;
                cout << ">>> Press ENTER to continue" << endl;
                cin.get();
                
                break;
            }
        }else if(choice == "2"){
            // User Logout
            client_message = "[User Logout]";
            if(client_message.empty()){
                cout << ">>> Press ENTER to continue" << endl;
                cin.get();
                continue;
            }

            // send to server
            send(client_fd, client_message.c_str(), client_message.size() + 1, 0);

            // receive from server
            int bytes_received = recv(client_fd, buffer, 4096, 0);
            if(bytes_received < 0){
                printError("Receiving data from the server");
                break;
            }

            // check if the user is logined
            username = getNowUsername(client_fd);

            cout << "[\033[1mServer\033[0m] " << string(buffer, 0, bytes_received) << endl;
            cout << endl;
            cout << ">>> Press ENTER to continue" << endl;
            cin.get();
        }else{
            printError("Invalid choice");
            cout << endl;
            cout << ">>> Press ENTER to continue" << endl;
            cin.get();
            continue;
        }

        // cout << "[\033[1mClient\033[0m] " << client_message << endl;

        // // Send the message to the server
        // send(client_fd, client_message.c_str(), client_message.size() + 1, 0);

        // int bytes_received = recv(client_fd, buffer, 4096, 0);
        // if(bytes_received < 0){
        //     printError("Receiving data from the server");
        //     break;
        // }
        // cout << "[\033[1mServer\033[0m] " << string(buffer, 0, bytes_received) << endl;
    }

    close(client_fd);
    return 0;
}

string userRegistration(string username){
    User account;
    string message;

    statusMessage(username);
    cout << endl;

    cout << "Please enter your \033[1musername\033[0m: ";
    getline(cin, account.username);
    cout << "Please enter your \033[1mpassword\033[0m: ";
    getline(cin, account.password);
    cout << endl;

    if(account.username.empty() || account.password.empty()){
        printError("Username or password cannot be empty");
        return "";
    }

    if(account.username.find(":") != string::npos || account.password.find(":") != string::npos || account.username.find(" ") != string::npos || account.password.find(" ") != string::npos){
        printError("Username or password cannot contain ':' and space");
        return "";
    }

    message = "[User Registration] " + account.username + ":" + account.password;
    return message;
}

string UserLogin(string &username){
    User account;
    string message;

    statusMessage(username);
    cout << endl;

    cout << "Please enter your \033[1musername\033[0m: ";
    getline(cin, account.username);
    cout << "Please enter your \033[1mpassword\033[0m: ";
    getline(cin, account.password);
    cout << endl;

    if(account.username.empty() || account.password.empty()){
        printError("Username or password cannot be empty");
        return "";
    }

    if(account.username.find(":") != string::npos || account.password.find(":") != string::npos || account.username.find(" ") != string::npos || account.password.find(" ") != string::npos){
        printError("Username or password cannot contain ':' and space");
        return "";
    }

    message = "[User Login] " + account.username + ":" + account.password;

    username = account.username;
    return message;
}

string getNowUsername(int client_fd){
    char buffer[4096];
    string client_message = "[Info] Get Username";
    send(client_fd, client_message.c_str(), client_message.size() + 1, 0);
    int bytes_received = recv(client_fd, buffer, 4096, 0);
    if(bytes_received < 0){
        printError("Receiving data from the server");
        return "";
    }
    if(string(buffer, 0, bytes_received).find("[\033[1;31mError\033[0m]") != string::npos){
        return "";
    }else{
        return string(buffer, 0, bytes_received).substr(string(buffer, 0, bytes_received).find(" ") + 1);
    }
}
