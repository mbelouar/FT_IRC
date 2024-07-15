#include "Channel.hpp"

// canonical form : 


Channel::Channel() {
    name = "";
    topic = "";
    chPassword = "";
    channelType = 0;
    hasPassword = 0;
}
Channel::Channel(std::string channelName, int channelOperators , int channelType) {
    name = channelName;
    topic = "";
    chPassword = "";
    channelType = channelType;
    hasPassword = 0;
}

Channel::Channel(const Channel &copy) {
    name = copy.name;
    topic = copy.topic;
    clients = copy.clients;
    operators = copy.operators;
    chPassword = copy.chPassword;
    channelType = copy.channelType;
    hasPassword = copy.hasPassword;
}

Channel &Channel::operator=(const Channel &copy) {
    if (this == &copy) {
    name = copy.name;
    topic = copy.topic;
    clients = copy.clients;
    operators = copy.operators;
    chPassword = copy.chPassword;
    channelType = copy.channelType;
    hasPassword = copy.hasPassword;
    }
    return *this;
}

Channel::~Channel() {
}

void Channel::setName(std::string channelName) {
    name = channelName;
}

// setters and getters :

void Channel::setTopic(std::string channelTopic) {
    topic = channelTopic;
}

void Channel::setClients(int id, const std::string &userName) {
    clients[id].set_nickname(userName);
}

void Channel::setOperators(int operator) {
    operators.push_back(operator);
}

void Channel::setChPassword(std::string channelPassword) {
    chPassword = channelPassword;
}

void Channel::setChannelType(int channelType) {
    channelType = channelType;
}

void Channel::setHasPassword(int hasPassword) {
    hasPassword = hasPassword;
}

void Channel::setTopic(std::string channelTopic) {
    topic = channelTopic;
}

void Channel::setInvitedList(int id, client &c){
    inviteList[id] = c;
}
std::string Channel::getName() {
    return name;
}

std::string Channel::getTopic() {
    return topic;
}



// std::map<int, client> Channel::getClients() {
//     return clients;
// }

std::vector<int> &Channel::getOperators() {
    return operators;
}

std::string Channel::getChPassword() {
    return chPassword;
}

int Channel::getChannelType() {
    return channelType;
}

// int Channel::getHasPassword() {
//     return hasPassword;
// }

int Channel::getHasPassword() const{
    return hasPassword;
}


std::map<int, client> Channel::getInvitedList() const{
    return inviteList;

}

std::map<int, client>::const_iterator Channel::beginClientIter() const {
    return clients.begin();
}

std::map<int, client>::const_iterator Channel::endClientIter() const {
    return clients.end();
}

const std::map<int, client> &Channel::getClientsFromChannel() const{
    return clients;

}

int Channel::getClientID(const std::string &nickname) const {
    for (const auto &client : clients) {
        if (client.second.getNickname() == nickname) {
            return client.first;
        }
    }
    return -1;
}

int Channel::isClientInChannel(int fd) {
    std::map<int, client>::iterator it;
    it = clients.find(fd);
    if (it != clients.end()) {
        return 1;
    }
    return 0;
}

size_t Channel::getClientNb() const {
    return clients.size();
}

int Channel::getHasPassword() const {
    return hasPassword;
}

void Channel::removeClient(int id) {
    clients.erase(id);
}

void Channel::removeInitedClient(int id) {
    inviteList.erase(id);
}

void Channel::removeOperator(int id) {
    auto it = std::find(operators.begin(), operators.end(), id);
    if (it != operators.end()) {
        operators.erase(it);
    }
}


bool Channel::isClientInvited(int clientId) const{
    bool isInvited = (inviteList.count(clientId) > 0);
    return isInvited;
}

