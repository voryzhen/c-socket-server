#include "server.h"

namespace rv_server {

    int RVserver::run_server()
    {
        if (init_server() != 0)
        {
            return -1;
        }
        std::cout << "I am now accepting connections ..." << std::endl;
        run_operating_loop();
        shutdown();
        return 0;
    }

    int RVserver::init_server()
    {

#ifdef _WIN32
        WSADATA wsaData;
        int wsa_startup_res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (wsa_startup_res != NO_ERROR) {
            std::cerr << "WSAStartup failed with error: " << wsa_startup_res << std::endl;
            return -1;
        }
#endif

        struct sockaddr_in addrin;
        addrin.sin_family = AF_INET;
        addrin.sin_port = htons(8000);
        addrin.sin_addr.s_addr = inet_addr("127.0.0.1");

        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        int bind_res = bind(server_socket, (struct sockaddr *) &addrin, sizeof(struct sockaddr));
        if (bind_res < 0) {
            std::cerr << "bind error: " << bind_res << std::endl;
            return -1;
        }

        int listen_res = listen(server_socket, SOMAXCONN);
        if (listen_res < 0) {
            std::cerr << "listen error: " << listen_res << std::endl;
            return -1;
        }

        return 0;

    }

    void RVserver::shutdown()
    {
#ifdef _WIN32
        WSACleanup();
#endif
    }

    void RVserver::run_operating_loop()
    {
        while (1)
        {
            unsigned int client = accept(server_socket, nullptr, nullptr);
            std::string welcomeMsg = "Welcome to the Awesome Cross Platform Chat Server!!!!!!1111";
            send(client, welcomeMsg.c_str(), (int) (welcomeMsg.size() + 1), 0);
        }
    }

};