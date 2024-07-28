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
        std::string           chPassword;
        std::vector<int>      operators;
        std::map<int, client> clients;
        std::map<int, client> inviteList;
        int                   channelType;
        int                   hasPassword;
        int                   channeLimit;

    public:
        Channel();
        Channel(std::string channelName, int channelType);
        Channel(const Channel &copy);
        Channel &operator=(const Channel &copy);
        ~Channel();

        // setters : 
        void setName(std::string channelName);
        void setTopic(std::string channelTopic);
        // void setClients(std::map<int, client> channelClients);
        void setClients(int id, const std::string &userName);
        void setOperators(int op);
        void setChPassword(std::string channelPassword);
        void setChannelType(int channelType);
        void setHasPassword(int hasPassword);
        void setInvitedList(int id, client &c);
        void setChannelLimit(int limit);

        // getters :
        std::string                             getName();
        std::string                             getTopic();
        std::vector<int>                        &getOperators();
        std::string                             getChPassword();
        std::map<int, client>::const_iterator   beginClientIter() const;
        std::map<int, client>::const_iterator   endClientIter() const;
        int                                     getChannelLimit() const;
        int                                     getChannelType();
        int                                     getClientID(const std::string &nickname) const;
        int                                     isClientInChannel(int fd);
        int                                     getHasPassword() const;
        size_t                                  getClientNb() const;
        std::map<int, client>                   getInvitedList() const;
        bool                                    isClientInvited(int clientId) const;
        const std::map<int, client>             &getClientsFromChannel() const;

        void removeClient(int fd);
        void addOperator(int id);
        void removeOperator(int id);
        bool isOperator(int id);

};




#endif