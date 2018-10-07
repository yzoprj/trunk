 #include "IOCP.h"
#include <WinSock.h>
#include <Windows.h>
#include "OperationHandler.h"
#include "TimeWheelTimer.h"
std::string getWindowsErrorMessage(DWORD errCode)
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
	_lpfnGetAcceptExSockAddress = NULL;
	_lpfnDisconnectEx = NULL;
	_lpfinConnectEx = NULL;
	_clientManager = new SocketContextManager;
	_acceptContextManger = new IOBufferManager;
	_bufferManager = new IOBufferManager;
	_sendTaskManager = new IOTaskManager;
	_timeWheelTimer = new TimeWheelTimer;

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
	
	_listenContext.reset(new SocketContext);
	

	_listenContext->sockId = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (_listenContext->sockId == INVALID_SOCKET)
	{
		WRITELOG("initialize server socket error!!!!");
		return false;
	}

	WRITELOG("Create New Listen Socket Success!");
	

	if (CreateIoCompletionPort((HANDLE)_listenContext->sockId, _IOCPHandle,(ULONG_PTR)_listenContext.get(), 0) == NULL)
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


	//if (initWSAFunction() == false)
	//{
	//	_listenContext->close();
	//	return false;
	//}


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
	

		if (postAccept(NULL) == false)
		{
			
			return false;
		}
	}

	return true;
}


void IOCPManager::getHostIP()
{
	char hostName[MAX_PATH];

	gethostname(hostName, MAX_PATH);

	hostent *host =  gethostbyname(hostName);

	inet_ntop(AF_INET, (((struct in_addr*)host->h_addr)), hostName, MAX_PATH);

	strcpy(_hostIP, hostName);
}

bool IOCPManager::initWSAFunction()
{
	// AcceptEx 和 GetAcceptExSockaddrs 的GUID，用于导出函数指针
	GUID guidAcceptEx = WSAID_ACCEPTEX;  
	GUID guidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 
	GUID guidDisConnectEx = WSAID_DISCONNECTEX;
	DWORD dwBytes = 0;


	getHostIP();


	SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	int result = WSAIoctl(sock,
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
		closesocket(sock);
		return false;
	}

	WRITELOG("Get AcceptEx Success!");

	result = WSAIoctl(sock,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidGetAcceptExSockAddrs,
		sizeof(guidGetAcceptExSockAddrs),
		&_lpfnGetAcceptExSockAddress,
		sizeof(_lpfnGetAcceptExSockAddress),
		&dwBytes,
		NULL,
		NULL);

	if (result == -1)
	{
		closesocket(sock);
		return false;
	}


	WRITELOG("Get GetAcceptExSocketAddress Success!");

	result = WSAIoctl(sock,
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
		closesocket(sock);
		return false;
	}

	WRITELOG("Get DisconnectEx Success!");

	GUID guidConnectionEx = WSAID_CONNECTEX;  

	result = WSAIoctl(sock,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidConnectionEx,
		sizeof(guidConnectionEx),
		&_lpfinConnectEx,
		sizeof(_lpfinConnectEx),
		&dwBytes,
		NULL,
		NULL);

	if(result == -1)
	{
		closesocket(sock);
		return false;
	}

	WRITELOG("Get ConnectionEx Success");

	closesocket(sock);
	return true;
}

