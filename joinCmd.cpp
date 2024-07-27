#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

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

    std::map<std::string, Channel>::iterator it = channels.find(channelName);

    if (channels.find(channelName) != channels.end()) {
        std::string globmsg = "";
        std::string topic = channels[channelName].getTopic();  // Get topic
        std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " JOIN " + channelName + " * :realname\r\n";
        globmsg += message;

        Channel& channel = it->second;
        std::string message2 = ":" + getClientHostName(fd) + " 353 " + getClientNickname(fd) + " = " + channelName + " :";
        bool first = true;
        
        std::map<int, client>::const_iterator clientIt;
        const std::map<int, client>& clients = channel.getClientsFromChannel();
        for (clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
            if (!first) {
                message2 += " ";
            }
            first = false;
            message2 += "@" + clientIt->second.getNickname() + "!" + clientIt->second.getNickname() + "@" + getClientHostName(fd);
        }
        message2 += "\r\n";
        globmsg += message2;    
        
        std::string message3 = ":" + getClientHostName(fd) + " 366 " + getClientNickname(fd) + " " + channelName + " :End of /NAMES list.\r\n";
        globmsg += message3;

        // send to other clients in the channel like this message :simo!simo@88ABE6.25BF1D.D03F86.88C9BD.IP JOIN #chan * :realname
        for (std::map<int, client>::const_iterator it = channel.beginClientIter(); it != channel.endClientIter(); ++it) {
            if (it->first != fd) {
                std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " JOIN " + channelName + " * :realname\r\n";
                sendMessage(it->first, message);
            }
        }
        sendMessage(fd, globmsg);
    } else {
        std::string globmsg = ":" + getNameId(fd) + " 403 " + channelName + " :No such channel\n";
        sendMessage(fd, globmsg);
    }
}

void Server::joinCmd(std::vector<std::string> &param, int fd) {
    if (param.empty() || param[0][0] != '#' || param[0].size() <= 1) {
        std::string message = ":" + getNameId(fd) + " 461 JOIN :Not enough parameters\n";
        sendMessage(fd, message);
        return;
    }

    std::string channelName = param[0];

    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    bool channelCreated = (it == channels.end());

    if (channelCreated) {
        // Create the channel if it doesn't exist
        setupChannel(channelName, fd, (param.size() == 2) ? param[1] : "");

        // Add the client as a moderator by default
        Channel &channel = channels[channelName];
        std::vector<int> operators = channel.getOperators();
        if (std::find(operators.begin(), operators.end(), fd) == operators.end()) {
            operators.push_back(fd);
            channel.setOperators(operators);
        }

        // Debug output to check moderators
        std::cout << "Channel \"" << channelName << "\" created. Debug output:" << std::endl;
        std::cout << "Current moderators for channel " << channelName << ": ";
        std::vector<int>::iterator it = operators.begin();
        while (it != operators.end()) {
            std::cout << *it << " ";
            ++it;
        }
        std::cout << std::endl;

        JoinMessage(fd, channelName);
    } else {
        std::cout << "Channel \"" << channelName << "\" already exists." << std::endl;

        // Get reference to the existing channel
        Channel &channel = channels[channelName];

        // Add the client to the channel
        channel.setClients(fd, getClientNickname(fd));

        // Add the client as a moderator if they are not already
        std::vector<int> operators = channel.getOperators();
        if (std::find(operators.begin(), operators.end(), fd) == operators.end()) {
            operators.push_back(fd);
            channel.setOperators(operators);
        }

        // Debug output to check moderators
        std::cout << "Channel \"" << channelName << "\" joined. Debug output:" << std::endl;
        std::cout << "Current moderators for channel " << channelName << ": ";
        std::vector<int>::iterator it = operators.begin();
        while (it != operators.end()) {
            std::cout << *it << " ";
            ++it;
        }
        std::cout << std::endl;

        // Inform the client about joining the channel
        JoinMessage(fd, channelName);
    }
}

