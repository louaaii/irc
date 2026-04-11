#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"
#include "Client.hpp"


class Channel {
private:
    std::string _name;
    std::string _topic;
    std::map<int, Client*> _members; // Map of client file descriptors to Client pointers
    std::set<int> _operators; // Set of client file descriptors who are operators   
public:
    Channel(const std::string& name);
    ~Channel();
    const std::string& getName() const;
    const std::string& getTopic() const;
    void setTopic(const std::string& topic);
    void addMember(Client* client);
    void removeMember(int fd);
    bool isMember(int fd) const;
    void addOperator(int fd);
    void removeOperator(int fd);
    bool isOperator(int fd) const;
};


#endif