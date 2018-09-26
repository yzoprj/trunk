// iocpfw.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "net/iocp/IOCP.h"
#include "net/iocp/IOCPThreadManager.h"
static string getWindowsErrorMessage(DWORD errCode)
{
	char buffer[1024] = {0};

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		errCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)buffer,
		1024, NULL);

	return buffer;

}

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

