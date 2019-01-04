#ifndef INETADDRESS_H
#define INETADDRESS_H

class InetAddress
{
	static const int IP_NAME_LENGTH  = 32;
public:

	InetAddress();
	InetAddress(int ip, int port);
	InetAddress(char *ip, int port);
	~InetAddress();

	 char *getIPName() const ;
	 unsigned short getIp() const ;
	 unsigned short getPort() const ;
private:
	unsigned int _ip;
	unsigned short _port;
	char _ipName[IP_NAME_LENGTH];
};

#endif // INETADDRESS_H
