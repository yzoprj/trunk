#pragma once

#include "IOCP.h"
#include "../../thread/CRTThreadEx.h"

class IOCPThread : public CRTThreadEx
{
public:
	IOCPThread(IOCPManager *iocp);

	void run();

private:
	IOCPManager *_iocp;
};

class IOCPThreadManager
{
public:
	IOCPThreadManager(void);
	~IOCPThreadManager(void);
	
	bool init(int threadCount = 0);
	
	void start();
	
	void stop();

	void waitAll();

	long send(string clientKey, const char *buffer, long long length, bool isCopy = true, int unitSize = MAX_BUFFER_LENGTH);

private:
	IOCPThread **_threads;
	void **_handles; 
	int _threadCount;
	IOCPManager _manager;
};

