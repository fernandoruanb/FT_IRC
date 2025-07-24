#include "../includes/Server.hpp"

void	Server::part(s_commands& com)
{
	if (com.args.size() < 1) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_needmoreparams("PART");
		return ;
	}
	std::string	msg;
	bool		haveMsg = false;
	for (size_t i = 0; i < com.args.size(); ++i) {
		if (com.args[i].size() > 0 && com.args[i][0] == ':') {
			msg = com.args[i].substr(1);
			haveMsg = true;
			continue ;
		}
		if (haveMsg) {
			msg += " " + com.args[i];
		}
	}
	for (size_t i = 0; i < com.args.size(); ++i) {
		if (com.args[i][0] == ':')
			break ;
		if (com.args[i].empty() || com.args[i][0] != '#') {
			this->sendBuffer[com.index] += msg_err_nosuchchannel(com.client->getNickName(), com.args[i]);
			std::cout << "channel EMPTY or no '#': " << com.args[i] << std::endl; /// debug
			continue ;
		}
		std::string channelName = com.args[i].substr(1);
		int channelIndex = getChannelsIndex((channelName));
		if (channelIndex == -1) {
			this->sendBuffer[com.index] += msg_err_nosuchchannel(com.client->getNickName(), channelName);
			std::cout << "channel not found: " << channelName << std::endl; /// debug
			continue ;
		}
		Channel* channel = this->channels->find(channelIndex)->second;
		if (!channel->isMemberOfChannel(com.fd) && !channel->isOperatorOfChannel(com.fd)) {
			this->sendBuffer[com.index] += msg_err_notonchannel(com.client->getNickName(), com.args[i]);
			std::cout << "client not member of channel: " << com.args[i] << std::endl; /// debug
			continue ;
		}
		if (channel->isMemberOfChannel(com.fd)) {
			com.client->getChannelsSet().erase(channelName);
			channel->removeMember(com.fd);
		}
		if (channel->isOperatorOfChannel(com.fd)) {
			com.client->getChannelsSet().erase(channelName);
			com.client->getOperatorChannels().erase(channelName);
		}

		if (haveMsg) {
			this->sendBuffer[com.index] += ":" + getClientsMap()->find(com.fd)->second->getNickName() + "!" + getClientsMap()->find(com.fd)->second->getUserName() + " PART " + com.args[i] + " :" + msg + "\n";
			broadcast(com.index, this->sendBuffer[com.index]);
		} else {
			this->sendBuffer[com.index] += ":" + getClientsMap()->find(com.fd)->second->getNickName() + "!" + getClientsMap()->find(com.fd)->second->getUserName() + " PART " + com.args[i] + "\n";
			broadcast(com.index, this->sendBuffer[com.index]);
		}
		this->sendBuffer[com.index] += "You have left the channel: " + com.args[i] + "\n";
	}
}
