#include "Client.hpp"

#include <ostream>
#include <iostream>

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

bool Client::isPassOk() const{
	return _passOK;
}

bool Client::hasNick() const{
	return _hasNick;
}

void Client::set_nick(std::string nick) {
	std::cout << "[CLIENT] Setting nickname: " << nick << " for fd " << _fd << std::endl;
	_nickname = nick;
	_hasNick = !_nickname.empty();
}

void Client::set_fd(int fd){
	_fd = fd;
}

void Client::setPassOk(bool flag) {
	std::cout << "[CLIENT] Password authentication " << (flag ? "passed" : "failed") << " for fd " << _fd << std::endl;
	_passOK = flag;
}

void Client::set_Username(std::string username) {
	std::cout << "[CLIENT] Setting username: " << username << " for fd " << _fd << std::endl;
	_username = username;
	_hasUser = !_username.empty();
}

void Client::set_hostname(std::string hostname) {
	std::cout << "[CLIENT] Setting hostname: " << hostname << " for fd " << _fd << std::endl;
	_hostname = hostname;
}

void Client::joinChannel(std::string channelName) {
	std::cout << "[CHANNEL] Client " << _fd << " joining channel: " << channelName << std::endl;
	_channels.insert(channelName);
}

void Client::partChannel(std::string channelName) {
	std::cout << "[CHANNEL] Client " << _fd << " leaving channel: " << channelName << std::endl;
	_channels.erase(channelName);
}

bool Client::isInChannel(std::string channelName) {
	return _channels.find(channelName) != _channels.end();
}
