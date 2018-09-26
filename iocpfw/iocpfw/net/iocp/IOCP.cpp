 #include "IOCP.h"
#include <WinSock.h>
#include <Windows.h>


string getWindowsErrorMessage(DWORD errCode)
{
	char buffer[1024] = {0};
	char outBuffer [1024] = {0};
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				errCode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)buffer,
				1024, NULL);


	sprintf(outBuffer, "ErrorMessage[%d]:%s", errCode, buffer);

	return outBuffer;

}

IOCPManager::IOCPManager(OperationHandler *opHandler /* = NULL */)
{
	_IOCPHandle = INVALID_HANDLE_VALUE;
	_lpfnAcceptEx = NULL;
	_lpfnAcceptExSockAddress = NULL;
	_lpfnDisconnectEx = NULL;
	
	_clientManager = new SocketContextManager;
	_acceptSocketManager = new SocketContextManager;
	_bufferManager = new IOBufferManager;
	_sendTaskManager = new IOTaskManager;

	if (opHandler == NULL)
	{
		_opHandler = new DefaultOperationHandler();
	}

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


bool IOCPManager::initializeIOCP(int threadCount)
{
	_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, threadCount);
	
	WRITELOG("Initialize IOCP Handle Success!");
	return true;
}


IOCPManager::~IOCPManager()
{

}


bool IOCPManager::initializeListenSocket()
{


	struct sockaddr_in serverAddress;
	
	_listenContext = new SocketContext;
	

	_listenContext->sockId = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (_listenContext->sockId == INVALID_SOCKET)
	{
		WRITELOG("initialize server socket error!!!!");
		return false;
	}

	WRITELOG("Create New Listen Socket Success!");
	
	getWSAFunction();

	if (CreateIoCompletionPort((HANDLE)_listenContext->sockId, _IOCPHandle,(ULONG_PTR)_listenContext, 0) == NULL)
	{
		_listenContext->close();
		WRITELOG("bind IOCP error!!!");
		return false;
	}


	WRITELOG("Bind Listen Socket To IOCP Success!");

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.S_un.S_addr = inet_addr(LISTEN_IP);
	serverAddress.sin_port = htons(LISTEN_PORT);


	
	if (bind(_listenContext->sockId, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		_listenContext->close();
		return false;
	}

	WRITELOG("Listen Socket Bind Success!");

	if (listen(_listenContext->sockId, SOMAXCONN) == SOCKET_ERROR)
	{
		_listenContext->close();
		return false;
	}

	WRITELOG("Listening....................");



	if (postAllAcceptSocket())
	{
		return false;
	}

	WRITELOG("Post All Accepted Socket Success!");


	return true;
}

bool IOCPManager::postAllAcceptSocket()
{
	for (int i = 0; i < MAX_POST_ACCEPT; i++)
	{
		SocketContext *ctx =  _acceptSocketManager->getNewContext();

		if (postAccept(ctx) == false)
		{
			_acceptSocketManager->removeContext(ctx);
			return false;
		}
	}

	return true;
}


bool IOCPManager::getWSAFunction()
{
	// AcceptEx 和 GetAcceptExSockaddrs 的GUID，用于导出函数指针
	GUID guidAcceptEx = WSAID_ACCEPTEX;  
	GUID guidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 
	GUID guidDisConnectEx = WSAID_DISCONNECTEX;
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
		return false;
	}

	WRITELOG("Get AcceptEx Success!");

	result = WSAIoctl(_listenContext->sockId,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidGetAcceptExSockAddrs,
		sizeof(guidGetAcceptExSockAddrs),
		&_lpfnAcceptExSockAddress,
		sizeof(_lpfnAcceptExSockAddress),
		&dwBytes,
		NULL,
		NULL);

	if (result == -1)
	{
		return false;
	}


	WRITELOG("Get GetAcceptExSocketAddress Success!");

	result = WSAIoctl(_listenContext->sockId,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidDisConnectEx,
		sizeof(guidDisConnectEx),
		&_lpfnDisconnectEx,
		sizeof(_lpfnDisconnectEx),
		&dwBytes,
		NULL,
		NULL);

	if (result == -1)
	{
		return false;
	}

	WRITELOG("Get DisconnectEx Success!");

	return true;
}

