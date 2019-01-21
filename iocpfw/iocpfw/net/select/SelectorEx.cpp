#include "SelectorEx.h"
//#include <WinSock2.h>
//#include <Windows.h>



#include "SelectorHandler.h"
#include "SocketEx.h"
#include "NetHelper.h"
#include "InetAddress.h"



typedef map<string, SelectorKey>::iterator SelectorKeyIter;

typedef map<string, shared_ptr<vector<char> > >::iterator BufferIter;


class DefaultSelectorHandler : public SelectorHandler
{

public:
	
	virtual bool handleConnected(SelectorKey &key) 
	{
		return true;
	}

	virtual bool handleAccept(SelectorKey &key, SelectorKey &newKey)
	{
		return true;
	}


	virtual bool handleRead(SelectorKey &key, shared_ptr<vector<char> > &buffer)
	{
		return true;
	}


	virtual bool handleWrite(SelectorKey &key, const RawBuffer &buffer)
	{
		return true;
	}


	virtual void handledisConnect(SelectorKey &key)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}


	virtual void handleError(SelectorKey &key)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

};

///////////////////////






////////////////////////////////

#define  MUTEX_GUARD() \
	MutexGuard guard(_cs)

SelectorEx::SelectorEx()
{
	_handler = NULL;
	_shutdown = 0;
}

SelectorEx::~SelectorEx()
{
	if (_handler != NULL)
	{
		delete _handler;
	}
}

void SelectorEx::setHandler(SelectorHandler *handler)
{
	if (handler == NULL)
	{
		_handler = new DefaultSelectorHandler;
	}else
	{
		_handler = handler;
	}
}

void SelectorEx::run()
{
	int flag = 0;
	bool result = false;
	timeval tv = {0, 3000};
	FD_SET readFields;
	FD_SET writeFields;
	FD_SET exceptionFields;
	FD_ZERO(&readFields);
	FD_ZERO(&writeFields);
	FD_ZERO(&exceptionFields);
	SelectorKeyIter iter;
	vector<SelectorKey> sessions;
	InterlockedIncrement(&_shutdown);

	// 插入一个空的socket 防止无socket导致select一直返回失败
	SelectorKey placeHolderKey(SSocketExPtr(new SocketEx(true)), SelectorKey::SK_Read);
	registerSession(placeHolderKey);
	while (_shutdown > 0)
	{
		{

			FD_ZERO(&readFields);
			FD_ZERO(&writeFields);
			FD_ZERO(&exceptionFields);

	

			MUTEX_GUARD();
			
			

			
			iter = _sessions.begin();
			sessions.clear();
			while (iter != _sessions.end())
			{
				FD_SET(iter->second.getSocket()->getSocketHandle(), &readFields);
				FD_SET(iter->second.getSocket()->getSocketHandle(), &exceptionFields);
				sessions.push_back(iter->second);
				
				iter++;
			}

			iter = _connectingSessions.begin();
			
			while (iter != _connectingSessions.end())
			{
				FD_SET(iter->second.getSocket()->getSocketHandle(), &writeFields);
				//sessions.push_back(iter->second);
				
				iter++;
			}
		}

		flag = select(0, &readFields, &writeFields, &exceptionFields, &tv);


		

		if (flag == 0)
		{
			continue;
		}else if (flag < 0)
		{
			unsigned long errorCode = WSAGetLastError();
			break;
		}

		if (_shutdown == 0)
		{
			break;
		}

		for (int i = 0; i < sessions.size(); i++)
		{
			// 当连接失败或者其他情况时 移除该socket
			if (FD_ISSET(sessions[i].getSocket()->getSocketHandle(), &exceptionFields))
			{
				_handler->handleError(sessions[i]);
				unregisterSession(sessions[i].getSocket()->getRemoteAddress());
				continue;
			}else if (FD_ISSET(sessions[i].getSocket()->getSocketHandle(), &readFields))
			{
				// 当注册是读写的时候 而且该socket可读可写 意味着连接成功
				
				if ((sessions[i].isReadable() || sessions[i].isWritable())
					&& FD_ISSET(sessions[i].getSocket()->getSocketHandle(), &writeFields))
				{
					printDebugInfo(strerror(errno));
					sessions[i].setConnecting(false);
					removeConnectingSession(sessions[i]);
					continue;
				}

				if (sessions[i].isAcceptable())
				{
					SelectorKey key;
					result = _handler->handleAccept(sessions[i], key);

					if (result == true)
					{
						registerSession(key);
					}
				}else if (sessions[i].isReadable())
				{
					shared_ptr<vector<char> > buffer;

					{
						MUTEX_GUARD();
						BufferIter bufIter = _sessionBuffers.find(
								NetHelper::generateNetKey(sessions[i].getSocket()->getRemoteAddress()));

						if (bufIter != _sessionBuffers.end())
						{
							buffer = bufIter->second;
						}
					}

					
					result = _handler->handleRead(sessions[i], buffer);

					if (result == false)
					{
						unregisterSession(sessions[i].getSocket()->getRemoteAddress());
					}
				}

			}
		}
		
	}
	

}