bool IOCPManager::postAccept(IOBuffer *context)
{
	if (_listenContext->sockId == INVALID_SOCKET)
	{
		return false;
	}


	DWORD dwBytes = 0;


	SOCKET sockId = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (sockId == INVALID_SOCKET)
	{
		return false;
	}
	

	if (context == NULL)
	{
		context = _acceptContextManger->addNewIoBuffer(IOBufferManager::getNewBuffer());
	}

	context->ioContext.sockId = sockId;
	context->ioContext.opType = AcceptOperation;





	//int result = _lpfnAcceptEx(_listenContext->sockId,
	//						context->sockId,
	//						ioContext->wsaBuf.buf,
	//						ioContext->wsaBuf.len - ((sizeof(SOCKADDR_IN)+16)*2),
	//						sizeof(SOCKADDR_IN) + 16,
	//						sizeof(SOCKADDR_IN) + 16,
	//						&dwBytes,
	//						(OVERLAPPED *)ioContext);


	int result = _lpfnAcceptEx(_listenContext->sockId,
							sockId,
							context->ioContext.wsaBuf.buf,
							0,
							sizeof(SOCKADDR_IN) + 16,
							sizeof(SOCKADDR_IN) + 16,
							&dwBytes,
							(OVERLAPPED *)&context->ioContext.overLapped);

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


bool IOCPManager::postRecv(IOBuffer *buffer)
{
	DWORD dwFlgas = 0;
	DWORD dwBytes = 0;


	

	buffer->ioContext.clearBuffer();
	buffer->cache.clear();

	buffer->ioContext.opType = RecvOperation;

	int result = WSARecv(buffer->ioContext.sockId, &buffer->ioContext.wsaBuf, 1, &dwBytes, &dwFlgas, &buffer->ioContext.overLapped, NULL);

	if ((result == SOCKET_ERROR) && (GetLastError() != WSA_IO_PENDING) )
	{
		return false;
	}

	return true;
}


bool IOCPManager::postSend(SSocketContextPtr &context)
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


bool IOCPManager::handleAccept(SSocketContextPtr &context, IOContext *ioContext)
{
	
	if (ioContext->overLapped.InternalHigh > 0)
	{
		handleFirstRecvWithData(context, ioContext);
	}else
	{
		handleFirstRecvWithoutData(context, ioContext);
	}

	ioContext->clearBuffer();
	ioContext->sockId = INVALID_SOCKET;

	IOBuffer *ioBuffer = CONTAINING_RECORD(ioContext, IOBuffer, ioContext);

	return postAccept(ioBuffer);
}


bool IOCPManager::handleRecv(SSocketContextPtr &context, IOContext *ioContext)
{



	// recieve 0 byte means remote host is disconnected
	//if (ioContext->overLapped.InternalHigh == 0)
	//{
	//	_bufferManager->removeIoBuffer((int)context.get());
	//	string str = SocketContextManager::getClientName(context) + " is disconnected!";
	//	_clientManager->removeClient(SocketContextManager::getClientName(context));
	//	
	//	
	//	WRITELOG(str.c_str());
	//	
	//	return false;
	//}

	IOBuffer *ioBuffer = CONTAINING_RECORD(ioContext, IOBuffer, ioContext);
	vector<char> &buffer = ioBuffer->cache;
	if (buffer.capacity() < buffer.size() + ioContext->overLapped.InternalHigh)
	{
		buffer.reserve(ioBuffer->cache.size() + ioContext->overLapped.InternalHigh + MAX_BUFFER_LENGTH / 8);
	}

	buffer.insert(buffer.begin() + buffer.size(), ioContext->wsaBuf.buf, ioContext->wsaBuf.buf + ioContext->overLapped.InternalHigh);
	buffer.push_back(0);

	/*const char *str = "123456781\n123456782\n123456783\n123456784\n123456785\n123456786\n123456787\n123456788\n123456789\n";
	sendData(SocketContextManager::getClientName(context), str ,  strlen(str), true);*/

	_opHandler->handleRecv(context, ioBuffer);

	ioContext->clearBuffer();

	_timeWheelTimer->addContext(context);

	return postRecv(ioBuffer);
}


bool IOCPManager::handleSend(SSocketContextPtr &context, IOContext *ioContext)
{



	ioContext->opBytes += ioContext->overLapped.InternalHigh;

	_opHandler->handleSend(context, ioContext);

	if (ioContext->owner != NULL)
	{
		IOTask *task = (IOTask *)ioContext->owner;
		task->increment(ioContext->overLapped.InternalHigh);
		task->incrementFinishTimes();
		_sendTaskManager->removeTask((long)task, false);
	
	}


	//if (ioContext->isLast == true)
	//{
	//	if (ioContext->owner != NULL)
	//	{
	//		if (((IOTask *)ioContext->owner)->isFinished() == true)
	//		{
	//			_sendTaskManager->removeTask(((IOTask *)ioContext->owner)->key);
	//		}
	//		
	//	}
	//}
	_timeWheelTimer->addContext(context);
	return true;
}


bool IOCPManager::bindWithIOCP(SSocketContextPtr &context)
{
	HANDLE handle = CreateIoCompletionPort((HANDLE)context->sockId, _IOCPHandle, (ULONG_PTR)context.get(), 0);
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
	_acceptContextManger->clearAll();
	_clientManager->clearAll();
	_listenContext->close();
	_bufferManager->clearAll();
	_sendTaskManager->clearAll();
}


void IOCPManager::startTimer()
{
	_timeWheelTimer->activate();
}

void IOCPManager::stopTimer()
{
	_timeWheelTimer->quit();
}

void IOCPManager::deinitialize()
{
	closeAll();
	delete _acceptContextManger;
	delete _clientManager;
	delete _bufferManager;
	delete _sendTaskManager;
	delete _opHandler;
	delete _timeWheelTimer;



}

bool IOCPManager::handleFirstRecvWithData(SSocketContextPtr &context, IOContext *ioContext)
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

	_lpfnGetAcceptExSockAddress(ioContext->wsaBuf.buf, 
		0,
		clientLength + 16,
		clientLength + 16,
		(sockaddr **)&localAddr,
		&localLength,
		(sockaddr **)&clientAddr,
		&clientLength);

	SSocketContextPtr newContext = _clientManager->getNewContext();
	newContext->isAcceptable = true;
	newContext->sockId = ioContext->sockId;
	//memcpy(newContext->recvOverLapped.buffer, context->recvOverLapped.buffer, MAX_BUFFER_LENGTH);
	memcpy(&newContext->sockAddr,&clientAddr, sizeof(SOCKADDR_IN));

	IOBuffer *ioBuffer = _bufferManager->addNewIoBuffer(IOBufferManager::getNewBuffer());
	


	bindWithIOCP(newContext);

	ioBuffer->ioContext.sockId = newContext->sockId;
	if (this->postRecv(ioBuffer) == false)
	{
		return false;
	}


	_opHandler->handleAccept(newContext);

	return true;

}


