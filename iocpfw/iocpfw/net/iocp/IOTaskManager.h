#pragma once



#include "IOCPContext.h"

struct IOTask
{
	int key;

	long long totalBytes;
	long long opBytes;
	list<IOContext *> ioList;

	IOTask();

	~IOTask();

	IOContext *createNewContext();

	bool isFinished();

	void increment(long long bytes);

	void clear();

};


class IOTaskManager
{
public:
	
	IOTaskManager();

	~IOTaskManager();

	IOTask *createNewTask();

	void removeTask(int key);

	void clearAll();

private:
	map<int, IOTask *> _taskMap;
	CriticalSection _cs;
};
