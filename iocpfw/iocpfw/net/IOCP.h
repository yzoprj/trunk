#pragma once
#include <WinSock2.h>
#include <MSWSock.h>
#include <map>
#include <utility>
#include <string>
#include <deque>
#include <memory>
#include <list>
#include "../thread/SyncObjects.h"
#include "../thread/Event.h"

using std::map;
using std::string;
using std::list;
using std::make_pair;

#ifndef MAX_BUFFER_LENGTH
#define MAX_BUFFER_LENGTH 8096
#endif

#define LISTEN_PORT 10008

#define LISTEN_IP "0.0.0.0"

#define  MAX_POST_ACCEPT 10

struct SocketContext;
struct IOTask;

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


	IOTask *createNewTask()
	{
		MutexGuard guard(cs);
		IOTask *task = new IOTask;
		taskMap.insert(make_pair((int)task, task));

		return task;
	}

	void removeTask(int key)
	{
		MutexGuard gurad(cs);
		taskMap.erase(key);
	}

	map<int, IOTask *> taskMap;
	CriticalSection cs;
};


struct SocketContext
{
	int index;
	bool isAcceptable;
	SOCKADDR sockAddr;
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

	void clear()
	{
		_clientMaps.clear();
	}

	SocketContext *getNewContext()
	{
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
		_clientMaps.insert(make_pair(clientName, ctx));
	}


	void removeContext(SocketContext *ctx)
	{
		_connectionList.remove(ctx);
		delete ctx;
	}

	void removeClient(string clientName)
	{
		map<string, SocketContext*>::iterator iter = _clientMaps.find(clientName);
		if (iter != _clientMaps.end())
		{
			removeContext(iter->second);
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


class OperationHandler
{
	virtual void handleRecv(SocketContext *context) = 0;

	virtual void handleSend(SocketContext *context) = 0;

	virtual void handleAccept(SocketContext *context) = 0;
};

class IOCPManager
{
public:

	IOCPManager();

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

	SocketManager _clientManager;
	SocketManager _acceptSocketManager;
	SocketContext *_listenContext;
	IOTaskManager _taskManager;
};