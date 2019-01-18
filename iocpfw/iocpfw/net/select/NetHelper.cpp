#include "NetHelper.h"
#include <string.h>
#include <WinSock2.h>
#include <Windows.h>
#include "InetAddress.h"
string NetHelper::generateNetKey(const InetAddress &inet)
{
	char buffer[256] = {0};
	sprintf(buffer, "%08X%02X", inet.getIP(), inet.getPort());

	return buffer;
}

string NetHelper::generateNetIP(unsigned long ip)
{
	SOCKADDR_IN addr;
	addr.sin_addr.S_un.S_addr = ip;

	char buffer[256] = {0};

	sprintf(buffer, "%d.%d.%d.%d", addr.sin_addr.S_un.S_un_b.s_b4,
								   addr.sin_addr.S_un.S_un_b.s_b3,
								   addr.sin_addr.S_un.S_un_b.s_b2,
								   addr.sin_addr.S_un.S_un_b.s_b1);

	return buffer;
}

NetHelper::NetHelper()
{

}

NetHelper::~NetHelper()
{

}
