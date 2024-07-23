#include "server.hpp"
#include "Channel.hpp"
#include "client.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>

void Server::command(int fd) {
    std::string line = getClientMessage(fd);
    std::istringstream stream(line);
    std::string command;
    std::vector<std::string> res;

    stream >> command;
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    std::string arg;
    while (stream >> arg) {
        res.push_back(arg);
    }
    if (command.compare("join") == 0) 
        joinCmd(res, fd);
    else if (command.compare("kick") == 0)
        kickCmd(res, fd);
    else if (command.compare("list") == 0)
        listCmd(res, fd);
    else if (command.compare("part") == 0)
        partCmd(res, fd);
    else if (command.compare("privmsg") == 0)
        privmsgCmd(res, fd);
    else if (command.compare("users") == 0)
        usersCmd(res, fd);
    else if (command.compare("nick") == 0)
        nickCmd(res, fd);
    else if (command.compare("quit") == 0)
        quitCmd(res, fd);
    else if (command.compare("topic") == 0)
        topicCmd(res, fd);
    else {
        std::string msg = "421 " + getClientNickname(fd) + " " + command + " :Unknown command\n";
        sendMessage(fd, msg);
    }
}
