#ifndef _HEXBUFFER_H_
#define _HEXBUFFER_H_

#include "RawBuffer.h"
#include "StringBuffer.h"



class HexBuffer
{
public:
	HexBuffer(){}
	HexBuffer(const RawBuffer &rb);
	HexBuffer(const HexBuffer &hb);
	//HexBuffer(HexBuffer &&hb);

	HexBuffer& operator =(const HexBuffer &hb);
	HexBuffer& operator =(const RawBuffer &hb);

	~HexBuffer(void);


	void			setHexString(char *buffer);
	void			setRawBuffer(const unsigned char *buffer, int bufLen);
	StringBuffer	getBuffer();
	RawBuffer		getRawBuffer();

protected:
	void convert(const ubyte *rawData, uint length);
	RawBuffer recovert(const ubyte *src, uint length);

private:
	StringBuffer sb;
};






#endif
