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
    if (channels.find(channelName) == channels.end()) {
        Channel newChannel(channelName, 0);  
        channels[channelName] = newChannel;
    } else {
        // Ensure not to reset the topic if the channel already exists
        Channel &channel = channels[channelName];
        if (channel.getTopic() == "No topic") {
            channel.setTopic("No topic");
        }
    }

    // Additional setup code
    Channel &channel = channels[channelName];
    channel.setClients(fd, getClientNickname(fd));
    if (password.empty()) {
        channel.setChannelType(0);  // Public
        channel.setHasPassword(0);
    } else {
        channel.setChannelType(1);  // Password protected
        channel.setHasPassword(1);
        channel.setChPassword(password);
    }
}

void Server::JoinMessage(int fd, const std::string &channelName) {
    if (channels.find(channelName) != channels.end()) {
        std::string globmsg = "";
        std::string topic = channels[channelName].getTopic();  // Get topic
        std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + "127.0.0.1 JOIN " + channelName + " * :realname\r\n";
        globmsg += message;
        std::string message2 = ":localhost 353 " + getClientNickname(fd) + " = " + channelName + " :@" + getClientNickname(fd) + "\r\n";
        globmsg += message2;
        std::string message3 = ":localhost 366 " + getClientNickname(fd) + " " + channelName + " :End of /NAMES list.\r\n";
        globmsg += message3;
        // std::string message4 = ":localhost 324 " + getClientNickname(fd) + " " + channelName + " +tn\r\n";
        // globmsg += message4;
        // std::string message5 = ":localhost 329 " + getClientNickname(fd) + " " + channelName + " 1721765133\r\n";
        // globmsg += message5;
        // std::string message6 = ":localhost 354 " + getClientNickname(fd) + " 152 " + channelName + " " + getClientNickname(fd) + "127.0.0.1 *.localhost " + getClientNickname(fd) + " H@x 0 :realname\r\n";
        // globmsg += message6;
        // std::string message7 = ":localhost 315 " + getClientNickname(fd) + " " + channelName + " :End of /WHO list.\r\n";
        // globmsg += message7;
        sendMessage(fd, globmsg);
    } else {
        std::string globmsg = ":" + getNameId(fd) + " 403 " + channelName + " :No such channel\n";
        sendMessage(fd, globmsg);
    }
}

std::string Server::getNameId(int fd) {
    std::vector<client>& clients = client::get_clients();
    for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->get_client_pfd().fd == fd) {
            return it->getNickname();
        }
    }
    std::string message = ":" + std::to_string(fd) + " 401 * :No such nick/channel\n";  
    sendMessage(fd, message);
    return "ERROR"; 
}

class IsClientWithFd {
public:
    IsClientWithFd(int fd) : fd_(fd) {}
    bool operator()(const client& c) const {
        return c.get_client_pfd().fd == fd_;
    }
private:
    int fd_;
};

void Server::joinCmd(std::vector<std::string> &param, int fd) {
    // Debugging output for received parameters
    std::cout << "Received /join command with parameters: ";
    for (size_t i = 0; i < param.size(); ++i) {
        std::cout << param[i] << " ";
    }
    std::cout << std::endl;

    // Check if the param vector is empty or invalid
    if (param.empty() || param[0][0] != '#' || (param[0].size() == 1)) {
        std::string message = ":" + getNameId(fd) + " 461 JOIN :Not enough parameters\n";
        sendMessage(fd, message);
        std::cout << "Error: Not enough parameters or invalid channel name." << std::endl;
        return;
    }

    // Split the parameters
    std::vector<std::string> chn = split(param[0], ',');
    std::vector<std::string> key;
    if (param.size() == 2) {
        key = split(param[1], ',');
    }

    std::cout << "Channel names to join: ";
    for (size_t i = 0; i < chn.size(); ++i) {
        std::cout << chn[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Keys provided: ";
    for (size_t i = 0; i < key.size(); ++i) {
        std::cout << key[i] << " ";
    }
    std::cout << std::endl;

    // Loop through each channel name
    for (size_t i = 0; i < chn.size(); ++i) {
        std::map<std::string, Channel>::iterator it = channels.find(chn[i]);
        bool channelCreated = (it == channels.end());

        // If channel doesn't exist, create it
        if (channelCreated) {
            setupChannel(chn[i], fd, (param.size() == 2 && key.size() > i) ? key[i] : "");
            JoinMessage(fd, chn[i]);
            continue;
        }

        Channel &channel = it->second;

        // Check if client is already in the channel
        if (channel.isClientInChannel(fd)) {
            std::string message = ":" + getNameId(fd) + " 443 * :You are already in this channel\n";
            sendMessage(fd, message);
            continue;
        }

        // Handle invite-only or password-protected channels
        if (channel.getChannelType() == 1) {
            if (channel.getInvitedList().empty() && param.size() != 2) {
                std::string message = ":" + getNameId(fd) + " 473 * Cannot join channel (+i)\n";
                sendMessage(fd, message);
                continue;
            }

            if (channel.isClientInvited(fd)) {
                channel.setClients(fd, getClientNickname(fd));
                JoinMessage(fd, chn[i]);
                continue;
            }

            if (channel.getHasPassword() == 1) {
                if ((key.size() > i && channel.getChPassword() == key[i]) || key.empty()) {
                    channel.setClients(fd, getClientNickname(fd));
                    JoinMessage(fd, chn[i]);
                } else {
                    std::string message = ":" + getNameId(fd) + " 475 * Cannot join channel (+k)\n";
                    sendMessage(fd, message);
                }
                continue;
            }
        }

        // Handle password for existing channels
        if (channel.getHasPassword() == 1) {
            if (key.size() > i && channel.getChPassword() == key[i]) {
                channel.setClients(fd, getClientNickname(fd));
                JoinMessage(fd, chn[i]);
            } else {
                std::string message = ":" + getNameId(fd) + " 475 * Cannot join channel (+k)\n";
                sendMessage(fd, message);
            }
            continue;
        }

        // Add the client to the channel and send a join message
        channel.setClients(fd, getClientNickname(fd));
        JoinMessage(fd, chn[i]);
    }
}

std::string Server::getClientNickname(int fd) {
    std::vector<client>& clients = client::get_clients();
    std::vector<client>::iterator it = std::find_if(clients.begin(), clients.end(), IsClientWithFd(fd));
    return (it != clients.end()) ? it->getNickname() : "Unknown";
}

std::string Server::getClientMessage(int fd) {
    std::vector<client>& clients = client::get_clients();
    std::vector<client>::iterator it = std::find_if(clients.begin(), clients.end(), IsClientWithFd(fd));
    return (it != clients.end()) ? it->get_message() : "";
}

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

    if (command.compare("join") == 0) {
        joinCmd(res, fd);
    } else {
        std::string msg = "421 " + getClientNickname(fd) + " " + command + " :Unknown command\n";
        sendMessage(fd, msg);
    }
}
