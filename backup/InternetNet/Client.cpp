/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:06:24 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/09 16:06:24 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int clientFD): clientFD(clientFD)
{
	std::cout << LIGHT_BLUE << "Constructor Client* Called" << RESET << std::endl;
}

Client::~Client(void)
{
	std::cout << LIGHT_BLUE "Destructor called for Client *" RESET << std::endl;
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
