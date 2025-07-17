#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/messages.hpp"
#include <cstring>

void Server::handlePing(std::map<int, Client*>* clients, int fd, std::string buffer, int pollIndex) {
    struct pollfd (&fds)[1024] = *getMyFds();
    std::map<int, Client*>::iterator it = clients->find(fd);
    if (it != clients->end()) {
        if (buffer.substr(0, 4) == "PING") {
            std::string response = "PONG";
            if (buffer.length() > 4) {
                response += buffer.substr(4); // inclui o espaço e o argumento, se houver
            }
            this->sendBuffer[pollIndex].clear();
            this->sendBuffer[pollIndex] += response + "\r\n";
            fds[pollIndex].events |= POLLOUT;
        }
    }
}
