#ifndef SOCKETCHANNELEX_H
#define SOCKETCHANNELEX_H

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;
class SocketEx;
class SocketChannelEx
{
public:
	SocketChannelEx();
	
	SocketChannelEx(shared_ptr<SocketEx> &sock);

	~SocketChannelEx();
	

	bool read(vector<char> &buffer);

	bool write(const vector<char> &buffer, int &outBytes);

	void setSocket(shared_ptr<SocketEx> &sock);
private:
	shared_ptr<SocketEx> _sock;
};

#endif // SOCKETCHANNELEX_H
