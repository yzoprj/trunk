#include "CThreadTimer.h"
#include <Windows.h>
#include <stdio.h>



//VOID CALLBACK TimerAPCProc(
//	LPVOID lpArg,               // Data value
//	DWORD dwTimerLowValue,      // Timer low value
//	DWORD dwTimerHighValue )    // Timer high value
//
//{
//	((CTimer *)lpArg)->run();
//	// Formal parameters not used in this example.
//	printf("11111111111111111");
//
//}



CThreadTimer::CThreadTimer(int milliseconds /* = 1000 */, unsigned long exitWaitTime /* = -1 */, const char *timerName /* = "CTimer" */)
	:CRTThreadEx(exitWaitTime)
{
	_waitTimes = milliseconds;
	this->setNameInternal(timerName);
}

CThreadTimer::~CThreadTimer(void)
{

}



void CThreadTimer::run()
{
	while(running == true)
	{
		Sleep(_waitTimes);
		this->timeout();
	}
}