void SelectorEx::stop()
{
	InterlockedDecrement(&_shutdown);
}

void SelectorEx::registerSession(const SocketEx &ss, SelectorKey::SelectOperationType opType, bool isConnecting /* = false */)
{
	
	SelectorKey key;
	key.setSocket(shared_ptr<SocketEx>(new SocketEx(ss)));
	key.setOperation(opType);
	key.setConnecting(isConnecting);
	registerSession(key);
}

void SelectorEx::registerSession(const SelectorKey &key)
{
	MUTEX_GUARD();


	_sessions.insert(make_pair(NetHelper::generateNetKey(key.getSocket()->getRemoteAddress()), key));
	_sessionBuffers.insert(make_pair(NetHelper::generateNetKey(key.getSocket()->getRemoteAddress()), 
							shared_ptr<vector<char> >(new vector<char>())));

	if (key.isConnecting())
	{
		_connectingSessions.insert(make_pair(NetHelper::generateNetKey(key.getSocket()->getRemoteAddress()), key));
	}
}

void SelectorEx::unregisterSession(const InetAddress &inet)
{
	MUTEX_GUARD();

	map<string, SelectorKey>::iterator iter = _sessions.find(NetHelper::generateNetKey(inet));
	if (iter == _sessions.end())
	{
		return;
	}

	//iter->second.getSocket()->close();
	iter->second.getSocket()->close();
	_connectingSessions.erase(NetHelper::generateNetKey(inet));
	_sessions.erase(iter);

	_sessionBuffers.erase(NetHelper::generateNetKey(inet));

}

void SelectorEx::handleData(SelectorKey & sk)
{

}

void SelectorEx::clearSessions()
{
	MUTEX_GUARD();
	SelectorKeyIter iter = _sessions.begin();

	while (iter != _sessions.end())
	{
		iter->second.getSocket()->close();
		iter++;
	}

	
	

	_connectingSessions.clear();
	_sessions.clear();
	_sessionBuffers.clear();
}

void SelectorEx::clearDisconnectionSession()
{
	MUTEX_GUARD();
	for (int i = 0; i < _disconnectSessions.size(); i++)
	{
		string key = NetHelper::generateNetKey(
			_disconnectSessions[i].getSocket()->getRemoteAddress());
		//SelectorKeyIter iter = _sessions.find(key);

		//if (iter != _sessions.end())
		//{
		//	//iter->second.getSocket()->close();
		//	_sessions.erase(iter);
		//	_sessionBuffers.erase(key);
		//}
		_sessions.erase(key);
		_connectingSessions.erase(key);
		_sessionBuffers.erase(key);
	}
}

void SelectorEx::removeConnectingSession(const SelectorKey & sk)
{
	MUTEX_GUARD();
	_connectingSessions.erase(NetHelper::generateNetKey(sk.getSocket()->getRemoteAddress()));
}
