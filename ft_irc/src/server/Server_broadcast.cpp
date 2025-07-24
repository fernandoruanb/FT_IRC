#include "../../includes/Server.hpp"

void    Server::broadcast(int sender, std::string line, int targetChannel)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Client*>::iterator it = clients->find(fds[sender].fd);
	std::map<int, Client*>::iterator itc;
	std::string	clientChannelName;
	std::string	ownerChannelName;
	std::string	targetChannelName;
	std::string	test;
	int	ownerChannel;

	if (it == clients->end())
	{
		std::cerr << RED "Error: The owner is a ghost!!!" RESET << std::endl;
		return ;
	}
	if (targetChannel != -1)
		ownerChannel = targetChannel;
	else
		ownerChannel = it->second->getChannelOfTime();
	ownerChannelName = (*channels)[ownerChannel]->getName();
	int	clientCurrentChannel;
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
	   clientCurrentChannel = it->second->getChannelOfTime();
	   clientChannelName = (*channels)[clientCurrentChannel]->getName();
           Client* client = it->second;
		if (client->getAuthenticated() && client->getNickName() != "*" && client->getRegistered() && clientCurrentChannel == ownerChannel)
		{
			this->sendBuffer[index] += this->sendBuffer[sender];
			this->sendBuffer[index] += " " + it->second->getNickName() + " :" + line;
			fds[index].events |= POLLOUT;
		}
		if (client->getAuthenticated() && client->getNickName() != "*" && client->getRegistered() && clientCurrentChannel != ownerChannel)
		{
			if ((*clients)[fds[sender].fd]->getChannelsSet().find(clientChannelName) != (*clients)[fds[sender].fd]->getChannelsSet().end() && (*clients)[fds[index].fd]->getChannelsSet().find(ownerChannelName) != (*clients)[fds[index].fd]->getChannelsSet().end())
			{
				std::cout << "A mensagem do owner: " << line << std::endl;
				(*clients)[fds[index].fd]->getSendHistory()[ownerChannel] += this->sendBuffer[sender] + " " + it->second->getNickName() + " :" + line;
				std::cout << ORANGE "ownerChannelName: " << ownerChannelName << std::endl;
				std::cout << BRIGHT_GREEN "clientChannelName: " << clientChannelName << RESET << std::endl;
				std::cout << "The History: " << (*clients)[fds[index].fd]->getSendHistory()[ownerChannel] << std::endl;
			}
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
