//
// Created by myhay on 22/04/16.
//

#ifndef PRUEBASUDP_UDPSOCKET_H
#define PRUEBASUDP_UDPSOCKET_H

#include <string>
#include <vector>
#include <functional>
#include <cstring>
#include <thread>
#include <queue>
#include <iostream>
#include <csignal>

#ifdef WIN32
#include <Winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#endif

enum PacketMode
{
    Reliable = 0,
    Unrealiable
};

struct PacketData
{
    bool Reliable;
    std::vector<unsigned char> Data;
};



class UDPSocket
{
    private:
        std::function<void(std::vector<unsigned char>)> _callback;

        int _fd;
        sockaddr_in _addr;
        sockaddr_in _ep;

        bool _connected;

        std::thread _queueThread;
        std::queue<PacketData> _sendQueue;

        friend void CheckQueue(UDPSocket*);

    public:
        UDPSocket();
        ~UDPSocket();

        void Connect(const std::string & ip, unsigned short port);
        void Disconnect();

        void Send(const std::vector<unsigned char> & data, bool isReliable = false);
        void SetRecvCallback(const std::function<void(std::vector<unsigned char>)> & callback);
};

void CheckQueue(UDPSocket* udpPtr);

#endif //PRUEBASUDP_UDPSOCKET_H
