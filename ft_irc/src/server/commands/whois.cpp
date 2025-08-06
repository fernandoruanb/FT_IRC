/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   whois.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@42sp.org.br>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 15:12:40 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/08/06 15:49:09 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void	Server::whois(s_commands& com)
{
	if (com.args.size() < 1)
	{
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 461 " + com.client->getNickName() + " WHOIS :Not enough parameters\r\n";
		return ;
	}
	std::string	clientName = com.args[com.args.size() - 1];
	std::map<int, Client*>* clients = getClientsMap();
	std::string	client = com.args[com.args.size() - 1];
	int	clientFD = getClientsFdByName(clientName);
	
	if (clientFD == -1)
	{
		com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 401 " + com.client->getNickName() + " " + com.args[com.args.size() - 1] + " :No such nick/channel\r\n";
		return ;
	}
	com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 311 " + com.client->getNickName() + " " + (*clients)[clientFD]->getNickName() + " " + (*clients)[clientFD]->getUserName() + " " + (*clients)[clientFD]->getHost() + " * :" + (*clients)[clientFD]->getRealName() + "\r\n";
	com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 312 " + com.client->getNickName() + " " + (*clients)[clientFD]->getNickName() + " " + SERVER_NAME + " :The best IRC Server of the world\r\n";
	com.client->getBufferOut() += std::string(":") + SERVER_NAME + " 318 " + com.client->getNickName() + " " + com.args[com.args.size() - 1] + " :End of /WHOIS list\r\n";
}
