#include "client.hpp"

client::client(pollfd client_pfd, std::string password)
{
    this->client_pfd = client_pfd;
    this->is_authenticated = false;
    this->is_registered = false;
    this->password = password;
}


client::~client()
{
//     std::cout << "client with nickname " << this->nickname << " is destroyed" << std::endl;

}

void client::set_nickname(std::string nickname)
{
    this->nickname = nickname;
}

void client::set_username(std::string username)
{
    this->username = username;
}

void client::set_realname(std::string realname)
{
    this->realname = realname;
}

bool client::set_authenticated()
{
    std::string line;
    if (this->is_registered == true)
    {
        return (true);
    }else
    {
        if (this->massage.find("NICK") != std::string::npos)
        {
            line = this->massage.substr(this->massage.find("NICK"), this->massage.find("\n"));
            this->nickname = line.substr(5, line.find("\n"));
        }
        if (this->massage.find("USER") != std::string::npos)
        {
            line = this->massage.substr(this->massage.find("USER"), this->massage.find("\n"));
            this->username = line.substr(5, line.find("\n"));
        }
        if (this->massage.find("REALNAME") != std::string::npos)
        {
            line = this->massage.substr(this->massage.find("REALNAME"), this->massage.find("\n"));
            this->realname = line.substr(9, line.find("\n"));
        }
        if (this->nickname != "" && this->username != "" && this->realname != "")
        {
            this->is_registered = true;
            return (true);
        }else
        {
            return (false);
        }

    }

     return (false);      
}

void client::set_registered(bool is_registered)
{
    this->is_registered = is_registered;
}

pollfd client::get_client_pfd()
{
    return this->client_pfd;
}

void client::set_client_pfd(pollfd client_pfd)
{
    this->client_pfd = client_pfd;
}





void client::set_massage(std::string massage)
{
    this->massage = this->massage + massage;
}

void client::print_massage()
{
    std::cout << this->massage << std::endl;
}



