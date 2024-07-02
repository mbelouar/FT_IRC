
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>

class Channel {
private:
    std::string name;
    std::vector<std::string> members;
    std::vector<std::string> operators;
    std::string topic;
    bool inviteOnly;
    bool topicRestricted;
    std::string password;
};

#endif
