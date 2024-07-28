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
            // :luna.AfterNET.Org 353 simo = #chan :simo!dd@88ABE6.25BF1D.D03F86.88C9BD.IP @bello!dd@88ABE6.25BF1D.D03F86.88C9BD.I
            // if the client is an operator
            if (channel.isOperator(clientIt->first)) {
                message2 += "@" + clientIt->second.getNickname() + "!" + clientIt->second.getNickname() + "@" + getClientHostName(clientIt->first);
            } else {
                message2 += clientIt->second.getNickname() + "!" + clientIt->second.getNickname() + "@" + getClientHostName(clientIt->first);
            }
        }
        message2 += "\n";
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
    std::string password = (param.size() > 1) ? param[1] : "";

    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    bool channelCreated = (it == channels.end());

    if (channelCreated) {
        // Create the channel if it doesn't exist
        setupChannel(channelName, fd, (param.size() == 2) ? param[1] : "");
        // add the client to the operator list
        channels[channelName].addOperator(fd);
        JoinMessage(fd, channelName);
    }
    else {
        std::cout << "Channel \"" << channelName << "\" already exists \""<< std::endl;
        // std::cout << "client name is : " << getClientNickname(fd) << std::endl;
        // std::cout << "channel name is : " << channelName << std::endl;

        // Get reference to the existing channel
        Channel &channel = channels[channelName];

        // Check if the channel is invite-only and if the client is invited
        if (channel.getChannelType() & 2) { // Check for invite-only flag
            if (!channel.isClientInvited(fd)) {
                std::string message = ":" + getNameId(fd) + " 473 " + channelName + " :Cannot join channel (+i)\n";
                sendMessage(fd, message);
                return;
            }
        }

        // Check if the channel has a password and if the provided password is correct
        if (channel.getChPassword() != password) {
            std::string message = ":" + getNameId(fd) + " 475 " + channelName + " :Cannot join channel (+k) - incorrect key\n";
            sendMessage(fd, message);
            return;
        }

        // Check if the channel has a user limit and if the limit is reached
        if (channel.getChannelLimit() > 0 && channel.getClientsFromChannel().size() >= static_cast<size_t>(channel.getChannelLimit())) {
            std::string message = ":" + getNameId(fd) + " 471 " + channelName + " :Cannot join channel (+l) - channel is full\n";
            sendMessage(fd, message);
            return;
        }

        // Add the client to the channel
        channel.setClients(fd, getClientNickname(fd));

        // Inform the client about joining the channel
        JoinMessage(fd, channelName);
    }
}
