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
	_clientMaps.clear();

	list<SocketContext *>::iterator iter = _connectionList.begin();
	while (iter != _connectionList.end())
	{
		delete *iter;
		iter++;
	}

	_connectionList.clear();
}

SocketContext *SocketContextManager::getNewContext()
{
	MutexGuard gurad(_cs);
	SocketContext *ctx = createNewContext();
	ctx->index = _connectionList.size();
	_connectionList.push_back(ctx);

	return ctx;
}

SocketContext *SocketContextManager::createNewContext()
{
	return new SocketContext;
}

void SocketContextManager::addNewClient(string clientName, SocketContext *context)
{
	MutexGuard guard(_cs);
	_clientMaps.insert(make_pair(clientName, context));
}

string SocketContextManager::getClientName(SocketContext *context)
{

	char buffer[128] = {0};
	char keyBuffer[128] = {0};


	sprintf(keyBuffer, "%s:%d",inet_ntop(AF_INET,&context->sockAddr.sin_addr, buffer, 128),
		htons(context->sockAddr.sin_port));

	return keyBuffer;

}

void SocketContextManager::addNewClient(SocketContext *context)
{
	addNewClient(getClientName(context), context);
}

void SocketContextManager::removeContext(SocketContext *context)
{
	MutexGuard guard(_cs);
	_connectionList.remove(context);
	delete context;
}

void SocketContextManager::removeClient(string clientName)
{
	MutexGuard guard(_cs);
	map<string, SocketContext*>::iterator iter = _clientMaps.find(clientName);
	if (iter != _clientMaps.end())
	{
		_connectionList.remove(iter->second);
		delete iter->second;
	}

	_clientMaps.erase(clientName);


}