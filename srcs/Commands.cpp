#include "Commands.hpp"
#include <iostream>
#include <cctype>

void Commands::sendToClient(int fd, const std::string& message, Server* server) {
    if (server->Clients.find(fd) != server->Clients.end()) {
        server->_outBuffers[fd] += message;
        if (!message.empty() && message[message.length() - 1] != '\n') {
            server->_outBuffers[fd] += "\r\n";
        }
    }
}

void Commands::sendError(int fd, const std::string& errorMsg, Server* server) {
    std::string error = ":server ERROR " + errorMsg + "\r\n";
    sendToClient(fd, error, server);
}

void Commands::sendToChannel(const std::string& channelName, const std::string& message,
                            Server* server, int excludeFd) {
    (void)channelName;
    (void)message;
    (void)server;
    (void)excludeFd;
    // Find channel and send to all members except excludeFd
    // This requires Channel implementation to be complete
}

bool Commands::isValidNickname(const std::string& nick) {
    if (nick.empty() || nick.length() > 9)
        return false;
    for (size_t i = 0; i < nick.length(); ++i) {
        char c = nick[i];
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '[' && c != ']')
            return false;
    }
    return true;
}

bool Commands::isValidChannelName(const std::string& name) {
    if (name.empty() || name[0] != '#')
        return false;
    if (name.length() > 50)
        return false;
    return true;
}

bool Commands::isChannelOperator(int fd, const std::string& channelName, Server* server) {
    (void)fd;
    (void)channelName;
    (void)server;
    // Check if client is operator of channel
    // This requires Channel implementation to be complete
    return true;  // Placeholder
}


void Commands::parseCommand(const std::string& command, std::vector<std::string>& args) {
    std::istringstream iss(command);
    std::string token;

    while (iss >> token) {
        if (token.empty())
            continue;
        if (args.empty()) {
            std::transform(token.begin(), token.end(), token.begin(), ::toupper);
        }
        args.push_back(token);
    }
}


void Commands::execute(int fd, const std::string& command, Server* server) {
    std::vector<std::string> args;
    parseCommand(command, args);

    if (args.empty())
        return;

    const std::string& cmd = args[0];

    if (cmd == "PASS")
        PASS(fd, args, server);
    else if (cmd == "NICK")
        NICK(fd, args, server);
    else if (cmd == "USER")
        USER(fd, args, server);
    else if (cmd == "PING")
        PING(fd, args, server);
    else if (cmd == "QUIT")
        QUIT(fd, args, server);
    else if (cmd == "MODE")
        MODE(fd, args, server);
    else if (cmd == "JOIN")
        JOIN(fd, args, server);
    else if (cmd == "PART")
        PART(fd, args, server);
    else if (cmd == "PRIVMSG")
        PRIVMSG(fd, args, server);
    else if (cmd == "TOPIC")
        TOPIC(fd, args, server);
    else if (cmd == "KICK")
        KICK(fd, args, server);
    else if (cmd == "INVITE")
        INVITE(fd, args, server);
    else {
        sendError(fd, "Unknown command: " + cmd, server);
    }
}


void Commands::PASS(int fd, const std::vector<std::string>& args, Server* server) {
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (client.isRegistered()) {
        sendError(fd, "Already registered", server);
        return;
    }
    if (args.size() != 2) {
        sendError(fd, "461 PASS :Not enough parameters", server);
        return;
    }
    const std::string& password = args[1];
    if (password == server->_password) {
        client.setPassOk(true);
        sendToClient(fd, ":server NOTICE AUTH :Password accepted", server);
    }
    else
        sendError(fd, "464 PASS :Password incorrect", server);
}

void Commands::NICK(int fd, const std::vector<std::string>& args, Server* server) {
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (args.size() != 2) {
        sendError(fd, "461 NICK :Not enough parameters", server);
        return;
    }
    const std::string& nickname = args[1];
    if (!isValidNickname(nickname)) {
        sendError(fd, "432 NICK " + nickname + " :Erroneous nickname", server);
        return;
    }
    for (std::map<int, Client>::iterator it = server->Clients.begin();
         it != server->Clients.end(); ++it) {
        if (it->first != fd && it->second.get_nick() == nickname) {
            sendError(fd, "433 NICK " + nickname + " :Nickname is already in use", server);
            return;
        }
    }
    client.set_nick(nickname);
    sendToClient(fd, ":server NOTICE AUTH :Nickname set to " + nickname, server);
}

