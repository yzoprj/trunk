#include "CTimer.h"
#include <Windows.h>
#include <stdio.h>
CTimer::CTimer(const char *timerName /* = "CTimer" */)
{
	_handle = CreateWaitableTimer(NULL, FALSE, timerName);
}


CTimer::~CTimer(void)
{
}


VOID CALLBACK TimerAPCProc(
	LPVOID lpArg,               // Data value
	DWORD dwTimerLowValue,      // Timer low value
	DWORD dwTimerHighValue )    // Timer high value

{
	((CTimer *)lpArg)->run();
	// Formal parameters not used in this example.
	printf("11111111111111111");

}


void CTimer::start(int milliseconds /* = 1000 */, unsigned long dueTime /* = 0 */)
{
	static LARGE_INTEGER   liDueTime;
	__int64 qwDueTime = -1 * 10000000;

	// Copy the relative time into a LARGE_INTEGER.
	liDueTime.LowPart  = (DWORD) ( qwDueTime & 0xFFFFFFFF );
	liDueTime.HighPart = (LONG)  ( qwDueTime >> 32 );

	int result = SetWaitableTimer(_handle, &liDueTime, 2000, TimerAPCProc, this, FALSE);
}

void CTimer::stop()
{
	SetWaitableTimer(_handle, NULL, 0, NULL, NULL, 0);

}


void __stdcall CTimer::timerRoutine(void *arg, 
	unsigned long dwTimerLowValue, unsigned long dwTimerHighValue )
{
	
}