bool IOCPManager::postAccept(SocketContext *context)
{
	if (_listenContext->sockId == INVALID_SOCKET)
	{
		return false;
	}


	DWORD dwBytes = 0;

	

	context->sockId = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);




	IOContext *ioContext = context->ioContext;
	ioContext->sockId = context->sockId;
	ioContext->opType = AcceptOperation;

	if (context->sockId == INVALID_SOCKET)
	{
		return false;
	}



	//int result = _lpfnAcceptEx(_listenContext->sockId,
	//						context->sockId,
	//						ioContext->wsaBuf.buf,
	//						ioContext->wsaBuf.len - ((sizeof(SOCKADDR_IN)+16)*2),
	//						sizeof(SOCKADDR_IN) + 16,
	//						sizeof(SOCKADDR_IN) + 16,
	//						&dwBytes,
	//						(OVERLAPPED *)ioContext);


	int result = _lpfnAcceptEx(_listenContext->sockId,
							context->sockId,
							ioContext->wsaBuf.buf,
							0,
							sizeof(SOCKADDR_IN) + 16,
							sizeof(SOCKADDR_IN) + 16,
							&dwBytes,
							(OVERLAPPED *)ioContext);

	if (result == FALSE)
	{
		int result = WSAGetLastError();
		
		if (result != WSA_IO_PENDING)
		{
			WRITELOG(getWindowsErrorMessage(result).c_str());
			return false;
		}
	}

	return true;
}


bool IOCPManager::postRecv(SocketContext *context)
{
	DWORD dwFlgas = 0;
	DWORD dwBytes = 0;


	IOContext *ioContext = context->ioContext;

	ioContext->clearBuffer();

	ioContext->sockId = context->sockId;
	ioContext->opType = RecvOperation;

	int result = WSARecv(context->sockId, &ioContext->wsaBuf, 1, &dwBytes, &dwFlgas, &ioContext->overLapped, NULL);

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


	IOTask *task = _sendTaskManager->createNewTask();
	IOContext *ioContext = task->createNewContext();
	ioContext->opType = RecvOperation;
	ioContext->sockId = context->sockId;

	int result = WSASend(context->sockId, &ioContext->wsaBuf, 
							1, &dwBytes, dwFlags, (OVERLAPPED *)&ioContext->overLapped, NULL);

	if ((result == SOCKET_ERROR) && (GetLastError() != WSA_IO_PENDING) )
	{
		return false;
	}

	return true;
}


bool IOCPManager::handleAccept(SocketContext *context, IOContext *ioContext)
{
	
	if (ioContext->overLapped.InternalHigh > 0)
	{
		handleFirstRecvWithData(context, ioContext);
	}else
	{
		handleFirstRecvWithoutData(context, ioContext);
	}

	context->clear();


	return postRecv(context);
}


bool IOCPManager::handleRecv(SocketContext *context, IOContext *ioContext)
{
	// recieve 0 byte means remote host is disconnected
	if (ioContext->overLapped.InternalHigh == 0)
	{
		_bufferManager->removeIoBuffer(context->sockId);
		_clientManager->removeClient(SocketContextManager::getClientName(context));
		
		string str = SocketContextManager::getClientName(context) + " is disconnected!";
		WRITELOG(str.c_str());
		
		return false;
	}

	vector<char > *buffer = context->recvBuff;

	if (buffer->capacity() < buffer->size() + ioContext->overLapped.InternalHigh)
	{
		buffer->reserve(buffer->size() + ioContext->overLapped.InternalHigh + MAX_BUFFER_LENGTH / 8);
	}

	buffer->insert(buffer->begin() + buffer->size(), ioContext->wsaBuf.buf, ioContext->wsaBuf.buf + ioContext->overLapped.InternalHigh);
	buffer->push_back(0);
	_opHandler->handleRecv(context, ioContext);


	return postRecv(context);
}


bool IOCPManager::handleSend(SocketContext *context, IOContext *ioContext)
{
	ioContext->opBytes += ioContext->overLapped.InternalHigh;

	_opHandler->handleSend(context, ioContext);

	if (ioContext->isLast == true)
	{
		if (ioContext->owner != NULL)
		{
			if (((IOTask *)ioContext->owner)->isFinished() == true)
			{
				_sendTaskManager->removeTask(((IOTask *)ioContext->owner)->key);
			}
			
		}
	}

	return true;
}


