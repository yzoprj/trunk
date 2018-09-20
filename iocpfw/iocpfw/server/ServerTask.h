#pragma once

class AsyncSocket;
class SocketChannel;
class RawBuffer;

enum HandleResultCode
{
	HRC_Success = 0,
	HRC_Failed = -1,
	HRC_NoConnected = -2,
	HRC_DisConnected = -3,
	HRC_IOPending = -4,
	HRC_UnknowError = -5
};

class ServerTask
{
public:
	ServerTask(void);
	~ServerTask(void);
	virtual HandleResultCode handleAccept(AsyncSocket &sock) = 0;
	virtual int handleRead(SocketChannel &channel, RawBuffer &data) = 0;
	virtual int handleWrite(SocketChannel &channel,  const RawBuffer &data) = 0;
	virtual HandleResultCode handleError(SocketChannel &channel) = 0;
	virtual HandleResultCode handleOther(SocketChannel &channel) = 0;
};

