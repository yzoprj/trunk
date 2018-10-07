#include "TimeWheelTimer.h"


TimeWheelTimer::TimeWheelTimer(long size /* = 60 */)
{
	_slotSize = size;
	_timeWheel = new TimeWheel("", size);
}


TimeWheelTimer::~TimeWheelTimer(void)
{
	delete _timeWheel;
}


void TimeWheelTimer::addContext(SSocketContextPtr &ptr)
{
	MutexGuard gurad(_cs);
	IOCPContextTask  *task = new IOCPContextTask(ptr);

	_timeWheel->insertTask(task, _slotSize);
}

void TimeWheelTimer::timeout()
{
	MutexGuard guard(_cs);
	//printf("TimerWheel update time [%s]\n", getCurrentTime().c_str());
	_timeWheel->updateWheel();
	_timeWheel->execute();
}