#ifndef SELECTORHANDLER_H
#define SELECTORHANDLER_H

#include "NetCommon.h"

class SelectorHandler
{
public:
	SelectorHandler();
	~SelectorHandler();

	virtual bool handleAccept(SelectorKey &key, SelectorKey &newKey) = 0;
	virtual bool handleRead(SelectorKey &key, shared_ptr<vector<char> > &buffer) = 0;
	virtual bool handleWrite(SelectorKey &key, const RawBuffer &buffer) = 0;
	virtual void handledisConnect(SelectorKey &key) = 0;
	virtual void handleError(SelectorKey &key) = 0;
private:

};




#endif // SELECTORHANDLER_H
