#pragma once



#include "IOCPContext.h"

struct IOTask
{
	int key;
	list<IOContext *> ioList;

	~IOTask();
	IOContext *createNewContext();

	bool isFinished();

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
