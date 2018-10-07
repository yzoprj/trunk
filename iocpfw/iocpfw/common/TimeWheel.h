#pragma once


#include <string>
#include <vector>
#include <time.h>
using std::string;
using std::vector;

class TimeWheelTaskBase
{

public:

	TimeWheelTaskBase(string taskName = "")
	{
		_taskName = taskName;
	}


	void setTime(time_t exeTime)
	{
		_executeTime = exeTime;
	}

	long long  getTime()
	{
		return _executeTime;
	}

	 virtual void execute()
	{
		//System.Console.WriteLine("执行时间：" + ExecuteTime.ToLongTimeString() + "-执行任务：" + TaskName
		//	+ "-当前时间：" + System.DateTime.Now.ToLongTimeString());
	}

	 virtual ~TimeWheelTaskBase(){}

private:

	string _taskName;
	time_t  _executeTime;
};


class TimeWheel
{
public:
	TimeWheel(string wheelName = "", int slotSize = 60, int unitTime = 1);
	~TimeWheel();

	bool insertTask(TimeWheelTaskBase *task, int executeTime);

	void updateWheel();

	unsigned int getResideTime();

	unsigned int getTotalTimeInWheel();

	TimeWheelTaskBase detachTask();

	virtual void execute();

	void attachTimeWheel(TimeWheel *timeWheel);

	void detachTimeWheel();

	string getCurrentSlotStatus();


private:
	void detachTimeWheel(TimeWheel *timeWheel);
	
private:
	vector<vector<TimeWheelTaskBase *> > _taskQueue;
	TimeWheel *_highLevelWheel;
	TimeWheel *_lowLevelWheel;
	string _wheelName;
	unsigned int _currentSlot ;
	unsigned int _unitTime ;
	unsigned int _slotSize;

};

