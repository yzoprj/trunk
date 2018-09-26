#include "IOCPThreadManager.h"
#include <Windows.h>

IOCPThread::IOCPThread(IOCPManager *iocp)
{
	_iocp = iocp;
}

void IOCPThread::run()
{
	_iocp->run();
}


IOCPThreadManager::IOCPThreadManager(void)
{
}


IOCPThreadManager::~IOCPThreadManager(void)
{
}


bool IOCPThreadManager::init()
{
	SYSTEM_INFO sysInfo;

	GetSystemInfo(&sysInfo);

	_threadCount = sysInfo.dwNumberOfProcessors * 2;

	_threads = new IOCPThread* [_threadCount];
	
	_handles = new void *[_threadCount];
	
	char buffer[128] = {0};
	for (int i = 0; i < _threadCount; i++)
	{
		_threads[i] = new IOCPThread(&_manager);
		_handles[i] = _threads[i]->getThreadHandle();
		sprintf(buffer, "IOCPThread[%d]", i);
		_threads[i]->setNameInternal(buffer);
	
	}


	_manager.initializeSocketLibrary();
	_manager.initializeIOCP(_threadCount);
	_manager.initializeListenSocket();

	return true;
}


void IOCPThreadManager::start()
{
	for (int i = 0; i < _threadCount; i++)
	{
		_threads[i]->start();
	}
}

void IOCPThreadManager::stop()
{
	

	_manager.shutdown();
	for (int i = 0; i < _threadCount; i++)
	{
		_manager.postShutdown();
	}


	waitAll();

	for (int i = 0; i < _threadCount ; i++)
	{
		delete _threads[i];
	}

	delete _handles;
	_manager.closeAll();
	_manager.deinitialize();
	_manager.deinitializeSocketLibrary();
}

void IOCPThreadManager::waitAll()
{
	if(WaitForMultipleObjects(_threadCount, _handles, TRUE, INFINITE) != WAIT_OBJECT_0)
	{
		return;
	}
}