#ifndef SERVER_HPP
#define SERVER_HPP



#include "Client.hpp"

class Server {
    private:
        int _listenFd;
        int _port;
        bool _running;
        std::string _password;

    public:
        Server(std::string port, const std::string& password);
        ~Server();
        void removeClient(int fd);
        void run();
        void setupSocket();
        void acceptClient();
        void handleClientRead(int fd);
};

#endif