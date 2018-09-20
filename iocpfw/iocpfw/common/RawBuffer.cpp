#include "RawBuffer.h"
#include <memory.h>




RawBuffer::RawBuffer(void)
{
	length = 0;
	data = NULL;
	defaultAlloc();
	
}

RawBuffer::RawBuffer(const RawBuffer &rb)
{
	length = rb.length;
	data = new ubyte[length];
	memcpy(data, rb.data, length);
}


RawBuffer::RawBuffer(RawBuffer &&rb)
	:data(rb.data), length(rb.length)
{
	rb.data = NULL;
	rb.length = 0;
}


RawBuffer::RawBuffer(ubyte *rawData, uint len)
{
	if (rawData == NULL  || len <= 0 )
	{
		defaultAlloc();
	}else
	{
		length = len;
		data = new ubyte[len];
		memcpy(data, rawData, len);
	}
}

RawBuffer& RawBuffer::operator =(RawBuffer rb)
{

	if (this == &rb)
	{
		return *this;
	}


	if (length != 0)
	{
		clear();
	}

	length = rb.length;
	data = new ubyte[length];
	memcpy(data, rb.data, length);
	return *this;
}

RawBuffer::~RawBuffer(void)
{
	clear();
}


void RawBuffer::clear()
{
	length = 0;
	if(data != NULL)
	{
		delete data;
		data = NULL;
	}
}

void RawBuffer::defaultAlloc()
{
	clear();
	length = 1;
	data = new ubyte[length];
	data[0] = 0x00;
}


void RawBuffer::clean()
{
	defaultAlloc();
}

void RawBuffer::append(ubyte *rawData, uint len)
{
	ubyte *newData = NULL;
	if (rawData == NULL  || len <= 0 )
	{
		return;
	}else
	{
		newData = new ubyte[length + len];
		memcpy(newData, data, length);
		memcpy(newData + length, rawData, len);
		length = length + len;
		if (data != NULL)
		{
			delete data;
		}
		data = newData;
	}
}

void RawBuffer::append(const RawBuffer &rb)
{
	append(rb.data, rb.length);
}


void RawBuffer::setRawData(ubyte *rawData, uint len)
{
	clear();

	if (len > 0)
	{
		data = new ubyte[len];
		memcpy(data, rawData, len);
		length = len;
	}else
	{
		defaultAlloc();
	}

	
}


ubyte RawBuffer::getDataByIndex(int index) const
{
	if (index < 0 && index >= length)
	{
		return 0;
	}

	return data[index];
}