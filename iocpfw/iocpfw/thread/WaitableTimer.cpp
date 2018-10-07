#include "WaitableTimer.h"
#include <Windows.h>
#include <string>


CWaitableTimer::CWaitableTimer(unsigned long isManual /* = 1 */, const char * timerName/* = "CWaitableTimer" */, 
				unsigned long threadWaitTime /* = -1 */, const char *threadName /* = "TimerThread" */)
	:CRTThreadEx(threadWaitTime)
{
	int len = strlen(timerName);

	if (len > MAX_PATH)
	{
		len = MAX_PATH;
	}

	this->setNameInternal(threadName);
	_timerName = new char[len  + 1];
	_timerName[len] = 0x00;
	memcpy(_timerName, timerName, len);
	
	_timerHandle = CreateWaitableTimer(NULL, isManual, _timerName);
	_isTimerRunning = false;
}

CWaitableTimer::~CWaitableTimer(void)
{
	delete _timerName;
	quit();
}

void CWaitableTimer::activate(unsigned long periodTime,  long dueTime)
{

	_isTimerRunning = true;
	_dueTime = dueTime * -10000L;
	_periodTime = periodTime;
	start();
}

void CWaitableTimer::reactivate(unsigned long periodTime, long dueTime)
{
	//activate(periodTime, dueTime);
}

void CWaitableTimer::quit()
{
	_isTimerRunning = false;

	if (_timerHandle != INVALID_HANDLE_VALUE)
	{
		CancelWaitableTimer(_timerHandle);

		stop();

		CloseHandle(_timerHandle);
		_timerHandle = INVALID_HANDLE_VALUE;
	}


	
}

void __stdcall CWaitableTimer::timerRoutine(void *arg, unsigned long dwTimerLowValue, unsigned long dwTimerHighValue)
{
	((CWaitableTimer *)arg)->timeout();
}

void CWaitableTimer::run()
{
	LARGE_INTEGER liDueTime;
	liDueTime.LowPart  = (DWORD) ( _dueTime & 0xFFFFFFFF );
	liDueTime.HighPart = (LONG)  ( _dueTime >> 32 );
	SetWaitableTimer(_timerHandle, &liDueTime, _periodTime, timerRoutine, this, FALSE);
	while(_isTimerRunning == true)
	{
		SleepEx(INFINITE, TRUE);
	}
}
