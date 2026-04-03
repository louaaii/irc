#include "Client.hpp"

Client::Client(int fd) : _fd(fd), _passOK(false), _hasNick(false), _registered(false){}

Client::~Client(){}

int Client::get_fd() const{
	return (_fd);
}

bool Client::isRegistered() const{
	return (_registered);
}

