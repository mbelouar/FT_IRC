#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

int Server::IsModerator(Channel &ch, std::string nickname) {
    std::vector<int> v = ch.getOperators();
    int i = ch.getClientID(nickname);
    if (std::find(v.begin(), v.end(), i) != v.end())
        return 1;
    return 0;
}

void Server::RemoveModerator(Channel &ch, std::string nickname) {
    std::vector<int> v = ch.getOperators();
    int i = ch.getClientID(nickname);
    v.erase(std::remove(v.begin(), v.end(), i), v.end());
    ch.setOperators(v);
}




// Validate the MODE command parameters
bool Server::validateModeCommand(const std::vector<std::string> &vec, int fd) {
    std::string message;

    if (vec.size() < 2) {
        message = ":" + getClientNickname(fd) + " 461 * :Not enough parameters\n";
        sendMessage(fd, message);
        return false;
    }

    std::map<std::string, Channel>::iterator it = channels.find(vec[0]);
    if (it == channels.end()) {
        message = ":" + getClientNickname(fd) + " 403 * :No such channel\n";
        sendMessage(fd, message);
        return false;
    }

    if (vec.size() == 1) {
        message = ":" + getClientNickname(fd) + " 324 * :Not enough parameters\n";
        sendMessage(fd, message);
        return false;
    }

    return true;
}

// Check if the client is a channel operator
bool Server::isOperator(int fd, Channel &channel) {
    std::vector<int> operators = channel.getOperators();
    return (std::find(operators.begin(), operators.end(), fd) != operators.end());
}

// Handle mode changes based on the mode command
void Server::handleModeChange(const std::vector<std::string> &vec, int fd, Channel &channel) {
    std::string message;
    int clientId = channel.getClientID(vec[2]);

    if (vec[1][0] == '+') {
        if (vec[1][1] == 'i') {
            handlePrivateMode(channel, fd);
        } else if (vec[1][1] == 'k') {
            handlePasswordMode(channel, vec, fd, clientId);
        } else if (vec[1][1] == 'o') {
            handleOperatorMode(channel, vec, fd, clientId);
        }
    } else if (vec[1][0] == '-') {
        if (vec[1][1] == 'i') {
            handlePublicMode(channel, fd);
        } else if (vec[1][1] == 'k') {
            handleRemovePasswordMode(channel, fd);
        } else if (vec[1][1] == 'o') {
            handleRemoveOperatorMode(channel, vec, fd, clientId);
        }
    }
}

// Handle the private mode (+i)
void Server::handlePrivateMode(Channel &channel, int fd) {
    std::string message;

    if (channel.getChannelType() == 1) {
        message = ":" + getClientNickname(fd) + " 342 * :Channel is already private\n";
        sendMessage(fd, message);
    } else {
        channel.setChannelType(1);
        channel.setHasPassword(0);
        channel.setChPassword("");
        message = ":" + getClientNickname(fd) + " 342 * :Channel is private now\n";
        sendMessage(fd, message);
    }
}

// Handle the password mode (+k)
void Server::handlePasswordMode(Channel &channel, const std::vector<std::string> &vec, int fd, int clientId) {
    std::string message;

    if (clientId == -1) {
        message = ":" + getClientNickname(fd) + " 401 * :No such nick/channel\n";
        sendMessage(fd, message);
    } else {
        if (vec.size() == 3) {
            channel.setChannelType(1);
            channel.setChPassword(vec[2]);
            message = ":" + getClientNickname(fd) + " 400 * :We have set the password\n";
            sendMessage(fd, message);
        } else {
            message = ":" + getClientNickname(fd) + " 696 * :Invalid mode params\n";
            sendMessage(fd, message);
        }
    }
}

// Handle the operator mode (+o)
void Server::handleOperatorMode(Channel &channel, const std::vector<std::string> &vec, int fd, int clientId) {
    std::string message;

    if (vec.size() == 3) {
        if (clientId != -1) {
            if (IsModerator(channel, vec[2]) == 1) {
                message = ":" + getClientNickname(fd) + " 400 * :" + "The user " + vec[2] + " is already a moderator\n";
                sendMessage(fd, message);
            } else {
                std::vector<int> ops = channel.getOperators();
                ops.push_back(clientId);
                channel.setOperators(ops);
                message = ":" + getClientNickname(fd) + " 400 * :" + "The user " + vec[2] + " is a moderator now\n";
                sendMessage(fd, message);
            }
        } else {
            message = ":" + getClientNickname(fd) + " 400 * :" + "The user " + vec[2] + " is not a member of channel\n";
            sendMessage(fd, message);
        }
    } else {
        message = ":" + getClientNickname(fd) + " 696 * :Invalid mode params\n";
        sendMessage(fd, message);
    }
}

// Handle the public mode (-i)
void Server::handlePublicMode(Channel &channel, int fd) {
    std::string message;

    if (channel.getChannelType() == 0) {
        message = ":" + getClientNickname(fd) + " 342 * :Channel is already public\n";
        sendMessage(fd, message);
    } else {
        channel.setChannelType(0);
        message = ":" + getClientNickname(fd) + " 342 * :Channel is public now\n";
        sendMessage(fd, message);
    }
}

// Handle the remove password mode (-k)
void Server::handleRemovePasswordMode(Channel &channel, int fd) {
    std::string message;

    if (channel.getChannelType() == 1 && !channel.getChPassword().empty()) {
        channel.setChPassword("");
        message = ":" + getClientNickname(fd) + " 400 * :Password removed\n";
        sendMessage(fd, message);
    } else {
        message = ":" + getClientNickname(fd) + " 342 * :Channel is not password protected\n";
        sendMessage(fd, message);
    }
}

// Handle the remove operator mode (-o)
void Server::handleRemoveOperatorMode(Channel &channel, const std::vector<std::string> &vec, int fd, int clientId) {
    std::string message;

    if (vec.size() == 3) {
        if (clientId != -1) {
            std::vector<int> ops = channel.getOperators();
            std::vector<int>::iterator it = std::find(ops.begin(), ops.end(), clientId);
            if (it != ops.end()) {
                ops.erase(it);
                channel.setOperators(ops);
                message = ":" + getClientNickname(fd) + " 400 * :" + "The user " + vec[2] + " is no longer a moderator\n";
                sendMessage(fd, message);
            } else {
                message = ":" + getClientNickname(fd) + " 400 * :" + "The user " + vec[2] + " is not a moderator\n";
                sendMessage(fd, message);
            }
        } else {
            message = ":" + getClientNickname(fd) + " 400 * :" + "The user " + vec[2] + " is not a member of channel\n";
            sendMessage(fd, message);
        }
    } else {
        message = ":" + getClientNickname(fd) + " 696 * :Invalid mode params\n";
        sendMessage(fd, message);
    }
}

// Main function to handle the MODE command
void Server::modeCmd(std::vector<std::string> &vec, int fd) {
    std::map<std::string, Channel>::iterator it;
    std::string message;
    
    std::cout << "Received mode command: " << std::endl;
    std::cout << "Channel: " << vec[0] << ", Mode: " << vec[1] << std::endl;
    if (vec.size() > 2) {
        std::cout << "Parameter: " << vec[2] << std::endl;
    }

    if (!validateModeCommand(vec, fd)) return;
    
    it = channels.find(vec[0]);
    Channel &channel = it->second;

    if (!isOperator(fd, channel)) {
        message = ":" + getClientNickname(fd) + " 482 * :You're not a channel operator\n";
        sendMessage(fd, message);
        return;
    }

    if (vec[1].size() == 2) {
        handleModeChange(vec, fd, channel);
    }
}