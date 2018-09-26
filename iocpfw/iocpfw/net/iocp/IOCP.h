#pragma once


#include "IOTaskManager.h"
#include "SocketContextManager.h"
#include "IOBufferManager.h"


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

	void shutdown();

	void postShutdown();

	void closeAll();

	bool initializeIOCP(int threadCount = 4);

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

	bool getWSAFunction();


	bool postAllAcceptSocket();

	void SendLargeData(SocketContext *context);


private:

	LPFN_ACCEPTEX _lpfnAcceptEx;

	LPFN_GETACCEPTEXSOCKADDRS _lpfnAcceptExSockAddress;

	LPFN_DISCONNECTEX _lpfnDisconnectEx;

	Event _shutdownEvent;

	HANDLE _IOCPHandle;

	int _threadCount;

	OperationHandler *_opHandler;
	SocketContextManager *_clientManager;
	SocketContextManager *_acceptSocketManager;
	SocketContext *_listenContext;
	IOBufferManager *_bufferManager;
	IOTaskManager *_sendTaskManager;
};