void Commands::USER(int fd, const std::vector<std::string>& args, Server* server) {
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (args.size() < 5) {
        sendError(fd, "461 USER :Not enough parameters", server);
        return;
    }
    const std::string& username = args[1];
    const std::string& hostname = args[3];
    if (username.empty() || username.length() > 16) {
        sendError(fd, "Invalid username", server);
        return;
    }
    client.set_Username(username);
    client.set_hostname(hostname);
    sendToClient(fd, ":server NOTICE AUTH :User information set", server);

    if (client.isRegistered()) {
        std::string nick = client.get_nick();
        sendToClient(fd, ":server 001 " + nick + " :Welcome to IRC", server);
        sendToClient(fd, ":server 002 " + nick + " :Your host is john tekken", server);
        sendToClient(fd, ":server 003 " + nick + " :This server was created today", server);
    }
}

void Commands::PING(int fd, const std::vector<std::string>& args, Server* server) {
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    if (args.size() < 2) {
        sendError(fd, "461 PING :Not enough parameters", server);
        return;
    }
    const std::string& pingMsg = args[1];
    std::string response = ":server PONG " + pingMsg;
    sendToClient(fd, response, server);
}

void Commands::QUIT(int fd, const std::vector<std::string>& args, Server* server) {
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    (void)args;
    sendToClient(fd, ":server NOTICE AUTH :Goodbye", server);
    server->removeClient(fd);
}

void Commands::JOIN(int fd, const std::vector<std::string>& args, Server* server) {
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (!client.isRegistered()) {
        sendError(fd, "451 JOIN :You have not registered", server);
        return;
    }
    if (args.size() < 2) {
        sendError(fd, "461 JOIN :Not enough parameters", server);
        return;
    }
    const std::string& channelName = args[1];
    std::string channelKey = "";
    if (args.size() > 2) {
        channelKey = args[2];
    }
    if (!isValidChannelName(channelName)) {
        sendError(fd, "403 " + channelName + " :No such channel", server);
        return;
    }
    client.joinChannel(channelName);
    std::string joinMsg = ":" + client.get_nick() + " JOIN " + channelName;
    sendToClient(fd, joinMsg, server);
}

void Commands::PART(int fd, const std::vector<std::string>& args, Server* server) {
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (args.size() < 2) {
        sendError(fd, "461 PART :Not enough parameters", server);
        return;
    }
    const std::string& channelName = args[1];
    std::string partMsg = "Leaving";
    if (args.size() > 2) {
        partMsg = args[2];
    }
    if (!client.isInChannel(channelName)) {
        sendError(fd, "442 " + channelName + " :You're not on that channel", server);
        return;
    }
    client.partChannel(channelName);
    std::string response = ":" + client.get_nick() + " PART " + channelName + " :" + partMsg;
    sendToClient(fd, response, server);
}

void Commands::PRIVMSG(int fd, const std::vector<std::string>& args, Server* server) {
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (args.size() < 3) {
        sendError(fd, "461 PRIVMSG :Not enough parameters", server);
        return;
    }
    const std::string& target = args[1];
    std::string message;
    for (size_t i = 2; i < args.size(); ++i) {
        if (i > 2) message += " ";
        message += args[i];
    }
    if (target[0] == '#') {
        if (!client.isInChannel(target)) {
            sendError(fd, "404 " + target + " :Cannot send to channel", server);
            return;
        }
        std::string channelMsg = ":" + client.get_nick() + " PRIVMSG " + target + " :" + message;
        sendToChannel(target, channelMsg, server, fd);
    }
    else {
        bool found = false;
        for (std::map<int, Client>::iterator it = server->Clients.begin();
             it != server->Clients.end(); ++it) {
            if (it->second.get_nick() == target) {
                std::string userMsg = ":" + client.get_nick() + " PRIVMSG " + target + " :" + message;
                sendToClient(it->first, userMsg, server);
                found = true;
                break;
            }
        }
        if (!found) {
            sendError(fd, "401 " + target + " :No such nick/channel", server);
        }
    }
}

void Commands::TOPIC(int fd, const std::vector<std::string>& args, Server* server) {
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (args.size() < 2) {
        sendError(fd, "461 TOPIC :Not enough parameters", server);
        return;
    }
    const std::string& channelName = args[1];
    if (!client.isInChannel(channelName)) {
        sendError(fd, "442 " + channelName + " :You're not on that channel", server);
        return;
    }
    if (!isChannelOperator(fd, channelName, server)) {
        sendError(fd, "482 " + channelName + " :You're not channel operator", server);
        return;
    }
    if (args.size() == 2) {
        sendToClient(fd, ":server TOPIC " + channelName + " :", server);
        return;
    }
    std::string newTopic;
    for (size_t i = 2; i < args.size(); ++i) {
        if (i > 2) newTopic += " ";
        newTopic += args[i];
    }
    std::string topicMsg = ":" + client.get_nick() + " TOPIC " + channelName + " :" + newTopic;
    sendToClient(fd, topicMsg, server);
}

