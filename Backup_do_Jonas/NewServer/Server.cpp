/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 17:19:31 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/09 17:19:31 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	setServerIRC(int serverIRC)
{
	this->serverIRC = serverIRC;
}

void	setPort(int port)
{
	this->port = port;
}

void	setPassWord(std::string password)
{
	this->password = password;
}

int	getServerIRC(void) const
{
	return (serverIRC);
}

std::string	getPassWord(void) const
{
	return (passWord);
}
