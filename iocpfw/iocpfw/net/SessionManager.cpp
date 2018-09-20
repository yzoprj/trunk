#include "SessionManager.h"

using std::make_pair;



SessionManager SessionManager::_manager;
CriticalSection SessionManager::sockSendMuetx;

SessionManager::SessionManager(void)
{
	_sessionIter = _sessions.end();
}


SessionManager::~SessionManager(void)
{
	clear();
}


void SessionManager::clear()
{
	MutexGuard mg(_cs);
	_sessionIter = _sessions.begin();
	while(_sessionIter != _sessions.end())
	{
		delete _sessionIter->second;
		_sessionIter++;
	}
	_sessions.clear();
}

void SessionManager::addSession(CommunicationSession *session)
{
	MutexGuard mg(_cs);
	CommunicatioInformation ci = session->getCommunicatioInformation();
	if (isSocketExisted(ci.generateKeyForCommunication().c_str()))
	{
		return;
	}
	_sessions.insert(make_pair(ci.generateKeyForCommunication(), session ));
	_sessionIter = _sessions.begin();
}

// 使用完要手动delete
CommunicationSession* SessionManager::getSession(const char *key)
{
	MutexGuard mg(_cs);
	CommunicationSession *session = NULL;
	map<string, CommunicationSession *>::iterator iter = _sessions.find(key);
	if (iter == _sessions.end())
	{
		session = NULL;
	}else if (iter->second->getCommunicatioInformation().getConnectType() == CT_Socket)
	{
		session = new SocketSession(*(SocketSession *)iter->second);
	}else
	{
		session = new SerialPortSession(*(SerialPortSession *)iter->second);
	}

	return session;
}

bool SessionManager::isSocketExisted(const char *key)
{
	return _sessions.find(key) == _sessions.end() ? false : true;
}

bool SessionManager::isSocketActived(const char *key)
{
	return _sessions.find(key) == _sessions.end() ? false : true;
}

SessionManager* SessionManager::getInstance()
{
	//static SessionManager sm;
	return &_manager;
}


CommunicationSession* SessionManager::getNext()
{
	if (_sessionIter != _sessions.end() && _sessions.size() > 0)
	{
		//if (_sessionIter->second->getType() == CNT_Socket)
		//{
		//	return (_sessionIter++)->second;
		//}

		do 
		{

			if (_sessionIter->second->getCommunicatioInformation().connType == CT_Socket)
			{
				return (_sessionIter++)->second;
			}else
			{
				_sessionIter++;
			}
		} while (_sessionIter != _sessions.end());


	}

	return NULL;
}

void SessionManager::resetIterator()
{
	_sessionIter = _sessions.begin();
}

void SessionManager::removeSession(const char *key)
{
	map<string, CommunicationSession *>::iterator iter = _sessions.find(key);
	
	if (iter != _sessions.end())
	{
		delete iter->second;
		_sessions.erase(iter);
	}
}

string SessionManager::generateKey(const char *name, int connType, int sockMode)
{
	char key[128];
	sprintf(key, "%02X%02X%s", connType, sockMode, name);
	return key;
}