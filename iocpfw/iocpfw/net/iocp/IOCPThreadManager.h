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
	
	bool init();
	
	void start();
	
	void stop();

	void waitAll();


private:
	IOCPThread **_threads;
	void **_handles; 
	int _threadCount;
	IOCPManager _manager;
};

