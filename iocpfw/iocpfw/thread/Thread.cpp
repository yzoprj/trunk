#include "Thread.h"



CThread::CThread(void)
{
	
	
	busied = FALSE;
	exitCode = 1;
	timeout = 5;
	hThread = CreateThread(NULL, 0, thrdProc, this, CREATE_SUSPENDED, &threadID);
}


CThread::CThread(DWORD timeout)
{


	busied = FALSE;
	this->timeout = timeout;
	exitCode = 0;
	hThread = CreateThread(NULL, 0, thrdProc, this, CREATE_SUSPENDED, &threadID);
}
DWORD CThread::start()
{
	if (hThread != INVALID_HANDLE_VALUE)
	{
		exitCode = 1;
		resume();
		return TRUE;
	}
	
	return FALSE;
}

DWORD CThread::resume()
{
	if (ResumeThread(hThread) == -1)
	{
		ran = TRUE;
		return TRUE;
	}
	ran = FALSE;
	return TRUE;
}
DWORD CThread::stop()
{
	if (ran == FALSE)
	{
		exitCode = 0;
		busied = FALSE;
		return TRUE;
	}
	
	if (busied == TRUE)
	{
		exitCode = 0;
		
		if (WaitForSingleObject(hThread, timeout * timeout) != WAIT_TIMEOUT)
		{
			busied = FALSE;
			ran = FALSE;
			return TRUE;
		}else
		{
			return destroy();
			
		}
	}

	return FALSE;
}

DWORD CThread::destroy()
{
	
	if (TerminateThread(hThread, exitCode) != -1)
	{
		busied = FALSE;
		ran = FALSE;
		return TRUE;
	}
	
	return  FALSE;
}


DWORD CThread::suspend()
{
	if (SuspendThread(hThread) != -1)
	{
		ran = FALSE;
		return TRUE;
	}


	return  FALSE;
}



CThread::~CThread(void)
{
	if (hThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hThread);
	}
}


DWORD CALLBACK CThread::thrdProc(void * arg)
{
	CThread *thrd = (CThread *)arg;
	thrd->run();
	return 0;
}

DWORD CThread::wait(DWORD waitTime)
{
	return WaitForSingleObject(hThread, waitTime * 1000) == WAIT_OBJECT_0 ? TRUE : FALSE;
}