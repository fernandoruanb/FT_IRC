/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:22:05 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/10 11:49:24 by jopereir         ###   ########.fr       */
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
		//Variables
		int	clientFD;
		int	index;
		bool	authenticated;
		std::string	nickname;
		std::string	username;
		// std::string	realname;
		std::string	sendBuffer;
		std::string	recvBuffer;
		bool		isOperator;
		
		//private methods
		void	clear(void);
		
		//Constructors
		Client(const Client&);
		Client& operator=(const Client&);
	public:
		//Constructor/destructor
		Client(void);
		~Client(void);
		
		//setters
		void	setNickName(const std::string&);
		void	setUserName(const std::string&);
		// void	setRealName(const std::string&);
		void	setAuthenticated(bool);
		void	setClientFD(int);
		void	setRecvBuffer(const std::string&);
		void	setSendBuffer(const std::string&);

		//Getters
		int		getClientFD(void) const;
		bool	getAuthenticated(void) const;
		bool	getIsOperator(void) const;
		int		getIndex(void) const;
		
		const std::string&	getNickName(void) const;
		const std::string&	getUserName(void) const;
		// const std::string&	getRealName(void) const;
		const std::string&	getSendBuffer(void) const;
		const std::string&	getRecvBuffer(void) const;

		//methods
		void	addNewClient(void);
		void	removeClient(void);
		bool	hasDataToSend(void);
};

std::ostream& operator<<(std::ostream&, const Client&);

#endif /* CLIENT_HPP */
