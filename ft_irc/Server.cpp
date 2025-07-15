/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 10:02:08 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/15 19:00:15 by jopereir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "messages.hpp"
#include <cstring>

static bool	isEmptyInput(const std::string &line)
{
	return (line.empty() || line == "\n" || line == "\r" || line == "\r\n");
}

static std::map<int, Channel*>* getChannelsMap(void)
{
	static std::map<int, Channel*> channelsMap;

	return (&channelsMap);
}

static std::map<int, Client*>* getClientsMap(void)
{
	static std::map<int, Client*> clientsMap;

	return (&clientsMap);
}

static struct pollfd(*getMyFds(void))[1024]
{
	static struct pollfd	fds[1024];

	return (&fds);
}

bool	Server::checkName(std::string Name)
{
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Channel*>::iterator it = channels->find(0);
	std::string	channelName;
	int	index;

	index = 1;
	while (index < this->numChannels && it != channels->end())
	{
		channelName = it->second->getName();
		if (channelName == Name)
			return (false);
		it = channels->find(index);
		index++;
	}
	return (true);
}

int	Server::findGoodIndex(void)
{
	std::map<int, Channel *>* channels = getChannelsMap();
	std::map<int, Channel *>::iterator it;
	int	index;

	index = 1;
	it = channels->find(index);
	while (index < INT_MAX && it != channels->end())
	{
		index++;
		it = channels->find(index);
	}
	return (index);
}

void	Server::createNewChannel(std::string Name, int clientFD)
{
	Channel* channel = new Channel(Name);
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Client*>::iterator it = clients->find(clientFD);
	int	index;

	if (numChannels == INT_MAX)
	{
		std::cerr << RED "Error: There are too many channels!!!" RESET << std::endl;
		delete channel;
	}

	if (it == clients->end())
	{
		std::cerr << RED "Error: A ghost can't become an admin of a channel" RESET << std::endl;
		delete channel;
		return ;
	}
	Client* client = it->second;
	if (!this->checkName(Name))
	{
		std::cerr << RED "Error: The new channel name is in use" RESET << std::endl;
		delete channel;
		return ;
	}
	index = this->findGoodIndex();
	(*channels)[index] = channel;
	std::cout << LIGHT_BLUE "The index of the channel " << YELLOW << Name << LIGHT_BLUE " is " << YELLOW << index << RESET << std::endl;
	this->numChannels++;
	client->setIsOperator(true);
	client->getOperatorChannels().insert(Name);
	client->getChannelsSet().insert(Name);
	std::cout << LIGHT_BLUE "Client " << YELLOW << clientFD << LIGHT_BLUE " is now the operator of " << YELLOW << Name << LIGHT_BLUE " Channel" RESET << std::endl;
}

std::string	Server::getText(std::string& buffer, size_t *pos, std::map<int, Client*>* clients, bool check_name = false)
{
	size_t	j = *pos;
	std::string	str;

	//Get real name
	if (*pos < buffer.size() && buffer[*pos] == ':')
	{
		(*pos)++;
		str = buffer.substr(*pos);
		*pos = buffer.size();
		return (str);
	}
	
	while (j < buffer.size() && (!std::isspace(buffer[j]) && buffer[j] != ':'))
		j++;
	str = buffer.substr(*pos, j - *pos);
	if (check_name)
	{
		std::map<int, Client*>::iterator	it;
		for (it = clients->begin(); it != clients->end(); it++)
			if (it->second->getUserName() == str)
				return ("");
	}
	*pos = ++j;
	return (str);
}

bool	Server::isValidArgs(const std::string &buffer, size_t pos, bool &op)
{
	int		cnt = 0;
	size_t	len = buffer.size();

	while (pos < len)
	{
		while (std::isspace(buffer[pos]) || buffer[pos] == ':')
			pos++;
		if (pos == len)
			break;

		cnt++;

		while (pos < len && (!std::isspace(buffer[pos]) && buffer[pos] != ':'))
			pos++;
	}
	if (cnt > 3)
		op = true;
	return (cnt >= 3);
}

