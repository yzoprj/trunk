#pragma once
#include <WinSock2.h>

#include <MSWSock.h>
#include <Ws2tcpip.h>
#include <map>
#include <utility>
#include <string>
#include <deque>
#include <memory>
#include <list>
#include <vector>
#include "../thread/SyncObjects.h"
#include "../thread/Event.h"

using std::map;
using std::string;
using std::list;
using std::vector;
using std::iterator;
using std::make_pair;


struct SocketContext;
struct IOTask;

typedef map<int, vector<char> *>::iterator IoBufferMapIterator;
typedef map<string, SocketContext *>::iterator SocketContextMapIterator;
typedef list<SocketContext *>::iterator SocketContextListIterator;

#ifndef MAX_BUFFER_LENGTH
#define MAX_BUFFER_LENGTH 8096
#endif

#define LISTEN_PORT 10008

#define LISTEN_IP "0.0.0.0"

#define  MAX_POST_ACCEPT 10



enum IOCPOperationType
{
	NullOperation =0x00,
	AcceptOperation = 0x01,
	RecvOperation = 0x02,
	SendOperation = 0x04,
	ErrorOperation = 0x8,
	DisconnectOperation = 0x10
};

// 自定义重叠结构定义

struct IOContext
{
	OVERLAPPED overLapped;
	IOCPOperationType opType;
	SOCKET sockId;
	IOTask *owner;
	DWORD totalBytes;
	DWORD opBytes;
	int index;
	bool isLast;
	bool isSelfDestroy;
	bool isBufferClear;
	WSABUF wsaBuf;
	
	char *buffer;

	IOContext()
		:opBytes(NullOperation),
		sockId(SOCKET_ERROR),
		owner(NULL),
		totalBytes(0),
		isLast(false),
		index(-1),
		isSelfDestroy(false),
		isBufferClear(true),
		buffer(NULL)
	{
		
		reallocBuffer();
		clearBuffer();
	}

	~IOContext()
	{
		if (isBufferClear == true)
		{
			delete buffer;
			buffer = NULL;
			wsaBuf.buf = buffer;
		}
	}


	bool isFinished()
	{
		if (opBytes == totalBytes)
		{
			return true;
		}
		return false;
	}


	void reallocBuffer(int size = MAX_BUFFER_LENGTH)
	{
		if (isBufferClear == true)
		{
			if (buffer != NULL)
			{
				delete buffer;


			}
			buffer = new char[size];
			wsaBuf.buf = buffer;
			wsaBuf.len = size;
		}
	}

	void clearBuffer()
	{

		ZeroMemory(&overLapped, sizeof(overLapped));
		ZeroMemory(wsaBuf.buf, wsaBuf.len);

		
	}
};


struct IOTask
{
	int key;
	list<IOContext *> ioList;
	IOContext *createNewContext()
	{
		IOContext *ctx = new IOContext;
		ctx->owner = this;
		ctx->index = ioList.size();
		ioList.push_back(ctx);
		return ctx;
	}

	bool isFinished()
	{
		if (ioList.size() > 0)
		{
			if (ioList.back()->isFinished())
			{
				return true;
			}else
			{
				return false;
			}
		}

		return true;
	}

	void clear()
	{
		list<IOContext *>::iterator iter = ioList.begin();

		while(iter != ioList.end())
		{
			delete *iter;
			iter++;
		}
		
		ioList.clear();
	}

};


class IOTaskManager
{
public:

	~IOTaskManager()
	{
		clearAll();
	}

	IOTask *createNewTask()
	{
		MutexGuard guard(_cs);
		IOTask *task = new IOTask;
		_taskMap.insert(make_pair((int)task, task));

		return task;
	}

	void removeTask(int key)
	{
		MutexGuard gurad(_cs);
		_taskMap.erase(key);
	}


	void clearAll()
	{
		map<int, IOTask *>::iterator iter = _taskMap.begin();

		while (iter != _taskMap.end())
		{
			delete iter->second;
			iter++;
		}
	}
private:

	map<int, IOTask *> _taskMap;
	CriticalSection _cs;
};


struct SocketContext
{
	int index;
	bool isAcceptable;
	SOCKADDR_IN sockAddr;
	SOCKET sockId;
	DWORD opSet;

	SocketContext()
	{
		index = -1;
		sockId = INVALID_SOCKET;
		opSet = 0;
		isAcceptable = false;
	}

	void clear()
	{

		memset(&sockAddr, 0 , sizeof(sockAddr));
		sockId = INVALID_SOCKET;
		opSet = 0;
	}

	void close()
	{
		if (sockId != INVALID_SOCKET)
		{
			shutdown(sockId, SD_BOTH);
			closesocket(sockId);
			sockId = INVALID_SOCKET;
		}
	}

	~SocketContext()
	{
		close();
	}
};




class SocketManager
{

public:
	SocketManager()
	{

	}

	~SocketManager()
	{
		clearAll();
	}

