#pragma once
#include "CRTThreadEx.h"

class CThreadTimer :  public CRTThreadEx
{
public:
	CThreadTimer(int milliseconds = 1000,  unsigned long exitWaitTime = -1, const char *timerName = "CTimer");
	~CThreadTimer(void);

	virtual void timeout() = 0;
private:
	void run();

	unsigned long _waitTimes;
};