void	Server::getClientInfo(std::map<int, Client*>* clients, std::string& buffer, int fd, int i)
{
	bool	optional = false;

	//	Find the client to register
	std::map<int, Client*>::iterator	it = clients->find(fd);
	struct	pollfd						(&fds)[1024] = *getMyFds();

	// (*clients)[fd];

	if (it == clients->end())
		return;

	//	Start to register
	Client*	myClient = it->second;
	
	if (myClient->getRegistered())
		return;

	size_t	pos = buffer.find("USER");
	if (pos != std::string::npos)
	{
		pos += 5;
		if (!isValidArgs(buffer, pos, optional))
		{
			this->sendBuffer[i] += msg_notice("USER <username> <hostname> <servername> :<realname>");
			fds[i].events |= POLLOUT;
			return;
		}
		
		myClient->setUserName(getText(buffer, &pos, clients, true));
		// std::string	tempName = getText(buffer, &pos, clients, true);
		if (myClient->getUserName().empty())
		{
			this->sendBuffer[i] += std::string(BRIGHT_RED) + "Error: " + RESET + "User name already in use.\n";
			fds[i].events |= POLLOUT;
			return;
		}
		
		//Seting the client info
		// myClient->setUserName(tempName);
		myClient->setHost(getText(buffer, &pos, clients));
		myClient->setServerName(getText(buffer, &pos, clients));
		if (optional)
			myClient->setRealName(getText(buffer, &pos, clients));
		this->sendBuffer[i].clear();
		this->sendBuffer[i] += msg_notice("Welcome " + myClient->getUserName() + "!");
		myClient->setRegistered(true);
	}
	fds[i].events |= POLLOUT;
}

bool Server::handleClientAuthentication(std::map<int, Client*>* clients, int fd, char* buffer, int pollIndex) {
	std::map<int, Client*>::iterator it = clients->find(fd);
	if (it != clients->end()) {
		Client* client = it->second;
		if (!client->getAuthenticated()) {
			struct pollfd (&fds)[1024] = *getMyFds();
			std::string input(buffer);
			if (input.empty() || input == "\r" || input == "\n" || input == "\r\n") {
				return true; // Ignore empty inputs
			}
			if (input.rfind("PASS ", 0) == 0) {
				std::string pass = input.substr(5);
				pass.erase(pass.find_last_not_of("\r\n") + 1);
				if (pass == this->getPassword()) {
					client->setAuthenticated(true);
					this->sendBuffer[pollIndex].clear();
					this->sendBuffer[pollIndex] += msg_notice("Authentication successful");
					this->sendBuffer[pollIndex] += msg_notice("USER <username> <hostname> <servername> :<realname>");
					fds[pollIndex].events |= POLLOUT;
					return true;
				} else {	
					this->sendBuffer[pollIndex].clear();
					this->sendBuffer[pollIndex] += msg_err_passwdmismatch();
					fds[pollIndex].events |= POLLOUT;
					return false;
				}
			   } else {
				   this->sendBuffer[pollIndex].clear();
				   this->sendBuffer[pollIndex] += msg_err_needmoreparams("PASS");
				   fds[pollIndex].events |= POLLOUT;
				   return false;
			}
		}
	}
	return true;
}

void	Server::inviteToChannel(std::string channelName, int operatorFD, int clientFD)
{
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>::iterator itch = channels->begin();
	std::map<int, Client*>::iterator itc = clients->find(operatorFD);

	if (itc == clients->end())
	{
		std::cerr << RED "Error: The operator doesn't exist. It's a ghost!" RESET << std::endl;
		return ;
	}
	if (!itc->second->getIsOperator())
	{
		std::cerr << RED "Error: There is someone trying to invite other people without enough permissions" RESET << std::endl;
		return ;
	}
	if (itc->second->getOperatorChannels().find(channelName) == itc->second->getOperatorChannels().end())
	{
		std::cerr << RED "Error: The operator is a valid operator but not from that channel" RESET << std::endl;
		return ;
	}
	itc = clients->find(clientFD);
	if (itc == clients->end())
	{
		std::cerr << RED "Error: The clientFD doesn't exist. It's a ghost!" RESET << std::endl;
		return ;
	}
	while (itch != channels->end())
	{
		if (itch->second->getName() == channelName)
			break ;
		++itch;
	}
	if (itch == channels->end())
	{
		std::cerr << RED "Error: The channel doesn't exist. It's a ghost!!!" RESET << std::endl;
		return ;
	}
	itch->second->setInviteFlag(true);
	itc->second->getInviteChannels().insert(channelName);
	std::cout << LIGHT_BLUE "The client " << YELLOW << clientFD << LIGHT_BLUE " received an invite to " << YELLOW << channelName << LIGHT_BLUE " channel by " << YELLOW << operatorFD << std::endl;
}

