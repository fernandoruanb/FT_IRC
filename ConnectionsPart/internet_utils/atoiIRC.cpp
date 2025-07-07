/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atoiIRC.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@42sp.org.br>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 16:15:03 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/04 18:08:57 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../internet.hpp"

int	atoiIRC(const char *port, int *err)
{
	long	result;
	int	index;

	result = 0;
	index = 0;
	while (port[index] >= '0' && port[index] <= '9')
	{
		result *= 10;
		result += port[index] - '0';
		if (result > 65535)
		{
			*err = 1;
			return (0);
		}
		index++;
	}
	return (result);
}
