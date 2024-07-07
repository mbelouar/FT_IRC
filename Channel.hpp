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

        // setters : 
        void setName(std::string channelName);
        void setTopic(std::string channelTopic);
        void setClients(std::map<int, client> channelClients);
        void setOperators(std::vector<int> channelOperators);
        void setChPassword(std::string channelPassword);
        void setChannelType(int channelType);
        void setHasPassword(int hasPassword);

        // getters :
        std::string getName();
        std::string getTopic();
        std::map<int, client> getClients();
        std::vector<int> getOperators();
        std::string getChPassword();
        int getChannelType();
        int getHasPassword();

}




#endif