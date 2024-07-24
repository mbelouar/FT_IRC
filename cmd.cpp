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
    // int size = res.size();
    // std::cout << "size of res is : " << size << std::endl;
    // for (int i = 0; i < size; i++) {
    //     std::cout << "res[" << i << "] = " << res[i] << std::endl;
    // }
    if (command.compare("join") == 0) 
        joinCmd(res, fd);
    else if (command.compare("kick") == 0)
        kickCmd(res, fd);
    else if (command.compare("topic") == 0)
        topicCmd(res, fd);
    else if (command.compare("who") == 0)
        whoCmd(res, fd);
    // else if (command.compare("privmsg") == 0)
    //     privmsgCmd(res, fd);
    else {
        std::string msg = "421 " + getClientNickname(fd) + " " + command + " :Unknown command\n";
        sendMessage(fd, msg);
    }
}
