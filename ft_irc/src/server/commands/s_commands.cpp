#include "../includes/Server.hpp"

s_commands::s_commands(std::string &l, std::map<int, Client*>* &c, int f, int i, std::string &a, std::string& com, std::string &buf)
	: sendBuffer(buf), line(l), clients(c), fd(f), index(i)
{
	isOnline = true;
	command = com;
	client = NULL;
	std::map<int, Client*>::iterator it = clients->find(fd);
	if (it != clients->end())
		client = it->second;
	
	if (a.empty() || a[0] == '\n' || a[0] == '\r')
		return;
	size_t	start = 0;
	size_t	j;
	for (j = 0; j < a.size(); j++)
		if (a[j] == ' ' || a[j] == '\n' || a[j] == '\r' || a[j] == ',')
		{
			if (j > start)
				args.push_back(a.substr(start, j - start));
			start = j + 1;
		}
	if (start < a.size())
	{
		while (a[j] && a[j] != '\n' && a[j] != '\r')
			j++;
		args.push_back(a.substr(start, j));
	}
}
