#pragma once

#include <WinSock2.h>

#include <MSWSock.h>
#include <Ws2tcpip.h>
#include <list>
#include <map>
#include <vector>
#include <string>
#include <iterator>
#include <utility>
#include <memory>
#include <unordered_map>
#include "../../thread/SyncObjects.h"
#include "../../thread/Event.h"


#ifndef MAX_BUFFER_LENGTH
#define MAX_BUFFER_LENGTH 64
#endif

#define LISTEN_PORT 10008

#define LISTEN_IP "0.0.0.0"

#define  MAX_POST_ACCEPT 10000


using std::list;
using std::map;
using std::unordered_map;
using std::iterator;
using std::string;
using std::vector;
using std::make_pair;
using std::shared_ptr;
using std::weak_ptr;

struct SocketContext;
struct IOBuffer;
typedef map<int, IOBuffer *>::iterator IoBufferMapIterator;
typedef map<std::string, SocketContext *>::iterator SocketContextMapIterator;
typedef list<SocketContext *>::iterator SocketContextListIterator;


typedef shared_ptr<SocketContext> SSocketContextPtr;
typedef weak_ptr<SocketContext> WSocketContextPtr;

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
	void *owner;
	unsigned long long capacity;
	unsigned long long opBytes;
	int index;
	bool isLast;
	bool isSelfDestroy;
	WSABUF wsaBuf;

	char *buffer;

	IOContext(int bufferSize = MAX_BUFFER_LENGTH, bool isClear = true);

	~IOContext();

	bool isFinished();

	int setBuffer(const char *sendBuffer, int length);

	void reallocBuffer(int size = MAX_BUFFER_LENGTH);

	void clearBuffer();

	//void setBuffer(char *buffer, unsigned long long bufferLength, bool isClear);
	
};




struct SocketContext
{
	int index;
	bool isAcceptable;
	SOCKADDR_IN sockAddr;
	SOCKET sockId;
	DWORD opSet;
	vector<char > *recvBuff;
	IOContext *ioContext;
	SocketContext();

	~SocketContext();

	void clear();

	void close();


};


void writeLog(const char *str, const char *filesource = NULL, const char *functionName = NULL);

#define  WRITELOG(str) \
	writeLog(str, __FILE__, __FUNCTION__);