#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::whoCmd(std::vector<std::string>& args, int fd) {
    if (args.size() == 0) {
        std::string msg = "431 " + getClientNickname(fd) + " :No nickname given\n";
        sendMessage(fd, msg);
        return;
    }

    const std::string& channelName = args[0];
    std::map<std::string, Channel>::iterator it = channels.find(channelName);

    if (it == channels.end()) {
        std::string msg = "403 " + getClientNickname(fd) + " " + channelName + " :No such channel\n";
        sendMessage(fd, msg);
    }
    else {
        // must know the clients in the channel
        std::cout << "List of users in the channel \"" << channelName << "\"     :" << std::endl;
    }
}