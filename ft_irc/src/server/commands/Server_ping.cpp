#include "../includes/Server.hpp"
<<<<<<< HEAD
#include "../includes/Client.hpp"
#include "../includes/messages.hpp"
#include <cstring>

void Server::handlePing(s_commands &commands) {
    struct pollfd (&fds)[1024] = *getMyFds();
    std::map<int, Client*>::iterator it = commands.clients->find(commands.fd);
    if (it != commands.clients->end()) {
        if (commands.line.substr(0, 4) == "PING") {
            std::string response = "PONG";
            if (commands.line.length() > 4) {
                response += commands.line.substr(4); // inclui o espaço e o argumento, se houver
            }
            this->sendBuffer[commands.index].clear();
            this->sendBuffer[commands.index] += response + "\r\n"; 
            fds[commands.index].events |= POLLOUT;
        }
    }
}
