#pragma once

#include "IOCPContext.h"

class SocketContextManager
{
public:
	SocketContextManager(void);
	
	~SocketContextManager(void);

	void clearAll();

	SocketContext *getNewContext();

	static SocketContext *createNewContext();

	static string getClientName(SocketContext *context);

	void addNewClient(string clientName, SocketContext *context);

	void addNewClient(SocketContext *context);

	void removeContext(SocketContext *context);

	void removeClient(string clientName);

private:
	list<SocketContext *> _connectionList;
	map<string, SocketContext *> _clientMaps;
	CriticalSection _cs;
	
};

