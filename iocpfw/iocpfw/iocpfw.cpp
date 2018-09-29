// iocpfw.cpp : 定义控制台应用程序的入口点。
//

#include "net/iocp/IOCP.h"
#include "net/iocp/IOCPThreadManager.h"
#include "thread/CRTThreadEx.h"
#include "thread/CThreadTimer.h"
#include "thread/WaitableTimer.h"


void printSystemTime()
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	printf("%d/%02d%/%02d %02d:%02d:%02d %03d\n",
						st.wYear,
						st.wMonth,
						st.wDay,
						st.wHour,
						st.wMinute,
						st.wSecond,
						st.wMilliseconds);
	
}

class TestTimer : public CWaitableTimer
{
	void timeout()
	{
		static int i = 1;
		printSystemTime();

		if (i == 1)
		{
			Sleep(10000);
			i++;
		}
	}
};



int main(int argc, char* argv[])
{
	IOCPManager manager;
	IOCPManager::initializeSocketLibrary();
	

	//manager.initializeIOCP();
	manager.initWSAFunction();
	//manager.initializeListenSocket();
	//manager.run();
	
	//manager.deinitializeSocketLibrary();

	//IOCPThreadManager manager;
	//manager.init();
	//manager.start();
	//manager.waitAll();



	return 0;
}

