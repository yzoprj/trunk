#include "OperationHandler.h"
#include "SocketContextManager.h"

void DefaultOperationHandler::handleRecv(SSocketContextPtr &sockContext, IOContext *ioContext)
{
	char buffer[8096] = {0};
	sprintf(buffer, "Index[%d]recv bytes[%u][%s]", ioContext->index, ioContext->overLapped.InternalHigh
		,sockContext->recvBuff->data());

	WRITELOG(buffer);
	sockContext->recvBuff->clear();
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
