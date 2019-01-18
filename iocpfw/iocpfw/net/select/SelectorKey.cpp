#include "SelectorKey.h"
#include "SocketEx.h"
using std::make_shared;
SelectorKey::SelectorKey()
{
	_connecting = false;
}

SelectorKey::SelectorKey(const SSocketExPtr &sock, 
			SelectOperationType opType)
{
	setSocket(sock);
	setOperation(opType);
	_connecting = false;
}

SelectorKey::SelectorKey(const SelectorKey &key)
{
	setSocket(key.getSocket());
	setOperation(key.getOperation());
	_connecting = key._connecting;
}

SelectorKey::~SelectorKey()
{

}

void SelectorKey::setConnecting(bool connecting)
{
	_connecting = connecting;
}

void SelectorKey::setSocket(const SSocketExPtr &sock)
{
	_sock = sock;
}

void SelectorKey::setSocket(const SocketEx &sock)
{
	_sock = make_shared<SocketEx>(sock);
}

void SelectorKey::setOperation(const SelectOperationType opType)
{
	_opType = opType;
}

SSocketExPtr SelectorKey::getSocket() const
{
	return _sock;
}

SelectorKey::SelectOperationType SelectorKey::getOperation() const
{
	return _opType;
}

bool SelectorKey::isConnecting() const
{
	return _connecting == true;
}

bool SelectorKey::isAcceptable() const
{
	return _opType == SK_Accept ? true : false;
}

bool SelectorKey::isReadable() const
{
	return _opType == SK_Read ? true : false;
}

bool SelectorKey::isWritable() const
{
	return _opType == SK_Write ? true : false;
}

bool SelectorKey::isDisconnect() const
{
	return _opType == SK_Disconnect ? true : false;
}

SelectorKey & SelectorKey::operator=(const SelectorKey &key)
{
	setSocket(key.getSocket());
	setOperation(key.getOperation());

	return *this;
}
