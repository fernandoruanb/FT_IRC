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

void	Server::user(s_commands	&commands)
{
	bool	optional = false;

	//	Find the client to register
	std::map<int, Client*>::iterator	it = commands.clients->find(commands.fd);
	struct	pollfd						(&fds)[1024] = *getMyFds();

	// (*clients)[fd];
	if (it == commands.clients->end())
		return;

	//	Start to register
	Client*	myClient = it->second;
	
	// if (myClient->getRegistered())
	// 	return;

	size_t	pos = commands.line.find("USER ");
	if (pos != std::string::npos)
	{
		pos += 5;
		if (!isValidArgs(commands.line, pos, optional))
		{
			this->sendBuffer[commands.index] += msg_notice("USER <username> <hostname> <servername> :<realname>");
			fds[commands.index].events |= POLLOUT;
			return;
		}
		std::string	temp = getText(commands.line, &pos, clients, !myClient->getRegistered());
		if (temp.empty())
		{
			this->sendBuffer[commands.index] += msg_error("Nickname is already in use", 433, commands);
			fds[commands.index].events |= POLLOUT;
			return;
		}
		
		//Seting the client info
		myClient->setUserName(temp);
		if (myClient->getNickName().empty())
			myClient->setNickName(temp);
		myClient->setHost(getText(commands.line, &pos, clients));
		myClient->setServerName(getText(commands.line, &pos, clients));
		if (optional)
			myClient->setRealName(getText(commands.line, &pos, clients));
		this->sendBuffer[commands.index].clear();
		// this->sendBuffer[i] += msg_notice("Welcome " + myClient->getUserName() + "!");
		this->sendBuffer[commands.index] += msg_welcome(myClient->getUserName());
		myClient->setRegistered(true);
	}
	fds[commands.index].events |= POLLOUT;
}
