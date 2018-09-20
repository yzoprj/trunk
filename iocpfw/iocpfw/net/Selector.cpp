
#include <WinSock2.h>
#include <Windows.h>
#include "Selector.h"
#include "SessionManager.h"
#include "../common/Common.h"


Selector::Selector(void)
{
	isRunning = false;
}


Selector::~Selector(void)
{
	//_sessions.clear();
}


void Selector::registerSession(AsyncSocket sock)
{
	//_sessions.insert(make_pair(sock.getIPAddres(), sock.getHandle()));
}


void Selector::run()
{
	int result = 0;
	timeval tv = {3, 0};
	FD_SET readFds;
	FD_ZERO(&readFds);

	SessionManager *sm = SessionManager::getInstance();

	CommunicatioInformation tcpCi = _acceptor.getSocket().getCommunicationInformation();
	
	CommunicatioInformation udpCi = _udpAcceptor.getCommunicationInformation();
	
	_connectSessons.insert(make_pair(tcpCi.generateKeyForSocket(), _acceptor.getSocket()));
	
	_connectSessons.insert(make_pair(udpCi.generateKeyForSocket(), _udpAcceptor));

	map<string, SocketSession>::iterator iter  = _connectSessons.begin();
	
	isRunning = true;
	
	while (isRunning)
	{
		FD_ZERO(&readFds);

		
		
		iter  = _connectSessons.begin();
			
		while (iter != _connectSessons.end())
		{
			FD_SET(iter->second.getSession().getHandle(), &readFds);
			iter++;
		}

		result = select(0, &readFds, NULL, NULL, &tv);
		if (result == 0)
		{
			continue;
		}

		if (isRunning == false)
		{
			break;
		}


		if (FD_ISSET(_acceptor.getSocket().getHandle(), &readFds))
		{
			printDebugInfo("New Connection By TCP Service Accept");

			AsyncSocket session = _acceptor.acceptSession();
			
			CommunicatioInformation sCi = session.getCommunicationInformation();
			
			//iter = _connectSessons.find(sCi.generateKeyForSocket());
			//if (iter != _connectSessons.end())
			//{
			//	iter->second.getSession().close();
			//	printDebugInfo("ÖØ¸´Á¬½Ó..........");
			//	_connectSessons.erase(iter);
			//}

			_connectSessons.insert(make_pair(sCi.generateKeyForSocket(), session));
			
			printDebugInfo("New Connection By TCP Server Accept");
			
		
		
		}else if (FD_ISSET(_udpAcceptor.getHandle(), &readFds))
		{
			printDebugInfo("New Message By UDP Server Accept");
			
			SocketSession ss(_udpAcceptor);
			
			handleData(ss);
		}
		else
		{
			iter = _connectSessons.begin();
			
			printDebugInfo("Client send message to the server in tcp mode");
			
			while (iter != _connectSessons.end())
			{
				SocketSession ss = iter->second;

				if (FD_ISSET(ss.session.getHandle(), &readFds))
				{
					handleData(ss);

				}
				iter++;
			}

			clearDisconnectionSession();

		}

		
	}

	clearSessions();
}


bool Selector::init(int serverPort)
{



	bool result = false;
	result = _acceptor.init(serverPort);
	
	if (result == false)
	{
		printDebugInfo("Acceptor initialize failed, please check!");
		
		return false;
	}

	result = _udpAcceptor.init(SM_UDP);
	if (result == false)
	{
		printDebugInfo("udp Acceptor initialize failed , please check!");
		
		_acceptor.getSocket().close();
		return false;
	}
	

	result = _udpAcceptor.bindPort(serverPort);

	if (result == false)
	{
		printDebugInfo("udp Acceptor binding port  failed , please check!");
		
		_udpAcceptor.close();
		
		_acceptor.getSocket().close();
	}

	return true;
	
}

string Selector::generateKey(int ip, int port, int sockType)
{
	char key[128] = {0};
	
	sprintf(key, "%08X%04X%02X", ip, port, sockType);

	return key;
}


void Selector::stop()
{
	isRunning = false;
}


void Selector::clearSessions()
{
	map<string, SocketSession>::iterator reliter = _connectSessons.begin();
	
	while (reliter != _connectSessons.end())
	{
		if (reliter->second.getSession().getHandle() == _acceptor.getSocket().getHandle()
			|| reliter->second.getSession().getHandle() == _udpAcceptor.getHandle())
		{
			reliter++;
			continue;
		}else
		{
			reliter->second.getSession().close();
		}
		reliter++;
	}

	_acceptor.getSocket().close();
	
	_udpAcceptor.close();
	
	_connectSessons.clear();

	SessionManager::getInstance()->clear();
}


void Selector::clearDisconnectionSession()
{
	if (_disconnctionSessions.size() > 0)
	{
		for (int i = 0; i < _disconnctionSessions.size(); i++)
		{
			_connectSessons.erase(_disconnctionSessions[i].getCommunicatioInformation().generateKeyForSocket());
			
			SessionManager::getInstance()->removeSession(_disconnctionSessions[i].getCommunicatioInformation().generateKeyForCommunication().c_str());

			if (_disconnctionSessions[i].getCommunicatioInformation().sockMode == SM_TCP)
			{
				_disconnctionSessions[i].getSession().close();
			}
		}
		
		_disconnctionSessions.clear();
	}

	
}