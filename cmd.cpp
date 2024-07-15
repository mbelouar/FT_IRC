#include "Server.hpp"
#include "Channel.hpp"
#include "client.hpp"
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>

void sendMessage(int fd, const std::string& msg) {
    if (send(fd, msg.c_str(), msg.size(), 0) == -1) {
        throw std::runtime_error("error: send() failed");
    }
}

std::vector<std::string> split(std::string line, char c) {
    std::vector<std::string> vec;
    size_t pos;
    while ((pos = line.find(c)) != std::string::npos) {
        vec.push_back(line.substr(0, pos));
        line.erase(0, pos + 1);
    }
    vec.push_back(line);
    return vec;
}

void Server::setupChannel(const std::string &channelName, int fd, const std::string &password) {
    Channel newChannel;
    newChannel.setName(channelName);
    newChannel.setTopic("No topic");
    newChannel.setClients(fd, _clients[fd].getNickname());
    newChannel.setOperators(fd);
    newChannel.setChannelType(password.empty() ? 0 : 1);
    newChannel.setHasPassword(password.empty() ? 0 : 1);
    newChannel.setChPassword(password);
    channels[channelName] = newChannel;
}

void Server::JoinMessage(int fd, const std::string &channelName) {
    std::string message = ":" + getNameId(fd) + " 332 JOIN :" + channels[channelName].getTopic() + "\n";
    sendMessage(fd, message);
}

std::string Server::getNameId(int fd) {
    std::map<int, client>::iterator it = _clients.find(fd);
    if (it != _clients.end()) {
        return it->second.getNickname();
    }
    std::string message = ":" + std::to_string(fd) + " 401 * :No such nick/channel\n";  
    sendMessage(fd, message);
    return "ERROR"; 
}

void Server::joinCmd(std::vector<std::string> &param, int fd) {
    if (param.empty() || param[0][0] != '#' || (param[0][0] == '#' && param[0].size() == 1)) {
        std::string message = ":" + getNameId(fd) + " 461 JOIN :Not enough parameters\n";
        sendMessage(fd, message);
        return;
    }

    std::vector<std::string> chn = split(param[0], ',');
    std::vector<std::string> key;
    if (param.size() == 2) {
        key = split(param[1], ',');
    }

    for (size_t i = 0; i < chn.size(); ++i) {
        std::map<std::string, Channel>::iterator it = channels.find(chn[i]);
        bool channelCreated = (it == channels.end());

        if (channelCreated) {
            setupChannel(chn[i], fd, (param.size() == 2 && key.size() > i) ? key[i] : "");
            JoinMessage(fd, chn[i]);
            continue;
        }

        Channel &channel = it->second;

        if (channel.isClientInChannel(fd) == 1) {
            std::string message = ":" + getNameId(fd) + " 443 * :You are already in this channel\n";
            sendMessage(fd, message);
            continue;
        }

        if (channel.getChannelType() == 1) {
            if (channel.getInvitedList().empty() && param.size() != 2) {
                std::string message = ":" + getNameId(fd) + " 473 * Cannot join channel (+i)\n";
                sendMessage(fd, message);
                continue;
            }

            if (channel.isClientInvited(fd)) {
                channel.setClients(fd, _clients[fd].getNickname());
                JoinMessage(fd, chn[i]);
                continue;
            }

            if (channel.getHasPassword() == 1) {
                if ((key.size() > i && channel.getChPassword() == key[i]) || key.empty()) {
                    channel.setClients(fd, _clients[fd].getNickname());
                    JoinMessage(fd, chn[i]);
                } else {
                    std::string message = ":" + getNameId(fd) + " 475 * Cannot join channel (+k)\n";
                    sendMessage(fd, message);
                }
                continue;
            }
        }

        if (channel.getHasPassword() == 1) {
            if (key.size() > i && channel.getChPassword() == key[i]) {
                channel.setClients(fd, _clients[fd].getNickname());
                JoinMessage(fd, chn[i]);
            } else {
                std::string message = ":" + getNameId(fd) + " 475 * Cannot join channel (+k)\n";
                sendMessage(fd, message);
            }
            continue;
        }

        channel.setClients(fd, _clients[fd].getNickname());
        JoinMessage(fd, chn[i]);
    }
}

void Server::command(int fd) {
    std::string line = _clients[fd].get_message();
    std::istringstream stream(line);
    std::string command;
    std::vector<std::string> res;

    stream >> command;
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    std::string arg;
    while (stream >> arg) {
        res.push_back(arg);
    }

    if (command == "join") {
        joinCmd(res, fd);
    } else {
        std::string msg = "421 " + _clients[fd].getNickname() + " " + command + " :Unknown command\n";
        sendMessage(fd, msg);
    }
}
