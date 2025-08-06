#include "../includes/Server.hpp"

static void whoUser(s_commands& com, const std::string& targetNick) {
	Client* targetClient = NULL;
	
	// debug
	std::cout << RED "WHO command for user: " RESET << targetNick << std::endl;

	// Encontrar o cliente pelo nick
	std::map<int, Client*>::iterator cit;
	for (cit = com.clients->begin(); cit != com.clients->end(); ++cit) {
		if (cit->second && cit->second->getNickName() == targetNick) {
			targetClient = cit->second;
			break;
		}
	}
	
	if (!targetClient) {
		com.sendBuffer += msg_err_nosuchnick(com.client->getNickName(), targetNick);
		return;
	}
	
	// Resposta 352 (RPL_WHOREPLY)
	std::string flags = "H";
	
	com.sendBuffer += ":" SERVER_NAME " 352 " + com.client->getNickName() 
					+ " * " + targetClient->getUserName() 
					+ " " + targetClient->getHost() + " " SERVER_NAME " " 
					+ targetClient->getNickName() + " " + flags + " :0 " 
					+ targetClient->getRealName() + "\r\n";
	
	// Resposta 315 (RPL_ENDOFWHO)
	com.sendBuffer += ":" SERVER_NAME " 315 " + com.client->getNickName() 
					+ " " + targetNick + " :End of WHO list\r\n";
}

static void whoChannel(s_commands& com, const std::string& targetChannel, std::map<int, Channel*>* &channels) {
	Channel* channel = NULL;
	
	// debug
	std::cout << RED "WHO command for channel: " RESET << targetChannel << std::endl;

	// Encontrar o canal - seguindo padrão do names.cpp
	std::map<int, Channel*>::iterator it;
	for (it = channels->begin(); it != channels->end(); it++) {
		if (it->second && it->second->getName() == targetChannel) {
			channel = it->second;
			break;
		}
	}
	
	if (!channel) {
		com.sendBuffer += msg_err_nosuchchannel(com.client->getNickName(), "#" + targetChannel);
		std::cout << RED "Channel not found: " RESET << targetChannel << std::endl; // DEBUG
		return;
	}
	
	// Verificar se o cliente está no canal - usando getChannelsSet como no names.cpp
	std::set<std::string>& myChannels = com.client->getChannelsSet();
	bool isInChannel = false;
	for (std::set<std::string>::iterator sit = myChannels.begin(); sit != myChannels.end(); ++sit) {
		if (*sit == targetChannel) {
			isInChannel = true;
			break;
		}
	}
	
	if (!isInChannel) {
		com.sendBuffer += msg_err_notonchannel(com.client->getNickName(), "#" + targetChannel);
		std::cout << RED "Client not in channel: " RESET << targetChannel << std::endl; // DEBUG
		return;
	}
	
	// Obter membros do canal - seguindo padrão do names.cpp
	std::set<int> members = channel->getMembersSet();
	std::set<int> operators = channel->getOperatorsSet();

	std::cout << YELLOW "DEBUG: Channel members count: " RESET << members.size() << std::endl; // DEBUG
	std::cout << YELLOW "DEBUG: Channel operators count: " RESET << operators.size() << std::endl; // DEBUG

	std::set<int>::const_iterator mit;
	for (mit = members.begin(); mit != members.end(); ++mit) {
		std::cout << "DEBUG: Iterating member fd: " << *mit << std::endl; // DEBUG
		int fd = *mit;
		Client* member = NULL;
		
		std::map<int, Client*>::iterator cit = com.clients->find(fd);
		if (cit != com.clients->end())
			member = cit->second;
		
		if (member) {
			std::string flags = "H"; // Here (presente)
			
			// Verificar se é operador - usando getOperatorsNames do Channel
			std::string operators = channel->getOperatorsNames();
			if (operators.find(member->getNickName()) != std::string::npos) {
				flags += "@";
			}
			
			//debug 
			std::string msg = ":" SERVER_NAME " 352 " + com.client->getNickName() 
							+ " #" + targetChannel + " " + member->getUserName() 
							+ " " + member->getHost() + " " SERVER_NAME " " 
							+ member->getNickName() + " " + flags + " :0 " 
							+ member->getRealName() + "\r\n";

			std::cout << msg;
			com.sendBuffer += msg;

			// Resposta 352 (RPL_WHOREPLY)
			// com.sendBuffer += ":" SERVER_NAME " 352 " + com.client->getNickName() 
			// 				+ " #" + targetChannel + " " + member->getUserName() 
			// 				+ " " + member->getHost() + " " SERVER_NAME " " 
			// 				+ member->getNickName() + " " + flags + " :0 " 
			// 				+ member->getRealName() + "\r\n";
		}
	}

    // ITERAR PELOS OPERADORES
    std::set<int>::const_iterator oit;
    for (oit = operators.begin(); oit != operators.end(); ++oit) {
        std::cout << "DEBUG: Iterating operator fd: " << *oit << std::endl;
        int fd = *oit;
        Client* op = NULL;
        
        std::map<int, Client*>::iterator cit = com.clients->find(fd);
        if (cit != com.clients->end())
            op = cit->second;
        
        if (op) {
            std::string flags = "H@"; // Operador com @
            
            std::string msg = ":" SERVER_NAME " 352 " + com.client->getNickName() 
                            + " #" + targetChannel + " " + op->getUserName() 
                            + " " + op->getHost() + " " SERVER_NAME " " 
                            + op->getNickName() + " " + flags + " :0 " 
                            + op->getRealName() + "\r\n";

            std::cout << msg;
            com.sendBuffer += msg;
        }
    }
    
    // Resposta 315 (RPL_ENDOFWHO)
    com.sendBuffer += ":" SERVER_NAME " 315 " + com.client->getNickName() 
                    + " #" + targetChannel + " :End of WHO list\r\n";
}


void Server::who(s_commands& com) {
	if (com.args.empty()) {
		com.sendBuffer += msg_err_needmoreparams(com.client->getNickName(), "WHO");
		return;
	}
	std::string target = com.args[0];
	if (target[0] == '#') {
		whoChannel(com, target.substr(1), this->channels);
	} else {
		whoUser(com, target);
	}
}