#include "InetAddress.h"
#include <WinSock2.h>
#include <MSWSock.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include "NetHelper.h"


InetAddress::InetAddress(int ip, int port)
	:_ip(ip), _port(port)

{
	memset(_ipName, 0, IP_NAME_LENGTH);

	

	strcpy(_ipName, NetHelper::generateNetIP(ip).c_str());
}


InetAddress::InetAddress(const char *ip, int port)
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

void InetAddress::setIP(int ip)
{
	_ip = ip;
	strcpy(_ipName, NetHelper::generateNetIP(ip).c_str());
}

void InetAddress::setIP(const char *ip)
{
	strcpy(_ipName, ip);
	_ip = inet_addr(ip);
}

void InetAddress::setPort(int port)
{
	_port = port;
}

char * InetAddress::getIPName() const 
{
	return (char *)_ipName;
}

unsigned int InetAddress::getIP() const
{
	return _ip;
}

unsigned short InetAddress::getPort() const
{
	return _port;
}

unsigned int InetAddress::getNetIP() const
{
	return htonl(_ip);
}

unsigned short InetAddress::getNetPort() const
{
	return htons(_port);
}
