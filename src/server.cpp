#include "server.h"

#include <string>
#include <utility>

namespace rv_server {

    RVServer::RVServer(std::string address, unsigned short port)
            : m_address(std::move(address)), m_port(port) {}

    int RVServer::run_server() {
        if (init_server() != 0)
            return -1;
        std::cout << "I am now accepting connections ..." << std::endl;
        run_operating_loop();
        shutdown();
        return 0;
    }

    int RVServer::init_server() {
#ifdef _WIN32
        WSADATA wsaData;
        int wsa_startup_res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (wsa_startup_res != NO_ERROR) {
            std::cerr << "WSAStartup failed with error: " << wsa_startup_res << std::endl;
            return -1;
        }
#endif
        // Set m_address
        struct sockaddr_in address_in{};
        address_in.sin_family = AF_INET;
        address_in.sin_port = htons(m_port);
        inet_pton(AF_INET, m_address.c_str(), &address_in.sin_addr.s_addr);

        // Create socket and bind m_address
        m_server_socket = socket(AF_INET, SOCK_STREAM, 0);
        int bind_res = bind(m_server_socket, (struct sockaddr *) &address_in, sizeof(struct sockaddr));
        if (bind_res < 0) {
            std::cerr << "bind error: " << bind_res << std::endl;
            return -1;
        }

        // Set socket to listen
        int listen_res = listen(m_server_socket, SOMAXCONN);
        if (listen_res < 0) {
            std::cerr << "listen error: " << listen_res << std::endl;
            return -1;
        }

        // Add server socket to set
        FD_ZERO(&m_socket_set);
        FD_SET(m_server_socket, &m_socket_set);

        return 0;
    }

    void RVServer::shutdown() {
#ifdef _WIN32
        WSACleanup();
#endif
    }

    void RVServer::run_operating_loop() {
        while (m_is_server_running) {
            fd_set copy_sock_set = m_socket_set;
#ifdef _WIN32
            int res = select(0, &copy_sock_set, nullptr, nullptr, nullptr);
            if ( res != SOCKET_ERROR && res > 0 ) {
                int loop_size = res;
#else
            int res = select(FD_SETSIZE, &copy_sock_set, nullptr, nullptr, nullptr);
            if ( res != SO_ERROR && res > 0 ) {
                int loop_size = FD_SETSIZE;
#endif
                for ( int i = 0; i < loop_size; i++ ) {
#ifdef _WIN32
                    unsigned int sock = copy_sock_set.fd_array[i];
#else
                    unsigned int sock = i;
#endif
                    if (FD_ISSET(sock, &copy_sock_set)) {
                        if (sock == m_server_socket) handle_accept();
                        else handle_client_request(sock);
                    }
                }
            }
        }
    }

    void RVServer::handle_accept() {
        unsigned int client = accept(m_server_socket, nullptr, nullptr);
        std::string welcome_msg = "Welcome to the Awesome Cross Platform Chat Server!!!!!!1111";
        send(client, welcome_msg.c_str(), (int) (welcome_msg.size() + 1), 0);
        FD_SET(client, &m_socket_set);
    }

    bool RVServer::get_request(unsigned int sock, std::string & command) {
        char msg[4096];
        int size = recv(sock, msg, 4096, 0);
        if (size > 0) {
            msg[size] = '\0';
            command = std::to_string(sock) + " says: " + std::string(msg) + ", " + std::to_string(size) + " bytes";
            return true;
        }
        return false;
    }

    void RVServer::handle_client_request(unsigned int sock) {
        std::string string_command;
        if (get_request(sock, string_command)) {
            send_message(string_command);
        } else {
#ifdef _WIN32
            closesocket(sock);
#else
            close(sock);
#endif
            FD_CLR(sock, &m_socket_set);
            std::string msg = std::to_string(sock) + " disconnected";
            std::cout << msg << std::endl;
#ifdef _WIN32
            for (int i = 0; i < m_socket_set.fd_count; i++ ) {
                unsigned int socket = m_socket_set.fd_array[i];
#else
            for (int i = 0; i < FD_SETSIZE; i++ ) {
                unsigned int socket = i;
#endif
                if (FD_ISSET(socket, &m_socket_set) && socket != m_server_socket) {
                    send(socket, msg.c_str(), (int) (msg.size() + 1), 0);
                }
            }
        }
    }

    void RVServer::send_message(const std::string &string_command) {
        std::cout << string_command << std::endl;
#ifdef _WIN32
        for (int i = 0; i < m_socket_set.fd_count; i++ ) {
            unsigned int sock = m_socket_set.fd_array[i];
#else
        for (int i = 0; i < FD_SETSIZE; i++ ) {
            unsigned int sock = i;
#endif
            if (FD_ISSET(sock, &m_socket_set) && sock != m_server_socket) {
                send(sock, string_command.c_str(), (int) (string_command.size() + 1), 0);
            }
        }
    }
}
