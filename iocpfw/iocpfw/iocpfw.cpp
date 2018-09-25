// iocpfw.cpp : 定义控制台应用程序的入口点。
//
#include "net/iocp/IOCP.h"

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
	IOCPManager manager;
	IOCPManager::initializeSocketLibrary();
	

	manager.initializeIOCP();
	manager.initializeListenSocket();
	manager.run();
	
	manager.deinitializeSocketLibrary();
	return 0;
}

