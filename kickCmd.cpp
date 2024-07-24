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
        std::string msg = "403 " + getClientNickname(fd) + " " + channelName + " :No such channel\n";
        sendMessage(fd, msg);
        return;
    }

    Channel& channel = it->second;
    int fdToKick = channel.getClientID(userToKick);
    if (fdToKick == -1) {
        std::string msg = "401 " + getClientNickname(fd) + " " + userToKick + " :No such nick/channel\n";
        sendMessage(fd, msg);
        return;
    }

    if (!channel.isClientInChannel(fdToKick)) {
        std::string msg = "441 " + getClientNickname(fd) + " " + userToKick + " " + channelName + " :They aren't on that channel\n";
        sendMessage(fd, msg);
        return;
    }
    else {
        std::cout << "Kicking user \"" << userToKick << "\" is on the channel \"" << channelName + "\"" << std::endl;
    }

   
    // std::cout << "Channel '" << it->first << "' BEFORE kick:" << std::endl;
    // const std::map<int, client>& clientsBeforeKick = it->second.getClientsFromChannel();
    // std::map<int, client>::const_iterator itClientBefore;
    // for (itClientBefore = clientsBeforeKick.begin(); itClientBefore != clientsBeforeKick.end(); ++itClientBefore) {
    //     std::cout << "Client ID: " << itClientBefore->first << ", Nickname: " << itClientBefore->second.getNickname() << std::endl;
    // }

    channel.removeClient(fdToKick);

    // std::cout << "Channel '" << it->first << "' AFTER kick:" << std::endl;
    // const std::map<int, client>& clientsAfterKick = it->second.getClientsFromChannel();
    // std::map<int, client>::const_iterator itClientAfter;
    // for (itClientAfter = clientsAfterKick.begin(); itClientAfter != clientsAfterKick.end(); ++itClientAfter) {
    //     std::cout << "Client ID: " << itClientAfter->first << ", Nickname: " << itClientAfter->second.getNickname() << std::endl;
    // }


    std::string msg = ":" + getClientNickname(fd) + " KICK " + channelName + " " + userToKick + " :Kicked by " + getClientNickname(fd) + "\n";
    sendMessage(fd, msg);
}