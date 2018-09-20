#ifndef _BYTETOOLS_H_
#define _BYTETOOLS_H_

#include "Common.h"



class ByteTools
{
public:
	static ushort	byteToHex(ubyte val);
	static ubyte	hexToByte(ubyte val);
	static ubyte	twoHexToByte(ubyte highHex, ubyte lowHex);
	static uint		rawArrayToHexString(ubyte *dst, uint maxsize, const ubyte *src, uint len);
	static uint		hexStringToRawString(ubyte *dst, uint maxsize, const ubyte *src, uint len);

	static short	twoByteToShort(ubyte val1, ubyte val2);
protected:
	static ubyte hexCodeArray[];
};





#endif
