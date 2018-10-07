#pragma once
#include "../../common/TimeWheel.h"
#include "../../thread/WaitableTimer.h"
#include "IOCPContext.h"
#include "../../thread/SyncObjects.h"
class TimeWheelTimer : public CWaitableTimer
{
public:

	class IOCPContextTask : public TimeWheelTaskBase
	{
	public:
		IOCPContextTask(SSocketContextPtr &ptr, const char *taskName = "")
			:TimeWheelTaskBase(taskName)
		{
			_ptr = ptr;
		}

		void execute()
		{

			printf("[%s]:Connection[%s] is timeout !!!!!!!\n", getCurrentTime().c_str(), _ptr->contextName);
			if (_ptr.use_count() <= 2)
			{
				_ptr->close();
			}
		}
	private:
		SSocketContextPtr _ptr;
	};

	TimeWheelTimer(long size = 120);
	~TimeWheelTimer(void);

	void timeout();

	void addContext(SSocketContextPtr &ptr);
private:
	TimeWheel *_timeWheel;
	CriticalSection _cs;
	int _slotSize;

};

