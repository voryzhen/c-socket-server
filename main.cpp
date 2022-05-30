//#include "src/server.h"
//
//int main ()
//{
//    rv_server::Server s;
//    s.run();
//}



#include<cstdio>
#include<cstring>


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

void * get_in_addr(struct sockaddr * sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

#ifdef _WIN32

///=================================================================================================
/// <summary>	Determines if winsock is initialized. </summary>
///
/// <remarks>	mtvee, 2017-03-30. </remarks>
///
/// <returns>	True if it is already done. </returns>
///=================================================================================================

bool WinsockInitialized()
{
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET && WSAGetLastError() == WSANOTINITIALISED) {
        return false;
    }

    closesocket(s);
    return true;
}
#endif

int main(int argc, char * argv[])
{
    // Variables for writing a server.
    /*
    1. Getting the address data structure.
    2. Openning a new socket.
    3. Bind to the socket.
    4. Listen to the socket.
    5. Accept Connection.
    6. Receive Data.
    7. Close Connection.
    */
    int status;
    struct addrinfo hints, *res;
    int listner;

#ifdef _WIN32
    //----------------------
    // Initialize Winsock.
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup failed with error: %ld\n", iResult);
        return 1;
    }
#endif


    // Before using hint you have to make sure that the data structure is empty
    memset(&hints, 0, sizeof hints);
    // Set the attribute for hint
    hints.ai_family = AF_UNSPEC; // We don't care V4 AF_INET or 6 AF_INET6
    hints.ai_socktype = SOCK_STREAM; // TCP Socket SOCK_DGRAM
    hints.ai_flags = AI_PASSIVE;

    // Fill the res data structure and make sure that the results make sense.
    status = getaddrinfo(NULL, "8888", &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    }

    // Create Socket and check if error occured afterwards
    listner = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listner < 0)
    {
        fprintf(stderr, "socket error: %s\n", gai_strerror(status));
    }

    // Bind the socket to the address of my local machine and port number
    status = bind(listner, res->ai_addr, res->ai_addrlen);
    if (status < 0)
    {
        fprintf(stderr, "bind: %s\n", gai_strerror(status));
    }

    status = listen(listner, 10);
    if (status < 0)
    {
        fprintf(stderr, "listen: %s\n", gai_strerror(status));
    }

    // Free the res linked list after we are done with it
    freeaddrinfo(res);


    // We should wait now for a connection to accept
    int new_conn_fd;
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    char s[INET6_ADDRSTRLEN]; // an empty string

    // Calculate the size of the data structure
    addr_size = sizeof client_addr;

    printf("I am now accepting connections ...\n");

    while (1) {
        // Accept a new connection and return back the socket desciptor
        new_conn_fd = accept(listner, (struct sockaddr *) & client_addr, &addr_size);
        if (new_conn_fd < 0)
        {
            fprintf(stderr, "accept: %s\n", gai_strerror(new_conn_fd));
            continue;
        }

        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), s, sizeof s);
        printf("I am now connected to %s \n", s);
        status = send(new_conn_fd, "Welcome", 7, 0);
        if (status == -1)
        {
#ifdef _WIN32
            closesocket(new_conn_fd);
            WSACleanup();
#else
            close(new_conn_fd);
#endif
            _exit(4);
        }

    }
    // Close the socket before we finish
#ifdef _WIN32
    closesocket(new_conn_fd);
#else
    close(new_conn_fd);
#endif

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}