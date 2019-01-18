#ifndef SELECTOREX_H
#define SELECTOREX_H


#include "NetCommon.h"
#include "SelectorKey.h"

class SelectorHandler;

class InetAddress;

class SelectorEx
{
public:
			
public:
	SelectorEx();
	~SelectorEx();
			
	void setHandler(SelectorHandler *handler);

	void run();

	void stop();

	void registerSession(const SocketEx &ss, SelectorKey::SelectOperationType  opType, bool isConnecting = false);
	void registerSession(const SelectorKey &key);
	void unregisterSession(const InetAddress &inet);
			
	
			
protected:

	//string generateKey(int ip, int port, int sockMode);
	
	virtual void handleData(SelectorKey & sk);

	void clearSessions();

	void clearDisconnectionSession();

	void removeConnectingSession(const SelectorKey & sk);
protected:
	map<string, SelectorKey > _sessions;
	map<string, SelectorKey > _connectingSessions;
	map<string, shared_ptr<vector<char> > > _sessionBuffers;
	
	vector<SelectorKey> _disconnectSessions;
	
	SelectorHandler *_handler;

	long _shutdown;

	CriticalSection _cs;
private:

};




#endif // SELECTOREX_H
