#include <iostream>
#include <string>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

// Function to get local IP addresses
void getLocalIPAddresses() {
    struct ifaddrs *interfaces = nullptr;
    struct ifaddrs *temp_addr = nullptr;

    // Retrieve the current interfaces
    if (getifaddrs(&interfaces) == -1) {
        cerr << "Error getting network interfaces." << endl;
        return;
    }

    cout << "Local IP Addresses:" << endl;
    temp_addr = interfaces;
    while (temp_addr != nullptr) {
        if (temp_addr->ifa_addr->sa_family == AF_INET) { // Check if it is an IPv4 address
            char addressBuffer[INET_ADDRSTRLEN];
            void* addr_ptr = &((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, addr_ptr, addressBuffer, INET_ADDRSTRLEN);
            cout << temp_addr->ifa_name << ": " << addressBuffer << endl;
        }
        temp_addr = temp_addr->ifa_next;
    }

    freeifaddrs(interfaces);
}

int main() {
    // Call the function to get local IP addresses
    getLocalIPAddresses();
    return 0;
}
