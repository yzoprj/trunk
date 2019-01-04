#include "NetHelper.h"
#include <string.h>
#include <WinSock2.h>
#include <Windows.h>
#include "InetAddress.h"
string NetHelper::generateNetKey(const InetAddress &inet)
{
	char buffer[256] = {0};
	sprintf(buffer, "%08X%02X", inet.getIp(), inet.getPort());

	return buffer;
}

NetHelper::NetHelper()
{

}

NetHelper::~NetHelper()
{

}
