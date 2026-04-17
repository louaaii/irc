#include "Commands.hpp"
#include "Channel.hpp"
#include <iostream>
#include <cctype>
#include <cstdlib>

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

void Commands::sendToChannel(const std::string& channelName, const std::string& message, Server* server, int excludeFd) {
    if (server->_channels.find(channelName) == server->_channels.end())
        return;
    for (std::map<int, Client>::iterator it = server->Clients.begin();
         it != server->Clients.end(); ++it) {
        if (it->first != excludeFd && it->second.isInChannel(channelName)) {
            sendToClient(it->first, message, server);
        }
    }
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
    if (server->_channels.find(channelName) == server->_channels.end())
        return false;
    Channel& channel = server->_channels[channelName];
    return channel.isOperator(fd);
}


void Commands::parseCommand(const std::string& command, std::vector<std::string>& args) {
    std::istringstream iss(command);
    std::string token;

    while (iss >> token) {
        if (token.empty())
            continue;
        if (args.empty())
            std::transform(token.begin(), token.end(), token.begin(), ::toupper);
        args.push_back(token);
    }
}

std::string Commands::extractMessage(const std::vector<std::string>& args, size_t startIdx) {
    if (startIdx >= args.size())
        return "";

    std::string message;
    for (size_t i = startIdx; i < args.size(); ++i) {
        if (i > startIdx) message += " ";
        std::string token = args[i];
        if (i == startIdx && !token.empty() && token[0] == ':') {
            token = token.substr(1);
        }
        message += token;
    }
    return message;
}


void Commands::execute(int fd, const std::string& command, Server* server) {
    std::vector<std::string> args;
    parseCommand(command, args);

    if (args.empty()) {
        std::cout << "[COMMAND] Empty command received" << std::endl;
        return;
    }

    const std::string& cmd = args[0];
    std::cout << "[COMMAND] Processing: " << cmd << " for client " << fd << std::endl;

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
    else
        sendError(fd, "Unknown command: " + cmd, server);
}


void Commands::PASS(int fd, const std::vector<std::string>& args, Server* server) {
    std::cout << "[PASS] Processing PASS command for fd: " << fd << std::endl;
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (client.isRegistered()) {
        std::cout << "[PASS] Client already registered" << std::endl;
        sendError(fd, "Already registered", server);
        return;
    }
    if (args.size() != 2) {
        std::cout << "[PASS] Invalid arguments" << std::endl;
        sendError(fd, "461 PASS :Not enough parameters", server);
        return;
    }
    const std::string& password = args[1];
    if (password == server->_password) {
        std::cout << "[PASS] Password accepted for client " << fd << std::endl;
        client.setPassOk(true);
        sendToClient(fd, ":server NOTICE AUTH :Password accepted", server);
    }
    else {
        std::cout << "[PASS] Password incorrect for client " << fd << std::endl;
        sendError(fd, "464 PASS :Password incorrect", server);
    }
}

void Commands::NICK(int fd, const std::vector<std::string>& args, Server* server) {
    std::cout << "[NICK] Processing NICK command for fd: " << fd << std::endl;
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (!client.isPassOk()) {
        std::cout << "[NICK] PASS command must be sent first" << std::endl;
        sendError(fd, "You must authenticate with PASS first", server);
        return;
    }
    if (args.size() != 2) {
        std::cout << "[NICK] Invalid arguments" << std::endl;
        sendToClient(fd, ":server NOTICE :Usage: NICK <nickname>", server);
        sendError(fd, "461 NICK :Not enough parameters", server);
        return;
    }
    const std::string& nickname = args[1];
    if (!isValidNickname(nickname)) {
        std::cout << "[NICK] Invalid nickname: " << nickname << std::endl;
        sendError(fd, "432 NICK " + nickname + " :Erroneous nickname", server);
        return;
    }
    for (std::map<int, Client>::iterator it = server->Clients.begin();
         it != server->Clients.end(); ++it) {
        if (it->first != fd && it->second.get_nick() == nickname) {
            std::cout << "[NICK] Nickname already in use: " << nickname << std::endl;
            sendError(fd, "433 NICK " + nickname + " :Nickname is already in use", server);
            return;
        }
    }
    std::cout << "[NICK] Setting nickname to " << nickname << " for client " << fd << std::endl;
    client.set_nick(nickname);
    sendToClient(fd, ":server NOTICE AUTH :Nickname set to " + nickname, server);
}

