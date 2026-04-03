#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cerrno>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

class Client{
    private:

        std::string _nickname;
        std::string _password;
        int socket;

    public:
        Client(fd);
        ~Client();

        get_fd();
        std::string get_nick();
        std::string get_pass();

        void set_fd();
        void set_nick();
        void set_pass();
};



#endif