#ifndef _SOCKET_CHANNEL_H_
#define _SOCKET_CHANNEL_H_
#include "ASyncSocket.h"
#include "../common/RawBuffer.h"



enum SocketErrorStatus
{
	SES_Success = 1,
	SES_Disconnect = 0,
	SES_ReadError = -1,
	SES_ReadNextTime = -2,
	SES_ReadAgain = -3
};

class SocketChannel
{
public:
	static const int MAX_READ_BUFFER_SIZE = 0x08FFF / 2; 

	SocketChannel();
	SocketChannel(const AsyncSocket &sock);
	~SocketChannel(void);

	void setSocket(const AsyncSocket &socket);

	int getErrorStatus() {return _errorStatus;}

	// for tcp
	int readN(RawBuffer &rb);
	int writeN(const RawBuffer &rb);
	int readByte(ubyte &val);
	int writeByte(ubyte val);

	// for udp

	int readN(RawBuffer &rb, CommunicatioInformation &ci);
	int writeN(const RawBuffer &rb, const CommunicatioInformation &ci);
	int readByte(ubyte &val, CommunicatioInformation &ci);
	int writeByte(ubyte val, CommunicatioInformation &ci);
protected:
	int handleResultErrorResult(int val);
private:
	AsyncSocket _sock;
	int _errorStatus;
};


#endif
