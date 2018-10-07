#pragma once

#include "IOCPContext.h"

struct IOBuffer
{
	IOContext ioContext;
	vector<char> cache;
	int key;
};

class IOBufferManager
{
public:
	IOBufferManager(void);
	~IOBufferManager(void);

	IOBuffer * addNewIoBuffer(int key);

	IOBuffer * addNewIoBuffer(IOBuffer *ioBuffer);
	void removeIoBuffer(long key);

	static IOBuffer *getNewBuffer();

	

	IOBuffer *getBufferByKey(int key);

	void clearAll();

private:
	CriticalSection _cs;
	map<int, IOBuffer *> _ioBufferMap;
};

