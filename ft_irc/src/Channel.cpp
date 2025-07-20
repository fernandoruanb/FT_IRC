/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:36:43 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/20 17:23:49 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

std::string	Channel::getOperatorsNames(void)
{
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itc;
	std::set<int> operators = this->getOperatorsSet();
	std::set<int>::iterator it = operators.begin();

	this->operatorsNames.clear();

	while (it != operators.end())
	{
		itc = clients->find(*it);
		if (itc == clients->end())
		{
			++it;
			continue ;
		}
		this->operatorsNames += std::string("@") + itc->second->getNickName() + " ";
		this->getMembersSet().erase(itc->first);
		++it;
	}
	return (this->operatorsNames);
}

std::string	Channel::getClientsNames(void)
{
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itc;
	std::set<int> clientsName = this->getMembersSet();
	std::set<int>::iterator it = clientsName.begin();

	this->clientsNames.clear();

	while (it != clientsName.end())
	{
		itc = clients->find(*it);
		if (itc == clients->end())
		{
			++it;
			continue ;
		}
		this->clientsNames += std::string(itc->second->getNickName()) + " ";
		++it;
	}
	return (this->clientsNames);
}

std::set<int>&	Channel::getMembersSet(void)
{
	return (members);
}

std::set<int>&	Channel::getOperatorsSet(void)
{
	return (operators);
}

int		Channel::getUserLimit(void) const
{
	return (userLimit);
}

int		Channel::getMembersNum(void) const
{
	return (membersNum);
}

std::string	Channel::getTimeStamp(void) const
{
	return (timestamp);
}

std::string	Channel::getOwnerTopic(void) const
{
	return (ownerTopic);
}

void	Channel::setTimeStamp(std::string time)
{
	this->timestamp = time;
}

void	Channel::setOwnerTopic(std::string nick)
{
	this->ownerTopic = nick;
}

void	Channel::addNewMember(int clientFD)
{
	std::cout << LIGHT_BLUE "Client " << YELLOW << clientFD << LIGHT_BLUE " added to " << YELLOW << this->name << LIGHT_BLUE " Channel" RESET << std::endl;
	this->members.insert(clientFD);
	++membersNum;
}

void	Channel::removeMember(int clientFD)
{
	std::cout << LIGHT_BLUE "Client " << YELLOW << clientFD << LIGHT_BLUE " removed from " << YELLOW << this->name << LIGHT_BLUE " Channel" RESET << std::endl;
	this->members.erase(clientFD);
	--membersNum;
}

Channel::Channel(std::string name): name(name), topic("We love IRC"), userLimit(1024), membersNum(0), inviteFlag(false), topicFlag(false)
{
	time_t	time = std::time(0);
	std::ostringstream	oss;

	oss << time;
	this->setTimeStamp(oss.str());
	this->setOwnerTopic("system");
	std::cout << LIGHT_BLUE "Channel " << YELLOW << name << LIGHT_BLUE << " created =D" << RESET << std::endl;
}

Channel::~Channel(void) {}

void	Channel::setName(std::string name)
{
	this->name = name;
}

void	Channel::setTopic(std::string topic)
{
	this->topic = topic;
}

void	Channel::setPassWord(std::string password)
{
	this->password = password;
}

void	Channel::setChannelIndex(int index)
{
	this->index = index;
}

int	Channel::getChannelIndex(void) const
{
	return (index);
}

void	Channel::setUserLimit(int userlimit)
{
	this->userLimit = userlimit;
}

void	Channel::setInviteFlag(bool inviteflag)
{
	this->inviteFlag = inviteflag;
}

void	Channel::setTopicFlag(bool topicflag)
{
	this->topicFlag = topicflag;	
}

std::string	Channel::getName(void) const
{
	return (name);
}

std::string	Channel::getTopic(void) const
{
	return (topic);
}

std::string	Channel::getPassWord(void) const
{
	return (password);
}

bool	Channel::getInviteFlag(void) const
{
	return (inviteFlag);
}

bool	Channel::getTopicFlag(void) const
{
	return (topicFlag);
}

void	Channel::setMode(const std::string& mod)
{
	this->mode = mod;
}
const std::string&	Channel::getMode(void) const
{
	return (mode);
}
