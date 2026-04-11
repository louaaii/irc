#include "Commands.hpp"

void Commands::parseCommand(const std::string& command, std::vector<std::string>& args) {
    std::istringstream iss(command);
    std::string token;
    while (iss >> token) {
        if(!token.empty())
            std::runtime_error("Empty command");
        args.push_back(token);
    }
}


void Commands::execute(int fd, const std::string& command, Server* server) {
    std::vector<std::string> args;
    parseCommand(command, args);

    if (args.empty())
        return;

    const std::string& cmd = args[0];
    if (cmd == "PASS") {
        if (args.size() != 2) {
            // Handle error: PASS command requires exactly one argument
            return;
        }
        const std::string& password = args[1];
        Client& client = server->Clients[fd];
        if (password == server->_password) {
            client.setPassOk(true);
            // Optionally send a success message to the client
        } else {
            // Optionally send an error message to the client
        }
    } else if (cmd == "NICK") {
        if (args.size() != 2) {
            // Handle error: NICK command requires exactly one argument
            return;
        }
        const std::string& nickname = args[1];
        Client& client = server->Clients[fd];
        client.set_nick(nickname);
        // Optionally send a success message to the client
    } else if (cmd == "USER") {
        if (args.size() != 5) {
            // Handle error: USER command requires exactly four arguments
            return;
        }
        // Handle USER command logic here
    } else if (cmd == "PING") {
        // Handle PING command logic here
    } else if (cmd == "QUIT") {
        // Handle QUIT command logic here
    } else if (cmd == "MODE") {
        // Handle MODE command logic here
    } else if (cmd == "JOIN") {
        // Handle JOIN command logic here
    } else if (cmd == "PART") {
        // Handle PART command logic here
    } else if (cmd == "PRIVMSG") {
        // Handle PRIVMSG command logic here
    } else if (cmd == "TOPIC") {
        // Handle TOPIC command logic here
    } else if (cmd == "KICK") {
        // Handle KICK command logic here
    } else if (cmd == "INVITE") {
        // Handle INVITE command logic here
    } else {
        // Handle unknown command
    }
}   


void Commands::PASS(int fd, const std::vector<std::string>& args, Server* server) {
    if (args.size() != 2) {
        // Handle error: PASS command requires exactly one argument
        return;
    }
    const std::string& password = args[1];
    Client& client = server->Clients[fd];
    if (password == server->_password) {
        client.setPassOk(true);
        // Optionally send a success message to the client
    } else {
        // Optionally send an error message to the client
    }
}

void Commands::NICK(int fd, const std::vector<std::string>& args, Server* server) {
    if (args.size() != 2) {
        // Handle error: NICK command requires exactly one argument
        return;
    }
    const std::string& nickname = args[1];
    Client& client = server->Clients[fd];
    client.set_nick(nickname);
    // Optionally send a success message to the client
}

void Commands::USER(int fd, const std::vector<std::string>& args, Server* server) {
    if (args.size() != 5) {
        // Handle error: USER command requires exactly four arguments
        return;
    }
    // Handle USER command logic here
}

void Commands::PING(int fd, const std::vector<std::string>& args, Server* server) {
    // Handle PING command logic here
}

void Commands::QUIT(int fd, const std::vector<std::string>& args, Server* server) {
    // Handle QUIT command logic here
}

void Commands::MODE(int fd, const std::vector<std::string>& args, Server* server) {
    // Handle MODE command logic here
}

void Commands::JOIN(int fd, const std::vector<std::string>& args, Server* server) {
    // Handle JOIN command logic here
}

void Commands::PART(int fd, const std::vector<std::string>& args, Server* server) {
    // Handle PART command logic here
}

void Commands::PRIVMSG(int fd, const std::vector<std::string>& args, Server* server) {
    // Handle PRIVMSG command logic here
}

void Commands::TOPIC(int fd, const std::vector<std::string>& args, Server* server) {
    // Handle TOPIC command logic here
}

void Commands::KICK(int fd, const std::vector<std::string>& args, Server* server) {
    // Handle KICK command logic here
}

void Commands::INVITE(int fd, const std::vector<std::string>& args, Server* server) {
    // Handle INVITE command logic here
}

