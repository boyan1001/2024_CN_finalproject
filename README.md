# 2024 Computer Network Final Project  

這是我在**計算機網路**課程的期末專題  

此專案是一個基於 socket programming 的簡單伺服器-客戶端應用程式  

本程式目前支援**用戶註冊、登入**等服務  
  
未來將開發成一個**實時線上聊天室**軟體  
  
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
  
`./client.o <server ip> <server port>` 可開啟 client，並將其與 `<server ip>:<server port>` 上的 server 連接    
*(目前只能用 server ip **127.0.0.1**)*  

### Server 端

啟動 server 後 server 便會進入 listen 模式  
  
等待 client 端與其連接  

連接完成後，server 僅負責回應 client 的要求（如查找現在使用帳號名稱、用戶登入與登出……等）  

不會主動向 client 傳訊息  

### Client 端

啟動 client 前，請先確認 server 在運作狀態中，之後再啟動 client

啟動 client 後，會先與 server 連接成功後，才會進入主頁面。  

#### 註冊帳號  

註冊帳號與密碼時，需要符合以下條件才可註冊：
- 不可出現冒號 :   
- 不可出現空格
  
若出現不合法的帳戶名與密碼，將會出現錯誤訊息  

#### 登入帳號  

欲登入帳號，必須先確認該帳號已註冊，才可登入帳號  

否則將跳出錯誤訊息要求重新輸入  
  
登入完成帳號之後，便會建立資料夾 `./code/data/client/[Your username]/`  

若要傳輸檔案，可將檔案放入這個資料夾內  

#### 登出帳號

登入完成帳號後，可選擇是否要登出帳號  

#### 聊天模式  

登入完成後，可依照主畫面提示進入聊天區  

輸入你想聊天的對象，等待對方回應你的邀請  

對方接受你的邀請並進入聊天室後，就可以開始聊天了  
  
#### 一般傳輸訊息
直接在輸入區輸入想傳輸的訊息即可  

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