void Commands::USER(int fd, const std::vector<std::string>& args, Server* server) {
    std::cout << "[USER] Processing USER command for fd: " << fd << std::endl;
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (!client.isPassOk()) {
        std::cout << "[USER] PASS command must be sent first" << std::endl;
        sendError(fd, "You must authenticate with PASS first", server);
        return;
    }
    if (!client.hasNick()) {
        std::cout << "[USER] NICK command must be sent before USER" << std::endl;
        sendError(fd, "You must set a nickname with NICK first", server);
        return;
    }
    if (args.size() < 5) {
        std::cout << "[USER] Invalid arguments - args.size() = " << args.size() << std::endl;
        for (size_t i = 0; i < args.size(); ++i) {
            std::cout << "[USER] args[" << i << "] = " << args[i] << std::endl;
        }
        sendToClient(fd, ":server NOTICE :Usage: USER <username> <hostname> <servername> <realname>", server);
        sendError(fd, "461 USER :Not enough parameters\nUsage : 'USER <username> <hostname> <servername> <realname>", server);
        return;
    }
    const std::string& username = args[1];
    const std::string& hostname = args[3];
    if (username.empty() || username.length() > 16) {
        std::cout << "[USER] Invalid username" << std::endl;
        sendError(fd, "Invalid username", server);
        return;
    }
    std::cout << "[USER] Setting username to " << username << " for client " << fd << std::endl;
    client.set_Username(username);
    client.set_hostname(hostname);
    sendToClient(fd, ":server NOTICE AUTH :User information set", server);
    if (client.isRegistered()) {
        std::cout << "[USER] Client " << fd << " is now registered!" << std::endl;
        std::string nick = client.get_nick();
        sendToClient(fd, ":server 001 " + nick + " :Welcome to IRC", server);
        sendToClient(fd, ":server 002 " + nick + " :Your host is john tekken", server);
        sendToClient(fd, ":server 003 " + nick + " :This server was created today", server);
    }
}

void Commands::PING(int fd, const std::vector<std::string>& args, Server* server) {
    std::cout << "[PING] Processing PING command for fd: " << fd << std::endl;
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    if (args.size() < 2) {
        std::cout << "[PING] Invalid arguments" << std::endl;
        sendError(fd, "461 PING :Not enough parameters", server);
        return;
    }
    const std::string& pingMsg = args[1];
    std::string response = ":server PONG " + pingMsg;
    std::cout << "[PING] Responding with PONG to client " << fd << std::endl;
    sendToClient(fd, response, server);
}

void Commands::QUIT(int fd, const std::vector<std::string>& args, Server* server) {
    std::cout << "[QUIT] Processing QUIT command for fd: " << fd << std::endl;
    if (server->Clients.find(fd) == server->Clients.end())
		return;
    (void)args;
    std::cout << "[QUIT] Disconnecting client " << fd << std::endl;
    server->removeClient(fd);
}

