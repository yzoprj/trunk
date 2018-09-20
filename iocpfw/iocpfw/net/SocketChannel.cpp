#include "SocketChannel.h"
#include <WinSock2.h>
#include <Windows.h>
#include <errno.h>
#include "SessionManager.h"
#include "../thread/SyncObjects.h"


SocketChannel::SocketChannel(void)
{

}


SocketChannel::~SocketChannel(void)
{

}

SocketChannel::SocketChannel(const AsyncSocket &sock)
{
	setSocket(sock);
}


void SocketChannel::setSocket(const AsyncSocket &socket)
{
	_sock = socket;
}


int SocketChannel::handleResultErrorResult(int val)
{

	if (val < 0)
	{
		if (errno == EWOULDBLOCK)
		{
			return SES_ReadNextTime;
		}else if (errno == EINTR)
		{
			return SES_ReadAgain;
		}
	}else if (val == 0)
	{
		return SES_Disconnect;
	}

	return val;
}

int SocketChannel::readByte(ubyte &val)
{
	

	int result = recv((SOCKET)_sock.getHandle(), (char *)&val, 1, 0);

	//if (result < 0)
	//{
	//	if (errno == EWOULDBLOCK)
	//	{
	//		return SES_ReadNextTime;
	//	}else if (errno == EINTR)
	//	{
	//		return SES_ReadAgain;
	//	}
	//}else if (result == 0)
	//{
	//	return SES_Disconnect;
	//}

	result = handleResultErrorResult(result);

	
	return result;
}

int SocketChannel::writeByte(ubyte val)
{
	int result  = send((SOCKET)_sock.getHandle(), (char *)&val, 1, 0);

	return handleResultErrorResult(result);
}


int SocketChannel::writeN(const RawBuffer &rb)
{
	int result = send((SOCKET)_sock.getHandle(), (char *)rb.getData(), rb.getLength(), 0);
	return handleResultErrorResult(result);
}


int SocketChannel::readN(RawBuffer &rb)
{
	int hasRecvCounts = 0;
	char buffer[MAX_BUFFER_LENGTH] = {0};
	int result = 0;

	while ((result = recv((SOCKET)_sock.getHandle(), buffer + hasRecvCounts, MAX_BUFFER_LENGTH - hasRecvCounts, 0)))
	{
		if (result < 0)
		{
			if (errno == EWOULDBLOCK)
			{
				break;
			}else if ( errno == EINTR)
			{
				break;
			}
		}else if (result == 0)
		{
			break;
		}

		hasRecvCounts += result;
		
	}
	result = handleResultErrorResult(result);

	if (result > 0)
	{
		rb.setRawData((unsigned char *)buffer, (unsigned int)hasRecvCounts);
		result = hasRecvCounts;
	}

	return result;
}



int SocketChannel::readByte(ubyte &val, CommunicatioInformation &ci)
{
	SOCKADDR_IN addr = {0};
	int addrSize = sizeof(addr);
	int result = 0;

	result = recvfrom(_sock.getHandle(), (char *)&val, 1, 0, (sockaddr *)&addr, &addrSize);

	if (result > 0)
	{
		ci.setConnectionType(CT_Socket);
		ci.hostAddress = addr.sin_addr.S_un.S_addr;
		ci.port = addr.sin_port;
		ci.setSocketMode(SM_UDP);
	}


	return result;

}

int SocketChannel::writeByte(ubyte val, CommunicatioInformation &ci)
{
	SOCKADDR_IN addr = {0};
	int addrSize = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = ci.port;
	addr.sin_addr.S_un.S_addr = ci.hostAddress;
	
	return sendto(_sock.getHandle(), (char *)&val, 1, 0, (sockaddr *)&addr, addrSize);
}

int SocketChannel::readN(RawBuffer &rb, CommunicatioInformation &ci)
{
	char buffer[MAX_READ_BUFFER_SIZE] = {0};

	SOCKADDR_IN addr = {0};
	int addrSize = sizeof(addr);
	int result = -1;

	result = recvfrom(_sock.getHandle(), buffer, MAX_READ_BUFFER_SIZE, 0, (sockaddr *)&addr, &addrSize);



	if (result > 0)
	{
		rb.setRawData((unsigned char *)buffer, result);
		ci.hostAddress = addr.sin_addr.S_un.S_addr;
		ci.port = addr.sin_port;
		ci.setConnectionType(CT_Socket);
		ci.setSocketMode(SM_UDP);
	}

	return result;

}

int SocketChannel::writeN(const RawBuffer &rb, const CommunicatioInformation &ci)
{
	MutexGuard mg(SessionManager::sockSendMuetx);
	
	SOCKADDR_IN addr = {0};
	int addrSize = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = ci.port;
	addr.sin_addr.S_un.S_addr = ci.hostAddress;

	return sendto(_sock.getHandle(), (char *)rb.getData(), rb.getLength(), 0, (sockaddr *)&addr, addrSize);
}