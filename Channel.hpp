#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "client.hpp"
class Channel {
    private:
        std::string           name;
        std::string           topic;
        std::map<int, client> clients;
        std::vector<int>      operators;
        std::string           chPassword;
        int                   channelType; 
        int                   hasPassword;

    public:
        Channel();
        Channel(std::string channelName, int channelOperators , int channelType);
        Channel(const Channel &copy);
        Channel &operator=(const Channel &copy);
        ~Channel();
}




#endif