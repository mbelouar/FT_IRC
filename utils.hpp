#ifndef UTILS_HPP
#define UTILS_HPP

#include <sys/socket.h>
#include <string>
#include <vector>

void sendMessage(int fd, const std::string& message);
std::vector<std::string> split(std::string line, char c);

#endif // UTILS_HPP