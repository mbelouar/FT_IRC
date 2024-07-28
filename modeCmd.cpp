#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::modeCmd(std::vector<std::string> &param, int fd) {
    if (param.size() < 2) {
        std::string message = ":" + getClientHostName(fd) + " 461 " + getClientNickname(fd) + " MODE :Not enough parameters\n";
        sendMessage(fd, message);
        return;
    }

    const std::string& channelName = param[0];
    std::map<std::string, Channel>::iterator it = channels.find(channelName);

    if (it == channels.end()) {
        std::string message = ":" + getClientHostName(fd) + " 403 " + getClientNickname(fd) + " " + channelName + " :No such channel\n";
        sendMessage(fd, message);
        return;
    }

    Channel& channel = it->second;
    if (!channel.isClientInChannel(fd)) {
        std::string message = ":" + getClientHostName(fd) + " 442 " + getClientNickname(fd) + " " + channelName + " :You're not on that channel\n";
        sendMessage(fd, message);
        return;
    }

    // Check if the client is an operator
    const std::vector<int>& operators = channel.getOperators();
    bool isOperator = false;
    for (std::vector<int>::const_iterator itOp = operators.begin(); itOp != operators.end(); ++itOp) {
        if (*itOp == fd) {
            isOperator = true;
            break;
        }
    }
    // // print operators
    // std::vector<int> ops = channel.getOperators();
    // for (size_t i = 0; i < ops.size(); i++) {
    //     // print operator's nickname
    //     std::cout << "Operator: " << getClientNickname(ops[i]) << std::endl;
    // }
    // :luna.AfterNET.Org 482 simo #chan :You're not channel operat
    if (!isOperator) {
        std::string message = ":" + getClientHostName(fd) + " 482 " + getClientNickname(fd) + " " + channelName + " :You're not channel operator\r\n";
        sendMessage(fd, message);
        return;
    }

    //print param
    for (size_t i = 0; i < param.size(); i++) {
        std::cout << "param: " << param[i] << std::endl;
    }

    // Process mode changes
    std::string modeChanges = param[1];
    std::vector<std::string> targets;
    if (param.size() > 2) {
        targets = split(param[2], ','); // Split all clients to be affected by the mode change
    }

    // the MODE command is started here
    bool addingMode = true;
    for (std::string::size_type i = 0; i < modeChanges.size(); ++i) {
        char mode = modeChanges[i];
        if (mode == '+') {
            addingMode = true;
            continue;
        } else if (mode == '-') {
            addingMode = false;
            continue;
        }
        // std::cout << "mode: " << mode << std::endl;
        switch (mode) {
            case 'i': {
                if (addingMode) {
                    channel.setChannelType(channel.getChannelType() | 2); // Set to invite-only
                    std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " MODE " + channelName + " +i\r\n";
                    sendMessage(fd, message);

                    // notify other clients
                    std::map<int, client>::const_iterator clientIt;
                    const std::map<int, client>& clients = channel.getClientsFromChannel();
                    for (clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
                        if (clientIt->first != fd) {
                            std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " MODE " + channelName + " +i\r\n";
                            sendMessage(clientIt->first, message);
                        }
                    }
                } else {
                    channel.setChannelType(channel.getChannelType() & ~2); // Remove invite-only
                    std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " MODE " + channelName + " -i\r\n";
                    sendMessage(fd, message);

                    // notify other clients
                    std::map<int, client>::const_iterator clientIt;
                    const std::map<int, client>& clients = channel.getClientsFromChannel();
                    for (clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
                        if (clientIt->first != fd) {
                            std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " MODE " + channelName + " -i\r\n";
                            sendMessage(clientIt->first, message);
                        }
                    }
                }
                break;
            }
            case 't': {
                if (addingMode) {
                    channel.setChannelType(channel.getChannelType() | 4); // Set topic restriction
                    std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " MODE " + channelName + " +t\r\n";
                    sendMessage(fd, message);

                    // notify other clients
                    std::map<int, client>::const_iterator clientIt;
                    const std::map<int, client>& clients = channel.getClientsFromChannel();
                    for (clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
                        if (clientIt->first != fd) {
                            std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " MODE " + channelName + " +t\r\n";
                            sendMessage(clientIt->first, message);
                        }
                    }
                } else {
                    channel.setChannelType(channel.getChannelType() & ~4); // Remove topic restriction
                    std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " MODE " + channelName + " -t\r\n";
                    sendMessage(fd, message);

                    // notify other clients
                    std::map<int, client>::const_iterator clientIt;
                    const std::map<int, client>& clients = channel.getClientsFromChannel();
                    for (clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
                        if (clientIt->first != fd) {
                            std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " MODE " + channelName + " -t\r\n";
                            sendMessage(clientIt->first, message);
                        }
                    }
                }
                break;
            }
            case 'k': {
                if (addingMode && !targets.empty()) {
                    channel.setChPassword(targets[0]);
                    channel.setHasPassword(1);
                    sendMessage(fd, ":" + getClientNickname(fd) + " MODE " + channelName + " +k " + targets[0] + "\n");
                } else {
                    channel.setChPassword("");
                    channel.setHasPassword(0);
                    sendMessage(fd, ":" + getClientNickname(fd) + " MODE " + channelName + " -k\n");
                }
                break;
            }
            case 'o': {
                for (std::vector<std::string>::size_type j = 0; j < targets.size(); ++j) {
                    const std::string &target = targets[j];
                    int clientId = channel.getClientID(target);
                    if (clientId != -1) {
                        if (addingMode) {
                            channel.addOperator(clientId);
                            std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " MODE " + channelName + " +o " + target + "\r\n";
                            sendMessage(fd, message);

                            // notify other clients
                            std::map<int, client>::const_iterator clientIt;
                            const std::map<int, client>& clients = channel.getClientsFromChannel();
                            for (clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
                                if (clientIt->first != fd) {
                                    std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " MODE " + channelName + " +o " + target + "\r\n";
                                    sendMessage(clientIt->first, message);
                                }
                            }
                        } else {
                            channel.removeOperator(clientId);
                            std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " MODE " + channelName + " -o " + target + "\r\n";
                            sendMessage(fd, message);

                            // notify other clients
                            std::map<int, client>::const_iterator clientIt;
                            const std::map<int, client>& clients = channel.getClientsFromChannel();
                            for (clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
                                if (clientIt->first != fd) {
                                    std::string message = ":" + getClientNickname(fd) + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " MODE " + channelName + " -o " + target + "\r\n";
                                    sendMessage(clientIt->first, message);
                                }
                            }
                        }
                    }
                }
                // std::cout << "Operator mode changed for client " << targets[0] << std::endl;
                // //print operators
                // std::vector<int> ops = channel.getOperators();
                // for (size_t i = 0; i < ops.size(); i++) {
                //     // print operator's nickname
                //     std::cout << ">> Operator: " << getClientNickname(ops[i]) << std::endl;
                // }
                break;
            }
            default: {
                std::string message = "472 " + getClientNickname(fd) + " " + channelName + " " + mode + " :is unknown mode char to me\n";
                sendMessage(fd, message);
                break;
            }
        }
    }
}
