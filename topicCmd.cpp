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
        std::string msg = "403 " + getClientNickname(fd) + " " + channelName + " :No such channel\n";
        sendMessage(fd, msg);
    }
    else if (args.size() == 1) {
        const std::string& topic = it->second.getTopic();
        if (topic.empty()) {
            std::string msg = "331 " + getClientNickname(fd) + " " + channelName + " :No topic is set\n";
            sendMessage(fd, msg);
        }
        else {
            std::string msg = "332 " + getClientNickname(fd) + " " + channelName + " " + topic + "\n";
            sendMessage(fd, msg);
        }
    }
    else {
        // Set topic
        std:: string newTopic = args[1];
        for (size_t i = 2; i < args.size(); ++i) {
            newTopic += " " + args[i];
        }
        std::string msg = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + "127.0.0.1 TOPIC " + channelName + " " + newTopic + "\n";
        sendMessage(fd, msg);

        Channel& channel = it->second;
        channel.setTopic(newTopic);
    }
}


