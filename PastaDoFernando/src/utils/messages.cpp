#include <string>
#include <sstream>
#include "../includes/Server.hpp"
#include "../includes/colours.hpp"

std::string msg_welcome(const std::string& nick) {
    return ( ":" SERVER_NAME " 001 " + nick + " :Welcome to the IRC Network, " + nick + "!\r\n" );
}

// std::string msg_yourhost(const std::string& nick) {
//     return ":" SERVER_NAME " 002 " + nick + " :Your host is " + SERVER_NAME + ", running version " VERSION "\r\n";
// }

// std::string msg_created() {
//     return ":" SERVER_NAME " 003 * :This server was created " __DATE__ " at " __TIME__ "\r\n";
// }

// std::string msg_svrinfo() {
//     return ":" SERVER_NAME " 004 * " SERVER_NAME " " VERSION " :<http://www.42.fr/>\r\n";
// } 

std::string msg_err_needmoreparams(const std::string& cmd) {
    return (CYAN ":" SERVER_NAME " 461 " + cmd + YELLOW " :Not enough parameters\r\n" RESET);
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

std::string msg_err_nosuchchannel(const std::string& channel) {
    return ":" SERVER_NAME " 403 * " + channel + " :No such channel\r\n";
}

std::string msg_err_notonchannel(const std::string& channel) {
    return ":" SERVER_NAME " 442 * " + channel + " :You're not on that channel\r\n";
}

std::string msg_err_channelisfull(const std::string& channel) {
    return ":" SERVER_NAME " 471 * " + channel + " :Cannot join channel (+l)\r\n";
}

std::string msg_err_chanoprivsneeded(const std::string& channel) {
    return ":" SERVER_NAME " 482 * " + channel + " :You're not channel operator\r\n";
}

std::string msg_err_erroneusnickname(const std::string& nick) {
    return ":" SERVER_NAME " 432 * " + nick + " :Erroneous nickname\r\n";
}

std::string msg_notice(const std::string& text) {
    return (CYAN ":" SERVER_NAME " NOTICE * : " YELLOW  + text + "\r\n" RESET);
}

std::string	msg_error(const std::string &message, int error_code, s_commands& com)
{
	std::stringstream	ss;

	ss << " " << error_code << " " << com.client->getNickName() << " " << com.command << " :";
	return (":" SERVER_NAME + ss.str() + message + "\r\n");
}
void	callCmdMsg(const std::string &message, int error, s_commands& com, std::string &buffer)
{
	buffer.clear();
	buffer = msg_error(message, error, com);
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
	return (":" + nickname + "!" + username + "@" + hostname + " KICK " + "#" + channel + " " + target + " : " + message + "\r\n");
}

std::string	my_invite_message(const std::string& nickname, const std::string& username, const std::string& hostname, const std::string& target, const std::string& channel)
{
	return (":" + nickname + "!" + username + "@" + hostname + " INVITE " + target + " :#" + channel + "\r\n");
}

std::string	my_topic_message(const std::string& nickname, const std::string& username, const std::string& hostname,  const std::string& channel, const std::string& topic)
{
	return (":" + nickname + "!" + username + "@" + hostname + " TOPIC " + "#" + channel + ":new " + topic + "\r\n");
}

std::string	my_privmsg_message(const std::string& nickname, const std::string& username, const std::string& hostname, const std::string& target, const std::string& message)
{
	return (":" + nickname + "!" + username + "@" + hostname + " PRIVMSG " + target + ":" + message + "\r\n");
}

std::string	my_join_message(const std::string& nickname, const std::string& username, const std::string& hostname, const std::string& channel)
{
	return (":" + nickname + "!" + username + "@" + hostname + " JOIN " + " #" + channel + "\r\n");
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
