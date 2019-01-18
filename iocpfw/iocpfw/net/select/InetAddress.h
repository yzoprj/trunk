#ifndef INETADDRESS_H
#define INETADDRESS_H

class InetAddress
{
	static const int IP_NAME_LENGTH  = 32;
public:

	InetAddress();
	InetAddress(int ip, int port);
	InetAddress(const char *ip, int port);
	~InetAddress();

	void setIP(const char *ip);
	void setIP(int ip);
	void setPort(int port);

	 char *getIPName() const ;
	 unsigned int getIP() const ;
	 unsigned short getPort() const ;

	 unsigned int getNetIP() const;
	 unsigned short getNetPort() const;
private:
	unsigned int _ip;
	unsigned short _port;
	char _ipName[IP_NAME_LENGTH];
};

#endif // INETADDRESS_H
