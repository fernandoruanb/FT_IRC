#include "../includes/Server.hpp"

void	Server::pass(s_commands& com)
{
	std::map<int, Client*>::iterator it = com.clients->find(com.fd);
	if (it != com.clients->end()) {
		Client* client = it->second;
		if (!client->getAuthenticated()) {
			struct pollfd (&fds)[1024] = *getMyFds();
			if (com.line.empty() || com.line == "\r" || com.line == "\n" || com.line == "\r\n") {
				return ;
			}
			if (com.line.rfind("PASS ", 0) == 0) {
				std::string pass = com.line.substr(5);
				pass.erase(pass.find_last_not_of("\r\n") + 1);
				if (pass == this->getPassword()) {
					client->setAuthenticated(true);
					this->sendBuffer[com.index].clear();
					this->sendBuffer[com.index] += msg_notice("Authentication successful");
					return ;
				} else {	
					this->sendBuffer[com.index].clear();
					this->sendBuffer[com.index] += msg_err_passwdmismatch();
					return ;
				}
			} else {
				this->sendBuffer[com.index].clear();
				this->sendBuffer[com.index] += msg_err_needmoreparams("PASS");
				return ;
			}
		}
	}
}
