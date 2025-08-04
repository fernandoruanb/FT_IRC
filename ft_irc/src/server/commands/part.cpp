#include "../includes/Server.hpp"

void	Server::part(s_commands& com)
{
	if (com.args.size() < 1) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_needmoreparams(com.client->getNickName(), "PART");
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
			this->sendBuffer[com.index] += msg_err_notonchannel(com.client->getNickName(), channelName);
			std::cout << "client not member of channel: " << com.args[i] << std::endl; /// debug
			continue ;
		}
		if (channel->isMemberOfChannel(com.fd) || channel->isOperatorOfChannel(com.fd)) {
			channel->removeMember(com.fd);
			if (channel->getMembersNum() == 0)
				deleteChannel(channel->getName(), com.fd);
			else
			{
				com.client->getOperatorChannels().erase(channelName);
				com.client->getChannelsSet().erase(channelName);
				com.client->getInviteChannels().erase(channelName);
				channel->removeMember(com.fd);
				channel->getOperatorsSet().erase(com.fd);
				channel->getMembersSet().erase(com.fd);
				com.client->getSendHistory()[channelIndex].clear();
				this->changeChannel("Generic", com.fd, 0);
			}
		}
        	if (com.client->getOperatorChannels().size() == 0)
                	com.client->setIsOperator(false);
		return ;

		if (haveMsg) {
			this->sendBuffer[com.index] += ":" + getClientsMap()->find(com.fd)->second->getNickName() + "!" + getClientsMap()->find(com.fd)->second->getUserName() + " PART " + com.args[i] + " :" + msg + "\n";
			broadcast(com.index, this->sendBuffer[com.index]);
		} else {
			this->sendBuffer[com.index] += ":" + getClientsMap()->find(com.fd)->second->getNickName() + "!" + getClientsMap()->find(com.fd)->second->getUserName() + " PART " + com.args[i] + "\n";
			broadcast(com.index, this->sendBuffer[com.index]);
		}
		com.client->getBufferOut() += ":" + com.client->getNickName() + "!" + com.client->getUserName() + "@" + com.client->getHost() + " PART " + " #" + channel->getName() + " :You have left the channel: " + "\r\n";
	}
}
