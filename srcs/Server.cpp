#include "../includes/Server.hpp"

#define MAX_EVENT 10

Server::Server(std::string port, const std::string& password){
    _port = atoi(port.c_str());
    _password = password;
    _running = false;
    _listenFd = -1;
	_epollfd = -1;
	memset(&ServerAdr, 0, sizeof(ServerAdr));

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
        int nfds;
        while ((nfds = epoll_wait(_epollfd, events, MAX_EVENT, -1)) == -1) {
            if (errno != EINTR)
                throw std::runtime_error("epoll_wait failed");
        }

        for(int i = 0; i < nfds; i++) {
            if(events[i].data.fd == _listenFd) {
                acceptClient();
            } else {
                if(events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                    removeClient(events[i].data.fd);
                } else if(events[i].events & EPOLLIN) {
                    handleClientRead(events[i].data.fd);
                }
            }
        }
    }
}

void Server::setupSocket(){
    _listenFd = socket(AF_INET, SOCK_STREAM, 0);
    const int OPT = 1;
    if (_listenFd == -1)
        throw std::runtime_error("Error opening socket");

    int flags = fcntl(_listenFd, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("fcntl F_GETFL failed");
    if (fcntl(_listenFd, F_SETFL, flags | O_NONBLOCK) == -1)
        throw std::runtime_error("fcntl F_SETFL failed");

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
	while (1){
		sockaddr_in ClientAdr;
		socklen_t CliAdrlen = sizeof(ClientAdr);

		int clientFd = accept(_listenFd, (sockaddr*)&ClientAdr, &CliAdrlen);
		if (clientFd == -1){
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			if (errno == EINTR)
				return;
			throw std::runtime_error("accept failed");
		}
		int flags = fcntl(clientFd, F_GETFL, 0);
		if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1){
			close(clientFd);
			throw std::runtime_error("fcntl(O_NONBLOCK) fail");
		}
		epoll_event ev;
		memset(&ev, 0, sizeof(ev));
		ev.events = EPOLLIN | EPOLLRDHUP;
		ev.data.fd = clientFd;

		if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, clientFd, &ev) == -1){
			close(clientFd);
			throw std::runtime_error("epoll_ctl add cli fail");
		}
		Clients[clientFd] = Client(clientFd);
	}
}


void Server::handleClientRead(int fd){
	char buffer[4096];
	ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytesRead == -1) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			removeClient(fd);
		}
		return;
	}
	if (bytesRead == 0) {
		removeClient(fd);
		return;
	}
	buffer[bytesRead] = '\0';
	_inBuffers[fd] += buffer;

	size_t pos = 0;
	while ((pos = _inBuffers[fd].find("\r\n")) != std::string::npos) {
		std::string command = _inBuffers[fd].substr(0, pos);
		_inBuffers[fd].erase(0, pos + 2);

		// TODO: Parse and execute command
		// Commands::execute(fd, command, this);
	}
}

void Server::removeClient(int fd) {
	epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	Clients.erase(fd);
	_inBuffers.erase(fd);
	_outBuffers.erase(fd);
}
