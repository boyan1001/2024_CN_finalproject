#include "audio.hpp"
#include "UI.hpp"

static const int LISTEN_PORT = 8888;   // 與 Server 設定的目標 port 一致
static const int AUDIO_SAMPLES = 1024; // 每次音訊回呼的 frame 數量
static const int BUFFER_SIZE = 4096;   // 接收 buffer 大小（可依實際需求調整）

queue<vector<Uint8>> g_audioQueue;
mutex g_audioQueueMutex;

struct sockaddr_in g_clientAddr;
socklen_t g_clientAddrLen = sizeof(g_clientAddr);

static bool quit = false;

// 音訊回呼：從 g_audioQueue 中取出資料
void audioPlaybackCallback(void *userdata, Uint8 *stream, int len)
{
    // 清空回呼用的輸出 buffer（避免殘留雜音）
    SDL_memset(stream, 0, len);

    // 取出佇列中的封包資料
    lock_guard<mutex> lock(g_audioQueueMutex);
    if (!g_audioQueue.empty())
    {
        auto &frontPacket = g_audioQueue.front();
        if ((int)frontPacket.size() >= len)
        {
            // 將資料拷貝到播放緩衝
            memcpy(stream, frontPacket.data(), len);
            // 將已播放的部分從封包移除
            frontPacket.erase(frontPacket.begin(), frontPacket.begin() + len);

            // 如果此封包資料已全部播放完，則 pop
            if (frontPacket.empty())
            {
                g_audioQueue.pop();
            }
        }
        else
        {
            // 如果封包資料不夠一次回呼需要的大小
            memcpy(stream, frontPacket.data(), frontPacket.size());
            // 這個封包已全部播放完，pop
            g_audioQueue.pop();
        }
    }
}

void audioCaptureCallback(void *userdata, Uint8 *stream, int len)
{
    int receiver_fd = *reinterpret_cast<int *>(userdata);
    // len 表示這次捕捉到的音訊資料大小 (byte)，stream 裝著音訊資料
    // 直接以 UDP 送到對端
    if (receiver_fd >= 0)
    {
        // 將音訊資料傳給 Client
        sendto(receiver_fd, stream, len, 0, reinterpret_cast<struct sockaddr *>(&g_clientAddr), g_clientAddrLen);
    }
}

void *audioRecvThread(void *arg)
{
    int sockfd = *reinterpret_cast<int *>(arg);

    char buffer[BUFFER_SIZE];
    struct sockaddr_in srcAddr;
    socklen_t srcAddrLen = sizeof(srcAddr);

    while (!quit)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        struct timeval tv; // timeout
        tv.tv_sec = 0;
        tv.tv_usec = 50000; // 50ms

        int ret = select(sockfd + 1, &readfds, nullptr, nullptr, &tv);
        if (ret < 0)
        {
            cerr << "Select failed" << endl;
            break;
        }
        else if (ret == 0)
        {
            continue;
        }

        if (!FD_ISSET(sockfd, &readfds))
            continue;

        ssize_t received = recvfrom(
            sockfd, buffer, BUFFER_SIZE, 0,
            reinterpret_cast<struct sockaddr *>(&srcAddr), &srcAddrLen);

        if (received <= 0)
        {
            // <= 0 表示 socket 關閉、錯誤或對端中斷
            // 視需求決定是否 break
            break;
        }

        string rcv_message(buffer, received);
        if (rcv_message == "[Chatting][Audio] <end>")
        {
            quit = true;
            break;
        }

        if (received > 0)
        {
            // 將收到的資料放到 queue
            vector<Uint8> packet(received);
            memcpy(packet.data(), buffer, received);

            lock_guard<mutex> lock(g_audioQueueMutex);
            g_audioQueue.push(move(packet));
        }
    }

    return nullptr; // pthread 規定必須回傳 void*
}

