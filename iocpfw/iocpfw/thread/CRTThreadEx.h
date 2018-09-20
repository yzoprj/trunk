
#ifndef _CRT_THREAD_H_
#define _CRT_THREAD_H_
#include <windows.h>


		class CRTThreadEx
		{
		public:

			enum ThreadExitCode
			{
				EXIT_THREAD = 0,
				RUN_THREAD = 1
			};
			CRTThreadEx(void);
			CRTThreadEx(DWORD timeout);
			~CRTThreadEx(void);

			bool suspend();

			bool start();
			bool destroy();
			bool resume();
			bool wait(DWORD waitTime);
			void setExitCode(DWORD isExit) { exitCode = isExit;}
			void setNameInternal(const char* name);  
			void closeHandle();
			HANDLE getThreadHandle() {return hThread;}
			unsigned int getThreadID() const {return threadID;}
			bool isBusy() const {return busied;}
			bool isRunning() const { return running;};
			virtual bool stop();
		protected:
			virtual void run() = 0;
		private:
			
			static unsigned int CALLBACK thrdProc(void * arg);


		protected:

			DWORD exitCode;
			DWORD timeout;
			bool busied;
		private:

			HANDLE hThread;
			unsigned int threadID;

			bool running;


		};


#endif
