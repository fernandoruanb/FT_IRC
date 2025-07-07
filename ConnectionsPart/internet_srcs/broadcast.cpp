/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   broadcast.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@42sp.org.br>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 12:09:31 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/06 21:06:57 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../internet.hpp"

void	broadcast(const int sender)
{
	t_server	*ircserver = getServer();
	nfds_t	index;
	ssize_t	bytes;
	std::string	temp;

	index = 1;
	bytes = 1;

	if (!ircserver->sendBuffer[sender].empty())
	{
		while (index < ircserver->nclFD)
		{
			if (ircserver->fds[index].fd == ircserver->fds[sender].fd)
			{
				index++;
				continue ;
			}
			ircserver->sendBuffer[index] += ircserver->sendBuffer[sender];
			ircserver->fds[index].events |= POLLOUT;
			index++;
		}
		ircserver->sendBuffer[sender].clear();
	}
}
