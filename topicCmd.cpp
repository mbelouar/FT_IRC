#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::topicCmd(std::vector<std::string>& args, int fd) {

    // std::cout << "args are : " << args[0] << " " << args[1] << " " << args[2] << std::endl;
    // std::cout << "size of args is : " << args.size() << std::endl;
    // int size = args.size();
    // for (int i = 0; i < size; i++) {
    //     std::cout << "args[" << i << "] = " << args[i] << std::endl;
    // }
    // if (args.size() <= 1 && args[0] == getClientNickname(fd)) {
    //     std::string msg = "403 " + getClientNickname(fd) + " :No such channel\n";
    //     sendMessage(fd, msg);
    //     return;
    // }

    const std::string& channelName = args[0];
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end()) {
        std::string msg = "403 " + getClientNickname(fd) + " " + channelName + " :No such channel\n";
        sendMessage(fd, msg);
    }
    else {
        std::string msg = "332 " + getClientNickname(fd) + " " + channelName + " :" + it->second.getTopic() + "\n";
        sendMessage(fd, msg);

        // Uncomment and adapt this block for topic setting once supported
        // Channel& channel = it->second;
        // if (args.size() == 3) {
        //     std::string newTopic = args[2];
        //     channel.setTopic(newTopic);
        //     std::string msg = "332 " + getClientNickname(fd) + " " + channelName + " :" + newTopic + "\n";
        //     sendMessage(fd, msg);
        // }
    }
}

