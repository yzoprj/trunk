#ifndef SELECTORKEY_H
#define SELECTORKEY_H
#include <memory>
using std::shared_ptr;

class SocketEx;
typedef shared_ptr<SocketEx> SSocketExPtr;


class SelectorKey
{

public:
	enum SelectOperationType{
		SK_Accept,
		SK_Read,
		SK_Write,
		SK_Disconnect
	};

	
public:
	SelectorKey();
	
	SelectorKey(const SSocketExPtr &sock, SelectOperationType opType);
	
	SelectorKey(const SelectorKey &key);

	SelectorKey &operator= (const SelectorKey &key);

	~SelectorKey();

	void setConnecting(bool connecting);
	
	void setSocket(const SSocketExPtr &sock);
	
	void setSocket(const SocketEx &sock);

	void setOperation(const SelectOperationType opType);

	SSocketExPtr getSocket() const ;

	SelectOperationType getOperation() const ;


	bool isConnecting() const;

	bool isAcceptable() const;

	bool isReadable() const;

	bool isWritable() const;

	bool isDisconnect() const;
private:
	SelectOperationType _opType;
	SSocketExPtr _sock;
	bool _connecting;
};

#endif // SELECTORKEY_H
