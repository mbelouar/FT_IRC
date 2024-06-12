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
std::vector<std::string> buffer_to_line(std::string buffer, std::string siparator)
{
    std::vector<std::string> lines;
    std::string line;
    std::istringstream stm(buffer);
    while (std::getline(stm, line))
    {
        size_t found = line.find_first_of(siparator);

        if (found != std::string::npos)
        {
            line = line.substr(0, found);
        }
        lines.push_back(line);
    }
    return lines;
}





bool client::set_authenticated()
{
    std::string line;
    if (this->is_authenticated == true)
    {
        std::cout << this->nickname << " is authenticated" << std::endl;
        return (true);
    }else
    {
        std::vector<std::string> lines = buffer_to_line(this->massage, "\r\n");
        std::cout << this->massage << std::endl;
        this->nickname = "dddddd";

        std::cout << "---------------vvv--------------------" << std::endl;
        std::cout << "the size of lines : " << lines.size() << std::endl;
        for (unsigned int i = 0; i < lines.size(); i++)
        {
           std::cout << lines[i] << std::endl;
          
        }

        std::cout << "---------------vvv--------------------" << std::endl;








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

void client::print_client()
{
    std::cout << "nickname : " << this->nickname << std::endl;
    std::cout << "username : " << this->username << std::endl;
    std::cout << "realname : " << this->realname << std::endl;
    std::cout << "password sabmited : " << this->sabmit_password << std::endl;
}



