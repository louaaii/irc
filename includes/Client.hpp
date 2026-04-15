#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>

class Client{
    private:

        std::string _username;
		std::string _nickname;
		std::string _hostname;
		std::string _inBuffer;
		std::string _outBuffer;
		std::set<std::string> _channels;

        int _fd;
		bool _passOK;
		bool _hasNick;
		bool _hasUser;

    public:
		Client();
        Client(int fd);
        ~Client();

    	int get_fd() const;
        std::string get_nick() const;

        void set_fd(int fd);
        void set_nick(std::string nick);
		void setPassOk(bool flag);
		bool isRegistered() const;
		bool isPassOk() const;
		bool hasNick() const;


		// outils pour plus tard
		void set_Username(std::string username);
		void joinChannel(std::string channelName);
		void partChannel(std::string channelName);
		bool isInChannel(std::string channelName);
		void set_hostname(std::string hostname);
		
};

#endif
