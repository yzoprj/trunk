#include "AcceptorEx.h"
#include <WinSock2.h>
#include <windows.h>



AcceptorEx::~AcceptorEx()
{

}

bool AcceptorEx::listen(const InetAddress &inet, int timeout /*= -1*/)
{
	if (bind(inet, false) == false)
	{
		return false;
	}

	if (timeout > 0)
	{
		setTimeout(timeout, timeout);
	}

	int result = ::listen((SOCKET)_handle, 64);

	if (result != 0)
	{
		return false;
	}

	return true;
}

SocketEx AcceptorEx::accept()
{
	SOCKET sock;
	SOCKADDR_IN addr;
	int size = sizeof(addr);
	sock = ::accept((SOCKET)_handle, (sockaddr *)&addr, &size);

	return SocketEx(sock);
}

AcceptorEx::AcceptorEx()
	:SocketEx(true)
{

}

AcceptorEx::AcceptorEx(const AcceptorEx &ex)
{

}

AcceptorEx & AcceptorEx::operator=(const AcceptorEx &ex)
{

	return *this;
}
