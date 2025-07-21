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
		this->sendBuffer[com.index] += msg_err_nosuchchannel(com.args[0]);
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
	if (!channel.isOperatorOfChannel(clientIndex)) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_chanoprivsneeded(com.args[0]);
		return ;
	}
	std::string message = com.args.size() > 2 ? com.args[2] : "";
	this->sendBuffer[targetIndex].clear();

	// this->sendBuffer[com.index].clear();
	// this->sendBuffer[com.index] = "You called KICK\n";
}
