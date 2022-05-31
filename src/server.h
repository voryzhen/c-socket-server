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

namespace rv_server{

    class RVserver
    {

        public:
            RVserver() = default;
            int run_server();

        private:

            unsigned int server_socket = 0;

            int init_server();
            void run_operating_loop();
            void shutdown();

    };

};