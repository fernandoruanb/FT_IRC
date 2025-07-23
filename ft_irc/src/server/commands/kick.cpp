#include "../includes/Server.hpp"

void	Server::kick(s_commands& com)
{
	if (com.args.size() < 2) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_needmoreparams("KICK");
		return ;
	}
	int channelIndex = getChannelsIndex(com.args[0]);
	if (channelIndex == -1) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_nosuchchannel(com.client->getNickName(), com.args[0]);
		return ;
	}
	Channel* channel = this->channels->find(channelIndex)->second;
	int clientIndex = getClientsIndex(com.fd);
	if (!channel->isMemberOfChannel(clientIndex)) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_notonchannel(com.client->getNickName(), com.args[0]);
		return ;
	}
	int targetIndex = getClientsIndex(std::atoi(com.args[1].c_str()));
	if (!channel->isMemberOfChannel(targetIndex)) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_usernotinchannel(com.client->getNickName(), com.args[1], com.args[0]);
		return ;
	}
	this->kickFromChannel(com.args[0], clientIndex, targetIndex);
}
