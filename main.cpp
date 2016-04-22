#include <iostream>
#include "UDPSocket.h"

void OnMessageReceived(std::vector<unsigned char> data)
{
    std::cout << "[RECV] " << std::string((char*)&data[0]) << std::endl;
}

int main(int argc, char *argv[])
{
    std::string ip = argv[1];
    int port = 5555;
    bool quit = false;

    try
    {
        UDPSocket mySocket;
        mySocket.Connect(ip, port);
        mySocket.SetRecvCallback(OnMessageReceived);

        while (!quit)
        {
            std::string data;
            std::getline(std::cin, data);

            // Si encontramos exit, salimos
            if (data.find("exit") != std::string::npos)
            {
                quit = true;
            }
            else
            {
                std::vector<unsigned char> buf(data.length());
                buf.insert(buf.begin(), data.begin(), data.end());
                mySocket.Send(buf);
            }
        }
    }
    catch (const std::runtime_error & ex)
    {
        std::cout << "Socket error: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

