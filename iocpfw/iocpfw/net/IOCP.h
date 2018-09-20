#pragma once
#include <WinSock2.h>
#include <MSWSock.h>


#define MAX_BUFFER_LENGTH 8192
#define LISTEN_PORT 10008
#define LISTEN_IP "0.0.0.0"

enum IOCPOperationType
{
	NullOperation =0x01,
	AcceptOperation = 0x02,
	RecvOperation = 0x04,
	SendOperation = 0x08,
	ErrorOperation = 0x10

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
		wsaBuf.buf	= buffer;
		wsaBuf.len	= 0;
		totalBytes	= 0;
		opBytes		= 0;
		ZeroMemory(&overLapped, sizeof(overLapped));
	}
};

struct SocketContext
{
	IOOverLapped recvOverLapped;
	IOOverLapped sendOverLapped;
	int sockedIndex;
	SOCKET sockId;
	DWORD opSet;

	SocketContext()
	{
		sockedIndex = -1;
		sockId = INVALID_SOCKET;
		opSet = 0;
	}

	~SocketContext()
	{
		if (sockId != INVALID_SOCKET)
		{
			shutdown(sockId, SD_BOTH);
			closesocket(sockId);
		}
	}
};


class SocketManager
{

};