void	Server::changeTopic(std::string channelName, int clientFD, std::string topic)
{
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Channel*>::iterator it = channels->begin();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itc = clients->find(clientFD);
	int	isOperator;

	while (it != channels->end())
	{
		it++;
		if (it != channels->end())
		{
			if (it->second->getName() != channelName)
				continue ;
			else
				break ;
		}
	}
	if (it == channels->end())
	{
		std::cerr << RED "Error: The channel doesn't exist to change topic" RESET << std::endl;
		return ;
	}
	if (itc == clients->end())
	{
		std::cerr << RED "Error: A ghost can't change the channel topic" RESET << std::endl;
		return ;
	}
	isOperator = itc->second->getIsOperator();
	if (it->second->getTopicFlag())
	{
		if (!isOperator)
		{
			std::cerr << RED "Error: The client isn't a true operator to do privileged action" RESET << std::endl;
			return ;
		}
	}
	if (it->first != itc->second->getChannelOfTime())
	{
		std::cerr << RED "Error: You can't change a topic in another channel" RESET << std::endl;
		return ;
	}
	it->second->setTopic(topic);
	std::cout << LIGHT_BLUE "The topic of the channel " << YELLOW << it->second->getName() << LIGHT_BLUE " changed to " << YELLOW << topic << RESET << std::endl;
}

void	Server::addNewClient(int clientFD)
{
	int	index;
	struct pollfd (&fds)[1024] = getPollFds();
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Channel*>::iterator it = channels->find(0);
	index = 1;
	while (index < 1024)
	{
		if (fds[index].fd == -1)
			break ;
		index++;
	}
	if (index == 1024)
	{
		std::cerr << RED "Error: Maximum of FDs!!!" RESET << std::endl;
		return ;
	}
	(*this->clients)[clientFD] = new Client(clientFD);
	Client* theClient = (*this->clients)[clientFD];
	theClient->getChannelsSet().insert("Generic");
	theClient->setChannelOfTime(0);
	if (it != channels->end())
	{
		std::cout << LIGHT_BLUE "Adding new client " << YELLOW << clientFD << LIGHT_BLUE " To generic channel =D" << std::endl;
		Channel* generic = it->second;
		generic->addNewMember(clientFD);
		if (clientFD == 4)
			this->createNewChannel("Channel One", clientFD);
		if (clientFD == 5)
		{
			this->createNewChannel("Channel Two", clientFD);
			this->createNewChannel("Channel Three", clientFD);
			this->createNewChannel("Channel Four", clientFD);
			this->createNewChannel("Channel Five", clientFD);
			this->createNewChannel("Channel Six", clientFD);
			this->createNewChannel("Channel Seven", clientFD);
			this->createNewChannel("Channel Eight", clientFD);
			this->createNewChannel("Channel Nine", clientFD);
			this->deleteChannel("Channel Nine", clientFD);
			this->deleteChannel("Channel Seven", clientFD);
			this->deleteChannel("Channel Four", clientFD);
			this->changeChannel("Channel Six", clientFD);
			this->createNewChannel("Channel Two", clientFD);
			this->changeChannel("Channel Two", clientFD);
			this->createNewChannel("Channel Seven", clientFD);
			this->deleteChannel("Channel Two", clientFD);
			this->changeChannel("Channel Eight", clientFD);
			this->changeTopic("Channel Eight", clientFD, "Masters of Universe");
			this->inviteToChannel("Channel Three", clientFD, 4);
			this->inviteToChannel("Channel Two", clientFD, 4);
			this->inviteToChannel("Channel One", 4, clientFD);
			this->deleteChannel("Channel Three", clientFD);
			this->createNewChannel("Channel Three", clientFD);
			it = channels->find(2);
			it->second->setInviteFlag(true);
			this->changeChannel("Channel Three", 4);
			this->inviteToChannel("Channel Three", clientFD, 4);
			this->changeChannel("Channel Three", 4);
			this->changeChannel("Generic", 4);
			this->changeChannel("Generic", 5);
		}
	}
	fds[index].fd = clientFD;
	fds[index].events = POLLIN;
	fcntl(clientFD, F_SETFL, O_NONBLOCK);
	this->numClients++;
	// NOTICE message to the new client. Asking for authentication.
	this->sendBuffer[index] = msg_notice("Connected. Please authenticate with \"PASS <password>\"");
	fds[index].events |= POLLOUT;
	std::cout << BRIGHT_GREEN "New Client added: " << YELLOW << clientFD << RESET << std::endl;
}

