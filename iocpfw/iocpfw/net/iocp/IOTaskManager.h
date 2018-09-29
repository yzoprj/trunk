#pragma once



#include "IOCPContext.h"

struct IOTask
{
	long key;

	long long totalBytes;
	long long opBytes;
	list<IOContext *> ioList;
	long owner;
	long long finishedTimes;
	IOTask();

	~IOTask();

	IOContext *createNewContext(int unitSize = MAX_BUFFER_LENGTH, bool usedBuffer = true);

	bool isFinished();

	bool isAllFinishedToClear();

	void increment(long long bytes);

	void incrementFinishTimes();

	void clear();

};


class IOTaskManager
{
public:
	
	IOTaskManager();

	~IOTaskManager();

	IOTask *createNewTask();

	void removeTask(long key);

	void clearAll();

private:
	map<long, IOTask *> _taskMap;
	CriticalSection _cs;
};
