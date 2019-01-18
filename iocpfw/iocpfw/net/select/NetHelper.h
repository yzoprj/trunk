#ifndef NETHELPER_H
#define NETHELPER_H

#include <string>
using std::string;

class InetAddress;
class NetHelper
{
public:
	static string generateNetKey(const InetAddress &inet);
	static string generateNetIP(unsigned long ip);
private:
	NetHelper();
	~NetHelper();
};

#endif // NETHELPER_H
