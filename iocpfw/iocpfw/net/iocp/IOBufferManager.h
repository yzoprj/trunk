#pragma once

#include "IOCPContext.h"
class IOBufferManager
{
public:
	IOBufferManager(void);
	~IOBufferManager(void);

	void addNewIoBuffer(int key);

	void removeIoBuffer(int key);


	vector<char> *getBufferByKey(int key);

	void clearAll();

private:
	CriticalSection _cs;
	map<int, vector<char> *> _ioBufferMap;
};

