#pragma once
#include "../thread/CRTThreadEx.h"
#include <memory>
class ServerTask;
class AsyncSocket;
class SocketSession;
class EventLoop : public CRTThreadEx
{
public:
	EventLoop(void);
	~EventLoop(void);

	void run(){};
private:
	ServerTask *task;
	
};

