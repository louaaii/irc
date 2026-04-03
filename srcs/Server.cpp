#include "Server.hpp"

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
    _pfds.push_back(pollfd{_listenFd, POLLIN, 0});
    _running = true;
    while(running){
        poll(_pfds);
        for(iterator it)
    }


}


void Server::setupSocket(){
    _listenFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    setsockopt();
    fcntl();
	ServerAdr.sin_family = AF_INET;
	ServerAdr.sin_port = htons(_port);
	ServerAdr.sin_addr.s_addr = INADDR_ANY;
    bind(_listenFd, (const sockaddr *)&ServerAdr, sizeof(ServerAdr));
    listen(_listenFd, SOMAXCONN);

}

void Server::acceptClient(){
    accept();
}
