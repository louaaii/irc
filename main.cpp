#include "includes/Server.hpp"

int main(int ac, char **av){
    if(ac != 3)
        return(std::cerr << "Wrong number of args", 1);
    try{
        Server server(av[1], av[2]);
        server.run();
    }
    catch(const std::exception& e){
        std::cout << e.what() << std::endl;
        close(server._listenFd);
    }
}