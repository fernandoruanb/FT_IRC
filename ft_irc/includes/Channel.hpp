/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:26:36 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/28 17:03:12 by jopereir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <set>
# include <iostream>
# include <string>
# include "Server.hpp"
# include "Client.hpp"

class Client;

class	Channel
{
	private:
		int	index;
		std::string	name;
		std::string	topic;
		std::string	password;
		int	userLimit;
		int	membersNum;
		bool	inviteFlag;
		bool	topicFlag;
		std::string	operatorsNames;
		std::string	clientsNames;
		std::string	ownerTopic;
		std::string	timestamp;
		std::string		mode;
		std::set<int>	members;
		std::set<int>	operators;
		std::set<int>	invited;
		Channel(const Channel &other);
		Channel& operator=(const Channel &other);
	public:
		Channel(std::string name);
		~Channel(void);
		std::string	getOperatorsNames(void);
		std::string	getClientsNames(void);
		std::set<int>&	getOperatorsSet(void);
		std::set<int>&	getMembersSet(void);
		void	removeMember(int clientFD);
		int	getMembersNum(void) const;
		std::string	&getTimeStamp(void);
		std::string	&getOwnerTopic(void);
		void	setOwnerTopic(std::string nick);
		void	setTimeStamp(std::string time);
		void	setChannelIndex(int index);
		int	getChannelIndex(void) const;
		void	setName(std::string name);
		void	setTopic(std::string topic);
		void	setPassWord(std::string password);
		void	setUserLimit(int userlimit);
		void	setInviteFlag(bool inviteflag);
		void	setTopicFlag(bool topicflag);
		void	setMode(const std::string&);
		const std::string&	getMode(void) const;
		std::string	&getName(void);
		std::string	&getTopic(void);
		std::string	&getPassWord(void);
		int	getUserLimit(void) const;
		bool	getInviteFlag(void) const;
		bool	getTopicFlag(void) const;
		void	addNewMember(int clientFD);
		bool	isMemberOfChannel(int fd) const;
		bool	isOperatorOfChannel(int fd) const;
		void	setOperator(int fd);
};
std::ofstream	operator<<(std::ostream &out, const Channel &other);
#endif /* CHANNEL_HPP */