void Commands::JOIN(int fd, const std::vector<std::string>& args, Server* server) {
    std::cout << "[JOIN] Processing JOIN command for fd: " << fd << std::endl;
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (!client.isRegistered()) {
        std::cout << "[JOIN] Client not registered" << std::endl;
        sendError(fd, "451 JOIN :You have not registered", server);
        return;
    }
    if (args.size() < 2) {
        std::cout << "[JOIN] Invalid arguments" << std::endl;
        sendToClient(fd, ":server NOTICE :Usage: JOIN <channel> [key]", server);
        sendError(fd, "461 JOIN :Not enough parameters", server);
        return;
    }
    const std::string& channelName = args[1];
    std::string channelKey = "";
    if (args.size() > 2) {
        channelKey = args[2];
    }
    if (!isValidChannelName(channelName)) {
        std::cout << "[JOIN] Invalid channel name: " << channelName << std::endl;
        sendError(fd, "403 " + channelName + " :No such channel", server);
        return;
    }
    std::cout << "[JOIN] Client " << fd << " (" << client.get_nick() << ") joining channel " << channelName << std::endl;

    if (server->_channels.find(channelName) == server->_channels.end()) {
        std::cout << "[JOIN] Creating new channel: " << channelName << std::endl;
        server->_channels[channelName] = Channel(channelName);
    }

    Channel& channel = server->_channels[channelName];

    std::string requiredKey = channel.getKey();
    if (!requiredKey.empty()) {
        if (args.size() < 3) {
            std::cout << "[JOIN] Channel requires a key" << std::endl;
            sendError(fd, "475 " + channelName + " :Cannot join channel (+k)", server);
            return;
        }
        if (channelKey != requiredKey) {
            std::cout << "[JOIN] Invalid channel key" << std::endl;
            sendError(fd, "475 " + channelName + " :Cannot join channel (+k)", server);
            return;
        }
    }

    if (channel.isInviteOnly()) {
        if (!channel.isUserInvited(fd)) {
            std::cout << "[JOIN] Channel is invite-only and user is not invited" << std::endl;
            sendError(fd, "473 " + channelName + " :Cannot join channel (+i)", server);
            return;
        }
        channel.removeInvitedUser(fd);
    }

    bool isFirstMember = channel.get_member_count() == 0;

    client.joinChannel(channelName);
    channel.addMember(&client);

    if (isFirstMember) {
        std::cout << "[JOIN] " << client.get_nick() << " is first member, making operator" << std::endl;
        channel.addOperator(fd);
    }

    std::string joinMsg = ":" + client.get_nick() + " JOIN " + channelName;
    sendToClient(fd, joinMsg, server);
    sendToChannel(channelName, joinMsg, server, fd);
    std::cout << "[JOIN] Broadcasted JOIN message to all members in " << channelName << std::endl;
}

void Commands::PART(int fd, const std::vector<std::string>& args, Server* server) {
    std::cout << "[PART] Processing PART command for fd: " << fd << std::endl;
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (args.size() < 2) {
        std::cout << "[PART] Invalid arguments" << std::endl;
        sendToClient(fd, ":server NOTICE :Usage: PART <channel> [message]", server);
        sendError(fd, "461 PART :Not enough parameters", server);
        return;
    }
    const std::string& channelName = args[1];
    std::string partMsg = "Leaving";
    if (args.size() > 2) {
        partMsg = args[2];
    }
    if (!client.isInChannel(channelName)) {
        std::cout << "[PART] Client not in channel: " << channelName << std::endl;
        sendError(fd, "442 " + channelName + " :You're not on that channel", server);
        return;
    }
    std::cout << "[PART] Client " << fd << " (" << client.get_nick() << ") leaving channel " << channelName << std::endl;
    client.partChannel(channelName);

    // Remove client from the channel's member list
    if (server->_channels.find(channelName) != server->_channels.end()) {
        server->_channels[channelName].removeMember(fd);
    }

    std::string response = ":" + client.get_nick() + " PART " + channelName + " :" + partMsg;
    sendToClient(fd, response, server);
    // Broadcast PART message to all other channel members
    sendToChannel(channelName, response, server, fd);
}

