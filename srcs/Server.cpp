#include "../includes/Server.hpp"

Server::Server(std::string port, const std::string& password){
    _port = atoi(port.c_str());
    _password = password;
    _running = false;
    _listenFd = -1;

    if(_port <= 1024 || _port >= 65535)
        throw();

    if(_password.empty())
        throw();

    
}

Server::~Server(){

}

void Server::run(){
    setupSocket();
    _pfds = _listenFd;
    _running = true
    while(running){
        poll(_pfds);
        for(iterator it)
    }


}


void Server::setupSocket(){
    socket(AF_INET, SOCK_STREAM, 0);
    setsockopt()
    fcntl()
    sockaddr_in;
    bind;
    listen;

}

void Server::acceptClient(){
    accept();
}