/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:36:43 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/09 16:36:43 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

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

void	Channel::setUserLimit(int userlimit)
{
	this->userlimit = userlimit;
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
	return (void);
}

std::string	Channel::getInviteFlag(void) const
{
	return (inviteFlag);
}

std::string	Channel::getTopicFlag(void) const
{
	return (topicFlag);
}
