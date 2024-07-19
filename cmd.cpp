#include "server.hpp"
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

    if (password.empty()) {
        newChannel.setChannelType(0);
        newChannel.setHasPassword(0);
    } else {
        newChannel.setChannelType(1);
        newChannel.setHasPassword(1);
        newChannel.setChPassword(password);
    }

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
    // Check if the param vector is empty, or the first parameter does not start with '#',
    // or if the first parameter is just '#' with no other characters.
    if (param.empty() || param[0][0] != '#' || (param[0][0] == '#' && param[0].size() == 1)) {
        // Construct an error message indicating "Not enough parameters" and send it to the client.
        std::string message = ":" + getNameId(fd) + " 461 JOIN :Not enough parameters\n";
        sendMessage(fd, message);
        return;
    }

    // Split the first parameter by commas to get the channel names.
    std::vector<std::string> chn = split(param[0], ',');
    std::vector<std::string> key;
    // If there are two parameters, split the second parameter by commas to get the keys.
    if (param.size() == 2) {
        key = split(param[1], ',');
    }

    // Loop through each channel name.
    for (size_t i = 0; i < chn.size(); ++i) {
        // Find the channel in the channels map.
        std::map<std::string, Channel>::iterator it = channels.find(chn[i]);
        bool channelCreated = (it == channels.end());

        // If the channel doesn't exist, create it and send a join message.
        if (channelCreated) {
            setupChannel(chn[i], fd, (param.size() == 2 && key.size() > i) ? key[i] : "");
            JoinMessage(fd, chn[i]);
            continue;
        }

        Channel &channel = it->second;

        // Check if the client is already in the channel.
        if (channel.isClientInChannel(fd) == 1) {
            std::string message = ":" + getNameId(fd) + " 443 * :You are already in this channel\n";
            sendMessage(fd, message);
            continue;
        }

        // If the channel is invite-only or has a password, handle those cases.
        if (channel.getChannelType() == 1) {
            // If the channel is invite-only and the client is not invited, send an error message.
            if (channel.getInvitedList().empty() && param.size() != 2) {
                std::string message = ":" + getNameId(fd) + " 473 * Cannot join channel (+i)\n";
                sendMessage(fd, message);
                continue;
            }

            // If the client is invited, add them to the channel and send a join message.
            if (channel.isClientInvited(fd)) {
                channel.setClients(fd, _clients[fd].getNickname());
                JoinMessage(fd, chn[i]);
                continue;
            }

            // If the channel has a password, check the provided key.
            if (channel.getHasPassword() == 1) {
                // If the provided key matches the channel password, add the client to the channel.
                if ((key.size() > i && channel.getChPassword() == key[i]) || key.empty()) {
                    channel.setClients(fd, _clients[fd].getNickname());
                    JoinMessage(fd, chn[i]);
                } else {
                    // If the key doesn't match, send an error message.
                    std::string message = ":" + getNameId(fd) + " 475 * Cannot join channel (+k)\n";
                    sendMessage(fd, message);
                }
                continue;
            }
        }

        // If the channel has a password, check the provided key.
        if (channel.getHasPassword() == 1) {
            // If the provided key matches the channel password, add the client to the channel.
            if (key.size() > i && channel.getChPassword() == key[i]) {
                channel.setClients(fd, _clients[fd].getNickname());
                JoinMessage(fd, chn[i]);
            } else {
                // If the key doesn't match, send an error message.
                std::string message = ":" + getNameId(fd) + " 475 * Cannot join channel (+k)\n";
                sendMessage(fd, message);
            }
            continue;
        }

        // If none of the above conditions are met, add the client to the channel and send a join message.
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