#include "CommunicationInformation.h"
#include <WinSock2.h>
#include <windows.h>



void CommunicatioInformation::setHostAddress(const char *hostAddress)
{
	if (connType == CT_SerialPort)
	{
		this->hostAddress = atoi(hostAddress);
	}else
	{
		this->hostAddress = inet_addr(hostAddress);
	}
}

void CommunicatioInformation::setPort(int port)
{
	this->port = htons(port);
}