void	Server::PollServerRoom(void)
{
	sockaddr_in	client;
	socklen_t	client_len;
	struct pollfd (&fds)[1024] = *getMyFds();
	int	newClientFD;

	client_len = sizeof(client);

	if (*this->running && fds[0].revents & POLLIN)
	{
		newClientFD = accept(this->serverIRC, (struct sockaddr *)&client, &client_len);
		if (newClientFD != -1) {
			this->addNewClient(newClientFD);
		}
	}
}

void	Server::PollInputClientMonitoring(void)
{
	int	index;
	ssize_t	bytes;
	char	buffer[513];
	std::map<int, Client*>* clients = getClientsMap();
	struct pollfd (&fds)[1024] = *getMyFds();

	index = 1;
	while (*this->running && index < this->numClients && fds[index].fd != -1)
	{
	   if (fds[index].revents & POLLIN)
	   {
		   bytes = recv(fds[index].fd, buffer, 512, 0);
		   if (bytes > 0) {
			   buffer[bytes] = '\0';
			   this->recvBuffer[index] += buffer; // Acumula o que chegou

			   size_t pos;
			   while ((pos = this->recvBuffer[index].find('\n')) != std::string::npos)
			   {
				   std::string line = this->recvBuffer[index].substr(0, pos + 1);
				   this->recvBuffer[index].erase(0, pos + 1);

				   std::string	name = (*clients)[fds[index].fd]->getUserName();
				   std::cout << BRIGHT_GREEN << (name.empty() ? "Client": name) << ": " << YELLOW << fds[index].fd << LIGHT_BLUE << " " << line << RESET << std::endl;
					// this->sendBuffer[index] = name + ": " + this->sendBuffer[index];

				   if (!handleClientAuthentication(clients, fds[index].fd, (char*)line.c_str(), index)) {
					   continue;
				   }

				   getClientInfo(clients, line, fds[index].fd, index);

				   Client* client = (*clients)[fds[index].fd];
				   if (!client->getAuthenticated() || !client->getRegistered())
					   continue;
				   if (line.rfind("PASS ", 0) == 0 || line.rfind("USER ", 0) == 0) {
					   continue;
				   }
				   this->sendBuffer[index].clear();
				   if (!isEmptyInput(line))
				   	this->sendBuffer[index] += "\n" + std::string(YELLOW) + (*clients)[fds[index].fd]->getUserName() + RESET + ": " + line;
				   this->broadcast(index);
				   //this->privmsg(index - 1, "You are very special =D\n");
				   fds[index].events |= POLLOUT;
			   }
		   }
		   if (bytes == 0)
		   {
			   std::cout << LIGHT_BLUE "Client " << YELLOW << fds[index].fd << LIGHT_BLUE " disconnected" << RESET << std::endl;
			   std::map<int, Client*>::iterator it = clients->find(fds[index].fd);
			   delete it->second;
			   close(fds[index].fd);
			   fds[index].fd = fds[numClients - 1].fd;
			   fds[numClients - 1].fd = -1;
			   fds[numClients - 1].events = 0;
			   this->manageBuffers(index);
			   this->numClients--;
		   }
	   }
	   index++;
   }
}

void	Server::PollOutMonitoring(void)
{
	int	index;
	ssize_t	bytes;
	struct pollfd (&fds)[1024] = *getMyFds();

	index = 0;
	while (*this->running && index < this->numClients && fds[index].fd != -1)
	{
		if (fds[index].revents & POLLOUT)
		{
			bytes = send(fds[index].fd, this->sendBuffer[index].c_str(), sendBuffer[index].size(), 0);
			if (bytes > 0)
				this->sendBuffer[index].erase(0, bytes);
			if (this->sendBuffer[index].empty())
				fds[index].events &= ~POLLOUT;
		}
		index++;
	}
}
static bool	*getRunning(void)
{
	static bool running = true;
	return (&running);
}

Server::~Server(void)
{
	std::cout << BRIGHT_GREEN "Thank you very much for using our IRC Server =D" RESET << std::endl;
}

int	Server::atoiIRC(std::string port)
{
	const char	*tmp;
	int	result;

	tmp = port.c_str();
	result = 0;

	if (port.empty())
		return (-1);
	while (*tmp >= '0' && *tmp <= '9')
	{
		result *= 10;
		result += *tmp - '0';
		if (result > 65535)
			return (-1);
		tmp++;
	}
	if (*tmp != '\0')
		return (-1);
	return (result);
}

