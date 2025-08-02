#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/messages.hpp"
#include <cstring>


bool	Server::checkName(std::string Name)
{
	Name = getLower(Name);
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Channel*>::iterator it = channels->find(0);
	std::string	channelName;
	int	index;

	index = 1;
	while (index < this->numChannels && it != channels->end())
	{
		channelName = it->second->getName();
		if (channelName == Name)
			return (false);
		it = channels->find(index);
		index++;
	}
	return (true);
}

int	Server::findGoodIndex(void)
{
	std::map<int, Channel *>* channels = getChannelsMap();
	std::map<int, Channel *>::iterator it;
	int	index;

	index = 1;
	it = channels->find(index);
	while (index < 1024 && it != channels->end())
	{
		index++;
		it = channels->find(index);
	}
	return (index);
}

static bool	checkChannelName(std::string name)
{
	name = getLower(name);
	const char	*temp = name.c_str();

	while (*temp)
	{
		if (*temp == ',' || *temp == ' ' || *temp == '\a')
			return (false);
		++temp;
	}
	return (true);
}

std::string	getLower(const std::string& str)
{
	std::string lower;

	for (size_t i = 0; i < str.size(); i++)
		lower += std::tolower(static_cast<unsigned char>(str[i]));

	return (lower);
}

void	Server::createNewChannel(std::string Name, int clientFD)
{
	Name = getLower(Name);
	Channel* channel = new Channel(Name);
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Client*>::iterator it = clients->find(clientFD);
	int	index;

	if (numChannels == 1024)
	{
		std::cerr << RED "Error: There are too many channels!!!" RESET << std::endl;
		delete channel;
		return ;
	}

	if (!checkChannelName(Name))
	{
		std::cerr << RED "Error: The channel name cannot have comma, space and bell character" RESET << std::endl;
		delete channel;
		return ;
	}

	if (it == clients->end())
	{
		std::cerr << RED "Error: A ghost can't become an admin of a channel" RESET << std::endl;
		delete channel;
		return ;
	}
	Client* client = it->second;
	if (!this->checkName(Name))
	{
		std::cerr << RED "Error: The new channel name is in use" RESET << std::endl;
		delete channel;
		return ;
	}
	index = this->findGoodIndex();
	(*channels)[index] = channel;
	std::cout << LIGHT_BLUE "The index of the channel " << YELLOW << Name << LIGHT_BLUE " is " << YELLOW << index << RESET << std::endl;
	this->numChannels++;
	client->setIsOperator(true);
	client->getOperatorChannels().insert(Name);
	client->getInviteChannels().insert(Name);
	client->getChannelsSet().insert(Name);
	client->addMode('o', index);
	(*channels)[index]->addNewMember(clientFD);
        (*channels)[index]->getOperatorsSet().insert(clientFD);
        (*channels)[index]->getMembersSet().erase(clientFD);
	std::cout << LIGHT_BLUE "Client " << YELLOW << clientFD << LIGHT_BLUE " is now the operator of " << YELLOW << Name << LIGHT_BLUE " Channel" RESET << std::endl;
	changeChannel(Name, clientFD, 1);
}


void	Server::promotionChannelOperator(std::string channel, int owner, int clientFD)
{
	channel = getLower(channel);
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itch;
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Channel*>::iterator itm;
	int	channelIndex;
	int	index;
	int	second;

	channelIndex = getChannelsIndex(channel);
	if (channelIndex == -1)
	{
		std::cerr << RED "Error: The channel doesn't exist" RESET << std::endl;
		return ;
	}
	index = getChannelsIndex(channel);
	if (index == -1)
	{
		std::cerr << RED "The channel " << YELLOW << channel << RED " doesn't exist" << RESET << std::endl;
		return ;
	}
	index = getClientsIndex(owner);
	second = getClientsIndex(clientFD);
	if (index == -1 || second == -1)
	{
		std::cerr << RED "The owner or client doesn't exist to be promote to an operator" RESET << std::endl;
		return ;
	}
	itch = clients->find(clientFD);
	if (itch->second->getOperatorChannels().find(channel) != itch->second->getOperatorChannels().end())
	{
		(*clients)[clientFD]->getOperatorChannels().insert(channel);
		(*clients)[clientFD]->getChannelsSet().insert(channel);
		(*clients)[clientFD]->setIsOperator(true);
		itm = channels->find(channelIndex);
		itm->second->getOperatorsSet().insert(clientFD);
		itm->second->getMembersSet().erase(clientFD);
		std::cout << LIGHT_BLUE "The client " << YELLOW << clientFD << LIGHT_BLUE "is now an operator of the channel " << YELLOW << channel << RESET << std::endl;
		return ;
	}
	std::cerr << RED "The client " << YELLOW << clientFD << RED " can't be promote to an operator of channel " << YELLOW << channel << RESET << std::endl;
}

