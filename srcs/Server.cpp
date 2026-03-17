#include "../includes/Server.hpp"

Server::Server(std::string port, const std::string& password){
    _port = atoi(port.c_str());
    _password = password;
    _running = false;

    if(_port <= 1024 || _port >= 65535)
        throw();

    
}

Server::~Server(){

}

void Server::run(){

}