bool IOCPManager::bindWithIOCP(SocketContext *context)
{
	HANDLE handle = CreateIoCompletionPort((HANDLE)context->sockId, _IOCPHandle, (ULONG_PTR)context, 0);
	if (handle == NULL)
	{
		int result = GetLastError();
		
		printf("Error Message:%s\n", getWindowsErrorMessage(result).c_str());

		return false;
	}

	return true;
}


void IOCPManager::shutdown()
{
	_shutdownEvent.setEvent();
}

void IOCPManager::postShutdown()
{
	PostQueuedCompletionStatus(_IOCPHandle, 0, NULL, NULL);
}

void IOCPManager::closeAll()
{
	_acceptSocketManager->clearAll();
	_clientManager->clearAll();
	_listenContext->close();
	_bufferManager->clearAll();
	_sendTaskManager->clearAll();
}

void IOCPManager::deinitialize()
{
	closeAll();
	delete _acceptSocketManager;
	delete _clientManager;
	delete _listenContext;
	delete _bufferManager;
	delete _sendTaskManager;
	delete _opHandler;



}

bool IOCPManager::handleFirstRecvWithData(SocketContext *context, IOContext *ioContext)
{
	SOCKADDR_IN *clientAddr = NULL;
	SOCKADDR_IN *localAddr = NULL;

	int clientLength = sizeof(SOCKADDR_IN);
	int localLength = sizeof(SOCKADDR_IN);

	//_lpfnAcceptExSockAddress(ioContext->wsaBuf.buf, 
	//						ioContext->wsaBuf.len - ((clientLength + 16) * 2),
	//						clientLength + 16,
	//						clientLength + 16,
	//						(sockaddr **)&localAddr,
	//						&localLength,
	//						(sockaddr **)&clientAddr,
	//						&clientLength);

	_lpfnAcceptExSockAddress(ioContext->wsaBuf.buf, 
		0,
		clientLength + 16,
		clientLength + 16,
		(sockaddr **)&localAddr,
		&localLength,
		(sockaddr **)&clientAddr,
		&clientLength);

	SocketContext *newContext = _clientManager->getNewContext();
	newContext->isAcceptable = true;
	newContext->sockId = ioContext->sockId;
	//memcpy(newContext->recvOverLapped.buffer, context->recvOverLapped.buffer, MAX_BUFFER_LENGTH);
	memcpy(&newContext->sockAddr,&clientAddr, sizeof(SOCKADDR_IN));

	_bufferManager->addNewIoBuffer(newContext->sockId);
	


	bindWithIOCP(newContext);


	if (this->postRecv(newContext) == false)
	{
		return false;
	}


	_opHandler->handleAccept(newContext);

	return true;

}


void IOCPManager::SendLargeData(SocketContext *context)
{
	IOTask *task = _sendTaskManager->createNewTask();
	unsigned long long totalSize = 1024 * 1024;
	char *buffer = new char[totalSize];

	memset(buffer, '1', totalSize);

	unsigned long long countSize = 0;
	while (countSize < totalSize)
	{
		if (totalSize - countSize < context->ioContext->wsaBuf.len)
		{

		}

		IOContext *ioContext = task->createNewContext();

		memcpy(ioContext->buffer, buffer + countSize, ioContext->capacity);
		countSize += ioContext->capacity;
		ioContext->opType = SendOperation;
		ioContext->sockId = context->sockId;

		
	}

	if (task->ioList.cbegin() != task->ioList.end())
	{
		task->ioList.back()->isLast = true;
	}

	list<IOContext *>::iterator iter = task->ioList.begin();
	for (;iter != task->ioList.end(); iter++)
	{
		WSASend(context->sockId, &(*iter)->wsaBuf, 1, NULL, 0, &(*iter)->overLapped, NULL);
		int result = WSAGetLastError();
		WRITELOG(getWindowsErrorMessage(result).c_str());
	}
	
	

}

