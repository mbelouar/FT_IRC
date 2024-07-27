#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::quitCmd(std::vector<std::string>& args, int fd) {
    if (args.size() == 0) {
        std::string msg = "QUIT :Leaving\n";
        sendMessage(fd, msg);
        return;
    }

    const std::vector<client>& clients = client::get_clients();
    // notify all clients that the user is leaving
    // :simo!simo@88ABE6.25BF1D.D03F86.88C9BD.IP QUIT :Quit: #cha
    std::string notice = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " QUIT :Quit: " + args[0] + "\n";
    for (std::vector<client>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->getFd() != fd) {
            sendMessage(it->getFd(), notice);
        }
    }

    std::string msg = "QUIT :" + args[0] + "\n";
    sendMessage(fd, msg);
}