#include <string>
#include <sstream>
#include "../includes/Server.hpp"
#include "../includes/colours.hpp"

std::string msg_welcome(Client* &target) {
	std::string	nick = target->getNickName();
    return ( ":" SERVER_NAME " 001 " + nick + " :Welcome to the IRC Network, " + nick + "!" + nick + "@" + target->getHost() + "\r\n" );
}

std::string msg_yourhost(const std::string& nick) {
    return ":" SERVER_NAME " 002 " + nick + " :Your host is " + SERVER_NAME + ", running version " VERSION "\r\n";
}

std::string msg_created(const std::string& nick) {
    return ":" SERVER_NAME " 003 " + nick + " :This server was created " __DATE__ " at " __TIME__ "\r\n";
}

std::string msg_svrinfo(const std::string& nick) {
    return ":" SERVER_NAME " 004 " + nick + " :" SERVER_NAME " " VERSION " o o\r\n";
}

std::string msg_err_needmoreparams(const std::string& nick, const std::string& cmd) {
    return (":" SERVER_NAME " 461 " + nick + " " + cmd + " :Not enough parameters\r\n" );
}

std::string msg_err_passwdmismatch() {
    return (CYAN ":" SERVER_NAME " 464 * " YELLOW ":Password incorrect\r\n" RESET);
}

std::string msg_err_nicknameinuse(const std::string& nick) {
    return ":" SERVER_NAME " 433 * " + nick + " :Nickname is already in use\r\n";
}

std::string msg_err_notregistered() {
    return ":" SERVER_NAME " 451 * :You have not registered\r\n";
}

std::string msg_err_unknowncommand(const std::string& cmd) {
    return ":" SERVER_NAME " 421 * " + cmd + " :Unknown command\r\n";
}

std::string msg_err_nosuchchannel(const std::string& nick, const std::string& channel) {
    return ":" SERVER_NAME " 403 " + nick + " #" + channel + " :No such channel\r\n";
}

std::string msg_err_notonchannel(const std::string& nick, const std::string& channel) {
    return ":" SERVER_NAME " 442 " + nick + " #" + channel + " :You're not on that channel\r\n";
}

std::string msg_err_usernotinchannel(const std::string nick, const std::string& user, const std::string& channel) {
	return ":" SERVER_NAME " 441 * " + nick + " " + user + " " + channel + " :They aren't on that channel\r\n";
}

std::string msg_err_channelisfull(const std::string& nick, const std::string& channel) {
    return ":" SERVER_NAME " 471 " + nick + " #" + channel + " :Cannot join channel (+l)\r\n";
}

std::string msg_err_chanoprivsneeded(const std::string& nick, const std::string& channel, const std::string& message) {
    return ":" SERVER_NAME " 482 " + nick + " #" + channel + " :" + message +"\r\n";
}

std::string msg_err_erroneusnickname(const std::string& nick) {
    return ":" SERVER_NAME " 432 " + nick + " :Erroneous nickname\r\n";
}

std::string msg_err_alreadyregistered(const std::string& nick) {
	return (":" SERVER_NAME " 462 " + nick + " :You may not reregister\r\n");
}

std::string msg_notice(const std::string& text) {
    return (":" SERVER_NAME " NOTICE * : " + text + "\r\n" RESET);
}

std::string	msg_error(const std::string &message, int error_code, s_commands& com)
{
	std::stringstream	ss;

	ss << " " << error_code << " " << com.client->getNickName() << " " << com.command << " :";
	return (":" SERVER_NAME + ss.str() + message + "\r\n");
}
void	callCmdMsg(const std::string &message, int error, s_commands& com, std::string &buffer)
{
	buffer += msg_error(message, error, com);
}

std::string	my_notice_info(const std::string& nickname, const std::string& message)
{
	return (":" SERVER_NAME " NOTICE " + nickname + " :[INFO] " + message + "\r\n");
}

std::string	my_notice_error(const std::string& nickname, const std::string& message)
{
	return (":" SERVER_NAME " NOTICE " + nickname + " :[ERROR] " + message + "\r\n");
}

std::string	my_kick_message(const std::string& nickname, const std::string& username, const std::string& hostname, const std::string& message, const std::string& target, const std::string& channel)
{
	return (":" + nickname + "!" + username + "@" + hostname + " KICK " + "#" + channel + " " + target + " :" + message + "\r\n");
}

std::string	my_invite_message(const std::string& nickname, const std::string& username, const std::string& hostname, const std::string& target, const std::string& channel)
{
	return (":" + nickname + "!" + username + "@" + hostname + " INVITE " + target + " :#" + channel + "\r\n");
}

std::string	my_topic_message(const std::string& nickname, const std::string& username, const std::string& hostname,  const std::string& channel, const std::string& topic)
{
	return (":" + nickname + "!" + username + "@" + hostname + " TOPIC " + "#" + channel + " :" + topic + "\r\n");
}

std::string	my_privmsg_message(const std::string& nickname, const std::string& username, const std::string& hostname, const std::string& target, const std::string& message)
{
	return (":" + nickname + "!" + username + "@" + hostname + " PRIVMSG " + target + ":" + message + "\r\n");
}

