#include "HexBuffer.h"
#include "ByteTools.h"



HexBuffer::HexBuffer(const RawBuffer & rb)
{
	convert(rb.getData(), rb.getLength());
}

HexBuffer::HexBuffer(const HexBuffer &hb)
{

	sb = hb.sb;
}

HexBuffer::~HexBuffer(void)
{
}


void HexBuffer::setHexString(char *buffer)
{
	sb = buffer;
}

RawBuffer HexBuffer::getRawBuffer()
{
	return recovert((const ubyte *)sb.getData(), sb.getLength());
}

StringBuffer HexBuffer::getBuffer()
{
	return sb;
}





void HexBuffer::convert(const ubyte *rawData, uint length)
{
	uint bufLen = length * 2 + 1;
	ubyte *buf = NULL;

	buf = new ubyte[bufLen];

	ByteTools::rawArrayToHexString(buf, bufLen, rawData, length);

	sb = (const char *)buf;
	delete buf;

}

RawBuffer HexBuffer::recovert(const ubyte *src, uint length)
{
	uint bufLen = length / 2;
	ubyte *buf = NULL;

	buf = new ubyte[bufLen];
	bufLen = ByteTools::hexStringToRawString(buf, bufLen, src, length);
	RawBuffer rb(buf, bufLen);
	delete buf;
	return rb;
}


void HexBuffer::setRawBuffer(const unsigned char *buffer, int bufLen)
{
	if (buffer == NULL || bufLen <= 0 )
	{
		return;
	}

	convert(buffer, bufLen);
}