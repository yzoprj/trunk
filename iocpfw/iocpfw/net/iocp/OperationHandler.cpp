#include "OperationHandler.h"
#include "SocketContextManager.h"
#include "IOBufferManager.h"
#include "IOTaskManager.h"
void DefaultOperationHandler::handleRecv(SSocketContextPtr &sockContext, IOBuffer *ioBuffer)
{
	char buffer[8096] = {0};
	sprintf(buffer, "Client[%d][%s]send bytes[%u][%s]", sockContext->index, SocketContextManager::getClientName(sockContext).c_str(), ioBuffer->ioContext.overLapped.InternalHigh
		,ioBuffer->cache.data());

	WRITELOG(buffer);
	ioBuffer->cache.clear();
}

void DefaultOperationHandler::handleSend(SSocketContextPtr &sockContext, IOContext *ioContext)
{
	char buffer[1024] = {0};
	IOTask *task = (IOTask *)ioContext->owner;
	sprintf(buffer, "Index[%d]send bytes[%u]==accumulate[%d] finishTimes[%d]", ioContext->index, ioContext->overLapped.InternalHigh
										, task->opBytes + ioContext->overLapped.InternalHigh, task->finishedTimes + 1);

	WRITELOG(buffer);
}

void DefaultOperationHandler::handleAccept(SSocketContextPtr &sockContext)
{
	char buffer[1024] = {0};
	sprintf(buffer,"new connect[%d]:[%s]", sockContext->index, SocketContextManager::getClientName(sockContext).c_str());

	WRITELOG(buffer);
}

void DefaultOperationHandler::handleConnection(SSocketContextPtr &sockContext, IOContext *ioConext)
{

}

void DefaultOperationHandler::handleDisconnect(SSocketContextPtr &sockContext)
{

}
