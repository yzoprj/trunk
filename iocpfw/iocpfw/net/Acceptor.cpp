#include "Acceptor.h"
#include <WinSock2.h>
#include <windows.h>


Acceptor::Acceptor(void)
{
	
}


Acceptor::~Acceptor(void)
{
}


AsyncSocket Acceptor::acceptSession()
{
	SOCKET sock;
	SOCKADDR_IN addr;
	int size = sizeof(addr);
	sock = accept((SOCKET)serverSock.getHandle(), (sockaddr *)&addr, &size);

	return AsyncSocket((unsigned int)sock);
	
}


bool Acceptor::init(int port)
{
	serverSock.init(SM_TCP, 3, false);
	serverSock.bindPort(port);
	int result = listen((SOCKET)serverSock.getHandle(), 64);
	
	
	if (result != 0)
	{
		return false;
	}

	return true;

}