void	Server::kickFromChannel(std::string channel, int owner, int clientFD)
{
	std::map<int, Channel *>* channels = getChannelsMap();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itch = clients->find(owner);
	std::map<int, Channel*>::iterator itm;
	std::string	channelName;
	int	channelOfTime;
	bool	isOperator;

	if (itch == clients->end())
	{
		std::cerr << RED "Error: There is a ghost trying to kick someone!!!" RESET << std::endl;
		return ;
	}
	isOperator = itch->second->getIsOperator();
	if (!isOperator)
	{
		std::cerr << RED "Error: The owner isn't a true operator of the channel " << YELLOW << channel << RED " to kick someone" RESET << std::endl;
		return ;
	}
	channelOfTime = itch->second->getChannelOfTime();
	itm = channels->find(channelOfTime);
	if (itm == channels->end())
	{
		std::cerr << RED "Error: It's impossible to kick someone from a ghost channel" RESET << std::endl;
		return ;
	}
	if (channel != itm->second->getName())
	{
		std::cerr << RED "Error: Your current channel isn't the target channel dear owner" RESET << std::endl;
		return ;
	}
	if (itch->second->getOperatorChannels().find(channel) == itch->second->getOperatorChannels().end())
	{
		std::cerr << RED "Error: You are an operator but not from the target channel" RESET << std::endl;
		return ;
	}
	itch = clients->find(clientFD);
	if (itch == clients->end())
	{
		std::cerr << RED "Error: The target client to kick doesn't exist" RESET << std::endl;
		return ;
	}
	if (itch->second->getChannelsSet().find(channel) == itch->second->getChannelsSet().end())
	{
		std::cerr << RED "Error: The client is not in the target channel " << YELLOW << channel << RESET << std::endl;
		return ;
	}
	itch->second->getOperatorChannels().erase(channel);
	if (itch->second->getOperatorChannels().size() == 0)
		itch->second->setIsOperator(false);
	itch->second->setChannelOfTime(0);
	std::cout << LIGHT_BLUE "The client " << YELLOW << clientFD << LIGHT_BLUE " has been kicked by " << YELLOW << owner << LIGHT_BLUE " and lost all privileges coming back to " << YELLOW "Generic" << LIGHT_BLUE " Channel" RESET << std::endl;
}

void	Server::removeOperatorPrivilegesFromEveryBody(std::string channel)
{
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Client*>::iterator it = clients->begin();
	std::map<int, Channel*>::iterator itm = channels->begin();
	int	channelOfTime;

	while (itm != channels->end())
	{
		if (channel == itm->second->getName())
			break ;
		itm++;
	}
	if (itm == channels->end())
	{
		std::cerr << RED "The channel to remove extra things doesn't exist" RESET << std::endl;
		return ;
	}
	while (it != clients->end())
	{
		it->second->getOperatorChannels().erase(channel);
		it->second->getChannelsSet().erase(channel);
		it->second->getInviteChannels().erase(channel);
		if (it->second->getOperatorChannels().size() == 0)
			it->second->setIsOperator(false);
		channelOfTime = it->second->getChannelOfTime();
		if (channelOfTime == itm->first)
			changeChannel("Generic", it->second->getClientFD());
		it++;
	}
	std::cout << BRIGHT_GREEN "The channel " << ORANGE << channel << BRIGHT_GREEN " was cleaned successfully" RESET << std::endl;
}

