#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/messages.hpp"

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

void	Server::user(std::map<int, Client*>* clients, std::string& buffer, int fd, int i)
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
	
	// if (myClient->getRegistered())
	// 	return;

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
		// this->sendBuffer[i] += msg_notice("Welcome " + myClient->getUserName() + "!");
		this->sendBuffer[i] += msg_welcome(myClient->getUserName());
		myClient->setRegistered(true);
	}
	fds[i].events |= POLLOUT;
}
