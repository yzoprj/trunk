#include "StringBuffer.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h>



StringBuffer::StringBuffer()
{
	data = NULL;
	size = 0;
	defaultAlloc();
}

StringBuffer::StringBuffer(const StringBuffer &rhs)
{
	data = NULL;
	size = 0;
	if (rhs.size != 0)
	{
		size = rhs.size;
		data = new char[size + 1];
		memcpy(data, rhs.data, rhs.size);
		data[size] = '\0';
	}else
	{
		defaultAlloc();
	}

}

#if _MSC_VER >= 1600
StringBuffer::StringBuffer(StringBuffer&& sb)
	:data(sb.data), size(sb.size)
{
	sb.data = NULL;
	sb.size = 0;
}

#endif


StringBuffer::StringBuffer(const char *buf)
{
	data = NULL;

	if(buf == NULL)
	{
		size = 0;
	}else
	{
		size = strlen(buf);
	}


	if (size != 0)
	{
		data = new char[size + 1];
		memcpy(data, buf, size);
	}else
	{
		defaultAlloc();
	}
}



StringBuffer& StringBuffer::operator=(const StringBuffer &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}
	if (rhs.size != 0)
	{
		if (size < rhs.size)
		{
			clear();
			size = rhs.size;
			data = new char[size + 1];

		}else
		{
			memset(data, 0, size);
			size = rhs.size;
		}
		memcpy(data, rhs.data, size);
		data[size] = '\0';
	}else
	{
		defaultAlloc();
	}

	return *this;
}

StringBuffer& StringBuffer::operator=(const char *rhs)
{
	int len  = 0;


	if(rhs == NULL)
	{
		len = 0;
	}else
	{
		len = strlen(rhs);
	}

	if (this->data == rhs)
	{
		return *this;
	}

	if (len != 0)
	{
		if (size < len)
		{
			clear();
			size = len;
			data = new char[size + 1];

		}else
		{
			memset(data, 0, size);
			size = len;
		}
		memcpy(data, rhs, size);
		data[size] = '\0';
	}else
	{
		defaultAlloc();
	}

	return *this;
}


bool StringBuffer::operator ==(const StringBuffer &rhs)
{
	if(this->size != rhs.size)
		return false;
	if (memcmp(this->data, rhs.data, size) != 0)
	{
		return false;
	}
	return true;
}

bool StringBuffer::operator ==(const char *rhs)
{
	int len  = 0;


	if(rhs == NULL)
	{
		len = -1;
	}else
	{
		len = strlen(rhs);
	}

	if (size != len)
	{
		return false;
	}

	if (memcmp(this->data, rhs, size) != 0)
	{
		return false;
	}
	return true;
}

StringBuffer::~StringBuffer()
{
	clear();
};

void StringBuffer::append(const char *buf)
{
	int appendLen  = 0;


	if(buf == NULL)
	{
		return;
	}else
	{
		appendLen = strlen(buf);
	}

	char *newData = new char[size + appendLen + 1];

	memcpy(newData, data, size);
	memcpy(newData + size, buf, appendLen);
	newData[size + appendLen ] = '\0';

	delete data;
	data = newData;
	size = size + appendLen;

}

void StringBuffer::append(const StringBuffer &buf)
{
	char *newData = new char[size + buf.size + 1];
	memcpy(newData, data, size);
	memcpy(newData + size, buf.data, buf.size);
	newData[size + buf.size ] = '\0';

	delete data;
	data = newData;
	size = size + buf.size;

}


void StringBuffer::defaultAlloc()
{
	if (data != NULL)
	{
		clear();
	}
	data = new char[size + 1];
	data[size] = '\0';
}

void StringBuffer::clear()
{
	size = 0;
	if (data != NULL)
	{
		delete data;
		data = NULL;
	}
}