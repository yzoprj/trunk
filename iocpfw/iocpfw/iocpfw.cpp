// iocpfw.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "thread/CTimer.h"
#include "net/iocp/IOCP.h"
#include "net/iocp/IOCPThreadManager.h"
#include "thread/CRTThreadEx.h"
#include <Windows.h>

int main(int argc, char* argv[])
{
	//IOCPManager manager;
	//IOCPManager::initializeSocketLibrary();
	//

	//manager.initializeIOCP();
	//manager.initializeListenSocket();
	//manager.run();
	//
	//manager.deinitializeSocketLibrary();

	IOCPThreadManager manager;
	manager.init();
	manager.start();
	manager.waitAll();

	return 0;
}

