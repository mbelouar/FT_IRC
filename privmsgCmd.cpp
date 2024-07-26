#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::privmsgCmd(std::vector<std::string>& args, int fd) {

    if (args.size() < 1) {
        std::string msg = "411 " + getClientNickname(fd) + " :No recipient given (PRIVMSG)\n\r";
        sendMessage(fd, msg);
        return;
    }
    else if (args.size() == 1) {
        std::string msg = "412 " + getClientNickname(fd) + " :No text to send\n\r";
        sendMessage(fd, msg);
        return;
    }

    const std::string& recipient = args[0];
    std::string message = args[1];
    for (size_t i = 2; i < args.size(); ++i) {
        message += " " + args[i];
    }

    std::vector<client>& clients = client::get_clients();
    bool recipientFound = false;
    client* recipientClient = nullptr;

    // Check if the recipient is a client
    for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->getNickname() == recipient) {
            recipientClient = &(*it);   // Point to the recipient client
            recipientFound = true;
            break;
        }
    }

    if (!recipientFound) { 
        // Recipient not found as a client
        std::map<std::string, Channel>::iterator channelIt = channels.find(recipient);

        if (channelIt == channels.end()) {
            std::string msg = "401 " + getClientNickname(fd) + " " + recipient + " :No such nick/channel\n\r";
            sendMessage(fd, msg);
        }
        else {
            std::cout << "Channel found" << std::endl;
            Channel& channel = channelIt->second;
            std::string msg = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " PRIVMSG " + recipient + " :" + message + "\n\r";
            for (std::map<int, client>::const_iterator it = channel.beginClientIter(); it != channel.endClientIter(); ++it) {
                if (it->first != fd) {
                    sendMessage(it->first, msg);
                }
            }
        }
    }
    else {
        // Recipient found as a client
        std::cout << "Client found" << std::endl;
        std::string msg = ":" + getClientNickname(fd) + " PRIVMSG " + recipient + " :" + message + "\n\r";
        sendMessage(recipientClient->get_client_pfd().fd, msg);
    }
}
