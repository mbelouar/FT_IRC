#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "utils.hpp"

void Server::nickCmd(std::vector<std::string>& args, int fd) {
    if (args.size() == 0) {
        std::string msg = "431 " + getClientNickname(fd) + " :No nickname given\n\r";
        sendMessage(fd, msg);
        return;
    }

    const std::string& newNickname = args[0];

    std::cout << "New nickname: " << newNickname << std::endl;

    // Check if the nickname is already in use
    const std::vector<client>& clients = client::get_clients();
    for (std::vector<client>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->getNickname() == newNickname) {
            std::string msg = "433 " + newNickname + " :Nickname is already in use\n\r";
            sendMessage(fd, msg);
            return;
        }
    }

    // print the name of the client before changing it
    std::string oldNickname = getClientNickname(fd);
    std::cout << "Fd: " << fd << " old nickname: " << oldNickname << std::endl;

    // Modify the client's nickname
    client::modify_client(fd, newNickname);

    // print the name of the client after changing it
    std::cout << "Fd: " << fd << " new nickname: " << getClientNickname(fd) << std::endl;

    // Notify other clients about the nickname change
    std::string notice = ":" + oldNickname + "!" + oldNickname + "@" + getClientHostName(fd) + " NICK :" + newNickname + "\n\r";
    for (std::vector<client>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->getFd() != fd) {
            sendMessage(it->getFd(), notice);
        }
    }

    // // Confirm the change to the client
    // :bello!bik@88ABE6.25BF1D.D03F86.88C9BD.IP NICK :bik
    std::string msg = ":" + oldNickname + "!" + getClientNickname(fd) + "@" + getClientHostName(fd) + " NICK :" + getClientNickname(fd) + "\n\r";
    sendMessage(fd, msg);
}
