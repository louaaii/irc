#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client{
    private:

        std::string _username;
		std::string _nickname;
		std::string _inBuffer;
		std::string _outBuffer;

		bool _passOK;
		bool _hasNick;
		bool _hasUser;
		bool _registered;
        int _fd;

    public:
        Client(int fd);
        ~Client();

    	int get_fd() const;
        std::string get_nick() const;

        void set_fd(int fd);
        void set_nick(std::string nick);
		void setPassOk(bool flag);
		bool isRegistered() const;
};

#endif
