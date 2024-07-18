#ifndef CLIENT_hpp
#define CLIENT_hpp

#include <iostream>
#include <poll.h>
#include <vector>
#include <sstream>
class client
{
    private :

        pollfd client_pfd;
        std::string nickname;
        std::string username;
        std::string realname;
        std::string password;
        std::string sabmit_password;

        bool is_authenticated;
        bool is_registered;
        std::string massage;
        int index;

    public :
        client();
        client(pollfd client_pfd, std::string password);
        ~client();
        void set_nickname(std::string nickname);
        void set_username(std::string username);
        void set_realname(std::string realname);
        bool set_authenticated();
        void set_registered(bool is_registered);
        std::string getNickname() const;
        std::string getUsername() const;
        pollfd get_client_pfd();
        void set_client_pfd(pollfd client_pfd);
        void set_massage_for_auth(std::string massage);
        void set_massage(std::string massage);
        void print_massage();
        void print_client();
        std::string get_message();
        void clear_massage();


};

std::vector<std::string> buffer_to_line(std::string buffer);

#endif