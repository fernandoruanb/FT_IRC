#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <unistd.h>
#include <poll.h>

typedef struct	s_server	t_server;
t_server*					getServer(void);

class Client
{
	private:
		std::string			name;
		struct pollfd		fds;
		bool				_isOnline;
		std::string			sendBuffer;
		std::string			recvBuffer;
	public:
		Client(void);
		Client(const Client &);
		Client&	operator=(const Client &);
		~Client(void);

		//Setter
		void	setName(const std::string &);

		//getters
		const bool&				isOnline(void) const;
		const struct pollfd&	getPollfd(void) const;
		const std::string&		getName(void) const;
		const std::string&		getSendBuffer(void) const;
		const std::string&		getRecvBuffer(void) const;

		//methods
		void	addNewClient(void);
		void	removeClient(void);
		bool	hasDataToSend(void);
};

#endif
