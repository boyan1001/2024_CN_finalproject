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