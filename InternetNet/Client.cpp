/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nasser <nasser@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:06:24 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/09 23:28:59 by nasser           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <iostream>
#include <ostream>

Client::Client(int fd)
	: clientFD(fd), authenticated(false), registered(false), isOperator(false)
{
}

Client::~Client(void)
{
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

const std::set<std::string>&	Client::getChannels(void) const
{
    return channels;
}
void	Client::removeChannel(std::string channel)
{
	channels.erase(channel);
}
std::ostream& operator<<(std::ostream &out, const Client &other)
{
    out << "Client(nick: " << other.getNickName()
        << ", user: " << other.getUserName()
        << ", fd: " << other.getClientFD()
        << ", host: " << other.getHost()
        << ", op: " << other.getIsOperator()
        << ", registered: " << other.getRegistered()
        << ")";
    return out;
}