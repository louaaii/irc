#include "../includes/Server.hpp"

#define MAX_EVENT 10

Server::Server(std::string port, const std::string& password){
    _port = atoi(port.c_str());
    _password = password;
    _running = false;
    _listenFd = -1;

    if(_port <= 1024 || _port >= 65535)
        throw std::invalid_argument("Wrong port");
    if(_password.empty())
        throw std::invalid_argument("Empty password");
    for(std::string::iterator it = _password.begin(); it != _password.end(); it++){
        if(!std::isprint(*it))
            throw std::invalid_argument("Password must have printable values");
    }
    setupSocket();
}



Server::~Server(){
    if(_listenFd != -1)
        close(_listenFd);
    if(_epollfd != -1)
        close(_epollfd);
}



void Server::run(){
    struct epoll_event ev, events[MAX_EVENT];
    
    _epollfd = epoll_create1(0);
    if(_epollfd == -1)
        throw std::runtime_error("epoll_create1 failed");
    
    ev.events = EPOLLIN;
    ev.data.fd = _listenFd;
    if(epoll_ctl(_epollfd, EPOLL_CTL_ADD, _listenFd, &ev) == -1)
        throw std::runtime_error("epoll_ctl failed");
    
    _running = true;
    while(_running) {
        int nfds = epoll_wait(_epollfd, events, MAX_EVENT, -1);
        if(nfds == -1)
            throw std::runtime_error("epoll_wait failed");
        
        for(int i = 0; i < nfds; i++) {
            if(events[i].data.fd == _listenFd) {
                acceptClient();
            } else {
                handleClientRead(events[i].data.fd);
            }
        }
    }
    close(_epollfd);
}


void Server::setupSocket(){
    _listenFd = socket(AF_INET, SOCK_STREAM, 0);
    const int OPT = 1;
    if (_listenFd == -1)
        throw std::runtime_error("Error opening socket");
    
    int status = fcntl(_listenFd, F_SETFL, fcntl(_listenFd, F_GETFL, 0) | O_NONBLOCK);
    if (status == -1)
        throw std::runtime_error("fcntl failed");
    
    if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &OPT, sizeof(int)) == -1)
        throw std::runtime_error("setsockopt failed");
    
    ServerAdr.sin_family = AF_INET;
    ServerAdr.sin_port = htons(_port);
    ServerAdr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(_listenFd, (const sockaddr *)&ServerAdr, sizeof(ServerAdr)) == -1)
        throw std::runtime_error("bind failed");
    
    if (listen(_listenFd, SOMAXCONN) == -1)
        throw std::runtime_error("listen failed");
}

void Server::acceptClient(){
	try{
		accept();
	}
	catch (...){
		throw std::runtime_error("client");
	}
}


void Server::handleClientRead(int fd){
    
}   