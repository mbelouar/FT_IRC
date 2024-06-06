#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <arpa/inet.h>

class Server
{
    private :
        int sockfd;
        struct addrinfo hints, *server;
        std::vector<pollfd> fds;
    public :
        Server(std::string ip, std::string port);
        void bind_socket();
        void listen_socket();
        std::vector<pollfd> get_fds();
        int get_sockfd();
        ~Server();

        
};
#endif