void	Server::deleteChannel(std::string channel, int clientFD)
{
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itch = clients->find(clientFD);
	std::string	channelName;
	int	channelOfTime;
	bool	isOperator;
	int	index;

	if (itch == clients->end())
	{
		std::cerr << RED "Error: Impossible to remove a channel because the client is a ghost" RESET << std::endl;
		return ;
	}
	isOperator = itch->second->getIsOperator();
	if (!isOperator)
	{
		std::cerr << RED "Error: The client " << YELLOW << clientFD << RED " isn't an operator" RESET << std::endl;
		return ;
	}
	if (itch->second->getOperatorChannels().find(channel) == itch->second->getOperatorChannels().end())
	{
		std::cerr << RED "Error: The client isn't a valid operator of the channel " << YELLOW << channel << RESET << std::endl;
		return ;
	}
	index = 1;
	std::map<int, Channel*>::iterator itc = channels->find(index);
	while (index < numChannels && itc != channels->end())
	{
		channelName = itc->second->getName();
		if (channelName == channel)
		{
			channelOfTime = itch->second->getChannelOfTime();
			if (channelOfTime == index)
			{
				std::cout << LIGHT_BLUE "Changing to " << YELLOW << "Generic" << LIGHT_BLUE " Channel client " << YELLOW << clientFD << RESET << std::endl;
				this->changeChannel("Generic", itch->second->getClientFD());
			}
			this->removeOperatorPrivilegesFromEveryBody(channelName);
			delete itc->second;
			channels->erase(itc);
			itch->second->getChannelsSet().erase(channelName);
			std::cout << LIGHT_BLUE "Channel " << YELLOW << channelName << LIGHT_BLUE << " removed successfully" RESET << std::endl;
			numChannels--;
			itch->second->getOperatorChannels().erase(channelName);
			if (itch->second->getOperatorChannels().size() == 0)
			{
				itch->second->setIsOperator(false);
				std::cout << LIGHT_BLUE "The Client " << YELLOW << clientFD << LIGHT_BLUE " lost the operator privileges about channel " << YELLOW << channelName << RESET << std::endl;
			}
			return ;
		}
		itc++;
		index++;
	}
	std::cerr << RED "Error: The channel " << YELLOW << channel << RED " doesn't exist" RESET << std::endl;
}

void	Server::changeChannel(std::string channel, int clientFD)
{
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itc = clients->find(clientFD);
	if (itc == clients->end())
	{
		std::cerr << RED "Error: The client is a ghost trying to changing a channel" RESET << std::endl;
		return ;
	}
	Client* client = itc->second;
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Channel*>::iterator itm;
	std::string	channelName;

	if (!client)
	{
		std::cerr << RED "Error: the client to change channel is a ghost" RESET << std::endl;
		return ;
	}
	itm = channels->begin();
	while (itm != channels->end())
	{
		channelName = itm->second->getName();
		if (channelName == channel)
		{
			Channel* channelOfficial = itm->second;
			if (itm->second->getInviteFlag())
			{
				std::cout << BRIGHT_GREEN << "The channel " << YELLOW << channelName << BRIGHT_GREEN " needs invite flags to change to it." << RESET << std::endl;
				if (itc->second->getInviteChannels().find(channelName) == itc->second->getInviteChannels().end())
				{
					std::cerr << RED "Error: The client doesn't have the invite necessary to change to this channel " << YELLOW << channelName <<  RESET << std::endl;
					return ;
				}
			}
			std::cout << LIGHT_BLUE "Client " << YELLOW << clientFD << LIGHT_BLUE " changing to " << YELLOW << channelOfficial->getName() << RESET << std::endl;
			client->setChannelOfTime(itm->first);
			itm->second->addNewMember(clientFD);
			client->getChannelsSet().insert(channel);
			return ;
		}
		itm++;
	}
	std::cerr << RED "Error: Impossible to change the channel because it's a ghost" RESET << std::endl;
}

Server::Server(std::string portCheck, std::string password): numChannels(0)
{
	int	port;

	try
	{
		this->running = getRunning();
		this->fds = getMyFds();
		this->clients = getClientsMap();
		this->channels = getChannelsMap();
		(*channels)[0] = new Channel("Generic");
		numChannels++;
		//this->createNewChannel("Channel One");
		port = atoiIRC(portCheck);
		if (port == -1)
			throw std::exception();
		if (password.empty())
			throw std::exception();
		std::cout << LIGHT_BLUE "Starting the server with your configuration 127.0.0.1:" << YELLOW <<  port << RESET << std::endl;
		init(port, password);

	} catch (std::exception &e) 
	{
		if (portCheck.empty())
			portCheck = "(null)";
		if (password.empty())
			password = "(null)";
		std::cerr << RED "Error: Invalid configuration (Port/Password) -> " << YELLOW << portCheck << " " << password << RESET << std::endl;
	}
}

