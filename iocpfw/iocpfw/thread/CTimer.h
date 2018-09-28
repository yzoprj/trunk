#pragma once
class CTimer
{
public:
	CTimer(void);
	~CTimer(void);

	void start();
	void stop();
	void setInternal(int microseconds);
	virtual void run() = 0;


private:

	void *handle;
};

