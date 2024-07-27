#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::modeCmd(std::vector<std::string> &param, int fd) {
    if (param.size() < 2) {
        std::string message = "461 " + getClientNickname(fd) + " MODE :Not enough parameters\n";
        sendMessage(fd, message);
        return;
    }

    // Implement the mode command here
}

