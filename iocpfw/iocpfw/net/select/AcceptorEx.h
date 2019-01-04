#ifndef ACCEPTOREX_H
#define ACCEPTOREX_H
#include "SocketEx.h"



class AcceptorEx : public SocketEx
{
public:
	AcceptorEx();
	~AcceptorEx();
	bool listen(const InetAddress &inet, int timeout = -1);
	SocketEx accept();
private:
	
	AcceptorEx(const AcceptorEx &ex);
	AcceptorEx &operator=(const AcceptorEx &ex);
};




#endif // ACCEPTOREX_H
