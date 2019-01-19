
#ifndef _CRT_THREAD_H_
#define _CRT_THREAD_H_

class CRTThreadEx
{
public:

	enum ThreadExitCode
	{
		EXIT_THREAD = 0,
		RUN_THREAD = 1
	};
	CRTThreadEx(void);
	CRTThreadEx(unsigned long exitTime);
	~CRTThreadEx(void);

	bool suspend();

	bool start();
	bool destroy();
	bool resume();
	bool wait(unsigned long waitTime);
	void setExitCode(unsigned long isExit) { exitCode = isExit;}
	const char* getThreadName(void);
	void setNameInternal(const char* name);  
	void closeHandle();
	void * getThreadHandle() {return hThread;}
	unsigned int getThreadID() const {return threadID;}
	bool isBusy() const {return busied;}
	bool isRunning() const { return running;};
	virtual bool stop();
protected:
	virtual void run() = 0;
private:

	static unsigned int __stdcall thrdProc(void * arg);


protected:

	unsigned long exitCode;
	unsigned long _objectWaitTime;
	bool busied;
	bool running;
private:

	void * hThread;
	unsigned int threadID;

	


};


#endif
