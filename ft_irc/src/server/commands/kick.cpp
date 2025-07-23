#include "../includes/Server.hpp"

void	Server::kick(s_commands& com)
{
	if (com.args.size() < 2) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_needmoreparams("KICK");
		return ;
	}
	Channel* channel = this->channels->find(channelIndex)->second;
	int clientIndex = getClientsIndex(com.fd);
	if (!channel.isMemberOfChannel(clientIndex)) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_notonchannel(com.args[0]);
		return ;
	}
	int targetIndex = getClientsIndex(com.args[1]);
	if (!channel.isMemberOfChannel(targetIndex)) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_usernotinchannel(com.args[1]);
		return ;
	}
	this->kickFromChannel(com.args[0], clientIndex, targetIndex);
}
