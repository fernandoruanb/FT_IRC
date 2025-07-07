/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manageBuffers.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@42sp.org.br>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 18:11:34 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/06 18:11:37 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../internet.hpp"

void	manageBuffers(int index)
{
	t_server	*ircserver = getServer();

	ircserver->recvBuffer[index] = ircserver->recvBuffer[ircserver->nclFD - 1];
	ircserver->recvBuffer[ircserver->nclFD - 1].clear();
	ircserver->sendBuffer[index] = ircserver->sendBuffer[ircserver->nclFD - 1];
	ircserver->sendBuffer[ircserver->nclFD - 1].clear();
}
