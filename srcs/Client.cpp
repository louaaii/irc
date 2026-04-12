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

void Client::set_Username(std::string username) {
	_username = username;
	_hasUser = !_username.empty();
}

void Client::set_hostname(std::string hostname) {
	_hostname = hostname;
}

void Client::joinChannel(std::string channelName) {
	_channels.insert(channelName);
}

void Client::partChannel(std::string channelName) {
	_channels.erase(channelName);
}

bool Client::isInChannel(std::string channelName) {
	return _channels.find(channelName) != _channels.end();
}
