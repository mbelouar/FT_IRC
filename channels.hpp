/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aet-tass <aet-tass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 16:18:46 by aet-tass          #+#    #+#             */
/*   Updated: 2024/07/02 16:20:12 by aet-tass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

public:
    Channel(std::string name);
    void join(std::string nickname);
    void leave(std::string nickname);
    void kick(std::string operatorNickname, std::string targetNickname);
    void invite(std::string operatorNickname, std::string targetNickname);
    void setTopic(std::string operatorNickname, std::string newTopic);
    void setMode(std::string operatorNickname, char mode, bool value);
};

#endif
