#include "ThreadPool.h"



ThreadPool::CPoolThread::CPoolThread(ThreadPool *thrdMgr)
{
	this->threadManager = thrdMgr;
}

void ThreadPool::CPoolThread::run()
{

	Task *task = NULL;
	while (threadManager->thrdSem.wait()  && exitCode)
	{

		{
			MutexGuard mlg(threadManager->thrdMutex);
			busied = TRUE;

			threadManager->freeMap.erase(this);
			threadManager->busyMap.insert(make_pair(this, this));


		}
		{
			MutexGuard mlg(threadManager->taskMutex);
			if (threadManager->taskQueue.size() > 0)
			{
				task = threadManager->taskQueue.front();
				threadManager->taskQueue.pop_front();

			}

		}

		if (task != NULL)
		{
			task->process();
			delete task;
		}
		task = NULL;
		{

			MutexGuard mlg(threadManager->thrdMutex);
			busied = FALSE;

			threadManager->busyMap.erase(this);
			threadManager->freeMap.insert(make_pair(this, this));
		}

	}
}



ThreadPool::ThreadPool(DWORD minTask, DWORD maxTask)
:taskSem(0, maxTask), thrdSem(0, maxTask), thrdMutex(false), taskMutex(false)
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	if (minTask == 0)
	{
		minTask = si.dwNumberOfProcessors;
		if (minTask > maxTask)
		{
			maxTask = minTask * 2;
		}
	}else if (minTask > minTask)
	{
		minTask = si.dwNumberOfProcessors;
		minTask = minTask * 2;
	}
	minTaskCount = minTask;
	maxTaskCount = maxTask;
}

ThreadPool::~ThreadPool(void)
{
}

void ThreadPool::init()
{

	for (size_t i = 0; i < minTaskCount; i++)
	{
		insertFreeThread();
	}
}


BOOL ThreadPool::append(Task *task)
{
	BOOL ret = TRUE;
	MutexGuard mlg(taskMutex);
	if (taskQueue.size() < maxTaskCount)
	{
		taskQueue.push_back(task);
		ret = TRUE;
	}else
	{
		delete task;
		ret = FALSE;
	}
	taskSem.post();
	return ret;
}

void ThreadPool::insertFreeThread()
{
	if (exitCode)
	{
		CThread *thrd = new CPoolThread(this);
		thrdList.push_back(thrd);
		freeMap.insert(make_pair(thrd, thrd));
		thrd->start();

	}

}

void ThreadPool::deleteFreeThread()
{
	if (freeMap.size() > 0)
	{
		map<CThread *, CThread *>::iterator iter = freeMap.begin();
		CThread *thrd = iter->second;
		freeMap.erase(iter);
		thrd->stop();
	}

}


void ThreadPool::controlThread()
{
	int incOrDec = 0;
	size_t maxThreshold = maxTaskCount ;
	size_t midThreshold = (maxTaskCount + minTaskCount) / 2;
	{
		MutexGuard mlg(taskMutex);

		if (taskQueue.size() > midThreshold)
			incOrDec = 1;
		else if (taskQueue.size() != 0  && taskQueue.size() < midThreshold)
			incOrDec = -1;
	}
	{
		MutexGuard mlg(thrdMutex);
		if (incOrDec == 1)
		{
			if ((thrdList.size() + 1) < midThreshold)
			{
				if (freeMap.size() == 0)
					insertFreeThread();

			}
		}else if (incOrDec == -1)
		{
			if (freeMap.size() - 1 > minTaskCount)
				deleteFreeThread();
		}
	}

}

void ThreadPool::run()
{

	while(taskSem.wait() && exitCode)
	{
		busied = TRUE;
		controlThread();
		thrdSem.post();

		printf("Total Thread = [%d], Busy Thread = [%d], Free Thread = [%d], Task Count = [%d]\n", thrdList.size(), busyMap.size()
			, freeMap.size(), taskQueue.size());
		busied = FALSE;
	}
}



DWORD ThreadPool::stop()
{
	{
		{
			MutexGuard mlg(thrdMutex);
			exitCode = 0;
		}
		taskSem.post();
		{
			MutexGuard mlg(taskMutex);
			for(list<Task *>::iterator iter = taskQueue.begin(); iter != taskQueue.end();iter++)	
			{
				delete (*iter);
			}
			taskQueue.clear();

		}
		HANDLE *hThrdArray = new HANDLE[thrdList.size()];
		{
			MutexGuard mgl(thrdMutex);
			int i = 0;
			for (list<CThread *>::iterator iter = thrdList.begin(); iter != thrdList.end(); iter++, i++)
			{
				(*iter)->setExitCode(0);
				hThrdArray[i] = (*iter)->getThreadHandle();
			}
		}
		for(size_t i = 0; i < thrdList.size(); i++)
		{
			thrdSem.post();
		}
		WaitForMultipleObjects(thrdList.size(), hThrdArray, TRUE, thrdList.size() * timeout);
		if (busyMap.size() > 0)
		{
			for(map<CThread *, CThread *>::iterator iter = busyMap.begin(); iter != busyMap.end(); iter++)
			{
				TerminateThread(iter->second->getThreadHandle(), 0);
			}
		}
		printf("Total Thread = [%d], Busy Thread = [%d], Free Thread = [%d]\n", thrdList.size(), busyMap.size()
			, freeMap.size());
		for ( list<CThread *>::iterator iter = thrdList.begin(); iter != thrdList.end(); iter++)
		{
			delete (*iter);
		}

		freeMap.clear();
		busyMap.clear();
		thrdList.clear();
		delete hThrdArray;
		return TRUE;
	}
}