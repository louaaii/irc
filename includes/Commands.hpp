#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "Server.hpp"
#include "Client.hpp"
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

class Commands {
public:
    static void parseCommand(const std::string& command, std::vector<std::string>& args);
    static void execute(int fd, const std::string& command, Server* server);

    static void PASS(int fd, const std::vector<std::string>& args, Server* server);
    static void NICK(int fd, const std::vector<std::string>& args, Server* server);
    static void USER(int fd, const std::vector<std::string>& args, Server* server);
    static void PING(int fd, const std::vector<std::string>& args, Server* server);
    static void QUIT(int fd, const std::vector<std::string>& args, Server* server);
    static void MODE(int fd, const std::vector<std::string>& args, Server* server);
    static void JOIN(int fd, const std::vector<std::string>& args, Server* server);
    static void PART(int fd, const std::vector<std::string>& args, Server* server);
    static void PRIVMSG(int fd, const std::vector<std::string>& args, Server* server);
    static void TOPIC(int fd, const std::vector<std::string>& args, Server* server);
    static void KICK(int fd, const std::vector<std::string>& args, Server* server);
    static void INVITE(int fd, const std::vector<std::string>& args, Server* server);

private:
    static void sendToClient(int fd, const std::string& message, Server* server);
    static void sendToChannel(const std::string& channelName, const std::string& message, 
                             Server* server, int excludeFd = -1);
    static void sendError(int fd, const std::string& errorMsg, Server* server);
    static bool isValidNickname(const std::string& nick);
    static bool isValidChannelName(const std::string& name);
    static bool isChannelOperator(int fd, const std::string& channelName, Server* server);
};

#endif