	void clearAll()
	{
		_clientMaps.clear();

		list<SocketContext *>::iterator iter = _connectionList.begin();
		while (iter != _connectionList.end())
		{
			delete *iter;
			iter++;
		}
		
		_connectionList.clear();
	}

	SocketContext *getNewContext()
	{
		MutexGuard gurad(_cs);
		SocketContext *ctx = createNewContext();
		ctx->index = _connectionList.size();
		_connectionList.push_back(ctx);

		return ctx;
	}

	static SocketContext *createNewContext()
	{
		return new SocketContext;
	}

	void addNewClient(string clientName, SocketContext *ctx)
	{
		MutexGuard guard(_cs);
		_clientMaps.insert(make_pair(clientName, ctx));
	}

	static string getClientName(SocketContext *ctx)
	{
		
		char buffer[128] = {0};
		char keyBuffer[128] = {0};
		

		sprintf(keyBuffer, "%s:%d",inet_ntop(AF_INET,&ctx->sockAddr.sin_addr, buffer, 128),
											htons(ctx->sockAddr.sin_port));

		return keyBuffer;
		
	}

	void addNewClient(SocketContext *ctx)
	{
		 addNewClient(getClientName(ctx), ctx);
	}

	void removeContext(SocketContext *ctx)
	{
		MutexGuard guard(_cs);
		_connectionList.remove(ctx);
		delete ctx;
	}

	void removeClient(string clientName)
	{
		MutexGuard guard(_cs);
		map<string, SocketContext*>::iterator iter = _clientMaps.find(clientName);
		if (iter != _clientMaps.end())
		{
			_connectionList.remove(iter->second);
			delete iter->second;
		}

		_clientMaps.erase(clientName);

		
	}

	void clearSocket(SocketContext *context)
	{

	}

private:
	list<SocketContext *> _connectionList;
	map<string, SocketContext *> _clientMaps;
	CriticalSection _cs;
};


class BufferManager
{
public:

	~BufferManager()
	{
		clearAll();
	}
	void addNewIoBuffer(SocketContext *context)
	{
		MutexGuard guard(_cs);
		_ioBufferMap.insert(make_pair(context->sockId, new vector<char>()));
	}

	void removeIoBuffer(int key)
	{
		MutexGuard guard(_cs);
		IoBufferMapIterator iter = _ioBufferMap.find(key);

		if (iter != _ioBufferMap.end())
		{
			delete iter->second;
			_ioBufferMap.erase(key);
		}
		
	}

	void clearAll()
	{
		MutexGuard guard(_cs);
		IoBufferMapIterator iter = _ioBufferMap.begin();

		while (iter != _ioBufferMap.end())
		{
			delete iter->second;
			iter++;
		}
		_ioBufferMap.clear();
	}
private:
	CriticalSection _cs;
	map<int, vector<char> *> _ioBufferMap;
};


class OperationHandler
{
public:
	virtual void handleRecv(SocketContext *sockContext, IOContext *ioContext) = 0;

	virtual void handleSend(SocketContext *sockContext, IOContext *ioContext) = 0;

	virtual void handleAccept(SocketContext *context) = 0;

	virtual void handleDisconnect(SocketContext *context) = 0;
};

class DefaultOperationHandler : public OperationHandler
{
	
public:
	virtual void handleRecv(SocketContext *sockContext, IOContext *ioContext);


	virtual void handleSend(SocketContext *sockContext, IOContext *ioContext);


	virtual void handleAccept(SocketContext *context);


	virtual void handleDisconnect(SocketContext *context);

};

class IOCPManager
{
public:

	IOCPManager(OperationHandler *opHandler = NULL);

	~IOCPManager();

	static void initializeSocketLibrary();

	static void deinitializeSocketLibrary();

	bool start();

	void stop();

	string getHostIP();


	bool postSend(SocketContext *context);

	bool postRecv(SocketContext *context);

	bool postDisconnect(SocketContext *context);

public:
	bool initializeIOCP();

	bool initializeListenSocket();

	void deinitialize();

	bool postAccept(SocketContext *context);

	bool handleAccept(SocketContext *context, IOContext *ioContext);
	

	bool handleFirstRecvWithData(SocketContext *context, IOContext *ioContext);
	
	bool handleFirstRecvWithoutData(SocketContext *context, IOContext *ioContext);

	bool handleRecv(SocketContext *context, IOContext *ioContext);

	bool bindWithIOCP(SocketContext *context);

	bool handleSend(SocketContext *context, IOContext *ioContext);

	bool handleDisconnect(SocketContext *context, IOContext *ioContext);

	void run();
private:

	LPFN_ACCEPTEX _lpfnAcceptEx;

	LPFN_GETACCEPTEXSOCKADDRS _lpfnAcceptExSockAddress;

	LPFN_DISCONNECTEX _lpfnDisconnectEx;
	Event _shutdownEvent;

	HANDLE _IOCPHandle;

	int _threadCount;

	OperationHandler *_opHandler;
	SocketManager *_clientManager;
	SocketManager *_acceptSocketManager;
	SocketContext *_listenContext;
	BufferManager *_bufferManager;
	IOTaskManager *_sendTaskManager;
};