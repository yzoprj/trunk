#ifndef _SYNCOBEJCTS_H_
#define _SYNCOBEJCTS_H_

#include <Windows.h>
#include "IMutex.h"


		class CriticalSection : public IMutex
		{
		public:
			CriticalSection()
			{
				InitializeCriticalSection(&cs);
			}

			~CriticalSection()
			{
				DeleteCriticalSection(&cs);
			}

			void* getHandle()
			{
				return &cs;
			}

			void lock()
			{
				EnterCriticalSection(&cs);
			}

			void unlock()
			{
				LeaveCriticalSection(&cs);
			}

		private:
			CRITICAL_SECTION cs;
		};




		class SyncMutex: public IMutex
		{
		public:
			SyncMutex(bool ower, int timeout = INFINITE, const char *name = NULL)
			{
				this->timeout = timeout;
				hMutex = CreateMutex(NULL, ower, (LPCTSTR)name);
			}
			~SyncMutex()
			{
				CloseHandle(hMutex);
			}



			void lock()
			{
				WaitForSingleObject(hMutex, timeout);
			}

			void unlock()
			{
				ReleaseMutex(hMutex);
			}

		private:
			HANDLE hMutex;
			int timeout;
		};


		class Semaphore
		{
		public:
			Semaphore(int initCount, int maxCount, int timeout = INFINITE, const char *name = NULL)
			{
				this->timeout = timeout;
				hSemaphore = CreateSemaphoreA(NULL, initCount, maxCount, name);
			}

			~Semaphore()
			{
				CloseHandle(hSemaphore);
			}


			bool wait()
			{
				return increase();
			}

			void post()
			{
				release();
			}


		private:

			bool increase()
			{
				return WaitForSingleObject(hSemaphore, timeout) == WAIT_OBJECT_0  ? true : false;
			}

			void release()
			{
				ReleaseSemaphore(hSemaphore, 1, NULL);
			}
		private:
			HANDLE hSemaphore;
			int timeout;
		};

#if WINVER > 0x600
		class CSCondition
		{
		public:
			CSCondition() {}
			~CSCondition() {}

		};
#endif





#endif