#pragma once

#ifdef _WIN32

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#pragma warning(disable: 4996)

#else

#include <sys/socket.h>

#endif

#include "iostream"
#include "string"
#include "sstream"

namespace rv_server {

    class Server {

        public:

            Server();
            void run();

        private:

            bool win_lib_ok = true;
            bool address_bound = true;
            bool running = true;
            SOCKET s_listen;
            fd_set master;

#ifdef _WIN32
            bool init_win_lib ();
#endif

            void init_server ( SOCKADDR_IN & address );
            void handle_client_request( SOCKET sock );
            void shutting_down_server();

    };

};
