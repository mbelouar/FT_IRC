#include "irc_server.hpp"
#include "server.hpp"
#include "client.hpp"
#include <algorithm> // for std::transform

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: ./irc_server <port> <password>" << std::endl;
        return (1);
    }

    Server irc_server("0.0.0.0", argv[1]);
    irc_server.setsockopt();
    irc_server.bind_socket();
    irc_server.listen_socket();

    client::get_fds() = irc_server.get_fds();

    while (1)
    {
        int r = poll(&client::get_fds()[0], client::get_fds().size(), -1);
        if (r == -1)
        {
            std::cout << "error in poll function" << std::endl;
        }
        unsigned int i = 0;
        while (i < client::get_fds().size())
        {
            if (client::get_fds()[i].revents & POLLIN)
            {
                if (client::get_fds()[i].fd == irc_server.get_sockfd())
                {
                    // New client
                    sockaddr_in clientaddr;
                    socklen_t client_len = sizeof(clientaddr);
                    int clientfd = accept(irc_server.get_sockfd(), (struct sockaddr*)&clientaddr, &client_len);

                    if (clientfd == -1)
                    {
                        std::cout << "error in accept function" << std::endl;
                    }
                    else
                    {
                        // std::cout << "new client connected " << inet_ntoa(clientaddr.sin_addr) << std::endl;
                        pollfd client_pfd;

                        client_pfd.fd = clientfd;
                        client_pfd.events = POLLIN;
                        client::get_fds().push_back(client_pfd);

                        client::get_clients().push_back(client(client_pfd, argv[2]));
                    }
                }
                else
                {
                    char buffer[1024];
                    int r = recv(client::get_fds()[i].fd, buffer, sizeof(buffer), 0);
                    if (r > 0)
                    {
                        buffer[r] = '\0';

                        // std::cout << buffer;
                        unsigned int l = 0;
                        while (l < client::get_clients().size())
                        {
                            if (client::get_clients()[l].get_client_pfd().fd == client::get_fds()[i].fd)
                            {
                                if (client::get_clients()[l].set_authenticated() == true)
                                {
                                    client::get_clients()[l].set_massage(buffer);
                                    // work will be done here 
                                    irc_server.command(client::get_fds()[i].fd);

                                    std::cout << "we are receiving message from authenticated client: " << client::get_clients()[l].get_message() << std::endl;
                                }
                                else
                                {
                                    client::get_clients()[l].set_massage_for_auth(buffer);
                                    // std::cout << client::get_clients()[l].get_client_pfd().fd << " is not authenticated" << std::endl;
                                    if (client::get_clients()[l].set_authenticated() == true)
                                    {
                                        std::cout << "now is authenticated" << std::endl;
                                        client::get_clients()[l].clear_massage();
                                        std::cout << client::get_clients()[l].get_message() << std::endl;
                                    }
                                }
                            }
                            l++;
                        }
                    }
                }
            }
            i++;
        }
    }
    return (0);
}
