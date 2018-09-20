#include "AsyncSocket.h"
#include <WinSock2.h>
#include <Windows.h>



AsyncSocket::AsyncSocket(void)
{
	_handle = INVALID_SOCKET;
	_ci.hostAddress = 0;
	_ci.port = 0;
	_ci.sockMode = SM_Null;
	_ci.connType = CT_Socket;
}

AsyncSocket::AsyncSocket(unsigned int handle, SocketMode sockMode /* = SM_TCP */)
{
	_ci.sockMode = sockMode;
	setHandle(handle);
}

AsyncSocket::~AsyncSocket(void)
{
	//closesocket((SOCKET)_handle);
}

void AsyncSocket::close()
{
	if (_handle != -1)
	{
		closesocket((SOCKET)_handle);
		_handle = -1;
	}
	
	
}


int AsyncSocket::connectRemote(const CommunicatioInformation &ci, bool isAsync)
{
	close();

	_ci = ci;

	
	int result = -1;

	result = init(_ci.sockMode, 3, isAsync);

	if (result == -1)
	{
		return -1;
	}

	if (_ci.sockMode == SM_TCP)
	{
		SOCKADDR_IN  remoteAddr = {0};

		remoteAddr.sin_family = AF_INET;
		remoteAddr.sin_port = htons(_ci.port);
		remoteAddr.sin_addr.S_un.S_addr = _ci.hostAddress;

		result = connect((SOCKET)_handle, (sockaddr *)&remoteAddr, sizeof(remoteAddr));

	}

	return result;
}
void AsyncSocket::setTimeout(int timeout)
{
	timeval tv = {0, timeout * 1000};
	int size = sizeof(timeval);

	setsockopt((SOCKET)_handle, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, size);
	setsockopt((SOCKET)_handle, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, size);
}

bool AsyncSocket::bindPort(int port)
{
	SOCKADDR_IN sockAddr = {0};
	int size = sizeof(SOCKADDR_IN);

	sockAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	
	int result =  bind((SOCKET)_handle, (SOCKADDR *)&sockAddr, size);

	if (result != 0)
	{
		return false;
	}
	getSockInfo(false);
	return true;
}

bool AsyncSocket::init(SocketMode sm,int timeout /* = 3 */, bool isAsync /* = true */, bool isRemote /* = true */)
{
	bool result = true;
	int flag = 1;

	if (sm == SM_TCP)
	{
		_handle = (unsigned int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}else
	{
		_handle = (unsigned int)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
	

	if (_handle == -1)
	{
		return false;
	}

	//setTimeout(timeout);
	if (isAsync == true)
	{
		flag = ioctlsocket((SOCKET)_handle, FIONBIO, (u_long *)&flag);
	}
	
	_ci.sockMode = sm;
	getSockInfo(isRemote);
	return true;
	

}



void AsyncSocket::getSockInfo(bool isRemote /* = true */)
{
	SOCKADDR_IN addr;
	int size = sizeof(addr);

	if (isRemote == true)
	{
		getpeername((SOCKET)_handle, (sockaddr *)&addr, &size);
	}else
	{
		getsockname((SOCKET)_handle, (sockaddr *)&addr, &size);
	}
	

	_ci.port = ntohs(addr.sin_port);
	//strcpy(_ipAddr, inet_ntoa(addr.sin_addr));
	_ci.hostAddress = addr.sin_addr.S_un.S_addr;
}

void AsyncSocket::setCommunicationInfomation(const CommunicatioInformation &ci)
{
	_ci = ci;
}

CommunicatioInformation AsyncSocket::getCommunicationInformation() const
{
	return _ci;
}


void AsyncSocket::setHandle(unsigned int handle)
{
	 _handle = handle; 

	 if (_ci.sockMode == SM_TCP)
	 {
		  getSockInfo();
	 }
	
}