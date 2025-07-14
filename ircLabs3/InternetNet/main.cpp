/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@42sp.org.br>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 10:50:11 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/08 15:32:24 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << LIGHT_BLUE "How to use: <ircserver> <port> <password>" RESET << std::endl;
		return (1);
	}
	else
		Server(argv[1], argv[2]);
	return (0);
}
