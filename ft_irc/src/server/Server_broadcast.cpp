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
	   if (this->kingsOfIRC.find(it->first) != this->kingsOfIRC.end())
	   {
		   ownerChannel = clientCurrentChannel;
		   ownerChannelName = clientChannelName;
	   }
		if (client->getAuthenticated() && client->getNickName() != "*" && client->getRegistered() && clientCurrentChannel == ownerChannel)
		{
			targetChannelName = (*channels)[targetChannel]->getName();
			this->sendBuffer[index] += this->sendBuffer[sender] + " #" + targetChannelName + " :" + line;
			//this->sendBuffer[index] += " " + it->second->getNickName() + " :" + line;
			fds[index].events |= POLLOUT;
		}
		if (client->getAuthenticated() && client->getNickName() != "*" && client->getRegistered() && clientCurrentChannel != ownerChannel)
		{
			targetChannelName = (*channels)[targetChannel]->getName();
			if (checkCompatibility(fds[sender].fd, fds[index].fd, targetChannelName))
			{
				std::cout << "A mensagem do owner: " << line << std::endl;
				(*clients)[fds[index].fd]->getBufferOut() += this->sendBuffer[sender] + " " + it->second->getNickName() + " #" + targetChannelName + " :" + line;
				std::cout << ORANGE "ownerChannelName: " << ownerChannelName << std::endl;
				std::cout << BRIGHT_GREEN "clientChannelName: " << clientChannelName << RESET << std::endl;
				std::cout << "The History: " << (*clients)[fds[index].fd]->getSendHistory()[ownerChannel] << std::endl;
				fds[index].events |= POLLOUT;
			}
		}
		index++;
        }
        this->sendBuffer[sender].clear();
    }
}

bool	Server::checkCompatibility(int ownerFD, int clientFD, std::string targetChannel)
{
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator it = clients->find(ownerFD);
	std::map<int, Client*>::iterator its = clients->find(clientFD);

	if (it == clients->end() || its == clients->end())
		return (false);
	if (it->second->getChannelsSet().find(targetChannel) != it->second->getChannelsSet().end() && its->second->getChannelsSet().find(targetChannel) != its->second->getChannelsSet().end())
		return (true);
	return (false);
}

void    Server::privmsg(int index, int sender, std::string message)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Channel*>::iterator itm;
	std::map<int, Client*>::iterator itv = clients->find(fds[index].fd);
	int	ownerChannel;
	std::string	ownerChannelName;
	std::string	clientChannelName;
	int	clientCurrentChannel;
	int	channel;

	if (index < 1 || itv == clients->end())
		return ;
	channel = itv->second->getChannelOfTime();
	clientCurrentChannel = channel;
	itm = channels->find(channel);
	if (itm == channels->end())
	{
		std::cerr << RED "The client channel doesn't exist" RESET << std::endl;
		return ;
	}
	clientChannelName = itm->second->getName();
	itv = clients->find(fds[sender].fd);
	if (itv == clients->end())
	{
		std::cerr << RED "The owner channel doesn't exist" RESET << std::endl;
		return ;
	}
	ownerChannel = itv->second->getChannelOfTime();
	itm = channels->find(ownerChannel);
	ownerChannelName = itm->second->getName();
	if (this->kingsOfIRC.find(itv->first) != this->kingsOfIRC.end())
	{
		ownerChannel = clientCurrentChannel;
		ownerChannelName = clientChannelName;
	}
    std::map<int, Client*>::iterator it = clients->find(fds[index].fd);
    Client* client = it->second;

    if (message.empty() || index < 0 || fds[index].fd == -1 || !client->getRegistered() || !client->getAuthenticated() || client->getNickName() == "*" || ownerChannel != channel)
	{
		if (client->getAuthenticated() && client->getNickName() != "*" && client->getRegistered() && clientCurrentChannel != ownerChannel)
                {
                        if (checkCompatibility(fds[sender].fd, fds[index].fd, "generic"))
			{
				(*clients)[fds[index].fd]->getBufferOut() += std::string(":") + (*clients)[fds[sender].fd]->getNickName() + "!" + (*clients)[fds[sender].fd]->getUserName() + "@" + (*clients)[fds[sender].fd]->getHost() + " PRIVMSG " + (*clients)[fds[index].fd]->getNickName() + " :" + message;
				fds[index].events |= POLLOUT;
			}
                }
		return ;
	}
    client->getBufferOut() += std::string(":") + (*clients)[fds[sender].fd]->getNickName() + "!" + (*clients)[fds[sender].fd]->getUserName() + "@" + (*clients)[fds[sender].fd]->getHost() + " PRIVMSG " + (*clients)[fds[index].fd]->getNickName() + " :" + message;
    fds[index].events |= POLLOUT;
}
