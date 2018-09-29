#pragma once

#include "CRTThreadEx.h"

class CWaitableTimer : public CRTThreadEx
{
public:
	CWaitableTimer(unsigned long isManual = 1, 
					const char * timerName= "CWaitableTimer",
					unsigned long threadWaitTime = -1, 
					const char *threadName = "TimerThread");
	~CWaitableTimer(void);

	void activate(unsigned long periodTime = 1000, long dueTime = 1);


	void reactivate(unsigned long periodTime = 1000, long dueTime = 1);

	void quit();

	virtual void timeout() = 0;

	static void __stdcall timerRoutine(void *arg,
						unsigned long  dwTimerLowValue,
						unsigned long  dwTimerHighValue);


	

private:
	void run();

private:
	char *_timerName;
	void *_timerHandle;
	bool _isTimerRunning;
	_int64 _dueTime;
	long  _periodTime;

};

