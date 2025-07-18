/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 10:50:11 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/17 15:23:12 by jopereir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

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