std::string	my_join_message(const std::string& nickname, const std::string& username, const std::string& hostname, const std::string& channel)
{
	return (":" + nickname + "!" + username + "@" + hostname + " JOIN " + "#" + channel + "\r\n");
}

std::string	my_join_rpl_topic(const std::string& nickname, const std::string& channel, const std::string& topic)
{
	return (":" SERVER_NAME " 332 " + nickname + " #" + channel + " :" + topic + "\r\n");
}

std::string	my_join_rpl_topic_whotime(const std::string& nick, const std::string& nick2, const std::string& user, const std::string& host, const std::string& channel, std::string& timestamp)
{
	return (":" SERVER_NAME " 333 " + nick + " #" + channel + " " + nick2 + "!" + user + "@" + host + " " + timestamp + "\r\n");
}

std::string	my_join_rpl_namreply(const std::string &nick, const std::string& channel)
{
	return (":" SERVER_NAME " 353 " + nick + " #" + channel + " " + ":");
}

std::string	my_join_rpl_endofnames(const std::string &nick, const std::string& channel)
{
	return (":" SERVER_NAME " 366 " + nick + " #" + channel + " :End of /NAMES list" + "\r\n");
}

std::string	my_part_message(const std::string& nick, const std::string& user, const std::string& host, const std::string& channel,  const std::string& message)
{
	return (":" + nick + "!" + user + "@" + host + " PART " + "#" + channel + " :" + message + "\r\n");
}

std::string	my_useronchannel(const std::string& owner, const std::string& nick, const std::string& channel, const std::string& message)
{
	return (":" SERVER_NAME " 443 " + owner + " " + nick + " #" + channel + " :" + message + "\r\n");
}

std::string	my_notonchannel(const std::string& nick, const std::string& channel, const std::string& message)
{
	return (":" SERVER_NAME " 442 " + nick + " #" + channel + " :" + message + "\r\n");
}

std::string	my_nosuchnickchannel(const std::string& nick, const std::string& target)
{
	return (":" SERVER_NAME " 401 " + nick + " " + target + " :no such nick/channel" + "\r\n");
}

std::string msg_nick_changed(const std::string& nick, const std::string& requestedNick, const std::string& finalNick) {
    return ":" SERVER_NAME " NOTICE " + nick + " :Nickname " + requestedNick + " is already in use. Using " + finalNick + " instead.\r\n";
}

std::string	my_usernotinchannel(std::string& nick, const std::string& target, const std::string& channel, const std::string& message)
{
	return (":" SERVER_NAME " 441 " + nick + " " + target + " #" + channel + " :" + message + "\r\n");
}

std::string msg_err_usersdontmatch(s_commands& com)
{
	return (
		":" SERVER_NAME " 502 "
		+ com.client->getNickName()
		+ " :Cannot change mode for other users\r\n"
	);
}

std::string msg_err_noprivileges(s_commands& com)
{
	return (
		":" SERVER_NAME " 481 "
		+ com.client->getNickName()
		+ " :Permission Denied - You're not an IRC operator\r\n"
	);
}

std::string	msg_err_unknownmode(s_commands&com, char mode)
{
	return (
		":" SERVER_NAME " 472 "
		+ com.client->getNickName()
		+ " "
		+ std::string(1, mode)
		+ " :is unknown mode char to me\r\n"
	);
}

std::string	msg_mode_userwelldone(s_commands& com, Client* &target)
{
	return (
		":"
		+ com.client->getNickName()
		+ " MODE "
		+ target->getNickName()
		+ " "
		+ com.args[1]
		+ "\r\n"
	);
}

std::string	msg_showusermodes(s_commands& com, Client* &target)
{
	return (
		":" SERVER_NAME " 221 "
		+ com.client->getNickName()
		+ " +"
		+ target->getMode(com.client->getChannelOfTime())
		+ "\r\n"
	);
}

std::string	msg_showchannelmodes(s_commands& com, Channel* &target)
{
	return (
		":" SERVER_NAME " 324 "
		+ com.client->getNickName()
		+ " #"
		+ target->getName()
		+ " +"
		+ target->getMode()
		+ "\r\n"
	);
}

std::string	msg_quit(s_commands& com, const std::string &message)
{
	return (
		":"
		+ com.client->getNickName()
		+ "!"
		+ com.client->getUserName()
		+ "@"
		+ com.client->getHost()
		+ " QUIT :"
		+ message
		+ "\r\n"
	);
}

std::string	msg_err_invalidparameter(s_commands& com, Channel*& channel)
{
	return (
		":" SERVER_NAME "696 "
		+ com.client->getNickName()
		+ " "
		+ channel->getName()
		+ " :Invalid user limit\r\n"
	);
}

std::string	msg_353(const std::string& nick, const std::string& channelName, const std::string& names)
{
	return (
		":" SERVER_NAME " 353 "
		+ nick
		+ " = #"
		+ channelName
		+ " :"
		+ names
		+ "\r\n"
	);
}

std::string	msg_366(const std::string& nick, const std::string& channelName)
{
	return (
		":" SERVER_NAME " 366 "
		+ nick
		+ " #"
		+ channelName
		+ " :End of /NAMES list.\r\n"
	);
}

std::string	msg_endlist(const std::string& nick)
{
	return (
		":" SERVER_NAME " 323 "
		+ nick
		+ " :End of /LIST\r\n"
	);
}
