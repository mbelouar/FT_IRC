#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::kickCmd(std::vector<std::string>& args, int fd) {
    if (args.size() <= 2) {
        std::string msg = "461 " + getClientNickname(fd) + " KICK :Not enough parameters\n";
        sendMessage(fd, msg);
        return;
    }

    const std::string& channelName = args[1];
    std::string userToKick = args[2];
    if (userToKick[0] == ':') {
        userToKick = userToKick.substr(1);
    }

    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end()) {
        std::string msg = "403 " + getClientNickname(fd) + " " + channelName + " :No such channel\n\r";
        sendMessage(fd, msg);
        return;
    }

    Channel& channel = it->second;
    int fdToKick = channel.getClientID(userToKick);
    if (fdToKick == -1) {
        std::string msg = "401 " + getClientNickname(fd) + " " + userToKick + " :No such nick/channel\n\r";
        sendMessage(fd, msg);
        return;
    }

    if (!channel.isClientInChannel(fdToKick)) {
        std::string msg = "441 " + getClientNickname(fd) + " " + userToKick + " " + channelName + " :They aren't on that channel\n\r";
        sendMessage(fd, msg);
        return;
    }
    else {
        std::cout << "Kicking user \"" << userToKick << "\" is on the channel \"" << channelName + "\"" << std::endl;
    }

    channel.removeClient(fdToKick);


    std::string msg = ":" + getClientNickname(fd) + " KICK " + channelName + " " + userToKick + " :" + getClientNickname(fd) + "\n\r";
    sendMessage(fd, msg);

    // Send the message to the client being kicked
    std::string kickMsg = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " KICK " + channelName + " " + userToKick + " :" + getClientNickname(fd) + "\n\r";
    sendMessage(fdToKick, kickMsg);

}