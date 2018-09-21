 #include "IOCP.h"
#include <WinSock.h>
#include <Windows.h>

IOCPManager::IOCPManager()
{

}

void IOCPManager::initializeSocketLibrary()
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
	_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	return true;
}


bool IOCPManager::initializeListenSocket()
{
	// AcceptEx 和 GetAcceptExSockaddrs 的GUID，用于导出函数指针
	GUID guidAcceptEx = WSAID_ACCEPTEX;  
	GUID guidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 

	struct sockaddr_in serverAddress;
	
	_listenContext = new SocketContext;
	
	_listenContext->sockId = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (_listenContext->sockId == INVALID_SOCKET)
	{
		return false;
	}

	if (CreateIoCompletionPort((HANDLE)_listenContext->sockId, _IOCPHandle,(ULONG_PTR)_listenContext, 0) == NULL)
	{
		_listenContext->close();
		return false;
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.S_un.S_addr = inet_addr(LISTEN_IP);
	serverAddress.sin_port = htons(LISTEN_PORT);

	if (bind(_listenContext->sockId, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		_listenContext->close();
		return false;
	}

	if (listen(_listenContext->sockId, SOMAXCONN) == SOCKET_ERROR)
	{
		_listenContext->close();
		return false;
	}

	DWORD dwBytes = 0;

	int result = WSAIoctl(_listenContext->sockId,
						SIO_GET_EXTENSION_FUNCTION_POINTER,
						&guidAcceptEx,
						sizeof(guidAcceptEx),
						&_lpfnAcceptEx,
						sizeof(_lpfnAcceptEx),
						&dwBytes,
						NULL,
						NULL);
		if(result == -1)
		{
			this->deinitialize();
			return false;
		}

		result = WSAIoctl(_listenContext->sockId,
							SIO_GET_EXTENSION_FUNCTION_POINTER,
							&guidGetAcceptExSockAddrs,
							sizeof(guidGetAcceptExSockAddrs),
							_lpfnAcceptExSockAddress,
							sizeof(_lpfnAcceptExSockAddress),
							&dwBytes,
							NULL,
							NULL);

		if (result == -1)
		{
			return false;
		}

		for (int i = 0; i < MAX_POST_ACCEPT; i++)
		{
			SocketContext *ctx =  _acceptSocketManager.getNewContext();

			if (postAccept(ctx) == false)
			{
				_acceptSocketManager.removeContext(ctx);
				return false;
			}
		}


	return true;
}


bool IOCPManager::postAccept(SocketContext *context)
{
	if (_listenContext->sockId == INVALID_SOCKET)
	{
		return false;
	}


	DWORD dwBytes = 0;
	context->recvOverLapped.opType = AcceptOperation;

	context->sockId = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);


	if (context->sockId == INVALID_SOCKET)
	{
		return false;
	}

	context->sendOverLapped.clearBuffer();

	int result = _lpfnAcceptEx(_listenContext->sockId,
							context->sockId,
							context->recvOverLapped.wsaBuf.buf,
							context->recvOverLapped.wsaBuf.len - ((sizeof(SOCKADDR_IN)+16)*2),
							sizeof(SOCKADDR_IN) + 16,
							sizeof(SOCKADDR_IN) + 16,
							&dwBytes,
							(OVERLAPPED *)&context->recvOverLapped);
	if (result == FALSE)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			return false;
		}
	}



	return true;
}


bool IOCPManager::postRecv(SocketContext *context)
{
	DWORD dwFlgas = 0;
	DWORD dwBytes = 0;

	WSABUF *buf = &context->recvOverLapped.wsaBuf;
	OVERLAPPED *ol = (OVERLAPPED *)&context->recvOverLapped;

	context->clear();
	context->recvOverLapped.opType = RecvOperation;
	context->recvOverLapped.clearBuffer();

	int result = WSARecv(context->sockId, buf, 1, &dwBytes, &dwFlgas, ol, NULL);

	if ((result == SOCKET_ERROR) && (GetLastError() != WSA_IO_PENDING) )
	{
		return false;
	}

	return true;
}


bool IOCPManager::postSend(SocketContext *context)
{
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;

	context->sendOverLapped.opType = RecvOperation;

	int result = WSASend(context->sockId, &context->sendOverLapped.wsaBuf, 
							1, &dwBytes, dwFlags, (OVERLAPPED *)&context->sendOverLapped, NULL);

	if ((result == SOCKET_ERROR) && (GetLastError() != WSA_IO_PENDING) )
	{
		return false;
	}

	return true;
}


bool IOCPManager::handleAccept(SocketContext *context)
{
	if (context->sendOverLapped.totalBytes > 0)
	{
		handleFirstRecvWithData(context);
	}else
	{
		handleFirstRecvWithoutData(context);
	}

	context->clear();


	return postSend(context);
}


bool IOCPManager::handleRecv(SocketContext *context)
{

	return true;
}


bool IOCPManager::handleSend(SocketContext *context)
{

	return true;
}


bool IOCPManager::bindWithIOCP(SocketContext *context)
{
	HANDLE handle = CreateIoCompletionPort((HANDLE)context->sockId, _IOCPHandle, (ULONG_PTR)&context, 0);
	if (handle == NULL)
	{
		return false;
	}

	return true;
}


void IOCPManager::deinitialize()
{

}

bool IOCPManager::handleFirstRecvWithData(SocketContext *context)
{
	SOCKADDR_IN *clientAddr = NULL;
	SOCKADDR_IN *localAddr = NULL;

	int clientLength = sizeof(SOCKADDR_IN);
	int localLength = sizeof(SOCKADDR_IN);

	_lpfnAcceptExSockAddress(context->recvOverLapped.wsaBuf.buf, 
							context->recvOverLapped.wsaBuf.len - ((clientLength + 16) * 2),
							clientLength + 16,
							clientLength + 16,
							(sockaddr **)&localAddr,
							&localLength,
							(sockaddr **)&clientAddr,
							&clientLength);



	SocketContext *newContext = _clientManager.getNewContext();
	newContext->isAcceptable = true;
	newContext->sockId = context->sockId;
	memcpy(newContext->recvOverLapped.buffer, context->recvOverLapped.buffer, MAX_BUFFER_LENGTH);
	memcpy(&newContext->sockAddr,&clientAddr, sizeof(SOCKADDR_IN));

	
	bindWithIOCP(newContext);

	newContext->recvOverLapped.opType = RecvOperation;
	
	if (this->postRecv(newContext) == false)
	{
		return false;
	}

	return false;

}

bool IOCPManager::handleFirstRecvWithoutData(SocketContext *context)
{
	SocketContext *newContext = _clientManager.getNewContext();

	SOCKADDR_IN clientAddr = {0};
	int len = sizeof(SOCKADDR_IN);

	getpeername(context->sockId, (sockaddr *)&clientAddr, &len);

	newContext->sockId = context->sockId;
	memcpy(&context->sockAddr, &clientAddr, len);

	bindWithIOCP(newContext);

	newContext->recvOverLapped.opType = RecvOperation;

	if (this->postRecv(newContext) == false)
	{
		return false;
	}

	return true;
}


void IOCPManager::run()
{

}