void	Server::handleSignal(int signal)
{
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>* channels = getChannelsMap();
	int	index;
	bool	*running = getRunning();

	index = 0;
	if (signal == SIGINT || signal == SIGTERM || signal == SIGQUIT)
	{
		std::cout << ORANGE "\nReceived signal to shutdown" RESET << std::endl;
		*running = false;
		while (index < 1024 && fds[index].fd != -1)
		{
			if (index > 0)
			{
				std::map<int, Client*>::iterator it = clients->find(fds[index].fd);
				delete it->second;
			}
			close(fds[index].fd);
			fds[index].fd = -1;
			index++;
		}
		clients->clear();
		index = 0;
		std::map<int, Channel*>::iterator it = channels->find(index);
		while (it != channels->end())
		{

			if (it == channels->end())
				break ;
			delete it->second;
			it++;
		}
		channels->clear();
	}
}

void	Server::init(int port, std::string password)
{
	this->setPassword(password);
	this->setPort(port);

	int	opt = 1;
	
	try {
		sockaddr_in	server;
		int serverIRC = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (serverIRC == -1)
			throw std::exception();

		signal(SIGINT, handleSignal);
		signal(SIGTERM, handleSignal);

		this->setServerIRC(serverIRC);
		fcntl(this->serverIRC, F_SETFL, O_NONBLOCK);

		this->numClients = 1;
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		server.sin_addr.s_addr = INADDR_ANY;

		setsockopt(this->serverIRC, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		if (bind(serverIRC, (struct sockaddr *)&server, sizeof(server)) == 0)
		{
			std::cout << BRIGHT_GREEN "Bind successfully on " << YELLOW "127.0.0.1:" << port << RESET << std::endl;
			if (listen(serverIRC, 1024) == 0)
			{
				std::cout << LIGHT_BLUE "Listen Mode Started =D" << std::endl;
				this->startIRCService();
			}
			else
			{
				std::cerr << RED "Error: The server can't listen anything" RESET << std::endl;
				throw std::exception();
			}
		}
		else
		{
			close(serverIRC);
			std::cerr << RED "Error: Bind failed" RESET << std::endl;
			throw std::exception();
		}
	} catch (std::exception &e) {}
}

void	Server::startPollFds(void)
{
	int	index;
	struct pollfd	(&fds)[1024] = this->getPollFds();

	index = 0;
	while (index < 1024)
	{
		fds[index].fd = -1;
		index++;
	}
}

void	Server::shutdownService(void)
{
	int	index;
	struct	pollfd (&fds)[1024] = this->getPollFds();

	index = 0;
	this->setIsRunning(false);
	while (index < 1024 && fds[index].fd != -1)
	{
		close(fds[index].fd);
		fds[index].fd = -1;
		index++;
	}
}

void	Server::startIRCService(void)
{
	this->startPollFds();
	struct pollfd (&fds)[1024] = *getMyFds();
	int	ready;

	fds[0].fd = getServerIRC();
	fds[0].events = POLLIN;

	while (*this->running)
	{
		ready = poll(fds, this->getNumberOfClients(), -1);

		if (ready < 0)
		{
			if (errno == EINTR)
				continue ;
			this->shutdownService();
		}
		this->PollServerRoom();
		this->PollInputClientMonitoring();
		this->PollOutMonitoring();
	}	
}

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
           if (client->getAuthenticated() && client->getRegistered() && channelTarget == channel)
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

    if (message.empty() || index < 0 || fds[index].fd == -1 || !client->getRegistered() || !client->getAuthenticated() || ownerChannel != channel)
        return ;
    this->sendBuffer[index] += message;
    fds[index].events |= POLLOUT;
}

void	Server::manageBuffers(int index)
{
	this->recvBuffer[index].clear();
	this->recvBuffer[index] = this->recvBuffer[this->numClients - 1];
	this->recvBuffer[this->numClients - 1].clear();
	this->sendBuffer[index].clear();
	this->sendBuffer[index] = this->sendBuffer[this->numClients - 1];
	this->sendBuffer[this->numClients - 1].clear();
}

std::string	Server::getPassword(void) const
{
	return (password);
}

int	Server::getPort(void) const
{
	return (port);
}

void	Server::setPort(int port)
{
	this->port = port;
}

void	Server::setPassword(std::string password)
{
	this->password = password;
}

void	Server::setIsRunning(bool signal)
{
	*this->running = signal;
}

void	Server::setServerIRC(int serverFD)
{
	this->serverIRC = serverFD;
}

int	Server::getServerIRC(void) const
{
	return (serverIRC);
}

struct pollfd	(&Server::getPollFds(void))[1024]
{
	return (*fds);
}

int	Server::getNumberOfClients(void) const
{
	return (numClients);
}
