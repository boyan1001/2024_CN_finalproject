#include "server.hpp"
#include "src/UI.hpp"

// get local ip address
string getLocalAddress(){
    struct ifaddrs *interfaces = nullptr;
    struct ifaddrs *temp_addr = nullptr;

    // get current interfaces
    if(getifaddrs(&interfaces) == -1){
        printError("Getting network interfaces");
        return "";
    }

    string local_ip = "";
    temp_addr = interfaces;

    // extract the local IP address from the interfaces
    while(temp_addr != nullptr){
        if(temp_addr->ifa_addr->sa_family == AF_INET){
            char addressBuffer[INET_ADDRSTRLEN];
            void* addr_ptr = &((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr;
            
            // convert the IP to a string and store it in local_ip
            inet_ntop(AF_INET, addr_ptr, addressBuffer, INET_ADDRSTRLEN);
            local_ip = addressBuffer;
            break;
        }
        temp_addr = temp_addr->ifa_next;
    }

    // free the memory
    freeifaddrs(interfaces);
    
    return local_ip;
}

// get ip addresses from external interface to the server
vector<string> getIPAddress(){
    struct ifaddrs *ifaddr, *ifa;\
    vector<string> ip_address;

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
            string ip = inet_ntoa(addr->sin_addr);
            ip_address.push_back(ip);
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