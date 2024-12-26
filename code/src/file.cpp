#include "file.hpp"
#include "crypt.hpp"
#include "UI.hpp"

bool sendFile(int client_fd, string file_path, string sender, string receiver, unsigned char *key, unsigned char *iv){
    // send file
    ifstream file(file_path, ios::binary);
    if(!file){
        printError("File not found");
        return false;
    }

    // get file size
    file.seekg(0, ios::end);
    int file_size = file.tellg();
    file.seekg(0, ios::beg);

    // get file name
    string file_name = file_path.substr(file_path.find_last_of("/") + 1);

    // send file size
    string header = "[Chatting][File] " + sender + ":" + receiver + ":" + file_name;
    string snd_message = header + ":" + to_string(file_size) + ":start";
    vector<unsigned char> snd_message_cipher = encrypt(snd_message, key, iv);
    send(client_fd, snd_message_cipher.data(), snd_message_cipher.size(), 0);

    // send file content
    char buffer[65536];
    while(file_size > 0){
        // cout << "[File] file_size: " << file_size << endl;
        file.read(buffer, 4096);
        std::streamsize actual_read = file.gcount();
        if(actual_read <= 0){
            break;
        }

        vector<unsigned char> file_content(buffer, buffer + actual_read);
        // cout << "[File] file_content size: " << file_content.size() << endl;
        // cout << "[File] file_content: ";
        // for(int i = 0; i < file_content.size(); i++){
        //     printf("%02x", file_content[i]);
        // }
        cout << endl;
        vector<unsigned char> file_content_cipher = encrypt_file(file_content, key, iv);
        // cout << "[File] file_content_cipher size: " << file_content_cipher.size() << endl;
        // cout << "[File] file_content_cipher: ";
        // for(int i = 0; i < file_content_cipher.size(); i++){
        //     printf("%02x", file_content_cipher[i]);
        // }
        cout << endl;

        int cipher_size = file_content_cipher.size();

        if (!sendAll(client_fd, reinterpret_cast<unsigned char*>(&cipher_size), sizeof(cipher_size))) {
            file.close();
            return false;
        }

        if (!sendAll(client_fd, file_content_cipher.data(), file_content_cipher.size())) {
            file.close();
            return false;
        }
        file_size -= actual_read;
    }

    file.close();
    return true;
}

bool recvFile(int client_fd, string file_path, int file_size, string sender, string receiver, unsigned char *key, unsigned char *iv) {
    string stem = filesystem::path(file_path).stem().string();
    string parent_path = filesystem::path(file_path).parent_path().string();
    string extension = filesystem::path(file_path).extension().string();
    char buffer[65536];
    vector<unsigned char> rcv_message_cipher;
    vector<unsigned char> rcv_file_message;
    string rcv_message;
    int bytes_received;

    // Ensure the directory exists
    filesystem::path dir = filesystem::path(file_path).parent_path();
    if (!filesystem::exists(dir)) {
        filesystem::create_directories(dir);
    }

    // Handle file name conflicts
    if(filesystem::exists(file_path)){
        int i = 1;
        while(filesystem::exists(parent_path + "/" + stem + "(" + to_string(i) + ")" + extension)){
            i++;
        }
        file_path = parent_path + "/" + stem + "(" + to_string(i) + ")" + extension;
    }

    ofstream file(file_path, ios::binary);
    if (!file.is_open()) {
        printError("Failed to open file for writing");
        return false;
    }

    while (file_size > 0) {
        int cipher_size;
        if (!recvAll(client_fd, reinterpret_cast<unsigned char*>(&cipher_size), sizeof(cipher_size))) {
            file.close();
            return false;
        }

        rcv_message_cipher = vector<unsigned char>(cipher_size);
        if (!recvAll(client_fd, rcv_message_cipher.data(), cipher_size)) {
            file.close();
            return false;
        }

        rcv_file_message = decrypt_file(rcv_message_cipher, key, iv);
        // cout << "[File] rcv_file_message size: " << rcv_file_message.size() << endl;
        // cout << "[File] rcv_file_message: ";
        // for(int i = 0; i < rcv_file_message.size(); i++){
        //     printf("%02x", rcv_file_message[i]);
        // }
        cout << endl;

        if (rcv_file_message.empty()) {
            file.close();
            return false;
        }

        file.write(reinterpret_cast<char*>(rcv_file_message.data()), rcv_file_message.size());
        file_size -= rcv_file_message.size();
    }

    file.close();
    return true;
}

void handleFile(int client_fd, int receiver_fd, int file_size, unsigned char *key, unsigned char *iv){
    char buffer[65536];
    int bytes_received;
    string rcv_message;
    vector<unsigned char> rcv_message_cipher;
    vector<unsigned char> rcv_file_message;
    vector<unsigned char> snd_message_cipher;

    // receive file content
    while(file_size > 0){
        memset(buffer, 0, sizeof(buffer));
        
        int cipher_size;
        if (!recvAll(client_fd, reinterpret_cast<unsigned char*>(&cipher_size), sizeof(cipher_size))) {
            return;
        }

        rcv_message_cipher = vector<unsigned char>(cipher_size);
        if (!recvAll(client_fd, rcv_message_cipher.data(), cipher_size)) {
            return;
        }

        rcv_file_message = decrypt_file(rcv_message_cipher, key, iv);
        // cout << "[File] rcv_file_message size: " << rcv_file_message.size() << endl;
        // cout << "[File] rcv_file_message: ";
        // for(int i = 0; i < rcv_file_message.size(); i++){
        //     printf("%02x", rcv_file_message[i]);
        // }
        cout << endl;

        if (rcv_file_message.empty()) {
            return;
        }

        if (!sendAll(receiver_fd, reinterpret_cast<unsigned char*>(&cipher_size), sizeof(cipher_size))) {
            return;
        }

        if (!sendAll(receiver_fd, rcv_message_cipher.data(), cipher_size)) {
            return;
        }

        file_size -= rcv_file_message.size();
    }
    return;
}

bool sendAll(int client_fd, const unsigned char *data, size_t size) {
    size_t total_sent = 0;
    while (total_sent < size) {
        ssize_t sent_bytes = send(client_fd, data + total_sent, size - total_sent, 0);
        if (sent_bytes < 0) {
            printError("sendAll failed");
            return false;
        }
        total_sent += sent_bytes;
    }
    return true;
}

bool recvAll(int client_fd, unsigned char *data, size_t size) {
    size_t total_received = 0;
    while (total_received < size) {
        ssize_t recv_bytes = recv(client_fd, data + total_received, size - total_received, 0);
        if (recv_bytes <= 0) {
            printError("recvAll failed");
            return false;
        }
        total_received += recv_bytes;
    }
    return true;
}