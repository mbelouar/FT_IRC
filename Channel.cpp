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

void Channel::setClients(std::map<int, client> channelClients) {
    clients = channelClients;
}

void Channel::setOperators(std::vector<int> channelOperators) {
    operators = channelOperators;
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

std::string Channel::getName() {
    return name;
}

std::string Channel::getTopic() {
    return topic;
}

std::map<int, client> Channel::getClients() {
    return clients;
}

std::vector<int> Channel::getOperators() {
    return operators;
}

std::string Channel::getChPassword() {
    return chPassword;
}

int Channel::getChannelType() {
    return channelType;
}

int Channel::getHasPassword() {
    return hasPassword;
}