void IOCPManager::SendLargeData(SSocketContextPtr &context)
{
	//IOTask *task = _sendTaskManager->createNewTask();
	unsigned long long totalSize = 1024 * 1024 * 12;
	char *buffer = new char[totalSize];
	//task->totalBytes = totalSize;
	memset(buffer, '1', totalSize);

	//unsigned long long countSize = 0;
	//while (countSize < totalSize)
	//{
	//	if (totalSize - countSize < context->ioContext->wsaBuf.len)
	//	{

	//	}

	//	IOContext *ioContext = task->createNewContext();

	//	memcpy(ioContext->buffer, buffer + countSize, ioContext->capacity);
	//	countSize += ioContext->capacity;
	//	ioContext->opType = SendOperation;
	//	ioContext->sockId = context->sockId;

	//	
	//}

	

	//if (task->ioList.cbegin() != task->ioList.end())
	//{
	//	task->ioList.back()->isLast = true;
	//}

	//list<IOContext *>::iterator iter = task->ioList.begin();
	//for (;iter != task->ioList.end(); iter++)
	//{
	//	WSASend(context->sockId, &(*iter)->wsaBuf, 1, NULL, 0, &(*iter)->overLapped, NULL);
	//	int result = WSAGetLastError();
	//	WRITELOG(getWindowsErrorMessage(result).c_str());
	//}
	
	sendData(SocketContextManager::getClientName(context), buffer, totalSize);
	delete buffer;

}

