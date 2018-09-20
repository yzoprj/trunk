
#ifndef _SESSION_MANAGER_H_
#define _SESSION_MANAGER_H_

#include <map>
#include <string>
#include <iterator>
#include "CommunicationSession.h"
#include "../thread/SyncObjects.h"

using std::string;
using std::map;
using std::iterator;





		class SessionManager
		{
		public:
			
			SessionManager(void);
			~SessionManager(void);
			static SessionManager *getInstance();
			static CriticalSection sockSendMuetx;
			CommunicationSession *getSession(const char *key);
			
			// Ωˆ  ”√”⁄Socket
			CommunicationSession *getNext();
			void resetIterator();
			void addSession(CommunicationSession *session);
			void removeSession(const char *key);
			bool isSocketExisted(const char *key);
			bool isSocketActived(const char *key);
			int  getCount() { return _sessions.size();}
			void clear();
			static string generateKey(const char *name, int connType, int sockMode);
		private:
			
		private:
			static SessionManager _manager;
			map<string, CommunicationSession *> _sessions;
			map<string, CommunicationSession *>::iterator _sessionIter;
			CriticalSection _cs;
		};


#endif


