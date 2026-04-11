#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "Server.hpp"
#include "Client.hpp"

class Commands {
public:
    void parseCommand(const std::string& command, std::vector<std::string>& args);
    void execute(int fd, const std::string& command, Server* server);

    void PASS(int fd, const std::vector<std::string>& args, Server* server);
    void NICK(int fd, const std::vector<std::string>& args, Server* server);
    void USER(int fd, const std::vector<std::string>& args, Server* server);
    void PING(int fd, const std::vector<std::string>& args, Server* server);
    void QUIT(int fd, const std::vector<std::string>& args, Server* server);
    void MODE(int fd, const std::vector<std::string>& args, Server* server);
    void JOIN(int fd, const std::vector<std::string>& args, Server* server);
    void PART(int fd, const std::vector<std::string>& args, Server* server);
    void PRIVMSG(int fd, const std::vector<std::string>& args, Server* server);
    void TOPIC(int fd, const std::vector<std::string>& args, Server* server);
    void KICK(int fd, const std::vector<std::string>& args, Server* server);
    void INVITE(int fd, const std::vector<std::string>& args, Server* server);
};



#endif