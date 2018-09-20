#ifndef __RAWBUFFER__
#define __RAWBUFFER__
#include "Common.h"

class RawBuffer
{
public:
	RawBuffer(void);

	RawBuffer(ubyte *rawData, uint len);

	RawBuffer(RawBuffer &&rb);

	RawBuffer(const RawBuffer &rb);

	RawBuffer& operator=(RawBuffer rb);	

	~RawBuffer(void);

	void	setRawData(ubyte *rawData, uint len);

	ubyte*	getData() const {return data;}

	ubyte	getDataByIndex(int index) const;

	uint	getLength() const {return length;}

	void	append(ubyte *rawData, uint len);

	void	append(const RawBuffer &rb);

	void	clean();
protected:
	void	clear();
	void	defaultAlloc();
private:
	ubyte *data;
	uint length;
};




#endif


