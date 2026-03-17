#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <vector>
#include <set>
#include <map>
#include <cctype>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <sys/epoll.h>
#include <signal.h>
#include <string>
#include <vector>
#include <sstream>


socket() → bind() → listen() → accept() → recv()/send()

class Server{
    private :
        int _listenfd;
        int _port;
        std::string _password;

    public : 
        Server(int port, const std::string& password);
        ~Server();
        void run();

};




#endif