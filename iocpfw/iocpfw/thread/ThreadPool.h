#ifndef __ORNG_THREADPOOL_H__
#define __ORNG_THREADTOOL_H__

#include <map>
#include <string>
#include <list>
#include <utility>



using std::map;
using std::list;
using std::string;
using std::make_pair;

#include "Thread.h"
#include "SyncObjects.h"
#include "Event.h"
#include "Task.h"



class ThreadPool : public CThread
{
public:

	ThreadPool(DWORD minThrdSize = 10, DWORD maxThrdSize = 20);
	~ThreadPool(void);
	BOOL append(Task *task);
	DWORD stop();
	void init();
protected:

	class CPoolThread : public CThread
	{
	public:
		explicit CPoolThread(ThreadPool *thrdMgr);
	protected:
		virtual void run();
	private:
		ThreadPool *threadManager;
	};
	//friend class CPoolThread;
	void controlThread();
	void insertFreeThread();
	void deleteFreeThread();
	virtual void run();

private:
	map<CThread *, CThread *> freeMap;
	map<CThread *, CThread *> busyMap;
	list<Task *> taskQueue;
	list<CThread *> thrdList;
	Semaphore thrdSem;
	Semaphore taskSem;
	SyncMutex taskMutex;
	SyncMutex thrdMutex;
	size_t minTaskCount;
	size_t maxTaskCount;
	
	
	
};



#endif


