#pragma once


#include "IOCPContext.h"

class OperationHandler
{
public:
	virtual void handleRecv(SSocketContextPtr &sockContext, IOBuffer *ioBuffer) = 0;

	virtual void handleSend(SSocketContextPtr &sockContext, IOContext *ioContext) = 0;

	virtual void handleAccept(SSocketContextPtr &sockContext) = 0;

	virtual void handleDisconnect(SSocketContextPtr &sockContext) = 0;
};

class DefaultOperationHandler : public OperationHandler
{

public:
	 void handleRecv(SSocketContextPtr &sockContext, IOBuffer *ioBuffer);


	void handleSend(SSocketContextPtr &sockContext, IOContext *ioContext);


	void handleAccept(SSocketContextPtr &sockContext);


	void handleDisconnect(SSocketContextPtr &sockContext);

};