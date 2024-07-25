#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::whoCmd(std::vector<std::string>& args, int fd) {
    if (args.size() == 0) {
        // No channel name provided
        std::string msg = "431 " + getClientNickname(fd) + " :No channel given\n";
        sendMessage(fd, msg);
        return;
    }

    const std::string& channelName = args[0];
    std::map<std::string, Channel>::iterator it = channels.find(channelName);

    if (it == channels.end()) {
        // Channel not found
        std::string msg = "403 " + getClientNickname(fd) + " " + channelName + " :No such channel\n";
        sendMessage(fd, msg);
    } else {
        // Channel exists, prepare to list the clients
        Channel& channel = it->second;
        std::string msg;

        // Start with a header message
        msg = ":" + getClientNickname(fd) + " 353 " + getClientNickname(fd) + " = " + channelName + " :";

        // List all clients in the channel
        bool first = true;
        std::map<int, client>::const_iterator clientIt;
        const std::map<int, client>& clients = channel.getClientsFromChannel();
        for (clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
            if (!first) {
                msg += " ";
            }
            first = false;
            msg += clientIt->second.getNickname(); // Adjust if you need more details
        }

        msg += "\n";

        // End of list message
        msg += ":" + getClientNickname(fd) + " 366 " + getClientNickname(fd) + " " + channelName + " :End of /WHO list.\n";

        // Send the response to the requesting client
        sendMessage(fd, msg);
    }
}