void Commands::PRIVMSG(int fd, const std::vector<std::string>& args, Server* server) {
    std::cout << "[PRIVMSG] Processing PRIVMSG command for fd: " << fd << std::endl;
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (args.size() < 3) {
        std::cout << "[PRIVMSG] Invalid arguments" << std::endl;
        sendToClient(fd, ":server NOTICE :Usage: PRIVMSG <target> <message>", server);
        sendError(fd, "461 PRIVMSG :Not enough parameters", server);
        return;
    }
    const std::string& target = args[1];
    std::string message = extractMessage(args, 2);

    if (target[0] == '#') {
        std::cout << "[PRIVMSG] Sending message to channel " << target << std::endl;
        if (server->_channels.find(target) == server->_channels.end()) {
            std::cout << "[PRIVMSG] Channel does not exist: " << target << std::endl;
            sendError(fd, "404 " + target + " :No such channel", server);
            return;
        }
        if (!client.isInChannel(target)) {
            std::cout << "[PRIVMSG] Client not in channel: " << target << std::endl;
            sendError(fd, "404 " + target + " :Cannot send to channel", server);
            return;
        }
        std::cout << "[PRIVMSG] Broadcasting to all members in " << target << std::endl;
        std::string channelMsg = ":" + client.get_nick() + " PRIVMSG " + target + " :" + message;
        sendToChannel(target, channelMsg, server, -1);
    }
    else {
        std::cout << "[PRIVMSG] Sending private message to " << target << std::endl;
        bool found = false;
        for (std::map<int, Client>::iterator it = server->Clients.begin();
             it != server->Clients.end(); ++it) {
            if (it->second.get_nick() == target) {
                std::string userMsg = ":" + client.get_nick() + " PRIVMSG " + target + " :" + message;
                sendToClient(it->first, userMsg, server);
                std::cout << "[PRIVMSG] Message delivered to " << target << std::endl;
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "[PRIVMSG] Target not found: " << target << std::endl;
            sendError(fd, "401 " + target + " :No such nick/channel", server);
        }
    }
}

void Commands::TOPIC(int fd, const std::vector<std::string>& args, Server* server) {
    std::cout << "[TOPIC] Processing TOPIC command for fd: " << fd << std::endl;
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (args.size() < 2) {
        std::cout << "[TOPIC] Invalid arguments" << std::endl;
        sendToClient(fd, ":server NOTICE :Usage: TOPIC <channel> [topic]", server);
        sendError(fd, "461 TOPIC :Not enough parameters", server);
        return;
    }
    const std::string& channelName = args[1];
    if (!client.isInChannel(channelName)) {
        std::cout << "[TOPIC] Client not in channel: " << channelName << std::endl;
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
    std::string newTopic = extractMessage(args, 2);
    std::string topicMsg = ":" + client.get_nick() + " TOPIC " + channelName + " :" + newTopic;
    std::cout << "[TOPIC] Broadcasting to all members in " << channelName << std::endl;
    sendToChannel(channelName, topicMsg, server, -1);
}

void Commands::KICK(int fd, const std::vector<std::string>& args, Server* server) {
    std::cout << "[KICK] Processing KICK command for fd: " << fd << std::endl;
    if (server->Clients.find(fd) == server->Clients.end())
        return;

    Client& client = server->Clients[fd];
    if (args.size() < 3) {
        std::cout << "[KICK] Invalid arguments" << std::endl;
        sendToClient(fd, ":server NOTICE :Usage: KICK <channel> <nick> [reason]", server);
        sendError(fd, "461 KICK :Not enough parameters", server);
        return;
    }
    const std::string& channelName = args[1];
    const std::string& targetNick = args[2];
    std::string reason = "Kicked";
    if (args.size() > 3) {
        reason = extractMessage(args, 3);
    }
    if (!client.isInChannel(channelName)) {
        std::cout << "[KICK] Client not in channel: " << channelName << std::endl;
        sendError(fd, "442 " + channelName + " :You're not on that channel", server);
        return;
    }
    if (!isChannelOperator(fd, channelName, server)) {
        std::cout << "[KICK] Client is not channel operator" << std::endl;
        sendError(fd, "482 " + channelName + " :You're not channel operator", server);
        return;
    }
    std::cout << "[KICK] Kicking " << targetNick << " from " << channelName << std::endl;
    for (std::map<int, Client>::iterator it = server->Clients.begin();
         it != server->Clients.end(); ++it) {
        if (it->second.get_nick() == targetNick && it->second.isInChannel(channelName)) {
            it->second.partChannel(channelName);
            std::string kickMsg = ":" + client.get_nick() + " KICK " + channelName +
                                 " " + targetNick + " :" + reason;
            std::cout << "[KICK] Broadcasting kick to all channel members" << std::endl;
            sendToChannel(channelName, kickMsg, server, -1);
            sendToClient(it->first, kickMsg, server);
            return;
        }
    }
    std::cout << "[KICK] Target not found: " << targetNick << std::endl;
    sendError(fd, "401 " + targetNick + " :No such nick/channel", server);
}

void Commands::INVITE(int fd, const std::vector<std::string>& args, Server* server) {
    std::cout << "[INVITE] Processing INVITE command for fd: " << fd << std::endl;
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (args.size() < 3) {
        std::cout << "[INVITE] Invalid arguments" << std::endl;
        sendToClient(fd, ":server NOTICE :Usage: INVITE <nick> <channel>", server);
        sendError(fd, "461 INVITE :Not enough parameters", server);
        return;
    }
    const std::string& targetNick = args[1];
    const std::string& channelName = args[2];
    if (!client.isInChannel(channelName)) {
        std::cout << "[INVITE] Client not in channel: " << channelName << std::endl;
        sendError(fd, "442 " + channelName + " :You're not on that channel", server);
        return;
    }
    if (!isChannelOperator(fd, channelName, server)) {
        std::cout << "[INVITE] Client is not channel operator" << std::endl;
        sendError(fd, "482 " + channelName + " :You're not channel operator", server);
        return;
    }
    int targetFd = -1;
    std::cout << "[INVITE] Looking for user: " << targetNick << std::endl;
    for (std::map<int, Client>::iterator it = server->Clients.begin();
         it != server->Clients.end(); ++it) {
        if (it->second.get_nick() == targetNick) {
            targetFd = it->first;
            break;
        }
    }
    if (targetFd == -1) {
        std::cout << "[INVITE] Target user not found: " << targetNick << std::endl;
        sendError(fd, "401 " + targetNick + " :No such nick/channel", server);
        return;
    }
    std::cout << "[INVITE] Sending invite to " << targetNick << " for channel " << channelName << std::endl;

    if (server->_channels.find(channelName) != server->_channels.end()) {
        server->_channels[channelName].addInvitedUser(targetFd);
    }

    std::string inviteMsg = ":" + client.get_nick() + " INVITE " + targetNick + " " + channelName;
    sendToClient(targetFd, inviteMsg, server);
    sendToClient(targetFd, ":server NOTICE " + targetNick + " :You have been invited to " + channelName + " by " + client.get_nick(), server);
    sendToClient(fd, ":server NOTICE : Invite sent", server);
}


void Commands::MODE(int fd, const std::vector<std::string>& args, Server* server) {
    std::cout << "[MODE] Processing MODE command for fd: " << fd << std::endl;
    if (server->Clients.find(fd) == server->Clients.end())
        return;
    Client& client = server->Clients[fd];
    if (args.size() < 3) {
        std::cout << "[MODE] Invalid arguments" << std::endl;
        sendToClient(fd, ":server NOTICE " + client.get_nick() + " :Usage: MODE <channel> <modes> [parameters]", server);
        sendToClient(fd, ":server NOTICE " + client.get_nick() + " :Example: MODE #channel +i", server);
        sendToClient(fd, ":server NOTICE " + client.get_nick() + " :Example: MODE #channel +k password", server);
        sendError(fd, "461 MODE :Not enough parameters", server);
        return;
    }
    const std::string& target = args[1];
    const std::string& modes = args[2];
    std::cout << "[MODE] Setting modes " << modes << " for channel " << target << std::endl;
    if (target[0] != '#') {
        std::cout << "[MODE] Invalid channel name: " << target << std::endl;
        sendError(fd, "403 " + target + " :No such channel", server);
        return;
    }
    if (!client.isInChannel(target)) {
        std::cout << "[MODE] Client not in channel: " << target << std::endl;
        sendError(fd, "442 " + target + " :You're not on that channel", server);
        return;
    }
    if (!isChannelOperator(fd, target, server)) {
        std::cout << "[MODE] Client is not channel operator" << std::endl;
        sendError(fd, "482 " + target + " :You're not channel operator", server);
        return;
    }
    if (server->_channels.find(target) == server->_channels.end()) {
        std::cout << "[MODE] Creating new channel: " << target << std::endl;
        server->_channels[target] = Channel(target);
    }
    Channel& channel = server->_channels[target];
    bool add = true;
    size_t paramIdx = 3;
    bool invalidMode = false;
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
            std::cout << "[MODE] Setting invite-only to " << add << std::endl;
            channel.setInviteOnly(add);
        }
        else if (modeChar == 't') {
            std::cout << "[MODE] Setting topic-restricted to " << add << std::endl;
            channel.setTopicRestricted(add);
        }
        else if (modeChar == 'k') {
            if (add) {
                if (paramIdx < args.size()) {
                    std::cout << "[MODE] Setting key: " << args[paramIdx] << std::endl;
                    channel.setKey(args[paramIdx]);
                    paramIdx++;
                }
                else {
                    std::cout << "[MODE] Mode +k requires a key parameter" << std::endl;
                    sendError(fd, "461 MODE :Not enough parameters for mode +k", server);
                    return;
                }
            }
            else {
                std::cout << "[MODE] Removing key" << std::endl;
                channel.removeKey();
            }
        }
        else if (modeChar == 'o') {
            if (paramIdx < args.size()) {
                std::string opNick = args[paramIdx];
                paramIdx++;
                for (std::map<int, Client>::iterator it = server->Clients.begin();
                     it != server->Clients.end(); ++it) {
                    if (it->second.get_nick() == opNick && it->second.isInChannel(target)) {
                        if (add) {
                            std::cout << "[MODE] Making " << opNick << " a channel operator" << std::endl;
                            channel.addOperator(it->first);
                        }
                        else {
                            std::cout << "[MODE] Removing " << opNick << " as channel operator" << std::endl;
                            channel.removeOperator(it->first);
                        }
                        break;
                    }
                }
            }
            else {
                std::cout << "[MODE] Mode +o requires a nick parameter" << std::endl;
                sendError(fd, "461 MODE :Not enough parameters for mode +o", server);
                return;
            }
        }
        else if (modeChar == 'l') {
            if (add) {
                if (paramIdx < args.size()) {
                    std::string limitStr = args[paramIdx];
                    bool validLimit = true;
                    for (size_t j = 0; j < limitStr.length(); ++j) {
                        if (!std::isdigit(limitStr[j])) {
                            validLimit = false;
                            break;
                        }
                    }
                    if (validLimit) {
                        int limit = atoi(limitStr.c_str());
                        if (limit > 0) {
                            std::cout << "[MODE] Setting user limit to " << limit << std::endl;
                            channel.setUserLimit(limit);
                        }
                        paramIdx++;
                    }
                }
                else {
                    std::cout << "[MODE] Mode +l requires a limit parameter" << std::endl;
                    sendError(fd, "461 MODE :Not enough parameters for mode +l", server);
                    return;
                }
            }
            else if (!add) {
                std::cout << "[MODE] Removing user limit" << std::endl;
                channel.removeUserLimit();
            }
        }
        else {
            std::cout << "[MODE] Invalid mode: " << modeChar << std::endl;
            invalidMode = true;
        }
    }

    if (invalidMode) {
        sendToClient(fd, ":server NOTICE " + client.get_nick() + " :Available channel modes:", server);
        sendToClient(fd, ":server NOTICE " + client.get_nick() + " :  i - Invite-only channel", server);
        sendToClient(fd, ":server NOTICE " + client.get_nick() + " :  t - Topic restricted (ops only)", server);
        sendToClient(fd, ":server NOTICE " + client.get_nick() + " :  k - Channel key (password)", server);
        sendToClient(fd, ":server NOTICE " + client.get_nick() + " :  o - Channel operator", server);
        sendToClient(fd, ":server NOTICE " + client.get_nick() + " :  l - User limit", server);
        invalidMode = false;
    }
    std::cout << "[MODE] Mode change complete for " << target << std::endl;
    std::string modeMsg = ":" + client.get_nick() + " MODE " + target + " " + modes;
    if (paramIdx < args.size()) {
        for (size_t i = 3; i < args.size(); ++i) {
            modeMsg += " " + args[i];
        }
    }
    sendToClient(fd, ":server NOTICE " + client.get_nick() + " :Mode changed: " + modes, server);
    sendToChannel(target, modeMsg, server, -1);
}

