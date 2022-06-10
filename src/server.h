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

    class RVServer
    {
    public:
        RVServer(std::string  address, unsigned short port);
        RVServer(std::string  address);
        RVServer() = default;
        int run_server();

    private:
        unsigned short m_port = 8000;
        std::string m_address = "127.0.0.1";

        unsigned int m_server_socket = 0;
        bool m_is_server_running = true;
        fd_set m_socket_set{};

        int init_server();
        void run_operating_loop();
        static void shutdown();
        void handle_accept();

        void handle_client_request(unsigned int sock);
        static bool get_request(unsigned int sock, std::string & command) ;
        void send_message(const std::string & string_command);
    };
}
