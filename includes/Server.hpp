#ifndef SERVER_HPP
#define SERVER_HPP



#include "Client.hpp"

class Server {
    private:
        int _listenFd;
        int _port;
        bool _running;
        std::string _password;
        int _epollfd;

        std::vector<struct pollfd> _pfds;
        std::map<int, std::string> _inBuffers;
		sockaddr_in ServerAdr;

    public:
        Server(std::string port, const std::string& password);
        ~Server();
        void run();
        void setupSocket();
        void acceptClient();
        void handleClientRead(int fd);
        void removeClient(int fd);
};

#endif