void	Server::inviteToChannel(std::string channelName, int operatorFD, int clientFD)
{
	channelName = getLower(channelName);
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Client*>* clients = getClientsMap();
	struct pollfd (&fds)[1024] = *getMyFds();
	std::map<int, Channel*>::iterator itch = channels->begin();
	std::map<int, Client*>::iterator itc = clients->find(operatorFD);
	std::map<int, Client*>::iterator operatorOwner = clients->find(operatorFD);
	int	channelIndex;
	int	clientIndex;
	std::string	nick;
	std::string	user;
	std::string	host;
	std::string	target;
	int	index = 0;

	if (itc == clients->end())
	{
		std::cerr << RED "Error: The operator doesn't exist. It's a ghost!" RESET << std::endl;
		return ;
	}
	nick = itc->second->getNickName();
	if (!itc->second->getIsOperator())
	{
		std::cerr << RED "Error: There is someone trying to invite other people without enough permissions" RESET << std::endl;
		itc->second->getBufferOut() += msg_err_chanoprivsneeded(nick, channelName, "You are not an operator");
		clientIndex = getClientsIndex(itc->first);
		fds[clientIndex].events |= POLLOUT;
		return ;
	}
	channelIndex = getChannelsIndex(channelName);
	if (channelIndex == -1)
	{
		std::cerr << RED "Error: The channel doesn't exist to invite someone to it" RESET << std::endl;
		itc->second->getBufferOut() += msg_err_nosuchchannel(itc->second->getNickName(), channelName);
		clientIndex = getClientsIndex(itc->first);
		fds[clientIndex].events |= POLLOUT;
		return ;
	}
	if (itc->second->getOperatorChannels().find(channelName) == itc->second->getOperatorChannels().end())
	{
		std::cerr << RED "Error: The operator is a valid operator but not from that channel" RESET << std::endl;
		itc->second->getBufferOut() += msg_err_chanoprivsneeded(nick, channelName, "You are not an operator of that channel");
		clientIndex = getClientsIndex(itc->first);
		fds[clientIndex].events |= POLLOUT;
		return ;
	}
	itc = clients->find(clientFD);
	if (itc == clients->end())
	{
		std::cerr << RED "Error: The clientFD doesn't exist. It's a ghost!" RESET << std::endl;
		return ;
	}
	while (itch != channels->end())
	{
		if (itch->second->getName() == channelName)
			break ;
		++itch;
	}
	if (itch == channels->end())
	{
		int	clientIndex;

		clientIndex = getClientsIndex(itc->first);
		std::cerr << RED "Error: The channel doesn't exist. It's a ghost!!!" RESET << std::endl;
		itc->second->getBufferOut() += msg_err_nosuchchannel(itc->second->getNickName(), channelName);
		fds[clientIndex].events |= POLLOUT;
		return ;
	}
	itch->second->setInviteFlag(true);
	itc->second->getInviteChannels().insert(channelName);
	index = getClientsIndex(itc->first);
	nick = operatorOwner->second->getNickName();
	user = operatorOwner->second->getUserName();
	host = operatorOwner->second->getHost();
	target = itc->second->getNickName();
	itc->second->getBufferOut() += my_invite_message(nick, user, host, target, channelName);
	fds[index].events |= POLLOUT;
	std::cout << LIGHT_BLUE "The client " << YELLOW << clientFD << LIGHT_BLUE " received an invite to " << YELLOW << channelName << LIGHT_BLUE " channel by " << YELLOW << operatorFD << std::endl;
}

