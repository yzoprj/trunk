#include "IOBufferManager.h"


IOBufferManager::IOBufferManager(void)
{
}


IOBufferManager::~IOBufferManager(void)
{
	clearAll();
}

IOBuffer * IOBufferManager::addNewIoBuffer(int key)
{
	MutexGuard guard(_cs);
	IOBuffer *ioBuffer = new IOBuffer;
	ioBuffer->key = key;
	_ioBufferMap.insert(make_pair(key, ioBuffer));
	return ioBuffer;
}


IOBuffer * IOBufferManager::addNewIoBuffer(IOBuffer *ioBuffer)
{
	_ioBufferMap.insert(make_pair(ioBuffer->key, ioBuffer));
	return ioBuffer;
}

void IOBufferManager::removeIoBuffer(long key)
{
	MutexGuard guard(_cs);
	IoBufferMapIterator iter = _ioBufferMap.find(key);

	if (iter != _ioBufferMap.end())
	{
		delete iter->second;
		_ioBufferMap.erase(key);
	}

}


IOBuffer * IOBufferManager::getNewBuffer()
{
	IOBuffer *ioBuffer = new IOBuffer;
	ioBuffer->key = (int)ioBuffer;
	return ioBuffer;
}

IOBuffer *IOBufferManager::getBufferByKey(int key)
{
	IoBufferMapIterator iter = _ioBufferMap.find(key);

	if (iter == _ioBufferMap.end())
	{
		return NULL;
	}

	return iter->second;
}

void IOBufferManager::clearAll()
{
	MutexGuard guard(_cs);
	IoBufferMapIterator iter = _ioBufferMap.begin();

	while (iter != _ioBufferMap.end())
	{
		delete iter->second;
		iter++;
	}
	_ioBufferMap.clear();
}