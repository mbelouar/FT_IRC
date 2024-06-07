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
    std::cout << "client with nickname " << this->nickname << " is destroyed" << std::endl;
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

void client::set_authenticated(bool is_authenticated)
{
    this->is_authenticated = is_authenticated;
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



client::client(pollfd client_pfd)
{
    std::cout << "new client has been created and his fd is : " << client_pfd.fd << std::endl;
    this->client_pfd = client_pfd;
    this->is_authenticated = false;
    this->is_registered = false;
}



void client::print_client()
{
    std::cout << "client with nickname : " << this->nickname << std::endl;
    std::cout << " and username : " << this->username << std::endl;
    std::cout << " and realname : " << this->realname << std::endl;
    std::cout << " and is_authenticated : " << this->is_authenticated << std::endl;
    std::cout << " and is_registered : " << this->is_registered << std::endl;
}
