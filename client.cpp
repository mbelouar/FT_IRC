#include "client.hpp"

client::client(pollfd client_pfd, std::string nickname, std::string username)
{
    this->client_pfd = client_pfd;
    this->nickname = nickname;
    this->username = username;
    this->is_authenticated = false;
    this->is_registered = false;
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

// bool client::set_authenticated()
// {
//     // this->is_authenticated = is_authenticated;
//     if (this->is_registered == true)
//     {
//         return (true);
//     }else
//     {
        
//     }
    

// }

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



client::client(pollfd client_pfd)
{
    this->client_pfd = client_pfd;
    this->is_authenticated = false;
    this->is_registered = false;
}

void client::set_massage(std::string massage)
{
    this->massage = this->massage + massage;
}

void client::print_massage()
{
    std::cout << this->massage << std::endl;
}



