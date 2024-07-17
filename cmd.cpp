#include "server.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "client.hpp"
#include "Channel.hpp"
#include <ctime>


std::vector<std::string> ft_split(std::string str, char sep)
{
    std::vector<std::string> res;
    std::string word;
    std::stringstream stream(str);

    for(; std::getline(stream, word, sep); ){
        if (!word.empty())
            res.push_back(word);
    }
    return (res);
}

void	ft_send(int fd, std::string message)
{
	send(fd, message.c_str(), message.size(), 0);

}

std::string get_adderss()
{
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    struct hostent *host = gethostbyname(hostname);
    if (host)
        return std::string(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    return "localhost";
}

void ft_replace(std::string &str, char c, std::string rep)
{
    size_t pos = str.find(c);
	while(pos != std::string::npos)
	{
		str.replace(pos, 1, rep);
		pos = str.find('\t');
	}
}

std::string to_upper(std::string str)
{
    for(int i = 0; str[i]; i++)
        str[i] = std::toupper(str[i]);

    return str;
}

std::vector<std::string> modifier(std::string str)
{
    std::vector<std::string> split;
    ft_replace(str, '\t', " ");
    ft_replace(str, '\r', " ");
    ft_replace(str, '\n', " ");
    split = ft_split(str, ' ');

    return split;
}

void    Server::parse_request(char *read, int fd, fd_set *master)
{
    // std::cout << read << std::endl;
    client client(fd);
    std::string request(read);
    std::string response;
    std::vector<std::string> split;

    if(!this->_map.count(fd))
        this->_map.insert(std::make_pair(fd, client));
    split = modifier(request);
    if(split.empty())
        response = "";
    else if(to_upper(split[0]) == "JOIN" )
        response = join_response(split, this->_map[fd]);
    else
        response = "";
    send(fd, response.c_str(), response.size(), 0);
}



std::string make_time(void)
{
	time_t curr_time;
	curr_time = time(NULL);
	return std::to_string(curr_time);
}


std::string Server::join_response(std::vector<std::string> split, client &client)
{
	Channel mychannel;

	if(!client.get_print())
		return (":localhost 451 * JOIN :You must finish connecting with nickname first.\r\n");
	if(split.size() == 1 || (split.size() == 2 && split[1] == "#"))
		return(":localhost 461 " + client.getNickname() + " " + split[0] + " :Not enough parameters\r\n");
	else if(split.size() >= 2)
	{
		std::vector<std::string> names;
		std::vector<std::string> key;
		names = ft_split(split[1], ',');
		if(split.size() > 2)
			key = ft_split(split[2], ',');
		for(size_t i = 0; i < names.size(); i++)
		{
			if(names[i][0] != '#')
				ft_send(client.get_pollfd().fd, ":localhost 403 " + client.getNickname() + " " + names[i] + " :No such channel\r\n");
			else if(client.check_member(names[i]))
				ft_send(client.get_pollfd().fd, ":localhost 443 " + client.getNickname() + " " + names[i] + ":is already on channel\r\n");
			else
			{
				if(!this->_channels.count(names[i]))
				{//if channel does not exist
					client.set_channel(names[i]);
					if(key.size() > i)
						mychannel = Channel(names[i], key[i]);
					else 
						mychannel = Channel(names[i], "");

					mychannel.add_member(client.get_pollfd().fd);
					mychannel.add_moderator(client.get_pollfd().fd);
					mychannel.set_creation_time(make_time());
					this->_channels.insert(make_pair(names[i], mychannel));

					ft_send(client.get_pollfd().fd, ":" + client.getNickname() + "!" + client.getUsername() + "@" + get_adderss() + " JOIN :" + names[i] + "\r\n");
				}
				else
				{//if channel exists
					if(this->_channels[names[i]].get_limitMode() && this->_channels[names[i]].get_limit() <= (int)this->_channels[names[i]].get_members().size())
						ft_send(client.get_pollfd().fd, ":localhost 471 " + client.getNickname() + " " + names[i] + " :Cannot join channel (+l)\r\n");
					else if(this->_channels[names[i]].get_inviteMode())
					{
						if(this->_channels[names[i]].is_invited(client.getNickname()))
						{
							client.set_channel(names[i]);
							this->_channels[names[i]].add_member(client.get_pollfd().fd);
							this->_channels[names[i]].broadcast_message(":" + client.getNickname() + "!~" + client.getUsername() + "@"+ get_adderss() + " JOIN :" + names[i] + "\r\n", 0);
						}
						else
							ft_send(client.get_pollfd().fd, ":localhost 473 " + client.getNickname() + " " + names[i] + " :Cannot join channel (+i)\r\n");
					}
					else
					{
						if(key.size() > i)
						{//if key is provided
							if(this->_channels[names[i]].get_pass() == key[i])
							{
								client.set_channel(names[i]);
								this->_channels[names[i]].add_member(client.get_pollfd().fd);
								this->_channels[names[i]].broadcast_message(":" + client.getNickname() + "!~" + client.getUsername() + "@"+ get_adderss() + " JOIN :" + names[i] + "\r\n", 0);
							}
							else
								ft_send(client.get_pollfd().fd, ":localhost 475 " + client.getNickname() + " " + names[i] + " :Cannot join channel (+k)\r\n");
						}
						else
						{//if key is not provided
							if(this->_channels[names[i]].get_pass() == "")
							{
								client.set_channel(names[i]);
								this->_channels[names[i]].add_member(client.get_pollfd().fd);
								this->_channels[names[i]].broadcast_message(":" + client.getNickname() + "!~" + client.getUsername() + "@"+ get_adderss() + " JOIN :" + names[i] + "\r\n", 0);
							}
							else
								ft_send(client.get_pollfd().fd, ":localhost 475 " + client.getNickname() + " " + names[i] + " :Cannot join channel (+k)\r\n");
						}
					}
				}
			}
		}
	}
	return ("");
}