bool IOCPManager::handleFirstRecvWithoutData(SSocketContextPtr &context, IOContext *ioContext)
{
	SSocketContextPtr newContext = _clientManager->getNewContext();

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

	_lpfnGetAcceptExSockAddress(ioContext->wsaBuf.buf, 
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

	IOBuffer *ioBuffer = _bufferManager->addNewIoBuffer((int)newContext.get());

	ioBuffer->ioContext.sockId = newContext->sockId;
	newContext->setContextName(SocketContextManager::getClientName(newContext).c_str());
	_clientManager->addNewClient(newContext->contextName, newContext);

	_opHandler->handleAccept(newContext);

	//SendLargeData(newContext);

	if (this->postRecv(ioBuffer) == false)
	{
		return false;
	}

	//context->close();
	_timeWheelTimer->addContext(newContext);
	return true;
}


bool IOCPManager::postDisconnect(SSocketContextPtr &context)
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


bool IOCPManager::handleDisconnect(SSocketContextPtr &context, IOContext *ioContext)
{

	_opHandler->handleDisconnect(context);
	delete ioContext;


	return true;
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

		QueueErrorCode errCode = handleError(context, ol, result, handleBytes);
		if (errCode == ERR_CONTINUE)
		{
			continue;
		}else if (errCode == ERR_EXIT)
		{
			break;
		}else
		{
			SSocketContextPtr contextPtr;
			if (context != _listenContext.get())
			{
				contextPtr = _clientManager->getContext(context);

				// client isn't existed, may be disconnected
				if (contextPtr.get() == NULL)
				{
					handleError(context);
					continue;
				}
			}else
			{
				contextPtr = _listenContext;
			}



			IOContext *ioContext = CONTAINING_RECORD(ol, IOContext, overLapped);

			switch(ioContext->opType)
			{
			case RecvOperation:
				handleRecv(contextPtr, ioContext);
				break;
			case AcceptOperation:
				handleAccept(contextPtr, ioContext);
				break;
			case SendOperation:
				handleSend(contextPtr, ioContext);
				break;
			case ConnnectionOperation:
				break;
			case DisconnectOperation:
				break;
			default:
				break;
			}

		}
	}
}


void IOCPManager::handleError(SocketContext *context)
{

}

IOCPManager::QueueErrorCode IOCPManager::handleError(SocketContext *sockContext, OVERLAPPED *ol, unsigned long result, unsigned long dwBytes)
{
	int retCode = 0;

	// 特定错误信息
	if (sockContext == NULL && ol == NULL)
	{
		return ERR_EXIT;
	}

	// 异常返回
	if (result == FALSE)
	{
		unsigned long errCode = GetLastError();
		WRITELOG(getWindowsErrorMessage(errCode).c_str());

		if (ol == NULL)
		{
			return ERR_CONTINUE;
		}else
		{
			IOContext *ioContext = CONTAINING_RECORD(ol, IOContext, overLapped);


			// 当传输错误的时候，累计已经返回的投递任务
			// 当达到task的任务总数时候，则清除
			// 并返回继续标记，处理以后任务
			if (ioContext->opType == SendOperation)
			{
				if (ioContext->owner != NULL)
				{
					IOTask *task = (IOTask *)ioContext->owner;

					task->incrementFinishTimes();

					//char buffer[256] = {0};

					//sprintf(buffer, "Index[%d]==accumulate[%d]==total[%d]", ioContext->index, task->finishedTimes,
					//								task->ioList.size());
					//WRITELOG(buffer);
					_sendTaskManager->removeTask((int)task, false);
				}
				
			}else if (ioContext->opType == RecvOperation ||
						ioContext->opType == ConnnectionOperation)
			{
				// 发送操作出现错误，直接断开连接
				SSocketContextPtr contextPtr = _clientManager->getContext(sockContext);

				// context 已经被删除
				if (contextPtr.get() == NULL )
				{
					return ERR_CONTINUE;
				}

				// context 等于监听套接字，意味着已经出现严重错误
				// 则退出
				if (contextPtr.get() == _listenContext.get())
				{
					return ERR_CONTINUE;
				}
				string str = SocketContextManager::getClientName(contextPtr) + " is disconnected with error";
				WRITELOG(str.c_str());
				_clientManager->removeContext(contextPtr);
				_bufferManager->removeIoBuffer((long)contextPtr.get());

			}

			return ERR_CONTINUE;

		}

	}else
	{
		// GetQueueCompletionStatus() 返回成功情况
		
		// 出现传输为0的情况，客户端已经断开
		if (dwBytes == 0)
		{
			IOContext *ioContext = CONTAINING_RECORD(ol, IOContext, overLapped);
			SSocketContextPtr contextPtr = _clientManager->getContext(sockContext);
			//对方主动关闭连接
			if(ioContext->opType == RecvOperation)
			{
				_clientManager->removeContext(contextPtr);
				_bufferManager->removeIoBuffer((long)contextPtr.get());

				string str = SocketContextManager::getClientName(contextPtr) + " is disconnected normally";
				WRITELOG(str.c_str());
				return ERR_CONTINUE;
			}
		}

	}

	return ERR_NEXT;
}


