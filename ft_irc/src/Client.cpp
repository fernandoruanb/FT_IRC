/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:06:24 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/20 14:28:57 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include <iostream>
#include <ostream>
#include "../includes/colours.hpp"

Client::Client(int fd)
	: channelOfTime(0), clientFD(fd), authenticated(false), registered(false), isOperator(false), nickname("*"), username("*"), host("localhost")
{
	std::cout << LIGHT_BLUE << "Constructor Client* Called in fd: " MAGENTA << fd << RESET << std::endl;
}

Client::~Client(void)
{
	std::cout << LIGHT_BLUE "Destructor called for Client *" RESET << std::endl;
}

std::set<std::string>&  Client::getInviteChannels(void)
{
	return (inviteChannels);
}

int	Client::getChannelOfTime(void) const
{
	return (channelOfTime);
}

void	Client::setChannelOfTime(int channel)
{
	this->channelOfTime = channel;
}

void	Client::setNickName(std::string nickname)
{
	this->nickname = nickname;
}

void	Client::setUserName(std::string username)
{
	this->username = username;
}

void	Client::setRealName(std::string	realname)
{
	this->realname = realname;
}

void	Client::setAuthenticated(bool authenticated)
{
	this->authenticated = authenticated;
}

void	Client::setClientFD(int clientFD)
{
	this->clientFD = clientFD;
}

void	Client::setRegistered(bool registered)
{
    this->registered = registered;
}

void	Client::setIsOperator(bool isOperator)
{
    this->isOperator = isOperator;
}

std::set<std::string>&	Client::getChannelsSet(void)
{
	return (channels);
}

void	Client::setHost(std::string host)
{
    this->host = host;
}

std::string	Client::getNickName(void) const
{
	return (nickname);
}

int	Client::getClientFD(void) const
{
	return (clientFD);
}

bool	Client::getAuthenticated(void) const
{
	return (authenticated);
}

std::string	Client::getUserName(void) const
{
	return (username);
}

std::string	Client::getRealName(void) const
{
	return (realname);
}

std::string&	Client::getBufferIn(void)
{
	return (bufferIn);
}

std::string&	Client::getBufferOut(void)
{
	return (bufferOut);
}

bool	Client::getIsOperator(void) const
{
	return (isOperator);
}

bool	Client::getRegistered(void) const
{
    return (registered);
}

std::string	Client::getHost(void) const
{
    return (host);
}

std::set<std::string>&	Client::getOperatorChannels(void)
{
	return (operatorChannels);
}

std::ostream& operator<<(std::ostream &out, const Client &other)
{
    out << YELLOW "Client("
		<< ORANGE "fd: " CYAN << other.getClientFD()
		<< ORANGE ", nick: " CYAN << other.getNickName()
        << ORANGE ", user: " CYAN << other.getUserName()
        << ORANGE ", host: " CYAN << other.getHost()
		<< ORANGE ", authenticated: " CYAN << (other.getAuthenticated() ? "true" : "false")
        << ORANGE ", registered: " CYAN << (other.getRegistered() ? "true" : "false")
        << ORANGE ", op: " CYAN << (other.getIsOperator() ? "true" : "false")
        << ")";
    return out;
}

void			Client::setServerName(const std::string& name)
{
	this->serverName = name;
}
const std::string&	Client::getServerName(void) const
{
	return (this->serverName);
}

void			Client::setMode(const std::string& c)
{
	mode = c;
}
const std::string&		Client::getMode(void) const
{
	return (mode);
}
