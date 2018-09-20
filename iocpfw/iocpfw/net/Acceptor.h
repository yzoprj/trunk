#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "ASyncSocket.h"

	class Acceptor
	{
	public:
		Acceptor(void);
		~Acceptor(void);
		bool init(int port);
		AsyncSocket acceptSession();
		AsyncSocket &getSocket() {return serverSock;}
	protected:
		AsyncSocket serverSock;
	};


#endif

