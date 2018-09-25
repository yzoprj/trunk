#include "IOBufferManager.h"


IOBufferManager::IOBufferManager(void)
{
}


IOBufferManager::~IOBufferManager(void)
{
	clearAll();
}

void IOBufferManager::addNewIoBuffer(int key)
{
	MutexGuard guard(_cs);
	_ioBufferMap.insert(make_pair(key, new vector<char>()));
}

void IOBufferManager::removeIoBuffer(int key)
{
	MutexGuard guard(_cs);
	IoBufferMapIterator iter = _ioBufferMap.find(key);

	if (iter != _ioBufferMap.end())
	{
		delete iter->second;
		_ioBufferMap.erase(key);
	}

}


vector<char > *IOBufferManager::getBufferByKey(int key)
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