void	Server::changeTopic(std::string channelName, int clientFD, std::string topic)
{
	channelName = getLower(channelName);
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Channel*>::iterator it = channels->begin();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itc = clients->find(clientFD);
	struct pollfd (&fds)[1024] = *getMyFds();
	int	isOperator;
	int	messageTarget;
	bool	theKing;
	int	clientIndex;
	std::string	nick;
	std::string	user;
	std::string	host;
	std::string	time;
	time_t	timestamp = std::time(0);
	std::ostringstream	oss;

	while (it != channels->end())
	{
		it++;
		if (it != channels->end())
		{
			if (it->second->getName() != channelName)
				continue ;
			else
				break ;
		}
	}
	if (itc == clients->end())
	{
		std::cerr << RED "Error: A ghost can't change the channel topic" RESET << std::endl;
		return ;
	}
	clientIndex = getClientsIndex(itc->first);
	if (it == channels->end())
	{
		std::cerr << RED "Error: The channel doesn't exist to change topic" RESET << std::endl;
		itc->second->getBufferOut() += msg_err_nosuchchannel(nick, channelName);
		fds[clientIndex].events |= POLLOUT;
		return ;
	}
	nick = itc->second->getNickName();
	isOperator = itc->second->getIsOperator();
	theKing = this->kingsOfIRC.find(itc->first) != this->kingsOfIRC.end();
	if (it->second->getTopicFlag())
	{
		if (!theKing && !isOperator)
		{
			std::cerr << RED "Error: The client isn't a true operator to do privileged action" RESET << std::endl;
			itc->second->getBufferOut() += msg_err_chanoprivsneeded(nick, channelName, "You are not an operator");
			fds[clientIndex].events |= POLLOUT;
			return ;
		}
		if (!theKing && itc->second->getOperatorChannels().find(channelName) == itc->second->getOperatorChannels().end())
		{
			std::cerr << RED "Error: The client is an operator but not from that channel" RESET << std::endl;
			itc->second->getBufferOut() += msg_err_chanoprivsneeded(nick, channelName, "You are not an operator of that channel");
			fds[clientIndex].events |= POLLOUT;
			return ;
		}
	}
	if (!theKing && it->first != itc->second->getChannelOfTime())
	{
		itc->second->getBufferOut() += msg_err_notonchannel(nick, channelName);
		std::cerr << RED "Error: You can't change a topic in another channel" RESET << std::endl;
		fds[clientIndex].events |= POLLOUT;
		return ;
	}
	oss << timestamp;
	it->second->setTopic(topic);
	time = oss.str();
	it->second->setTimeStamp(time);
	nick = itc->second->getNickName();
	user = itc->second->getUserName();
	host = itc->second->getHost();
	it->second->setOwnerTopic(nick);
	std::cout << LIGHT_BLUE "The topic of the channel " << YELLOW << it->second->getName() << LIGHT_BLUE " changed to " << YELLOW << topic << RESET << std::endl;
	messageTarget = getClientsIndex(clientFD);
	itc->second->getBufferOut() += my_topic_message(nick, user, host, channelName, topic);
	fds[messageTarget].events |= POLLOUT;
}


void	Server::changeChannelInviteFlag(std::string channel, bool flag)
{
	channel = getLower(channel);
	std::map<int, Channel*>* channels = getChannelsMap();
	int	index;

	index = getChannelsIndex(channel);
	if (index == -1)
	{
		std::cerr << RED "Error: impossible to set the invite flag from a ghost channel" RESET << std::endl;
		return ;
	}
	(*channels)[index]->setInviteFlag(flag);
	std::cout << BRIGHT_GREEN "The channel " << YELLOW << channel << BRIGHT_GREEN " only accepts new clients if they have an invite right now" << RESET << std::endl;
}

int	Server::getChannelsIndex(std::string channel)
{
	channel = getLower(channel);
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Channel*>::iterator itch = channels->begin();

	while (itch != channels->end())
	{
		if (itch->second->getName() == channel)
			return (itch->first);
		++itch;
	}
	return (-1);
}


