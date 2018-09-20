#include "IOCP.h"
#include <Windows.h>

IOCPManager::IOCPManager()
{

}

void IOCPManager::deinitializeSocketLibrary()
{
	WSADATA wsa;

	WSAStartup(MAKEWORD(2,2), &wsa);


}


void IOCPManager::deinitializeSocketLibrary()
{
	WSACleanup();
}


bool IOCPManager::initializeIOCP()
{
	_IOCPHandle = CreateIoCompletionPort(NULL, NULL, NULL, 0);
	return true;
}


bool IOCPManager::initializeListenSocket()
{

	return true;
}


bool IOCPManager::postAccept(SocketContext *context)
{


	return true;
}


bool IOCPManager::postRecv(SocketContext *context)
{

	return true;
}


bool IOCPManager::postSend(SocketContext *context)
{

	return true;
}


bool IOCPManager::handleAccept(SocketContext *context)
{

	return true;
}


bool IOCPManager::handleRecv(SocketContext *context)
{

	return true;
}


bool IOCPManager::handleSend(SocketContext *context)
{

}


bool IOCPManager::bindWithIOCP(SocketContext *context)
{

}


bool IOCPManager::handleFirstRecvWithData(SocketContext *context)
{

}

bool IOCPManager::handleFirstRecvWithoutData(SocketContext *context)
{

}
