#include "server.hpp"
#include "utils.hpp"

Server::Server(std::string ip, std::string port)
{

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int r = getaddrinfo(ip.c_str(), port.c_str(), &hints, &server);
    if (r != 0)
    {
        std::cout << "faild" << std::endl;
    }
    sockfd = socket(server->ai_family,server->ai_socktype, server->ai_protocol);
    if (sockfd == -1)
    {
        std::cout << "error in creating socket" << std::endl;
    }

    pollfd pfd;
    pfd.fd = this->sockfd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    this->fds.push_back(pfd);
}


Server::~Server()
{
    freeaddrinfo(this->server);
    close(this->sockfd);
    std::cout << "server closed" << std::endl;
}

void Server::bind_socket()
{
    int r = bind(sockfd, server->ai_addr, server->ai_addrlen);
    if ( r == -1)
    {
        std::cout << "error in bind function" << std::endl;
    }
}

void Server::setsockopt()
{
    int opt = 1;
int r = ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (r == -1)
    {
        std::cout << "error in setsockopt function" << std::endl;
    }
}

void Server::listen_socket()
{
    int r = listen(sockfd, 1000);
    if(r == -1)
    {
        std::cout << "error in listen function" << std::endl;
    }
}

int Server::get_sockfd()
{
    return this->sockfd;
}

std::vector<pollfd> Server::get_fds()
{
    return this->fds;
}

std::string Server::getClientNickname(int fd) {
    std::vector<client>& clients = client::get_clients();
    std::vector<client>::iterator it = std::find_if(clients.begin(), clients.end(), IsClientWithFd(fd));
    return (it != clients.end()) ? it->getNickname() : "Unknown";
}

std::string Server::getClientMessage(int fd) {
    std::vector<client>& clients = client::get_clients();
    std::vector<client>::iterator it = std::find_if(clients.begin(), clients.end(), IsClientWithFd(fd));
    return (it != clients.end()) ? it->get_message() : "";
}

std::string Server::getClientHostName(int fd) {
    std::vector<client>& clients = client::get_clients();
    std::vector<client>::iterator it = std::find_if(clients.begin(), clients.end(), IsClientWithFd(fd));
    return (it != clients.end()) ? it->get_host_name() : "";
}

std::string Server::getNameId(int fd) {
    std::vector<client>& clients = client::get_clients();
    for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->get_client_pfd().fd == fd) {
            return it->getNickname();
        }
    }
    std::string message = ":" + std::to_string(fd) + " 401 * :No such nick/channel\n";  
    sendMessage(fd, message);
    return "ERROR"; 
}