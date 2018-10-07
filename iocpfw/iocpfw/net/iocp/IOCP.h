#pragma once


#include "IOTaskManager.h"
#include "SocketContextManager.h"
#include "IOBufferManager.h"


class TimeWheelTimer;

class OperationHandler;


class IOCPManager
{
public:

	enum QueueErrorCode
	{
		ERR_CONTINUE,
		ERR_EXIT,
		ERR_NEXT,
		
	};

	friend class IOCPThreadManager;

	IOCPManager(OperationHandler *opHandler = NULL);

	~IOCPManager();

	static void initializeSocketLibrary();

	static void deinitializeSocketLibrary();

	bool initWSAFunction();

	bool start();

	void stop();

	void shutdown();

	void postShutdown();


	void postConnection(const char *ip, int port);

	void closeAll();

	bool initializeIOCP(int threadCount = 4);

	bool initializeListenSocket();

	void startTimer();

	void stopTimer();

	void deinitialize();

	void run();
	
	WSocketContextPtr getSocketContext(const string clientKey);
	 
	long sendData(string clientKey, const char *buffer, long long length, bool isCopy = true, int unitSize = MAX_BUFFER_LENGTH);

protected:

	bool bindWithIOCP(SSocketContextPtr &context);

	

	bool postAllAcceptSocket();

	void SendLargeData(SSocketContextPtr &context);

	bool postSend(SSocketContextPtr &context);

	bool postRecv(IOBuffer *buffer);

	bool postDisconnect(SSocketContextPtr &context);

	bool postAccept(IOBuffer *context);

	bool handleAccept(SSocketContextPtr &context, IOContext *ioContext);

	bool handleFirstRecvWithData(SSocketContextPtr &context, IOContext *ioContext);

	bool handleFirstRecvWithoutData(SSocketContextPtr &context, IOContext *ioContext);

	bool handleRecv(SSocketContextPtr &context, IOContext *ioContext);

	bool handleSend(SSocketContextPtr &context, IOContext *ioContext);

	bool handleDisconnect(SSocketContextPtr &context, IOContext *ioContext);

	void handleError(SocketContext *context);

	QueueErrorCode handleError(SocketContext *sockContext, OVERLAPPED *ol, unsigned long result, unsigned long dwBytes);

	void handleConnection(SSocketContextPtr &context, IOContext *ioContext);

	void getHostIP();

	bool isSocketAlive(SSocketContextPtr &ptr);
private:

	LPFN_ACCEPTEX _lpfnAcceptEx;

	LPFN_GETACCEPTEXSOCKADDRS _lpfnGetAcceptExSockAddress;

	LPFN_DISCONNECTEX _lpfnDisconnectEx;

	LPFN_CONNECTEX _lpfinConnectEx;

	Event _shutdownEvent;

	HANDLE _IOCPHandle;

	int _threadCount;
	char _hostIP[MAX_PATH];
	OperationHandler *_opHandler;
	SocketContextManager *_clientManager;
	IOBufferManager *_acceptContextManger;
	SSocketContextPtr _listenContext;
	IOBufferManager *_bufferManager;
	IOTaskManager *_sendTaskManager;
	TimeWheelTimer *_timeWheelTimer;
};