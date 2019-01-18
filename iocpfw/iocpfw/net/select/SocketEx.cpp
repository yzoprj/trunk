#include "SocketEx.h"
#include <WinSock2.h>
#include <Windows.h>
#include <errno.h>
#include "InetAddress.h"
//#include <QDebug>

SocketEx::SocketEx(bool isCreated /* = false */)
{
	_connected = 0;
	_localAddress = NULL;
	_remoteAddress = NULL;
	_local = true;
	_handle = INVALID_SOCKET;

	if (isCreated == true)
	{
		_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		setValid(true);
	}
}

SocketEx::SocketEx(unsigned int handle)
{
	_localAddress = NULL;
	_remoteAddress = NULL;
	_handle = handle;
	_local = true;
	if (_handle != INVALID_SOCKET)
	{
		_connected = 1;
	}else
	{
		_connected = 0;
	}
}

SocketEx::SocketEx(const SocketEx &sock)
{
	setSocket(sock);

}

SocketEx::SocketEx(SocketEx &&sock)
{
	_connected = sock._connected;
	_handle = sock._handle;
	_localAddress = sock._localAddress;
	_remoteAddress = sock._remoteAddress;
	_local = sock._local;
	
	sock._localAddress = NULL;
	sock._remoteAddress = NULL;
	sock._handle = INVALID_SOCKET;
	sock._connected = 0;

}

SocketEx::~SocketEx()
{
	//if (_localAddress != NULL)
	//{
	//	delete _localAddress;
	//}

	//if (_remoteAddress  != NULL)
	//{
	//	delete _remoteAddress;
	//}
	
	this->close();
}

bool SocketEx::setNoBlock(bool blocked /*= false*/)
{
	int flag = ioctlsocket((SOCKET)_handle, FIONBIO, (u_long *)&flag);

	if (flag == 0)
	{
		return true;
	}
	return false;
}

bool SocketEx::bind(int port, bool blocked /*= true*/)
{
	 return bind(InetAddress(0, port), blocked);
}

bool SocketEx::bind(const InetAddress &addr, bool blocked /*= true*/)
{
	int flag = 1;
	SOCKADDR_IN sockAddr = {0};
	int size = sizeof(SOCKADDR_IN);

	sockAddr.sin_addr.S_un.S_addr = addr.getNetIP();
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = addr.getNetPort();

	//close();

	if (_local == false)
	{
		return false;
	}

	if (_localAddress == NULL)
	{
		_localAddress = new InetAddress(addr.getIP(), addr.getPort());
	}else
	{
		_localAddress->setIP(addr.getIP());
		_localAddress->setPort(addr.getPort());
	}

	if (_handle == INVALID_SOCKET)
	{
		_handle = (unsigned int)::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		setValid(true);
	}

	if (blocked == false)
	{
		flag = ioctlsocket((SOCKET)_handle, FIONBIO, (u_long *)&flag);
	}

	if(::bind((SOCKET)_handle, (sockaddr *)&sockAddr, size) != 0)
	{
		return false;
	}


	
	return true;

}

bool SocketEx::connect(const InetAddress &addr, bool blockd /*= true*/)
{
	close();

	SOCKADDR_IN  remoteAddr = {0};

	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = addr.getNetPort();
	remoteAddr.sin_addr.S_un.S_addr = addr.getNetIP();

	_handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_handle == INVALID_SOCKET)
	{
		return false;
	}


	if (blockd)
	{
		setNoBlock();
	}

	setLocal(false);


	if (_remoteAddress == NULL)
	{
		_remoteAddress = new InetAddress(addr);
	}else
	{
		_remoteAddress->setIP(addr.getIP());
		_remoteAddress->setPort(addr.getPort());
	}

	int result = ::connect((SOCKET)_handle, (sockaddr *)&remoteAddr, sizeof(remoteAddr));

	if (result == 0)
	{
		setValid(true);
	}else if (result < 0)
	{
		int errorCode = WSAGetLastError();
		int erroCode = errno;
		if (errorCode == 10035)
		{
			return true;
		}
	}

	return false;
}

void SocketEx::setValid(bool flag)
{
	if (flag == true)
	{
		InterlockedIncrement(&_connected);
	}else
	{
		InterlockedDecrement(&_connected);
	}
}

bool SocketEx::isValidSocket() const
{
	return _connected == 0 ? false : true;



}

bool SocketEx::isLocalSocket() const
{
	return _local;
}

void SocketEx::setTimeout(int sendTimeout /* = 3 */, int recvTimeout /* = 3 */)
{
	timeval tv1 = {0, sendTimeout * 1000};
	timeval tv2 = {0, recvTimeout * 1000};
	int size = sizeof(timeval);

	setsockopt((SOCKET)_handle, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv1, size);
	setsockopt((SOCKET)_handle, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv2, size);
}

void SocketEx::setLocal(bool isLocal)
{
	_local = isLocal;
}

InetAddress SocketEx::getRemoteAddress() const
{
	getSocketInformation(true);
	return _remoteAddress == NULL ?  *_remoteAddress : InetAddress(0, 0);
}

InetAddress SocketEx::getLocalAddress() const
{
	getSocketInformation(false);
	return _localAddress == NULL ? *_localAddress : InetAddress(0, 0);
}

unsigned int SocketEx::getSocketHandle() const 
{
	return _handle;
}

void SocketEx::close()
{
	if (_connected != 0)
	{
		if (_handle != INVALID_SOCKET)
		{
			shutdown(_handle, SD_BOTH);
			closesocket(_handle);
		}

	}

	if (_localAddress != NULL)
	{
		delete _localAddress;
		_localAddress = NULL;
	}

	if (_remoteAddress != NULL)
	{
		delete _remoteAddress;
		_remoteAddress = NULL;
	}

	setValid(false);


}

void SocketEx::getSocketInformation(bool isRemote /* = false */) const 
{
	SOCKADDR_IN addr;
	int size = sizeof(addr);

	if (isRemote == true)
	{
		if (_local == true)
		{
			_remoteAddress = new InetAddress(0, 0);
		}

		if (_remoteAddress == NULL)
		{
			
			if(!getpeername((SOCKET)_handle, (sockaddr *)&addr, &size))
			{
				_remoteAddress = new InetAddress(addr.sin_addr.S_un.S_addr, addr.sin_port);
			}
			
			
		}
	}else
	{
		if (_localAddress == NULL)
		{

			if (!getsockname((SOCKET)_handle, (sockaddr *)&addr, &size))
			{
				_localAddress = new InetAddress(addr.sin_addr.S_un.S_addr, addr.sin_port);
			}
			

		}
	}
}

void SocketEx::setSocket(const SocketEx &sock)
{
	if (_localAddress != NULL)
	{
		delete _localAddress;
	}

	if (_remoteAddress != NULL)
	{
		delete _remoteAddress;
	}

	if (sock._remoteAddress != NULL)
	{
		_remoteAddress = new InetAddress(*sock._remoteAddress);
	}

	if (sock._localAddress != NULL)
	{
		_localAddress = new InetAddress(*sock._localAddress);
	}

	_handle = sock._handle;
	_connected = sock._connected;
	_local = sock._local;
	const_cast<SocketEx *>(&sock)->setValid(false);
}

SocketEx & SocketEx::operator=(const SocketEx &sock)
{
	setSocket(sock);

	return *this;
}
