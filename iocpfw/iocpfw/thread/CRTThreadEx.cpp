#include "CRTThreadEx.h"
#include <Windows.h>
#include <process.h>



const DWORD kVCThreadNameException = 0x406D1388;  

typedef struct tagTHREADNAME_INFO {  
	DWORD dwType;  // Must be 0x1000.  
	LPCSTR szName;  // Pointer to name (in user addr space).  
	DWORD dwThreadID;  // Thread ID (-1=caller thread).  
	DWORD dwFlags;  // Reserved for future use, must be zero.  
} THREADNAME_INFO;  

CRTThreadEx::CRTThreadEx(void)
{


	running = FALSE;
	busied = FALSE;
	exitCode = 0;
	_objectWaitTime = INFINITE;
	hThread = (HANDLE)_beginthreadex(NULL, 0, thrdProc, this, CREATE_SUSPENDED, &threadID);
}


CRTThreadEx::CRTThreadEx(unsigned long exitTime)
{

	running = FALSE;
	busied = FALSE;
	this->_objectWaitTime = exitTime;
	exitCode = 0;
	
	hThread = (HANDLE)_beginthreadex(NULL, 0, thrdProc, this, CREATE_SUSPENDED, &threadID);
}
bool CRTThreadEx::start()
{
	running = true;
	if (hThread != INVALID_HANDLE_VALUE)
	{
		exitCode = 1;
		resume();
	}else
	{
		exitCode = 1;
		//runningFlag = TRUE;
		hThread = (HANDLE)_beginthreadex(NULL, 0, thrdProc, this, CREATE_SUSPENDED, &threadID);
		resume();

	}

	return true;
}

bool CRTThreadEx::resume()
{
	if (ResumeThread(hThread) != -1)
	{
		running = TRUE;
		
		return true;
	}
	running = FALSE;
	return false;
}

bool CRTThreadEx::stop()
{
	if (running == true)
	{

		exitCode = 0;
		running = false;
		if (WaitForSingleObject(hThread, _objectWaitTime) == WAIT_TIMEOUT)
		{
			destroy();
		}
		
		
	
	}
	closeHandle();
	return true;
}

bool CRTThreadEx::destroy()
{
	running = false;

	// unsafe code 
	exitCode = -1;
	TerminateThread(hThread, exitCode);
	closeHandle();
	return  true;
}


bool CRTThreadEx::suspend()
{
	if (SuspendThread(hThread) != -1)
	{
		running = false;
	}


	return  true;
}



CRTThreadEx::~CRTThreadEx(void)
{

	stop();
}


unsigned int CALLBACK CRTThreadEx::thrdProc(void * arg)
{
	CRTThreadEx *thrd = (CRTThreadEx *)arg;
	thrd->run();
	return 0;
}

bool CRTThreadEx::wait(DWORD waitTime)
{
	return WaitForSingleObject(hThread, waitTime * 1000) == WAIT_OBJECT_0 ? true : false;
}

void CRTThreadEx::run()
{

}


void CRTThreadEx::setNameInternal(const char* name)
{
	//只在调试的时候生效   
	if (!::IsDebuggerPresent())   
		return;  
	THREADNAME_INFO info;  
	info.dwType = 0x1000;  
	info.szName = name;  
	info.dwThreadID = threadID;  
	info.dwFlags = 0;  

	__try  
	{  
		RaiseException(kVCThreadNameException, 0, sizeof(info)/sizeof(DWORD), reinterpret_cast<DWORD_PTR*>(&info));  
	}  
	__except(EXCEPTION_CONTINUE_EXECUTION)   
	{  
	}  
}


const char* GetThreadName(void)
{
	char* pszName=NULL;
	__asm{
		mov eax, fs:[0x18]    //    Locate the caller's TIB
		mov eax, [eax+0x14]    //    Read the pvArbitary field in the TIB
		mov [pszName], eax    //    pszName = pTIB->pvArbitary
	}
	return pszName ? pszName : "Win32 Thread";
}

void CRTThreadEx::closeHandle()
{
	if (hThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hThread);
		hThread = INVALID_HANDLE_VALUE;
	}
}