bool IOCPManager::isSocketAlive(SSocketContextPtr &ptr)
{
	int sentBytes = send(ptr->sockId, "", 0, 0);

	if (sentBytes == -1)
	{
		unsigned long dwCode = WSAGetLastError();

		WRITELOG(getWindowsErrorMessage(dwCode).c_str());
		return false;
	}

	return true;
}

WSocketContextPtr IOCPManager::getSocketContext(const string clientKey)
{
	return _clientManager->getContext(clientKey);
}


long IOCPManager::sendData(string clientKey, const char *buffer, long long length,
					bool isCopy /* = true */, int unitSize /* = MAX_BUFFER_LENGTH */)
{


	if (buffer == NULL || length <= 0)
	{
		return -1;
	}

	
	WSocketContextPtr wptr = _clientManager->getContext(clientKey);

	SSocketContextPtr ptr = wptr.lock();
	
	if (ptr == NULL)
	{
		return -2;
	}

	_timeWheelTimer->addContext(ptr);


	IOTask *task = _sendTaskManager->createNewTask();
	IOContext *ioContext = NULL;


	if (isCopy == false)
	{
		task->source = (void *)buffer;
	}

	if (length < unitSize)
	{
		ioContext = task->createNewContext(unitSize, isCopy);
		ioContext->opType = SendOperation;
		ioContext->setBuffer(buffer, length);
		ioContext->isLast = true;
	}else
	{

		int taskCount = length / unitSize;
		int delta = length % unitSize;

		for (int i = 0; i < taskCount; i++)
		{
			ioContext = task->createNewContext(unitSize, isCopy);
			ioContext->setBuffer(buffer + unitSize * i, unitSize);
			ioContext->opType = SendOperation;
		}

		if (delta > 0)
		{
			ioContext = task->createNewContext(unitSize, isCopy);
			ioContext->setBuffer(buffer + unitSize * taskCount, delta);
			//ioContext->isLast = true;
		}
		ioContext->isLast = true;

	}

	task->totalBytes = length;

	
	list<IOContext *>::iterator iter = task->ioList.begin();
	int size = task->ioList.size();
	for (int i = 1; i <= size; i++)
	{
		WSASend(ptr->sockId, &(*iter)->wsaBuf, 1, NULL, 0, &(*iter)->overLapped, NULL);
		int result = WSAGetLastError();
		WRITELOG(getWindowsErrorMessage(result).c_str());

		if (i < size)
		{
			iter++;
		}
	}


	return 1;
}


bool IOCPManager::start()
{

	return true;
}

void IOCPManager::stop()
{
	return ;
}

void IOCPManager::postConnection(const char *ip, int port)
{

	unsigned long dwBytes = 0;

	SSocketContextPtr ptr =  _clientManager->getNewContext();
	ptr->sockAddr.sin_family = AF_INET;
	ptr->sockAddr.sin_port = htons(port);
	ptr->sockAddr.sin_addr.S_un.S_addr = inet_addr(ip);

	ptr->sockId = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	addr.sin_addr.S_un.S_addr = inet_addr(_hostIP);

	
	if (bind(ptr->sockId, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		WRITELOG(getWindowsErrorMessage(WSAGetLastError()).c_str());
		return;
	}

	if (bindWithIOCP(ptr) == false)
	{
		return;
	}

	_clientManager->addNewClient(ptr);
	IOBuffer *ioBuffer = _bufferManager->addNewIoBuffer((int)ptr.get());
	ioBuffer->ioContext.opType = ConnnectionOperation;
	if (_lpfinConnectEx(ptr->sockId, (sockaddr *)&ptr->sockAddr,
			sizeof(SOCKADDR_IN),
			ioBuffer->ioContext.wsaBuf.buf,
			0,
			&dwBytes, &ioBuffer->ioContext.overLapped) == FALSE)
	{
		DWORD result = WSAGetLastError();
		if (result != ERROR_IO_PENDING)
		{
			WRITELOG(getWindowsErrorMessage(result).c_str());
		}
	}

}