#pragma once


#include <string>
#include <vector>

using std::string;
using std::vector;

class TimeWheelTaskBase
{

public:

	TimeWheelTaskBase(string taskName)
	{
		_taskName = taskName;
	}


	void setTime(unsigned int exeTime)
	{
		_executeTime = exeTime;
	}

	unsigned int getTime()
	{
		return _executeTime;
	}

	 virtual void Execute()
	{
		//System.Console.WriteLine("执行时间：" + ExecuteTime.ToLongTimeString() + "-执行任务：" + TaskName
		//	+ "-当前时间：" + System.DateTime.Now.ToLongTimeString());
	}

private:

	string _taskName;
	unsigned int _executeTime;
};


class TimeWheel
{
public:
	TimeWheel(string wheelName = "", int slotSize = 60, int unitTime = 1);
	~TimeWheel();

	bool insertTask(TimeWheelTaskBase &task, unsigned int executeTime);

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
	vector<vector<TimeWheelTaskBase> > _taskQueue;
	TimeWheel *_highLevelWheel;
	TimeWheel *_lowLevelWheel;
	string _wheelName;
	unsigned int _currentSlot ;
	unsigned int _unitTime ;
	unsigned int _slotSize;

};

