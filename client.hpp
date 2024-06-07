#ifndef CLIENT_hpp
#define CLIENT_hpp

#include <iostream>
#include <poll.h>

class client
{
    private :
        pollfd client_pfd;
        std::string nickname;
        std::string username;
        std::string realname;

        bool is_authenticated;
        bool is_registered;
    public :
        client(pollfd client_pfd);
        client(pollfd client_pfd, std::string nickname, std::string username);
        ~client();
        void set_nickname(std::string nickname);
        void set_username(std::string username);
        void set_realname(std::string realname);
        void set_authenticated(bool is_authenticated);
        void set_registered(bool is_registered);
        pollfd get_client_pfd();
        void set_client_pfd(pollfd client_pfd);




        void print_client();
        
};



#endif

