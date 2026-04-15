#include "includes/Server.hpp"

int main(int ac, char **av){
    if(ac != 3)
        return(std::cerr << "Wrong number of args", 1);
    try{
        std::cout << "[SERVER] Starting IRC server on port " << av[1] << "..." << std::endl;
        Server server(av[1], av[2]);
        std::cout << "[SERVER] Server initialized successfully" << std::endl;
        server.run();
    }
    catch(const std::exception& e){
        std::cout << e.what() << std::endl;
		return 1;
    }
	return 0;
}
