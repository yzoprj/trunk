#include "SocketContextManager.h"


SocketContextManager::SocketContextManager(void)
{
}


SocketContextManager::~SocketContextManager(void)
{
	clearAll();
}


void SocketContextManager::clearAll()
{
	MutexGuard guard(_cs);
	_clientMap.clear();
	_connectionMap.clear();
}

SSocketContextPtr SocketContextManager::getNewContext()
{
	MutexGuard gurad(_cs);
	SSocketContextPtr ptr = createNewContext();
	
	ptr->index = _connectionMap.size();
	//_connectionMap.insert(make_pair(ctx, ctx));

	return ptr;
}

SSocketContextPtr SocketContextManager::createNewContext()
{
	return SSocketContextPtr(new SocketContext);
}

void SocketContextManager::addNewClient(const string clientName, SSocketContextPtr context)
{
	MutexGuard guard(_cs);
	_clientMap.insert(make_pair(clientName, WSocketContextPtr(context)));
	_connectionMap.insert(make_pair((int)context.get(), context));
}

string SocketContextManager::getClientName(const SSocketContextPtr &context)
{

	char buffer[128] = {0};
	char keyBuffer[128] = {0};


	sprintf(keyBuffer, "%s:%d",inet_ntop(AF_INET,&context->sockAddr.sin_addr, buffer, 128),
		htons(context->sockAddr.sin_port));

	return keyBuffer;

}

void SocketContextManager::addNewClient(SSocketContextPtr context)
{
	addNewClient(getClientName(context), context);
}



void SocketContextManager::removeContext(SSocketContextPtr context)
{
	MutexGuard guard(_cs);
	_connectionMap.erase((int)context.get());
	_clientMap.erase(getClientName(context));
}

void SocketContextManager::removeClient(const string clientName)
{
	MutexGuard guard(_cs);
	unordered_map<string, WSocketContextPtr>::iterator iter = _clientMap.find(clientName);
	if (iter != _clientMap.end())
	{
		_connectionMap.erase((int)(iter->second.lock().get()));

	}

	_clientMap.erase(clientName);


}



WSocketContextPtr SocketContextManager::getContext(const string clientName)
{
	MutexGuard guard(_cs);
	unordered_map<string, WSocketContextPtr>::iterator iter = _clientMap.find(clientName);
	if (iter != _clientMap.end())
	{
		return iter->second;
	}

	return WSocketContextPtr();
}

bool SocketContextManager::isContextExisted(const SocketContext *context)
{
	MutexGuard guard(_cs);
	unordered_map<int, SSocketContextPtr>::iterator iter = _connectionMap.find((int)context);
	if (iter != _connectionMap.end())
	{
		return true;
	}

	return false;
}


SSocketContextPtr SocketContextManager::getContext(SocketContext *context)
{
	MutexGuard guard(_cs);
	unordered_map<int, SSocketContextPtr>::iterator iter = _connectionMap.find((int)context);
	if (iter != _connectionMap.end())
	{
		return iter->second;
	}

	return SSocketContextPtr();
}