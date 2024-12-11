#include "client.hpp"
#include "src/UI.hpp"

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