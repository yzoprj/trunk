#ifndef SOCKETEX_H
#define SOCKETEX_H

class InetAddress;


class SocketEx
{
public:
	SocketEx(bool isCreated = false);
	
	SocketEx(unsigned int handle);
	
	SocketEx(const SocketEx &sock);
	
	SocketEx(SocketEx &&sock);

	SocketEx &operator=(const SocketEx &sock);
	
	virtual ~SocketEx();

	bool setNoBlock(bool blocked = false);

	bool bind(int port, bool blocked = true);

	bool bind(const InetAddress &addr, bool blocked = true);

	bool connect(const InetAddress &addr, bool blockd = true);

	bool isValidSocket();

	void setTimeout(int sendTimeout = 3, int recvTimeout = 3);

	InetAddress getRemoteAddress() const;

	InetAddress getLocalAddress() const ;

	unsigned int getSocketHandle() const ;

	void close();

protected:
	void getSocketInformation(bool isRemote = false) const;
	void setSocket(const SocketEx &sock);
	void setValid(bool flag);
protected:
	unsigned int _handle;

	mutable InetAddress *_remoteAddress;
	mutable InetAddress *_localAddress;

	long _connected;
};

#endif // SOCKETEX_H
