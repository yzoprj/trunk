#include "IOCPContext.h"


IOContext::IOContext()
	:opBytes(NullOperation),
	sockId(SOCKET_ERROR),
	owner(NULL),
	totalBytes(0),
	isLast(false),
	index(-1),
	isSelfDestroy(false),
	isBufferClear(true),
	buffer(NULL)
{

	reallocBuffer();
	clearBuffer();
}

IOContext::~IOContext()
{
	if (isBufferClear == true)
	{
		delete buffer;
		buffer = NULL;
		wsaBuf.buf = buffer;
	}
}


bool IOContext::isFinished()
{
	if (opBytes == totalBytes)
	{
		return true;
	}
	return false;
}


void IOContext::reallocBuffer(int size)
{
	if (isBufferClear == true)
	{
		if (buffer != NULL)
		{
			delete buffer;


		}
		totalBytes = size;
		buffer = new char[size];
		wsaBuf.buf = buffer;
		wsaBuf.len = size;
	}
}

void IOContext::clearBuffer()
{

	ZeroMemory(&overLapped, sizeof(overLapped));
	ZeroMemory(wsaBuf.buf, wsaBuf.len);


}


SocketContext::SocketContext()
{
	index = -1;
	sockId = INVALID_SOCKET;
	opSet = 0;
	isAcceptable = false;
	recvBuff = NULL;
	ioContext = new IOContext;
	ioContext->owner = this;
}

void SocketContext::clear()
{

	memset(&sockAddr, 0 , sizeof(sockAddr));
	sockId = INVALID_SOCKET;
	opSet = 0;
}

void SocketContext::close()
{
	if (sockId != INVALID_SOCKET)
	{
		shutdown(sockId, SD_BOTH);
		closesocket(sockId);
		sockId = INVALID_SOCKET;
	}
}

SocketContext::~SocketContext()
{
	close();
	delete ioContext;
	ioContext = NULL;
}



void writeLog(const char *str, const char *filesource /* = NULL */, const char *functionName /* = NULL */)
{
	char *file = NULL;
	char *function = NULL;

	if (filesource == NULL)
	{
		file = "";
	}else
	{
		file = (char *)filesource;
	}
	
	if (functionName == NULL)
	{
		function = "";
	}else
	{
		function = (char *)functionName;
	}

	SYSTEMTIME st;
	GetSystemTime(&st);

	printf("[%02d:%02d:%02d][%s][%s]:%s\n", st.wHour, st.wMinute, st.wSecond,
								file, function, str);

}