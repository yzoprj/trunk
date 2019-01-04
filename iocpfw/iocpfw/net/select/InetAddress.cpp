#include "InetAddress.h"
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
InetAddress::InetAddress(int ip, int port)
	:_ip(ip), _port(port)

{
	memset(_ipName, 0, IP_NAME_LENGTH);
//	inet_ntop(AF_INET, &_ip, _ipName, IP_NAME_LENGTH);
}

InetAddress::InetAddress(char *ip, int port)
{
	if (ip != NULL)
	{
		strcpy(_ipName, ip);
		_port = port;
		_ip = inet_addr(ip);
	}else
	{
		memset(_ipName, 0, IP_NAME_LENGTH);
	}

}

InetAddress::InetAddress()
	:_ip(0), _port(0)
{
	memset(_ipName, 0, IP_NAME_LENGTH);
}

InetAddress::~InetAddress()
	
{

}

char * InetAddress::getIPName() const 
{
	return (char *)_ipName;
}

unsigned short InetAddress::getIp() const
{
	return _ip;
}

unsigned short InetAddress::getPort() const
{
	return _port;
}
