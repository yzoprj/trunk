#pragma once
class CTimer
{
public:
	CTimer(const char *timerName = "CTimer");
	~CTimer(void);

	void start(int milliseconds = 1000, unsigned long dueTime = 0);
	void stop();

	
	static void __stdcall timerRoutine(void *arg, unsigned long dwTimerLowValue,
												 unsigned long dwTimerHighValue );
	virtual void run() = 0;

	void *_handle;
private:

	
};

