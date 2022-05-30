#include "server.h"

namespace rv_server
{

    Server::Server()
    {
#ifdef _WIN32
        if ( ! init_win_lib () ) { win_lib_ok = false; }
#endif
    };

    void Server::run()
    {

#ifdef _WIN32
        if (!win_lib_ok)
        {
            std::cout << "Win lib init error" << std::endl;
            return;
        }
#endif

        init_server ();

        if (!address_bound)
        {
            std::cout << "Address binding error" << std::endl;
            return;
        }

        FD_ZERO(&master);
        FD_SET(s_listen, &master);

        std::cout << "All system ready ..." << std::endl;

        run_loop();

        shutting_down_server();
    }

    void Server::run_loop()
    {
        while (running)
        {
            fd_set copy = master;

            int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
#ifdef _WIN32
            for (int i = 0; i < socketCount; i++)
            {
                unsigned int sock = copy.fd_array[i];
                handle_fired_socket(sock);
            }
#else
            handle_fired_socket(socketCount);
#endif
        }
    }

    void Server::handle_fired_socket( unsigned int socket )
    {
        if (socket == s_listen)
        {
            unsigned int client = accept(s_listen, nullptr, nullptr);
            FD_SET(client, &master);
            std::string welcomeMsg = "Welcome to the Awesome Chat Server!";
            send(client, welcomeMsg.c_str(), (int)(welcomeMsg.size() + 1), 0);
        }
        else
        {
            handle_client_request ( socket );
        }
    }

    void Server::shutting_down_server()
    {
        std::cout << "Shutting down the server" << std::endl;
        // Remove the listening socket from the master file descriptor set and close it
        // to prevent anyone else trying to connect.
        FD_CLR(s_listen, &master);
        close_server_socket(s_listen);

        // Message to let users know what's happening.
        std::string msg = "Server is shutting down. Goodbye\r\n";

#ifdef _WIN32
        while (master.fd_count > 0)
        {
            // Get the socket number
            unsigned int sock = master.fd_array[0];

            // Send the goodbye message
            send(sock, msg.c_str(), msg.size() + 1, 0);

            // Remove it from the master file list and close the socket
            FD_CLR(sock, &master);
            close_server_socket(sock);
        }

        // Cleanup winsock
        WSACleanup();
#endif
    }

    void Server::handle_client_request( int sock )
    {
        char buf[4096];

        memset(buf, NULL, 4096);

        // Receive message
        int bytesIn = recv(sock, buf, 4096, 0);
        if (bytesIn <= 0)
        {
            // Drop the client
            close_server_socket(sock);
            FD_CLR(sock, &master);
        }
        else
        {
            // Check to see if it's a command. \quit kills the server
            if (buf[0] == '\\')
            {
                // Is the command quit?
                std::string cmd = std::string(buf, bytesIn);
//                if (cmd == "\\quit")
                if (buf[1] == 'q')
                {
                    running = false;
                    return;
                }
                // Unknown command
                std::cout << "cmd: " << cmd << ", running: " << running << std::endl;
            }

            std::cout << "cmd from: " << sock << ", running: " << running << std::endl;

            // Send message to other clients, and definiately NOT the listening socket
#ifdef _WIN32
            for (int i = 0; i < master.fd_count; i++)
            {
                unsigned int outSock = master.fd_array[i];
                if (outSock != s_listen && outSock != sock)
                {
                    std::ostringstream ss;
                    ss << "SOCKET #" << sock << ": " << buf << "\r\n";
                    std::string strOut = ss.str();

                    send(outSock, strOut.c_str(), strOut.size() + 1, 0);
                }
            }
#endif
        }
    }

    void Server::init_server ()
    {
#ifdef _WIN32
        SOCKADDR_IN address;
#else
        struct sockaddr_in address;
#endif
        address.sin_addr.s_addr = inet_addr("127.0.0.1");
        address.sin_port = htons(1111);
        address.sin_family = AF_INET;

        s_listen = socket( AF_INET, SOCK_STREAM, NULL );
#ifdef _WIN32
        const int is_bound = bind( s_listen, ( SOCKADDR * ) & address, sizeof ( address ) );
#else
        const int is_bound = bind( s_listen, ( struct sockaddr * ) & address, sizeof ( address ) );
#endif
        if (is_bound != 0) {
            address_bound = false;
        } else {
            listen(s_listen, SOMAXCONN);
        }
    }

    void Server::close_server_socket( unsigned int socket )
    {
#ifdef _WIN32
        closesocket(socket);
#else
        pclose(socket);
#endif
    }

#ifdef _WIN32
    bool Server::init_win_lib ()
    {
        WSAData wsaData;
        WORD DLLVersion = MAKEWORD(2,1);
        if ( WSAStartup ( DLLVersion, &wsaData ) != 0 )
            return false;
        return true;
    }
#endif

};
