#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::inviteCmd(std::vector<std::string>& args, int fd) {
    // start implementation invite command
    if (args.size() < 2) {
        std::string msg = "461 " + getClientNickname(fd) + " INVITE :Not enough parameters\n";
        sendMessage(fd, msg);
        return;
    }
}