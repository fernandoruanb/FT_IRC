/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:36:43 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/18 11:55:25 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

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
}

Channel::Channel(std::string name): name(name), members(0), userlimit(1024), topic("We love IRC"), inviteFlag(false), topicFlag(false)
{
	time_t	time = std::time(0);
	std::ostringstream	oss;

	oss << time;
	this->setTimeStamp(oss.str());
	this->setOwnerTopic("System");
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
