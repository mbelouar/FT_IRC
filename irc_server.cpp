#include "irc_server.hpp"
#include "server.hpp"
#include "client.hpp"


int main()
{

Server irc_server("localhost", "8080");
irc_server.bind_socket();
irc_server.listen_socket();



std::vector<pollfd> fds;

fds = irc_server.get_fds();

std::cout << "server is running :: fds : " << fds.front().fd << std::endl;

while (1)
{
    int r = poll(fds.data(), fds.size(), -1);
    if (r == -1)
    {
        std::cout << "error in poll function" << std::endl;  
    }
    unsigned int i = 0;
    while ( i < fds.size())
    {
        
        if (fds[i].revents & POLLIN)
        {
            if (fds[i].fd == irc_server.get_sockfd())
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
                    std::cout << "new client connected " << inet_ntoa(clientaddr.sin_addr) << std::endl;
                    pollfd client_pfd;
                    client_pfd.fd = clientfd;
                    client_pfd.events = POLLIN;
                    fds.push_back(client_pfd);
                }
            }
            else
            {
                char buffer[1024];
                int r = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (r > 0)
                {
                    buffer[r] = '\0';
                    std::cout << buffer;
                    // send(fds[i].fd, "your message has been received\n", strlen("your message has been received\n"), 0);
                    // send(fds[i].fd, buffer, r, 0);
                    unsigned int j = 0;
                    while ( j < fds.size())
                    {
                        if(fds[j].fd != irc_server.get_sockfd())
                        {
                            std::cout << "this massage is send from client : " << fds[i].fd << " to client : " << fds[j].fd << std::endl;
                            send(fds[j].fd, buffer, r, 0);
                        }
                        j++;
                    }
                    
                }
            }
        }
        i++;
    }
}

return (0);
}
