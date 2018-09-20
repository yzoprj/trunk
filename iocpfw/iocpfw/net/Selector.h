#ifndef _SELECTOR_H_
#define _SELECTOR_H_
#include "Acceptor.h"
#include "CommunicationSession.h"
//#include <WinSock2.h>
#include <map>
#include <string>
#include <utility>
#include <vector>
using std::string;
using std::map;
using std::vector;
using std::make_pair;


	class Selector
	{
	public:
		Selector(void);
		~Selector(void);
		
		
		void run();
		void stop();

		virtual bool init(int serverPort);
		virtual void handleData(SocketSession & ss) = 0;
		
	protected:
		bool isSessionActived(char *ipAddr);
		string generateKey(int ip, int port, int sockMode);

		void clearSessions();
		void clearDisconnectionSession();
		void registerSession(AsyncSocket sock);

		map<string, SocketSession> _connectSessons;
		vector<SocketSession> _disconnctionSessions;
	private:
		Acceptor _acceptor;
		AsyncSocket _udpAcceptor;
		
		bool isRunning;
		//FD_SET _sessionset;
	};



#endif
















