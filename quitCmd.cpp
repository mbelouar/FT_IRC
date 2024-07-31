#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::quitCmd(std::vector<std::string>& args, int fd) {
    if (args.size() == 0) {
        // erase client from clients vector
        std::vector<client>& clients = client::get_clients(); // remove const to allow modification
        for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ) {
            if (it->getFd() == fd) {
                it = clients.erase(it);
            } else {
                ++it; // only increment if not erased
            }
        }
        std::string msg = "QUIT :Leaving\n";
        sendMessage(fd, msg);
        return;
    }

    std::vector<client>& clients = client::get_clients(); // remove const to allow modification
    std::string notice = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " QUIT :Quit: " + args[0] + "\n";
    for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ) {
        if (it->getFd() == fd) {
            it = clients.erase(it);
        } else {
            sendMessage(it->getFd(), notice);
            ++it; // only increment if not erased
        }
    }

    std::string msg = "QUIT :" + args[0] + "\n";
    sendMessage(fd, msg);
}