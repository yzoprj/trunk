#ifndef _CONCURRENT_QUEUE_H_
#define _CONCURRENT_QUEUE_H_

#include <queue>
#include <string>
#include "SyncObjects.h"
using std::deque;
using std::string;


	template <typename T>
	class ConcurrentQueue
	{
	public:
		ConcurrentQueue(void) {}
		~ConcurrentQueue(void) {}
		void insert(T t)
		{
			MutexGuard mg(cs);
			objs.push_back(t);
		}
		void remove(T t)
		{
			//MutexGuard(cs);

			//if ()
			//{
			//}
			//T result = objs.front();
			//objs.pop_front();

			//return result;
		}

		T removeFront()
		{
			T result;
			MutexGuard mg(cs);
			if (objs.size() > 0)
			{
				result = objs.front();
				objs.pop_front();
			}

			return result;
		}
		void clear()
		{
			objs.clear();
		}

		int getCount()
		{
			MutexGuard mg(cs);
			return objs.size();
		}

	private:
		
		CriticalSection cs;
		deque<T> objs;
	};


#endif

