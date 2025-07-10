/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fruan-ba <fruan-ba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:22:05 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/09 15:22:05 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include "Server.hpp"

class	Client
{
	private:
		int	clientFD;
		int	index;
		bool	authenticated;
		std::string	nickname;
		std::string	username;
		std::string	realname;
		std::string	bufferIn;
		std::string	bufferOut;
		bool	isOperator;
		Client(const Client &other);
		Client& operator=(const Client &other);
	public:
		Client(void);
		~Client(void);
		void	setNickName(std::string nickname);
		void	setUserName(std::string username);
		void	setRealName(std::string realname);
		void	setAuthenticated(bool authenticated);
		void	setClientFD(int clientFD);
		int	getClientFD(void) const;
		bool	getAuthenticated(void) const;
		std::string	getNickName(void) const;
		std::string	getUserName(void) const;
		std::string	getRealName(void) const;
		std::string&	getBufferIn(void);
		std::string&	getBufferOut(void);
		bool	getIsOperator(void) const;
};
std::ofstream operator<<(std::ostream &out, const Client &other);
#endif /* CLIENT_HPP */
