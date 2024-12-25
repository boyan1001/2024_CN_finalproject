#include "code/src/crypt.hpp"
#include <iostream>
using namespace std;

static unsigned char key[32];
static unsigned char iv[16];

int main(){
    generateAESKeyAndIV(key, iv);
    string test_message = "Test Username Message";
    string encrypted = encrypt(test_message, key, iv);
    cout << "Encrypted: " << encrypted << endl;
    string decrypted = decrypt(encrypted, key, iv);
    cout << "Decrypted: " << decrypted << endl;

    if (decrypted == test_message)
    {
        cout << "Encryption/Decryption successful!" << endl;
    }
    else
    {
        cerr << "Encryption/Decryption failed. Result: " << decrypted << endl;
    }
    return 0;
}