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
    std::cout << "[CHANNEL] Topic changed in " << _name << " to: " << topic << std::endl;
    _topic = topic;
}

void Channel::addMember(Client* client) {
    if (client != NULL) {
        int fd = client->get_fd();
        std::cout << "[CHANNEL] Adding member fd " << fd << " to channel " << _name << std::endl;
        _members[fd] = client;
    }
}

void Channel::removeMember(int fd) {
    std::cout << "[CHANNEL] Removing member fd " << fd << " from channel " << _name << std::endl;
    _members.erase(fd);
    _operators.erase(fd);
}

bool Channel::isMember(int fd) const {
    return _members.find(fd) != _members.end();
}

int Channel::get_member_count() const {
    return _members.size();
}

void Channel::addOperator(int fd) {
    if (isMember(fd)) {
        std::cout << "[CHANNEL] Making fd " << fd << " operator of channel " << _name << std::endl;
        _operators.insert(fd);
    }
}

void Channel::removeOperator(int fd) {
    std::cout << "[CHANNEL] Removing fd " << fd << " as operator of channel " << _name << std::endl;
    _operators.erase(fd);
}

bool Channel::isOperator(int fd) const {
    return _operators.find(fd) != _operators.end();
}

void Channel::setInviteOnly(bool flag) {
    std::cout << "[CHANNEL] Setting invite-only mode to " << flag << " for " << _name << std::endl;
    _inviteOnly = flag;
}

bool Channel::isInviteOnly() const {
    return _inviteOnly;
}

void Channel::addInvitedUser(int fd) {
    std::cout << "[CHANNEL] Adding fd " << fd << " to invite list for " << _name << std::endl;
    _invitedUsers.insert(fd);
}

bool Channel::isUserInvited(int fd) const {
    return _invitedUsers.find(fd) != _invitedUsers.end();
}

void Channel::removeInvitedUser(int fd) {
    _invitedUsers.erase(fd);
}

void Channel::setTopicRestricted(bool flag) {
    std::cout << "[CHANNEL] Setting topic-restricted mode to " << flag << " for " << _name << std::endl;
    _topicRestricted = flag;
}

bool Channel::isTopicRestricted() const {
    return _topicRestricted;
}

void Channel::setKey(const std::string& key) {
    std::cout << "[CHANNEL] Setting key for " << _name << std::endl;
    _key = key;
}

std::string Channel::getKey() const {
    return _key;
}

void Channel::removeKey() {
    std::cout << "[CHANNEL] Removing key from " << _name << std::endl;
    _key = "";
}

void Channel::setUserLimit(int limit) {
    std::cout << "[CHANNEL] Setting user limit to " << limit << " for " << _name << std::endl;
    _userLimit = limit;
}

int Channel::getUserLimit() const {
    return _userLimit;
}

void Channel::removeUserLimit() {
    std::cout << "[CHANNEL] Removing user limit from " << _name << std::endl;
    _userLimit = -1;
}
