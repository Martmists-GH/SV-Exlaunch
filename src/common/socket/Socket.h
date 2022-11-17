#pragma once

#include "nn/result.h"

#define PACKET_MAX_SIZE 0x500

enum class SocketState {
    UNINITIALIZED = 0,
    CONNECTED = 1,
    UNAVAILABLE = 2,
    DISCONNECTED = 3
};

class Socket {
public:
    Socket() = default;

    nn::Result init(const char* ip, u16 port);

    void sendMessage(const char* message);
    const char* receiveMessage();
    void close();

protected:
    SocketState mState;

private:
    int mSocketFd;
};

