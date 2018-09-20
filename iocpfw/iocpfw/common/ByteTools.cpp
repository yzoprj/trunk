#include "ByteTools.h"
#include <ctype.h>



ubyte ByteTools::hexCodeArray[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};


ushort ByteTools::byteToHex(ubyte val)
{
	ushort result = 0;
	result = hexCodeArray[((val & 0xF0) >> 4)] << 8;
	result = result + hexCodeArray[(val &0x0F)];

	return result;
}


ubyte ByteTools::hexToByte(ubyte val)
{

	val = toupper(val);
	if (val >= 'A' && val <= 'F')
	{
		val = val - 0x30 - 7;
	}else
	{
		val = val - 0x30;
	}

	return val;
}

ubyte ByteTools::twoHexToByte(ubyte highHex, ubyte lowHex)
{
	ubyte result;
	
	result = ((hexToByte(highHex)) << 4) + hexToByte(lowHex);
	return result;
}


uint ByteTools::rawArrayToHexString(ubyte *dst, uint maxsize, const ubyte *src, uint len)
{
	uint fixedLen = 0;
	ushort tmpVal = 0;
	if (len * 2 + 1 > maxsize)
	{
		fixedLen = (maxsize - 1) / 2;
	}else
	{
		fixedLen = len;
	}

	for (uint i = 0; i < fixedLen; i++)
	{
		tmpVal = byteToHex(src[i]);
		dst[i * 2] = (ubyte)((tmpVal & 0xFF00) >> 8);
		dst[i * 2 + 1] = (ubyte)(tmpVal & 0x00FF);
	}

	dst[fixedLen * 2]  = '\0';
	return fixedLen * 2;

}

uint ByteTools::hexStringToRawString(ubyte *dst, uint maxsize, const ubyte *src, uint len)
{
	int fixedLen = 0;

	fixedLen = len / 2;
	if (len / 2 > maxsize)
	{
		fixedLen = maxsize;
	}
	
	for(int i = 0; i < fixedLen; i++)
	{
		dst[i] = twoHexToByte(src[i * 2], src[i * 2 + 1]);
	}

	return fixedLen;

}


short ByteTools::twoByteToShort(ubyte val1, ubyte val2)
{


	return ((((unsigned short)val1) << 8) && 0xFF00) + val2;
}