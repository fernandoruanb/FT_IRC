#include "../includes/Server.hpp"

void	Server::part(s_commands& com)
{
	if (com.args.size() < 1) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_needmoreparams("PART");
		return ;
	}
	std::string	msg;
	bool		foundMsg = false;
	for (int i = 0; i < com.args.size(); ++i) {
		if (com.args[i].size() > 0 && com.args[i][0] == ':') {
			msg = com.args[i].substr(1);
			foundMsg = true;
			continue ;
		}
		if (foundMsg) {
			msg += " " + com.args[i];
		}
	}
	for (int i = 0; i < com.args.size(); ++i) {
		if (com.args[i][0] == ':')
			continue ;
		if (com.args[i].empty() || com.args[i][0] != '#') {
			this->sendBuffer[com.index] += msg_err_nosuchchannel(com.args[i]);
			continue ;
		}
		int channelIndex = getChannelsIndex(com.args[i]);
		if (channelIndex == -1) {
			this->sendBuffer[com.index] += msg_err_nosuchchannel(com.args[i]);
			continue ;
		}
		Channel* channel = this->channels->find(channelIndex)->second;
		int clientIndex = getClientsIndex(com.fd);
		if (!channel->isMemberOfChannel(clientIndex)) {
			this->sendBuffer[com.index] += msg_err_notonchannel(com.args[i]);
			continue ;
		}
		channel->removeMember(clientIndex);
		if (foundMsg) {
			this->sendBuffer[com.index] += ":" + getClientsMap()->find(com.fd)->second->getNickName() + "!" + getClientsMap()->find(com.fd)->second->getUserName() + " PART " + com.args[i] + " :" + msg + "\n";
			broadcast(com.index);
		} else {
			this->sendBuffer[com.index] += ":" + getClientsMap()->find(com.fd)->second->getNickName() + "!" + getClientsMap()->find(com.fd)->second->getUserName() + " PART " + com.args[i] + "\n";
			broadcast(com.index);
		}
		this->sendBuffer[com.index] += "You have left the channel: " + com.args[i] + "\n";
	}
}
