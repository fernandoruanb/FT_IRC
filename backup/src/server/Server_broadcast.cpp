#include "../../includes/Server.hpp"

void    Server::broadcast(int sender)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator it = clients->find(fds[sender].fd);
	if (it == clients->end())
	{
		std::cerr << RED "Error: The owner is a ghost!!!" RESET << std::endl;
		return ;
	}
	int	channelTarget = it->second->getChannelOfTime();
	int	channel;
	int    index;

    index = 1;
    if (!this->sendBuffer[sender].empty())
    {
        while (index < this->numClients && fds[index].fd != -1)
        {
            if (index == sender)
            {
                index++;
                continue ;
            }
           std::map<int, Client*>::iterator it = clients->find(fds[index].fd);
	   if (it == clients->end())
	   {
		index++;
		continue ;
	   }
	   channel = it->second->getChannelOfTime();
           Client* client = it->second;
           if (client->getAuthenticated() && client->getNickName() != "*" && client->getRegistered() && channelTarget == channel)
            {
                this->sendBuffer[index] += this->sendBuffer[sender];
                fds[index].events |= POLLOUT;
            }
            index++;
        }
        this->sendBuffer[sender].clear();
    }
}

void    Server::privmsg(int index, int sender, std::string message)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itv = clients->find(fds[index].fd);
	int	ownerChannel;
	int	channel;

	if (index < 1 || itv == clients->end())
		return ;
	channel = itv->second->getChannelOfTime();
	itv = clients->find(fds[sender].fd);
	if (itv == clients->end())
		return ;
	ownerChannel = itv->second->getChannelOfTime();
    std::map<int, Client*>::iterator it = clients->find(fds[index].fd);
    Client* client = it->second;

    if (message.empty() || index < 0 || fds[index].fd == -1 || !client->getRegistered() || !client->getAuthenticated() || client->getNickName() == "*" || ownerChannel != channel)
        return ;
    client->getBufferOut() += message;
    fds[index].events |= POLLOUT;
}
