#pragma once


#include "IOTaskManager.h"
#include "SocketContextManager.h"
#include "IOBufferManager.h"


class OperationHandler;


class IOCPManager
{
public:

	friend class IOCPThreadManager;

	IOCPManager(OperationHandler *opHandler = NULL);

	~IOCPManager();

	static void initializeSocketLibrary();

	static void deinitializeSocketLibrary();

	bool start();

	void stop();

	void shutdown();

	void postShutdown();

	void closeAll();

	bool initializeIOCP(int threadCount = 4);

	bool initializeListenSocket();

	void deinitialize();

	void run();
	
	WSocketContextPtr getSocketContext(const string clientKey);


protected:

	bool bindWithIOCP(SSocketContextPtr &context);

	bool getWSAFunction();

	bool postAllAcceptSocket();

	void SendLargeData(SSocketContextPtr &context);

	bool postSend(SSocketContextPtr &context);

	bool postRecv(SSocketContextPtr &context);

	bool postDisconnect(SSocketContextPtr &context);

	bool postAccept(SSocketContextPtr &context);

	bool handleAccept(SSocketContextPtr &context, IOContext *ioContext);

	bool handleFirstRecvWithData(SSocketContextPtr &context, IOContext *ioContext);

	bool handleFirstRecvWithoutData(SSocketContextPtr &context, IOContext *ioContext);

	bool handleRecv(SSocketContextPtr &context, IOContext *ioContext);

	bool handleSend(SSocketContextPtr &context, IOContext *ioContext);

	bool handleDisconnect(SSocketContextPtr &context, IOContext *ioContext);

	void handleError(SocketContext *context);
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
	SSocketContextPtr _listenContext;
	IOBufferManager *_bufferManager;
	IOTaskManager *_sendTaskManager;
};