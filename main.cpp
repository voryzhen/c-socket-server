#include "iostream"

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<arpa/inet.h>
#endif

int main ()
{
#ifdef _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup failed with error: %ld\n", iResult);
        return 1;
    }
#endif

    struct sockaddr_in addrin;
    addrin.sin_family = AF_INET; // Установить семейство адресов
    addrin.sin_port = htons(800); // Установить порт
    addrin.sin_addr.s_addr = inet_addr("127.0.0.1"); // Установить адрес

    unsigned int sockfd = socket(AF_INET,SOCK_STREAM,0); // Получить ФД
    int bind_res = bind (sockfd, (struct sockaddr *) & addrin, sizeof (struct sockaddr));
    if (bind_res < 0)
    {
        std::cerr << "bind: " << bind_res << std::endl;
    }

    int listen_res = listen(sockfd, 10);
    if (listen_res < 0)
    {
        std::cerr << "listen: " << listen_res << std::endl;
    }

    printf("I am now accepting connections ...\n");

    while (1)
    {
        unsigned int client = accept(sockfd, nullptr, nullptr);
        std::string welcomeMsg = "Welcome to the Awesome Chat Server!!!!!!1111";
        send(client, welcomeMsg.c_str(), (int)(welcomeMsg.size() + 1), 0);
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
