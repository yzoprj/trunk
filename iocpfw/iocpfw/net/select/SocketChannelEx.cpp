#include "SocketChannelEx.h"
#include <WinSock2.h>
#include <WinSock.h>
#include <Windows.h>
#include <errno.h>
#include "SocketEx.h"

SocketChannelEx::SocketChannelEx()
{

}

SocketChannelEx::SocketChannelEx(shared_ptr<SocketEx> &sock)
{
	_sock = sock;
}

SocketChannelEx::~SocketChannelEx()
{

}

bool SocketChannelEx::read(vector<char> &buffer)
{
	int readBytes = 0;
	int result = 0;
	unsigned long nBytes = 0;
	result = ioctlsocket((SOCKET)_sock->getSocketHandle(), FIONREAD, &nBytes);

	if (result != 0)
	{
		return false;
	}

	if (nBytes == 0)
	{
		return true;
	}

	char *tmpBuf = new char[nBytes];

	while((result = recv((SOCKET)_sock->getSocketHandle(), tmpBuf + readBytes, 
		nBytes - readBytes, 0)) > 0)
	{
		readBytes += result;
	}

	if(result == 0)
	{
		delete tmpBuf;
		return false;
	}else if (result == -1)
	{
		if (errno != EWOULDBLOCK 
			|| errno != EAGAIN
			|| errno != EINTR)
		{
			delete tmpBuf;
			return false;
		}
	}

	if (readBytes > 0)
	{
		buffer.insert(buffer.end(), tmpBuf, tmpBuf + readBytes);
	}


	delete tmpBuf;
}

bool SocketChannelEx::write(const vector<char> &buffer, int &outBytes)
{
	int result = send((SOCKET)_sock->getSocketHandle(), (char *)buffer.data(), buffer.size(), 0);

	if (result < 0)
	{
		outBytes = 0;
		return false;
	}

	outBytes = result;
	return true;
}

void SocketChannelEx::setSocket(shared_ptr<SocketEx> &sock)
{
	_sock = sock;
}