bool IOCPManager::handleFirstRecvWithoutData(SocketContext *context, IOContext *ioContext)
{
	SocketContext *newContext = _clientManager->getNewContext();

	//SOCKADDR_IN clientAddr = {0};
	//int len = sizeof(SOCKADDR_IN);

	//getpeername(context->sockId, (sockaddr *)&clientAddr, &len);


	SOCKADDR_IN *clientAddr = NULL;
	SOCKADDR_IN *localAddr = NULL;

	int clientLength = sizeof(SOCKADDR_IN);
	int localLength = sizeof(SOCKADDR_IN);

	//_lpfnAcceptExSockAddress(ioContext->wsaBuf.buf, 
	//						ioContext->wsaBuf.len - ((clientLength + 16) * 2),
	//						clientLength + 16,
	//						clientLength + 16,
	//						(sockaddr **)&localAddr,
	//						&localLength,
	//						(sockaddr **)&clientAddr,
	//						&clientLength);

	_lpfnAcceptExSockAddress(ioContext->wsaBuf.buf, 
		0,
		clientLength + 16,
		clientLength + 16,
		(sockaddr **)&localAddr,
		&localLength,
		(sockaddr **)&clientAddr,
		&clientLength);

	newContext->sockId = ioContext->sockId;
	memcpy(&newContext->sockAddr, clientAddr, clientLength);

	bindWithIOCP(newContext);

	_bufferManager->addNewIoBuffer(newContext->sockId);


	newContext->recvBuff = _bufferManager->getBufferByKey(newContext->sockId);


	_clientManager->addNewClient(SocketContextManager::getClientName(context), context);

	_opHandler->handleAccept(newContext);

	SendLargeData(newContext);

	if (this->postRecv(newContext) == false)
	{
		return false;
	}

	

	return true;
}


bool IOCPManager::postDisconnect(SocketContext *context)
{
	IOContext *ioContext  = new IOContext;
	ioContext->sockId = context->sockId;
	ioContext->opType = DisconnectOperation;
	if(_lpfnDisconnectEx(context->sockId, &ioContext->overLapped,
						TF_REUSE_SOCKET, 0) == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}


bool IOCPManager::handleDisconnect(SocketContext *context, IOContext *ioContext)
{

	_opHandler->handleDisconnect(context);
	delete ioContext;


	

	return postAccept(context);
}

void IOCPManager::run()
{
	OVERLAPPED *ol = NULL;
	SocketContext *context = NULL;
	DWORD handleBytes = 0;
	while (WAIT_OBJECT_0 != WaitForSingleObject(_shutdownEvent.getHandle(), 0))
	{
		int result = GetQueuedCompletionStatus(_IOCPHandle, &handleBytes, 
										(PULONG_PTR)&context,
										&ol,INFINITE);

		if (context == NULL)
		{
			break;
		}

		if (result == 0)
		{
			DWORD dwError = GetLastError();

			continue;
		}else
		{
			IOContext *ioContext = CONTAINING_RECORD(ol, IOContext, overLapped);

			switch(ioContext->opType)
			{
				case RecvOperation:
					handleRecv(context, ioContext);
					break;
				case AcceptOperation:
					handleAccept(context, ioContext);
					break;
				case SendOperation:
					handleSend(context, ioContext);
					break;
				default:
					break;
			}


		}

	}
}

void DefaultOperationHandler::handleRecv(SocketContext *sockContext, IOContext *ioContext)
{
	char buffer[8096] = {0};
	sprintf(buffer, "Index[%d]recv bytes[%u][%s]", ioContext->index, ioContext->overLapped.InternalHigh
							,sockContext->recvBuff->data());

	WRITELOG(buffer);
	sockContext->recvBuff->clear();
}

void DefaultOperationHandler::handleSend(SocketContext *sockContext, IOContext *ioContext)
{
	char buffer[1024] = {0};
	sprintf(buffer, "Index[%d]send bytes[%u]", ioContext->index, ioContext->overLapped.InternalHigh);
	
	WRITELOG(buffer);
}

void DefaultOperationHandler::handleAccept(SocketContext *context)
{
	char buffer[1024] = {0};
	sprintf(buffer,"new connect:[%s]", SocketContextManager::getClientName(context).c_str());
	
	WRITELOG(buffer);
}

void DefaultOperationHandler::handleDisconnect(SocketContext *context)
{
	
}
