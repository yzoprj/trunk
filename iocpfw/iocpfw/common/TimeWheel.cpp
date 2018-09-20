#include "TimeWheel.h"
#include <time.h>


TimeWheel::TimeWheel(string wheelName, int slotSize, int unitTime)
{
	_wheelName = wheelName;
	_slotSize = slotSize;
	_unitTime = unitTime;
	_highLevelWheel = NULL;
	_lowLevelWheel = NULL;
	_currentSlot = -1;
	_taskQueue.resize(_slotSize, vector<TimeWheelTaskBase>());
}


TimeWheel::~TimeWheel()
{
	detachTimeWheel();
}



bool TimeWheel::insertTask(TimeWheelTaskBase &task, unsigned int executeTime)
{
	unsigned int resideTime = getResideTime();
	unsigned int totalTime = getTotalTimeInWheel();

	// when execute time is less then a unit time
	// push the tasks to low level wheel;
	if (executeTime < _unitTime)
	{
		if (_lowLevelWheel != NULL)
		{
			_lowLevelWheel->insertTask(task, executeTime);
			return true;
		}
	}

	// when execute time is higher then the time in the wheel
	// push the task to the high level wheel
	if (executeTime > totalTime)
	{
		if (_highLevelWheel == NULL)
		{
			return false;
		}
		_highLevelWheel->insertTask(task, executeTime);
	}

	int slot = 0;
	int delta = 0;

	// when the task pushed from low level wheel
	if (_lowLevelWheel != NULL)
	{
		// minus the reside time in the wheel
		int newExecuteTime = executeTime - (totalTime - resideTime);
		slot = newExecuteTime / _unitTime - 1;
		delta = newExecuteTime % _unitTime;

		if(delta > 0)
		{
			slot = slot + 1;
		}
	}
	else
	{
		slot = executeTime / _unitTime;
		delta = executeTime % _unitTime;
	}

	if (slot < 0)
	{
		slot = 0;
	}

	if (slot <= 0 && _currentSlot < 0)
	{
		slot = 1;
	}

	slot = (slot + _currentSlot) % _slotSize;
	_taskQueue[slot].push_back(task);
	return true;
}


void TimeWheel::updateWheel()
{
	_currentSlot++;

	if (_currentSlot == _slotSize)
	{
		_currentSlot = _currentSlot % _slotSize;

		if (_highLevelWheel != NULL)
		{
			_highLevelWheel->updateWheel();
		}
	}

	if (_lowLevelWheel != NULL)
	{
		time_t currentTime = time(NULL);
		for (size_t i = 0; i < _taskQueue[_currentSlot].size(); i++)
		{
			unsigned int taskExecuteTime = _taskQueue[_currentSlot][i].getTime();
			unsigned int delta = taskExecuteTime - (unsigned int)currentTime;
			if (delta < _unitTime)
			{
				_lowLevelWheel->insertTask(_taskQueue[_currentSlot][i], (int)delta);
			}
		}

		vector<TimeWheelTaskBase>::iterator iter = _taskQueue[_currentSlot].begin();

		while (iter != _taskQueue[_currentSlot].end())
		{
			unsigned int taskExecuteTime = iter->getTime();
			unsigned int delta = taskExecuteTime - (unsigned int)currentTime;

			if (delta < _unitTime)
			{
				_taskQueue[_currentSlot].erase(iter);
				iter = _taskQueue[_currentSlot].begin();
				continue;
			}
			iter++;
		}


	}

}



unsigned int TimeWheel::getResideTime()
{
	int resideTime = (_slotSize - _currentSlot - 1) * _unitTime;
	if (_lowLevelWheel != NULL)
	{
		resideTime += _lowLevelWheel->getResideTime();
	}

	return resideTime;
}

unsigned int TimeWheel::getTotalTimeInWheel()
{
	return _slotSize * _unitTime;
}


void TimeWheel::execute()
{
	time_t currentTime = time(NULL);
	if (_currentSlot < 0 )
	{
		return;
	}

	vector<TimeWheelTaskBase>::iterator iter = _taskQueue[_currentSlot].begin();
	while (iter != _taskQueue[_currentSlot].end())
	{
		unsigned int taskExecuteTime = iter->getTime();
		unsigned int delta = taskExecuteTime - (unsigned int)currentTime;

		if (delta < _unitTime)
		{
			iter->Execute();
			_taskQueue[_currentSlot].erase(iter);
			iter = _taskQueue[_currentSlot].begin();
			continue;
		}
		iter++;
	}
}


void TimeWheel::attachTimeWheel(TimeWheel *timeWheel)
{
	timeWheel->_unitTime = getTotalTimeInWheel();
	timeWheel->_currentSlot = 0;
	timeWheel->_lowLevelWheel = this;
	_highLevelWheel = timeWheel;
}


void TimeWheel::detachTimeWheel()
{
	if (_highLevelWheel != NULL)
	{
		delete _highLevelWheel;
		_highLevelWheel = NULL;
	}

}

void TimeWheel::detachTimeWheel(TimeWheel *timeWheel)
{

}

string  getCurrentSlotStatus()
{
	string str;
	return str;
}


class TimeWheelManager
{
public:

	TimeWheelManager(int timeout = 1)
	{

	}

	void start()
	{

	}

	void stop()
	{

	}

	void bulidWheel()
	{
		_timeWheel = new TimeWheel();
	}

	void addTask(TimeWheelTaskBase &task, unsigned int executeTime)
	{
		_timeWheel->insertTask(task, executeTime);
	}

	void execute()
	{
		_timeWheel->updateWheel();
		_timeWheel->execute();
	}

private:
	unsigned int timerHandler;
	TimeWheel *_timeWheel;
	
	static TimeWheelManager instance();

};
