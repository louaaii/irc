#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <cstring>
#include <cstdlib>
#include <cerrno>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "Client.hpp"

class Server {
    private:
        int _listenFd;
        int _port;
        bool _running;
        std::string _password;
        int _epollfd;

        std::map<int, Client> Clients;
		sockaddr_in ServerAdr;
		std::map<int, std::string> _inBuffers;
		std::map<int, std::string> _outBuffers;

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
