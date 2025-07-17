#include <string>
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