bool receiverAudio(int sender_fd, string sender, queue<string> &chatting_message)
{

    // initialize quit
    quit = false;

    // init SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        cerr << "SDL_Init Error: " << SDL_GetError() << endl;
        return false;
    }

    // build recv thread
    pthread_t recvThread;
    if (pthread_create(&recvThread, nullptr, audioRecvThread, &sender_fd) < 0)
    {
        cerr << "Failed to create a thread" << endl;
        SDL_Quit();
        return false;
    }

    // sdl audio
    SDL_AudioSpec wantSpec, haveSpec;
    SDL_zero(wantSpec);
    wantSpec.freq = 48000;
    wantSpec.format = AUDIO_S16LSB; // 16-bit
    wantSpec.channels = 2;
    wantSpec.samples = AUDIO_SAMPLES;
    wantSpec.callback = audioPlaybackCallback;

    SDL_AudioDeviceID playbackDevice = SDL_OpenAudioDevice(nullptr, 0, &wantSpec, &haveSpec, 0);

    if (playbackDevice == 0)
    {
        cerr << "Failed to open audio: " << SDL_GetError() << endl;
        quit = true;
        SDL_Quit();
        return false;
    }

    // playing audio
    SDL_PauseAudioDevice(playbackDevice, 0);

    while (!quit)
    {
        // print chatroom
        printChatRoom(sender, chatting_message);
        cout << endl;
        cout << "Receiving audio from \033[1m" << sender << "\033[0m" << endl;

        for (int i = 0; i < 10; i++)
        {
            if (quit)
                break;
            cout << "." << flush;
            SDL_Delay(1000);
        }
    }

    // end receiving
    SDL_PauseAudioDevice(playbackDevice, 1);
    SDL_CloseAudioDevice(playbackDevice);

    // end recv thread
    pthread_join(recvThread, nullptr);
    SDL_Quit();
    cout << "Audio receiver stopped." << endl;

    return true;
}

bool senderAudio(int receiver_fd, string target)
{
    // initialize quit
    quit = false;

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        cerr << "SDL_Init Error: " << SDL_GetError() << endl;
        return false;
    }

    // sdl audio
    SDL_AudioSpec wantSpec, haveSpec;
    SDL_zero(wantSpec);
    wantSpec.freq = 48000;
    wantSpec.format = AUDIO_S16LSB; // 16-bit
    wantSpec.channels = 2;
    wantSpec.samples = AUDIO_SAMPLES;
    wantSpec.userdata = &receiver_fd;
    wantSpec.callback = audioCaptureCallback;

    SDL_zero(haveSpec);
    SDL_AudioDeviceID captureDevice = SDL_OpenAudioDevice(nullptr, 1, &wantSpec, &haveSpec, 0);

    if (captureDevice == 0)
    {
        cerr << "Failed to open audio: " << SDL_GetError() << endl;
        SDL_Quit();
        return false;
    }

    cout << "Audio capture device opened." << endl;

    // start capture
    SDL_PauseAudioDevice(captureDevice, 0);
    cout << "Start capturing audio and sending via UDP..." << endl;
    cout << endl;
    cout << ">>> Press ENTER to stop" << endl;
    cin.get();

    // end capture
    SDL_PauseAudioDevice(captureDevice, 1);
    SDL_CloseAudioDevice(captureDevice);
    SDL_Quit();

    // send end signal
    string end_signal = "[Chatting][Audio] <end>";
    sendto(receiver_fd, end_signal.data(), end_signal.size(), 0, reinterpret_cast<struct sockaddr *>(&g_clientAddr), g_clientAddrLen);

    cout << "Audio sender stopped." << endl;
    return true;
}

bool handleAudio(int sender_fd, int receiver_fd)
{
    // initialize quit
    quit = false;

    char buffer[4096];
    ssize_t bytes_received;

    while (1)
    {
        ssize_t bytes_received = recvfrom(sender_fd, buffer, 4096, 0, reinterpret_cast<struct sockaddr *>(&g_clientAddr), &g_clientAddrLen);
        if (bytes_received < 0)
        {
            cerr << "Receiving data from the client" << endl;
            return false;
        }

        string rcv_message(buffer, bytes_received);
        if (rcv_message == "[Chatting][Audio] <end>")
        {
            sendto(receiver_fd, buffer, bytes_received, 0, reinterpret_cast<struct sockaddr *>(&g_clientAddr), g_clientAddrLen);
            break;
        }
        if (bytes_received > 0)
        {
            sendto(receiver_fd, buffer, bytes_received, 0, reinterpret_cast<struct sockaddr *>(&g_clientAddr), g_clientAddrLen);
        }

        memset(buffer, 0, sizeof(buffer));
    }
    return true;
}