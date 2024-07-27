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
        std::map<std::string , Channel> channels;
        std::map<std::string, client> clients;
        // std::map<int, client> _clients;
       
    public :
        Server(std::string ip, std::string port);
        void bind_socket();
        void setsockopt();
        void listen_socket();
        std::vector<pollfd> get_fds();
        int get_sockfd();
        ~Server();

        void joinCmd(std::vector<std::string> &param, int fd);
        void privmsgCmd(std::vector<std::string>& args, int fd);
        void modeCmd(std::vector<std::string>& args, int fd);
        void whoCmd(std::vector<std::string>& args, int fd);
        void kickCmd(std::vector<std::string> &param, int fd);
        void nickCmd(std::vector<std::string> &param, int fd);
        void topicCmd(std::vector<std::string> &param, int fd);
        void partCmd(std::vector<std::string>& args, int fd);
        void quitCmd(std::vector<std::string>& args, int fd);
        int getClientFdByNickname(const std::string& nickname);
        std::string getNameId(int fd);
        void joinMessage(int fd, const std::string &channelName);
        void setupChannel(const std::string &channelName, int fd, const std::string &password);
        void JoinMessage(int fd, const std::string &channelName);
        void command(int fd);
        std::string getClientMessage(int fd);
        std::string getClientNickname(int fd);
        std::string getClientHostName(int fd);

        class IsClientWithFd {
            public:
                IsClientWithFd(int fd) : fd_(fd) {}
                bool operator()(const client& c) const {
                    return c.get_client_pfd().fd == fd_;
                }
            private:
                int fd_;
        };
};
#endif