#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::topicCmd(std::vector<std::string>& args, int fd) {

    const std::string& channelName = args[0];
    std::map<std::string, Channel>::iterator it = channels.find(channelName);

    if (it == channels.end()) {
        std::string msg = ":" + getClientHostName(fd) + " 403 " + getClientNickname(fd) + " " + channelName + " :No such channel\r\n";
        sendMessage(fd, msg);
    }
    else if (args.size() == 1) {
        const std::string& topic = it->second.getTopic();
        if (topic.empty()) {
            std::string msg = ":" + getClientHostName(fd) + " 331 " + getClientNickname(fd) + " " + channelName + " :No topic is set\r\n";
            sendMessage(fd, msg);
        }
        else {
            std::string msg = ":" + getClientHostName(fd) + " 332 " + getClientNickname(fd) + " " + channelName + " :" + topic + "\r\n";
            sendMessage(fd, msg);
        }
    }
    else {
        // Set topic
        std:: string newTopic = args[1];
        for (size_t i = 2; i < args.size(); ++i) {
            newTopic += " " + args[i];
        }
        std::string msg = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " TOPIC " + channelName + " :" + newTopic + "\r\n";
        sendMessage(fd, msg);

        // notify all clients in the channel
        std::map<int, client>::const_iterator clientIt;
        const std::map<int, client>& clients = it->second.getClientsFromChannel();
        for (clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
            if (clientIt->first != fd) {
                sendMessage(clientIt->first, msg);
            }
        }

        Channel& channel = it->second;
        channel.setTopic(newTopic);
    }
}


