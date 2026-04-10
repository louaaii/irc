#include "Client.hpp"

Client::Client() : _fd(-1), _passOK(false), _hasNick(false), _hasUser(false){}

Client::Client(int fd) : _fd(fd), _passOK(false), _hasNick(false), _hasUser(false){}

Client::~Client(){}

int Client::get_fd() const{
	return (_fd);
}

std::string Client::get_nick() const{
	return (_nickname);
}

bool Client::isRegistered() const{
	return (_passOK && _hasNick && _hasUser);
}

void Client::set_nick(std::string nick) {
	_nickname = nick;
	_hasNick = !_nickname.empty();
}

void Client::set_fd(int fd){
	_fd = fd;
}

void Client::setPassOk(bool flag) {
	_passOK = flag;
}
