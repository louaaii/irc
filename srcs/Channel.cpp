#include "Channel.hpp"
#include <cstdlib>

Channel::Channel() : _name(""), _topic(""), _inviteOnly(false), _topicRestricted(false), _key(""), _userLimit(-1) {
}

Channel::Channel(const std::string& name) : _name(name), _topic(""), _inviteOnly(false), _topicRestricted(false), _key(""), _userLimit(-1) {
}

Channel::~Channel() {
}

const std::string& Channel::getName() const {
    return _name;
}

const std::string& Channel::getTopic() const {
    return _topic;
}

void Channel::setTopic(const std::string& topic) {
    _topic = topic;
}

void Channel::addMember(Client* client) {
    if (client != NULL) {
        int fd = client->get_fd();
        _members[fd] = client;
    }
}

void Channel::removeMember(int fd) {
    _members.erase(fd);
    _operators.erase(fd);
}

bool Channel::isMember(int fd) const {
    return _members.find(fd) != _members.end();
}

void Channel::addOperator(int fd) {
    if (isMember(fd))
        _operators.insert(fd);
}

void Channel::removeOperator(int fd) {
    _operators.erase(fd);
}

bool Channel::isOperator(int fd) const {
    return _operators.find(fd) != _operators.end();
}

void Channel::setInviteOnly(bool flag) {
    _inviteOnly = flag;
}

bool Channel::isInviteOnly() const {
    return _inviteOnly;
}

void Channel::setTopicRestricted(bool flag) {
    _topicRestricted = flag;
}

bool Channel::isTopicRestricted() const {
    return _topicRestricted;
}

void Channel::setKey(const std::string& key) {
    _key = key;
}

std::string Channel::getKey() const {
    return _key;
}

void Channel::removeKey() {
    _key = "";
}

void Channel::setUserLimit(int limit) {
    _userLimit = limit;
}

int Channel::getUserLimit() const {
    return _userLimit;
}

void Channel::removeUserLimit() {
    _userLimit = -1;
}
