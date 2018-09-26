#include "IOTaskManager.h"


IOTask::~IOTask()
{
	clear();
}

IOContext *IOTask::createNewContext()
{
	IOContext *ctx = new IOContext;
	ctx->owner = this;
	ctx->index = ioList.size();
	ioList.push_back(ctx);
	return ctx;
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
	if (ioList.size() > 0)
	{
		if (ioList.back()->isLast == true && ioList.back()->isFinished())
		{
			return true;
		}else
		{
			return false;
		}
	}else
	{
		return true;
	}

	return false;
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
	task->key = (int)task;
	_taskMap.insert(make_pair((int)task, task));

	return task;
}

void IOTaskManager::removeTask(int key)
{
	MutexGuard gurad(_cs);
	map<int, IOTask*>::iterator iter = _taskMap.find(key);
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
	map<int, IOTask *>::iterator iter = _taskMap.begin();

	while (iter != _taskMap.end())
	{
		delete iter->second;
		iter++;
	}
}