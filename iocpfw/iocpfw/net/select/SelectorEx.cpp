#include "SelectorEx.h"
//#include <WinSock2.h>
//#include <Windows.h>
#include "../../common/Common.h"
#include "SelectorHandler.h"
#include "SocketEx.h"
#include "NetHelper.h"
#include "InetAddress.h"




typedef map<string, SelectorKey>::iterator SelectorKeyIter;

typedef map<string, shared_ptr<vector<char> > >::iterator BufferIter;


class DefaultSelectorHandler : public SelectorHandler
{

public:
	

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
	timeval tv = {3, 0};
	FD_SET readFields;
	FD_ZERO(&readFields);

	SelectorKeyIter iter;
	vector<SelectorKey> sessions;
	InterlockedIncrement(&_shutdown);

	while (_shutdown > 0)
	{
		{
			MUTEX_GUARD();
			iter = _sessions.begin();
			while (iter != _sessions.end())
			{
				FD_SET(iter->second.getSocket()->getSocketHandle(), &readFields);
				sessions.push_back(iter->second);
				iter++;
			}
		}

		flag = select(0, &readFields, NULL, NULL, &tv);

		if (flag == 0)
		{
			continue;
		}

		if (_shutdown == 0)
		{
			break;
		}

		for (int i = 0; i < sessions.size(); i++)
		{
			if (FD_ISSET(sessions[i].getSocket()->getSocketHandle(), &readFields))
			{
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

void SelectorEx::registerSession(const SocketEx &ss, SelectorKey::SelectOperationType opType)
{
	
	SelectorKey key;
	key.setSocket(shared_ptr<SocketEx>(new SocketEx(ss)));
	key.setOperation(opType);
	
}

void SelectorEx::registerSession(const SelectorKey &key)
{
	MUTEX_GUARD();
	_sessions.insert(make_pair(NetHelper::generateNetKey(key.getSocket()->getRemoteAddress()), key));
	_sessionBuffers.insert(make_pair(NetHelper::generateNetKey(key.getSocket()->getRemoteAddress()), 
							shared_ptr<vector<char> >(new vector<char>())));
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
		_sessionBuffers.erase(key);
	}
}
