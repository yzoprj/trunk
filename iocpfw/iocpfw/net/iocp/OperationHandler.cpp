#include "OperationHandler.h"
#include "SocketContextManager.h"
#include "IOBufferManager.h"

void DefaultOperationHandler::handleRecv(SSocketContextPtr &sockContext, IOBuffer *ioBuffer)
{
	char buffer[8096] = {0};
	sprintf(buffer, "Index[%d]recv bytes[%u][%s]", ioBuffer->ioContext.index, ioBuffer->ioContext.overLapped.InternalHigh
		,ioBuffer->cache.data());

	WRITELOG(buffer);
	ioBuffer->cache.clear();
}

void DefaultOperationHandler::handleSend(SSocketContextPtr &sockContext, IOContext *ioContext)
{
	char buffer[1024] = {0};
	sprintf(buffer, "Index[%d]send bytes[%u]", ioContext->index, ioContext->overLapped.InternalHigh);

	WRITELOG(buffer);
}

void DefaultOperationHandler::handleAccept(SSocketContextPtr &sockContext)
{
	char buffer[1024] = {0};
	sprintf(buffer,"new connect:[%s]", SocketContextManager::getClientName(sockContext).c_str());

	WRITELOG(buffer);
}

void DefaultOperationHandler::handleDisconnect(SSocketContextPtr &sockContext)
{

}
