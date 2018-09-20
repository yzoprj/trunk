#ifndef __ORNG_CTHREAD_H__
#define __ORNG_CTHREAD_H__
#include <windows.h>



class CThread
{
public:

	enum ThreadExitCode
	{
		EXIT_THREAD = 0,
		RUN_THREAD = 1
	};
	CThread(void);
	CThread(DWORD timeout);
	~CThread(void);

	DWORD suspend();
	
	DWORD start();
	DWORD destroy();
	DWORD resume();
	DWORD wait(DWORD waitTime);
	void setExitCode(DWORD isExit) { exitCode = isExit;}
	
	HANDLE getThreadHandle() {return hThread;}
	DWORD getThreadID() const {return threadID;}
	BOOL isBusy() const {return busied;}
	BOOL isRunning() const { return ran;};
	virtual DWORD stop();
protected:
	virtual void run() = 0;
private:
	static DWORD CALLBACK thrdProc(void * arg);


protected:
	DWORD exitCode;
	DWORD timeout;
	BOOL busied;
private:
	
	HANDLE hThread;
	DWORD threadID;
	
	BOOL ran;
	
	
};


#endif