void	Server::kickFromChannel(std::string channel, int owner, int clientFD, std::string message)
{
	channel = getLower(channel);
	std::map<int, Channel *>* channels = getChannelsMap();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itch = clients->find(owner);
	std::map<int, Client*>::iterator own = clients->find(owner);
	std::map<int, Channel*>::iterator itm;
	std::string	channelName;
	int	clientIndex;
	int	channelOfTime;
	bool	theKing;
	struct pollfd (&fds)[1024] = *getMyFds();
	bool	isOperator;
	std::string	nick;
	std::string	user;
	std::string	host;
	std::string	target;
	int	messageTarget = 0;

	if (itch == clients->end())
	{
		std::cerr << RED "Error: There is a ghost trying to kick someone!!!" RESET << std::endl;
		return ;
	}
	clientIndex = getClientsIndex(itch->first);
	nick = itch->second->getNickName();
	theKing = this->kingsOfIRC.find(itch->first) != this->kingsOfIRC.end();
	isOperator = itch->second->getIsOperator();
	if (!theKing && !isOperator)
	{
		std::cerr << RED "Error: The owner isn't a true operator of the channel " << YELLOW << channel << RED " to kick someone" RESET << std::endl;
		itch->second->getBufferOut() += msg_err_chanoprivsneeded(nick, channel, "You are not an operator");
		fds[clientIndex].events |= POLLOUT;
		return ;
	}
	channelOfTime = itch->second->getChannelOfTime();
	itm = channels->find(channelOfTime);
	if (itm == channels->end())
	{
		std::cerr << RED "Error: It's impossible to kick someone from a ghost channel" RESET << std::endl;
		itch->second->getBufferOut() += msg_err_nosuchchannel(nick, channel);
		fds[clientIndex].events |= POLLOUT;
		return ;
	}
	if (!theKing && channel != itm->second->getName())
	{
		std::cerr << RED "Error: Your current channel isn't the target channel dear owner" RESET << std::endl;
		return ;
	}
	if (!theKing && itch->second->getOperatorChannels().find(channel) == itch->second->getOperatorChannels().end())
	{
		std::cerr << RED "Error: You are an operator but not from the target channel" RESET << std::endl;
		itch->second->getBufferOut() += msg_err_chanoprivsneeded(nick, channel, "You are not an operator of that channel");
		fds[clientIndex].events |= POLLOUT;
		return ;
	}
	itch = clients->find(clientFD);
	if (itch == clients->end())
	{
		std::cerr << RED "Error: The target client to kick doesn't exist" RESET << std::endl;
		return ;
	}
	if (itch->second->getChannelsSet().find(channel) == itch->second->getChannelsSet().end())
	{
		std::cerr << RED "Error: The client is not in the target channel " << YELLOW << channel << RESET << std::endl;
		return ;
	}
	nick = own->second->getNickName();
	user = own->second->getUserName();
	host = own->second->getHost();
	target = itch->second->getNickName();
	itm->second->removeMember(itch->first);
	std::cout << "kick " << itm->second->getMembersNum() << std::endl;
	if (itm->second->getMembersNum() == 0)
		deleteChannel(itm->second->getName(), itch->first);
	else
	{
		itch->second->getOperatorChannels().erase(channel);
		itch->second->getChannelsSet().erase(channel);
		itch->second->getInviteChannels().erase(channel);
		itm->second->getOperatorsSet().erase(itch->first);
		itm->second->getMembersSet().erase(itch->first);
		if (itch->second->getOperatorChannels().size() == 0)
			itch->second->setIsOperator(false);
		this->changeChannel("generic", itch->first, 0);
	}
	std::cout << LIGHT_BLUE "The client " << YELLOW << clientFD << LIGHT_BLUE " has been kicked by " << YELLOW << owner << LIGHT_BLUE " and lost all privileges coming back to " << YELLOW "generic" << LIGHT_BLUE " Channel" RESET << std::endl;
	messageTarget = getClientsIndex(clientFD);
	itch->second->getBufferOut() += my_kick_message(nick, user, host, message, target, channel);
	fds[messageTarget].events |= POLLOUT;
}

