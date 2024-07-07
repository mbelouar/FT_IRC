#include "Channel.hpp"

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