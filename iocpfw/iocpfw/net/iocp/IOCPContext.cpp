#include "IOCPContext.h"


IOContext::IOContext(int bufferSize /* = MAX_BUFFER_LENGTH */, bool isClear /* = true */)
	:opBytes(NullOperation),
	sockId(SOCKET_ERROR),
	owner(NULL),
	capacity(0),
	isLast(false),
	index(-1),
	isSelfDestroy(false),
	buffer(NULL)
{
	if (isClear == false)
	{
		capacity = 0;
	}else
	{
		capacity = bufferSize;
	}
	reallocBuffer();
	clearBuffer();
}

IOContext::~IOContext()
{
	if (capacity > 0)
	{
		delete buffer;
		buffer = NULL;
	}

}


bool IOContext::isFinished()
{
	if (opBytes == wsaBuf.len)
	{
		return true;
	}
	return false;
}


void IOContext::reallocBuffer(int size)
{
	if (capacity > 0)
	{
		if (size <= 0)
		{
			capacity = MAX_BUFFER_LENGTH;
		}else
		{
			capacity = size;
		}

		delete buffer;

		buffer = new char[capacity];
		wsaBuf.buf = buffer;
		wsaBuf.len = capacity;
	}
}

int IOContext::setBuffer(const char *inBuffer, int bufferLength)
{
	//if (capacity > 0)
	//{
	//	delete this->buffer;
	//}

	//if (isClear == false)
	//{
	//	delete buffer;
	//	capacity = 0;
	//	opBytes = 0;
	//	this->buffer = buffer;
	//	wsaBuf.buf = buffer;
	//	wsaBuf.len = bufferLength;
	//}else
	//{
	//	capacity = bufferLength;
	//	this->buffer = buffer;
	//}
	int sizeCount = 0;
	if (capacity == 0)
	{
		wsaBuf.buf = (char *)inBuffer;
		wsaBuf.len = bufferLength;
		sizeCount = bufferLength;
		
	}else
	{
		if (bufferLength > capacity)
		{
			sizeCount = capacity;
		}else
		{
			sizeCount = bufferLength;
		}

		memcpy(buffer, inBuffer, sizeCount);
		wsaBuf.len = sizeCount;
	}

	return sizeCount;
}

void IOContext::clearBuffer()
{

	ZeroMemory(&overLapped, sizeof(overLapped));

	if (capacity > 0)
	{
		ZeroMemory(buffer, capacity);
	}
	


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
	ioContext->clearBuffer();
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

	printf("Thread[%04X]=[%02d:%02d:%02d %03d][%-40s]:%s\n", GetCurrentThreadId(), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
								function, str);

}