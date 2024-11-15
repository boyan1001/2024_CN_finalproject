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
│  │  └── account.csv  用戶資料純放區
│  ├── /src/           其他主程式會用到的函式庫
│  │  ├── UI.cpp       有關TUI設計的函式庫
│  │  ├── UI.hpp
│  │  └── ...
│  ├── client.cpp      client端的程式碼
│  ├── client.hpp
│  ├── server.cpp      server端的程式碼
│  ├── server.hpp
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
本專案只需使用系統內建的標準 C++ 函式庫，因此無需安裝額外的函式庫。  
  
請確認環境支援下列 header file：
- `<bits/stdc++.h>`（C++ 中大多基本函式庫）。
- `<fstream>`、`<sstream>`（用於資料的讀出讀入）。 
- `<sys/socket.h>`、`<netinet/in.h>`、`<arpa/inet.h>`（用於 socket 程式設計和網路通訊）。

## ⚙️ 如何使用  

輸入 `make` 即可編譯   
  
`./server.o <server port>` 可以在 server port 開啟 server    
  
`./client.o <server ip> <server port>` 可開啟 client，並將其與 `<server ip>:<server port>` 上的 server 連接    
*(目前只能用 server ip **127.0.0.1**)*  

### Server 端

啟動 server 後 server 便會進入 listen 模式  
  
等待 client 端與其連接  

連接完成後，server 僅負責回應 client 的要求（如查找現在使用帳號名稱、用戶登入與登出……等）  

不會主動向 client 傳訊息  

### Client 端

*(本程式目前僅支援 1 個 client 與 1 個 server 連接)*

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

#### 登出帳號

登入玩帳號後，可選擇是否要登出帳號  

若不登出就退出 client 程式  

只要 server 仍在運作，重新開啟 client 端仍會在已登入狀態內  

## 🪪 Lisence  
[MIT](LICENSE) © Hank Chen  
