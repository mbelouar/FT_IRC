#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::privmsgCmd(std::vector<std::string>& args, int fd) {
    if (args.size() < 2) {
        std::string msg = "411 " + getClientNickname(fd) + " :No recipient given (PRIVMSG)\n";
        sendMessage(fd, msg);
        return;
    }

    const std::string& recipient = args[0];
    const std::string& message = args[1];

    std::map<std::string, client>::iterator it = clients.find(recipient);
    if (it == clients.end()) {
        std::string msg = "401 " + getClientNickname(fd) + " " + recipient + " :No such nick/channel\n";
        sendMessage(fd, msg);
    } else {
        std::string msg = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + " PRIVMSG " + recipient + " :" + message + "\n";
        sendMessage(fd, msg);
    }
}
