#include "../includes/Server.hpp"

static int	getTheTargetFD(const std::string target, int numClients)
{
	std::map<int, Client*>* clients = getClientsMap();

	struct pollfd (&fds)[1024] = *getMyFds();
	int	index = 1;
	int	targetFD = -1;

	while (index < numClients && fds[index].fd != -1)
	{
		if ((*clients)[fds[index].fd]->getNickName() == target)
		{
			targetFD = fds[index].fd;
			return (targetFD);
		}
		++index;
	}
	return (targetFD);
}

void	Server::kick(s_commands& com)
{
	struct pollfd (&fds)[1024] = *getMyFds();

	if (com.args.size() < 2) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_needmoreparams("KICK");
		return ;
	}

	int channelIndex = getChannelsIndex(com.args[0].substr(1));
	if (channelIndex == -1) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_nosuchchannel(com.client->getNickName(), com.args[0].substr(1));
		return ;
	}
	
	Channel* channel = this->channels->find(channelIndex)->second;
	int clientIndex = getClientsIndex(com.fd);
	int theKing = this->kingsOfIRC.find(com.fd) != this->kingsOfIRC.end();

	if (!theKing && !channel->isMemberOfChannel(clientIndex)) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_notonchannel(com.client->getNickName(), com.args[0].substr(1));
		return ;
	}

	int targetFD = getTheTargetFD(com.args[1], this->numClients);
	if (!theKing && !channel->isMemberOfChannel(targetFD)) {
		this->sendBuffer[com.index].clear();
		this->sendBuffer[com.index] += msg_err_usernotinchannel(com.client->getNickName(), com.args[1], com.args[0].substr(1));
		return ;
	}
	this->kickFromChannel(com.args[0].substr(1), fds[com.index].fd, targetFD);
}