void	Server::removeOperatorPrivilegesFromEveryBody(std::string channel)
{
	channel = getLower(channel);
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Client*>::iterator it = clients->begin();
	std::map<int, Channel*>::iterator itm = channels->begin();
	int	channelOfTime;

	while (itm != channels->end())
	{
		if (channel == itm->second->getName())
			break ;
		itm++;
	}
	if (itm == channels->end())
	{
		std::cerr << RED "The channel to remove extra things doesn't exist" RESET << std::endl;
		return ;
	}
	while (it != clients->end())
	{
		it->second->getOperatorChannels().erase(channel);
		it->second->getChannelsSet().erase(channel);
		it->second->getInviteChannels().erase(channel);
		itm->second->removeMember(it->first);
		itm->second->getOperatorsSet().erase(it->first);
		itm->second->getMembersSet().erase(it->first);
		if (it->second->getOperatorChannels().size() == 0)
			it->second->setIsOperator(false);
		channelOfTime = it->second->getChannelOfTime();
		if (channelOfTime == itm->first)
			changeChannel("generic", it->second->getClientFD(), 0);
		this->kingsOfIRC.erase(it->first);
		it++;
	}
	std::cout << BRIGHT_GREEN "The channel " << ORANGE << channel << BRIGHT_GREEN " was cleaned successfully" RESET << std::endl;
}

bool	Server::AuthenticationKeyProcess(const std::string channel, const std::string key)
{
	std::map<int, Channel*>* channels = getChannelsMap();
	std::string	password;
	int	index;

	index = getChannelsIndex(channel);
	password = (*channels)[index]->getPassWord();

	if (key == password)
		return (true);
	return (false);
}

void	Server::deleteChannel(std::string channel, int clientFD)
{
	channel = getLower(channel);
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itch = clients->find(clientFD);
	std::string	channelName;
	struct pollfd (&fds)[1024] = *getMyFds();
	int	channelOfTime;
	bool	isOperator;
	std::string	nick;
	int	index;

	if (itch == clients->end())
	{
		std::cerr << RED "Error: Impossible to remove a channel because the client is a ghost" RESET << std::endl;
		return ;
	}
	nick = itch->second->getNickName();
	isOperator = itch->second->getIsOperator();
	if (!isOperator)
	{
		std::cerr << RED "Error: The client " << YELLOW << clientFD << RED " isn't an operator" RESET << std::endl;
		itch->second->getBufferOut() += msg_err_chanoprivsneeded(nick, channel, "You are not an operator");
		fds[itch->first].events |= POLLOUT;
		return ;
	}
	if (itch->second->getOperatorChannels().find(channel) == itch->second->getOperatorChannels().end())
	{
		std::cerr << RED "Error: The client isn't a valid operator of the channel " << YELLOW << channel << RESET << std::endl;
		itch->second->getBufferOut() += msg_err_chanoprivsneeded(nick, channel, "You are not an operator of that channel");
		fds[itch->first].events |= POLLOUT;
		return ;
	}
	std::map<int, Channel*>::iterator itc = channels->begin();
	while (itc != channels->end())
	{
		channelName = itc->second->getName();
		if (channelName == channel)
		{
			channelOfTime = itch->second->getChannelOfTime();
			if (channelOfTime == itch->first)
			{
				std::cout << LIGHT_BLUE "Changing to " << YELLOW << "generic" << LIGHT_BLUE " Channel client " << YELLOW << clientFD << RESET << std::endl;
				this->changeChannel("generic", itch->second->getClientFD(), 1);
			}
			this->removeOperatorPrivilegesFromEveryBody(channelName);
			delete itc->second;
			channels->erase(itc);
			itch->second->getChannelsSet().erase(channelName);
			std::cout << LIGHT_BLUE "Channel " << YELLOW << channelName << LIGHT_BLUE << " removed successfully" RESET << std::endl;
			numChannels--;
			itch->second->getOperatorChannels().erase(channelName);
			if (itch->second->getOperatorChannels().size() == 0)
			{
				itch->second->setIsOperator(false);
				std::cout << LIGHT_BLUE "The Client " << YELLOW << clientFD << LIGHT_BLUE " lost the operator privileges about channel " << YELLOW << channelName << RESET << std::endl;
			}
			return ;
		}
		itc++;
	}
	std::cerr << RED "Error: The channel " << YELLOW << channel << RED " doesn't exist" RESET << std::endl;
}

