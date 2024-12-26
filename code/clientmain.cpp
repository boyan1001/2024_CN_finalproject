#include "client.hpp"
#include "src/UI.hpp"
#include "src/crypt.hpp"

static unsigned char key[32] = {0};
static unsigned char iv[16] = {0};

int main(int argc, char *argv[])
{

    // argument
    if (argc != 3)
    {
        printError("Invalid argument, Usage: ./client.o <server domain> <server port>");
        exit(1);
    }

    string server_ip;
    if(isIP(argv[1])){
        server_ip = argv[1];
    }else{
        server_ip = getIPfromDomain(argv[1]);
    }
    int server_port = atoi(argv[2]);

    system("clear");

    // print title
    title(1);
    clientWelcome();

    // Create a socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd == -1)
    {
        printError("Creating a socket");
        exit(1);
    }

    // connect to the server
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip.c_str());

    int connection_status = connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection_status == -1)
    {
        printError("Connecting to the server");
        exit(1);
    }

    // If connected, start sending and receiving data
    system("clear");
    title(1);
    cout << "Connected to the server..." << endl;
    cout << endl;
    cout << "[\033[1;32mSuccess\033[0m] Connection established" << endl;
    cout << "[\033[1mServer IP\033[0m] " << server_ip << endl;
    cout << "[\033[1mServer Port\033[0m] " << server_port << endl;
    cout << endl;
    cout << ">>> Press ENTER to continue" << endl;
    cin.get();

    // get the AES key and IV
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_received = recv(client_fd, key, 32, 0);
    if (bytes_received < 0)
    {
        printError("Receiving data from the server");
        return 1;
    }
    bytes_received = recv(client_fd, iv, 16, 0);
    if (bytes_received < 0)
    {
        printError("Receiving data from the server");
        return 1;
    }
    // receive data from the server
    string username = getNowUsername(client_fd, key, iv);
    

    while (1)
    {
        system("clear");
        title(1);

        string choice;
        if (username.empty())
        {
            choice = clientMainMenu(username, 0);
        }
        else
        {
            choice = clientMainMenu(username, 1);
        }
        string snd_message;
        vector<unsigned char> snd_message_cipher;
        string rcv_message;
        vector<unsigned char> rcv_message_cipher;

        if (choice == "R")
        {
            returnMessage();
            close(client_fd);
            return 0;
        }
        else if (choice == "0")
        {
            // User Registration
            while (1)
            {
                system("clear");
                title(1);

                // enter data
                snd_message = userRegistration(username);
                if (snd_message.empty())
                {
                    cout << ">>> Press ENTER to continue" << endl;
                    cin.get();
                    continue;
                }

                // send to server
                snd_message_cipher = encrypt(snd_message, key, iv);
                send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

                // receive from server
                ssize_t bytes_received = recv(client_fd, buffer, 4096, 0);
                if (bytes_received < 0)
                {
                    printError("Receiving data from the server");
                    break;
                }
                rcv_message_cipher = vector<unsigned char>(buffer, buffer + bytes_received);
                // rcv_message_cipher = rcv_message_cipher.substr(0, rcv_message_cipher.find('\0')); // remove useless characters
                rcv_message = decrypt(rcv_message_cipher, key, iv);
                cout << rcv_message << endl;
                cout << endl;
                cout << ">>> Press ENTER to continue" << endl;
                cin.get();
                break;
            }
        }
        else if (choice == "1" && username.empty())
        {
            // User Login
            while (1)
            {
                system("clear");
                title(1);
                // enter data
                snd_message = UserLogin(username);
                if (snd_message.empty())
                {
                    cout << ">>> Press ENTER to continue" << endl;
                    cin.get();
                    continue;
                }

                // send to server
                snd_message_cipher = encrypt(snd_message, key, iv);
                send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

                // receive from server
                ssize_t bytes_received = recv(client_fd, buffer, 4096, 0);
                if (bytes_received < 0)
                {
                    printError("Receiving data from the server");
                    continue;
                }

                rcv_message_cipher = vector<unsigned char>(buffer, buffer + bytes_received);
                // rcv_message_cipher = rcv_message_cipher.substr(0, rcv_message_cipher.find('\0')); // remove useless characters
                rcv_message = decrypt(rcv_message_cipher, key, iv);
                
                if (rcv_message.find("[\033[1;31mError\033[0m]") != string::npos)
                {
                    cout << rcv_message << endl;
                }
                else
                {
                    // check if the user is logined
                    cout << rcv_message << endl;
                    username = getNowUsername(client_fd, key, iv);

                    // mkdir
                    string dir = "./data/client/" + username;
                    mkdir(dir.c_str(), 0777);
                    mkdir((dir + "/file").c_str(), 0777);
                    mkdir((dir + "/audio").c_str(), 0777);
                    mkdir((dir + "/video").c_str(), 0777);
                }

                cout << endl;
                cout << ">>> Press ENTER to continue" << endl;
                cin.get();

                break;
            }
        }
        else if (choice == "2" && !username.empty())
        {
            // User Logout
            snd_message = "[User Logout]";
            if (snd_message.empty())
            {
                cout << ">>> Press ENTER to continue" << endl;
                cin.get();
                continue;
            }

            // send to server
            snd_message_cipher = encrypt(snd_message, key, iv);
            send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

            // receive from server
            ssize_t bytes_received = recv(client_fd, buffer, 4096, 0);
            rcv_message_cipher = vector<unsigned char>(buffer, buffer + bytes_received);
            // rcv_message_cipher = rcv_message_cipher.substr(0, rcv_message_cipher.find('\0')); // remove useless characters
            rcv_message = decrypt(rcv_message_cipher, key, iv);

            if (bytes_received < 0)
            {
                printError("Receiving data from the server");
                break;
            }

            // check if the user is logined
            username = getNowUsername(client_fd, key, iv);

            cout << rcv_message << endl;
            cout << endl;
            cout << ">>> Press ENTER to continue" << endl;
            cin.get();
        }
        else if(choice == "3" && !username.empty())
        {
            chatting(client_fd, username, key, iv);
            continue;
        }
        else
        {
            printError("Invalid choice");
            cout << endl;
            cout << ">>> Press ENTER to continue" << endl;
            cin.get();
            continue;
        }
    }

    close(client_fd);
    return 0;
}