void Commands::KICK(int fd, const std::vector<std::string>& args, Server* server) {
    if (server->Clients.find(fd) == server->Clients.end())
        return;

    Client& client = server->Clients[fd];
    if (args.size() < 3) {
        sendError(fd, "461 KICK :Not enough parameters", server);
        return;
    }
    const std::string& channelName = args[1];
    const std::string& targetNick = args[2];
    std::string reason = "Kicked";
    if (args.size() > 3) {
        reason = args[3];
    }
    if (!client.isInChannel(channelName)) {
        sendError(fd, "442 " + channelName + " :You're not on that channel", server);
        return;
    }
    if (!isChannelOperator(fd, channelName, server)) {
        sendError(fd, "482 " + channelName + " :You're not channel operator", server);
        return;
    }
    for (std::map<int, Client>::iterator it = server->Clients.begin();
         it != server->Clients.end(); ++it) {
        if (it->second.get_nick() == targetNick && it->second.isInChannel(channelName)) {
            it->second.partChannel(channelName);
            std::string kickMsg = ":" + client.get_nick() + " KICK " + channelName +
                                 " " + targetNick + " :" + reason;
            sendToClient(it->first, kickMsg, server);
            sendToClient(fd, kickMsg, server);
            return;
        }
    }
    sendError(fd, "401 " + targetNick + " :No such nick/channel", server);
}

void Commands::INVITE(int fd, const std::vector<std::string>& args, Server* server) {
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (args.size() < 3) {
        sendError(fd, "461 INVITE :Not enough parameters", server);
        return;
    }
    const std::string& targetNick = args[1];
    const std::string& channelName = args[2];
    if (!client.isInChannel(channelName)) {
        sendError(fd, "442 " + channelName + " :You're not on that channel", server);
        return;
    }
    if (!isChannelOperator(fd, channelName, server)) {
        sendError(fd, "482 " + channelName + " :You're not channel operator", server);
        return;
    }
    int targetFd = -1;
    for (std::map<int, Client>::iterator it = server->Clients.begin();
         it != server->Clients.end(); ++it) {
        if (it->second.get_nick() == targetNick) {
            targetFd = it->first;
            break;
        }
    }
    if (targetFd == -1) {
        sendError(fd, "401 " + targetNick + " :No such nick/channel", server);
        return;
    }
    std::string inviteMsg = ":" + client.get_nick() + " INVITE " + targetNick + " " + channelName;
    sendToClient(targetFd, inviteMsg, server);
    sendToClient(fd, ":server NOTICE : Invite sent", server);
}

void Commands::MODE(int fd, const std::vector<std::string>& args, Server* server) {
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (args.size() < 3) {
        sendError(fd, "461 MODE :Not enough parameters", server);
        return;
    }
    const std::string& target = args[1];
    const std::string& modes = args[2];
    if (target[0] != '#') {
        sendError(fd, "403 " + target + " :No such channel", server);
        return;
    }
    if (!client.isInChannel(target)) {
        sendError(fd, "442 " + target + " :You're not on that channel", server);
        return;
    }
    if (!isChannelOperator(fd, target, server)) {
        sendError(fd, "482 " + target + " :You're not channel operator", server);
        return;
    }
    bool add = true;
    size_t paramIdx = 3;

    for (size_t i = 0; i < modes.length(); ++i) {
        char modeChar = modes[i];

        if (modeChar == '+') {
            add = true;
            continue;
        }
        else if (modeChar == '-') {
            add = false;
            continue;
        }
        if (modeChar == 'i') {
            // Invite-only mode
        } else if (modeChar == 't') {
            // Topic restricted mode
        } else if (modeChar == 'k') {
            // Channel key (password)
            if (add && paramIdx < args.size()) {
                paramIdx++;
            }
        } else if (modeChar == 'o') {
            // Operator privilege
            if (paramIdx < args.size()) {
                std::string opNick = args[paramIdx];
                paramIdx++;

                for (std::map<int, Client>::iterator it = server->Clients.begin();
                     it != server->Clients.end(); ++it) {
                    if (it->second.get_nick() == opNick && it->second.isInChannel(target)) {
                        // Change operator status
                        break;
                    }
                }
            }
        } else if (modeChar == 'l') {
            // User limit
            if (add && paramIdx < args.size()) {
                paramIdx++;
            }
        }
    }
    std::string modeMsg = ":" + client.get_nick() + " MODE " + target + " " + modes;
    if (paramIdx < args.size()) {
        for (size_t i = paramIdx; i < args.size(); ++i) {
            modeMsg += " " + args[i];
        }
    }
    sendToClient(fd, modeMsg, server);
}

