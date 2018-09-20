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

#define MAX_BUFFER_LENGTH 8192
#define LISTEN_PORT 10008
#define LISTEN_IP "0.0.0.0"

enum IOCPOperationType
{
	NullOperation =0x00,
	AcceptOperation = 0x01,
	RecvOperation = 0x02,
	SendOperation = 0x04,
	ErrorOperation = 0x8

};

// 自定义重叠结构定义

struct IOOverLapped
{
	OVERLAPPED overLapped;
	IOCPOperationType opType;
	DWORD totalBytes;
	DWORD opBytes;
	WSABUF wsaBuf;
	char buffer[MAX_BUFFER_LENGTH];

	IOOverLapped()
	{
		opBytes		= 0;
		ZeroMemory(&overLapped, sizeof(overLapped));
		clearBuffer();
	}

	void clearBuffer()
	{
		ZeroMemory(buffer, MAX_BUFFER_LENGTH);
		wsaBuf.buf	= buffer;
		wsaBuf.len	= 0;
		totalBytes	= 0;
	}
};

struct SocketContext
{
	IOOverLapped recvOverLapped;
	IOOverLapped sendOverLapped;
	int sockedIndex;
	bool isAcceptable;
	SOCKADDR sockAddr;
	SOCKET sockId;
	DWORD opSet;

	SocketContext()
	{
		sockedIndex = -1;
		sockId = INVALID_SOCKET;
		opSet = 0;
		isAcceptable = false;
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

	IOCPManager()
	{

	}

	static void initializeSocketLibrary();

	static void deinitializeSocketLibrary();

	bool start();

	void stop();

	string getHostIP();


	bool postSend(SocketContext *context);

	bool postRecv(SocketContext *context);

protected:
	bool initializeIOCP();

	bool initializeListenSocket();

	bool deinitialize();

	bool postAccept(SocketContext *context);

	bool handleAccept(SocketContext *context);
	

	bool handleFirstRecvWithData(SocketContext *context);
	
	bool handleFirstRecvWithoutData(SocketContext *context);

	bool handleRecv(SocketContext *context);

	bool bindWithIOCP(SocketContext *context);

	bool handleSend(SocketContext *context);

private:

	LPFN_ACCEPTEX _lpfnAcceptEx;

	LPFN_GETACCEPTEXSOCKADDRS _lpfnAcceptExSockAddress;

	Event _shutdownEvent;

	HANDLE _IOCPHandle;

	int _threadCount;

	SocketManager _sockManager;

	SocketContext _listenContext;
};