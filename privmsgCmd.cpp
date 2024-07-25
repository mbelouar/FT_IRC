#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::privmsgCmd(std::vector<std::string>& args, int fd) {

    if (args.size() < 1) {
        std::string msg = "411 " + getClientNickname(fd) + " :No recipient given (PRIVMSG)\n";
        sendMessage(fd, msg);
        return;
    }
    else if (args.size() == 1) {
        std::string msg = "412 " + getClientNickname(fd) + " :No text to send\n";
        sendMessage(fd, msg);
        return;
    }

    const std::string& recipient = args[0];
    std::string message = args[1];
    for (size_t i = 2; i < args.size(); ++i) {
        message += " " + args[i];
    }

    std::map<std::string, client>::iterator clientIt = clients.find(recipient);
    std::map<std::string, Channel>::iterator channelIt = channels.find(recipient);

    if (clientIt == clients.end() && channelIt == channels.end()) {
        // Recipient not found in both clients and channels
        std::string msg = "401 " + getClientNickname(fd) + " " + recipient + " :No such nick/channel\n";
        sendMessage(fd, msg);
    }
    else if (clientIt != clients.end()) {
        // Recipient found as a client
        client& recipientClient = clientIt->second;
        std::string msg = ":" + getClientNickname(fd) + " PRIVMSG " + recipient + " :" + message + "\n";
        sendMessage(recipientClient.get_client_pfd().fd, msg);
    }
    else if (channelIt != channels.end()) {
        // Recipient found as a channel
        std::cout << "Channel found" << std::endl;
        Channel& channel = channelIt->second;
        // t!tttt@88ABE6.25BF1D.D03F86.88C9BD.IP
        std::string msg = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@127.0.0.1" + " PRIVMSG " + recipient + " :" + message + "\n";
        for (std::map<int, client>::const_iterator it = channel.beginClientIter(); it != channel.endClientIter(); ++it) {
            sendMessage(it->first, msg);
        }
    }
}
