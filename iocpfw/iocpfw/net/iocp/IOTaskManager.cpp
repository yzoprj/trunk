#include "IOTaskManager.h"


IOTask::IOTask()
{
	totalBytes = 0;
	opBytes = 0;
	finishedTimes = 0;
}

IOTask::~IOTask()
{
	clear();
}

IOContext *IOTask::createNewContext(int unitSize, bool usedBuffer /* = true */)
{
	IOContext *ctx = new IOContext(unitSize, usedBuffer);
	ctx->owner = this;
	ctx->index = ioList.size();
	ioList.push_back(ctx);
	return ctx;
}

void IOTask::increment(long long bytes)
{
	InterlockedExchangeAdd64(&opBytes, bytes);
}


void IOTask::clear()
{
	list<IOContext *>::iterator iter = ioList.begin();

	while(iter != ioList.end())
	{
		delete *iter;
		iter++;
	}

	ioList.clear();
}

bool IOTask::isFinished()
{
	//if (ioList.size() > 0)
	//{
	//	if (ioList.back()->isFinished())
	//	{
	//		return true;
	//	}else
	//	{
	//		return false;
	//	}
	//}

	if (totalBytes == opBytes)
	{
		return true;
	}

	return false;
}


bool IOTask::isAllFinishedToClear()
{
	if (finishedTimes == ioList.size())
	{
		return true;
	}

	return false;
}

void IOTask::incrementFinishTimes()
{
	InterlockedIncrement64(&finishedTimes);
}


IOTaskManager::IOTaskManager()
{

}

IOTaskManager::~IOTaskManager()
{
	clearAll();
}

IOTask *IOTaskManager::createNewTask()
{
	MutexGuard guard(_cs);
	IOTask *task = new IOTask;
	task->key = (long)task;
	_taskMap.insert(make_pair((long)task, task));

	return task;
}

void IOTaskManager::removeTask(long key)
{
	MutexGuard gurad(_cs);
	map<long, IOTask*>::iterator iter = _taskMap.find(key);
	if (iter == _taskMap.end())
	{
		return;
	}else
	{
		delete iter->second;
		_taskMap.erase(key);
	}
	
}


void IOTaskManager::clearAll()
{
	map<long, IOTask *>::iterator iter = _taskMap.begin();

	while (iter != _taskMap.end())
	{
		delete iter->second;
		iter++;
	}
}