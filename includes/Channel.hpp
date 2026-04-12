#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include <map>
#include <set>


class Channel {
    private:
        std::string _name;
        std::string _topic;
        std::map<int, Client*> _members; 
        std::set<int> _operators;
        bool _inviteOnly;
        bool _topicRestricted;
        std::string _key;
        int _userLimit;
        
    public:
        Channel();
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
        
        void setInviteOnly(bool flag);
        bool isInviteOnly() const;
        void setTopicRestricted(bool flag);
        bool isTopicRestricted() const;
        void setKey(const std::string& key);
        std::string getKey() const;
        void removeKey();
        void setUserLimit(int limit);
        int getUserLimit() const;
        void removeUserLimit();
};


#endif