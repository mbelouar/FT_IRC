#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <arpa/inet.h>
#include <map>
#include "client.hpp"
#include "Channel.hpp"

class Server
{
    private :
        int sockfd;
        struct addrinfo hints, *server;
        std::vector<pollfd> fds;

        std::map<int, client>										_map;
		static bool 												romoved_channel;
		std::map<std::string, Channel>								_channels;
       
    public :
        Server(std::string ip, std::string port);
        void bind_socket();
        void setsockopt();
        void listen_socket();
        std::vector<pollfd> get_fds();
        int get_sockfd();
        ~Server();

        void    parse_request(char *read, int fd, fd_set *master);
        std::string join_response(std::vector<std::string> split, client &client);

        
};
#endif

