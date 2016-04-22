//
// Created by myhay on 22/04/16.
//

#include "UDPSocket.h"


UDPSocket::UDPSocket() : _connected(false)
{
    // Inicialización socket
    _fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (_fd < 0)
    {
        throw std::runtime_error("Could not create UDP socket.");
    }

    // Definimos el socket como no bloqueante
    fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL, 0) | O_NONBLOCK);

    memset(&_addr, 0, sizeof(sockaddr_in));
}

UDPSocket::~UDPSocket()
{
    if (_connected)
    {
        Disconnect();
    }
}

void UDPSocket::Connect(const std::string & ip, unsigned short port)
{
    // Definimos puerto
    _ep.sin_family = AF_INET;
    _ep.sin_addr.s_addr = inet_addr(ip.c_str());
    _ep.sin_port = htons(port);

    // Definimos nuestra estructura de endpoint
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = INADDR_ANY;

    _connected = true;

    if (bind(_fd, (sockaddr*) &_addr, sizeof(sockaddr_in)) < 0)
    {
        std::cout << errno << std::endl;
    }

    // Iniciamos el hilo
    _queueThread = std::thread(CheckQueue, this);
}

void UDPSocket::Disconnect()
{
    if (_connected)
    {
        _connected = false;
        _queueThread.join();
    }
}

void UDPSocket::Send(const std::vector<unsigned char> & data, bool isReliable)
{
    PacketData packet;

    packet.Reliable = isReliable;
    packet.Data.resize(data.size() + 1);

    if (isReliable)
    {
        packet.Data[0] = PacketMode::Reliable;
    }
    else
    {
        packet.Data[0] = PacketMode::Unrealiable;
    }

    packet.Data.insert(packet.Data.begin() + 1, data.begin(), data.end());
    _sendQueue.push(packet);
}

void UDPSocket::SetRecvCallback(const std::function<void(std::vector<unsigned char>)> &  callback)
{
    _callback = callback;
}

void CheckQueue(UDPSocket *udpPtr)
{
    while (udpPtr->_connected)
    {
        // Si hay mensajes para enviar
        if (!udpPtr->_sendQueue.empty())
        {
            std::vector<unsigned char> buf = udpPtr->_sendQueue.front().Data;

            std::cout << "[SEND] (" << buf.size() << ") " << std::string((char*)&buf[0]) << std::endl;

            sendto(udpPtr->_fd, &buf[0], buf.size(), 0, (sockaddr*) &udpPtr->_ep, sizeof(udpPtr->_ep));
            udpPtr->_sendQueue.pop();
        }

        // Si hay mensajes para recibir
        std::vector<unsigned char> tmpBuf(1024);
        sockaddr_in clientAddr;
        socklen_t slen = sizeof(clientAddr);
        memset(&clientAddr, 0, sizeof(sockaddr_in));

        ssize_t sz = recvfrom(udpPtr->_fd, &tmpBuf[0], tmpBuf.size(), 0, (sockaddr*) &clientAddr, &slen);

        if (sz > 0)
        {
            tmpBuf.resize(sz);
            udpPtr->_callback(tmpBuf);
        }
    }
}