#pragma once

#include "IOCPContext.h"

class SocketContextManager
{
public:
	SocketContextManager(void);
	
	~SocketContextManager(void);

	void clearAll();

	SSocketContextPtr getNewContext();

	static SSocketContextPtr createNewContext();

	static string getClientName(const SSocketContextPtr &context);

	void addNewClient(SSocketContextPtr context);

	void addNewClient(const string clientName, SSocketContextPtr context);

	void removeClient(const string clientName);

	void removeContext(SSocketContextPtr context);

	WSocketContextPtr getContext(const string clientName);

	SSocketContextPtr getContext(SocketContext *context);

	bool isContextExisted(const SocketContext *context);

private:
	unordered_map<long , SSocketContextPtr> _connectionMap;
	unordered_map<string, WSocketContextPtr> _clientMap;
	CriticalSection _cs;
	
};

