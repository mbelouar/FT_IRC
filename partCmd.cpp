#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::partCmd(std::vector<std::string>& args, int fd) {
    if (args.size() == 0) {
        std::string msg = "461 " + getClientNickname(fd) + " PART :Not enough parameters\n";
        sendMessage(fd, msg);
        return;
    }

    const std::string& channelName = args[0];
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end()) {
        std::string msg = "403 " + getClientNickname(fd) + " " + channelName + " :No such channel\n";
        sendMessage(fd, msg);
        return;
    }

    Channel& channel = it->second;
    if (!channel.isClientInChannel(fd)) {
        std::string msg = "442 " + getClientNickname(fd) + " " + channelName + " :You're not on that channel\n";
        sendMessage(fd, msg);
        return;
    }

    channel.removeClient(fd);
    // :simo!simo@88ABE6.25BF1D.D03F86.88C9BD.IP PART #chan :Leaving
    std::string msg = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " PART " + channelName + " :Leaving\n";

    // Send the message to all clients in the channel
    std::map<int, client>::const_iterator clientIt;
    const std::map<int, client>& clients = channel.getClientsFromChannel();
    for (clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
        sendMessage(clientIt->first, msg);
    }

    sendMessage(fd, msg);
}