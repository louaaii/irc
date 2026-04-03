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
    while(run){

    }
    
    
}

Server::~Server(){

}

void Server::run(){
    setupSocket();
    _pfds.push_back(pollfd{_listenFd, POLLIN, 0});
    _running = true;
    while(running){
        poll(_pfds);
        for(std::Iterator it)
    }


}


void Server::setupSocket(){
    _listenFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(_listenFd == -1)
        perror("Error opening socket");
    if(setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int) < 0))
        perror("setsockopt failed");
    int status = fcntl(_listenFd, F_SETFL, fcntl(_listenFd, F_GETFL, 0) | O_NONBLOCK);
    if(status == -1)
        perror("calling fcntl");
	ServerAdr.sin_family = AF_INET;
	ServerAdr.sin_port = htons(_port);
	ServerAdr.sin_addr.s_addr = INADDR_ANY;
    bind(_listenFd, (const sockaddr *)&ServerAdr, sizeof(ServerAdr));
    listen(_listenFd, SOMAXCONN);

}

void Server::acceptClient(){
    accept();
}
