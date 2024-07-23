#include "utils.hpp"

void sendMessage(int fd, const std::string& message) {
    if (send(fd, message.c_str(), message.size(), 0) == -1) {
        perror("send");
    }
}

std::vector<std::string> split(std::string line, char c) {
    std::vector<std::string> vec;
    size_t pos;
    while ((pos = line.find(c)) != std::string::npos) {
        vec.push_back(line.substr(0, pos));
        line.erase(0, pos + 1);
    }
    vec.push_back(line);
    return vec;
}