void	Server::changeChannel(std::string channel, int clientFD, bool flag)
{
	channel = getLower(channel);
	std::map<int, Client*>* clients = getClientsMap();
	std::map<int, Client*>::iterator itc = clients->find(clientFD);
	struct pollfd (&fds)[1024] = *getMyFds();
	std::string	nick;
	std::string	user;
	std::string	host;
	std::string	ownerTopic;
	std::string	message;
	std::string	time;
	std::string	topic;
	bool	theKing = this->kingsOfIRC.find(clientFD) != this->kingsOfIRC.end();
	int	clientIndex;
	int	channelIndex;
	int	messageTarget = 0;
	if (itc == clients->end())
	{
		std::cerr << RED "Error: The client is a ghost trying to changing a channel" RESET << std::endl;
		return ;
	}
	clientIndex = getClientsIndex(clientFD);
	fds[clientIndex].events |= POLLOUT;
	Client* client = itc->second;
	std::map<int, Channel*>* channels = getChannelsMap();
	std::map<int, Channel*>::iterator itm;
	std::map<int, Channel*>::iterator last;
	std::string	channelName;

	if (!client)
	{
		std::cerr << RED "Error: the client to change channel is a ghost" RESET << std::endl;
		return ;
	}
	itm = channels->begin();
	while (itm != channels->end())
	{
		channelName = itm->second->getName();
		if (channelName == channel)
		{
			channelIndex = getChannelsIndex(channel);
			if (itc->second->getChannelOfTime() == channelIndex && flag != 1)
			{
				std::cerr << RED "Error: You are trying to change to the same channel that you are" RESET << std::endl;
				itc->second->getBufferOut() += std::string(":") + SERVER_NAME + " 443 " + itc->second->getNickName() + " " + itc->second->getNickName() + " #" + channelName + " :is already on channel" + "\r\n";
				fds[clientIndex].events |= POLLOUT;
				return ;
			}
			last = channels->find(itc->second->getChannelOfTime());
			Channel* channelOfficial = itm->second;
			if (itm->second->getInviteFlag())
			{
				std::cout << BRIGHT_GREEN << "The channel " << YELLOW << channelName << BRIGHT_GREEN " needs invite flags to change to it." << RESET << std::endl;
				if (!theKing && itc->second->getInviteChannels().find(channelName) == itc->second->getInviteChannels().end())
				{
					std::cerr << RED "Error: The client " << YELLOW << itc->first << RED " doesn't have the invite necessary to change to this channel " << YELLOW << channelName <<  RESET << std::endl;
					return ;
				}
			}
			if (!theKing && itm->second->getMembersNum() >= itm->second->getUserLimit())
			{
				std::cerr << RED "Error: The channel userlimit is full!!!" RESET << std::endl;
				itc->second->getBufferOut() += msg_err_channelisfull(nick, channel);
				fds[itc->first].events |= POLLOUT;
				return ;
			}
			std::cout << LIGHT_BLUE "Client " << YELLOW << clientFD << LIGHT_BLUE " changing to " << YELLOW << channelOfficial->getName() << RESET << std::endl;
			client->setChannelOfTime(itm->first);
			itm->second->addNewMember(clientFD);
			itm->second->getMembersSet().insert(clientFD);
			client->getChannelsSet().insert(channel);
			messageTarget = getClientsIndex(itc->first);
			nick = client->getNickName();
			user = client->getUserName();
			host = client->getHost();
			ownerTopic = itm->second->getOwnerTopic();
			time = itm->second->getTimeStamp();
			topic = itm->second->getTopic();
			message = "You left the channel";
			if (flag != 1)
				client->getBufferOut() += my_part_message(nick, user, host, last->second->getName(), message);
			client->getBufferOut() += my_join_message(nick, user, host, channel);
			client->getBufferOut() += my_join_rpl_topic(nick, channel, topic);
			if (!time.empty())
			{
				if (nick == "system")
				{
					ownerTopic = "*";
					user = "*";
					host = "localhost";
				}
				client->getBufferOut() += my_join_rpl_topic_whotime(nick, ownerTopic, user, host, channel, time);
			}
			client->getBufferOut() += my_join_rpl_namreply(nick, channel);
			client->getBufferOut()  += itm->second->getOperatorsNames();
			client->getBufferOut() += itm->second->getClientsNames() + "\r\n";
			client->getBufferOut() += my_join_rpl_endofnames(nick, channel);
			fds[messageTarget].events |= POLLOUT;
			return ;
		}
		itm++;
	}
	createNewChannel(channel, clientFD);
}
