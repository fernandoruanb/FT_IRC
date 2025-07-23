/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jopereir <jopereir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:22:05 by fruan-ba          #+#    #+#             */
/*   Updated: 2025/07/23 13:08:21 by fruan-ba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <set>

class	Client
{
	private:
		int				channelOfTime;
		int				clientFD;
		int				index;
		bool			authenticated;
		bool			registered;
		bool			isOperator;
		std::string		nickname;
		std::string		username;
		std::string		realname;
		std::string		sendHistory[1024];
		std::string		bufferIn;
		std::string		bufferOut;
		std::string		host;
		std::string		serverName;
		std::string		mode;
		Client(const Client &other);
		Client& operator=(const Client &other);
		std::set<std::string>	operatorChannels;
		std::set<std::string>	inviteChannels;
		std::set<std::string>	channels;
		Client(void);
		void			addChannel(std::string channel);
		void			removeChannel(std::string channel);
	public:
		Client(int clientFD);
		~Client(void);
		void			setChannelOfTime(int channel);
		void			setNickName(std::string nickname);
		void			setUserName(std::string username);
		void			setRealName(std::string realname);
		void			setAuthenticated(bool authenticated);
		void			setClientFD(int clientFD);
		void			setRegistered(bool registered);
		void			setIsOperator(bool isOperator);
		void			setHost(std::string host);
		void			setServerName(const std::string&);
		void			setMode(const std::string&);
		const std::string&		getMode(void) const;
		const std::string	&getServerName(void) const;
		std::set<std::string>&		getChannelsSet(void);
		std::string		getNickName(void) const;
		std::string		getUserName(void) const;
		int				getClientFD(void) const;
		bool			getAuthenticated(void) const;
		std::string		getRealName(void) const;
		int			getChannelOfTime(void) const;
		std::string&	getBufferIn(void);
		std::string&	getBufferOut(void);
		std::set<std::string>&	getOperatorChannels(void);
		std::set<std::string>&	getInviteChannels(void);
		bool			getIsOperator(void) const;
		bool			getRegistered(void) const;
		std::string		getHost(void) const; // nickname!username@host
		std::string     (&getSendHistory(void))[1024];
		std::set<int>&	getChannels(void);
};
std::ostream& operator<<(std::ostream &out, const Client &other);
#endif /* CLIENT_HPP */
