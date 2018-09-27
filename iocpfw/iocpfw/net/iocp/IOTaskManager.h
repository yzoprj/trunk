#pragma once



#include "IOCPContext.h"

struct IOTask
{
	int key;

	long long totalBytes;
	long long opBytes;
	list<IOContext *> ioList;
	SSocketContextPtr owner;
	long long failedTimes;
	IOTask();

	~IOTask();

	IOContext *createNewContext();

	bool isFinished();

	bool isFaliedToClear();

	void increment(long long bytes);

	void incrementFailedTimes();

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
