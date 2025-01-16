# 2024 Computer Network Final Project  
![contributor](https://img.shields.io/github/contributors/boyan1001/2024_CN_finalproject?style=for-the-badge)
![license](https://img.shields.io/github/license/boyan1001/2024_CN_finalproject?style=for-the-badge)
![Last Commit](https://img.shields.io/github/last-commit/boyan1001/2024_CN_finalproject?style=for-the-badge)
![Pull Requests](https://img.shields.io/github/issues-pr/boyan1001/2024_CN_finalproject?style=for-the-badge)
![C++](https://img.shields.io/badge/C++-00599C.svg?logo=c%2B%2B&logoColor=white&style=for-the-badge)

這是我在**計算機網路**課程的期末專題  

此專案是一個基於 socket programming 的簡單伺服器-客戶端應用程式  
  
## 🧱 結構

```sh
2024_CN_finalproject
├── /code/
│  ├── /data/  
│  │  ├── /client/       用戶資料存放區，用於檔案傳輸
│  │  └── /server/
│  │     └── account.csv server-side 儲存用戶資訊，用於登入檢查
│  ├── /src/             其他主程式會用到的函式庫
│  │  ├── UI.cpp         有關TUI設計的函式庫
│  │  ├── crypt.cpp      有關加密的函式庫  
│  │  ├── file.cpp       有關檔案傳輸的函式庫  
│  │  └── audio.cpp      有關 audio streaming 的函式庫  
│  ├── client.cpp        client-side 的主要函式庫
│  ├── client.hpp
│  ├── clientmain.cpp    client-side 的主程式
│  ├── server.cpp        server-side 的主要函式庫
│  ├── server.hpp
│  ├── servermain.cpp    server-side 的主程式  
│  └── Makefile
├── .gitignore
├── LICENSE
└── README.md
```

## 🖥️ 要求    

若要順利的執行程式，有以下幾點是需要注意的  

### 作業系統  

本程式可在 **Unix-based** 系統（如 Linux 或 macOS）上順利執行  

如果使用 Windows，可以使用 **WSL** 來模擬環境。  

### 編譯器  

本程式全都由 **C++** 完成  

因此請先確保你的環境是否有安裝 **g++**
```sh
g++ --version
```
若無安裝，可透過下方方式安裝  
```sh
sudo apt install g++
```
### 函式庫  
請先透過下方指令下載 **OpenSSL** 與 **SDL2** 函式庫：  
```sh
sudo apt-get update
sudo apt-get install libsdl2-dev
sudo apt install openssl
```
  
另外，確認環境支援下列 header file：
- `<bits/stdc++.h>`（C++ 中大多基本函式庫）。
- `<fstream>`、`<sstream>`（用於資料的讀出讀入）。 
- `<sys/socket.h>`、`<netinet/in.h>`、`<arpa/inet.h>`（用於 socket 程式設計和網路通訊）。

## ⚙️ 如何使用  

在 `code` 目錄下執行 `make` 命令即可編譯  
  
`./server.o <server port>` 可以在 server port 開啟 server    
  
`./client.o <server ip or server domain> <server port>` 可開啟 client，並將其與 `<server ip or server domain>:<server port>` 上的 server 連接  
(可直接輸入 server ip 或輸入 server 所在的網域，程式會把 server domain 轉換成 server ip)  

### Server 端

啟動 server 後 server 便會進入 listen 模式  
  
等待 client 端與其連接  

連接完成後，server 僅負責回應 client 的要求（如查找現在使用帳號名稱、用戶登入與登出……等）  

不會主動向 client 傳訊息  

### Client 端

啟動 client 前，請先確認 server 在運作狀態中，之後再啟動 client

啟動 client 後，會先與 server 連接成功後，才會進入主頁面。  

![image](https://github.com/user-attachments/assets/4d0c9101-56dd-4925-9ccf-8ee198bdaff1)


#### 註冊帳號  

註冊帳號與密碼時，需要符合以下條件才可註冊：
- 不可出現冒號 :   
- 不可出現空格
  
若出現不合法的帳戶名與密碼，將會出現錯誤訊息  

若註冊成功後，使用者**帳號**與**密碼**將會保存至 server 端的 `./code/data/server/account.csv`，用來作為註冊與登入檢查  

#### 登入帳號  

在主畫面按下 `1` 之後，即可進入登入帳號介面  

欲登入帳號，必須先確認該帳號已註冊，才可登入帳號  

否則將跳出錯誤訊息要求重新輸入  
  
登入完成帳號之後，**client 端**便會建立資料夾 `./code/data/client/[Your username]/`  

若要傳輸檔案，可將檔案放入這個資料夾內  

此外，**server 端**程式會將用戶名名稱存放至 `static unordered_map<string, int> name_to_fd`，用來記錄該帳戶所分配到的 socket  

#### 登出帳號

登入完成帳號後，回到主畫面  

按下 `2` 之後，即可登出帳號  

#### 聊天模式  

登入完成後，可依照主畫面提示進入  **chatroom**

![image](https://github.com/user-attachments/assets/d8d3d6b8-5d14-49e0-8298-9ad3404dc1cc)

輸入你想聊天的對象，等待對方回應你的邀請  

對方須同樣在 **chatroom** 中輸入你的名字  

邀請成功需要滿足以下條件：
1. 雙方都必須**在線上**  
2. 雙方都需要在 **chatroom** 中輸入對方名字
3. 雙方必須在都**空閒**著，沒有在跟其他人對話

邀請成功後會出現下方圖片，按下 `Enter` 後即可開始聊天  
  
![image](https://github.com/user-attachments/assets/a246e62b-00ce-4c32-8b75-63616bc860f8)

當對方**不在線上**，程式會出現下方畫面，按下 `Enter` 後可退回主頁面  
  
![image](https://github.com/user-attachments/assets/f285fe25-5168-41a6-a58c-77478884f6b6)  

當對方**在線上但尚未在 chatroom、尚未輸入你的名字或尚未空閒**時，會出現下方畫面  
  
![image](https://github.com/user-attachments/assets/217a37e9-1e46-4f40-bfb4-a9f92acb9dc1)  

按下 'Enter' 後會詢問你要**等待**還是**離開**  
  
![image](https://github.com/user-attachments/assets/0a30b092-ae02-4485-b5af-50316f2bed2a)

若選擇**離開**，即回到主畫面  

若選擇**等待**，則進入等待畫面。  
  
![image](https://github.com/user-attachments/assets/2d179c06-25ba-4c95-853b-c67baed11884)

當對方輸入你的名字後，就可以開始聊天了  

#### 加密  
本程式除了 **Audio streaming** 外，皆使用 **OpenSSL** 中的 **AES 加密**，內容大致如下：  
- **加密一般訊息用**  
```cpp
vector<unsigned char> encrypt(const string &plain_text, const unsigned char *key, const unsigned char *iv)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        cerr << "EVP_CIPHER_CTX_new failed" << endl;
        return {};
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
    {
        cerr << "EVP_EncryptInit_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    int len = static_cast<int>(plain_text.size());
    int block_size = EVP_CIPHER_block_size(EVP_aes_256_cbc());
    int max_len = len + block_size;

    vector<unsigned char> cipher_text(max_len);
    int update_len = 0, final_len = 0;

    if (EVP_EncryptUpdate(ctx, cipher_text.data(), &update_len, reinterpret_cast<const unsigned char *>(plain_text.data()), len) != 1)
    {
        cerr << "EVP_EncryptUpdate failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    if (EVP_EncryptFinal_ex(ctx, cipher_text.data() + update_len, &final_len) != 1)
    {
        cerr << "EVP_EncryptFinal_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_free(ctx);

    // 修正 vector 長度為加密後的實際長度
    cipher_text.resize(update_len + final_len);

    // cout << "Encrypted size: " << cipher_text.size() << endl;
    return cipher_text;
}
```
- **解密一般訊息用**
```cpp
string decrypt(const vector<unsigned char> &origin_cipher_text, const unsigned char *key, const unsigned char *iv)
{

    if (origin_cipher_text.size() % 16 != 0)
    {
        cerr << "Cipher length is not a multiple of block size, decryption aborted." << endl;
        return {};
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        cerr << "Failed to create EVP_CIPHER_CTX" << endl;
        return {};
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
    {
        cerr << "EVP_DecryptInit_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    int len = static_cast<int>(origin_cipher_text.size());
    int block_size = EVP_CIPHER_block_size(EVP_aes_256_cbc());
    int max_len = len + block_size;

    vector<unsigned char> plain_text(max_len, 0);
    int update_len = 0, final_len = 0;

    if (EVP_DecryptUpdate(ctx, plain_text.data(), &update_len, origin_cipher_text.data(), len) != 1)
    {
        cerr << "EVP_DecryptUpdate failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    if (EVP_DecryptFinal_ex(ctx, plain_text.data() + update_len, &final_len) != 1)
    {
        cerr << "EVP_DecryptFinal_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_free(ctx);

    plain_text.resize(update_len + final_len);

    return string(plain_text.begin(), plain_text.end());
}
```
- **加密檔案用**
```cpp
vector<unsigned char> encrypt_file(const vector<unsigned char> &plain_text, const unsigned char *key, const unsigned char *iv)
{
    // 建立 Context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        cerr << "EVP_CIPHER_CTX_new failed" << endl;
        return {};
    }

    // 初始化加密：AES-256-CBC
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1)
    {
        cerr << "EVP_EncryptInit_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 準備足夠的輸出空間：明文長度 + 區塊大小
    int len = static_cast<int>(plain_text.size());
    int block_size = EVP_CIPHER_block_size(EVP_aes_256_cbc());
    int max_len = len + block_size;

    vector<unsigned char> cipher_text(max_len);
    int update_len = 0, final_len = 0;

    // 加密 Update
    if (EVP_EncryptUpdate(ctx, cipher_text.data(), &update_len, plain_text.data(), len) != 1)
    {
        cerr << "EVP_EncryptUpdate failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 加密 Final
    if (EVP_EncryptFinal_ex(ctx, cipher_text.data() + update_len, &final_len) != 1)
    {
        cerr << "EVP_EncryptFinal_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 釋放 Context
    EVP_CIPHER_CTX_free(ctx);

    // 根據實際加密長度縮小 cipher_text
    cipher_text.resize(update_len + final_len);

    return cipher_text;
}
```

- **解密檔案用**
```cpp
string decrypt(const vector<unsigned char> &origin_cipher_text, const unsigned char *key, const unsigned char *iv)
{

    if (origin_cipher_text.size() % 16 != 0)
    {
        cerr << "Cipher length is not a multiple of block size, decryption aborted." << endl;
        return {};
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        cerr << "Failed to create EVP_CIPHER_CTX" << endl;
        return {};
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
    {
        cerr << "EVP_DecryptInit_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    int len = static_cast<int>(origin_cipher_text.size());
    int block_size = EVP_CIPHER_block_size(EVP_aes_256_cbc());
    int max_len = len + block_size;

    vector<unsigned char> plain_text(max_len, 0);
    int update_len = 0, final_len = 0;

    if (EVP_DecryptUpdate(ctx, plain_text.data(), &update_len, origin_cipher_text.data(), len) != 1)
    {
        cerr << "EVP_DecryptUpdate failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    if (EVP_DecryptFinal_ex(ctx, plain_text.data() + update_len, &final_len) != 1)
    {
        cerr << "EVP_DecryptFinal_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_free(ctx);

    plain_text.resize(update_len + final_len);

    return string(plain_text.begin(), plain_text.end());
}
```

- **生成 AES key 與 AES iv**
```cpp
void generateAESKeyAndIV(unsigned char *key, unsigned char *iv)
{
    if (RAND_bytes(key, 32) != 1)
    {
        cerr << "Failed to generate AES key" << endl;
        exit(EXIT_FAILURE);
    }

    if (RAND_bytes(iv, 16) != 1)
    {
        cerr << "Failed to generate AES IV" << endl;
        exit(EXIT_FAILURE);
    }
}
```

#### 一般傳輸訊息
![image](https://github.com/user-attachments/assets/63152943-7571-42de-9771-0d746af67b3c)  
直接在輸入區輸入想傳輸的訊息即可  

傳輸訊息的流程大致如下：　　
- 先包裝**寄件人**的訊息，附上相關資訊  
- 經過 **AES 加密**
- 加密訊息傳至 **server**
- 加密訊息在 **server** 中解密，取得**寄件人**與**收件人**帳戶名
- 利用帳戶名在 `static unordered_map<string, int> name_to_fd` 中找到對應 socket
- 訊息加密後傳輸到**收件人**
- **收件人**解密訊息，解開包裝得到訊息

訊息包裝大致如下：  
`[Chatting][Message] sender:receiver:message`  

#### 傳輸檔案  
請先把檔案放到 `./code/data/client/[Your username]/` 中  

然後輸入 `<file> [Your filename]`  

按下 Enter 後，等待片刻對方就會在對方的 `./code/data/client/[Receiver's username]/` 中收到檔案  

#### Audio streaming  
請在輸入區輸入 `<audio streaming>` 即可與對方開始 audio streaming  

想結束請依照提示按下 Enter 就可終止 audio streaming  

**只有發送方可以終止 audio streaming**  

#### 離開聊天室 
於輸入區輸入 `<exit>` 即可離開  

輸入完成後，對方聊天室也會顯示你已離開　　

對方也會同時退出　　

## 🪪 Lisence  
[MIT](LICENSE) © Hank Chen  
