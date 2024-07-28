#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::inviteCmd(std::vector<std::string>& args, int fd) {
    if (args.size() < 2) {
        std::string msg = ":" + getClientHostName(fd) + " 461 " + getClientNickname(fd) + " INVITE :Not enough parameters\r\n";
        sendMessage(fd, msg);
        return;
    }

    const std::string& nickname = args[0];
    const std::string& channelName = args[1];

    std::map<std::string, Channel>::iterator it = channels.find(channelName);

    if (it == channels.end()) {
        std::string msg = ":" + getClientHostName(fd) + " 403 " + getClientNickname(fd) + " " + channelName + " :No such channel\r\n";
        sendMessage(fd, msg);
    } else {
        Channel& channel = it->second;
        if (!channel.isClientInChannel(fd)) {
            std::string msg = ":" + getClientHostName(fd) + " 442 " + getClientNickname(fd) + " " + channelName + " :You're not on that channel\r\n";
            sendMessage(fd, msg);
        } else {
            int clientFd = getClientFdByNickname(nickname);
            if (clientFd == -1) {
                std::string msg = ":" + getClientHostName(fd) + " 401 " + getClientNickname(fd) + " " + nickname + " :No such nick/channel\r\n";
                sendMessage(fd, msg);
            } else {
                // Add the client to the invite list
                channel.addInvite(clientFd);

                std::string msg = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " INVITE " + nickname + " :" + channelName + "\r\n";
                sendMessage(clientFd, msg);
                std::string msg2 = ":" + getClientHostName(fd) + " 341 " + getClientNickname(fd) + " " + nickname + " " + channelName + "\r\n";
                sendMessage(fd, msg2);
            }
        }
    }
}