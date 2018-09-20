#ifndef __ORNG_SYNCQUEUE_HPP__
#define __ORNG_SYNCQUEUE_HPP__
#include "SyncObjects.h"
#include "../common/ObjectProxy.hpp"
#include "../common/Common.h"
#include <deque>

using JKProtocol::Common::ObjectProxy;
using std::deque;



template<typename T>
class SyncQueue
{
public:
	SyncQueue() {}
	~SyncQueue() {}

	void push_back(T *context)
	{
		MutexGuard guard(cs);
		objQueue.push_back(ObjectProxy(context));
	}

	ObjectProxy<T *> pop()
	{
		MutexGuard guard(cs);
		ObjectProxy<T *> op = objQueue.front();
		objQueue.pop_front();
		return op;
	}

	uint size() { return objQueue.size();}

private:
	CriticalSection cs;
	deque<ObjectProxy<T *> > objQueue;
};


#endif